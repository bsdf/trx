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
 * Portions created by the Initial Developer are Copyright (C) 2008
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

    FUNCTION(CALL_INDIRECT, SIG4(U,P,P,I,P), icalli)
    FUNCTION(CALL_INDIRECT, SIG4(A,P,P,I,P), acalli)
    FUNCTION(CALL_INDIRECT, SIG4(F,P,P,I,P), fcalli)
    FUNCTION(CALL_INDIRECT, SIG5(I,P,P,I,P,P), icallimt)
    FUNCTION(CALL_INDIRECT, SIG5(A,P,P,I,P,P), acallimt)
    FUNCTION(CALL_INDIRECT, SIG5(F,P,P,I,P,P), fcallimt)

    METHOD(ENVADDR(MethodEnv::newActivation), SIG1(P,P), newActivation)
    METHOD(ENVADDR(MethodEnv::newcatch), SIG2(P,P,P), newcatch)
    METHOD(ENVADDR(MethodEnv::newfunction), SIG3(P,P,P,P), newfunction)

    Atom callprop_miss(CallCache&, Atom obj, int argc, Atom* args, MethodEnv*);

    #ifdef DOPROF
    # define PROF_IF(label, expr) bool hit = (expr); _nvprof(label, hit); if (hit)
    #else
    # define PROF_IF(label, expr) if (expr)
    #endif

    // if the cached obj was a ScriptObject, we have a hit when
    // the new object's tag is kObjectType and the cached vtable matches exactly
    #define OBJ_HIT(obj, c)  (atomKind(obj) == kObjectType && atomObj(obj)->vtable == (c).vtable)

    // if the cached obj was a primitive, we only need a matching atom tag for a hit
    #define PRIM_HIT(val, c) (atomKind(val) == c.tag)

    REALLY_INLINE Atom invoke_cached_method(CallCache& c, Atom obj, int argc, Atom* args)
    {
        // force arg0 = obj; if caller used OP_callproplex then receiver was null.
        args[0] = obj;
        return c.method->coerceEnter(argc, args);
    }

    template <class T>
    REALLY_INLINE T load_cached_slot(BindingCache& c, Atom obj)
    {
        return *((T*) (uintptr_t(atomObj(obj)) + c.slot_offset));
    }

    // calling a declared method on a ScriptObject
    Atom callprop_obj_method(CallCache& c, Atom obj, int argc, Atom* args, MethodEnv* env)
    {
        PROF_IF ("callprop_obj_method hit", OBJ_HIT(obj,c))
            return invoke_cached_method(c, obj, argc, args);
        return callprop_miss(c, obj, argc, args, env);
    }

    // calling a function in a declared slot, specialized for slot type
    template <class T>
    Atom callprop_obj_slot(CallCache& c, Atom obj, int argc, Atom* args, MethodEnv* env)
    {
        PROF_IF ("callprop_obj_slot hit", OBJ_HIT(obj,c))
            return op_call(env, load_cached_slot<T>(c, obj), argc, args);
        return callprop_miss(c, obj, argc, args, env);
    }

    // calling an unknown binding on an object, i.e. a dynamic property
    Atom callprop_obj_none(CallCache& c, Atom obj, int argc, Atom* args, MethodEnv* env)
    {
        PROF_IF ("callprop_obj_none hit", OBJ_HIT(obj,c)) {
            // cache hit: access dynamic properties and proto-chain directly
            return call_obj_dynamic(obj, c.name, argc, args);
        }
        return callprop_miss(c, obj, argc, args, env);
    }

    // calling a declared method on a primitive value
    Atom callprop_prim_method(CallCache& c, Atom prim, int argc, Atom* args, MethodEnv* env)
    {
        PROF_IF ("callprop_prim_method hit", PRIM_HIT(prim, c))
            return invoke_cached_method(c, prim, argc, args);
        return callprop_miss(c, prim, argc, args, env);
    }

    // calling a dynamic property on a primitive's prototype
    Atom callprop_prim_none(CallCache& c, Atom prim, int argc, Atom* args, MethodEnv* env)
    {
        PROF_IF ("callprop_prim_none hit", PRIM_HIT(prim, c)) {
            // cache hit since all prims are final
            // possible speedup: cached the prototype object itself, or at least
            // get the prototype object using a table lookup with tag as the index
            return call_prim_dynamic(env, prim, c.name, argc, args);
        }
        return callprop_miss(c, prim, argc, args, env);
    }

    // generic call handler for uncommon cases
    Atom callprop_generic(CallCache& c, Atom obj, int argc, Atom* args, MethodEnv* env)
    {
        // go back to callprop_miss() after this call in case atom type changes
        // to something allowing a smarter handler
        c.call_handler = callprop_miss;
        Toplevel* toplevel = env->toplevel();
        VTable* vtable = toplevel->toVTable(obj);
        tagprof("callprop_generic obj", obj);
        tagprof("callprop_generic bind", toplevel->getBinding(vtable->traits, c.name));
        return toplevel->callproperty(obj, c.name, argc, args, vtable);
    }

    static const CallCache::Handler callprop_obj_handlers[8] = {
        &callprop_obj_none,     // BKIND_NONE
        &callprop_obj_method,   // BKIND_METHOD
        0,                      // BKIND_VAR (custom handler picked below)
        0,                      // BKIND_CONST
        0,                      // BKIND_unused (impossible)
        &callprop_generic,      // BKIND_GET
        &callprop_generic,      // BKIND_SET (error)
        &callprop_generic       // BKIND_GETSET
    };

    // handlers for ScriptObject slots, indexed by SlotStorageType.  We only
    // care about slot types that hold something we can call; other cases
    // will be errors, so we use the generic handler for them.
    static const CallCache::Handler callprop_slot_handlers[SST_MAX_VALUE] = {
        &callprop_obj_slot<Atom>,           // SST_atom
        &callprop_generic,                  // SST_string
        &callprop_generic,                  // SST_namespace
        &callprop_obj_slot<ScriptObject*>,  // SST_scriptobject
        &callprop_generic,                  // SST_int32
        &callprop_generic,                  // SST_uint32
        &callprop_generic,                  // SST_bool32
        &callprop_generic                   // SST_double
    };

    static const CallCache::Handler callprop_prim_handlers[8] = {
        &callprop_prim_none,    // BKIND_NONE
        &callprop_prim_method,  // BKIND_METHOD
        0,                      // BKIND_VAR (impossible on primitive)
        0,                      // BKIND_CONST (impossible on primitive)
        0,                      // BKIND_unused (impossible)
        &callprop_generic,      // BKIND_GET
        0,                      // BKIND_SET (impossible on primitive)
        0                       // BKIND_GETSET (impossible on primitive)
    };

    // cache handler when cache miss occurs:
    //  - Look up the binding using the Multiname saved in the cache
    //  - save the object vtable (for ScriptObject*) or atom tag (all others)
    //  - pick a handler and save the MethodEnv* or slot_offset
    //  - invoke the new handler, which WILL NOT miss on this first call
    Atom callprop_miss(CallCache& c, Atom obj, int argc, Atom* args, MethodEnv* env)
    {
        AssertNotNull(obj);
        Toplevel* toplevel = env->toplevel();
        VTable* vtable = toplevel->toVTable(obj);
        Traits* obj_type = vtable->traits;
        Binding b = toplevel->getBinding(obj_type, c.name);
        if (AvmCore::isMethodBinding(b)) {
            // cache the method we will call
            c.method = vtable->methods[AvmCore::bindingToMethodId(b)];
        }
        if (isObjectPtr(obj)) {
            c.vtable = vtable;
            if (AvmCore::isSlotBinding(b)) {
                // precompute the slot offset, then install a cache handler that's
                // specialized for the slot type
                void *slot_ptr, *obj_ptr = atomObj(obj);
                SlotStorageType sst = obj_type->getTraitsBindings()->
                    calcSlotAddrAndSST(AvmCore::bindingToSlotId(b), obj_ptr, slot_ptr);
                c.call_handler = callprop_slot_handlers[sst];
                c.slot_offset = uintptr_t(slot_ptr) - uintptr_t(obj_ptr);
            } else {
                // all other bindings
                c.call_handler = callprop_obj_handlers[AvmCore::bindingKind(b)];
            }
        } else {
            // must be a primitive: int, bool, string, namespace, or number
            c.call_handler = callprop_prim_handlers[AvmCore::bindingKind(b)];
            c.tag = atomKind(obj);
            AvmAssert(c.tag != 0);
        }
        return c.call_handler(c, obj, argc, args, env);
    }
    FUNCTION(CALL_INDIRECT, SIG6(A,P,P,A,I,P,P), call_cache_handler)

    // called by jit'd code when an OP_callproperty or callproplex could not be
    // early bound to a property and we must do the property lookup at runtime,
    // AND the multiname has runtime parts, so we couldn't use a CallCache.
    Atom callprop_late(MethodEnv* caller_env, Atom base, const Multiname* name, int argc, Atom* args)
    {
        CallCache c(name, NULL);  // temporary cache, just so we can call the generic handler.
        return callprop_generic(c, base, argc, args, caller_env);
    }
    FUNCTION(FUNCADDR(callprop_late), SIG5(A,P,A,P,I,P), callprop_late)

    // forward decl
    Atom getprop_miss(GetCache&, MethodEnv*, Atom obj);

    // getting any property (catch-all)
    Atom getprop_generic(GetCache& c, MethodEnv* env, Atom obj)
    {
        // reinstall miss handler so we don't end up dead-ended here in this handler.
        c.get_handler = getprop_miss;
        Toplevel* toplevel = env->toplevel();
        VTable* vtable = toplevel->toVTable(obj);
        tagprof("getprop_generic obj", obj);
        tagprof("getprop_generic bind", toplevel->getBinding(vtable->traits, c.name));
        return toplevel->getproperty(obj, c.name, vtable);
    }

    // overloaded helpers that convert a raw value to Atom.  helper will be
    // chosen based on the <T> parameter to getprop_obj_slot, below.
    enum Bool32 {};                    // can't use bool when sizeof(bool) != sizeof(int32_t)
    REALLY_INLINE Atom boxslot(AvmCore*, OpaqueAtom a) { return (Atom)a; }
    REALLY_INLINE Atom boxslot(AvmCore*, String* a) { return a->atom(); }
    REALLY_INLINE Atom boxslot(AvmCore*, Namespace* a) { return a->atom(); }
    REALLY_INLINE Atom boxslot(AvmCore*, ScriptObject* a) { return a->atom(); }
    REALLY_INLINE Atom boxslot(AvmCore* core, int32_t i) { return core->intToAtom(i); }
    REALLY_INLINE Atom boxslot(AvmCore* core, uint32_t u) { return core->uintToAtom(u); }
    REALLY_INLINE Atom boxslot(AvmCore*, Bool32 b) { return ((b != 0)<<3)|kBooleanType; }
    REALLY_INLINE Atom boxslot(AvmCore* core, double d) { return core->doubleToAtom(d); }

    // getting a slot on an object, specialized on slot type to streamline boxing
    template <class T>
    Atom getprop_obj_slot(GetCache& c, MethodEnv* env, Atom obj)
    {
        PROF_IF ("getprop_obj_slot hit", OBJ_HIT(obj, c)) {
            return boxslot(atomObj(obj)->core(), load_cached_slot<T>(c, obj));
        }
        return getprop_miss(c, env, obj);
    }

