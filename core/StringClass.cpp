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


#include "avmplus.h"
#include "BuiltinNatives.h"

namespace avmplus
{
    StringClass::StringClass(VTable* cvtable)
        : ClassClosure(cvtable)
    {
        toplevel()->_stringClass = this;
        createVanillaPrototype();

        // Some sanity tests for string/wchar* comparison routines
#if 0 && defined(_DEBUG)
        Stringp a = core()->newConstantStringLatin1("a");
        Stringp b = core()->newConstantStringLatin1("b");
        Stringp c = core()->newConstantStringLatin1("c");

        AvmAssert( (*a == *a));
        AvmAssert(!(*a != *a));
        AvmAssert( (*a >= *a));
        AvmAssert( (*a <= *a));
        AvmAssert(!(*a > *a));
        AvmAssert(!(*a < *a));

        AvmAssert(!(*a == *b));
        AvmAssert( (*a != *b));
        AvmAssert(!(*a >= *b));
        AvmAssert( (*a <= *b));
        AvmAssert(!(*a > *b));
        AvmAssert( (*a < *b));

        AvmAssert(!(*c == *b));
        AvmAssert( (*c != *b));
        AvmAssert( (*c >= *b));
        AvmAssert(!(*c <= *b));
        AvmAssert( (*c > *b));
        AvmAssert(!(*c < *b));

        wchar d[2];
        d[0] = 'a';
        d[1] = 0;

        AvmAssert( (d == *a));
        AvmAssert(!(d != *a));
        AvmAssert( (d >= *a));
        AvmAssert( (d <= *a));
        AvmAssert(!(d > *a));
        AvmAssert(!(d < *a));

        AvmAssert( (*a == d));
        AvmAssert(!(*a != d));
        AvmAssert( (*a >= d));
        AvmAssert( (*a <= d));
        AvmAssert(!(*a > d));
        AvmAssert(!(*a < d));

        AvmAssert(!(d == *b));
        AvmAssert( (d != *b));
        AvmAssert(!(d >= *b));
        AvmAssert( (d <= *b));
        AvmAssert(!(d > *b));
        AvmAssert( (d < *b));

        wchar e[2];
        e[0] = 'b';
        e[1] = 0;

        AvmAssert(!(*c == e));
        AvmAssert( (*c != e));
        AvmAssert( (*c >= e));
        AvmAssert(!(*c <= e));
        AvmAssert( (*c > e));
        AvmAssert(!(*c < e));
#endif
    }

    // this = argv[0] (ignored)
    // arg1 = argv[1]
    // argN = argv[argc]
    Atom StringClass::construct(int argc, Atom* argv)
    {
        if (argc == 0) {
            return core()->kEmptyString->atom();
        } else {
            return core()->string(argv[1])->atom();
        }
        // TODO ArgumentError if argc > 1
    }

    // this = argv[0] (ignored)
    // arg1 = argv[1]
    // argN = argv[argc]
    Atom StringClass::call(int argc, Atom* argv)
    {
        return construct(argc, argv);
    }

    Stringp StringClass::AS3_fromCharCode(Atom *argv, int argc)
    {
        AvmCore* core = this->core();
        Stringp out = core->kEmptyString;
        for (int i=0; i<argc; i++)
        {
            wchar c = wchar(AvmCore::integer(argv[i]));
            if (c <= 0xff)
            {
                // append16 will always append as k16, forcing the string
                // to be widened, as String::_append doesn't understand kAuto.
                // That can/should probably be smarted, but for now,
                // improve the smarts here:
                uint8_t c8 = uint8_t(c);
                out = out->appendLatin1((char*)&c8, 1);
            }
            else
            {
                // note: this code is allowed to construct a string
                // containing illegal UTF16 sequences!
                // (eg, String.fromCharCode(0xD800).charCodeAt(0) -> 0xD800).
                out = out->append16(&c, 1);
            }
        }
        return out;
    }

