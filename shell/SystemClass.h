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

#ifndef __avmshell_SystemClass__
#define __avmshell_SystemClass__


namespace avmshell
{
    class GC_AS3_EXACT(AbstractBaseClass, avmplus::ClassClosure)
    {
    public:
        AbstractBaseClass(avmplus::VTable* cvtable) : ClassClosure(cvtable) {}
        
        GC_NO_DATA(AbstractBaseClass)

        DECLARE_SLOTS_AbstractBaseClass;
    };

    class GC_AS3_EXACT(AbstractBaseObject, avmplus::ScriptObject)
    {
    public:
        AbstractBaseObject(avmplus::VTable* vtable, avmplus::ScriptObject* prototype) : avmplus::ScriptObject(vtable, prototype) {}

        GC_NO_DATA(AbstractBaseObject)

        DECLARE_SLOTS_AbstractBaseObject;
    };

    class GC_AS3_EXACT(NativeSubclassOfAbstractBaseClass, avmplus::ClassClosure)
    {
    public:
        NativeSubclassOfAbstractBaseClass(avmplus::VTable* cvtable) : avmplus::ClassClosure(cvtable) {}

        GC_NO_DATA(NativeSubclassOfAbstractBaseClass)

        DECLARE_SLOTS_NativeSubclassOfAbstractBaseClass;
    };

    class GC_AS3_EXACT(NativeSubclassOfAbstractBaseObject, AbstractBaseObject)
    {
    public:
        NativeSubclassOfAbstractBaseObject(avmplus::VTable* vtable, avmplus::ScriptObject* prototype) : AbstractBaseObject(vtable, prototype) {}

        GC_NO_DATA(NativeSubclassOfAbstractBaseObject)

        DECLARE_SLOTS_NativeSubclassOfAbstractBaseObject;
    };

    class GC_AS3_EXACT(RestrictedBaseClass, avmplus::ClassClosure)
    {
    public:
        RestrictedBaseClass(avmplus::VTable* cvtable) : avmplus::ClassClosure(cvtable) {}

        GC_NO_DATA(RestrictedBaseClass)

        DECLARE_SLOTS_RestrictedBaseClass;
    };

    class GC_AS3_EXACT(RestrictedBaseObject, avmplus::ScriptObject)
    {
    public:
        RestrictedBaseObject(avmplus::VTable* vtable, avmplus::ScriptObject* prototype) : avmplus::ScriptObject(vtable, prototype) {}

        GC_NO_DATA(RestrictedBaseObject)

        DECLARE_SLOTS_RestrictedBaseObject;
    };

    class GC_AS3_EXACT(NativeSubclassOfRestrictedBaseClass, avmplus::ClassClosure)
    {
    public:
        NativeSubclassOfRestrictedBaseClass(avmplus::VTable* cvtable) : avmplus::ClassClosure(cvtable) {}

        GC_NO_DATA(NativeSubclassOfRestrictedBaseClass)

        DECLARE_SLOTS_NativeSubclassOfRestrictedBaseClass;
    };

    class GC_AS3_EXACT(NativeSubclassOfRestrictedBaseObject, RestrictedBaseObject)
    {
    public:
        NativeSubclassOfRestrictedBaseObject(avmplus::VTable* vtable, avmplus::ScriptObject* prototype) : RestrictedBaseObject(vtable, prototype) {}

        GC_NO_DATA(NativeSubclassOfRestrictedBaseObject)

        DECLARE_SLOTS_NativeSubclassOfRestrictedBaseObject;
    };

    class GC_AS3_EXACT(AbstractRestrictedBaseClass, avmplus::ClassClosure)
    {
    public:
        AbstractRestrictedBaseClass(avmplus::VTable* cvtable) : avmplus::ClassClosure(cvtable) {}

        GC_NO_DATA(AbstractRestrictedBaseClass)

        DECLARE_SLOTS_AbstractRestrictedBaseClass;
    };

    class GC_AS3_EXACT(AbstractRestrictedBaseObject, avmplus::ScriptObject)
    {
    public:
        AbstractRestrictedBaseObject(avmplus::VTable* vtable, avmplus::ScriptObject* prototype) : avmplus::ScriptObject(vtable, prototype) {}

        GC_NO_DATA(AbstractRestrictedBaseObject)

        DECLARE_SLOTS_AbstractRestrictedBaseObject;
    };

    class GC_AS3_EXACT(NativeSubclassOfAbstractRestrictedBaseClass, avmplus::ClassClosure)
    {
    public:
        NativeSubclassOfAbstractRestrictedBaseClass(avmplus::VTable* cvtable) : avmplus::ClassClosure(cvtable) {}

        GC_NO_DATA(NativeSubclassOfAbstractRestrictedBaseClass)

        DECLARE_SLOTS_NativeSubclassOfAbstractRestrictedBaseClass;
    };

    class GC_AS3_EXACT(NativeSubclassOfAbstractRestrictedBaseObject, AbstractRestrictedBaseObject)
    {
    public:
        NativeSubclassOfAbstractRestrictedBaseObject(avmplus::VTable* vtable, avmplus::ScriptObject* prototype) : AbstractRestrictedBaseObject(vtable, prototype) {}

        GC_NO_DATA(NativeSubclassOfAbstractRestrictedBaseObject)

        DECLARE_SLOTS_NativeSubclassOfAbstractRestrictedBaseObject;
    };

    // this class exists solely to test native classes that use MI.
    class MIClass : public avmplus::ClassClosure
    {
    public:
        MIClass(avmplus::VTable* cvtable) : avmplus::ClassClosure(cvtable) {}
        ~MIClass() {}