#if defined(_MSC_VER) && defined(AVMPLUS_IA32)
    // Force this function to have a frame pointer so the double used is 8-byte aligned
    #pragma optimize("y", off)
#endif
    SSE2_ONLY(
    // getting a double slot on an object
    Atom getprop_obj_slot_double_sse2(GetCache& c, MethodEnv* env, Atom obj)
    {
        PROF_IF ("getprop_obj_slot hit", OBJ_HIT(obj, c)) {
            return atomObj(obj)->core()->doubleToAtom_sse2(load_cached_slot<double>(c, obj));
        }
        return getprop_miss(c, env, obj);
    }
    )
#if defined(_MSC_VER) && defined(AVMPLUS_IA32)
    #pragma optimize("y", on)
#endif

    // calling a getter method on an object
    Atom getprop_obj_get(GetCache& c, MethodEnv* env, Atom obj)
    {
        PROF_IF ("getprop_obj_get hit", OBJ_HIT(obj, c)) {
            return c.method->coerceEnter(obj);
        }
        return getprop_miss(c, env, obj);
    }

    // getting a dynamic property on an object
    Atom getprop_obj_none(GetCache& c, MethodEnv* env, Atom obj)
    {
        PROF_IF ("getprop_obj_none hit", OBJ_HIT(obj, c)) {
            return atomObj(obj)->getMultinameProperty(c.name);
        }
        return getprop_miss(c, env, obj);
    }

    // getting an object method (method closure creation)
    Atom getprop_obj_method(GetCache& c, MethodEnv* env, Atom obj)
    {
        PROF_IF ("getprop_obj_method hit", OBJ_HIT(obj, c)) {
            return env->toplevel()->methodClosureClass()->create(c.method, obj)->atom();
        }
        return getprop_miss(c, env, obj);
    }

    // getting the result of a getter property on a primitive, e.g. String.length
    Atom getprop_prim_get(GetCache& c, MethodEnv* env, Atom obj)
    {
        PROF_IF ("getprop_prim_get hit", PRIM_HIT(obj, c)) {
            return c.method->coerceEnter(obj);
        }
        return getprop_miss(c, env, obj);
    }

    // handlers for gets on ScriptObject* objects, indexed by bindingKind(Binding)
    static const GetCache::Handler getprop_obj_handlers[8] = {
        &getprop_obj_none,      // BKIND_NONE
        &getprop_obj_method,    // BKIND_METHOD (method extraction)
        0,                      // BKIND_VAR (will use slot handler table below)
        0,                      // BKIND_CONST (will use slot handler table)
        0,                      // BKIND_unused (impossible)
        &getprop_obj_get,       // BKIND_GET
        &getprop_generic,       // BKIND_SET (error)
        &getprop_obj_get        // BKIND_GETSET
    };

    // handlers for slots on ScriptObject, indexed by SlotStorageType
    static const GetCache::Handler getprop_slot_handlers[SST_MAX_VALUE] = {
        &getprop_obj_slot<OpaqueAtom>,      // SST_atom
        &getprop_obj_slot<String*>,         // SST_string
        &getprop_obj_slot<Namespace*>,      // SST_namespace
        &getprop_obj_slot<ScriptObject*>,   // SST_scriptobject
        &getprop_obj_slot<int32_t>,         // SST_int32
        &getprop_obj_slot<uint32_t>,        // SST_uint32
        &getprop_obj_slot<Bool32>,          // SST_bool32
        &getprop_obj_slot<double>           // SST_double
    };

    // handlers when object is primitive, indexed by bindingKind(Binding)
    static const GetCache::Handler getprop_prim_handlers[8] = {
        &getprop_generic,     // BKIND_NONE
        &getprop_generic,     // BKIND_METHOD
        0,                    // BKIND_VAR (impossible on primitive)
        0,                    // BKIND_CONST (impossible on primitive)
        0,                    // BKIND_unused (impossible)
        getprop_prim_get,     // BKIND_GET
        0,                    // BKIND_SET (impossible on primitive)
        0,                    // BKIND_GETSET (impossible on primitive)
    };

    Atom getprop_miss(GetCache& c, MethodEnv* env, Atom obj)
    {
        // cache handler when cache miss occurs
        AvmAssert(!AvmCore::isNullOrUndefined(obj));
        Toplevel* toplevel = env->toplevel();
        VTable* vtable = toplevel->toVTable(obj);
        Traits* actual_type = vtable->traits;
        Binding b = toplevel->getBinding(actual_type, c.name);
        if (AvmCore::isMethodBinding(b)) {
            c.method = vtable->methods[AvmCore::bindingToMethodId(b)];
        } else if (AvmCore::hasGetterBinding(b)) {
            c.method = vtable->methods[AvmCore::bindingToGetterId(b)];
        }
        if (isObjectPtr(obj)) {
            c.vtable = vtable;
            if (AvmCore::isSlotBinding(b)) {
                // precompute the slot offset, then install a cache handler that's
                // hardwired to read the slot and convert value to Atom.
                void *slot_ptr, *obj_ptr = atomObj(obj);
                const SlotStorageType sst = actual_type->getTraitsBindings()->
                    calcSlotAddrAndSST(AvmCore::bindingToSlotId(b), (void*)obj_ptr, slot_ptr);
                SSE2_ONLY(if(toplevel->core()->config.njconfig.i386_sse2 && sst == SST_double)
                {
                    c.get_handler = &getprop_obj_slot_double_sse2;
                }
                else
                )
                {
                    c.get_handler = getprop_slot_handlers[sst];
                }
                c.slot_offset = uintptr_t(slot_ptr) - uintptr_t(obj_ptr);
            }
            else {
                c.get_handler = getprop_obj_handlers[AvmCore::bindingKind(b)];
                if (AvmCore::isMethodBinding(b) &&
                    AvmCore::isXMLorXMLList(obj) &&
                    c.name->containsAnyPublicNamespace()) {
                    // special case for XML objects: dynamic props hide declared methods
                    // dynamic property lookup
                    c.get_handler = getprop_obj_none;
                }
            }
        } else {
            // must be a primitive: int, bool, string, namespace, or number
            c.get_handler = getprop_prim_handlers[AvmCore::bindingKind(b)];
            c.tag = atomKind(obj);
            AvmAssert(c.tag != 0);
        }
        return c.get_handler(c, env, obj);
    }
    FUNCTION(CALL_INDIRECT, SIG4(A,P,P,P,A), get_cache_handler)

    Atom getprop_late(MethodEnv* env, Atom obj, const Multiname* name)
    {
        GetCache c(name, NULL);
        return getprop_generic(c, env, obj);
    }
    FUNCTION(FUNCADDR(getprop_late), SIG3(A,P,A,P), getprop_late)

    FUNCTION(FUNCADDR(AvmCore::handleInterruptMethodEnv), SIG1(V,P), handleInterruptMethodEnv)
    FASTFUNCTION(FUNCADDR(AvmCore::handleStackOverflowMethodEnv), SIG1(V,P), handleStackOverflowMethodEnv)
    METHOD(ENVADDR(MethodEnv::nextname), SIG3(A,P,A,I), nextname)
    METHOD(ENVADDR(MethodEnv::nextvalue), SIG3(A,P,A,I), nextvalue)
    METHOD(ENVADDR(MethodEnv::hasnext), SIG3(I,P,A,I), hasnext)
    METHOD(COREADDR(AvmCore::coerce_s), SIG2(P,P,A), coerce_s)
    METHOD(COREADDR(AvmCore::string), SIG2(P,P,A), string)
    PUREMETHOD(COREADDR(AvmCore::doubleToString), SIG2(P,P,F), doubleToString)
    PUREMETHOD(COREADDR(AvmCore::uintToString), SIG2(P,P,U), uintToString)
    PUREMETHOD(COREADDR(AvmCore::intToString), SIG2(P,P,I), intToString)
    PUREMETHOD(COREADDR(AvmCore::doubleToAtom), SIG2(A,P,F), doubleToAtom)
    PUREFUNCTION(FUNCADDR(AvmCore::doubleToInt32), SIG1(I,F), doubleToInt32)
    PUREFUNCTION(FUNCADDR(AvmCore::boolean), SIG1(I,A), boolean)
    PUREFUNCTION(FUNCADDR(AvmCore::toUInt32), SIG1(U,A), toUInt32)
    PUREFUNCTION(FUNCADDR(AvmCore::integer_d), SIG1(I,F), integer_d)
    PUREFUNCTION(FUNCADDR(AvmCore::integer_i), SIG1(I,A), integer_i)
    PUREFUNCTION(FUNCADDR(AvmCore::number_d), SIG1(F,A), number_d)
    PUREFUNCTION(FUNCADDR(AvmCore::integer_u), SIG1(U,A), integer_u)
    PUREFUNCTION(FUNCADDR(AvmCore::integer), SIG1(I,A), integer)
    PUREFUNCTION(FUNCADDR(AvmCore::number), SIG1(F,A), number)
    METHOD(ENVADDR(MethodEnv::hasnextproto), SIG3(I,P,P,P), hasnextproto)

    PUREMETHOD(STRINGADDR(String::_charCodeAtFI),  SIG2(F,P,I), String_charCodeAtFI)
    PUREMETHOD(STRINGADDR(String::_charCodeAtII),  SIG2(I,P,I), String_charCodeAtII)
    PUREMETHOD(STRINGADDR(String::AS3_charCodeAt), SIG2(F,P,F), String_charCodeAtFF)
    PUREMETHOD(STRINGADDR(String::_charCodeAtIF),  SIG2(I,P,F), String_charCodeAtIF)
    PUREMETHOD(STRINGADDR(String::_charCodeAtFU),  SIG2(F,P,U), String_charCodeAtFU)
    PUREMETHOD(STRINGADDR(String::_charCodeAtIU),  SIG2(I,P,U), String_charCodeAtIU)
    FASTFUNCTION(FUNCADDR(String::equalsWithNullChecks),  SIG2(I,P,P), String_equals)

    PUREMETHOD(STRINGADDR(String::_charAtI),  SIG2(P,P,I), String_charAtI)
    PUREMETHOD(STRINGADDR(String::_charAtU),  SIG2(P,P,U), String_charAtU)
    PUREMETHOD(STRINGADDR(String::AS3_charAt),  SIG2(P,P,F), String_charAtF)

    // FIXME: Bug 570049
    // Note that Toplevel::throwTypeError() assumes that Toplevel::typeErrorClass()
    // returns non-null.  In contrast, the interpreter signals a nullcheck failure
    // with MethodEnv::nullcheckfail(), which allows for a null return from typeErrorClass()
    // and reports a different diagnostic.  It is unclear that this error is actually
    // possible, but we should get to the bottom of it and do the same thing in both the
    // interpreter and the JIT.

    void upe(MethodEnv* env)
    {
        env->toplevel()->throwTypeError(kConvertUndefinedToObjectError);
    }
    FUNCTION(FUNCADDR(upe), SIG1(V,P), upe)

    void npe(MethodEnv* env)
    {
        env->toplevel()->throwTypeError(kConvertNullToObjectError);
    }
    FUNCTION(FUNCADDR(npe), SIG1(V,P), npe)

    typedef VTable* (*toVTable_Toplevel)(Toplevel*, Atom);
    PUREFUNCTION(FUNCADDR((toVTable_Toplevel)&toVTable<Toplevel*>), SIG2(P,P,A), toVTable)

    union AnyVal {
        Atom atom;              // SST_atom
        const Namespace* ns;    // SST_namespace
        String* str;            // SST_string
        ScriptObject* obj;      // SST_scriptobject
        int32_t i;              // SST_int32
        uint32_t u;             // SST_uint32
        int32_t b;              // SST_bool32
        double d;               // SST_double
    };

    // Note that CodgegenLIR::analyze_call(), part of live variable
    // analysis, has special knowledge of the signature and semantics
    // of makeatom() and beginCatch() and may need to be updated if
    // changes are made here.

    Atom makeatom(AvmCore* core, AnyVal* native, SlotStorageType tag)
    {
        switch (tag) {
        default:
            AvmAssert(false);
        case SST_atom:
            AvmAssert(atomKind(native->atom) != 0);
            return native->atom;
        case SST_namespace:
            AvmAssert(atomKind(native->atom) == 0);
            return native->ns->atom();
        case SST_string:
            AvmAssert(atomKind(native->atom) == 0);
            AvmAssert(uintptr_t(native->str) > 1000 || native->str == 0);
            return native->str->atom();
        case SST_scriptobject:
            AvmAssert(atomKind(native->atom) == 0);
            return native->obj->atom();
        case SST_int32:
            return core->intToAtom(native->i);
        case SST_uint32:
            return core->uintToAtom(native->u);
        case SST_bool32:
            AvmAssert(native->b == 0 || native->b == 1);
            return native->b ? trueAtom : falseAtom;
        case SST_double:
            return core->doubleToAtom(native->d);
        }
    }
    PUREFUNCTION(FUNCADDR(makeatom), SIG3(A,P,P,I), makeatom)

    void set_slot_from_atom(Atom atom, Traits* traits, AnyVal* slotPtr, uint8_t* tagPtr)
    {
        BuiltinType bt = Traits::getBuiltinType(traits);
        SlotStorageType tag = valueStorageType(bt);

        switch (tag) {
        case SST_atom:
            AvmAssert(atomKind(atom) != 0);
            slotPtr->atom = atom;
            break;
        case SST_namespace:
            AvmAssert(atomKind(atom) == kNamespaceType);
            slotPtr->ns = (Namespace*)atomPtr(atom);
            break;
        case SST_string:
            AvmAssert(atomKind(atom) == kStringType);
            slotPtr->str = (String*)atomPtr(atom);
            break;
        case SST_scriptobject:
            AvmAssert(atomKind(atom) == kObjectType);
            slotPtr->obj = (ScriptObject*)atomPtr(atom);
            break;
        case SST_int32:
            AvmAssert(atomKind(atom) == kIntptrType || atomKind(atom) == kDoubleType);
            slotPtr->i = AvmCore::integer_i(atom);
            break;
        case SST_uint32:
            AvmAssert(atomKind(atom) == kIntptrType || atomKind(atom) == kDoubleType);
            slotPtr->u = AvmCore::integer_u(atom);
            break;
        case SST_bool32:
            AvmAssert(atomKind(atom) == kBooleanType);
            slotPtr->b = (int32_t)atomGetBoolean(atom);
            break;
        case SST_double:
            AvmAssert(atomKind(atom) == kIntptrType || atomKind(atom) == kDoubleType);
            slotPtr->d = AvmCore::number_d(atom);
            break;
        default:
            AvmAssert(false);
        }
        *tagPtr = (uint8_t)tag;
    }

    int32_t beginCatch(AvmCore* core,
                       ExceptionFrame* ef,
                       MethodInfo* info,
                       intptr_t pc,
                       AnyVal* slotPtr, uint8_t* tagPtr)
    {
        int32_t ordinal;
        ef->beginCatch();
        Exception* exception = core->exceptionAddr;
        ExceptionHandler* handler = core->findExceptionHandlerNoRethrow(info, pc, exception, &ordinal);
        if (!handler) {
            // No matching exception, so rethrow.
            core->throwException(exception);
        }
        ef->beginTry(core);
        set_slot_from_atom(exception->atom, handler->traits, slotPtr, tagPtr);
        return ordinal;
    }
    FUNCTION(FUNCADDR(beginCatch), SIG6(I,P,P,P,P,P,P), beginCatch)

    FUNCTION(SETJMP, SIG2(I,P,I), fsetjmp)
    METHOD(EFADDR(ExceptionFrame::beginTry), SIG2(V,P,P), beginTry)
    METHOD(EFADDR(ExceptionFrame::endTry), SIG1(V,P), endTry)

    void setprop_miss(SetCache& c, Atom obj, Atom val, MethodEnv* env);

    void setprop_generic(SetCache& c, Atom obj, Atom val, MethodEnv* env)
    {
        // reinstall miss handler so we don't end up dead-ended here in this handler.
        c.set_handler = setprop_miss;
        Toplevel* toplevel = env->toplevel();
        VTable* vtable = toplevel->toVTable(obj);
        tagprof("setprop_generic obj", obj);
        tagprof("setprop_generic val", val);
        tagprof("setprop_generic bind", toplevel->getBinding(vtable->traits, c.name));
        toplevel->setproperty(obj, c.name, val, vtable);
    }

    // sst_atom (*), for slot types not requiring any coercion
    void setprop_slot_any(SetCache& c, Atom obj, Atom val, MethodEnv* env)
    {
        PROF_IF ("setprop_slot_any hit", OBJ_HIT(obj, c)) {
            ScriptObject* obj_ptr = atomObj(obj);
            Atom* slot_ptr = (Atom*)(uintptr_t(obj_ptr) + c.slot_offset);
            WBATOM(c.gc, obj_ptr, slot_ptr, val);
        } else {
            setprop_miss(c, obj, val, env);
        }
    }

    // coerce and store Object slot; converts undefined->null, no other typecheck
    REALLY_INLINE void store_cached_slot(SetCache& c, ScriptObject* obj, OpaqueAtom* slot_ptr, Atom val)
    {
        AvmAssert(c.gc == obj->gc());
        WBATOM(c.gc, obj, (Atom*)slot_ptr, (val != undefinedAtom ? val : nullObjectAtom));
    }

    // helper to throw a type error, intentionally not inline
    void throw_checktype_error(SetCache& c, ScriptObject* obj_ptr, Atom val)
    {
        AvmCore* core = obj_ptr->core();
        obj_ptr->toplevel()->throwTypeError(kCheckTypeFailedError, core->atomToErrorString(val),
                core->toErrorString(c.slot_type));
    }

    // coerce & store into ScriptObject subclass-typed slot
    REALLY_INLINE void store_cached_slot(SetCache& c, ScriptObject* obj, ScriptObject** slot_ptr, Atom val)
    {
        if (AvmCore::isNullOrUndefined(val) ||
            (atomKind(val) == kObjectType && atomObj(val)->traits()->subtypeof(c.slot_type))) {
            WBRC(c.slot_type->core->gc, obj, slot_ptr, atomPtr(val));
        } else {
            throw_checktype_error(c, obj, val);
        }
    }

    // coerce & store into int slot
    REALLY_INLINE void store_cached_slot(SetCache&, ScriptObject*, int32_t* slot_ptr, Atom val)
    {
        *slot_ptr = AvmCore::integer(val);
    }

    // coerce & store into uint slot
    REALLY_INLINE void store_cached_slot(SetCache&, ScriptObject*, uint32_t* slot_ptr, Atom val)
    {
        *slot_ptr = AvmCore::toUInt32(val);
    }

    // coerce & store into Boolean slot
    REALLY_INLINE void store_cached_slot(SetCache&, ScriptObject*, Bool32* slot_ptr, Atom val)
    {
        *slot_ptr = (Bool32)AvmCore::boolean(val);
    }

    // coerce & store into Number slot
    REALLY_INLINE void store_cached_slot(SetCache&, ScriptObject*, double* slot_ptr, Atom val)
    {
        *slot_ptr = AvmCore::number(val);
    }

    // set slot on an object, sst_scriptobject, sst_int32, sst_uint32
    template <class T>
    void setprop_slot(SetCache& c, Atom obj, Atom val, MethodEnv* env)
    {
        PROF_IF ("setprop_slot hit", OBJ_HIT(obj, c)) {
            ScriptObject* obj_ptr = atomObj(obj);
            T* slot_ptr = (T*) (uintptr_t(obj_ptr) + c.slot_offset);
            store_cached_slot(c, obj_ptr, slot_ptr, val);
        } else {
            setprop_miss(c, obj, val, env);
        }
    }

    // storing dynamic property on object
    void setprop_none(SetCache& c, Atom obj, Atom val, MethodEnv* env)
    {
        PROF_IF ("setprop_none hit", OBJ_HIT(obj, c)) {
            atomObj(obj)->setMultinameProperty(c.name, val);
        } else {
            setprop_miss(c, obj, val, env);
        }
    }

    // calling a setter on an object
    void setprop_setter(SetCache& c, Atom obj, Atom val, MethodEnv* env)
    {
        PROF_IF ("setprop_setter hit", OBJ_HIT(obj, c)) {
            // todo; obj is always a good type here
            // we could specialize coerceUnboxEnter on the setter type, like for slots
            Atom args[2] = { obj, val };
            c.method->coerceEnter(1, args);
        } else {
            setprop_miss(c, obj, val, env);
        }
    }

    // handlers for sets on ScriptObject* objects, indexed by bindingKind(Binding)
    static const SetCache::Handler setprop_obj_handlers[8] = {
        &setprop_none,          // BKIND_NONE
        &setprop_generic,       // BKIND_METHOD (dynamic set for xml, otherwise error)
        0,                      // BKIND_VAR (will use slot handler table below)
        &setprop_generic,       // BKIND_CONST (error)
        0,                      // BKIND_unused (impossible)
        &setprop_generic,       // BKIND_GET (error)
        &setprop_setter,        // BKIND_SET
        &setprop_setter         // BKIND_GETSET
    };

    // handlers for sets of a declared var on a ScriptObject, indexed by bindingKind
    static const SetCache::Handler setprop_slot_handlers[SST_MAX_VALUE] = {
        &setprop_slot<OpaqueAtom>,          // SST_atom (for slot type Object.  type * is an exception)
        &setprop_generic,                   // SST_string
        &setprop_generic,                   // SST_namespace
        &setprop_slot<ScriptObject*>,       // SST_scriptobject
        &setprop_slot<int32_t>,             // SST_int32
        &setprop_slot<uint32_t>,            // SST_uint32
        &setprop_slot<Bool32>,              // SST_bool32
        &setprop_slot<double>,              // SST_double
    };

    void setprop_miss(SetCache& c, Atom obj, Atom val, MethodEnv* env)
    {
        // cache handler when cache miss occurs
        AvmAssert(!AvmCore::isNullOrUndefined(obj));
        Toplevel* toplevel = env->toplevel();
        VTable* vtable = toplevel->toVTable(obj);
        Traits* actual_type = vtable->traits;
        Binding b = toplevel->getBinding(actual_type, c.name);
        if (AvmCore::hasSetterBinding(b)) {
            c.method = vtable->methods[AvmCore::bindingToSetterId(b)];
        }
        if (isObjectPtr(obj)) {
            c.vtable = vtable;
            if (AvmCore::isVarBinding(b)) {
                // precompute the slot offset, then install a cache handler that's
                // hardwired to coerce Atom to slot type then store
                void *slot_ptr, *obj_ptr = atomObj(obj);
                uint32_t slot = AvmCore::bindingToSlotId(b);
                const TraitsBindings* tb = actual_type->getTraitsBindings();
                const SlotStorageType sst = tb->calcSlotAddrAndSST(slot, obj_ptr, slot_ptr);
                // use handler based on sst
                c.set_handler = setprop_slot_handlers[sst];
                c.slot_type = tb->getSlotTraits(slot);
                c.slot_offset = uintptr_t(slot_ptr) - uintptr_t(obj_ptr);
                if (sst == SST_atom) {
                    // slot type is * or Object, handlers want gc ptr instead of traits ptr
                    if (c.slot_type == NULL) {
                        // slot type is *
                        c.set_handler = setprop_slot_any;
                    }
                    // This line must come after the test of c.slot_type above, as c.gc overlays it!
                    // See the union in class SetCache.
                    c.gc = actual_type->core->gc;
                }
            } else {
                // non-var handler, including const slots (setting them is an error)
                c.set_handler = setprop_obj_handlers[AvmCore::bindingKind(b)];
            }
        } else {
            // must be a primitive: int, bool, string, namespace, or number.
            // all paths lead to an error, so just use the generic handler.
            c.set_handler = &setprop_generic;
        }
        c.set_handler(c, obj, val, env);
    }
    FUNCTION(CALL_INDIRECT, SIG5(V,P,P,A,A,P), set_cache_handler)

    // fully dynamic generic handler for OP_setproperty
    void setprop_late(MethodEnv* env, Atom obj, const Multiname* name, Atom val)
    {
        SetCache c(name, NULL);  // temporary cache, just so we can call the generic handler.
        setprop_generic(c, obj, val, env);
    }
    FUNCTION(FUNCADDR(setprop_late), SIG4(V,P,A,P,A), setprop_late)

    // fully dynamic generic handler for OP_intproperty
    void initprop_late(MethodEnv* env, Atom obj, const Multiname* name, Atom value)
    {
        tagprof("initprop_late", obj);
        env->initproperty(obj, name, value, toVTable(env->toplevel(), obj));
    }
    FUNCTION(FUNCADDR(initprop_late), SIG4(V,P,A,P,A), initprop_late)

    // implements OP_getproperty with unknown base object or index type, but a multiname
    // that includes public and therefore exposes dynamic properties
    Atom getprop_index(MethodEnv* caller_env, Atom obj, const Multiname *name, Atom index)
    {
        if (atomIsIntptr(index) && atomCanBeUint32(index)) {
            if (isObjectPtr(obj)) {
                _nvprof("getprop_index P-fast", 1);
                return AvmCore::atomToScriptObject(obj)->getUintProperty(uint32_t(atomGetIntptr(index)));
            }
        }
        _nvprof("getprop_index P-fast", 0);
        Multiname tempname = *name;
        VTable* vtable = toVTable(caller_env->toplevel(), obj);
        return caller_env->getpropertyHelper(obj, &tempname, vtable, index);
    }
    FUNCTION(FUNCADDR(getprop_index), SIG4(A,P,A,P,A), getprop_index)

    // called when we don't know the base object type and we have a runtime
    // index expression that is public and therefore able to access dynamic properties.
    // (typically this is late-bound array access)
    void setprop_index(MethodEnv* caller_env, Atom obj, const Multiname* name, Atom value, Atom index)
    {
        if (isObjectPtr(obj)) {
            if (atomIsIntptr(index) && atomCanBeUint32(index)) {
                // todo: obj is probably a dense array or vector
                AvmCore::atomToScriptObject(obj)->setUintProperty(uint32_t(atomGetIntptr(index)), value);
                _nvprof("setprop_index P-fast", 1);
                return;
            }
        }

        _nvprof("setprop_index P-fast", 0);
        Multiname tempname = *name;
        caller_env->setpropertyHelper(obj, &tempname, value, toVTable(caller_env->toplevel(), obj), index);
    }
    FUNCTION(FUNCADDR(setprop_index),  SIG5(V,P,A,P,A,A), setprop_index)

