/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is [Open Source Virtual Machine.].
 *
 * The Initial Developer of the Original Code is
 * Adobe System Incorporated.
 * Portions created by the Initial Developer are Copyright (C) 2010
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Adobe AS3 Team
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#include "avmplus.h"

#ifdef VMCFG_NANOJIT

#include "CodegenLIR.h"


//
// JIT compiler for invoker stubs
//
namespace avmplus
{
    bool InvokerCompiler::canCompileInvoker(MethodInfo* method)
    {
        MethodSignaturep ms = method->getMethodSignature();
        int32_t rest_offset = ms->rest_offset();
        int32_t param_count = ms->param_count();
        if (rest_offset > int32_t((param_count+1) * sizeof(Atom)) &&
            method->needRestOrArguments()) {
            // situation: natively represented args need more space than provided,
            // and this method uses varargs (rest args or arguments array); this means
            // the declared args must grow, which requires allocating an unknown amount
            // of space since we don't know (at compile time) how many extra varags are present.
            //
            // punt for now.  to better handle this in the future, the cases are:
            //
            // jit function with rest args:
            //   the prolog will create an array with these extra args, so all
            //   we should need is a way to pass in the pointer to them.  (tweak ABI).
            //
            // jit function that needs arguments:
            //   the prolog will create an array with all the args re-boxed as Atom,
            //   plus the extra ones.  (the unboxed copies are still available in the callee).
            //
            // native function with rest args:
            //   the native function abi passes rest args via (argc,Atom*) parameters
            //   which we could adapt to here without shifting or copying, if we could
            //   bypass the normal Gpr/FprMethodProc ABI.
            //
            // native function with arguments:
            //   doesn't happen.  native functions only support rest args.
            //
            // Given the current JIT and native ABI, we can't support shifting and
            // copying the extra unknown number of args.  With changes to the native
            // ABI, we could pass a reference to the extra args without any copying.
            return false;
        }
        return true;
    }

    // compiler driver
    AtomMethodProc InvokerCompiler::compile(MethodInfo* method)
    {
        InvokerCompiler compiler(method);
        compiler.generate_lir();
        return (AtomMethodProc) compiler.assemble();
    }

    InvokerCompiler::InvokerCompiler(MethodInfo* method)
        : LirHelper(method->pool())
        , method(method)
        , ms(method->getMethodSignature())
        , maxargs_br(NULL)
        , minargs_br(NULL)
    {
        this->method = method;
        this->ms = method->getMethodSignature();

        initCodeMgr(method->pool());
        frag = new (*lir_alloc) Fragment(0 verbose_only(, 0));
        LirBuffer* lirbuf = frag->lirbuf = new (*lir_alloc) LirBuffer(*lir_alloc);
        lirbuf->abi = ABI_CDECL;
        LirWriter* lirout = new (*alloc1) LirBufWriter(lirbuf, core->config.njconfig);
        verbose_only(
            if (verbose()) {
                lirbuf->printer = new (*lir_alloc) LInsPrinter(*lir_alloc, TR_NUM_USED_ACCS);
                lirbuf->printer->addrNameMap->addAddrRange(pool->core, sizeof(AvmCore), 0, "core");
            }
        )
        debug_only(
            lirout = validate2 = new (*alloc1) ValidateWriter(lirout, lirbuf->printer, "InvokerCompiler");
        )
        verbose_only(
            if (verbose()) {
                CodeMgr *codeMgr = method->pool()->codeMgr;
                core->console << "compileInvoker " << method << "\n";
                core->console <<
                    " required=" << ms->requiredParamCount() <<
                    " optional=" << (ms->param_count() - ms->requiredParamCount()) << "\n";
                lirout = new (*alloc1) VerboseWriter(*alloc1, lirout, lirbuf->printer, &codeMgr->log);
            }
        )
#if defined(NANOJIT_ARM)
        if (core->config.njconfig.soft_float)
        {
            lirout = new (*alloc1) SoftFloatFilter(lirout);
        }
#endif
        // add other LirWriters here
        this->lirout = lirout;
        emitStart(*alloc1, lirbuf, lirout);
    }

    // recipe for an invoke wrapper:
    //    Atom <generated invoker>(MethodEnv* env, int argc, Atom* argv) {
    //        1. check argc:            env->startCoerce(argc, env->get_ms());
    //        2. unbox args:            unboxCoerceArgs, unrolled for each arg
    //        3. return box(call(...))
    //    }

