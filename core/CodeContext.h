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

#ifndef __avmplus_CodeContext__
#define __avmplus_CodeContext__

namespace avmplus
{
    // CodeContext is used to track which security context we are in.
    // When an AS3 method is called, the AS3 method will ensure that core->codeContext() will return its context.
    // Note that CodeContext should not be instantiated directly (except in certain situations
    // by AvmCore); client code should create a concrete subclass.
    class CodeContext : public MMgc::GCObject
    {
        friend class AvmCore;

    protected:
        explicit CodeContext(DomainEnv* env, const BugCompatibility* bugCompatibility)
            : m_domainEnv(env)
            , m_bugCompatibility(bugCompatibility)
        {
            AvmAssert(env != NULL);
        }

    public:
        REALLY_INLINE DomainEnv* domainEnv() const { return m_domainEnv; }
        REALLY_INLINE const BugCompatibility* bugCompatibility() const { return m_bugCompatibility; }

    private:
        GCMember<DomainEnv>                 m_domainEnv;
        GCMember<const BugCompatibility>    m_bugCompatibility;
    };

    class EnterCodeContext
    {
    public:
        EnterCodeContext() : m_core(NULL) {}  // support dynamic entry

        explicit EnterCodeContext(AvmCore* core, CodeContext* new_cc) : m_core(NULL)
        {
            enter(core,new_cc);
        }
        void enter(AvmCore* core, CodeContext* new_cc)
        {
            AvmAssert(core != NULL);
            AvmAssert(m_core == NULL); // Don't permit multiple entry.
            m_core = core;
            m_frame.enter(core, new_cc);
            // fix for https://bugzilla.mozilla.org/show_bug.cgi?id=537980
            // ensure that dxns has a suitable default value for artifical MethodFrames
            m_frame.setDxns(core->publicNamespace);
        }
        ~EnterCodeContext()
        {
            if (m_core) {
                m_frame.exit(m_core);
            }
        }
    private:

        AvmCore* m_core;
        MethodFrame m_frame;
    };


}

#endif /* __avmplus_CodeContext__ */