#ifdef VMCFG_FASTPATH_ADD_INLINE
    // This is equivalent to setprop_index but performs addition of index1 and index2
    // to compute the index. This is only run when (index1+index2) overflows an int32_t value
    // or is negative and is not time critical.
    void setprop_index_add(MethodEnv* caller_env, Atom obj, const Multiname* name, Atom value, int32_t index1, int32_t index2)
    {
        Atom index = caller_env->toplevel()->core()->doubleToAtom((double) index1 + (double) index2);
        if (isObjectPtr(obj)) {
            if (atomIsIntptr(index) && atomCanBeUint32(index)) {
                // todo: obj is probably a dense array or vector
                AvmCore::atomToScriptObject(obj)->setUintProperty(uint32_t(atomGetIntptr(index)), value);
                _nvprof("setprop_index_add P-fast", 1);
                return;
            }
        }

        _nvprof("setprop_index_add P-fast", 0);
        Multiname tempname = *name;
        caller_env->setpropertyHelper(obj, &tempname, value, toVTable(caller_env->toplevel(), obj), index);
    }
    FUNCTION(FUNCADDR(setprop_index_add),  SIG6(V,P,A,P,A,I,I), setprop_index_add)

    // This is equivalent to setprop_index but performs subtraction of index1 and index2
    // to compute the index. This is only run when (index1-index2) overflows an int32_t value
    // or is negative and is not time critical.
    void setprop_index_subtract(MethodEnv* caller_env, Atom obj, const Multiname* name, Atom value, int32_t index1, int32_t index2)
    {
        Atom index = caller_env->toplevel()->core()->doubleToAtom((double) index1 - (double) index2);
        if (isObjectPtr(obj)) {
            if (atomIsIntptr(index) && atomCanBeUint32(index)) {
                // todo: obj is probably a dense array or vector
                AvmCore::atomToScriptObject(obj)->setUintProperty(uint32_t(atomGetIntptr(index)), value);
                _nvprof("setprop_index_subtract P-fast", 1);
                return;
            }
        }

        _nvprof("setprop_index_subtract P-fast", 0);
        Multiname tempname = *name;
        caller_env->setpropertyHelper(obj, &tempname, value, toVTable(caller_env->toplevel(), obj), index);
    }
    FUNCTION(FUNCADDR(setprop_index_subtract),  SIG6(V,P,A,P,A,I,I), setprop_index_subtract)
