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

#ifndef __avmplus_Toplevel__
#define __avmplus_Toplevel__


namespace avmplus
{
    class VectorClass;
    class builtinClassManifest;

    /**
     * class Toplevel
     */
    class GC_CPP_EXACT(Toplevel, MMgc::GCFinalizedObject)
    {
        friend class ClassManifestBase;
    protected:
        Toplevel(AbcEnv* abcEnv);
    public:
        static Toplevel* create(MMgc::GC* gc, AbcEnv* abcEnv);

        GCRef<builtinClassManifest> builtinClasses() const;

        void init_mainEntryPoint(ScriptEnv* main, builtinClassManifest* builtins);

        AbcEnv* abcEnv() const;
        DomainEnv* domainEnv() const;
        AvmCore* core() const;
        MMgc::GC* gc() const;
        ScriptObject* global() const;
        Atom atom() const;

        // these are now all just convenience legacy wrappers
        // around builtinClasses()->get_SomeClass()
        GCRef<ArgumentErrorClass> argumentErrorClass() const;
        GCRef<ArrayClass> arrayClass() const;
        GCRef<BooleanClass> booleanClass() const;
        GCRef<ByteArrayClass> byteArrayClass() const;
        GCRef<ClassClass> classClass() const;
        GCRef<EOFErrorClass> eofErrorClass() const;
        GCRef<IOErrorClass> ioErrorClass() const;
        GCRef<MemoryErrorClass> memoryErrorClass() const;
        GCRef<DateClass> dateClass() const;
        GCRef<DoubleVectorClass> doubleVectorClass() const;
        GCRef<ErrorClass> errorClass() const;
        GCRef<EvalErrorClass> evalErrorClass() const;
        GCRef<FunctionClass> functionClass() const;
        GCRef<IntClass> intClass() const;
        GCRef<IntVectorClass> intVectorClass() const;
        GCRef<MethodClosureClass> methodClosureClass() const;
        GCRef<NamespaceClass> namespaceClass() const;
        GCRef<NumberClass> numberClass() const;
        GCRef<ObjectVectorClass> objectVectorClass() const;
        GCRef<QNameClass> qnameClass() const;
        GCRef<RangeErrorClass> rangeErrorClass() const;
        GCRef<ReferenceErrorClass> referenceErrorClass() const;
        GCRef<RegExpClass> regexpClass() const;
        GCRef<SecurityErrorClass> securityErrorClass() const;
        GCRef<StringClass> stringClass() const;
        GCRef<SyntaxErrorClass> syntaxErrorClass() const;
        GCRef<TypeErrorClass> typeErrorClass() const;
        GCRef<UIntClass> uintClass() const;
        GCRef<UIntVectorClass> uintVectorClass() const;
        GCRef<URIErrorClass> uriErrorClass() const;
        GCRef<VectorClass> vectorClass() const;
        GCRef<VerifyErrorClass> verifyErrorClass() const;
        GCRef<XMLClass> xmlClass() const;
        GCRef<XMLListClass> xmlListClass() const;

        void throwVerifyError(int id) const;
        void throwVerifyError(int id, Stringp arg1) const;
        void throwVerifyError(int id, Stringp arg1, Stringp arg2) const;
        void throwVerifyError(int id, Stringp arg1, Stringp arg2, Stringp arg3) const;

        void throwTypeError(int id) const;
        void throwTypeError(int id, Stringp arg1) const;
        void throwTypeError(int id, Stringp arg1, Stringp arg2) const;
        void throwTypeErrorWithName(int id, const char* namestr) const;

        void throwError(int id) const;
        void throwError(int id, Stringp arg1) const;
        void throwError(int id, Stringp arg1, Stringp arg2) const;

        void throwArgumentError(int id) const;
        void throwArgumentError(int id, Stringp arg1) const;
        void throwArgumentError(int id, const char *arg1) const;
        void throwArgumentError(int id, Stringp arg1, Stringp arg2) const;

        void throwRangeError(int id) const;
        void throwRangeError(int id, Stringp arg1) const;
        void throwRangeError(int id, Stringp arg1, Stringp arg2) const;
        void throwRangeError(int id, Stringp arg1, Stringp arg2, Stringp arg3) const;

        void throwReferenceError(int id, const Multiname* multiname, const Traits* traits) const;
        void throwReferenceError(int id, const Multiname* multiname) const;

