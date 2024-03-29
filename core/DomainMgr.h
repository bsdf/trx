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

#ifndef __avmplus_DomainMgr__
#define __avmplus_DomainMgr__

namespace avmplus
{

/**
 *  The Domain manager implementation
 *  is responsible for all aspects of
 *  finding names in a given Domain stack.
 */
class DomainMgr : public MMgc::GCObject
{
public:
    DomainMgr(AvmCore* core);

    // DomainMgr methods:
    void addNamedTraits(PoolObject* pool, Stringp name, Namespacep ns, Traits* traits);
    void addNamedInstanceTraits(PoolObject* pool, Stringp name, Namespacep ns, Traits* itraits);
    Traits* findBuiltinTraitsByName(PoolObject* pool, Stringp name);
    Traits* findTraitsInPoolByNameAndNS(PoolObject* pool, Stringp name, Namespacep ns);
    Traits* findTraitsInPoolByMultiname(PoolObject* pool, const Multiname& multiname);

    void addNamedScript(PoolObject* pool, Stringp name, Namespacep ns, MethodInfo* script);
    MethodInfo* findScriptInPoolByMultiname(PoolObject* pool, const Multiname& multiname);

    void addNamedScriptEnvs(AbcEnv* abcEnv, const GCList<ScriptEnv>& envs);
    ScriptEnv* findScriptEnvInDomainEnvByMultiname(DomainEnv* domainEnv, const Multiname& multiname);
    ScriptEnv* findScriptEnvInAbcEnvByMultiname(AbcEnv* abcEnv, const Multiname& multiname);

#ifdef DEBUGGER
    ScriptEnv* findScriptEnvInAbcEnvByNameOnly(AbcEnv* abcEnv, Stringp name);
#endif

private:
    Traits* findTraitsInDomainByNameAndNSImpl(Domain* domain, Stringp name, Namespacep ns, bool cacheIfFound);
    Traits* findTraitsInPoolByNameAndNSImpl(PoolObject* pool, Stringp name, Namespacep ns, bool cacheIfFound);
    MethodInfo* findScriptInDomainByNameAndNSImpl(Domain* domain, Stringp name, Namespacep ns, bool cacheIfFound);
    MethodInfo* findScriptInDomainByMultinameImpl(Domain* domain, const Multiname& multiname, Namespace*& nsFound);
    MethodInfo* findScriptInPoolByNameAndNSImpl(PoolObject* pool, Stringp name, Namespacep ns, bool cacheIfFound);
    ScriptEnv* findScriptEnvInDomainEnvByMultinameImpl(DomainEnv* domainEnv, const Multiname& multiname);
#ifdef DEBUGGER
    ScriptEnv* findScriptEnvInDomainEnvByNameOnlyImpl(DomainEnv* domainEnv, Stringp name);
#endif

    ScriptEnv* mapScriptToScriptEnv(DomainEnv* domainEnv, MethodInfo* mi);
    MethodInfo* findScriptInPoolByMultinameImpl(PoolObject* pool, const Multiname& multiname);
#ifdef DEBUGGER
    MethodInfo* findScriptInDomainByNameOnlyImpl(Domain* domain, Stringp name, Namespace*& nsFound);
    MethodInfo* findScriptInPoolByNameOnlyImpl(PoolObject* pool, Stringp name);
#endif

private:
    AvmCore* const core;
};


}
#endif // __avmplus_DomainMgr__