#endif // VMCFG_FASTPATH_ADD_INLINE

    // helper for OP_initproperty, called when the namespace has a dynamic name
    // but known namespace that includes public, typical for late bound a[i] = ...
    // expressions occurring within constructors.
    void initprop_index(MethodEnv* caller_env, Atom obj, const Multiname* name, Atom value, Atom index)
    {
        _nvprof("initprop_index", 1);
        Multiname tempname = *name;
        VTable* vtable = toVTable(caller_env->toplevel(), obj);
        caller_env->initpropertyHelper(obj, &tempname, value, vtable, index);
    }
    FUNCTION(FUNCADDR(initprop_index),  SIG5(V,P,A,P,A,A), initprop_index)

#ifdef VMCFG_FASTPATH_ADD_INLINE
    // This is equivalent to initprop_index but performs addition of index1 and index2
    // to compute the index. This is only run when (index1+index2) overflows an int32_t value
    // or is negative and is not time critical.
    void initprop_index_add(MethodEnv* caller_env, Atom obj, const Multiname* name, Atom value, int32_t index1, int32_t index2)
    {
        Atom index = caller_env->toplevel()->core()->doubleToAtom((double) index1 + (double) index2);
        _nvprof("initprop_index_add", 1);
        Multiname tempname = *name;
        VTable* vtable = toVTable(caller_env->toplevel(), obj);
        caller_env->initpropertyHelper(obj, &tempname, value, vtable, index);
    }
    FUNCTION(FUNCADDR(initprop_index_add),  SIG6(V,P,A,P,A,I,I), initprop_index_add)

    // This is equivalent to initprop_index but performs subtraction of index1 and index2
    // to compute the index. This is only run when (index1-index2) overflows an int32_t value
    // or is negative and is not time critical.
    void initprop_index_subtract(MethodEnv* caller_env, Atom obj, const Multiname* name, Atom value, int32_t index1, int32_t index2)
    {
        Atom index = caller_env->toplevel()->core()->doubleToAtom((double) index1 - (double) index2);
        _nvprof("initprop_index_subtract", 1);
        Multiname tempname = *name;
        VTable* vtable = toVTable(caller_env->toplevel(), obj);
        caller_env->initpropertyHelper(obj, &tempname, value, vtable, index);
    }
    FUNCTION(FUNCADDR(initprop_index_subtract),  SIG6(V,P,A,P,A,I,I), initprop_index_subtract)
