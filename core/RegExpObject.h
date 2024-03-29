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

#ifndef __avmplus_RegExpObject__
#define __avmplus_RegExpObject__

namespace avmplus
{
    class CompiledRegExp : public MMgc::RCObject
    {
    public:
        CompiledRegExp(void* regex) : regex(regex) {}
        ~CompiledRegExp();

        void * regex; // The compiled regular expression
    };

    /**
     * The RegExpObject class is the C++ implementation of instances
     * of the "RegExp" class, as defined in the ECMAScript standard.
     */
    class GC_AS3_EXACT(RegExpObject, ScriptObject)
    {
    protected:
        RegExpObject(VTable* ivtable, ScriptObject* delegate);
        RegExpObject(VTable* ivtable, ScriptObject* delegate, Stringp pattern, Stringp options);

    private:
        /* Copy constructor */
        RegExpObject(RegExpObject *toCopy);

    public:
        REALLY_INLINE static RegExpObject* create(MMgc::GC* gc, RegExpClass* cls, RegExpObject *toCopy)
        {
            return new (gc, MMgc::kExact, cls->ivtable()->getExtraSize()) RegExpObject(toCopy);
        }
        
        REALLY_INLINE static RegExpObject* create(MMgc::GC* gc, VTable* ivtable, ScriptObject* delegate)
        {
            return new (gc, MMgc::kExact, ivtable->getExtraSize()) RegExpObject(ivtable, delegate);
        }

        REALLY_INLINE static RegExpObject* create(MMgc::GC* gc, VTable* ivtable, ScriptObject* delegate, Stringp pattern, Stringp options)
        {
            return new (gc, MMgc::kExact, ivtable->getExtraSize()) RegExpObject(ivtable, delegate, pattern, options);
        }

#ifdef DRC_TRIVIAL_DESTRUCTOR
        ~RegExpObject();
#endif

        // call is implicit exec
        // atom[0] = this
        // atom[1] = arg1
        // atom[argc] = argN
        Atom call(int argc, Atom* argv);

        int search(Stringp subject);
        ArrayObject* split(Stringp subject, uint32_t limit);
        ArrayObject* match(Stringp subject);

        Atom AS3_exec(Stringp subject);
        Atom replace(Stringp subject, Stringp replacement);
        Atom replace(Stringp subject, ScriptObject *replaceFunction);

        Stringp get_source() const { return m_source; }
        bool get_global() const { return m_global; }
        int get_lastIndex() const { return m_lastIndex; }
        void set_lastIndex(int newIndex) { m_lastIndex = newIndex; }
        inline bool hasOption(int mask) { return (m_optionFlags&mask) != 0; }
        bool get_ignoreCase();
        bool get_multiline();
        bool get_dotall();
        bool get_extended();

    private:
        void completeInitialization(String* options);

        Atom stringFromUTF8(const char *buffer, int len);

        ArrayObject* _exec(Stringp subject, StIndexableUTF8String& utf8Subject);

        ArrayObject* _exec(Stringp subject,
                          StIndexableUTF8String& utf8Subject,
                          int startIndex,
                          int& matchIndex,
                          int& matchLen);

        void fixReplaceLastIndex(const char *src,
                                 int subjectLength,
                                 int matchLen,
                                 int& newLastIndex,
                                 StringBuffer& resultBuffer);

        int numBytesInUtf8Character(const uint8_t *in);

    // ------------------------ DATA SECTION BEGIN
        GC_DATA_BEGIN(RegExpObject)

    private:
        GCMember<String>         GC_POINTER(m_source);
        GCMember<CompiledRegExp> GC_POINTER(m_pcreInst);
        int                      m_lastIndex;
        int                      m_optionFlags;
        bool                     m_global;
        bool                     m_hasNamedGroups;

        GC_DATA_END(RegExpObject)

        DECLARE_SLOTS_RegExpObject;
    // ------------------------ DATA SECTION END
    };
}

#endif /* __avmplus_RegExpObject__ */