        DECLARE_SLOTS_MIClass;
    };

    // this class exists solely to test native classes that use MI.
    class MixinClassThatDoesNotDescendFromScriptObject
    {
    public:
        const double factor;
        MixinClassThatDoesNotDescendFromScriptObject(double f) : factor(f) {}
        // evil, wrong version that we DO NOT WANT
        double plus(double v) { return v * factor; }
    };

    // this class exists solely to test native classes that use MI.
    class MIObjectImpl : public avmplus::ScriptObject
    {
    public:
        const double amount;
        MIObjectImpl(avmplus::VTable* vtable, avmplus::ScriptObject* prototype, double a) : avmplus::ScriptObject(vtable, prototype), amount(a) {}
        double plus(double v) { return v + amount; }
    };

    class GC_AS3_EXACT(CheckBaseClass, avmplus::ClassClosure)
    {
    public:
        CheckBaseClass(avmplus::VTable* cvtable) : avmplus::ClassClosure(cvtable) {}

        GC_NO_DATA(CheckBaseClass)

        DECLARE_SLOTS_CheckBaseClass;
    };

    class GC_AS3_EXACT(CheckBaseObject, avmplus::ScriptObject)
    {
    public:
        CheckBaseObject(avmplus::VTable* vtable, avmplus::ScriptObject* prototype) : avmplus::ScriptObject(vtable, prototype) {}

        GC_NO_DATA(CheckBaseObject)

        DECLARE_SLOTS_CheckBaseObject;
    };

    class GC_AS3_EXACT(NativeBaseClass, avmplus::ClassClosure)
    {
    public:
        NativeBaseClass(avmplus::VTable* cvtable) : avmplus::ClassClosure(cvtable) {}

        GC_NO_DATA(NativeBaseClass)

        DECLARE_SLOTS_NativeBaseClass;
    };

    class GC_AS3_EXACT(NativeBaseObject, avmplus::ScriptObject)
    {
    public:
        NativeBaseObject(avmplus::VTable* vtable, avmplus::ScriptObject* prototype) : avmplus::ScriptObject(vtable, prototype) {}

        GC_DATA_BEGIN(NativeBaseObject)
        char nine_bytes[9]; // deliberately ending on a misaligned boundary
        GC_DATA_END(NativeBaseObject)
        
        DECLARE_SLOTS_NativeBaseObject;
    };

    // this class exists solely to test native classes that use MI.
    class MIObject : public MIObjectImpl, public MixinClassThatDoesNotDescendFromScriptObject
    {
    public:
        MIObject(avmplus::VTable* vtable, avmplus::ScriptObject* prototype) : MIObjectImpl(vtable, prototype, 1), MixinClassThatDoesNotDescendFromScriptObject(2) {}
        ~MIObject() {}

        DECLARE_SLOTS_MIObject;
    };

    /**
     * A simple class that has some native methods.
     * Included as an example for writers of native methods,
     * and also to provide some useful QA instrumentation.
     */
    class GC_AS3_EXACT(SystemClass, avmplus::ClassClosure)
    {
        uint64_t initialTime;

        SystemClass(avmplus::VTable* cvtable);
    public:
        REALLY_INLINE static SystemClass* create(MMgc::GC* gc, avmplus::VTable* cvtable)
        {
            return new (gc, MMgc::kExact, cvtable->getExtraSize()) SystemClass(cvtable);
        }

        ~SystemClass();

        // set by shell
        static int user_argc;
        static char **user_argv;

        /**
         * Implementation of System.exit
         * AS usage: System.exit(status);
         * Exits the VM with OS exit code specified by  status.
         */
        void exit(int status);

        /**
         * Implementation of System.getAvmplusVersion
         * AS usage: System.getAvmplusVersion();
         * Returns the current version of AVM+ in the form
         * "1.0 d100"
         */
        avmplus::Stringp getAvmplusVersion();
        
        /**
         * Implementation of System.getFeatures
         * AS usage: System.getFeatures();
         * Returns the compiled in features of AVM+
         */
        avmplus::Stringp getFeatures();
        
        /**
         * Implementation of System.getRunmode
         * AS usage: System.getRunmode();
         * Returns the current runmode
         */
        avmplus::Stringp getRunmode();

        /**
         * Implementation of System.exec
         * AS usage: exitCode = System.exec("command");
         * Executes the specified command line and returns
         * the status code
         */
        int exec(avmplus::Stringp command);

        void trace(avmplus::ArrayObject* a);
        void write(avmplus::Stringp s);


        /**
         * @name Debugging Extensions
         */
        /*@{*/
        void debugger();
        bool isDebugger();
        /*@}*/

                /**
         * @name ActionScript Extensions
         * ActionScript extensions to ECMAScript
         */
        /*@{*/
        unsigned getTimer();
        /*@}*/

        avmplus::ArrayObject * getArgv();

        avmplus::Stringp readLine();

        double get_totalMemory();
        double get_freeMemory();
        double get_privateMemory();

        int32_t get_swfVersion();
        int32_t get_apiVersion();

        // Initiate a garbage collection; future versions will not return before completed.
        void forceFullCollection();

        // Queue a garbage collection request.
        void queueCollection();

        // function exists solely to test native-methods with custom namespaces
        void ns_example_nstest() { }

        // function exists solely to test ScriptObject::isGlobalObject
        bool isGlobal(avmplus::Atom o);

        void disposeXML(avmplus::XMLObject *xmlObject);

        void pauseForGCIfCollectionImminent(double imminence);

        GC_NO_DATA(SystemClass)

        DECLARE_SLOTS_SystemClass;
    };
}

#endif /* __avmshell_SystemClass__ */