#endif // VMCFG_FASTPATH_ADD_INLINE

    METHOD(COREADDR(AvmCore::setDxns), SIG3(V,P,P,P), setDxns)
    METHOD(COREADDR(AvmCore::setDxnsLate), SIG3(V,P,P,A), setDxnsLate)

    // implementation of OP_istypelate with unknown type
    int istypelate(MethodEnv* env, Atom val, Atom type)
    {
        AvmAssert(!AvmCore::isNullOrUndefined(type)); // jit inlined the null check.
        Traits* t = env->toplevel()->toClassITraits(type);
        return AvmCore::istype(val, t); // implicitly cast bool -> int
    }
    FUNCTION(FUNCADDR(istypelate), SIG3(I,P,A,A), istypelate)

    // implementation of OP_istype (known type, no side effects)
    int istype(Atom val, Traits* t)
    {
        return AvmCore::istype(val, t); // implicitly cast bool -> int
    }
    PUREFUNCTION(FUNCADDR(istype), SIG2(I,A,P), istype)

    uint32_t restargcHelper(ArrayObject* restLocal, uint32_t argc)
    {
        if (restLocal == 0) // Array has not been created lazily
            return argc;

        // The array has been created lazily; get the array length.
        // We know it is a proper Array, earlier analysis guarantees
        // that there are no assignments to the variable.
        return ((ArrayObject*)restLocal)->get_length();
    }
    FUNCTION(FUNCADDR(restargcHelper), SIG2(U,P,U), restargcHelper)

    Atom restargHelper(Toplevel* toplevel, const Multiname* m, Atom val, ArrayObject** restLocal, uint32_t argc, Atom* restArea)
    {
        uint32_t index;

        if (*restLocal != 0)    // Array has been created lazily
            goto exception;

        if (atomIsIntptr(val) && atomCanBeUint32(val)) {
            index = (uint32_t)atomGetIntptr(val);
            if (index >= argc) // Out of range
                goto exception;
        }
        else if (atomKind(val) == kDoubleType) {
            // Non-int index, check for Number values that are in the int range and nonnegative
            double d = AvmCore::atomToDouble(val);
            if (!(d >= 0 && d <= 0xFFFFFFFFU && MathUtils::floor(d) == d && (index = (uint32_t)d) < argc))
                goto exception;
        }
        else
            goto exception;

        return restArea[index];

    exception:
        // Corner cases for which we cons the array if it hasn't been
        // consed already: non-int or out-of-range property name.
        if (*restLocal == 0)
            *restLocal = toplevel->arrayClass()->newarray(restArea, argc);

        // Obvious optimization
        if (atomIsIntptr(val) && atomCanBeUint32(val))
            return ((ArrayObject*)*restLocal)->getUintProperty((uint32_t)atomGetIntptr(val));

        ArrayObject* array = *restLocal;
        Multiname m2(*m);
        // Multiname requires an interned string for setName
        m2.setName(toplevel->core()->intern(val));
        return toplevel->getproperty(array->atom(), &m2, array->vtable);
    }
    FUNCTION(FUNCADDR(restargHelper), SIG6(A,P,P,A,P,U,P), restargHelper)

    PUREFASTFUNCTION(FUNCADDR(AvmCore::stricteq), SIG2(A,A,A), stricteq)
    METHOD(COREADDR(AvmCore::equals), SIG3(A,P,A,A), equals)
    PUREMETHOD(COREADDR(AvmCore::concatStrings), SIG3(P,P,P,P), concatStrings)