    ArrayObject* StringClass::_match(Stringp in, Atom regexpAtom)
    {
        AvmCore* core = this->core();

        if (!AvmCore::istype(regexpAtom, core->traits.regexp_itraits))
        {
            // ECMA-262 15.5.4.10
            // If the argument is not a RegExp, invoke RegExp(exp)
            regexpAtom = core->newRegExp(toplevel()->regexpClass(),
                                         core->string(regexpAtom),
                                         core->kEmptyString)->atom();
        }

        RegExpObject *reObj = (RegExpObject*) AvmCore::atomToScriptObject(regexpAtom);
        return reObj->match(in);
    }

    Stringp StringClass::_replace(Stringp subject, Atom pattern, Atom replacementAtom)
    {
        AvmCore* core = this->core();

        ScriptObject *replaceFunction = NULL;
        Stringp replacement = NULL;
        if (AvmCore::istype(replacementAtom, core->traits.function_itraits)) {
            replaceFunction = AvmCore::atomToScriptObject(replacementAtom);
        } else {
            replacement = core->string(replacementAtom);
        }

        if (AvmCore::istype(pattern, core->traits.regexp_itraits)) {
            // RegExp mode
            RegExpObject *reObj = (RegExpObject*) core->atomToScriptObject(pattern);
            if (replaceFunction) {
                return core->string(reObj->replace(subject, replaceFunction));
            } else {
                return core->string(reObj->replace(subject, replacement));
            }

        } else {
            // String replace mode
            Stringp searchString = core->string(pattern);

            int index = subject->indexOf(searchString);
            if (index == -1) {
                // Search string not found; return input unchanged.
                return subject;
            }

            if (replaceFunction) {
                // Invoke the replacement function to figure out the
                // replacement string
                Atom argv[4] = { undefinedAtom,
                                 searchString->atom(),
                                 core->uintToAtom(index),
                                 subject->atom() };
                replacement = core->string(toplevel()->op_call(replaceFunction->atom(),
                                                               3, argv));
            }

            Stringp out = subject->substring(0, index);
            out = String::concatStrings(out, replacement);
            out = String::concatStrings(out, subject->substring(index + searchString->length(), subject->length()));
            return out;
        }
    }

    int StringClass::_search(Stringp in, Atom regexpAtom)
    {
        AvmCore* core = this->core();

        if (!AvmCore::istype(regexpAtom, core->traits.regexp_itraits)) {
            // ECMA-262 15.5.4.10
            // If the argument is not a RegExp, invoke RegExp(exp)
            regexpAtom = core->newRegExp(toplevel()->regexpClass(),
                                                core->string(regexpAtom),
                                                core->kEmptyString)->atom();
        }

        RegExpObject *reObj = (RegExpObject*) AvmCore::atomToScriptObject(regexpAtom);
        return reObj->search(in);
    }

    ArrayObject* StringClass::_split(Stringp in, Atom delimAtom, uint32_t limit)
    {
        AvmCore* core = this->core();

        if (limit == 0)
            return toplevel()->arrayClass()->newArray();

        if (in->length() == 0)
        {
            ArrayObject* out = toplevel()->arrayClass()->newArray();
            out->setUintProperty(0,in->atom());
            return out;
        }

        // handle RegExp case
        if (AvmCore::istype(delimAtom, core->traits.regexp_itraits))
        {
            RegExpObject *reObj = (RegExpObject*) AvmCore::atomToScriptObject(delimAtom);
            return reObj->split(in, limit);
        }

        ArrayObject *out = toplevel()->arrayClass()->newArray();
        Stringp delim = core->string(delimAtom);

        int ilen = in->length();
        int dlen = delim->length();
        int count = 0;

        if (dlen <= 0)
        {
            // delim is empty string, split on each char
            for (int i = 0; i < ilen && (unsigned)i < limit; i++)
            {
                Stringp sub = in->substr(i, 1);
                out->setUintProperty(count++, sub->atom());
            }
            return out;
        }

        int32_t start = 0;

        while (start <= in->length())
        {
            if ((limit != 0xFFFFFFFFUL) && (count >= (int) limit))
                break;
            int32_t bgn = in->indexOf(delim, start);
            if (bgn < 0)
                // not found, use the string remainder
                bgn = in->length();
            Stringp sub = in->substring(start, bgn);
            out->setUintProperty(count++, sub->atom());
            start = bgn + delim->length();
        }
        return out;
    }
}