        void throwReferenceError(int id, const Multiname& multiname, const Traits* traits) const;
        void throwReferenceError(int id, const Multiname& multiname) const;

        void throwReferenceError(int id, String* name, const Traits* traits) const;
        void throwReferenceError(int id, String* name) const;

        inline Toplevel* toplevel() { return this; }
        inline const Toplevel* toplevel() const { return this; }

        REALLY_INLINE void checkNull(void* instance, const char* name)
        {
            if (instance == NULL)
                throwNullPointerError(name);
        }
        
        void FASTCALL throwNullPointerError(const char* name);

        void FASTCALL throwMemoryError(int id);
        void FASTCALL throwIOError(int id);
        void FASTCALL throwEOFError(int id);

        //
        // methods that used to be on AvmCore but depend on the caller's environment
        //
        ScriptObject* toPrototype(Atom atom);
        VTable* toVTable(Atom atom);

        /** toObject + get traits */
        Traits*        toTraits(Atom atom);

        /**
         * OP_call.
         *
         * this = atomv[0]
         * arg1 = atomv[1]
         * argN = atomv[argc]
         */
        Atom op_call(Atom method,
                     int argc,
                     Atom* atomv);

        /**
         * OP_construct.
         *
         * this = atomv[0] (ignored)
         * arg1 = atomv[1]
         * argN = atomv[argc]
         */
        Atom op_construct(Atom ctor,
                          int argc,
                          Atom* atomv);


        /** Implementation of OP_callproperty */
        Atom callproperty(Atom base, const Multiname* name, int argc, Atom* atomv, VTable* vtable);

        /** Implementation of OP_constructprop */
        Atom constructprop(const Multiname* name, int argc, Atom* atomv, VTable* vtable);

        /**
        * OP_applytype.
        *
        * arg1 = atomv[0]
        * argN = atomv[argc-1]
        */
        Atom op_applytype(Atom obj,
            int argc,
            Atom* atomv);

        /**
         * Implements the ToAttributeName API as specified in E4X 10.5.1, pg 37
         */
        QNameObject* ToAttributeName (Atom arg);
        QNameObject* ToAttributeName (const Stringp arg);

        /**
         * Implements the ToXMLName API as specified in E4X 10.6.1, page 38
         */
        void ToXMLName (const Atom arg, Multiname& m);

        /**
         * E4X support for coercing regular Multinames into E4X specific ones
         */
        void CoerceE4XMultiname (const Multiname *m, Multiname &out);

        /**
         * operator instanceof from ES3
         */
        Atom instanceof(Atom atom, Atom ctor);

        /** returns the instance traits of the factorytype of the passed atom */
        Traits* toClassITraits(Atom atom);

        /**
         * operator in from ES3
         */
        Atom in_operator(Atom name, Atom obj);

    public:
        /** legacy wrapper around coerce() from instr.h */
        Atom coerce(Atom atom, Traits* expected) const;

        /**
         * Reads a property from an object, with the property
         * to retrieve specified by its binding.
         * The binding was likely retrieved using getBinding.
         * @param obj Object to retrieve property from
         * @param b The binding of the property
         * @param traits The traits of the object
         */
        Atom getproperty(Atom obj, const Multiname* name, VTable* vtable);

        /**
         * Determines if a specified object has a specified property
         * where the property is specified by a multiname.
         * @param obj Object on which to look for the property
         * @param multiname The name of the property
         * @param vtable The vtable of the object
         * @return  true if the object has a readable property with the
                    specified name, false otherwise.
         */
        bool hasproperty(Atom obj, const Multiname* multiname, VTable* vtable);

        /**
         * Delete a specified property on a specified object,where the property is specified
         * by a multiname.
         * @param obj Object on which to look for the specified property.
         * @param multiname The name of the property
         * @param vtable The vtable of the object
         * @return  true if the property is deleted. false if the property cannot be deleted.
         */
        bool deleteproperty( Atom obj, const Multiname* multiname, VTable* vtable ) const;

        void setproperty(Atom obj, const Multiname* multiname, Atom value, VTable* vtable) const;
        void setproperty_b(Atom obj, const Multiname* multiname, Atom value, VTable* vtable, Binding b) const;

        /**
         * operator +
         */
        Atom add2(Atom lhs, Atom rhs);