#ifdef VMCFG_FASTPATH_ADD
    FUNCTION(FUNCADDR(op_add_a_aa), SIG3(A,P,A,A), op_add_a_aa)
    FUNCTION(FUNCADDR(op_add_a_ai), SIG3(A,P,A,I), op_add_a_ai)
    FUNCTION(FUNCADDR(op_add_a_ia), SIG3(A,P,I,A), op_add_a_ia)
    FUNCTION(FUNCADDR(op_add_a_ad), SIG3(A,P,A,F), op_add_a_ad)
    FUNCTION(FUNCADDR(op_add_a_da), SIG3(A,P,F,A), op_add_a_da)
#else
    FUNCTION(FUNCADDR(op_add), SIG3(A,P,A,A), op_add)
#endif

    PUREMETHOD(COREADDR(AvmCore::EscapeAttributeValue), SIG2(P,P,A), EscapeAttributeValue)
    PUREMETHOD(COREADDR(AvmCore::ToXMLString), SIG2(P,P,A), ToXMLString)
    METHOD(ENVADDR(MethodEnv::delpropertyHelper), SIG4(A,P,A,P,A), delpropertyHelper)
    METHOD(ENVADDR(MethodEnv::internRtns), SIG2(P,P,A), internRtns)
    METHOD(ENVADDR(MethodEnv::delproperty), SIG3(A,P,A,P), delproperty)

    METHOD(ENVADDR(MethodEnv::setpropertylate_u), SIG4(V,P,A,U,A), setpropertylate_u)
    METHOD(ARRAYADDR(ArrayObject::_setUintProperty), SIG3(V,P,U,A), ArrayObject_setUintProperty)
    METHOD(VECTOROBJADDR(ObjectVectorObject::_setUintProperty), SIG3(V,P,U,A), ObjectVectorObject_setUintProperty)
    METHOD(VECTOROBJADDR(ObjectVectorObject::_setKnownUintProperty), SIG3(V,P,U,A), ObjectVectorObject_setKnownUintProperty)
    METHOD(VECTOROBJADDR(ObjectVectorObject::_setKnownUintPropertyWithPointer), SIG3(V,P,U,A), ObjectVectorObject_setKnownUintPropertyWithPointer)
    METHOD(VECTORUINTADDR(UIntVectorObject::_setUintProperty), SIG3(V,P,U,A), UIntVectorObject_setUintProperty)
    METHOD(VECTORUINTADDR(UIntVectorObject::_setNativeUintProperty), SIG3(V,P,U,U), UIntVectorObject_setNativeUintProperty)
    METHOD(VECTORINTADDR(IntVectorObject::_setUintProperty), SIG3(V,P,U,A), IntVectorObject_setUintProperty)
    METHOD(VECTORINTADDR(IntVectorObject::_setNativeUintProperty), SIG3(V,P,U,I), IntVectorObject_setNativeUintProperty)
    METHOD(VECTORDOUBLEADDR(DoubleVectorObject::_setUintProperty), SIG3(V,P,U,A), DoubleVectorObject_setUintProperty)
    METHOD(VECTORDOUBLEADDR(DoubleVectorObject::_setNativeUintProperty), SIG3(V,P,U,F), DoubleVectorObject_setNativeUintProperty)

    METHOD(ENVADDR(MethodEnv::setpropertylate_i), SIG4(V,P,A,I,A), setpropertylate_i)
    METHOD(ARRAYADDR(ArrayObject::_setIntProperty), SIG3(V,P,I,A), ArrayObject_setIntProperty)
    METHOD(VECTOROBJADDR(ObjectVectorObject::_setIntProperty), SIG3(V,P,I,A), ObjectVectorObject_setIntProperty)
    METHOD(VECTOROBJADDR(ObjectVectorObject::_setKnownIntProperty), SIG3(V,P,I,A), ObjectVectorObject_setKnownIntProperty)
    METHOD(VECTOROBJADDR(ObjectVectorObject::_setKnownIntPropertyWithPointer), SIG3(V,P,I,A), ObjectVectorObject_setKnownIntPropertyWithPointer)
    METHOD(VECTORUINTADDR(UIntVectorObject::_setIntProperty), SIG3(V,P,I,A), UIntVectorObject_setIntProperty)
    METHOD(VECTORUINTADDR(UIntVectorObject::_setNativeIntProperty), SIG3(V,P,I,U), UIntVectorObject_setNativeIntProperty)
    METHOD(VECTORINTADDR(IntVectorObject::_setIntProperty), SIG3(V,P,I,A), IntVectorObject_setIntProperty)
    METHOD(VECTORINTADDR(IntVectorObject::_setNativeIntProperty), SIG3(V,P,I,I), IntVectorObject_setNativeIntProperty)
    METHOD(VECTORDOUBLEADDR(DoubleVectorObject::_setIntProperty), SIG3(V,P,I,A), DoubleVectorObject_setIntProperty)
    METHOD(VECTORDOUBLEADDR(DoubleVectorObject::_setNativeIntProperty), SIG3(V,P,I,F), DoubleVectorObject_setNativeIntProperty)

    METHOD(ENVADDR(MethodEnv::setpropertylate_d), SIG4(V,P,A,F,A), setpropertylate_d)
    METHOD(ARRAYADDR(ArrayObject::_setDoubleProperty), SIG3(V,P,F,A), ArrayObject_setDoubleProperty)
    METHOD(VECTOROBJADDR(ObjectVectorObject::_setDoubleProperty), SIG3(V,P,F,A), ObjectVectorObject_setDoubleProperty)
    METHOD(VECTOROBJADDR(ObjectVectorObject::_setKnownDoubleProperty), SIG3(V,P,F,A), ObjectVectorObject_setKnownDoubleProperty)
    METHOD(VECTOROBJADDR(ObjectVectorObject::_setKnownDoublePropertyWithPointer), SIG3(V,P,F,A), ObjectVectorObject_setKnownDoublePropertyWithPointer)
    METHOD(VECTORUINTADDR(UIntVectorObject::_setDoubleProperty), SIG3(V,P,F,A), UIntVectorObject_setDoubleProperty)
    METHOD(VECTORUINTADDR(UIntVectorObject::_setNativeDoubleProperty), SIG3(V,P,F,U), UIntVectorObject_setNativeDoubleProperty)
    METHOD(VECTORINTADDR(IntVectorObject::_setDoubleProperty), SIG3(V,P,F,A), IntVectorObject_setDoubleProperty)
    METHOD(VECTORINTADDR(IntVectorObject::_setNativeDoubleProperty), SIG3(V,P,F,I), IntVectorObject_setNativeDoubleProperty)
    METHOD(VECTORDOUBLEADDR(DoubleVectorObject::_setDoubleProperty), SIG3(V,P,F,A), DoubleVectorObject_setDoubleProperty)
    METHOD(VECTORDOUBLEADDR(DoubleVectorObject::_setNativeDoubleProperty), SIG3(V,P,F,F), DoubleVectorObject_setNativeDoubleProperty)

    METHOD(ENVADDR(MethodEnv::getpropertylate_u), SIG3(A,P,A,U), getpropertylate_u)
    METHOD(ARRAYADDR(ArrayObject::_getUintProperty), SIG2(A,P,U), ArrayObject_getUintProperty)
    METHOD(VECTOROBJADDR(ObjectVectorObject::_getUintProperty), SIG2(A,P,U), ObjectVectorObject_getUintProperty)
    METHOD(VECTORUINTADDR(UIntVectorObject::_getUintProperty), SIG2(A,P,U), UIntVectorObject_getUintProperty)
    METHOD(VECTORUINTADDR(UIntVectorObject::_getNativeUintProperty), SIG2(U,P,U), UIntVectorObject_getNativeUintProperty)
    METHOD(VECTORINTADDR(IntVectorObject::_getUintProperty), SIG2(A,P,U), IntVectorObject_getUintProperty)
    METHOD(VECTORINTADDR(IntVectorObject::_getNativeUintProperty), SIG2(I,P,U), IntVectorObject_getNativeUintProperty)
    METHOD(VECTORDOUBLEADDR(DoubleVectorObject::_getUintProperty), SIG2(A,P,U), DoubleVectorObject_getUintProperty)
    METHOD(VECTORDOUBLEADDR(DoubleVectorObject::_getNativeUintProperty), SIG2(F,P,U), DoubleVectorObject_getNativeUintProperty)

    METHOD(ENVADDR(MethodEnv::getpropertylate_i), SIG3(A,P,A,I), getpropertylate_i)
    METHOD(ARRAYADDR(ArrayObject::_getIntProperty), SIG2(A,P,I), ArrayObject_getIntProperty)
    METHOD(VECTOROBJADDR(ObjectVectorObject::_getIntProperty), SIG2(A,P,I), ObjectVectorObject_getIntProperty)
    METHOD(VECTORUINTADDR(UIntVectorObject::_getIntProperty), SIG2(A,P,I), UIntVectorObject_getIntProperty)
    METHOD(VECTORUINTADDR(UIntVectorObject::_getNativeIntProperty), SIG2(U,P,I), UIntVectorObject_getNativeIntProperty)
    METHOD(VECTORINTADDR(IntVectorObject::_getIntProperty), SIG2(A,P,I), IntVectorObject_getIntProperty)
    METHOD(VECTORINTADDR(IntVectorObject::_getNativeIntProperty), SIG2(I,P,I), IntVectorObject_getNativeIntProperty)
    METHOD(VECTORDOUBLEADDR(DoubleVectorObject::_getIntProperty), SIG2(A,P,I), DoubleVectorObject_getIntProperty)
    METHOD(VECTORDOUBLEADDR(DoubleVectorObject::_getNativeIntProperty), SIG2(F,P,I), DoubleVectorObject_getNativeIntProperty)

    METHOD(ENVADDR(MethodEnv::getpropertylate_d), SIG3(A,P,A,F), getpropertylate_d)
    METHOD(ARRAYADDR(ArrayObject::_getDoubleProperty), SIG2(A,P,F), ArrayObject_getDoubleProperty)
    METHOD(VECTOROBJADDR(ObjectVectorObject::_getDoubleProperty), SIG2(A,P,F), ObjectVectorObject_getDoubleProperty)
    METHOD(VECTORINTADDR(IntVectorObject::_getDoubleProperty), SIG2(I,P,F), IntVectorObject_getDoubleProperty)
    METHOD(VECTORINTADDR(IntVectorObject::_getNativeDoubleProperty), SIG2(I,P,F), IntVectorObject_getNativeDoubleProperty)
    METHOD(VECTORUINTADDR(UIntVectorObject::_getDoubleProperty), SIG2(U,P,F), UIntVectorObject_getDoubleProperty)
    METHOD(VECTORUINTADDR(UIntVectorObject::_getNativeDoubleProperty), SIG2(U,P,F), UIntVectorObject_getNativeDoubleProperty)
    METHOD(VECTORDOUBLEADDR(DoubleVectorObject::_getDoubleProperty), SIG2(F,P,F), DoubleVectorObject_getDoubleProperty)
    METHOD(VECTORDOUBLEADDRF(DoubleVectorObject::_getNativeDoubleProperty), SIG2(F,P,F), DoubleVectorObject_getNativeDoubleProperty)

    METHOD(ENVADDR(MethodEnv::haspropertylate_u), SIG3(I,P,A,U), haspropertylate_u)
    METHOD(ENVADDR(MethodEnv::haspropertylate_i), SIG3(I,P,A,I), haspropertylate_i)
    METHOD(ENVADDR(MethodEnv::findproperty), SIG7(A,P,P,P,I,P,B,P), findproperty)
    METHOD(ENVADDR(MethodEnv::checkfilter), SIG2(V,P,A), checkfilter)
    METHOD(ENVADDR(MethodEnv::getdescendants), SIG3(A,P,A,P), getdescendants)
    METHOD(ENVADDR(MethodEnv::newclass), SIG5(P,P,P,P,P,P), newclass)
    METHOD(ENVADDR(MethodEnv::op_newobject), SIG3(P,P,P,I), op_newobject)

    typedef Atom (*constructprop_MethodEnv)(MethodEnv*, const Multiname*, int, Atom*);
    FUNCTION(FUNCADDR((constructprop_MethodEnv)&constructprop<MethodEnv*>), SIG4(A,P,P,I,P), construct_late)

    typedef Atom (*op_construct_MethodEnv)(MethodEnv*, Atom, int, Atom*);
    FUNCTION(FUNCADDR((op_construct_MethodEnv)&op_construct<MethodEnv*>), SIG4(A,P,A,I,P), op_construct)

    METHOD(ENVADDR(MethodEnv::callsuper), SIG4(A,P,P,I,P), callsuper)
    METHOD(ENVADDR(MethodEnv::setsuper), SIG4(V,P,A,P,A), setsuper)
    METHOD(ENVADDR(MethodEnv::getsuper), SIG3(A,P,A,P), getsuper)
    METHOD(COREADDR(AvmCore::throwAtom), SIG2(V,P,A), throwAtom)
    PUREFUNCTION(FUNCADDR(MathUtils::mod), SIG2(F,F,F), mod)
    PUREFUNCTION(FUNCADDR(MathUtils::acos), SIG1(F,F), Math_acos)
    PUREFUNCTION(FUNCADDR(MathUtils::asin), SIG1(F,F), Math_asin)
    PUREFUNCTION(FUNCADDR(MathUtils::atan), SIG1(F,F), Math_atan)
    PUREFUNCTION(FUNCADDR(MathUtils::ceil), SIG1(F,F), Math_ceil)
    PUREFUNCTION(FUNCADDR(MathUtils::cos), SIG1(F,F), Math_cos)
    PUREFUNCTION(FUNCADDR(MathUtils::exp), SIG1(F,F), Math_exp)
    PUREFUNCTION(FUNCADDR(MathUtils::floor), SIG1(F,F), Math_floor)
    PUREFUNCTION(FUNCADDR(MathUtils::log), SIG1(F,F), Math_log)
    PUREFUNCTION(FUNCADDR(MathUtils::round), SIG1(F,F), Math_round)
    PUREFUNCTION(FUNCADDR(MathUtils::sin), SIG1(F,F), Math_sin)
    PUREFUNCTION(FUNCADDR(MathUtils::sqrt), SIG1(F,F), Math_sqrt)
    PUREFUNCTION(FUNCADDR(MathUtils::tan), SIG1(F,F), Math_tan)
    PUREFUNCTION(FUNCADDR(MathUtils::atan2), SIG2(F,F,F), Math_atan2)
    PUREFUNCTION(FUNCADDR(MathUtils::pow), SIG2(F,F,F), Math_pow)
    PUREMETHOD(COREADDR(AvmCore::_typeof), SIG2(P,P,A), typeof)
    FUNCTION(FUNCADDR(AvmCore::atomWriteBarrier), SIG4(V,P,P,P,A), atomWriteBarrier)
    METHOD(GCADDR(GC::privateWriteBarrierRC), SIG4(V,P,P,P,P), privateWriteBarrierRC)
    PUREMETHOD(COREADDR(AvmCore::uintToAtom), SIG2(A,P,U), uintToAtom)
    PUREMETHOD(COREADDR(AvmCore::intToAtom), SIG2(A,P,I), intToAtom)
    FASTFUNCTION(FUNCADDR(AvmCore::compare), SIG2(A,A,A), compare)
    PUREMETHOD(ENVADDR(MethodEnv::createRestHelper), SIG3(P,P,I,P), createRestHelper)
    PUREMETHOD(ENVADDR(MethodEnv::createArgumentsHelper), SIG3(P,P,I,P), createArgumentsHelper)

    void initMultinameLate(AvmCore* core, Multiname& name, Atom index)
    {
        if (AvmCore::isObject(index))
        {
            ScriptObject* i = AvmCore::atomToScriptObject(index);
            if (i->traits() == core->traits.qName_itraits)
            {
                QNameObject* qname = (QNameObject*) i;
                bool attr = name.isAttr();
                qname->getMultiname(name);
                name.setAttr(attr);
                return;
            }
        }

        name.setName(core->intern(index));
    }
    FUNCTION(FUNCADDR(initMultinameLate), SIG3(V,P,P,A), initMultinameLate)

    METHOD(ENVADDR(MethodEnv::initMultinameLateForDelete), SIG3(V,P,P,A), initMultinameLateForDelete)
    PUREFUNCTION(FUNCADDR(MathUtils::doubleToBool), SIG1(I,F), doubleToBool)

