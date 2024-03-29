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
    // ---------------------------
    
    FunctionClass::FunctionClass(VTable* cvtable)
        : ClassClosure(cvtable)
    {
        Toplevel* toplevel = this->toplevel();
        toplevel->_functionClass = this;

        AvmAssert(traits()->getSizeOfInstance() == sizeof(FunctionClass));

        setPrototypePtr(call_createEmptyFunction());
        prototypePtr()->setDelegate(toplevel->objectClass->prototypePtr());

        //
        // now that Object, Class, and Function are initialized, we
        // can set up Object.prototype.  other classes will init normally.
        //

        // init Object prototype
        toplevel->objectClass->initPrototype();
    }

    // Function called as constructor ... not supported from user code
    // this = argv[0] (ignored)
    // arg1 = argv[1]
    // argN = argv[argc]
    Atom FunctionClass::construct(int argc, Atom* /*argv*/)
    {
        // ISSUE do we need an exception here?
        // cn: if argc is 0, this is harmless and we have to return an anonymous
        // function that itself if its > 0, then we can't support it

        /*
        from ECMA 327 5.1 Runtime Compilation
        An implementation that does not support global eval() or calling Function as a function or constructor
        SHALL throw an EvalError exception whenever global eval() (ES3 section 15.1.2.1), Function(p1,
        p2, ..., pn, body) (ES3 section 15.3.1.1), or new Function(p1, p2, ..., pn, body) (ES3 section 15.3.2.1) is
        called.
        */

        if (argc != 0)
        {
            toplevel()->evalErrorClass()->throwError(kFunctionConstructorError);
        }

        return call_createEmptyFunction()->atom();
    }

    int32_t FunctionObject::get_length()
    {
        return m_callEnv->method->getMethodSignature()->param_count();
    }

    FunctionObject::FunctionObject(VTable* cvtable, MethodEnv* call)
        : ClassClosure(cvtable, ClassClosure::createScriptObjectProc)
        , m_callEnv(call)
    {
        AvmAssert(m_callEnv != NULL);
        // Since FunctionObject is (pseudo)final, we shouldn't need to calculate this every time,
        // but let's reality-check here just in case.
        AvmAssert(calcCreateInstanceProc(cvtable) == ClassClosure::createScriptObjectProc);
    }

    /**
     * Function.prototype.call()
     */
    Atom FunctionObject::AS3_call(Atom thisArg, Atom *argv, int argc)
    {
        thisArg = get_coerced_receiver(thisArg);
        return core()->exec->call(m_callEnv, thisArg, argc, argv);
    }

    /**
     * Function.prototype.apply()
     */
    Atom FunctionObject::AS3_apply(Atom thisArg, Atom argArray)
    {
        thisArg = get_coerced_receiver(thisArg);

        // when argArray == undefined or null, same as not being there at all
        // see Function/e15_3_4_3_1.as

        if (!AvmCore::isNullOrUndefined(argArray))
        {
            AvmCore* core = this->core();

            // FIXME: why not declare argArray as Array in Function.as?
            if (!AvmCore::istype(argArray, ARRAY_TYPE))
                toplevel()->throwTypeError(kApplyError);

            return core->exec->apply(m_callEnv, thisArg, (ArrayObject*)AvmCore::atomToScriptObject(argArray));
        }
        else
        {
            return m_callEnv->coerceEnter(thisArg);
        }
    }

    // this = argv[0] (ignored)
    // arg1 = argv[1]
    // argN = argv[argc]
    Atom FunctionObject::construct(int argc, Atom* argv)
    {
        AvmAssert(argv != NULL); // need at least one arg spot passed in

        ScriptObject* obj = newInstance();

        // this is a function
        argv[0] = obj->atom(); // new object is receiver
        Atom result = m_callEnv->coerceEnter(argc, argv);

        // for E3 13.2.2 compliance, check result and return it if (Type(result) is Object)

        /* ISSUE does this apply to class constructors too?

        answer: no.  from E4: A constructor may invoke a return statement as long as that
        statement does not supply a value; a constructor cannot return a value. The newly
        created object is returned automatically. A constructors return type must be omitted.
        A constructor always returns a new instance. */

        return AvmCore::isNull(result) || AvmCore::isObject(result) ? result : obj->atom();
    }

#if defined(DEBUGGER) || defined(VMCFG_AOT)
    /*virtual*/ MethodEnv* FunctionObject::getCallMethodEnv()
    {
        return m_callEnv;
    }
#endif

    /*virtual*/ Atom FunctionObject::call(int argc, Atom* argv)
    {
        argv[0] = get_coerced_receiver(argv[0]);
        return m_callEnv->coerceEnter(argc, argv);
    }

    /*virtual*/ CodeContext* FunctionObject::getFunctionCodeContext() const
    {
        return m_callEnv->scope()->abcEnv()->codeContext();
    }

    /*virtual*/ Atom FunctionObject::get_coerced_receiver(Atom a) const
    {
        if (AvmCore::isNullOrUndefined(a))
        {
            // use callee's global object as this.
            // see E3 15.3.4.4
            a = m_callEnv->scope()->getScope(0);
        }
        MethodSignaturep ms = m_callEnv->method->getMethodSignature();
        return toplevel()->coerce(a, ms->paramTraits(0));
    }

    /*virtual*/ Stringp FunctionObject::implToString() const
    {
        AvmCore* core = this->core();
        Stringp s = core->concatStrings(core->newConstantStringLatin1("[object Function-"), core->intToString(m_callEnv->method->method_id()));
        return core->concatStrings(s, core->newConstantStringLatin1("]"));
    }

}
