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

#ifndef __avmplus_AvmPlusScriptableObject__
#define __avmplus_AvmPlusScriptableObject__


namespace avmplus
{

#ifdef DEBUGGER
    // This really ought to be defined in Sampler.h, but is here to simplify header-file inclusion
    // order issues.
    typedef struct SamplerObjectType_* SamplerObjectType;
#endif

    class AvmPlusScriptableObject : public MMgc::RCObject
    {
    public:
        // used by WeakValueHashtable to correctly atom'ize a pointer to one of these
        virtual Atom toAtom() const = 0;

#ifdef DEBUGGER
        AvmPlusScriptableObject(SamplerObjectType sot);

        /**
         * Returns the number of bytes of memory taken by this object, that are not
         * also shared by any other objects.  For example, a dependent String would
         * include sizeof(String) but not the size of the buffer of the master string;
         * but a dynamic String would include both sizeof(String) and the size of
         * the buffer.
         *
         * Used by the profiler to tell the user the shallow size of the object.
         */
        virtual uint64_t bytesUsed() const = 0;
#endif

        inline AvmCore* core() const
        {
            return (AvmCore*)MMgc::GC::GetGC(this)->core();
        }
    };
}

#endif //__avmplus_AvmPlusScriptableObject__
