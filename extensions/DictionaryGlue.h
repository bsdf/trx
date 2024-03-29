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


#ifndef DICTIONARYGLUE_INCLUDED
#define DICTIONARYGLUE_INCLUDED

namespace avmplus
{
    class GC_AS3_EXACT_WITH_HOOK(DictionaryObject, ScriptObject)
    {
        friend class DictionaryClass;
    protected:
        DictionaryObject(VTable *vtable, ScriptObject *delegate);
        
    public:

        void init(bool weakKeys);

        // multiname and Stringp forms fall through to ScriptObject
        virtual Atom getAtomProperty(Atom name) const;
        virtual bool hasAtomProperty(Atom name) const;
        virtual bool deleteAtomProperty(Atom name);
        virtual void setAtomProperty(Atom name, Atom value);

        virtual Atom nextName(int index);
        virtual Atom nextValue(int index);
        virtual int nextNameIndex(int index);

        bool isUsingWeakKeys() const { return getHeapHashtable()->weakKeys(); }

    private:

        // Deliberately declared out-of-line; inline declaration inexplicably
        // makes certain permutations of XCode 3.2.x for iOS segfault.
        // Voodoo coding FTW!
        void gcTraceHook_DictionaryObject(MMgc::GC *gc);
            
        inline HeapHashtable* getHeapHashtable() const
        {
            // uintptr_t (rather than char*) to avoid "increases required alignment" warning
            return *(HeapHashtable**)(uintptr_t(this) + vtable->traits->getHashtableOffset());
        }

        Atom FASTCALL getKeyFromObject(Atom object) const;

    // ------------------------ DATA SECTION BEGIN
        GC_NO_DATA(DictionaryObject)

    private:
        DECLARE_SLOTS_DictionaryObject;
    // ------------------------ DATA SECTION END
    };

    class GC_AS3_EXACT(DictionaryClass, ClassClosure)
    {
    private:
        DictionaryClass(VTable *vtable);
    public:
    // ------------------------ DATA SECTION BEGIN
        GC_NO_DATA(DictionaryClass)

    private:
        DECLARE_SLOTS_DictionaryClass;
    // ------------------------ DATA SECTION END
    };
}

#endif /* DICTIONARYGLUE_INCLUDED */