        /**
         * Implements the GetDefaultNamespace API as specified in E4X 12.1.1, pg 59
         *
         */
        Namespace *getDefaultNamespace();

        /**
         * Retrieve a binding for a property of a class.
         * This differs from the other overload of getBinding
         * in that it takes a multiname instead of a name/ns
         * pair.
         * The returned binding can then be used to read/write
         * the property
         * @param traits    The traits of the class
         * @param multiname The multiname of the property
         */
        Binding getBinding(Traits* traits, const Multiname* multiname) const;

        // like getBinding, but do extra work to find the initial declarer of the member
        Binding getBindingAndDeclarer(Traits* traits, const Multiname& multiname, Traitsp& declarer) const;

        /**
         * @name ECMA-262 Appendix B.2 extensions
         * Extensions to ECMAScript, in ECMA-262 Appendix B.2
         */
        /*@{*/
        static Stringp escape(ScriptObject*, Stringp in);
        static Stringp unescape(ScriptObject*, Stringp in);
        /*@}*/

        /**
         * E262-3 eval, but for the top level only (no lexical capture)
         */
        static Atom eval(ScriptObject*, Atom in);

        /**
         * This is a variant of escape() which doesn't encode
         * Unicode characters using the %u sequence
         */
        Stringp escapeBytes(Stringp in);

        /**
         * @name Toplevel Function Properties
         * Function properties of the global object (ECMA 15.1.2)
         */
        /*@{*/
        static Stringp decodeURI(ScriptObject*, Stringp uri);
        static Stringp decodeURIComponent(ScriptObject*, Stringp uri);
        static Stringp encodeURI(ScriptObject*, Stringp uri);
        static Stringp encodeURIComponent(ScriptObject*, Stringp uri);
        static bool isNaN(ScriptObject*, double d);
        static bool isFinite(ScriptObject*, double d);
        static double parseInt(ScriptObject*, Stringp in, int radix);
        static double parseFloat(ScriptObject*, Stringp in);
        static bool bugzilla(ScriptObject*, int32_t n);
        /*@}*/

        // For E4X
        static bool isXMLName(ScriptObject*, Atom v);

        unsigned int readU30(const uint8_t *&p) const;

        // subclasses can override this to check for security violations
        // and prohibit certain operations. default implementation always
        // allows but FlashPlayer takes advantage of this.
        virtual bool sampler_trusted(ScriptObject* /*sampler*/);

        ScopeChain* toplevel_scope();

        //  -------------------------------------------------------
        
        //
        // These methods are used by DataIO to handle conversion to/from specific
        // non-Unicode text encodings. The default implementation understands nothing, and
        // always returns 0.
        //
        // Subclasses may implement a larger set of codepages (see http://en.wikipedia.org/wiki/Code_page)
        // but should ensure that any recognized by charsetToCodepage()
        // are also handled by the conversion routines. They should also
        // ensure to return zero for illegal input.
        //
        // Note that there may be multiple charset strings that map to
        // the same codepage; this is fine (although the defaults
        // above don't express all known acceptable charset equivalents)
        //
        virtual uint32_t charsetToCodepage(String* charset);

        //
        // Read length bytes from the DataInput and convert it to a String
        // under the assumption it is formatted in the given codepage.
        // and return it as a String.
        //
        // codepage can be assumed to be a value returned by the charsetToCodepage() method.
        //
        // Note that the implementation is not allowed to call DataInput::ReadMultiByte.
        //
        // If the data is malformed (e.g., is not valid for the
        // given codepage), this method may return an arbitrary
        // string or throw an error, but it must never return null.
        // (Returning an arbitrary string seems odd, but is the behavior
        // in certain existing versions of Flash, so must be kept legal.)
        //
        virtual String* readMultiByte(uint32_t codepage, uint32_t length, DataInput* input);

        //
        // Convert the given (Unicode) String into the given codepage and write it to the given DataOutput.
        //
        // codepage can be assumed to be a value returned by the charsetToCodepage() method.
        //
        // Note that the implementation is not allowed to call DataOutut::WriteMultiByte.
        //
        // If the data is malformed (e.g., is not valid for the
        // given codepage), this method may write an arbitrary
        // string to the DataOutput.
        // (Returning an arbitrary string seems odd, but is the behavior
        // in certain existing versions of Flash, so must be kept legal.)
        //
        virtual void writeMultiByte(uint32_t codepage, String* str, DataOutput* output);

