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

namespace avmplus
{

REALLY_INLINE MethodEnvProcHolder::MethodEnvProcHolder()
{}

REALLY_INLINE MethodInfoProcHolder::MethodInfoProcHolder()
{}

REALLY_INLINE bool BaseExecMgr::isInterpreted(MethodEnv* env)
{
    return env->method->isInterpreted() != 0;
}

REALLY_INLINE uintptr_t ImtHolder::getIID(MethodInfo* m)
{
    AvmAssert(m->declaringTraits()->isInterface());
#ifdef VMCFG_VERIFYALL
    // In verifyonly mode, make all IIDs be zero to erase memory layout noise.
    return m->pool()->core->config.verifyonly ? 0 : (uintptr_t)m;
#else
    return (uintptr_t) m;
#endif
}

REALLY_INLINE uint32_t ImtHolder::hashIID(uintptr_t iid)
{
    return iid % IMT_SIZE;
}

REALLY_INLINE uint32_t ImtHolder::hashIID(MethodInfo* m)
{
    return hashIID(getIID(m));
}

#ifdef VMCFG_OSR
REALLY_INLINE bool OSR::countEdge(const MethodEnv* env, MethodInfo* m, MethodSignaturep ms)
{
    // Decrement and return true if zero and OSR is supported.  If OSR
    // is not supported, countEdge will remain 0 without triggering OSR,
    // until the next count which will wrap around to 0xFFFFFFFF.  So we only
    // test isSupported() once every 2^32 invocations.
    return --m->_abc.countdown == 0 && isSupported(env->abcEnv(), m, ms);
}
#endif

} // namespace avmplus
