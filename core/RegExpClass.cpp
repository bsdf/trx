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

#include "pcre.h"

namespace avmplus
{
    RegExpClass::RegExpClass(VTable* cvtable)
        : ClassClosure(cvtable)
    {
        AvmAssert(traits()->getSizeOfInstance() == sizeof(RegExpClass));

        AvmCore* core = this->core();
        ScriptObject* object_prototype = toplevel()->objectClass->prototypePtr();
        String* pattern = core->newConstantStringLatin1("(?:)");
        setPrototypePtr(RegExpObject::create(core->GetGC(), cvtable->ivtable, object_prototype, pattern, core->kEmptyString));
    }

    // this = argv[0] (ignored)
    // arg1 = argv[1]
    // argN = argv[argc]
    Atom RegExpClass::call(int argc, Atom* argv)
    {
        // ECMA-262 15.10.4.1: If pattern is RegExp and flags is undefined,
        // return pattern unchanged.
        if (argc > 0) {
            Atom flagsAtom = (argc>1) ? argv[2] : undefinedAtom;
            if (AvmCore::istype(argv[1], traits()->itraits) && flagsAtom == undefinedAtom) {
                return argv[1];
            }
        }

        // Otherwise, call the RegExp constructor.
        return construct(argc, argv);
    }

    // this = argv[0] (ignored)
    // arg1 = argv[1]
    // argN = argv[argc]
    Atom RegExpClass::construct(int argc, Atom* argv)
    {
        AvmCore* core = this->core();
        Stringp pattern;

        Atom patternAtom = (argc>0) ? argv[1] : undefinedAtom;
        Atom optionsAtom = (argc>1) ? argv[2] : undefinedAtom;

        if (AvmCore::istype(patternAtom, traits()->itraits)) {
            // Pattern is a RegExp object
            if (optionsAtom != undefinedAtom) {
                // ECMA 15.10.4.1 says to throw an error if flags specified
                toplevel()->throwTypeError(kRegExpFlagsArgumentError);
            }
            // Return a clone of the RegExp object
            RegExpObject* regExpObject = (RegExpObject*)AvmCore::atomToScriptObject(patternAtom);
            return RegExpObject::create(core->GetGC(), this, regExpObject)->atom();
        } else {
            if (patternAtom != undefinedAtom) {
                pattern = core->string(argv[1]);
            } else {
                // cn:  disable this, breaking ecma3 tests.   was: todo look into this. it's what SpiderMonkey does.
                pattern = core->kEmptyString; //core->newConstantStringLatin1("(?:)");
            }
        }

        Stringp options = NULL;
        if (optionsAtom != undefinedAtom) {
            options = core->string(optionsAtom);
        }

        RegExpObject* inst = RegExpObject::create(core->GetGC(), this->ivtable(), this->prototypePtr(), pattern, options);
        return inst->atom();
    }
}