        //  -------------------------------------------------------

        //
        // If a ByteArray doesn't begin with a BOM, Flash may want ByteArray.toString()
        // to attempt a conversion from the system's default codepage as though the data
        // is MBCS. If such a conversion is desirable and possible, you should return the
        // result as a String. If the conversion is either impossible or undesirable, return NULL.
        //
        virtual String* tryFromSystemCodepage(const uint8_t* data);

        //  -------------------------------------------------------
        
        //
        // Deserialize an Atom value from the input using the specified encoding.
        // If the encoding is not valid, or the input stream is malformed, throw an error.
        //
        // Note that the implementation is not allowed to call DataInput::ReadObject.
        //
        // Note that the default implementation always throws an error; this is
        // simply a hook for embedders to use for custom implementations.
        //
        virtual Atom readObject(ObjectEncoding encoding, DataInput* input);

        //
        // Serialize the given Atom value into the output using the specified encoding.
        // If the encoding is not valid, throw an error.
        //
        // Note that the implementation is not allowed to call DataOutput::WriteObject.
        //
        // Note that the default implementation always throws an error; this is
        // simply a hook for embedders to use for custom implementations.
        //
        virtual void writeObject(ObjectEncoding encoding, DataOutput* output, Atom a);

        //  -------------------------------------------------------

        //
        // This is called anytime a ByteArrayObject (or subclass thereof) is created;
        // it allows the ByteArray to be pre-initialized (eg via SetCopyOnWriteData),
        // most typically if it is a subclass that is bound to a particular data set.
        // (Most typically this is done via ByteArrayAsset in Flex code.) The relevant
        // mapping tables don't exist in the VM proper, so this is a no-op in current Tamarin.
        //
        virtual void byteArrayCreated(ByteArrayObject* byteArrayObject);

        //  -------------------------------------------------------


    protected:
        ClassClosure* findClassInScriptEnv(int class_id, ScriptEnv* env);

    private:

        static int parseHexChar(wchar c);
        static Stringp decode(AvmCore* core, Stringp in, bool decodeURIComponentFlag);
        static Stringp encode(AvmCore* core, Stringp in, bool encodeURIComponentFlag);

        static const uint32_t unescaped[];
        static const uint32_t uriUnescaped[];
        static const uint32_t uriReservedPlusPound[];

        static bool contains(const uint32_t *uriSet, uint32_t ch);

    // ------------------------ DATA SECTION BEGIN
        GC_DATA_BEGIN(Toplevel)

    private:
        GCMember<AbcEnv>               GC_POINTER(_abcEnv);
        GCMember<ScriptEnv>            GC_POINTER(_mainEntryPoint);
        GCMember<builtinClassManifest> GC_POINTER(_builtinClasses);
    public:
        // objectClass is still needed for bootstrapping
        // the VM; it's still public because of 100's of
        // to-be-expunged references in Flash/AIR
        GCMember<ObjectClass>          GC_POINTER(objectClass);
    private:
        // This are also needed for bootstrapping, but shouldn't be accessible
        // to the general public.
        // NB: if you add or change these, you may need to
        // update ClassManifestBase::lazyInitClass.
        friend class ClassClass;
        friend class FunctionClass;
        friend class MethodEnv;
        GCMember<ClassClass>        GC_POINTER(_classClass);
        GCMember<FunctionClass>     GC_POINTER(_functionClass);
    private:
        // These exist solely for CodegenLIR to access
        // NB: if you add or change these, you may need to
        // update ClassManifestBase::lazyInitClass.
        friend class CodegenLIR;
        friend class BooleanClass;
        friend class NamespaceClass;
        friend class NumberClass;
        friend class IntClass;
        friend class UIntClass;
        friend class StringClass;
        GCMember<BooleanClass>      GC_POINTER(_booleanClass);
        GCMember<NamespaceClass>    GC_POINTER(_namespaceClass);
        GCMember<NumberClass>       GC_POINTER(_numberClass);
        GCMember<IntClass>          GC_POINTER(_intClass);
        GCMember<UIntClass>         GC_POINTER(_uintClass);
        GCMember<StringClass>       GC_POINTER(_stringClass);

        GC_DATA_END(Toplevel)
    // ------------------------ DATA SECTION END
    };
}

#endif /* __avmplus_Toplevel__ */
