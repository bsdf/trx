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
    ClassClosure* Toplevel::findClassInScriptEnv(int class_id, ScriptEnv* env)
    {
        PoolObject* pool = env->abcEnv()->pool();
        Traits* traits = pool->getClassTraits(class_id)->itraits;
        Multiname qname(traits->ns(), traits->name());
        ScriptObject* container = env->finddef(&qname);

        Atom classAtom = getproperty(container->atom(), &qname, container->vtable);
        ClassClosure* cc = (ClassClosure*)AvmCore::atomToScriptObject(classAtom);
        return cc;
    }

    ScriptObject* Toplevel::toPrototype(Atom atom)
    {
        if (!AvmCore::isNullOrUndefined(atom))
        {
            switch (atomKind(atom))
            {
            default:

            case kNamespaceType:
                return namespaceClass()->prototypePtr();

            case kStringType:
                return stringClass()->prototypePtr();

            case kObjectType:
                return AvmCore::atomToScriptObject(atom)->getDelegate();

            case kDoubleType:
            case kIntptrType:
                // ISSUE what about int?
                return numberClass()->prototypePtr();

            case kBooleanType:
                return booleanClass()->prototypePtr();
            }
        }
        else
        {
            // TypeError in ECMA
            throwTypeError(
                (atom == undefinedAtom) ? kConvertUndefinedToObjectError :
                                            kConvertNullToObjectError);
            return NULL;
        }
    }

    // equivalent to ToObject, obj->vtable->traits.  exception if null or undefined.
    Traits* Toplevel::toTraits(Atom atom)
    {
        if (!AvmCore::isNullOrUndefined(atom))
        {
            switch (atomKind(atom))
            {
            case kObjectType:
                return AvmCore::atomToScriptObject(atom)->traits();
            case kNamespaceType:
                return core()->traits.namespace_itraits;
            case kStringType:
                return core()->traits.string_itraits;
            case kBooleanType:
                return core()->traits.boolean_itraits;
            case kIntptrType:
            case kDoubleType:
                // ISSUE what about int?
                return core()->traits.number_itraits;
            }
        }
        else
        {
            // TypeError in ECMA
            GCRef<TypeErrorClass> error = typeErrorClass();
            if( error )
                error->throwError(
                    (atom == undefinedAtom) ? kConvertUndefinedToObjectError :
                                        kConvertNullToObjectError);
            else
                throwVerifyError(kCorruptABCError);
        }
        return NULL;
    }

    // E4X 10.5.1, pg 37
    QNameObject* Toplevel::ToAttributeName(Atom attributeName)
    {
        if (!AvmCore::isNullOrUndefined(attributeName))
        {
            AvmCore* core = this->core();
            switch (atomKind(attributeName))
            {
            case kNamespaceType:
                attributeName = AvmCore::atomToNamespace(attributeName)->getURI()->atom();
                break;
            case kObjectType:
                // check for XML, XMLList, Object, AttributeName, AnyName
                // if XML, toString, then do default string case
                // if XMLList, toString, then do default string case
                // if AttributeName, return the input argument
                // if AnyName, return the result of calling "ToAttributeName(*)"
                // if QName, return attributeName
                // otherwise, do toString, then to default case
                if (AvmCore::isQName(attributeName))
                {
                    QNameObject *q = AvmCore::atomToQName(attributeName);
                    if (q->isAttr())
                        return q;
                    else
                        return QNameObject::create(core->GetGC(), qnameClass(), attributeName, true);
                }
                else
                {
                    attributeName = core->string(attributeName)->atom();
                    break;
                }
                break;
            case kStringType:
                {
                    break;
                }
            case kBooleanType:
            case kIntptrType:
            case kDoubleType:
            default: // number
                throwTypeError(kConvertUndefinedToObjectError);
            }

            return QNameObject::create(core->GetGC(), qnameClass(), attributeName, true);
        }
        else
        {
            throwTypeError(kConvertUndefinedToObjectError);
            return NULL;
        }
    }

    // E4X 10.6.1, page 38
    // This returns a Multiname created from a unqualified generic type.
    // The multiname returned will have one namespace and will be correctly
    // marked as an attribute if input is an attribute
    void Toplevel::ToXMLName(const Atom p, Multiname& m)
    {
        Stringp s = NULL;
        AvmCore* core = this->core();

        if (!AvmCore::isNullOrUndefined(p))
        {
            switch (atomKind(p))
            {
            case kNamespaceType:
                s = AvmCore::atomToNamespace(p)->getURI();
                break;
            case kObjectType:
                {
                    // check for XML, XMLList, Object, AttributeName, AnyName
                    // if XML, toString, then do default string case
                    // if XMLList, toString, then do default string case
                    // if AttributeName, return the input argument
                    // if AnyName, return the result of calling "ToAttributeName(*)"
                    // if QName, return attributeName
                    if (AvmCore::isQName(p))
                    {
                        QNameObject *q = AvmCore::atomToQName(p);

                        m.setAttr(q->isAttr() ? true : false);
                        m.setNamespace(core->newNamespace(q->getURI()));
                        Stringp name = q->get_localName();
                        if (name == core->kAsterisk)
                        {
                            m.setAnyName(); // marks it as an anyName
                        }
                        else
                        {
                            m.setName(name);
                        }
                        return;
                    }
                    else // XML, XMLList or generic object - convert to string
                    {
                        s = core->string(p);
                        break;
                    }
                }
            case kIntptrType:
            case kDoubleType:
            case kStringType:
            case kBooleanType:
                {
                    s = core->string(p);
                    break;
                }
            }
        }
        else
        {
            throwTypeError(kConvertUndefinedToObjectError);
            return;
        }

        // At this point p should be a string atom
        AvmAssert (s != 0);

        // if s is integer, throw TypeError
        // if first character of s is "@", return __toAttributeName (string - @)
        // else, return QName (s)
        if (s->charAt(0) == '@')
        {
            // __toAttributeName minus the @
            Stringp news = s->substring(1, s->length());
            m.setName(core->internString(news));
            m.setAttr();
        }
        else
        {
            m.setName(core->internString(s));
        }

        if (m.getName() == core->kAsterisk)
        {
            m.setAnyName(); // marks it as an anyName
        }

        // NOTE need a public, so use the caller's public
        m.setNamespace(core->findPublicNamespace());
    }

    void Toplevel::CoerceE4XMultiname(const Multiname *m, Multiname &out)
    {
        // This function is used to convert raw string access into correct
        // Multiname types:
        // x["*"]
        // x["@*"]
        // Unqualified anyName types are correct handled in Multiname::matches
        // so we do not edit their namespaces here.  (They should have one
        // namespace which is null according to the E4X spec.)
        //
        // Unqualified regular types have the default XML namespace added to their
        // namespace count here.

        AvmAssert(!m->isRuntime());

        AvmCore *core = this->core();

        if (m->isQName())
        {
            AvmAssert(m->namespaceCount() == 1);
            out.setNamespace(m);
            out.setQName();
        }
        else
        {
            // If we're any namespace, no work required.
            if (m->isAnyNamespace())
            {
                out.setAnyNamespace();
            }
            else
            {
                // search for a match in our nsSet for the defaultNamespace
                Namespace *defaultNs = this->getDefaultNamespace();
                bool bMatch = false;
                for (int i=0, n=m->namespaceCount(); i < n; i++)
                {
                    Namespace *ns = m->getNamespace(i);
                    if (ns && ns->getPrefix() == defaultNs->getPrefix() &&
                        ns->getURI() == defaultNs->getURI() &&
                        ns->getType() == defaultNs->getType())
                    {
                        bMatch = true;
                        break;
                    }
                }

                // For an unqualified reference, we need to add in the default xml namespace
                // since we did not find a match for it in our namespace set
                if (!bMatch)
                {
                    int newNameCount = m->namespaceCount() + 1;
                    NamespaceSet* nsset = NamespaceSet::_create(core->GetGC(), newNameCount);
                    for (int i=0, n=m->namespaceCount(); i < n; i++)
                    {
                        nsset->_initNsAt(i, m->getNamespace(i));
                    }
                    //Stringp s1 = string(getDefaultNamespace()->getPrefix());
                    //Stringp s2 = string(getDefaultNamespace()->getURI());
                    nsset->_initNsAt(newNameCount-1, this->getDefaultNamespace());
                    out.setNsset(nsset);
                }
                else
                {
                    if (m->namespaceCount() > 1)
                        out.setNsset(m->getNsset());
                    else
                        out.setNamespace (m->getNamespace());
                }
            }
        }

        out.setAttr(m->isAttr() ? true : false);

        if (m->isAnyName())
        {
            out.setAnyName();
        }
        else
        {
            Stringp s = m->getName();
            if ((s->length() == 1) && (s->charAt(0) == '*'))
            {
                // Mark is as an "anyName" (name == undefined makes isAnyName true)
                out.setAnyName();
            }
            else if ((s->length() >= 1) && (s->charAt(0) == '@'))
            {
                // If we're already marked as an attribute, we don't want to modify
                // our string in any way.  Degenerative cases where you call:
                // XML.attribute (new QName("@*"))
                if (!out.isAttr())
                {
                    // check for "@*"
                    if ((s->length() == 2) && (s->charAt(1) == '*'))
                        out.setAnyName();
                    else
                        out.setName(core->internString(s->substring(1, s->length())));
                    out.setAttr();
                }
                else
                {
                    if (m->isAnyName())
                        out.setAnyName();
                    else
                        out.setName(m->getName());
                }
            }
            else
            {
                if (m->isAnyName())
                    out.setAnyName();
                else
                    out.setName(m->getName());
            }
        }
    }

    Atom Toplevel::instanceof(Atom atom, Atom ctor)
    {
        AvmCore* core = this->core();
        if (atomKind(ctor) != kObjectType ||
            (!AvmCore::istype(ctor, core->traits.function_itraits) &&
            !AvmCore::istype(ctor, core->traits.class_itraits)))
        {
            throwTypeError(kCantUseInstanceofOnNonObjectError);
        }
        // check for null before tryign to call toPrototype(atom), which will throw an error for null.
        if (AvmCore::isNull(atom))
            return falseAtom;

        ClassClosure* c = (ClassClosure*)AvmCore::atomToScriptObject(ctor);

        ScriptObject *proto = c->prototypePtr();
        ScriptObject *o = toPrototype(atom);

        for (; o != NULL; o = o->getDelegate())
        {
            if (o == proto)
                return trueAtom;
        }

        return falseAtom;
    }


    Traits* Toplevel::toClassITraits(Atom atom)
    {
        if (!AvmCore::isObject(atom)) // isObject(null) => false
        {
            // TypeError in ECMA
            // ISSUE the error message should say "whatever" is not a class
            throwTypeError(
                       (atom == undefinedAtom) ? kConvertUndefinedToObjectError :
                                            kConvertNullToObjectError);
        }

        Traits* itraits = AvmCore::atomToScriptObject(atom)->traits()->itraits;
        if (itraits == NULL)
            throwTypeError(kIsTypeMustBeClassError);
        return itraits;
    }

    Atom Toplevel::in_operator(Atom nameatom, Atom obj)
    {
        AvmCore* core = this->core();
        Traits* t = this->toTraits(obj); // toTraits(null) => exception

        bool has_interned = false;
        if (!AvmCore::isDictionaryLookup(nameatom, obj))
        {
            Stringp name;

            if (avmplus::atomIsIntptr(nameatom) &&
                avmplus::atomCanBeUint32(nameatom))
            {
                ScriptObject* o = (atomKind(obj) == kObjectType) ?
                    AvmCore::atomToScriptObject(obj) :
                    this->toPrototype(obj);
                return o->hasUintProperty((uint32_t)atomGetIntptr(nameatom)) ?
                    trueAtom :
                    falseAtom;
            }

            name = core->intern(nameatom);
            has_interned = true;

            // ISSUE should we try this on each object on the proto chain or just the first?
            TraitsBindingsp td = t->getTraitsBindings();
            // NOTE looking for a name that matches the caller's version, so use caller's public
            if (td->findBinding(name, core->findPublicNamespace()) != BIND_NONE)
                return trueAtom;

            nameatom = name->atom();
        }

        ScriptObject* o = atomKind(obj)==kObjectType ?
                AvmCore::atomToScriptObject(obj) :
                this->toPrototype(obj);
        do
        {
            if (o->hasAtomProperty(nameatom))
                return trueAtom;

            // First hasAtomProperty call can be a dictionary and take any
            // type of object but the rest need an interned string.
            if (!has_interned)
            {
                nameatom=core->intern(nameatom)->atom();
                has_interned = true;
            }
        }
        while ((o = o->getDelegate()) != NULL);
        return falseAtom;
    }

    bool Toplevel::hasproperty(Atom obj, const Multiname* multiname, VTable* vtable)
    {
        const Binding propBinding = getBinding(vtable->traits, multiname);
        bool result = false;
        switch (AvmCore::bindingKind(propBinding))
        {
        case BKIND_NONE:
            {
                if (multiname->isValidDynamicName())
                {
                    // Property does not have binding.  Might be a dynamic property.
                    // Have to walk the prototype chain.
                    const ScriptObject* curObj = AvmCore::isObject(obj) ?
                                                    AvmCore::atomToScriptObject(obj) :
                                                    toPrototype(obj);

                    // Walk the prototype chain looking for the property.
                    while ((!result) && (curObj != NULL))
                    {
                        result = curObj->hasMultinameProperty(multiname);
                        curObj = curObj->getDelegate();
                    }
                }
            }
            break;
        case BKIND_METHOD:  // "property" is really a method.
        case BKIND_VAR:     // "property" is a member variable.
        case BKIND_CONST:   // "property" is a member constant.
        case BKIND_GET:     // "property" is implemented with getter
        case BKIND_SET:     // "property" is implemented with setter
        case BKIND_GETSET:  // "property" is implemented with getter and setter
            result = true;
            break;
        default:
            AvmAssert(false);
        }
        return result;
    }

    bool Toplevel::deleteproperty(Atom obj, const Multiname* multiname, VTable* vtable) const
    {
        const Binding propBinding = getBinding(vtable->traits, multiname);
        bool result = false;
        switch (AvmCore::bindingKind(propBinding))
        {
        case BKIND_NONE:
            {
                // Property does not have binding.  See if a dynamic property
                // with the specified name can be set on the object.
                // For it to be legal to set a dynamic property on the
                // object in question, the object must be a regular ( not a primitive )
                // script object, the property name must be a legal dynamic property name,
                // and the object must not be sealed ( must be a dynamic object ).
                if (AvmCore::isObject(obj))
                {
                    ScriptObject* o = AvmCore::atomToScriptObject(obj);
                    if (multiname->isValidDynamicName() && o->traits()->needsHashtable())
                    {
                        result = o->deleteMultinameProperty(multiname);
                    }
                }
            }
            break;

        case BKIND_METHOD:
            {
                if (AvmCore::isXMLorXMLList(obj) && multiname->containsAnyPublicNamespace())
                {
                    ScriptObject* o = AvmCore::atomToScriptObject(obj);
                    // dynamic props should hide declared methods
                    result = o->deleteMultinameProperty(multiname);
                }
            }
            break;

        case BKIND_VAR:
        case BKIND_CONST:
        case BKIND_GET:
        case BKIND_SET:
        case BKIND_GETSET:
            result = false;
            break;
        default:
            AvmAssert(false);
        }
        return result;
    }

    Atom Toplevel::getproperty(Atom obj, const Multiname* multiname, VTable* vtable)
    {
        Binding b = getBinding(vtable->traits, multiname);
        switch (AvmCore::bindingKind(b))
        {
        case BKIND_METHOD:
        {
            if (AvmCore::isXMLorXMLList(obj) && multiname->containsAnyPublicNamespace())
            {
                // dynamic props should hide declared methods
                ScriptObject* so = AvmCore::atomToScriptObject(obj);
                return so->getMultinameProperty(multiname);
            }
            // extracting a method
            MethodEnv *m = vtable->methods[AvmCore::bindingToMethodId(b)];
            return methodClosureClass()->create(m, obj)->atom();
        }

        case BKIND_VAR:
        case BKIND_CONST:
        {
            int slot = AvmCore::bindingToSlotId(b);
            return AvmCore::atomToScriptObject(obj)->getSlotAtom(slot);
        }

        case BKIND_NONE:
            if (atomKind(obj) == kObjectType)
            {
                // try dynamic lookup on instance.  even if the traits are sealed,
                // we might need to search the prototype chain
                return AvmCore::atomToScriptObject(obj)->getMultinameProperty(multiname);
            }
            else
            {
                // primitive types are not dynamic, so we can go directly
                // to their __proto__ object.  but they are sealed, so fail if
                // the property is not found on the proto chain.

                if (multiname->isValidDynamicName())
                {
                    ScriptObject* delegate = toPrototype(obj);
                    return delegate->ScriptObject::getStringPropertyFromProtoChain(multiname->getName(), delegate, toTraits(obj));
                }
                else
                {
                    throwReferenceError(kReadSealedError, multiname, toTraits(obj));
                    return undefinedAtom;
                }
            }

        case BKIND_GET:
        case BKIND_GETSET:
        {
            // Invoke the getter
            int m = AvmCore::bindingToGetterId(b);
            MethodEnv *f = vtable->methods[m];
            return f->coerceEnter(obj);
        }
        case BKIND_SET:
        {
            // read on write-only property
            throwReferenceError(kWriteOnlyError, multiname, vtable->traits);
        }

        default:
            // internal error
            AvmAssert(false);
            return undefinedAtom;
        }
    }

    void Toplevel::setproperty(Atom obj, const Multiname* multiname, Atom value, VTable* vtable) const
    {
        Binding b = getBinding(vtable->traits, multiname);
        setproperty_b(obj,multiname,value,vtable,b);
    }

    void Toplevel::setproperty_b(Atom obj, const Multiname* multiname, Atom value, VTable* vtable, Binding b) const
    {
        switch (AvmCore::bindingKind(b))
        {
        case BKIND_METHOD:
        {
            if (AvmCore::isXMLorXMLList(obj) && multiname->containsAnyPublicNamespace())
            {
                // dynamic props should hide declared methods
                ScriptObject* so = AvmCore::atomToScriptObject(obj);
                so->setMultinameProperty(multiname, value);
                return;
            }
            // trying to assign to a method.  error.
            throwReferenceError(kCannotAssignToMethodError, multiname, vtable->traits);
        }

        case BKIND_CONST:
        {
            // OP_setproperty can never set a const.  initproperty must be used
            throwReferenceError(kConstWriteError, multiname, vtable->traits);
            return;
        }
        case BKIND_VAR:
        {
            AvmCore::atomToScriptObject(obj)->coerceAndSetSlotAtom(AvmCore::bindingToSlotId(b), value);
            return;
        }

        case BKIND_SET:
        case BKIND_GETSET:
        {
            // Invoke the setter
            uint32_t m = AvmCore::bindingToSetterId(b);
            AvmAssert(m < vtable->traits->getTraitsBindings()->methodCount);
            MethodEnv* method = vtable->methods[m];
            Atom atomv_out[2] = { obj, value };
            method->coerceEnter(1, atomv_out);
            return;
        }
        case BKIND_GET:
        {
            throwReferenceError(kConstWriteError, multiname, vtable->traits);
            return;
        }

        case BKIND_NONE:
        {
            if (AvmCore::isObject(obj))
            {
                AvmCore::atomToScriptObject(obj)->setMultinameProperty(multiname, value);
            }
            else
            {
                // obj represents a primitive Number, Boolean, int, or String, and primitives
                // are sealed and final.  Cannot add dynamic vars to them.

                // property could not be found and created.
                throwReferenceError(kWriteSealedError, multiname, vtable->traits);
            }
            return;
        }

        default:
            // internal error if we get here
            AvmAssert(false);
        }
    }

    // E4X 12.1.1, pg 59
    Namespace* Toplevel::getDefaultNamespace()
    {
        // Walking the scope chain now would require a pointer into the local
        // variable space of the currently executing function.  Instead we save/
        // restore the defaultNamespace location as we enter/leave methods, so we
        // always can get to the current value.
        AvmCore* core = this->core();
        Namespace* dxns = core->dxns();
        if (!dxns)
            throwTypeError(kNoDefaultNamespaceError);
        return dxns;
    }

    Binding Toplevel::getBindingAndDeclarer(Traits* traits, const Multiname& ref, Traitsp& declarer) const
    {
        Binding b = BIND_NONE;
        if (traits && ref.isBinding())
        {
            b = traits->getTraitsBindings()->findBindingAndDeclarer(ref, declarer);
            if (b == BIND_AMBIGUOUS)
            {
                // ERROR.  more than one binding is available.  throw exception.
                throwTypeError(kAmbiguousBindingError, core()->toErrorString(ref));
            }
        }
        return b;
    }

    Stringp Toplevel::decodeURI(ScriptObject* self, Stringp uri)
    {
        AvmCore* core = self->core();
        if (!uri) uri = core->knull;
        Stringp out = decode(core, uri, false);
        if (!out) {
            self->toplevel()->uriErrorClass()->throwError(kInvalidURIError, core->toErrorString("decodeURI"));
        }
        return out;
    }

    Stringp Toplevel::decodeURIComponent(ScriptObject* self, Stringp uri)
    {
        AvmCore* core = self->core();
        if (!uri) uri = core->knull;
        Stringp out = decode(core, uri, true);
        if (!out) {
            self->toplevel()->uriErrorClass()->throwError(kInvalidURIError, core->toErrorString("decodeURIComponent"));
        }
        return out;
    }

    Stringp Toplevel::encodeURI(ScriptObject* self, Stringp uri)
    {
        AvmCore* core = self->core();
        if (!uri) uri = core->knull;
        Stringp out = encode(core, uri, false);
        if (!out) {
            self->toplevel()->uriErrorClass()->throwError(kInvalidURIError, core->toErrorString("encodeURI"));
        }
        return out;
    }

    Stringp Toplevel::encodeURIComponent(ScriptObject* self, Stringp uri)
    {
        AvmCore* core = self->core();
        if (!uri) uri = core->knull;
        Stringp out = encode(core, uri, true);
        if (!out) {
            self->toplevel()->uriErrorClass()->throwError(kInvalidURIError, core->toErrorString("encodeURIComponent"));
        }
        return out;
    }

    bool Toplevel::isNaN(ScriptObject*, double n)
    {
        return MathUtils::isNaN(n);
    }

    /**
     * isFinite(number) in section 15.1.2.5 of ecma 262 edition 3
     *
     * Applies ToNumber to argument then returns false if the result is NaN, Negative
     * Infinity, or Positive Infinity, and true otherwise
     *
     * Special case - if isFinite is called with no args, should return false based on
     * section 10.1.3 that states if a function is called with less arguments then
     * params, the variables get assigned 'undefined'.  ToNumber(undefined) returns NaN
     *
     * @return true if arg is Finite, false otherwise
     */
    bool Toplevel::isFinite(ScriptObject*, double d)
    {
        return !(MathUtils::isInfinite(d)||MathUtils::isNaN(d));
    }

    double Toplevel::parseInt(ScriptObject* self, Stringp in, int radix)
    {
        AvmCore* core = self->core();
        if (!in) in = core->knull;
        double n = MathUtils::parseInt(in, radix, false);
        return n;
    }

    double Toplevel::parseFloat(ScriptObject* self, Stringp in)
    {
        double result;

        AvmCore* core = self->core();
        if (!in) in = core->knull;
        if (!MathUtils::convertStringToDouble(in, &result, false))
            result = MathUtils::kNaN;

        return result;
    }

    Stringp Toplevel::escape(ScriptObject* self, Stringp in)
    {
        AvmCore* core = self->core();

        if (!in) in = core->knull;

        StringBuffer buffer(core);

        StringIndexer str_idx(in);
        for (int i=0, n=in->length(); i<n; i++) {
            wchar ch = str_idx[i];
            if (contains(unescaped, ch)) {
                buffer << ch;
            } else if (ch & 0xff00) {
                buffer << "%u";
                buffer.writeHexWord(ch);
            } else {
                buffer << '%';
                buffer.writeHexByte((uint8_t)ch);
            }
        }

        return core->newStringUTF8(buffer.c_str());
    }

    Stringp Toplevel::escapeBytes(Stringp input)
    {
        AvmCore* core = this->core();

        StUTF8String inputUTF8(input);
        const uint8_t* src = (const uint8_t*) inputUTF8.c_str();

        StringBuffer buffer(core);

        for (int i=0, n=inputUTF8.length(); i<n; i++) {
            uint8_t ch = src[i];
            if (contains(unescaped, ch)) {
                buffer << (wchar)ch;
            } else {
                buffer << '%';
                buffer.writeHexByte((uint8_t)ch);
            }
        }
        return core->newStringUTF8(buffer.c_str());
    }

    Atom Toplevel::eval(ScriptObject* self, Atom input)
    {
        (void)self;
        (void)input;
#ifdef VMCFG_EVAL
        if (AvmCore::isString(input)) {
            AvmCore* core = self->core();
            CodeContext* codeContext = core->codeContext();
            String* filename = NULL;                        // should be NULL to denote eval code, for now
            NativeInitializer* nativeInitializer = NULL;    // "native" not supported for eval code
            Toplevel* toplevel = self->toplevel();
            String *newsrc = AvmCore::atomToString(input)->appendLatin1("\0", 1);
            ApiVersion apiVersion = core->getApiVersionFromCallStack();
            return core->handleActionSource(newsrc, filename, toplevel, nativeInitializer, codeContext, apiVersion);
        }
#endif // VMCFG_EVAL
        return undefinedAtom;
    }

    // Helper function.
    int Toplevel::parseHexChar(wchar c)
    {
        if ('0' <= c && c <= '9') {
            return c-'0';
        }
        if ('A' <= c && c <= 'F') {
            return c-'A'+10;
        }
        if ('a' <= c && c <= 'f') {
            return c-'a'+10;
        }
        return -1;
    }

    Stringp Toplevel::unescape(ScriptObject* self, Stringp in)
    {
        AvmCore* core = self->core();

        if (!in) in = core->knull;

        Stringp out = core->kEmptyString;
        int32_t pos = 0;
        StringIndexer str(in);
        while (pos < in->length())
        {
            uint32_t ch = str[pos++];
            if (ch == '%')
            {
                int32_t curPos = pos;
                int len = 2;
                if (pos <= (in->length() - 5) && str[pos] == 'u')
                {
                    len = 4;
                    pos++;
                }
                if ((pos + len) <= in->length())
                {
                    ch = 0;
                    while (len--)
                    {
                        int v = parseHexChar((wchar) str[pos++]);
                        if (v < 0)
                        {
                            pos = curPos;
                            ch = '%';
                            break;
                        }
                        ch = (uint32_t)((ch<<4) | v);
                    }
                }
            }
            wchar ch16 = (wchar) ch;
            // note: this code is allowed to construct a string
            // containing illegal UTF16 sequences!
            out = out->append16(&ch16, 1);
        }
        return out;
    }

    Stringp Toplevel::encode(AvmCore* core, Stringp in, bool encodeURIComponentFlag)
    {
        StringBuffer out(core);

        StUTF16String in16(in);
        const wchar *src = in16.c_str();
        int len = in->length();
        
        bool bugzilla609416 = core->currentBugCompatibility()->bugzilla609416;

        while (len--) {
            wchar ch = *src;

            if (contains(uriUnescaped, ch) ||
                (!encodeURIComponentFlag &&
                 contains(uriReservedPlusPound, ch)))
            {
                out << (char)ch;
                src++;
            } else {
                if (ch >= 0xDC00 && ch <= 0xDFFF) {
                    return NULL;
                }
                uint32_t V;
                if (ch >= 0xD800 && ch < 0xDC00) {
                    if (src[1] < 0xDC00 || src[1] > 0xDFFF) {
                        return NULL;
                    }
                    if (bugzilla609416)
                    {
                        V = (ch - 0xD800) * 0x400 + (src[1] - 0xDC00) + 0x10000;
                        src += 2;
                        len--; // need to reduce the len to account for the surrogate byte we just read
                    }
                    else
                    {
                        V = (ch - 0xD800) * 0x400 + (src[1] - 0xDC00) * 0x10000;
                        src += 2;
                    }
                } else {
                    V = ch;
                    src++;
                }
                uint8_t Octets[6];
                int OctetsLen = UnicodeUtils::Ucs4ToUtf8(V, Octets);
                if (!OctetsLen) {
                    return NULL;
                }
                for (int i=0; i<OctetsLen; i++) {
                    out << '%';
                    out.writeHexByte(Octets[i]);
                }
            }
        }
        return core->newStringUTF8(out.c_str());
    }

    Stringp Toplevel::decode(AvmCore* core, Stringp in, bool decodeURIComponentFlag)
    {
        StringIndexer chars(in);
        int length = in->length();
        wchar *out = (wchar*) core->GetGC()->Alloc(length*2+1); // decoded result is at most length wchar chars long
        int outLen = 0;
        
        bool bugzilla609416 = core->currentBugCompatibility()->bugzilla609416;

        for (int k = 0; k < length; k++) {
            wchar C = chars[k];
            if (C == '%') {
                int start = k;
                if ((k + 2) >= length) {
                    return NULL;
                }
                int v1 = parseHexChar(chars[k+1]);
                if (v1 == -1) {
                    return NULL;
                }
                int v2 = parseHexChar(chars[k+2]);
                if (v2 == -1) {
                    return NULL;
                }
                k += 2;
                uint8_t B = (uint8_t)((v1<<4) | v2);
                uint32_t V;
                if (!(B & 0x80)) {
                    V = (wchar)B;
                } else {
                    // 13. Let n be the smallest non-negative number
                    //     such that (B << n) & 0x80 is equal to 0.
                    int n = 1;
                    while (((B<<n) & 0x80) != 0) {
                        n++;
                    }

                    // 14. If n equals 1 or n is greater than 4,
                    //     throw a URIError exception.
                    if (n == 1 || n > 4) {
                        return NULL;
                    }
                    uint8_t Octets[4];
                    Octets[0] = B;
                    if (k + 3*(n-1) >= length) {
                        return NULL;
                    }
                    for (int j=1; j<n; j++) {
                        k++;
                        if (chars[k] != '%') {
                            return NULL;
                        }
                        int v1 = parseHexChar(chars[k+1]);
                        if (v1 == -1) {
                            return NULL;
                        }
                        int v2 = parseHexChar(chars[k+2]);
                        if (v2 == -1) {
                            return NULL;
                        }
                        B = (uint8_t)((v1<<4) | v2);

                        // 23. If the two most significant bits
                        //     in B are not 10, throw a URIError exception.
                        if ((B&0xC0) != 0x80) {
                            return NULL;
                        }

                        k += 2;
                        Octets[j] = B;
                    }

                    // 29. Let V be the value obtained by applying the UTF-8 transformation
                    // to Octets, that is, from an array of octets into a 32-bit value.
                    if (!UnicodeUtils::Utf8ToUcs4(Octets, n, &V, bugzilla609416)) {
                        return NULL;
                    }
                }

                if (V < 0x10000) {
                    // Check for reserved set
                    if (!decodeURIComponentFlag) {
                        if (contains(uriReservedPlusPound, V)) {
                            while (start <= k) {
                                out[outLen++] = chars[start++];
                            }
                        } else {
                            out[outLen++] = (wchar)V;
                        }
                    } else {
                        out[outLen++] = (wchar)V;
                    }
                } else {
                    // 31. If V is greater than 0x10FFFF, throw a URIError exception.
                    if (V > 0x10FFFF) {
                        return NULL;
                    }
                    // 32. Let L be (((V - 0x10000) & 0x3FF) + 0xDC00).
                    // 33. Let H be ((((V - 0x10000) >> 10) & 0x3FF) + 0xD800).
                    uint32_t L = (((V - 0x10000) & 0x3FF) + 0xDC00);
                    uint32_t H = ((((V - 0x10000) >> 10) & 0x3FF) + 0xD800);
                    out[outLen++] = (wchar)H;
                    out[outLen++] = (wchar)L;
                }
            } else {
                out[outLen++] = C;
            }
        }

        return core->newStringUTF16(out, outLen);
    }

    /*
     * uriUnescaped is defined in Section 15.1 of the ECMA-262 specification
     */
    const uint32_t Toplevel::uriUnescaped[] = {
        0x00000000,
        0x03ff6782,
        0x87fffffe,
        0x47fffffe
    };

    /*
     * uriReserved is defined in Section 15.1 of the ECMA-262 specification
     * The '#' sign is added in accordance with the definition of
     * the encodeURI/decodeURI functions
     */
    const uint32_t Toplevel::uriReservedPlusPound[] = {
        0x00000000,
        0xac009858,
        0x00000001,
        0x00000000
    };

    /**
     * unescaped is a bitmap representing the set of 69 nonblank
     * characters defined in ECMA-262 Section B.2.1 for the
     * escape top-level function
     */
    const uint32_t Toplevel::unescaped[] = {
        0x00000000,
        0x03ffec00,
        0x87ffffff,
        0x07fffffe
    };

    bool Toplevel::isXMLName(ScriptObject* self, Atom v)
    {
        return self->core()->isXMLName(v);
    }

    unsigned int Toplevel::readU30(const uint8_t *&p) const
    {
        unsigned int result = AvmCore::readU32(p);
        if (result & 0xc0000000)
            throwVerifyError(kCorruptABCError);
        return result;
    }

