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
 * Portions created by the Initial Developer are Copyright (C) 1993-2006
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
#include "BuiltinNatives.h"

using namespace MMgc;

namespace avmplus
{
    // ---------------

#ifndef VMCFG_AOT
    NativeInitializer::NativeInitializer(AvmCore* _core,
                                            char const* const* _versioned_uris,
                                            const uint8_t* _abcData,
                                            uint32_t _abcDataLen,
                                            uint32_t _methodCount,
                                            uint32_t _classCount) :
        core(_core),
        versioned_uris(_versioned_uris),
        abcData(_abcData),
        abcDataLen(_abcDataLen),
        methods((MethodType*)core->GetGC()->Calloc(_methodCount, sizeof(MethodType), GC::kZero)),
        classes((ClassType*)core->GetGC()->Calloc(_classCount, sizeof(ClassType), GC::kZero)),
        methodCount(_methodCount),
        classCount(_classCount)
    {
        if (versioned_uris)
            core->addVersionedURIs(versioned_uris);
    }
#else
    NativeInitializer::NativeInitializer(AvmCore* _core,
                                            char const* const* _versioned_uris,
                                            const AOTInfo *_aotInfo,
                                            uint32_t _methodCount,
                                            uint32_t _classCount) :
        core(_core),
        versioned_uris(_versioned_uris),
        abcData(_aotInfo->abcBytes),
        abcDataLen(_aotInfo->nABCBytes),
        methods((MethodType*)core->GetGC()->Calloc((_methodCount>0 ? _methodCount : 1), sizeof(MethodType), GC::kZero)),
        classes((ClassType*)core->GetGC()->Calloc((_classCount>0 ? _classCount : 1), sizeof(ClassType), GC::kZero)),
        methodCount(_methodCount),
        classCount(_classCount)
        , aotInfo(_aotInfo)
        , compiledMethods(_aotInfo->abcMethods)
        , compiledMethodCount(_aotInfo->nABCMethods)
    {
        if (versioned_uris)
            core->addVersionedURIs(versioned_uris);
    }
#endif


    void NativeInitializer::fillInMethods(const NativeMethodInfo* _methodEntry)
    {
        while (_methodEntry->method_id != -1)
        {
            // if we overwrite a native method mapping, something is hosed
            AvmAssert(methods[_methodEntry->method_id] == NULL);
            methods[_methodEntry->method_id] = _methodEntry;
            _methodEntry++;
        }
    }

    void NativeInitializer::fillInClasses(const NativeClassInfo* _classEntry)
    {
        while (_classEntry->class_id != -1)
        {
            // if we overwrite a native class mapping, something is hosed
            AvmAssert(classes[_classEntry->class_id]  == NULL);
            classes[_classEntry->class_id] = _classEntry;
            _classEntry++;
        }
    }

    PoolObject* NativeInitializer::parseBuiltinABC(Domain* domain)
    {
        AvmAssert(domain != NULL);

        ScriptBuffer code = ScriptBuffer(new (core->GetGC()) ReadOnlyScriptBufferImpl(abcData, abcDataLen));

        return core->parseActionBlock(code, /*start*/0, /*toplevel*/NULL, domain, this, kApiVersion_VM_INTERNAL/*active api*/);
    }

    NativeInitializer::~NativeInitializer()
    {
        // might as well explicitly free now
        core->GetGC()->Free(methods);
        core->GetGC()->Free(classes);
    }

#ifdef VMCFG_AOT
    bool NativeInitializer::getCompiledInfo(NativeMethodInfo *info, AvmThunkNativeHandler* handlerOut, Multiname &returnTypeName, uint32_t i) const
    {
        info->thunker = (GprMethodProc)0;
        // NativeMethodInfo.handler is a union of
        // pointer to function and pointer to member function.
        // Set them both so the entire union is initialized.
        // See bugzilla#647660
        info->handler.method = NULL;
        info->handler.function = NULL;

        if (i < compiledMethodCount && compiledMethods[i])
        {
            bool isNumberRetType = false;
            if (NUMBER_TYPE) {
                Multiname numberTypeName(NUMBER_TYPE->ns(), NUMBER_TYPE->name());
                isNumberRetType = returnTypeName.matches(&numberTypeName);
            }
            info->thunker = isNumberRetType ? (GprMethodProc)aotThunkerN : (GprMethodProc)aotThunker;
            handlerOut->function = compiledMethods[i];
            return true;
        }
        else
        {
            handlerOut->function = (AvmThunkNativeFunctionHandler)0;
            return false;
        }
    }
#endif

    void ClassManifestBase::fillInClass(uint32_t class_id, ClassClosure* c)
    {
        AvmAssert(class_id < _count);
        // This is subtle but important: this call exists solely as a hack for the Vector<>
        // classes, which don't look up by name properly. Int/UInt/DoubleVectorClass
        // are all initialized once, as you'd expected, but ObjectVectorClass is initialized
        // multiple times: once for Vector<*>, then again for each specialization. We
        // only want the first one stored (not subsequent specializations), so only
        // store if nothing is there yet.
        if (_classes[class_id] == NULL)
            WBRC(c->gc(), this, &_classes[class_id], c);
    }
    
    ClassClosure* FASTCALL ClassManifestBase::lazyInitClass(uint32_t class_id)
    {
        AvmAssert(class_id < _count);
        ClassClosure** cc = &_classes[class_id];
        if (*cc == NULL)
        {
            PoolObject* pool = _env->abcEnv()->pool();
            Traits* traits = pool->getClassTraits(class_id)->itraits;
            Multiname qname(traits->ns(), traits->name());
            ScriptObject* container = _env->finddef(&qname);
            Atom classAtom = _env->toplevel()->getproperty(container->atom(), &qname, container->vtable);
            WBRC(pool->core->GetGC(), this, cc, (ClassClosure*)AvmCore::atomToScriptObject(classAtom));
        }
#ifdef _DEBUG
        // make sure the handful of special-cased entries in Toplevel match what we have cached.
        Toplevel* t = _env->toplevel();
        if (this == t->_builtinClasses)
        {
            switch(class_id)
            {
            case avmplus::NativeID::abcclass_Object: AvmAssert(*cc == t->objectClass); break;
            case avmplus::NativeID::abcclass_Class: AvmAssert(*cc == t->_classClass); break;
            case avmplus::NativeID::abcclass_Function: AvmAssert(*cc == t->_functionClass); break;
            case avmplus::NativeID::abcclass_Boolean: AvmAssert(*cc == t->_booleanClass); break;
            case avmplus::NativeID::abcclass_Namespace: AvmAssert(*cc == t->_namespaceClass); break;
            case avmplus::NativeID::abcclass_Number: AvmAssert(*cc == t->_numberClass); break;
            case avmplus::NativeID::abcclass_int: AvmAssert(*cc == t->_intClass); break;
            case avmplus::NativeID::abcclass_uint: AvmAssert(*cc == t->_uintClass); break;
            case avmplus::NativeID::abcclass_String: AvmAssert(*cc == t->_stringClass); break;
            }
        }
#endif
        // This function should *never* return NULL; if we get in this situation, it's
        // almost certainly due to an unexpected error during VM bootstrapping.
        AvmAssert(*cc != NULL);
        return *cc;
    }

    bool ClassManifestBase::gcTrace(MMgc::GC* gc, size_t /*ignored*/)
    {
        gc->TraceLocation<ScriptEnv>(&_env);
        gc->TraceLocations<ClassClosure>(&_classes[0], _count);
        return false;
    }
}
