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
 * Portions created by the Initial Developer are Copyright (C) 2004-2006
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

#ifndef __avmplus_AvmCore_inlines__
#define __avmplus_AvmCore_inlines__

namespace avmplus
{

REALLY_INLINE DomainMgr* AvmCore::domainMgr() const
{
    return m_domainMgr;
}

#ifdef DEBUGGER
REALLY_INLINE Debugger* AvmCore::debugger() const
{
    return _debugger;
}

REALLY_INLINE Profiler* AvmCore::profiler() const
{
    return _profiler;
}

REALLY_INLINE Sampler* AvmCore::get_sampler() const
{
    return _sampler;
}

REALLY_INLINE void AvmCore::sampleCheck()
{
    if (_sampler)
        _sampler->sampleCheck();
}
#endif // DEBUGGER

REALLY_INLINE bool AvmCore::interruptCheck(bool interruptable)
{
    return interruptable && (interrupted != NotInterrupted);
}

REALLY_INLINE QCache* AvmCore::tbCache()
{
    return m_tbCache;
}

REALLY_INLINE QCache* AvmCore::tmCache()
{
    return m_tmCache;
}

REALLY_INLINE QCache* AvmCore::msCache()
{
    return m_msCache;
}

#ifdef VMCFG_NANOJIT // accessors

#if defined AVMPLUS_IA32 || defined AVMPLUS_AMD64
REALLY_INLINE bool AvmCore::use_sse2() const
{
    return config.njconfig.i386_sse2;
}
#endif

#endif // VMCFG_NANOJIT

REALLY_INLINE void AvmCore::stackCheck(MethodEnv* env)
{
    // Take the address of a local variable to get stack pointer
    void* dummy;
    //fprintf(stderr, "%p\n", &dummy);
    if ((uintptr_t)&dummy < minstack)
        handleStackOverflowMethodEnv(env);
}

REALLY_INLINE void AvmCore::stackCheck(Toplevel* toplevel)
{
    // Take the address of a local variable to get stack pointer
    void* dummy;
    //fprintf(stderr, "%p\n", &dummy);
    if ((uintptr_t)&dummy < minstack)
        handleStackOverflowToplevel(toplevel);
}

REALLY_INLINE /*static*/ bool AvmCore::isObject(Atom atom)
{
    return atomKind(atom) == kObjectType && !isNull(atom);
}

REALLY_INLINE /*static*/ bool AvmCore::isPointer(Atom atom)
{
    // kDouble, all kTypes less than kSpecialType, and kSpecialType if it's not "null" - i.e. not undefinedAtom, but a bibopType.
    return (atomKind(atom+1) <= (kSpecialBibopType+1)) && atom!=undefinedAtom;
}

REALLY_INLINE /*static*/ bool AvmCore::isNamespace(Atom atom)
{
    return atomKind(atom) == kNamespaceType && !isNull(atom);
}

REALLY_INLINE /*static*/ BindingKind AvmCore::bindingKind(Binding b)
{
    return BindingKind(uintptr_t(b) & 7);
}

REALLY_INLINE /*static*/ bool AvmCore::isMethodBinding(Binding b)
{
    return bindingKind(b) == BKIND_METHOD;
}

REALLY_INLINE /*static*/ bool AvmCore::isAccessorBinding(Binding b)
{
    return bindingKind(b) >= BKIND_GET;
}

REALLY_INLINE /*static*/ bool AvmCore::hasSetterBinding(Binding b)
{
    return (bindingKind(b) & 6) == BKIND_SET;
}

REALLY_INLINE /*static*/ bool AvmCore::hasGetterBinding(Binding b)
{
    return (bindingKind(b) & 5) == BKIND_GET;
}

REALLY_INLINE /*static*/ int AvmCore::bindingToGetterId(Binding b)
{
    AvmAssert(hasGetterBinding(b));
    return int(uintptr_t(b)) >> 3;
}

REALLY_INLINE /*static*/ int AvmCore::bindingToSetterId(Binding b)
{
    AvmAssert(hasSetterBinding(b));
    return 1 + (int(uintptr_t(b)) >> 3);
}

REALLY_INLINE /*static*/ int AvmCore::bindingToMethodId(Binding b)
{
    AvmAssert(isMethodBinding(b));
    return int(uintptr_t(b)) >> 3;
}

REALLY_INLINE /*static*/ int AvmCore::bindingToSlotId(Binding b)
{
    AvmAssert(isSlotBinding(b));
    return int(uintptr_t(b)) >> 3;
}

REALLY_INLINE /*static*/ int AvmCore::isSlotBinding(Binding b)
{
    AvmAssert((BKIND_CONST & 6)==BKIND_VAR);
    return (bindingKind(b) & 6) == BKIND_VAR;
}

REALLY_INLINE /*static*/ Binding AvmCore::makeSlotBinding(uintptr_t id, BindingKind kind)
{
    AvmAssert(kind == BKIND_VAR || kind == BKIND_CONST);
    return Binding((id << 3) | kind);
}

REALLY_INLINE /*static*/ Binding AvmCore::makeMGSBinding(uintptr_t id, BindingKind kind)
{
    AvmAssert(kind == BKIND_METHOD || kind == BKIND_GET || kind == BKIND_SET);
    return Binding((id << 3) | kind);
}

REALLY_INLINE /*static*/ Binding AvmCore::makeGetSetBinding(Binding b)
{
    AvmAssert(bindingKind(b) == BKIND_GET || bindingKind(b) == BKIND_SET);
    return Binding((uintptr_t(b) & ~7) | BKIND_GETSET);
}

REALLY_INLINE /*static*/ int AvmCore::isVarBinding(Binding b)
{
    return bindingKind(b) == BKIND_VAR;
}

REALLY_INLINE /*static*/ int AvmCore::isConstBinding(Binding b)
{
    return bindingKind(b) == BKIND_CONST;
}

REALLY_INLINE bool AvmCore::isFunction(Atom atom)
{
    return istype(atom, traits.function_itraits);
}

/** Helper method; returns true if atom's type is double */
REALLY_INLINE /*static*/ bool AvmCore::isDouble(Atom atom)
{
    return atomKind(atom) == kDoubleType;
}

REALLY_INLINE /*static*/ bool AvmCore::isNumber(Atom atom)
{
    MMGC_STATIC_ASSERT(kIntptrType == 6 && kDoubleType == 7);
    return (atom&6) == kIntptrType || (atomKind(atom)==kSpecialBibopType && atom!=AtomConstants::undefinedAtom && bibopKind(atom)==kBibopFloatType);
}

REALLY_INLINE /*static*/ bool AvmCore::isBoolean(Atom atom)
{
    return atomKind(atom) == kBooleanType;
}

REALLY_INLINE /*static*/ bool AvmCore::isNull(Atom atom)
{
    return ISNULL(atom);
}

REALLY_INLINE /*static*/ bool AvmCore::isUndefined(Atom atom)
{
    return (atom == undefinedAtom);
}

REALLY_INLINE /*static*/ bool AvmCore::isNullOrUndefined(Atom atom)
{
    return ((uintptr_t)atom) <= (uintptr_t)kSpecialBibopType;
}

REALLY_INLINE /*static*/ uint32_t AvmCore::toUInt32(Atom atom)
{
    return (uint32_t)integer(atom);
}

REALLY_INLINE /*static*/ double AvmCore::toInteger(Atom atom)
{
    if (atomIsIntptr(atom))
    {
        return (double) atomGetIntptr(atom);
    }
    else
    {
        return MathUtils::toInt(AvmCore::number(atom));
    }
}

REALLY_INLINE /*static*/ int32_t AvmCore::integer_i(Atom a)
{
    if (atomIsIntptr(a))
    {
        intptr_t const i = atomGetIntptr(a);
        AvmAssert(i == int32_t(i));
        return int32_t(i);
    }
    else
    {
        return int32_t(atomToDouble(a));
    }
}

REALLY_INLINE /*static*/ uint32_t AvmCore::integer_u(Atom a)
{
    if (atomIsIntptr(a))
    {
        intptr_t const i = atomGetIntptr(a);
        AvmAssert(i == intptr_t(uint32_t(i)));
        return uint32_t(i);
    }
    else
    {
        return (uint32_t) atomToDouble(a);
    }
}

#ifdef AVMPLUS_SSE2_ALWAYS
REALLY_INLINE /*static*/ int AvmCore::integer_d(double d)
{
    return integer_d_sse2(d);
}
REALLY_INLINE Atom AvmCore::doubleToAtom(double n)
{
    return doubleToAtom_sse2(n);
}
#else
// they are defined out-of-line
#endif // AVMPLUS_SSE2_ALWAYS

REALLY_INLINE /*static*/ double AvmCore::number_d(Atom a)
{
    AvmAssert(isNumber(a));

    if (atomIsIntptr(a))
        return (double)atomGetIntptr(a);
    else
        return atomToDouble(a);
}

REALLY_INLINE Atom AvmCore::intAtom(Atom atom)
{
    return intToAtom(AvmCore::integer(atom));
}

REALLY_INLINE Atom AvmCore::uintAtom(Atom atom)
{
    return uintToAtom(AvmCore::toUInt32(atom));
}

REALLY_INLINE /*static*/ bool AvmCore::isString(Atom atom)
{
    return atomKind(atom) == kStringType && !isNull(atom);
}

REALLY_INLINE /*static*/ bool AvmCore::isName(Atom atom)
{
    return isString(atom) && atomToString(atom)->isInterned();
}

REALLY_INLINE ScriptObject* AvmCore::newObject(VTable *vtable, ScriptObject *delegate)
{
    return ScriptObject::create(GetGC(), vtable, delegate);
}

/** Helper function; reads a signed 24-bit integer from pc */
REALLY_INLINE /*static*/ int AvmCore::readS24(const uint8_t *pc)
{
    #ifdef VMCFG_UNALIGNED_INT_ACCESS
        // unaligned short access still faster than 2 byte accesses
        return ((uint16_t*)pc)[0] | ((int8_t*)pc)[2]<<16;
    #else
        return pc[0] | pc[1]<<8 | ((int8_t*)pc)[2]<<16;
    #endif
}

/**
 * Returns the size of the instruction + all it's operands.  For OP_lookupswitch the size will not include
 * the size for the case targets.
 */
REALLY_INLINE /*static*/ int AvmCore::calculateInstructionWidth(const uint8_t* p)
{
    int a, b;
    unsigned int c, d;
    const uint8_t* p2 = p;
    readOperands(p2, c, a, d, b);
    return int(p2-p);
}

/** Helper function; reads an unsigned 16-bit integer from pc */
REALLY_INLINE /*static*/ int32_t AvmCore::readU16(const uint8_t *pc)
{
    #ifdef VMCFG_UNALIGNED_INT_ACCESS
        // unaligned short access still faster than 2 byte accesses
        return *((uint16_t*)pc);
    #else
        return pc[0] | pc[1]<<8;
    #endif
}

// note, return of true means we definitely DO NOT need a coerce,
// but return of false still means we *might* need to (ie, negating the result of this function
// isn't "needscoerce")
REALLY_INLINE /*static*/ bool AvmCore::atomDoesNotNeedCoerce(Atom a, BuiltinType bt)
{
    // cheat and use "kUnusedAtomTag" for all null values (streamlines the test)
    AvmAssert(atomKind(a) != kUnusedAtomTag);
    const int kind = isNull(a) ? kUnusedAtomTag : atomKind(a);
    return ((1<<bt) & k_atomDoesNotNeedCoerce_Masks[kind]) != 0;
}

REALLY_INLINE /*static*/ Atom AvmCore::istypeAtom(Atom atom, Traits* itraits)
{
    return istype(atom, itraits) ? trueAtom : falseAtom;
}

REALLY_INLINE /*static*/ Atom AvmCore::astype(Atom atom, Traits* expected)
{
    return istype(atom, expected) ? atom : nullObjectAtom;
}

// return true if there is a pending interrupt of the specific InterruptReason.
REALLY_INLINE bool AvmCore::interruptCheckReason(InterruptReason r) const
{
    AvmAssert(r != NotInterrupted);
    return interrupted == r;
}

REALLY_INLINE /*static*/ bool AvmCore::isXML(Atom atm)
{
    return isBuiltinType(atm, BUILTIN_xml);
}

REALLY_INLINE /*static*/ bool AvmCore::isXMLList(Atom atm)
{
    return isBuiltinType(atm, BUILTIN_xmlList);
}

REALLY_INLINE /*static*/ bool AvmCore::isXMLorXMLList(Atom atm)
{
    return isBuiltinTypeMask(atm, (1<<BUILTIN_xml)|(1<<BUILTIN_xmlList));
}

REALLY_INLINE /*static*/ bool AvmCore::isDate(Atom atm)
{
    return isBuiltinType(atm, BUILTIN_date);
}

REALLY_INLINE /*static*/ bool AvmCore::isQName(Atom atm)
{
    return isBuiltinType(atm, BUILTIN_qName);
}

REALLY_INLINE /*static*/ bool AvmCore::isDictionaryLookup(Atom key, Atom obj)
{
    return isObject(key) && isDictionary(obj);
}

REALLY_INLINE /*static*/ Namespacep AvmCore::atomToNamespace(Atom atom)
{
    AvmAssert(atomKind(atom)==kNamespaceType);
    return (Namespacep)atomPtr(atom);
}

REALLY_INLINE /*static*/ double AvmCore::atomToDouble(Atom atom)
{
    AvmAssert(atomKind(atom)==kDoubleType);
    return *(const double*)atomPtr(atom);
}

/**
 * Convert an Atom of kStringType to a Stringp
 * @param atom atom to convert.  Note that the Atom
 *             must be of kStringType
 */
REALLY_INLINE /*static*/ Stringp AvmCore::atomToString(Atom atom)
{
    AvmAssert(atomKind(atom)==kStringType);
    return (Stringp)atomPtr(atom);
}

// Avoid adding validation checks here and returning NULL.  If this
// is returning a bad value, the higher level function should be fixed
// or AbcParser/Verifier should be enhanced to catch this case.
REALLY_INLINE /*static*/ ScriptObject* AvmCore::atomToScriptObject(const Atom atom)
{
    AvmAssert(atomKind(atom)==kObjectType);
    return (ScriptObject*)atomPtr(atom);
}

// Helper function, allows generic objects to work with InlineHashtable,
// uses double type which is the only non-RC pointer tag.
// The key here is that these methods round-trip any pointer value to the
// same pointer value, there is no casting that might adjust the pointer.
#ifdef _DEBUG
// defined out-of-line
#else
REALLY_INLINE /*static*/ Atom AvmCore::genericObjectToAtom(const void* obj)
{
    return (Atom)obj|kDoubleType;
}
#endif

REALLY_INLINE /*static*/ const void* AvmCore::atomToGenericObject(Atom a)
{
    return atomPtr(a);
}

REALLY_INLINE /*static*/ bool AvmCore::isGenericObject(Atom a)
{
    return atomKind(a)==kDoubleType;
}

REALLY_INLINE /*static*/ bool AvmCore::getIndexFromAtom(Atom a, uint32_t *result)
{
    if (atomIsIntptr(a) && atomCanBeUint32(a))
    {
        *result = uint32_t(atomGetIntptr(a));
        return true;
    }
    else
    {
        AvmAssert(AvmCore::isString(a));
        return atomToString(a)->parseIndex(*result);
    }
}

REALLY_INLINE Atom AvmCore::allocDouble(double n)
{
    double *d = (double*)GetGC()->AllocDouble();
    *d = n;
    return kDoubleType | (uintptr_t)d;
}

#ifdef VMCFG_LOOKUP_CACHE
REALLY_INLINE uint32_t AvmCore::lookupCacheTimestamp() const
{
    return lookup_cache_timestamp == ~0U ? 0 : lookup_cache_timestamp;
}

REALLY_INLINE bool AvmCore::lookupCacheIsValid(uint32_t t) const
{
    return t == lookup_cache_timestamp;
}

REALLY_INLINE void AvmCore::invalidateLookupCache()
{
    if (lookup_cache_timestamp != ~0U)
        ++lookup_cache_timestamp;
}
#endif // VMCFG_LOOKUP_CACHE

#ifdef VMCFG_NANOJIT
REALLY_INLINE void AvmCore::flushBindingCachesNextSweep()
{
    m_flushBindingCachesNextSweep = true;
}
#endif // VMCFG_NANOJIT

REALLY_INLINE bool AvmCore::isValidApiVersion(ApiVersion apiVersion)
{
    AvmAssert((int)apiVersion >= 0 && (int)apiVersion < kApiVersion_count);
    return (kApiVersionSeriesMembership[apiVersion] & m_activeApiVersionSeriesMask) != 0;
}

REALLY_INLINE ApiVersion AvmCore::getValidApiVersion(ApiVersion apiVersion)
{
    AvmAssert((int)apiVersion >= 0 && (int)apiVersion < kApiVersion_count);
    return kApiVersionSeriesTransfer[apiVersion][m_activeApiVersionSeries];
}

REALLY_INLINE Namespacep AvmCore::getPublicNamespace(PoolObject* pool)
{
    AvmAssert(pool != NULL);
    return publicNamespaces->nsAt(pool->getApiVersion());
}

REALLY_INLINE Namespacep AvmCore::getPublicNamespace(ApiVersion apiVersion)
{
    AvmAssert(isValidApiVersion(apiVersion));
    return publicNamespaces->nsAt(apiVersion);
}

REALLY_INLINE Namespacep AvmCore::getAnyPublicNamespace()
{
    // This may seem counterintuitive, but the idea is that
    // we want a public namespace that will match "any" "public"
    // namespace; thus we choose INTERNAL since it always matches everything.
    return publicNamespaces->nsAt(kApiVersion_VM_INTERNAL);
}

// NOTE, the code in enter/exit is replicated in CodegenLIR.cpp;
// if you make changes here, you may need to make changes there as well.
REALLY_INLINE void MethodFrame::enter(AvmCore* core, MethodEnv* e)
{
    AvmAssert(core->codeContextThread == VMPI_currentThread());
    AvmAssert(!(uintptr_t(e) & FLAGS_MASK));
    // implicitly leave IS_EXPLICIT_CODECONTEXT and DXNS_NOT_NULL clear
    this->envOrCodeContext = uintptr_t(e);
    this->next = core->currentMethodFrame;
    core->currentMethodFrame = this;
    #ifdef _DEBUG
    this->dxns = (Namespace*)(uintptr_t)0xdeadbeef;
    #endif
}

REALLY_INLINE void MethodFrame::enter(AvmCore* core, CodeContext* cc)
{
    AvmAssert(core->codeContextThread == VMPI_currentThread());
    AvmAssert(!(uintptr_t(cc) & FLAGS_MASK));
    // set IS_EXPLICIT_CODECONTEXT and leave DXNS_NOT_NULL clear
    this->envOrCodeContext = uintptr_t(cc) | IS_EXPLICIT_CODECONTEXT;
    this->next = core->currentMethodFrame;
    core->currentMethodFrame = this;
    #ifdef _DEBUG
    this->dxns = (Namespace*)(uintptr_t)0xdeadbeef;
    #endif
}

REALLY_INLINE void MethodFrame::exit(AvmCore* core)
{
    AvmAssert(core->codeContextThread == VMPI_currentThread());
    AvmAssert(core->currentMethodFrame == this);
    core->currentMethodFrame = this->next;
}

REALLY_INLINE CodeContext* MethodFrame::cc() const
{
    return (envOrCodeContext & IS_EXPLICIT_CODECONTEXT)
        ? (CodeContext*)(envOrCodeContext & ~FLAGS_MASK)
        : NULL;
}

REALLY_INLINE MethodEnv* MethodFrame::env() const
{
    return (envOrCodeContext & IS_EXPLICIT_CODECONTEXT)
        ? NULL
        : (MethodEnv*)(envOrCodeContext & ~FLAGS_MASK);
}

REALLY_INLINE void MethodFrame::setDxns(Namespace* ns)
{
    AvmAssert(ns != NULL);
    envOrCodeContext |= DXNS_NOT_NULL;
    dxns = ns;
}

template<class CLASSMANIFEST>
CLASSMANIFEST* AvmCore::prepareBuiltinActionPool(PoolObject* pool,
                                      Toplevel* toplevel,
                                      CodeContext* codeContext)
{
    AvmAssert(pool != NULL);
    AvmAssert(toplevel != NULL);

    // get the main entry point and its global traits
    if (pool->scriptCount() == 0)
    {
        toplevel->throwVerifyError(kMissingEntryPointError);
    }

    AvmAssert(codeContext != NULL);
    AvmAssert(codeContext->domainEnv() != NULL);
    AvmAssert(codeContext->domainEnv()->domain() == pool->domain);

    AbcEnv* abcEnv = AbcEnv::create(GetGC(), pool, codeContext);
    ScriptEnv* entryPoint = initAllScripts(toplevel, abcEnv);
    CLASSMANIFEST* manifest = CLASSMANIFEST::create(entryPoint);
    return manifest;
}

REALLY_INLINE void AvmCore::assignPoolId(PoolObject* pool)
{
    // each pool is assigned an id that is based on a running total of MethodInfo's
    // this is so that MethodInfo.unique_method_id() can return a AvmCore-wide unique
    // identifier for any given method.
    pool->setUniqueId(this->currentMethodInfoCount);
    uint32_t num = pool->methodCount();
    AvmAssert(num > 0);
    currentMethodInfoCount += num;
}

} // namespace avmplus

#endif /* __avmplus_AvmCore_inlines__ */