#ifdef VMCFG_VERIFYALL
#   define VERIFYFAILED(core, msg) verifyFailed(core, msg)
    void verifyFailed(AvmCore* core, String* msg)
    {
        if (core->config.verifyonly) {
            core->console << "VERIFY FAILED: " << msg << '\n';
            exit(1);
        }
    }
#else
#   define VERIFYFAILED(core, msg)
#endif

    void Toplevel::throwVerifyError(int id) const
    {
        VERIFYFAILED(core(), core()->formatErrorMessageV(id));
        verifyErrorClass()->throwError(id);
    }

#ifdef DEBUGGER
    void Toplevel::throwVerifyError(int id, Stringp arg1) const
    {
        VERIFYFAILED(core(), core()->formatErrorMessageV(id, arg1));
        verifyErrorClass()->throwError(id, arg1);
    }

    void Toplevel::throwVerifyError(int id, Stringp arg1, Stringp arg2) const
    {
        VERIFYFAILED(core(), core()->formatErrorMessageV(id, arg1, arg2));
        verifyErrorClass()->throwError(id, arg1, arg2);
    }
#endif

    void Toplevel::throwVerifyError(int id, Stringp arg1, Stringp arg2, Stringp arg3) const
    {
        VERIFYFAILED(core(), core()->formatErrorMessageV(id, arg1, arg2, arg3));
        verifyErrorClass()->throwError(id, arg1, arg2, arg3);
    }

    void Toplevel::throwTypeError(int id) const
    {
        typeErrorClass()->throwError(id);
    }

    void Toplevel::throwTypeError(int id, Stringp arg1) const
    {
        typeErrorClass()->throwError(id, arg1);
    }

    void Toplevel::throwTypeError(int id, Stringp arg1, Stringp arg2) const
    {
        typeErrorClass()->throwError(id, arg1, arg2);
    }

    void Toplevel::throwTypeErrorWithName(int id, const char* namestr) const
    {
        // The construction of the multiname and the resulting error string is
        // delegated to this function because in-lining it in the caller prevents
        // taill calling.  See comments in op_call, above.

        Multiname name(core()->findPublicNamespace(), core()->internString(core()->newStringLatin1(namestr)));
        throwTypeError(id, core()->toErrorString(&name));
    }

    void Toplevel::throwError(int id) const
    {
        errorClass()->throwError(id);
    }

    void Toplevel::throwError(int id, Stringp arg1) const
    {
        errorClass()->throwError(id, arg1);
    }

    void Toplevel::throwError(int id, Stringp arg1, Stringp arg2) const
    {
        errorClass()->throwError(id, arg1, arg2);
    }

    void Toplevel::throwArgumentError(int id) const
    {
        argumentErrorClass()->throwError(id);
    }

    void Toplevel::throwArgumentError(int id, Stringp arg1) const
    {
        argumentErrorClass()->throwError(id, arg1);
    }

    void Toplevel::throwArgumentError(int id, const char *s) const
    {
        argumentErrorClass()->throwError(id, core()->toErrorString(s));
    }

    void Toplevel::throwArgumentError(int id, Stringp arg1, Stringp arg2) const
    {
        argumentErrorClass()->throwError(id, arg1, arg2);
    }

    void Toplevel::throwRangeError(int id) const
    {
        rangeErrorClass()->throwError(id);
    }

    void Toplevel::throwRangeError(int id, Stringp arg1) const
    {
        rangeErrorClass()->throwError(id, arg1);
    }

    void Toplevel::throwRangeError(int id, Stringp arg1, Stringp arg2) const
    {
        rangeErrorClass()->throwError(id, arg1, arg2);
    }

    void Toplevel::throwRangeError(int id, Stringp arg1, Stringp arg2, Stringp arg3) const
    {
        rangeErrorClass()->throwError(id, arg1, arg2, arg3);
    }

    void Toplevel::throwReferenceError(int id, const Multiname* multiname, const Traits* traits) const
    {
        referenceErrorClass()->throwError(id, core()->toErrorString(multiname), core()->toErrorString((Traits*)traits));
    }

    void Toplevel::throwReferenceError(int id, const Multiname* multiname) const
    {
        referenceErrorClass()->throwError(id, core()->toErrorString(multiname));
    }

    void FASTCALL Toplevel::throwNullPointerError(const char* name)
    {
        throwTypeErrorWithName(kNullPointerError, name);
    }

    void FASTCALL Toplevel::throwMemoryError(int id)
    {
        memoryErrorClass()->throwError(id);
    }

    void FASTCALL Toplevel::throwIOError(int id)
    {
        ioErrorClass()->throwError(id);
    }

    void FASTCALL Toplevel::throwEOFError(int id)
    {
        eofErrorClass()->throwError(id);
    }

    void Toplevel::throwReferenceError(int id, String* name, const Traits* traits) const
    {
        Multiname mn(core()->findPublicNamespace(), name);
        referenceErrorClass()->throwError(id, core()->toErrorString(mn), core()->toErrorString(traits));
    }

    void Toplevel::throwReferenceError(int id, String* name) const
    {
        Multiname mn(core()->findPublicNamespace(), name);
        referenceErrorClass()->throwError(id, core()->toErrorString(mn));
    }

    // virtual
    bool Toplevel::sampler_trusted(ScriptObject* /*sampler*/)
    {
        return true;
    }

    // Toplevel::bugzilla implements a Script, not a Method, so "self" is a plain ScriptObject, not a Toplevel
    /*static*/ bool Toplevel::bugzilla(ScriptObject* self, int32_t bug)
    {
        const BugCompatibility* bugCompatibility = self->core()->currentBugCompatibility();

        // This is ad-hoc, we only need to introduce cases here for
        // bugs that are actually worked around in AS3 code.
#define HANDLE_BUG(n) \
            case n: return bugCompatibility->bugzilla##n;

        switch (bug)
        {
            HANDLE_BUG(504525)
            HANDLE_BUG(574600)
            HANDLE_BUG(661330)
            default:        AvmAssert(0); return false;
        }
#undef HANDLE_BUG

    }

    //  -------------------------------------------------------
    uint32_t Toplevel::charsetToCodepage(String* /*charset*/)
    {
        return 0;
    }

    String* Toplevel::readMultiByte(uint32_t /*codepage*/, uint32_t /*length*/, DataInput* /*input*/)
    {
        AvmAssert(0);
        return core()->kEmptyString;
    }

    void Toplevel::writeMultiByte(uint32_t /*codepage*/, String* /*str*/, DataOutput* /*output*/)
    {
        AvmAssert(0);
    }

    //  -------------------------------------------------------
    String* Toplevel::tryFromSystemCodepage(const uint8_t* /*data*/)
    {
        return NULL;
    }

    //  -------------------------------------------------------
    Atom Toplevel::readObject(ObjectEncoding /*encoding*/, DataInput* /*input*/)
    {
        throwArgumentError(kInvalidArgumentError, "objectEncoding");
        return undefinedAtom;
    }

    void Toplevel::writeObject(ObjectEncoding /*encoding*/, DataOutput* /*output*/, Atom /*a*/)
    {
        throwArgumentError(kInvalidArgumentError, "objectEncoding");
    }

    //  -------------------------------------------------------

    void Toplevel::byteArrayCreated(ByteArrayObject* /*byteArrayObject*/)
    {
    }
}