    // jit-compile an invoker for mi
    void InvokerCompiler::generate_lir()
    {
        // invoker params
        LIns* env_param = param(0, "env");
        LIns* argc_param = p2i(param(1, "argc"));
        LIns* args_param = param(2, "args");
        coreAddr = InsConstPtr(core);

        // if unboxing args will make them expand, allocate more space.
        int32_t rest_offset = ms->rest_offset();
        if (rest_offset > int32_t((ms->param_count()+1)*sizeof(Atom))) {
            AvmAssert(!method->needRestOrArguments());
            args_out = lirout->insAlloc(rest_offset);
        } else {
            // we can do in-place unboxing of args.
            args_out = args_param;
        }

        // 1. check argc
        emit_argc_check(argc_param);

        // 2. unbox & coerce args
        downcast_args(env_param, argc_param, args_param);

        // 3. call, box result, return atom
        call_method(env_param, argc_param);

        // error handler for argc error
        if (minargs_br || maxargs_br) {
            LIns* errlabel = label();
            if (minargs_br) minargs_br->setTarget(errlabel);
            if (maxargs_br) maxargs_br->setTarget(errlabel);
            callIns(FUNCTIONID(argcError), 2, env_param, argc_param);
        }

        // mark the endpoint of generated LIR with an instruction the Assembler allows at the end
        frag->lastIns = livep(env_param);

        // we're done with LIR generation, free up what we can.
        mmfx_delete(alloc1);
        alloc1 = NULL;
    }

    void InvokerCompiler::emit_argc_check(LIns* argc_param)
    {
        int min_argc = ms->requiredParamCount();
        int param_count = ms->param_count();
        if (min_argc == param_count && !ms->argcOk(param_count + 1)) {
            // exactly param_count args required
            // if (argc != param_count) goto error
            maxargs_br = jnei(argc_param, param_count);
        } else {
            if (!ms->argcOk(param_count+1)) {
                // extra params are not allowed, must check for max args
                // if (argc > param_count) goto error
                maxargs_br = jgti(argc_param, param_count);
            }
            if (min_argc > 0) {
                // at least 1 param is required, so check
                // if (argc < min_argc) goto error
                minargs_br = jlti(argc_param, min_argc);
            }
        }
    }

    void InvokerCompiler::downcast_arg(int i, int offset, LIns* env_param, LIns* args_param)
    {
        BuiltinType bt = ms->paramTraitsBT(i);
        if (bt != BUILTIN_any) {
            LIns* atom = ldp(args_param, i*sizeof(Atom), ACCSET_OTHER);
            LIns* native = downcast_expr(atom, ms->paramTraits(i), env_param);
            lirout->insStore(native, args_out, offset, ACCSET_OTHER);
        } else if (copyArgs()) {
            LIns* atom = ldp(args_param, i*sizeof(Atom), ACCSET_OTHER);
            lirout->insStore(atom, args_out, offset, ACCSET_OTHER);
        }
    }

    void InvokerCompiler::downcast_args(LIns* env_param, LIns* argc_param, LIns* args_param)
    {
        // the receiver arg (arg0) only needs to be unboxed, not coerced
        verbose_only( if (verbose())
            core->console << "unbox arg 0 " << ms->paramTraits(0) << "\n";
        )
        LIns* atom = ldp(args_param, 0, ACCSET_OTHER);
        LIns* native = atomToNative(ms->paramTraitsBT(0), atom);
        if (native != atom || copyArgs())
            lirout->insStore(native, args_out, 0, ACCSET_OTHER);
        int offset = argSize(ms, 0);

        // the required args need to be coerced and unboxed
        int i;
        int required_count = ms->requiredParamCount();
        for (i = 1; i <= required_count; i++) {
            verbose_only( if (verbose())
                core->console << "arg " << i << " " << ms->paramTraits(i) << "\n";
            )
            downcast_arg(i, offset, env_param, args_param);
            offset += argSize(ms, i);
        }

        // optional args also need coercing and unboxing when they're present
        int param_count = ms->param_count();
        if (required_count < param_count) {
            int optional_count = param_count - required_count;
            int branch_count = 0;
            LIns** branches = new (*alloc1) LIns*[optional_count];
            for (; i <= param_count; i++) {
                verbose_only( if (verbose())
                    core->console << "optional arg " << i << " " << ms->paramTraits(i) << "\n";
                )
                // if (argc < i) { goto done }
                branches[branch_count++] = jlti(argc_param, i);
                downcast_arg(i, offset, env_param, args_param);
                offset += argSize(ms, i);
            }
            if (branch_count > 0) {
                // done: patch all the optional-arg branches
                LIns* done_label = label();
                for (i = 0; i < branch_count; i++)
                    branches[i]->setTarget(done_label);
            }
        }
    }

