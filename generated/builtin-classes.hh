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
 * The Original Code is [Open Source Virtual Machine].
 *
 * The Initial Developer of the Original Code is
 * Adobe System Incorporated.
 * Portions created by the Initial Developer are Copyright (C) 2008
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

/* machine generated file -- do not edit */

#ifndef _H_nativegen_classes_builtin
#define _H_nativegen_classes_builtin

namespace avmplus {

//-----------------------------------------------------------
// flash.errors::IOError
//-----------------------------------------------------------
class IOErrorObject : public avmplus::ErrorObject
{
    GC_DECLARE_EXACT_METHODS
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    friend class avmplus::IOErrorClass;
    REALLY_INLINE explicit IOErrorObject(VTable* ivtable, ScriptObject* delegate) : avmplus::ErrorObject(ivtable, delegate) {}
private:
    explicit IOErrorObject(const IOErrorObject&); // unimplemented
    void operator=(const IOErrorObject&); // unimplemented
};

#define avmplus_IOErrorObject_isExactInterlock 1
//-----------------------------------------------------------
// flash.errors::IOError$
//-----------------------------------------------------------
class IOErrorClass : public avmplus::ClassClosure
{
    GC_DECLARE_EXACT_METHODS
public:
    static avmplus::ClassClosure* FASTCALL createClassClosure(avmplus::VTable* cvtable);
public:
    static avmplus::ScriptObject* FASTCALL createInstanceProc(avmplus::ClassClosure*);
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
public:
    inline GCRef<avmplus::IOErrorObject> constructObject(GCRef<avmplus::String> arg1, int32_t arg2)
    {
        avmplus::AvmCore* const core = ((avmplus::AvmCore*)(this->core()));
        avmplus::Atom args[3] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom(), arg1->atom(), core->intToAtom(arg2) };
        avmplus::Atom const result = this->construct(2, args);
        return GCRef<avmplus::IOErrorObject>((avmplus::IOErrorObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    inline GCRef<avmplus::IOErrorObject> constructObject(GCRef<avmplus::String> arg1)
    {
        avmplus::Atom args[2] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom(), arg1->atom() };
        avmplus::Atom const result = this->construct(1, args);
        return GCRef<avmplus::IOErrorObject>((avmplus::IOErrorObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    inline GCRef<avmplus::IOErrorObject> constructObject()
    {
        avmplus::Atom args[1] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom() };
        avmplus::Atom const result = this->construct(0, args);
        return GCRef<avmplus::IOErrorObject>((avmplus::IOErrorObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
public:
    REALLY_INLINE bool isType(avmplus::Atom value)
    {
        return isTypeImpl(value);
    }
    REALLY_INLINE bool isType(GCRef<avmplus::ScriptObject> value)
    {
        return isTypeImpl(value->atom());
    }
    REALLY_INLINE GCRef<avmplus::IOErrorObject> asType(avmplus::Atom value)
    {
        avmplus::Atom const result = asTypeImpl(value);
        return GCRef<avmplus::IOErrorObject>((avmplus::IOErrorObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::IOErrorObject> asType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = asTypeImpl(value->atom());
        return GCRef<avmplus::IOErrorObject>((avmplus::IOErrorObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::IOErrorObject> coerceToType(avmplus::Atom value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value);
        return GCRef<avmplus::IOErrorObject>((avmplus::IOErrorObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::IOErrorObject> coerceToType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value->atom());
        return GCRef<avmplus::IOErrorObject>((avmplus::IOErrorObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    inline explicit IOErrorClass(VTable* cvtable) : avmplus::ClassClosure(cvtable) { createVanillaPrototype(); }
private:
    explicit IOErrorClass(const IOErrorClass&); // unimplemented
    void operator=(const IOErrorClass&); // unimplemented
};

#define avmplus_IOErrorClass_isExactInterlock 1
//-----------------------------------------------------------
// flash.errors::MemoryError
//-----------------------------------------------------------
class MemoryErrorObject : public avmplus::ErrorObject
{
    GC_DECLARE_EXACT_METHODS
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    friend class avmplus::MemoryErrorClass;
    REALLY_INLINE explicit MemoryErrorObject(VTable* ivtable, ScriptObject* delegate) : avmplus::ErrorObject(ivtable, delegate) {}
private:
    explicit MemoryErrorObject(const MemoryErrorObject&); // unimplemented
    void operator=(const MemoryErrorObject&); // unimplemented
};

#define avmplus_MemoryErrorObject_isExactInterlock 1
//-----------------------------------------------------------
// flash.errors::MemoryError$
//-----------------------------------------------------------
class MemoryErrorClass : public avmplus::ClassClosure
{
    GC_DECLARE_EXACT_METHODS
public:
    static avmplus::ClassClosure* FASTCALL createClassClosure(avmplus::VTable* cvtable);
public:
    static avmplus::ScriptObject* FASTCALL createInstanceProc(avmplus::ClassClosure*);
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
public:
    inline GCRef<avmplus::MemoryErrorObject> constructObject(GCRef<avmplus::String> arg1, int32_t arg2)
    {
        avmplus::AvmCore* const core = ((avmplus::AvmCore*)(this->core()));
        avmplus::Atom args[3] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom(), arg1->atom(), core->intToAtom(arg2) };
        avmplus::Atom const result = this->construct(2, args);
        return GCRef<avmplus::MemoryErrorObject>((avmplus::MemoryErrorObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    inline GCRef<avmplus::MemoryErrorObject> constructObject(GCRef<avmplus::String> arg1)
    {
        avmplus::Atom args[2] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom(), arg1->atom() };
        avmplus::Atom const result = this->construct(1, args);
        return GCRef<avmplus::MemoryErrorObject>((avmplus::MemoryErrorObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    inline GCRef<avmplus::MemoryErrorObject> constructObject()
    {
        avmplus::Atom args[1] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom() };
        avmplus::Atom const result = this->construct(0, args);
        return GCRef<avmplus::MemoryErrorObject>((avmplus::MemoryErrorObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
public:
    REALLY_INLINE bool isType(avmplus::Atom value)
    {
        return isTypeImpl(value);
    }
    REALLY_INLINE bool isType(GCRef<avmplus::ScriptObject> value)
    {
        return isTypeImpl(value->atom());
    }
    REALLY_INLINE GCRef<avmplus::MemoryErrorObject> asType(avmplus::Atom value)
    {
        avmplus::Atom const result = asTypeImpl(value);
        return GCRef<avmplus::MemoryErrorObject>((avmplus::MemoryErrorObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::MemoryErrorObject> asType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = asTypeImpl(value->atom());
        return GCRef<avmplus::MemoryErrorObject>((avmplus::MemoryErrorObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::MemoryErrorObject> coerceToType(avmplus::Atom value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value);
        return GCRef<avmplus::MemoryErrorObject>((avmplus::MemoryErrorObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::MemoryErrorObject> coerceToType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value->atom());
        return GCRef<avmplus::MemoryErrorObject>((avmplus::MemoryErrorObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    inline explicit MemoryErrorClass(VTable* cvtable) : avmplus::ClassClosure(cvtable) { createVanillaPrototype(); }
private:
    explicit MemoryErrorClass(const MemoryErrorClass&); // unimplemented
    void operator=(const MemoryErrorClass&); // unimplemented
};

#define avmplus_MemoryErrorClass_isExactInterlock 1
//-----------------------------------------------------------
// flash.errors::IllegalOperationError
//-----------------------------------------------------------
class IllegalOperationErrorObject : public avmplus::ErrorObject
{
    GC_DECLARE_EXACT_METHODS
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    friend class avmplus::IllegalOperationErrorClass;
    REALLY_INLINE explicit IllegalOperationErrorObject(VTable* ivtable, ScriptObject* delegate) : avmplus::ErrorObject(ivtable, delegate) {}
private:
    explicit IllegalOperationErrorObject(const IllegalOperationErrorObject&); // unimplemented
    void operator=(const IllegalOperationErrorObject&); // unimplemented
};

#define avmplus_IllegalOperationErrorObject_isExactInterlock 1
//-----------------------------------------------------------
// flash.errors::IllegalOperationError$
//-----------------------------------------------------------
class IllegalOperationErrorClass : public avmplus::ClassClosure
{
    GC_DECLARE_EXACT_METHODS
public:
    static avmplus::ClassClosure* FASTCALL createClassClosure(avmplus::VTable* cvtable);
public:
    static avmplus::ScriptObject* FASTCALL createInstanceProc(avmplus::ClassClosure*);
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
public:
    inline GCRef<avmplus::IllegalOperationErrorObject> constructObject(GCRef<avmplus::String> arg1, int32_t arg2)
    {
        avmplus::AvmCore* const core = ((avmplus::AvmCore*)(this->core()));
        avmplus::Atom args[3] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom(), arg1->atom(), core->intToAtom(arg2) };
        avmplus::Atom const result = this->construct(2, args);
        return GCRef<avmplus::IllegalOperationErrorObject>((avmplus::IllegalOperationErrorObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    inline GCRef<avmplus::IllegalOperationErrorObject> constructObject(GCRef<avmplus::String> arg1)
    {
        avmplus::Atom args[2] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom(), arg1->atom() };
        avmplus::Atom const result = this->construct(1, args);
        return GCRef<avmplus::IllegalOperationErrorObject>((avmplus::IllegalOperationErrorObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    inline GCRef<avmplus::IllegalOperationErrorObject> constructObject()
    {
        avmplus::Atom args[1] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom() };
        avmplus::Atom const result = this->construct(0, args);
        return GCRef<avmplus::IllegalOperationErrorObject>((avmplus::IllegalOperationErrorObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
public:
    REALLY_INLINE bool isType(avmplus::Atom value)
    {
        return isTypeImpl(value);
    }
    REALLY_INLINE bool isType(GCRef<avmplus::ScriptObject> value)
    {
        return isTypeImpl(value->atom());
    }
    REALLY_INLINE GCRef<avmplus::IllegalOperationErrorObject> asType(avmplus::Atom value)
    {
        avmplus::Atom const result = asTypeImpl(value);
        return GCRef<avmplus::IllegalOperationErrorObject>((avmplus::IllegalOperationErrorObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::IllegalOperationErrorObject> asType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = asTypeImpl(value->atom());
        return GCRef<avmplus::IllegalOperationErrorObject>((avmplus::IllegalOperationErrorObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::IllegalOperationErrorObject> coerceToType(avmplus::Atom value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value);
        return GCRef<avmplus::IllegalOperationErrorObject>((avmplus::IllegalOperationErrorObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::IllegalOperationErrorObject> coerceToType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value->atom());
        return GCRef<avmplus::IllegalOperationErrorObject>((avmplus::IllegalOperationErrorObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    inline explicit IllegalOperationErrorClass(VTable* cvtable) : avmplus::ClassClosure(cvtable) { createVanillaPrototype(); }
private:
    explicit IllegalOperationErrorClass(const IllegalOperationErrorClass&); // unimplemented
    void operator=(const IllegalOperationErrorClass&); // unimplemented
};

#define avmplus_IllegalOperationErrorClass_isExactInterlock 1
//-----------------------------------------------------------
// flash.errors::EOFError
//-----------------------------------------------------------
class EOFErrorObject : public avmplus::IOErrorObject
{
    GC_DECLARE_EXACT_METHODS
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    friend class avmplus::EOFErrorClass;
    REALLY_INLINE explicit EOFErrorObject(VTable* ivtable, ScriptObject* delegate) : avmplus::IOErrorObject(ivtable, delegate) {}
private:
    explicit EOFErrorObject(const EOFErrorObject&); // unimplemented
    void operator=(const EOFErrorObject&); // unimplemented
};

#define avmplus_EOFErrorObject_isExactInterlock 1
//-----------------------------------------------------------
// flash.errors::EOFError$
//-----------------------------------------------------------
class EOFErrorClass : public avmplus::ClassClosure
{
    GC_DECLARE_EXACT_METHODS
public:
    static avmplus::ClassClosure* FASTCALL createClassClosure(avmplus::VTable* cvtable);
public:
    static avmplus::ScriptObject* FASTCALL createInstanceProc(avmplus::ClassClosure*);
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
public:
    inline GCRef<avmplus::EOFErrorObject> constructObject(GCRef<avmplus::String> arg1, int32_t arg2)
    {
        avmplus::AvmCore* const core = ((avmplus::AvmCore*)(this->core()));
        avmplus::Atom args[3] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom(), arg1->atom(), core->intToAtom(arg2) };
        avmplus::Atom const result = this->construct(2, args);
        return GCRef<avmplus::EOFErrorObject>((avmplus::EOFErrorObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    inline GCRef<avmplus::EOFErrorObject> constructObject(GCRef<avmplus::String> arg1)
    {
        avmplus::Atom args[2] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom(), arg1->atom() };
        avmplus::Atom const result = this->construct(1, args);
        return GCRef<avmplus::EOFErrorObject>((avmplus::EOFErrorObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    inline GCRef<avmplus::EOFErrorObject> constructObject()
    {
        avmplus::Atom args[1] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom() };
        avmplus::Atom const result = this->construct(0, args);
        return GCRef<avmplus::EOFErrorObject>((avmplus::EOFErrorObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
public:
    REALLY_INLINE bool isType(avmplus::Atom value)
    {
        return isTypeImpl(value);
    }
    REALLY_INLINE bool isType(GCRef<avmplus::ScriptObject> value)
    {
        return isTypeImpl(value->atom());
    }
    REALLY_INLINE GCRef<avmplus::EOFErrorObject> asType(avmplus::Atom value)
    {
        avmplus::Atom const result = asTypeImpl(value);
        return GCRef<avmplus::EOFErrorObject>((avmplus::EOFErrorObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::EOFErrorObject> asType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = asTypeImpl(value->atom());
        return GCRef<avmplus::EOFErrorObject>((avmplus::EOFErrorObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::EOFErrorObject> coerceToType(avmplus::Atom value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value);
        return GCRef<avmplus::EOFErrorObject>((avmplus::EOFErrorObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::EOFErrorObject> coerceToType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value->atom());
        return GCRef<avmplus::EOFErrorObject>((avmplus::EOFErrorObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    inline explicit EOFErrorClass(VTable* cvtable) : avmplus::ClassClosure(cvtable) { createVanillaPrototype(); }
private:
    explicit EOFErrorClass(const EOFErrorClass&); // unimplemented
    void operator=(const EOFErrorClass&); // unimplemented
};

#define avmplus_EOFErrorClass_isExactInterlock 1
//-----------------------------------------------------------
// JSON
//-----------------------------------------------------------
class JSONObject : public avmplus::ScriptObject
{
    GC_DECLARE_EXACT_METHODS
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    friend class avmplus::JSONClass;
    REALLY_INLINE explicit JSONObject(VTable* ivtable, ScriptObject* delegate) : avmplus::ScriptObject(ivtable, delegate) {}
private:
    explicit JSONObject(const JSONObject&); // unimplemented
    void operator=(const JSONObject&); // unimplemented
};

#define avmplus_JSONObject_isExactInterlock 1
//-----------------------------------------------------------
// Walker
//-----------------------------------------------------------
class WalkerObject : public avmplus::ScriptObject
{
    GC_DECLARE_EXACT_METHODS
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    REALLY_INLINE avmplus::FunctionObject* get_reviver() const { return m_slots_WalkerObject.m_reviver; }
    REALLY_INLINE void set_reviver(avmplus::FunctionObject* newVal) { m_slots_WalkerObject.m_reviver = newVal; }
private:
    avmplus::NativeID::avmplus_WalkerObjectSlots m_slots_WalkerObject;
protected:
    friend class avmplus::WalkerClass;
    REALLY_INLINE explicit WalkerObject(VTable* ivtable, ScriptObject* delegate) : avmplus::ScriptObject(ivtable, delegate) {}
private:
    explicit WalkerObject(const WalkerObject&); // unimplemented
    void operator=(const WalkerObject&); // unimplemented
};

#define avmplus_WalkerObject_isExactInterlock 1
//-----------------------------------------------------------
// Walker$
//-----------------------------------------------------------
class WalkerClass : public avmplus::ClassClosure
{
    GC_DECLARE_EXACT_METHODS
public:
    static avmplus::ClassClosure* FASTCALL createClassClosure(avmplus::VTable* cvtable);
public:
    static avmplus::ScriptObject* FASTCALL createInstanceProc(avmplus::ClassClosure*);
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
public:
    inline GCRef<avmplus::WalkerObject> constructObject(GCRef<avmplus::FunctionObject> arg1)
    {
        avmplus::Atom args[2] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom(), arg1.reinterpretCast<avmplus::ScriptObject>()->atom() };
        avmplus::Atom const result = this->construct(1, args);
        return GCRef<avmplus::WalkerObject>((avmplus::WalkerObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
public:
    REALLY_INLINE bool isType(avmplus::Atom value)
    {
        return isTypeImpl(value);
    }
    REALLY_INLINE bool isType(GCRef<avmplus::ScriptObject> value)
    {
        return isTypeImpl(value->atom());
    }
    REALLY_INLINE GCRef<avmplus::WalkerObject> asType(avmplus::Atom value)
    {
        avmplus::Atom const result = asTypeImpl(value);
        return GCRef<avmplus::WalkerObject>((avmplus::WalkerObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::WalkerObject> asType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = asTypeImpl(value->atom());
        return GCRef<avmplus::WalkerObject>((avmplus::WalkerObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::WalkerObject> coerceToType(avmplus::Atom value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value);
        return GCRef<avmplus::WalkerObject>((avmplus::WalkerObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::WalkerObject> coerceToType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value->atom());
        return GCRef<avmplus::WalkerObject>((avmplus::WalkerObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    inline explicit WalkerClass(VTable* cvtable) : avmplus::ClassClosure(cvtable) { createVanillaPrototype(); }
private:
    explicit WalkerClass(const WalkerClass&); // unimplemented
    void operator=(const WalkerClass&); // unimplemented
};

#define avmplus_WalkerClass_isExactInterlock 1
//-----------------------------------------------------------
// flash.utils::IDataInput
//-----------------------------------------------------------
class IDataInputInterface : public avmplus::ScriptObject
{
public:
    inline uint32_t call_get_bytesAvailable()
    {
        const avmplus::Multiname* const mn = this->traits()->pool->precomputedMultiname(1381);
        avmplus::Atom const result = this->toplevel()->getproperty(this->atom(), mn, this->vtable);
        return avmplus::AvmCore::toUInt32(result);
    }
    inline GCRef<avmplus::String> call_get_endian()
    {
        const avmplus::Multiname* const mn = this->traits()->pool->precomputedMultiname(1384);
        avmplus::Atom const result = this->toplevel()->getproperty(this->atom(), mn, this->vtable);
        return GCRef<avmplus::String>(avmplus::AvmCore::atomToString(result));
    }
    inline uint32_t call_get_objectEncoding()
    {
        const avmplus::Multiname* const mn = this->traits()->pool->precomputedMultiname(1383);
        avmplus::Atom const result = this->toplevel()->getproperty(this->atom(), mn, this->vtable);
        return avmplus::AvmCore::toUInt32(result);
    }
    inline bool call_readBoolean()
    {
        const avmplus::Multiname* const mn = this->traits()->pool->precomputedMultiname(1369);
        avmplus::Atom args[1] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom() };
        avmplus::Atom const result = this->toplevel()->callproperty(this->atom(), mn, 0, args, this->vtable);
        return ((result) != avmplus::falseAtom);
    }
    inline int32_t call_readByte()
    {
        const avmplus::Multiname* const mn = this->traits()->pool->precomputedMultiname(1370);
        avmplus::Atom args[1] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom() };
        avmplus::Atom const result = this->toplevel()->callproperty(this->atom(), mn, 0, args, this->vtable);
        return avmplus::AvmCore::integer(result);
    }
    inline void call_readBytes(GCRef<avmplus::ByteArrayObject> arg1, uint32_t arg2, uint32_t arg3)
    {
        avmplus::AvmCore* const core = ((avmplus::AvmCore*)(this->core()));
        const avmplus::Multiname* const mn = this->traits()->pool->precomputedMultiname(1368);
        avmplus::Atom args[4] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom(), arg1.reinterpretCast<avmplus::ScriptObject>()->atom(), core->uintToAtom(arg2), core->uintToAtom(arg3) };
        avmplus::Atom const result = this->toplevel()->callproperty(this->atom(), mn, 3, args, this->vtable);
        AvmAssert(result == undefinedAtom); (void)result;
    }
    inline double call_readDouble()
    {
        const avmplus::Multiname* const mn = this->traits()->pool->precomputedMultiname(1377);
        avmplus::Atom args[1] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom() };
        avmplus::Atom const result = this->toplevel()->callproperty(this->atom(), mn, 0, args, this->vtable);
        return avmplus::AvmCore::number(result);
    }
    inline double call_readFloat()
    {
        const avmplus::Multiname* const mn = this->traits()->pool->precomputedMultiname(1376);
        avmplus::Atom args[1] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom() };
        avmplus::Atom const result = this->toplevel()->callproperty(this->atom(), mn, 0, args, this->vtable);
        return avmplus::AvmCore::number(result);
    }
    inline int32_t call_readInt()
    {
        const avmplus::Multiname* const mn = this->traits()->pool->precomputedMultiname(1374);
        avmplus::Atom args[1] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom() };
        avmplus::Atom const result = this->toplevel()->callproperty(this->atom(), mn, 0, args, this->vtable);
        return avmplus::AvmCore::integer(result);
    }
    inline GCRef<avmplus::String> call_readMultiByte(uint32_t arg1, GCRef<avmplus::String> arg2)
    {
        avmplus::AvmCore* const core = ((avmplus::AvmCore*)(this->core()));
        const avmplus::Multiname* const mn = this->traits()->pool->precomputedMultiname(1378);
        avmplus::Atom args[3] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom(), core->uintToAtom(arg1), arg2->atom() };
        avmplus::Atom const result = this->toplevel()->callproperty(this->atom(), mn, 2, args, this->vtable);
        return GCRef<avmplus::String>(avmplus::AvmCore::atomToString(result));
    }
    inline avmplus::Atom call_readObject()
    {
        const avmplus::Multiname* const mn = this->traits()->pool->precomputedMultiname(1382);
        avmplus::Atom args[1] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom() };
        avmplus::Atom const result = this->toplevel()->callproperty(this->atom(), mn, 0, args, this->vtable);
        return result;
    }
    inline int32_t call_readShort()
    {
        const avmplus::Multiname* const mn = this->traits()->pool->precomputedMultiname(1372);
        avmplus::Atom args[1] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom() };
        avmplus::Atom const result = this->toplevel()->callproperty(this->atom(), mn, 0, args, this->vtable);
        return avmplus::AvmCore::integer(result);
    }
    inline GCRef<avmplus::String> call_readUTF()
    {
        const avmplus::Multiname* const mn = this->traits()->pool->precomputedMultiname(1379);
        avmplus::Atom args[1] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom() };
        avmplus::Atom const result = this->toplevel()->callproperty(this->atom(), mn, 0, args, this->vtable);
        return GCRef<avmplus::String>(avmplus::AvmCore::atomToString(result));
    }
    inline GCRef<avmplus::String> call_readUTFBytes(uint32_t arg1)
    {
        avmplus::AvmCore* const core = ((avmplus::AvmCore*)(this->core()));
        const avmplus::Multiname* const mn = this->traits()->pool->precomputedMultiname(1380);
        avmplus::Atom args[2] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom(), core->uintToAtom(arg1) };
        avmplus::Atom const result = this->toplevel()->callproperty(this->atom(), mn, 1, args, this->vtable);
        return GCRef<avmplus::String>(avmplus::AvmCore::atomToString(result));
    }
    inline uint32_t call_readUnsignedByte()
    {
        const avmplus::Multiname* const mn = this->traits()->pool->precomputedMultiname(1371);
        avmplus::Atom args[1] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom() };
        avmplus::Atom const result = this->toplevel()->callproperty(this->atom(), mn, 0, args, this->vtable);
        return avmplus::AvmCore::toUInt32(result);
    }
    inline uint32_t call_readUnsignedInt()
    {
        const avmplus::Multiname* const mn = this->traits()->pool->precomputedMultiname(1375);
        avmplus::Atom args[1] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom() };
        avmplus::Atom const result = this->toplevel()->callproperty(this->atom(), mn, 0, args, this->vtable);
        return avmplus::AvmCore::toUInt32(result);
    }
    inline uint32_t call_readUnsignedShort()
    {
        const avmplus::Multiname* const mn = this->traits()->pool->precomputedMultiname(1373);
        avmplus::Atom args[1] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom() };
        avmplus::Atom const result = this->toplevel()->callproperty(this->atom(), mn, 0, args, this->vtable);
        return avmplus::AvmCore::toUInt32(result);
    }
    inline void call_set_endian(GCRef<avmplus::String> arg1)
    {
        const avmplus::Multiname* const mn = this->traits()->pool->precomputedMultiname(1384);
        this->toplevel()->setproperty(this->atom(), mn, arg1->atom(), this->vtable);
        avmplus::Atom const result = undefinedAtom;
        AvmAssert(result == undefinedAtom); (void)result;
    }
    inline void call_set_objectEncoding(uint32_t arg1)
    {
        avmplus::AvmCore* const core = ((avmplus::AvmCore*)(this->core()));
        const avmplus::Multiname* const mn = this->traits()->pool->precomputedMultiname(1383);
        this->toplevel()->setproperty(this->atom(), mn, core->uintToAtom(arg1), this->vtable);
        avmplus::Atom const result = undefinedAtom;
        AvmAssert(result == undefinedAtom); (void)result;
    }
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    friend class avmplus::IDataInputClass;
    REALLY_INLINE explicit IDataInputInterface(VTable* ivtable, ScriptObject* delegate) : avmplus::ScriptObject(ivtable, delegate) {}
private:
    explicit IDataInputInterface(const IDataInputInterface&); // unimplemented
    void operator=(const IDataInputInterface&); // unimplemented
};

//-----------------------------------------------------------
// flash.utils::IDataInput$
//-----------------------------------------------------------
class IDataInputClass : public avmplus::ClassClosure
{
public:
    REALLY_INLINE bool isType(avmplus::Atom value)
    {
        return isTypeImpl(value);
    }
    REALLY_INLINE bool isType(GCRef<avmplus::ScriptObject> value)
    {
        return isTypeImpl(value->atom());
    }
    REALLY_INLINE GCRef<avmplus::IDataInputInterface> asType(avmplus::Atom value)
    {
        avmplus::Atom const result = asTypeImpl(value);
        return GCRef<avmplus::IDataInputInterface>((avmplus::IDataInputInterface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::IDataInputInterface> asType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = asTypeImpl(value->atom());
        return GCRef<avmplus::IDataInputInterface>((avmplus::IDataInputInterface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::IDataInputInterface> coerceToType(avmplus::Atom value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value);
        return GCRef<avmplus::IDataInputInterface>((avmplus::IDataInputInterface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::IDataInputInterface> coerceToType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value->atom());
        return GCRef<avmplus::IDataInputInterface>((avmplus::IDataInputInterface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    inline explicit IDataInputClass(VTable* cvtable) : avmplus::ClassClosure(cvtable) { createVanillaPrototype(); }
private:
    explicit IDataInputClass(const IDataInputClass&); // unimplemented
    void operator=(const IDataInputClass&); // unimplemented
};

//-----------------------------------------------------------
// flash.utils::IDataOutput
//-----------------------------------------------------------
class IDataOutputInterface : public avmplus::ScriptObject
{
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    friend class avmplus::IDataOutputClass;
    REALLY_INLINE explicit IDataOutputInterface(VTable* ivtable, ScriptObject* delegate) : avmplus::ScriptObject(ivtable, delegate) {}
private:
    explicit IDataOutputInterface(const IDataOutputInterface&); // unimplemented
    void operator=(const IDataOutputInterface&); // unimplemented
};

//-----------------------------------------------------------
// flash.utils::IDataOutput$
//-----------------------------------------------------------
class IDataOutputClass : public avmplus::ClassClosure
{
public:
    REALLY_INLINE bool isType(avmplus::Atom value)
    {
        return isTypeImpl(value);
    }
    REALLY_INLINE bool isType(GCRef<avmplus::ScriptObject> value)
    {
        return isTypeImpl(value->atom());
    }
    REALLY_INLINE GCRef<avmplus::IDataOutputInterface> asType(avmplus::Atom value)
    {
        avmplus::Atom const result = asTypeImpl(value);
        return GCRef<avmplus::IDataOutputInterface>((avmplus::IDataOutputInterface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::IDataOutputInterface> asType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = asTypeImpl(value->atom());
        return GCRef<avmplus::IDataOutputInterface>((avmplus::IDataOutputInterface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::IDataOutputInterface> coerceToType(avmplus::Atom value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value);
        return GCRef<avmplus::IDataOutputInterface>((avmplus::IDataOutputInterface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::IDataOutputInterface> coerceToType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value->atom());
        return GCRef<avmplus::IDataOutputInterface>((avmplus::IDataOutputInterface*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    inline explicit IDataOutputClass(VTable* cvtable) : avmplus::ClassClosure(cvtable) { createVanillaPrototype(); }
private:
    explicit IDataOutputClass(const IDataOutputClass&); // unimplemented
    void operator=(const IDataOutputClass&); // unimplemented
};

//-----------------------------------------------------------
// flash.utils::CompressionAlgorithm
//-----------------------------------------------------------
class CompressionAlgorithmObject : public avmplus::ScriptObject
{
    GC_DECLARE_EXACT_METHODS
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
protected:
    friend class avmplus::CompressionAlgorithmClass;
    REALLY_INLINE explicit CompressionAlgorithmObject(VTable* ivtable, ScriptObject* delegate) : avmplus::ScriptObject(ivtable, delegate) {}
private:
    explicit CompressionAlgorithmObject(const CompressionAlgorithmObject&); // unimplemented
    void operator=(const CompressionAlgorithmObject&); // unimplemented
};

#define avmplus_CompressionAlgorithmObject_isExactInterlock 1
//-----------------------------------------------------------
// flash.utils::CompressionAlgorithm$
//-----------------------------------------------------------
class CompressionAlgorithmClass : public avmplus::ClassClosure
{
    GC_DECLARE_EXACT_METHODS
public:
    static avmplus::ClassClosure* FASTCALL createClassClosure(avmplus::VTable* cvtable);
public:
    static avmplus::ScriptObject* FASTCALL createInstanceProc(avmplus::ClassClosure*);
public:
    AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )
private:
    AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )
public:
    inline GCRef<avmplus::CompressionAlgorithmObject> constructObject()
    {
        avmplus::Atom args[1] = { thisRef.reinterpretCast<avmplus::ScriptObject>()->atom() };
        avmplus::Atom const result = this->construct(0, args);
        return GCRef<avmplus::CompressionAlgorithmObject>((avmplus::CompressionAlgorithmObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
public:
    REALLY_INLINE bool isType(avmplus::Atom value)
    {
        return isTypeImpl(value);
    }
    REALLY_INLINE bool isType(GCRef<avmplus::ScriptObject> value)
    {
        return isTypeImpl(value->atom());
    }
    REALLY_INLINE GCRef<avmplus::CompressionAlgorithmObject> asType(avmplus::Atom value)
    {
        avmplus::Atom const result = asTypeImpl(value);
        return GCRef<avmplus::CompressionAlgorithmObject>((avmplus::CompressionAlgorithmObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::CompressionAlgorithmObject> asType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = asTypeImpl(value->atom());
        return GCRef<avmplus::CompressionAlgorithmObject>((avmplus::CompressionAlgorithmObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::CompressionAlgorithmObject> coerceToType(avmplus::Atom value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value);
        return GCRef<avmplus::CompressionAlgorithmObject>((avmplus::CompressionAlgorithmObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
    REALLY_INLINE GCRef<avmplus::CompressionAlgorithmObject> coerceToType(GCRef<avmplus::ScriptObject> value)
    {
        avmplus::Atom const result = coerceToTypeImpl(value->atom());
        return GCRef<avmplus::CompressionAlgorithmObject>((avmplus::CompressionAlgorithmObject*)(avmplus::AvmCore::atomToScriptObject(result)));
    }
private:
    friend class avmplus::NativeID::SlotOffsetsAndAsserts;
public:
    REALLY_INLINE avmplus::String* get_ZLIB() const { return m_slots_CompressionAlgorithmClass.m_ZLIB; }
    REALLY_INLINE void setconst_ZLIB(avmplus::String* newVal) { m_slots_CompressionAlgorithmClass.m_ZLIB = newVal; }
public:
    REALLY_INLINE avmplus::String* get_DEFLATE() const { return m_slots_CompressionAlgorithmClass.m_DEFLATE; }
    REALLY_INLINE void setconst_DEFLATE(avmplus::String* newVal) { m_slots_CompressionAlgorithmClass.m_DEFLATE = newVal; }
private:
    avmplus::NativeID::avmplus_CompressionAlgorithmClassSlots m_slots_CompressionAlgorithmClass;
protected:
    inline explicit CompressionAlgorithmClass(VTable* cvtable) : avmplus::ClassClosure(cvtable) { createVanillaPrototype(); }
private:
    explicit CompressionAlgorithmClass(const CompressionAlgorithmClass&); // unimplemented
    void operator=(const CompressionAlgorithmClass&); // unimplemented
};

#define avmplus_CompressionAlgorithmClass_isExactInterlock 1
}

#endif // _H_nativegen_classes_builtin