SSE2_ONLY(
    PUREMETHOD(COREADDR(AvmCore::doubleToAtom_sse2), SIG2(A,P,F), doubleToAtom_sse2)
    PUREFUNCTION(FUNCADDR(AvmCore::integer_d_sse2), SIG1(I,F), integer_d_sse2)
)

#ifdef DEBUGGER
    METHOD(ENVADDR(MethodEnv::debugEnter), SIG5(V,P,P,P,P,P), debugEnter)
    METHOD(ENVADDR(MethodEnv::debugExit), SIG2(V,P,P), debugExit)
    METHOD(DEBUGGERADDR(Debugger::debugFile), SIG2(V,P,P), debugFile)
    METHOD(DEBUGGERADDR(Debugger::debugLine), SIG2(V,P,I), debugLine)
    METHOD(COREADDR(AvmCore::sampleCheck), SIG1(V,P), sampleCheck)
#endif

    // MOPS helper functions
    FASTFUNCTION(FUNCADDR(mop_rangeCheckFailed), SIG1(V,P), mop_rangeCheckFailed)
    FASTFUNCTION(FUNCADDR(mop_lix8), SIG1(I,P), mop_lix8)
    FASTFUNCTION(FUNCADDR(mop_lix16), SIG1(I,P), mop_lix16)
    FASTFUNCTION(FUNCADDR(mop_liz8), SIG1(I,P), mop_liz8)
    FASTFUNCTION(FUNCADDR(mop_liz16), SIG1(I,P), mop_liz16)
    FASTFUNCTION(FUNCADDR(mop_li32), SIG1(I,P), mop_li32)
    FASTFUNCTION(FUNCADDR(mop_lf32), SIG1(F,P), mop_lf32)
    FASTFUNCTION(FUNCADDR(mop_lf64), SIG1(F,P), mop_lf64)
    FASTFUNCTION(FUNCADDR(mop_si8), SIG2(V,P,I), mop_si8)
    FASTFUNCTION(FUNCADDR(mop_si16), SIG2(V,P,I), mop_si16)
    FASTFUNCTION(FUNCADDR(mop_si32), SIG2(V,P,I), mop_si32)
    FUNCTION(FUNCADDR(mop_sf32), SIG2(V,P,F), mop_sf32)
    FUNCTION(FUNCADDR(mop_sf64), SIG2(V,P,F), mop_sf64)

    typedef Atom (*op_call_MethodEnv)(MethodEnv*, Atom, int, Atom*);
    FUNCTION(FUNCADDR((op_call_MethodEnv)&avmplus::op_call<MethodEnv*>), SIG4(A,P,A,I,P), op_call)

    typedef Atom (*op_applytype_MethodEnv)(MethodEnv*, Atom, int, Atom*);
    FUNCTION(FUNCADDR((op_applytype_MethodEnv)&avmplus::op_applytype<MethodEnv*>), SIG4(A,P,A,I,P), op_applytype)

    typedef ArrayObject* (*newarray_MethodEnv)(MethodEnv*, int, Atom*);
    FUNCTION(FUNCADDR((newarray_MethodEnv)&newarray<MethodEnv*>), SIG3(P,P,I,P), newarray)

    typedef Atom (*astype_late_MethodEnv)(MethodEnv*, Atom, Atom);
    PUREFUNCTION(FUNCADDR((astype_late_MethodEnv)&astype_late<MethodEnv*>), SIG3(A,P,A,A), astype_late)
    PUREFUNCTION(FUNCADDR(AvmCore::astype), SIG2(A,A,P), astype)

    typedef Atom (*instanceof_MethodEnv)(MethodEnv*, Atom, Atom);
    FUNCTION(FUNCADDR((instanceof_MethodEnv)&instanceof<MethodEnv*>), SIG3(A,P,A,A), instanceof)

    typedef Atom (*op_in_MethodEnv)(MethodEnv*, Atom, Atom);
    FUNCTION(FUNCADDR((op_in_MethodEnv)&op_in<MethodEnv*>), SIG3(A,P,A,A), op_in)

    FUNCTION(FUNCADDR(&coerceobj_obj), SIG3(V,P,P,P), coerceobj_obj)
    FUNCTION(FUNCADDR(&coerceobj_atom), SIG3(V,P,A,P), coerceobj_atom)

    typedef Atom (*coerce_MethodEnv)(MethodEnv*, Atom, Traits*);
    PUREFUNCTION(FUNCADDR((coerce_MethodEnv)&coerce<MethodEnv*>), SIG3(A,P,A,P), coerce)

    PUREFUNCTION(FUNCADDR(finddef_cache), SIG3(P,P,P,U), finddef_cache)

    METHOD(ENVADDR(MethodEnv::argcError), SIG2(V,P,I), argcError)

    // JIT profiling

#ifdef DOPROF
    void jitProfileEvent(void* id)
    {
        _jvprof(id, 1);
    }

    void jitProfileValue32(void* id, int32_t value)
    {
        _jvprof(id, value);
    }

    void jitProfileHist32(void* id, int32_t value)
    {
        _jhprof(id, value);
    }

    FUNCTION(FUNCADDR(jitProfileEvent), SIG1(V,P), jitProfileEvent)
    FUNCTION(FUNCADDR(jitProfileValue32), SIG2(V,P,I), jitProfileValue32)
    FUNCTION(FUNCADDR(jitProfileHist32), SIG2(V,P,I), jitProfileHist32)
#endif