    void* InvokerCompiler::assemble()
    {
        CodeMgr* codeMgr = method->pool()->codeMgr;

        verbose_only(if (pool->isVerbose(LC_Liveness, method)) {
            Allocator live_alloc;
            LirReader in(frag->lastIns);
            nanojit::live(&in, live_alloc, frag, &codeMgr->log);
        })

        // disable hardening features when compiling thunks
        nanojit::Config cfg = core->config.njconfig;
        cfg.harden_function_alignment = false;
        cfg.harden_nop_insertion = false;

        // Use the 'active' log if we are in verbose output mode otherwise sink the output
        LogControl* log = &(codeMgr->log);
        verbose_only(
            SinkLogControl sink;
            log = pool->isVerbose(VB_jit,method) ? log : &sink;
        )

        Assembler *assm = new (*lir_alloc) Assembler(codeMgr->codeAlloc, codeMgr->allocator, *lir_alloc, log, cfg);
        verbose_only( StringList asmOutput(*lir_alloc); )
        verbose_only( if (!pool->isVerbose(VB_raw, method)) assm->_outputCache = &asmOutput; )
        LirReader bufreader(frag->lastIns);
        assm->beginAssembly(frag);
        assm->assemble(frag, &bufreader);
        assm->endAssembly(frag);

        verbose_only(
            assm->_outputCache = 0;
            for (Seq<char*>* p = asmOutput.get(); p != NULL; p = p->tail)
                assm->outputf("%s", p->head);
        );
        if (!assm->error()) {
            if (method->isNative()) {
                PERFM_NVPROF("C++ invoker bytes", CodeAlloc::size(assm->codeList));
            } else {
                PERFM_NVPROF("JIT invoker bytes", CodeAlloc::size(assm->codeList));
            }
            return frag->code();
        } else {
            return NULL;
        }
    }

    void InvokerCompiler::call_method(LIns* env_param, LIns* argc_param)
    {
        // We know we've called the method at least once, so method->implGPR pointer is correct.
        CallInfo* call = (CallInfo*) lir_alloc->alloc(sizeof(CallInfo));
        call->_isPure = 0;
        call->_storeAccSet = ACCSET_STORE_ANY;
        call->_abi = ABI_FUNCTION;
        verbose_only( if (verbose()) {
            StUTF8String name(method->getMethodName());
            char *namestr = new (*lir_alloc) char[VMPI_strlen(name.c_str())+1];
            VMPI_strcpy(namestr, name.c_str());
            call->_name = namestr;
        })
        switch (ms->returnTraitsBT()) {
        case BUILTIN_number:
            call->_address = (uintptr_t) method->_implFPR;
            call->_typesig = SIG3(F,P,I,P);
            break;
        case BUILTIN_int: case BUILTIN_uint: case BUILTIN_boolean:
            call->_address = (uintptr_t) method->_implGPR;
            call->_typesig = SIG3(I,P,I,P);
            break;
        default:
            call->_address = (uintptr_t) method->_implGPR;
            call->_typesig = SIG3(A,P,I,P);
            break;
        }
        LIns* result = callIns(call, 3, env_param, argc_param, args_out);
        livep(args_out);
        // box and return the result
        retp(nativeToAtom(result, ms->returnTraits()));
    }

    LIns* InvokerCompiler::downcast_expr(LIns* atom, Traits* t, LIns* env)
    {
        switch (bt(t)) {
        case BUILTIN_object:
            // return (atom == undefinedAtom) ? nullObjectAtom : atom;
            return choose(eqp(atom, undefinedAtom), nullObjectAtom, atom);
        case BUILTIN_int:
            return i2p(callIns(FUNCTIONID(integer), 1, atom));
        case BUILTIN_uint:
            return ui2p(callIns(FUNCTIONID(toUInt32), 1, atom));
        case BUILTIN_number:
            return callIns(FUNCTIONID(number), 1, atom);
        case BUILTIN_boolean:
            return ui2p(callIns(FUNCTIONID(boolean), 1, atom));
        case BUILTIN_string:
            return callIns(FUNCTIONID(coerce_s), 2, InsConstPtr(t->core), atom);
        case BUILTIN_namespace:
            return andp(callIns(FUNCTIONID(coerce), 3, env, atom, InsConstPtr(t)), ~7);
        case BUILTIN_void:
        case BUILTIN_null:
        case BUILTIN_any:
            AvmAssert(false);
        default:
            return downcast_obj(atom, env, t);
        }
    }

#ifdef NJ_VERBOSE
    bool InvokerCompiler::verbose()
    {
        return method->pool()->isVerbose(VB_jit, method);
    }
#endif

}
#endif // VMCFG_NANOJIT
