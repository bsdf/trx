#!/usr/bin/env python
# -*- Mode: Python; indent-tabs-mode: nil -*-
# vi: set ts=4 sw=4 expandtab:
# ***** BEGIN LICENSE BLOCK *****
# Version: MPL 1.1/GPL 2.0/LGPL 2.1
#
# The contents of this file are subject to the Mozilla Public License Version
# 1.1 (the "License"); you may not use this file except in compliance with
# the License. You may obtain a copy of the License at
# http://www.mozilla.org/MPL/
#
# Software distributed under the License is distributed on an "AS IS" basis,
# WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
# for the specific language governing rights and limitations under the
# License.
#
# The Original Code is [Open Source Virtual Machine].
#
# The Initial Developer of the Original Code is
# Adobe System Incorporated.
# Portions created by the Initial Developer are Copyright (C) 2007
# the Initial Developer. All Rights Reserved.
#
# Contributor(s):
#   Adobe AS3 Team
#
# Alternatively, the contents of this file may be used under the terms of
# either the GNU General Public License Version 2 or later (the "GPL"), or
# the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
# in which case the provisions of the GPL or the LGPL are applicable instead
# of those above. If you wish to allow use of your version of this file only
# under the terms of either the GPL or the LGPL, and not to allow others to
# use your version of this file under the terms of the MPL, indicate your
# decision by deleting the provisions above and replace them with the notice
# and other provisions required by the GPL or the LGPL. If you do not delete
# the provisions above, a recipient may use your version of this file under
# the terms of any one of the MPL, the GPL or the LGPL.
#
# ***** END LICENSE BLOCK *****

# Documentation for native annotations.
#
# Classes can be annotated with 'native' metainformation:
#
#   [native(cls="...", ...)]
#   public class Fnord extends FnordBase { ... }
#
# or
#
#   [native("...")]
#   public class Fnord extends FnordBase { ... }
#
# The latter form is equivalent to the former with a single cls attribute (?).
#
# The named attributes are:
#
#   cls             the C++ name for the class object's class (required)
#   instance        the C++ name for the instance object's class (required)
#   gc              the GC method for both class and instance objects
#   classgc         the GC method for the class object
#   instancegc      the GC method for the instance object
#   methods         ?
#   construct       indicates that the class has a nonstandard construction pipeline:
#
#                   "none": the class cannot be instantiated, nor can subclasses; we will generate a stub
#                       that throws kCantInstantiateError. (Typically used for classes that provide only 
#                       static members and/or static methods.)
#
#                   "check": the class is expected to provide a function of the form
#
#                           static void FASTCALL preCreateInstanceCheck(ClassClosure* cls)
#
#                       which will be called prior to creation of an instance; this function is
#                       allowed to throw an exception to prevent object creation. (Throwing
#                       an exception in the C++ ctor is discouraged, as our current longjmp implementation
#                       of exception handling can result in a half-constructed object, which could cause
#                       issues at object destruction time.)
#
#                   "restricted": subclasses must be defined in the same ABC chunk as the baseclass (otherwise they will 
#                       throw kCantInstantiateError). This is an odd beast, used by Flash for classes 
#                       which can't be subclassed by user code (only by builtin code.)
#
#                   "restricted-check": Like restricted, but with a preCreateInstanceCheck function required as well.
#
#                   "abstract": the class cannot be instantiated, but its subclasses can (ie, an Abstract Base Class);
#                       we will generate a stub that throws kCantInstantiateError when appropriate.
#
#                   "abstract-restricted": is the union of of abstract+restricted: the class can't be instantiated,
#                       and subclasses can only from from the same ABC chunk.
#
#                   "native": the class can be instantiated only from the C++ constructObject() wrapper; 
#                       attempting to instantiate from AS3 will throw kCantInstantiateError.
#
#                   * THE FOLLOWING VALUES FOR construct ARE ONLY LEGAL FOR USE IN THE BASE VM BUILTINS * 
#
#                   "override": the Class must provide an override of the ScriptObject::construct() method.
#                       (We will autogenerate a createInstanceProc stub that asserts if called.) 
#
#                   "instance": the Class *and* Instance must provide an override of the ScriptObject::construct() method.
#                       (We will autogenerate a createInstanceProc stub that asserts if called.) 
#
#   instancebase    if the instance inherits from a C++ class that can't be inferred from the AS3 inheritance,
#                   it must be specified here. this is not supported for new code; it's provided for temporary support
#                   of existing code.
#
# The allowable values for "gc", "classgc", and "instancegc" are "exact"
# and "conservative"; the default is "conservative".  If the value is "exact" 
# then tracers are autogenerated based on annotations on the C++ class and 
# on the pointer fields in the C++ class.
#
# For more information about GC see the documentation in utils/exactgc.as.

import optparse, struct, os, sys, StringIO
from optparse import OptionParser
from struct import *
from os import path
from math import floor
from sys import stderr

import sys, traceback

parser = OptionParser(usage="usage: %prog [importfile [, importfile]...] file...")
parser.add_option("-v", "--thunkvprof", action="store_true", default=False)
parser.add_option("-e", "--externmethodandclassetables", action="store_true", default=False, help="generate extern decls for method and class tables")
parser.add_option("--native-id-namespace", dest="nativeIDNS", default="avmplus::NativeID", help="Specifies the C++ namespace in which all generated should be in.")
parser.add_option("--root-implementation-namespace", dest="rootImplNS", default="avmplus", help="Specifies the C++ namespace in under which all implementation classes can be found.")
opts, args = parser.parse_args()

if not args:
    parser.print_help()
    exit(2)

NEED_ARGUMENTS      = 0x01
NEED_ACTIVATION     = 0x02
NEED_REST           = 0x04
HAS_OPTIONAL        = 0x08
IGNORE_REST         = 0x10
NATIVE              = 0x20
HAS_ParamNames      = 0x80

CONSTANT_Utf8               = 0x01
CONSTANT_Int                = 0x03
CONSTANT_UInt               = 0x04
CONSTANT_PrivateNs          = 0x05
CONSTANT_Double             = 0x06
CONSTANT_Qname              = 0x07
CONSTANT_Namespace          = 0x08
CONSTANT_Multiname          = 0x09
CONSTANT_False              = 0x0A
CONSTANT_True               = 0x0B
CONSTANT_Null               = 0x0C
CONSTANT_QnameA             = 0x0D
CONSTANT_MultinameA         = 0x0E
CONSTANT_RTQname            = 0x0F
CONSTANT_RTQnameA           = 0x10
CONSTANT_RTQnameL           = 0x11
CONSTANT_RTQnameLA          = 0x12
CONSTANT_NameL              = 0x13
CONSTANT_NameLA             = 0x14
CONSTANT_NamespaceSet       = 0x15
CONSTANT_PackageNs          = 0x16
CONSTANT_PackageInternalNs  = 0x17
CONSTANT_ProtectedNs        = 0x18
CONSTANT_ExplicitNamespace  = 0x19
CONSTANT_StaticProtectedNs  = 0x1A
CONSTANT_MultinameL         = 0x1B
CONSTANT_MultinameLA        = 0x1C
CONSTANT_TypeName           = 0x1D

TRAIT_Slot          = 0x00
TRAIT_Method        = 0x01
TRAIT_Getter        = 0x02
TRAIT_Setter        = 0x03
TRAIT_Class         = 0x04
TRAIT_Const         = 0x06
TRAIT_mask          = 15

ATTR_final          = 0x10
ATTR_override       = 0x20
ATTR_metadata       = 0x40

CTYPE_VOID          = 0
CTYPE_ATOM          = 1
CTYPE_BOOLEAN       = 2
CTYPE_INT           = 3
CTYPE_UINT          = 4
CTYPE_DOUBLE        = 5
CTYPE_STRING        = 6
CTYPE_NAMESPACE     = 7
CTYPE_OBJECT        = 8
CTYPE_FLOAT         = 9

MIN_API_MARK = 0xE000
MAX_API_MARK = 0xF8FF

MPL_HEADER = "/* ***** BEGIN LICENSE BLOCK *****\n" \
            " * Version: MPL 1.1/GPL 2.0/LGPL 2.1\n" \
            " *\n" \
            " * The contents of this file are subject to the Mozilla Public License Version\n" \
            " * 1.1 (the \"License\"); you may not use this file except in compliance with\n" \
            " * the License. You may obtain a copy of the License at\n" \
            " * http://www.mozilla.org/MPL/\n" \
            " *\n" \
            " * Software distributed under the License is distributed on an \"AS IS\" basis,\n" \
            " * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License\n" \
            " * for the specific language governing rights and limitations under the\n" \
            " * License.\n" \
            " *\n" \
            " * The Original Code is [Open Source Virtual Machine].\n" \
            " *\n" \
            " * The Initial Developer of the Original Code is\n" \
            " * Adobe System Incorporated.\n" \
            " * Portions created by the Initial Developer are Copyright (C) 2008\n" \
            " * the Initial Developer. All Rights Reserved.\n" \
            " *\n" \
            " * Contributor(s):\n" \
            " *   Adobe AS3 Team\n" \
            " *\n" \
            " * Alternatively, the contents of this file may be used under the terms of\n" \
            " * either the GNU General Public License Version 2 or later (the \"GPL\"), or\n" \
            " * the GNU Lesser General Public License Version 2.1 or later (the \"LGPL\"),\n" \
            " * in which case the provisions of the GPL or the LGPL are applicable instead\n" \
            " * of those above. If you wish to allow use of your version of this file only\n" \
            " * under the terms of either the GPL or the LGPL, and not to allow others to\n" \
            " * use your version of this file under the terms of the MPL, indicate your\n" \
            " * decision by deleting the provisions above and replace them with the notice\n" \
            " * and other provisions required by the GPL or the LGPL. If you do not delete\n" \
            " * the provisions above, a recipient may use your version of this file under\n" \
            " * the terms of any one of the MPL, the GPL or the LGPL.\n" \
            " *\n" \
            " * ***** END LICENSE BLOCK ***** */"

# Python 2.5 and earlier didn't reliably handle float("nan") and friends uniformly
# across all platforms. This is a workaround that appears to be more reliable.
# if/when we require Python 2.6 or later we can use a less hack-prone approach
kPosInf = 1e300000
kNegInf = -1e300000
kNaN = kPosInf / kPosInf

def is_nan(val):
    strValLower = str(val).lower()
    return strValLower == "nan"

def is_pos_inf(val):
    # [-]1.#INF on Windows in Python 2.5.2!
    strValLower = str(val).lower()
    return strValLower.endswith("inf") and not strValLower.startswith("-")

def is_neg_inf(val):
    # [-]1.#INF on Windows in Python 2.5.2!
    strValLower = str(val).lower()
    return strValLower.endswith("inf") and strValLower.startswith("-")

class Error(Exception):
    nm = ""
    def __init__(self, n):
        self.nm = n
    def __str__(self):
        return self.nm

BASE_CLASS_NAME = "avmplus::ClassClosure"
BASE_INSTANCE_NAME = "avmplus::ScriptObject"

TYPEMAP_RETTYPE = {
    CTYPE_OBJECT:       "%s*",
    CTYPE_ATOM:         "avmplus::Atom",
    CTYPE_VOID:         "void",
    CTYPE_BOOLEAN:      "bool",
    CTYPE_INT:          "int32_t",
    CTYPE_UINT:         "uint32_t",
    CTYPE_DOUBLE:       "double",
    CTYPE_FLOAT:        "float",
    CTYPE_STRING:       "avmplus::String*",
    CTYPE_NAMESPACE:    "avmplus::Namespace*",
}

TYPEMAP_RETTYPE_GCREF = {
    CTYPE_OBJECT:       lambda t: "GCRef<%s>" % t.fqcppname(),
    CTYPE_ATOM:         lambda t: "avmplus::Atom",
    CTYPE_VOID:         lambda t: "void",
    CTYPE_BOOLEAN:      lambda t: "bool",
    CTYPE_INT:          lambda t: "int32_t",
    CTYPE_UINT:         lambda t: "uint32_t",
    CTYPE_DOUBLE:       lambda t: "double",
    CTYPE_FLOAT:        lambda t: "float",
    CTYPE_STRING:       lambda t: "GCRef<avmplus::String>",
    CTYPE_NAMESPACE:    lambda t: "GCRef<avmplus::Namespace>",
}

TYPEMAP_THUNKRETTYPE = {
    CTYPE_OBJECT:       "avmplus::Atom(%s)",
    CTYPE_ATOM:         "avmplus::Atom(%s)",
    CTYPE_VOID:         "avmplus::Atom(%s)",
    CTYPE_BOOLEAN:      "avmplus::Atom(%s)",
    CTYPE_INT:          "avmplus::Atom(%s)",
    CTYPE_UINT:         "avmplus::Atom(%s)",
    CTYPE_DOUBLE:       "double(%s)",
    CTYPE_FLOAT:        "float(%s)",
    CTYPE_STRING:       "avmplus::Atom(%s)",
    CTYPE_NAMESPACE:    "avmplus::Atom(%s)",
}

TYPEMAP_MEMBERTYPE = {
    CTYPE_OBJECT:       "MMgc::GCTraceableObject::GCMember<%s>",
    CTYPE_ATOM:         "avmplus::AtomWB",
    CTYPE_VOID:         "#error",
    CTYPE_BOOLEAN:      "avmplus::bool32",
    CTYPE_INT:          "int32_t",
    CTYPE_UINT:         "uint32_t",
    CTYPE_DOUBLE:       "double",
    CTYPE_FLOAT:        "float",
    CTYPE_STRING:       "MMgc::GCTraceableObject::GCMember<avmplus::String>",
    CTYPE_NAMESPACE:    "MMgc::GCTraceableObject::GCMember<avmplus::Namespace>",
}

TYPEMAP_ARGTYPE = {
    CTYPE_OBJECT:       "%s*",
    CTYPE_ATOM:         "avmplus::Atom",
    CTYPE_VOID:         "void",
    CTYPE_BOOLEAN:      "avmplus::bool32",
    CTYPE_INT:          "int32_t",
    CTYPE_UINT:         "uint32_t",
    CTYPE_DOUBLE:       "double",
    CTYPE_FLOAT:        "float",
    CTYPE_STRING:       "avmplus::String*",
    CTYPE_NAMESPACE:    "avmplus::Namespace*",
}

TYPEMAP_ARGTYPE_SUFFIX = {
    CTYPE_OBJECT:       "OBJECT",
    CTYPE_ATOM:         "ATOM",
    CTYPE_VOID:         "VOID",
    CTYPE_BOOLEAN:      "BOOLEAN",
    CTYPE_INT:          "INT",
    CTYPE_UINT:         "UINT",
    CTYPE_DOUBLE:       "DOUBLE",
    CTYPE_FLOAT:        "FLOAT",
    CTYPE_STRING:       "STRING",
    CTYPE_NAMESPACE:    "NAMESPACE",
}

TYPEMAP_ARGTYPE_FOR_UNBOX = {
    CTYPE_OBJECT:       "%s*",
    CTYPE_ATOM:         "avmplus::Atom",
    CTYPE_VOID:         "#error",
    CTYPE_BOOLEAN:      "avmplus::bool32",
    CTYPE_INT:          "int32_t",
    CTYPE_UINT:         "uint32_t",
    CTYPE_DOUBLE:       "double",
    CTYPE_FLOAT:        "float",
    CTYPE_STRING:       "avmplus::String*",
    CTYPE_NAMESPACE:    "avmplus::Namespace*",
}

TYPEMAP_TO_ATOM = {
    # The explicit cast to ScriptObject is necessary because the type in
    # question may have only been forward-declared, thus calling methods
    # on it isn't legal; since we know it's a ScriptObject, we can force the issue.
    # Note that, for this reason, we can't use staticCast, as the compiler
    # will complain that they are apparently unrelated types. 
    CTYPE_OBJECT:       lambda val: "%s.reinterpretCast<avmplus::ScriptObject>()->atom()" % val,
    CTYPE_ATOM:         lambda val: "%s" % val,
    CTYPE_VOID:         lambda val: "undefinedAtom",
    CTYPE_BOOLEAN:      lambda val: "((%s) ? trueAtom : falseAtom)" % val,
    CTYPE_INT:          lambda val: "core->intToAtom(%s)" % val,
    CTYPE_UINT:         lambda val: "core->uintToAtom(%s)" % val,
    CTYPE_DOUBLE:       lambda val: "core->doubleToAtom(%s)" % val,
    CTYPE_FLOAT:        lambda val: "core->floatToAtom(%s)" % val,
    CTYPE_STRING:       lambda val: "%s->atom()" % val,
    CTYPE_NAMESPACE:    lambda val: "%s->atom()" % val,
}

TYPEMAP_TO_ATOM_NEEDS_CORE = {
    CTYPE_OBJECT:       False,
    CTYPE_ATOM:         False,
    CTYPE_VOID:         False,
    CTYPE_BOOLEAN:      False,
    CTYPE_INT:          True,
    CTYPE_UINT:         True,
    CTYPE_DOUBLE:       True,
    CTYPE_FLOAT:        True,
    CTYPE_STRING:       False,
    CTYPE_NAMESPACE:    False,
}

TYPEMAP_ATOM_TO_GCREF = {
    # We can't use static_cast<> because the subclass might be only forward-declared at this point;
    # use good old brute-force cast instead.
    CTYPE_OBJECT:       lambda val,t: "GCRef<%s>((%s*)(avmplus::AvmCore::atomToScriptObject(%s)))" % (t.fqcppname(),t.fqcppname(),val),
    CTYPE_ATOM:         lambda val,t: "%s" % val,
    CTYPE_VOID:         lambda val,t: "avmplus::undefinedAtom",
    CTYPE_BOOLEAN:      lambda val,t: "((%s) != avmplus::falseAtom)" % val,
    CTYPE_INT:          lambda val,t: "avmplus::AvmCore::integer(%s)" % val,
    CTYPE_UINT:         lambda val,t: "avmplus::AvmCore::toUInt32(%s)" % val,
    CTYPE_DOUBLE:       lambda val,t: "avmplus::AvmCore::number(%s)" % val,
    CTYPE_FLOAT:        lambda val,t: "avmplus::AvmCore::singlePrecisionFloat(%s)" % val,
    CTYPE_STRING:       lambda val,t: "GCRef<avmplus::String>(avmplus::AvmCore::atomToString(%s))" % val,
    CTYPE_NAMESPACE:    lambda val,t: "GCRef<avmplus::Namespace>(avmplus::AvmCore::atomToNamespace(%s))" % val,
}


def uint(i):
    return int(i) & 0xffffffff

def c_argtype_from_enum(ct):
    assert ct != CTYPE_OBJECT
    r = TYPEMAP_ARGTYPE[ct]
    return r

def to_cname(nm):
    nm = str(nm)
    nm = nm.replace("+", "_");
    nm = nm.replace("-", "_");
    nm = nm.replace("?", "_");
    nm = nm.replace("!", "_");
    nm = nm.replace("<", "_");
    nm = nm.replace(">", "_");
    nm = nm.replace("=", "_");
    nm = nm.replace("(", "_");
    nm = nm.replace(")", "_");
    nm = nm.replace("\"", "_");
    nm = nm.replace("'", "_");
    nm = nm.replace("*", "_");
    nm = nm.replace(" ", "_");
    nm = nm.replace(".", "_");
    nm = nm.replace("$", "_");
    nm = nm.replace("::", "_");
    nm = nm.replace(":", "_");
    nm = nm.replace("/", "_");
    return nm


GLUECLASSES_WITHOUT_NS = frozenset((
    'bool',
    'double',
    'float',
    'int32_t',
    'uint32_t'))

def ns_prefix(ns, iscls):
    if not ns.isPublic() and not ns.isInternal():
        if ns.isPrivate() and not iscls:
            return "private_";
        if ns.isProtected():
            return "protected_";
        if ns.srcname != None:
            return to_cname(str(ns.srcname)) + "_"
    p = to_cname(ns.uri);
    if len(p) > 0:
        p += "_"
    return p

def stripVersion(uri):
    if len(uri) > 0:
        uri16 = uri.decode('utf8')
        cc = ord(uri16[-1])
        if cc >= MIN_API_MARK and cc <= MAX_API_MARK:
            return cc-MIN_API_MARK, uri16[0:len(uri16)-1].encode('utf8')
    return -1,uri

class Namespace:
    uri = ""
    kind = 0
    srcname = None
    def __init__(self, uri, kind):
        self.uri = uri
        self.kind = kind
    def __str__(self):
        return self.uri
    def isPublic(self):
        return self.kind in [CONSTANT_Namespace, CONSTANT_PackageNs] and self.uri == ""
    def isInternal(self):
        return self.kind in [CONSTANT_PackageInternalNs]
    def isPrivate(self):
        return self.kind in [CONSTANT_PrivateNs]
    def isProtected(self):
        return self.kind in [CONSTANT_ProtectedNs, CONSTANT_StaticProtectedNs]
    def stripVersion(self):
        api, strippeduri = stripVersion(self.uri)
        # it's important to return 'self' (and not an identical clone)
        # if we are unversioned, otherwise native methods with custom namespaces
        # may be emitted incorrectly
        if api < 0:
            return self
        newself = Namespace(strippeduri, self.kind)
        newself.srcname = self.srcname
        return newself

class QName:
    ns = None
    name = ""
    def __init__(self, ns, name):
        self.ns = ns
        self.name = name
    def __str__(self):
        if str(self.ns) == "":
            return self.name
        if self.ns == None:
            return "*::" + self.name
        return str(self.ns) + "::" + self.name

class Multiname:
    nsset = None
    name = ""
    def __init__(self, nsset, name):
        self.nsset = nsset
        self.name = name
    def __str__(self):
        nsStrings = map(lambda ns: '"' + str(ns).decode("utf8") + '"', self.nsset)
        stringForNSSet = '[' + ', '.join(nsStrings) + ']'
        return stringForNSSet + '::' + unicode(self.name.decode("utf8"))

class TypeName:
    name = ""
    types = None
    def __init__(self, name, types):
        self.name = name
        self.types = types
    def __str__(self):
        # @todo horrible special-casing, improve someday
        s = str(self.name)
        t = str(self.types[0])
        if t == "int":
            s += "$int"
        elif t == "uint":
            s += "$uint"
        elif t == "Number":
            s += "$double"
        elif t == "float":
            s += "$float"
        else:
            s += "$object"
        return s

class MetaData:
    name = ""
    attrs = {}
    def __init__(self, name):
        self.name = name
        self.attrs = {}

class MemberInfo:
    id = -1
    kind = -1
    name = ""
    name_index = -1
    metadata = None

class MethodInfo(MemberInfo):
    flags = 0
    debugName = ""
    paramTypes = None
    paramNames = None
    optional_count = 0
    optionalValues = None
    returnType = None
    local_count = 0
    max_scope = 0
    max_stack = 0
    code_length = 0
    code = None
    activation = None
    native_id_name = None
    native_method_name = None
    final = False
    override = False
    receiver = None
    unbox_this = -1 # -1 == undetermined, 0 = no, 1 = yes
    vtable_index = -1

    def isNative(self):
        return (self.flags & NATIVE) != 0

    def needRest(self):
        return (self.flags & NEED_REST) != 0

    def hasOptional(self):
        return (self.flags & HAS_OPTIONAL) != 0

    def assign_names(self, traits, prefix):
        self.receiver = traits

        if not self.isNative():
            return

        if self == traits.init:
            raise Error("ctors cannot be native")

        assert(isinstance(self.name, QName))
        self.native_id_name = prefix + ns_prefix(self.name.ns, False) + self.name.name
        self.native_method_name = self.name.name

        if self.kind == TRAIT_Getter:
            self.native_id_name += "_get"
            self.native_method_name = "get_" + self.native_method_name
        elif self.kind == TRAIT_Setter:
            self.native_id_name += "_set"
            self.native_method_name = "set_" + self.native_method_name

        if self.name.ns.srcname != None:
            self.native_method_name = str(self.name.ns.srcname) + "_" + self.native_method_name

        # if we are an override, prepend the classname to the C method name.
        # (native method implementations must not be virtual, and some compilers
        # will be unhappy if a subclass overrides a method with the same name and signature
        # without it being virtual.) Note that we really only need to do this if the ancestor
        # implementation is native, rather than pure AS3, but we currently do it regardless.
        if self.override:
            self.native_method_name = traits.name.name + "_" + self.native_method_name

        self.native_method_name = to_cname(self.native_method_name)

class SlotInfo(MemberInfo):
    type = ""
    value = ""
    ns = None
    fileOffset = -1

class NativeInfo:
    traits = None
    itraits = None
    class_gc_exact = None
    instancebase_name = None
    instance_gc_exact = None
    method_map_name = None
    construct = None

    def __init__(self, traits, itraits):
        self.traits = traits
        self.itraits = itraits

    def parse_one_nativeinfo(self, attrs, is_vm_builtin):

        for k in attrs.keys():
            v = attrs[k]
            if (k == "script"):
                raise Error("native scripts are no longer supported; please use a native class instead and wrap with AS3 code as necessary.")

            elif (k == "cls"):
                self.set_class(v)
            elif (k == "instance"):
                self.set_instance(v)
            elif (k == "instancebase"):
                self.set_instancebase(v)
            elif (k == "gc"):
                self.set_classGC(v)
                self.set_instanceGC(v)
            elif (k == "classgc"):
                self.set_classGC(v)
            elif (k == "instancegc"):
                self.set_instanceGC(v)
            elif (k == "methods"):
                if v != "auto":
                    self.method_map_name = v
            elif (k == "construct"):
                self.set_construct(v, is_vm_builtin)
            elif (k == "friend"):
                self.set_friend(v)
            else:
                raise Error("unknown attribute native(%s)" % k)
        if (self.traits.cpp_name_comps == None) and (self.itraits.cpp_name_comps == None):
            # it's OK to specify construct="native" for a pure AS3 class
            # it's OK to specify friend="whatever" for a pure AS3 class
            # it's OK to specify gc="whatever" for a pure AS3 class
            if not (self.construct == "native" or self.traits.cpp_friend_classes != None or self.class_gc_exact != None or self.instance_gc_exact != None):
                raise Error("native metadata must specify (cls,instance): %s" % str(attrs))
  
    def fullyQualifiedCPPClassName(self, className):
        r = className.split('::')
        if (len(opts.rootImplNS) > 0) and not className.startswith('::') and not className in GLUECLASSES_WITHOUT_NS:
            r.insert(0, opts.rootImplNS)
        return r

    def set_friend(self, name):
        if self.traits.cpp_friend_classes != None:
            raise Error("native(friend) may not be specified multiple times for the same class: %s %s" % (self.traits.fqcppname(), name))
        self.traits.cpp_friend_classes = name.split(',')
        self.itraits.cpp_friend_classes = name.split(',')

    def set_class(self, name):
        if self.traits.cpp_name_comps != None:
            raise Error("native(cls) may not be specified multiple times for the same class: %s %s" % (self.traits.fqcppname(), name))
        self.traits.cpp_name_comps = self.fullyQualifiedCPPClassName(name)

    def set_instance(self, name):
        if self.itraits.cpp_name_comps != None:
            raise Error("native(instance) may not be specified multiple times for the same class: %s %s" % (self.itraits.fqcppname(), name))
        if name == "ScriptObject" or name == BASE_INSTANCE_NAME:
            raise Error("native(instance='ScriptObject') is no longer supported:")
        self.itraits.cpp_name_comps = self.fullyQualifiedCPPClassName(name)

    def set_instancebase(self, name):
        if self.instancebase_name != None:
            raise Error("native(instancebase) may not be specified multiple times for the same class: %s %s" % (self.instancebase_name, name))
        comps = self.fullyQualifiedCPPClassName(name)
        self.instancebase_name = '::'.join(filter(lambda ns: len(ns) > 0, comps))

    def set_construct(self, value, is_vm_builtin):
        if self.construct != None:
            raise Error("native(construct) may not be specified multiple times for the same class")
        if value in ["override", "instance"]:
            if not is_vm_builtin:
                raise Error('construct=%s may only be specified for the VM builtins' % value)
            self.construct = value
        elif value in ["none", "abstract", "abstract-restricted", "restricted", "check", "restricted-check", "native"]:
            self.construct = value
        else:
            raise Error('native metadata specified illegal value, "%s" for construct field.' % value)
   
    def set_classGC(self, gc):
        if self.class_gc_exact != None:
            raise Error("native(classgc) may not be specified multiple times for the same class: %s %s" % (self.traits.fqcppname(), gc))
        if gc == "exact":
            self.class_gc_exact = True
        elif gc == "conservative":
            self.class_gc_exact = False
        else:
            raise Error("native(classgc) can only be specified as 'exact' or 'conservative': %s %s" % (self.traits.fqcppname(), gc))

    def set_instanceGC(self, gc):
        if self.instance_gc_exact != None:
            raise Error("native(instancegc) may not be specified multiple times for the same class: %s %s" % (self.itraits.fqcppname(), gc))
        if gc == "exact":
            self.instance_gc_exact = True
        elif gc == "conservative":
            self.instance_gc_exact = False
        else:
            raise Error("native(instancegc) can only be specified as 'exact' or 'conservative': %s %s" % (self.itraits.fqcppname(), gc))

    @staticmethod
    def parse_native_info(t, is_vm_builtin):

        itraits = t.itraits
        ni = NativeInfo(t, itraits)

        if t.metadata != None:
            for md in t.metadata:
                if md.name == "native":
                    ni.parse_one_nativeinfo(md.attrs, is_vm_builtin)

        if itraits.is_interface:
            if t.cpp_name_comps != None or itraits.cpp_name_comps != None:
                raise Error("It is not legal to specify native(cls/instance) for an interface")
            # this is a little hacky, but it allows us to assume that all traits have a valid CPP name.
            # since we don't yet surface a C++ wrapper for interfaces, this will have to do.
            t.cpp_name_comps = ni.fullyQualifiedCPPClassName(itraits.name.name + "Class")
            t.is_synthetic = True
            itraits.cpp_name_comps = ni.fullyQualifiedCPPClassName(itraits.name.name + "Interface")
            itraits.is_synthetic = True
            itraits.base = "Object"

        else:
            if t.cpp_name_comps == None and itraits.cpp_name_comps != None:
                raise Error("It is not legal to specify native(instance) without native(cls)")

            if t.cpp_name_comps == None:
                t.cpp_name_comps = ni.fullyQualifiedCPPClassName(itraits.name.name + "Class")
                t.is_synthetic = True
                # if gc metadata not specified, assume we want it exact
                if ni.class_gc_exact == None:
                    ni.class_gc_exact = True

            if itraits.cpp_name_comps == None:
                if str(t.name) == "Object$":
                    # "Object" is special-cased.
                    itraits.cpp_name_comps = BASE_INSTANCE_NAME.split('::')
                else:
                    itraits.cpp_name_comps = ni.fullyQualifiedCPPClassName(itraits.name.name + "Object")
                    itraits.is_synthetic = True
                    # if gc metadata not specified, assume we want it exact
                    if ni.instance_gc_exact == None:
                        ni.instance_gc_exact = True

        # force to True or False (no "None" allowed)
        if ni.class_gc_exact != True:
            ni.class_gc_exact = False;
        if ni.instance_gc_exact != True:
            ni.instance_gc_exact = False;
        t.is_gc_exact = ni.class_gc_exact
        itraits.is_gc_exact = ni.instance_gc_exact

        if ni.construct != None:
            t.construct = ni.construct
            if t.construct in ["override", "instance"]:
                t.has_construct_method_override = True
            if t.construct in ["abstract-restricted", "restricted", "restricted-check"]:
                t.is_restricted_inheritance = True
            if t.construct in ["abstract", "abstract-restricted"]:
                t.is_abstract_base = True
            if t.construct in ["check", "restricted-check"]:
                t.has_pre_create_check = True
            if ni.construct == "instance":
                itraits.construct = "override"
                itraits.has_construct_method_override = True

        if str(t.name) == "Object$":
            # "Object" is special-cased.
            t.createInstanceProcName = "ClassClosure::createScriptObjectProc"

        elif t.construct in ["none", "abstract", "abstract-restricted"]:
            t.createInstanceProcName = "ClassClosure::cantInstantiateCreateInstanceProc"

        elif t.construct in ["override", "instance"] or itraits.ctype != CTYPE_OBJECT:
            t.createInstanceProcName = "ClassClosure::impossibleCreateInstanceProc"

        elif t.construct == "native":
            t.createInstanceProcName = "ClassClosure::cantInstantiateCreateInstanceProc"
            t.has_custom_createInstanceProc = True
            assert t.cpp_name_comps != None

        elif t.cpp_name_comps != None:
            t.createInstanceProcName = "%s::createInstanceProc" % t.fqcppname()
            t.has_custom_createInstanceProc = True
        
        # t.fqinstancebase_name = ni.instancebase_name # not supported for classes
        t.method_map_name = t.fqcppname()  # custom method_map_name never applies to class
        t.slotsStructName = to_cname(t.fqcppname()) + 'Slots'
        t.slotsInstanceName = 'm_slots_' + t.cppname()
        
        assert itraits != None
        itraits.fqinstancebase_name = ni.instancebase_name
        if ni.method_map_name != None:
            itraits.method_map_name = ni.method_map_name
        else:
            itraits.method_map_name = itraits.fqcppname()
        itraits.slotsStructName = to_cname(itraits.fqcppname()) + 'Slots'
        itraits.slotsInstanceName = 'm_slots_' + itraits.cppname()


BMAP = {
    "Object": CTYPE_ATOM, # yes, items of exactly class "Object" are stored as Atom; subclasses are stored as pointer-to-Object
    "null": CTYPE_ATOM,
    "*": CTYPE_ATOM,
    "void": CTYPE_VOID,
    "int": CTYPE_INT,
    "uint": CTYPE_UINT,
    "Number": CTYPE_DOUBLE,
    "Boolean": CTYPE_BOOLEAN,
    "String": CTYPE_STRING,
    "Namespace": CTYPE_NAMESPACE,
    "float": CTYPE_FLOAT
};

class Traits:
    name = ""
    qname = None
    init = None
    itraits = None
    ctraits = None
    base = None
    flags = 0
    protectedNs = 0
    is_sealed = False
    is_final = False
    is_interface = False
    interfaces = None
    names = None
    slots = None
    tmethods = None
    tmethod_index_count = -1
    tmethods_name_map = None
    members = None
    class_id = -1
    ctype = CTYPE_OBJECT
    metadata = None
    cpp_name_comps = None
    cpp_friend_classes = None
    slotsStructName = None
    slotsInstanceName = None
    nextSlotId = 0
    is_gc_exact = False
    construct = None
    createInstanceProcName = None
    # some values for "construct" imply an override to the ScriptObject::construct method,
    # some don't. this simplifies things.
    has_construct_method_override = False
    has_custom_createInstanceProc = False
    is_restricted_inheritance = False
    is_abstract_base = False
    has_pre_create_check = False
    is_synthetic = False
    # FIXME, this is a hack for MI classes in AIR
    fqinstancebase_name = None
    # FIXME, this is a hack for MI classes in AIR
    method_map_name = None

    def __init__(self, name):
        self.names = {}
        self.slots = []
        self.tmethods = []
        self.name = name
        if BMAP.has_key(str(name)):
            self.ctype = BMAP[str(name)]

    def __str__(self):
        return str(self.name)
    
    def cpp_offsetof_slots(self):
        if (len(self.slots) > 0):
            return "offsetof(%s, %s)" % (self.fqcppname(), self.slotsInstanceName)
        else:
            return "0"

    # What's the C++ type to use when declaring this type as a member of a GCObject?
    def cpp_gcmember_name(self):
        r = TYPEMAP_MEMBERTYPE[self.ctype]
        if self.ctype == CTYPE_OBJECT:
            r = r % self.fqcppname()
        return r

    # What's the C++ type to use when declaring this type as an input argument to a C++ method?
    def cpp_argument_name(self):
        r = TYPEMAP_ARGTYPE[self.ctype]
        if self.ctype == CTYPE_OBJECT:
            r = r % self.fqcppname()
        return r

    # What's the C++ type to use when unboxing this type as an input argument to a C++ method?
    def cpp_unboxing_argument_name(self):
        r = TYPEMAP_ARGTYPE_FOR_UNBOX[self.ctype]
        if self.ctype == CTYPE_OBJECT:
            r = r % self.fqcppname()
        return r

    # What's the C++ type to use when returning this as a function result?
    def cpp_return_name(self):
        r = TYPEMAP_RETTYPE[self.ctype]
        if self.ctype == CTYPE_OBJECT:
            r = r % self.fqcppname()
        return r

    # return the fully qualified cpp name, eg "foo::bar::FooObject"
    def fqcppname(self):
        return '::'.join(filter(lambda ns: len(ns) > 0, self.cpp_name_comps))

    # return the cpp name minus namespaces, eg "FooObject"
    def cppname(self):
        return self.cpp_name_comps[-1]

    # return the cpp namespace(s), eg "foo::bar"
    def cppns(self):
        return '::'.join(filter(lambda ns: len(ns) > 0, self.cpp_name_comps[:-1]))

    # return the complete name if it starts with ::, otherwise just the cppname
    def niname(self):
        if self.cpp_name_comps[0] == "":
            return '::'.join(self.cpp_name_comps)
        else:
            return self.cpp_name_comps[-1]


NULL = Traits("*")
UNDEFINED = Traits("void")

class ByteArray:
    data = None
    pos = 0
    def __init__(self, data):
        self.data = data
        self.pos = 0

    def readU8(self):
        r = unpack_from("B", self.data, self.pos)[0]
        self.pos += 1
        assert(r >= 0 and r <= 255)
        return r

    def readU16(self):
        r = unpack_from("<h", self.data, self.pos)[0]
        self.pos += 2
        assert(r >= 0 and r <= 65535)
        return r

    def readDouble(self):
        r = unpack_from("<d", self.data, self.pos)[0]
        self.pos += 8
        return r

    def readBytes(self, lenbytes):
        r = self.data[self.pos:self.pos+lenbytes]
        self.pos += lenbytes
        return r

    def readUTF8(self):
        lenbytes = self.readU30()
        return self.readBytes(lenbytes)

    def readU30(self):
        result = self.readU8()
        if not result & 0x00000080:
            return result
        result = (result & 0x0000007f) | (self.readU8() << 7)
        if not result & 0x00004000:
            return result
        result = (result & 0x00003fff) | (self.readU8() << 14)
        if not result & 0x00200000:
            return result
        result = (result & 0x001fffff) | (self.readU8() << 21)
        if not result & 0x10000000:
            return result
        result = (result & 0x0fffffff) | (self.readU8() << 28)
        return result

class Abc:
    data = None
    major = 0
    minor = 0
    ints = None
    uints = None
    doubles = None
    strings = None
    namespaces = None
    nssets = None
    names = None
    defaults = None
    methods = None
    instances = None
    metadata = None
    classes = None
    scripts = None
    scriptName = ""
    publicNs = Namespace("", CONSTANT_Namespace)
    anyNs = Namespace("*", CONSTANT_Namespace)
    versioned_uris = {}
    is_vm_builtin = False

    magic = 0

    qnameToName = {}
    nameToQName = {}

    def __init__(self, data, scriptName):
        self.scriptName = scriptName
        self.data = ByteArray(data)

        if self.data.readU16() != 16 or self.data.readU16() != 46:
            raise Error("Bad Abc Version")

        self.parseCpool()

        self.defaults = [ (None, 0) ] * 32
        self.defaults[CONSTANT_Utf8] = (self.strings, CTYPE_STRING)
        self.defaults[CONSTANT_Int] = (self.ints, CTYPE_INT)
        self.defaults[CONSTANT_UInt] = (self.uints, CTYPE_UINT)
        self.defaults[CONSTANT_Double] = (self.doubles, CTYPE_DOUBLE)
        self.defaults[CONSTANT_False] = ({ CONSTANT_False: False }, CTYPE_BOOLEAN)
        self.defaults[CONSTANT_True] = ({ CONSTANT_True: True }, CTYPE_BOOLEAN)
        self.defaults[CONSTANT_Namespace] = (self.namespaces, CTYPE_NAMESPACE)
        self.defaults[CONSTANT_PrivateNs] = (self.namespaces, CTYPE_NAMESPACE)
        self.defaults[CONSTANT_PackageNs] = (self.namespaces, CTYPE_NAMESPACE)
        self.defaults[CONSTANT_PackageInternalNs] = (self.namespaces, CTYPE_NAMESPACE)
        self.defaults[CONSTANT_ProtectedNs] = (self.namespaces, CTYPE_NAMESPACE)
        self.defaults[CONSTANT_StaticProtectedNs] = (self.namespaces, CTYPE_NAMESPACE)
        self.defaults[CONSTANT_ExplicitNamespace] = (self.namespaces, CTYPE_NAMESPACE)
        self.defaults[CONSTANT_Null] = ({ CONSTANT_Null: None }, CTYPE_ATOM)

        self.parseMethodInfos()
        self.parseMetadataInfos()
        self.parseInstanceInfos()
        self.parseClassInfos()
        self.parseScriptInfos()
        self.parseMethodBodies()

        for i in range(0, len(self.classes)):
            c = self.classes[i]
            assert(isinstance(c.name, QName))
            prefix = ns_prefix(c.name.ns, True) + to_cname(c.name.name)
            c.class_id = i
            NativeInfo.parse_native_info(c, self.is_vm_builtin)
            self.assign_names(c, prefix)
            if c.itraits:
                self.assign_names(c.itraits, prefix)

        for i in range(0, len(self.scripts)):
            script = self.scripts[i]
            if script != None:
                for j in range(0, len(script.tmethods)):
                    m = script.tmethods[j]
                    if m.metadata != None:
                        for md in m.metadata:
                            if md.name == "native":
                                if md.attrs.has_key("script"):
                                    raise Error("native(script) is no longer supported; please use a native(\"function-name\") instead: " + str(m.name))
                                if len(md.attrs) != 1 or not md.attrs.has_key(""):
                                    raise Error("native(\"function-name\") is the only form supported here" + str(m.name))
                                if not m.isNative():
                                    raise Error("native(\"function-name\") can only be used on native functions" + str(m.name))
                                m.receiver = None
                                m.native_method_name = md.attrs[""]     # override
                                m.native_id_name = "native_script_function_" + ns_prefix(m.name.ns, False) + m.name.name


    def assign_names(self, traits, prefix):
        if traits.init != None:
            traits.init.assign_names(traits, prefix)
        for j in range(0, len(traits.tmethods)):
            traits.tmethods[j].assign_names(traits, prefix)

    def default_ctype_and_value(self,d):
        kind, index = d
        deftable = self.defaults[kind]
        if deftable[0] != None:
            val = str(deftable[0][index])
            ct = deftable[1]
        else:
            assert(kind == 0 and index == 0)
            val = "undefinedAtom"
            ct = CTYPE_ATOM # yes, not void
        rawval = val
        if ct == CTYPE_DOUBLE:
            # Python apparently doesn't have isNaN, isInf
            if is_nan(val):
                val = "MathUtils::kNaN"
            elif is_neg_inf(val):
                val = "MathUtils::kNegInfinity"
            elif is_pos_inf(val):
                val = "MathUtils::kInfinity"
            elif float(val) >= -2147483648.0 and float(val) <= 2147483647.0 and float(val) == floor(float(val)):
                ct = CTYPE_INT
                val = "%.0f" % float(val)
            elif float(val) >= 0.0 and float(val) <= 4294967295.0 and float(val) == floor(float(val)):
                ct = CTYPE_UINT
                val = "%.0fU" % float(val)
        elif ct == CTYPE_STRING:
            for i in range(0, len(self.strings)):
                if (self.strings[i] == str(val)):
                    val = "AvmThunkGetConstantString(%d)/* \"%s\" */" % (i, self.strings[i])
                    break
        elif ct == CTYPE_BOOLEAN:
            assert(str(val) == "False" or str(val) == "True")
            if str(val) == "False":
                val = "false"
            else:
                val = "true"
        elif ct == CTYPE_FLOAT:
        	val = "!!! ERROR, float not handled !!!"
        if str(val) == "None":
            val = "nullObjectAtom"
        return ct,val,rawval

    def parseCpool(self):

        n = self.data.readU30()
        self.ints = [0] * max(1,n)
        for i in range(1, n):
            ii = self.data.readU30()
            if float(ii) > 2147483647.0:
                ii = int(ii - 4294967296.0)
            assert(int(ii) >= -2147483648 and int(ii) <= 2147483647)
            self.ints[i] = int(ii)

        n = self.data.readU30()
        self.uints = [0] * max(1,n)
        for i in range(1, n):
            self.uints[i] = uint(self.data.readU30())

        n = self.data.readU30()
        self.doubles = [ kNaN ] * max(1,n)
        for i in range(1, n):
            self.doubles[i] = self.data.readDouble()

        n = self.data.readU30()
        self.strings = [""] * max(1,n)
        for i in range(1, n):
            self.strings[i] = self.data.readUTF8()

        n = self.data.readU30()
        self.namespaces = [self.anyNs] * max(1,n)
        for i in range(1, n):
            nskind = self.data.readU8()
            if nskind in [CONSTANT_Namespace,
                            CONSTANT_PackageNs,
                            CONSTANT_PackageInternalNs,
                            CONSTANT_ProtectedNs,
                            CONSTANT_ExplicitNamespace,
                            CONSTANT_StaticProtectedNs]:
                uri = self.strings[self.data.readU30()]
                self.namespaces[i] = Namespace(uri, nskind)
                # if it's public, and the final char has the magic "version mark",
                # it's a versioned namespace uri
                if nskind in [CONSTANT_Namespace, CONSTANT_PackageNs]:
                    api, strippeduri = stripVersion(uri)
                    if api >= 0:
                        if not strippeduri in self.versioned_uris:
                            self.versioned_uris[strippeduri] = []
                        if not api in self.versioned_uris[strippeduri]:
                            self.versioned_uris[strippeduri].append(api)

            elif nskind in [CONSTANT_PrivateNs]:
                self.data.readU30() # skip
                self.namespaces[i] = Namespace("private", CONSTANT_PrivateNs)

        n = self.data.readU30()
        self.nssets = [ None ] * max(1,n)
        for i in range(1, n):
            count = self.data.readU30()
            self.nssets[i] = []
            for j in range(0, count):
                self.nssets[i].append(self.namespaces[self.data.readU30()])

        n = self.data.readU30()
        self.names = [ None ] * max(1,n)
        for i in range(1, n):
            namekind = self.data.readU8()
            if namekind in [CONSTANT_Qname, CONSTANT_QnameA]:
                self.names[i] = QName(self.namespaces[self.data.readU30()], self.strings[self.data.readU30()])

            elif namekind in [CONSTANT_RTQname, CONSTANT_RTQnameA]:
                self.names[i] = QName(self.anyNs, self.strings[self.data.readU30()])

            elif namekind in [CONSTANT_RTQnameL, CONSTANT_RTQnameLA]:
                self.names[i] = None

            elif namekind in [CONSTANT_NameL, CONSTANT_NameLA]:
                self.names[i] = QName(Namespace(""), None)

            elif namekind in [CONSTANT_Multiname, CONSTANT_MultinameA]:
                name = self.strings[self.data.readU30()]
                nsset = self.nssets[self.data.readU30()]
                self.names[i] = Multiname(nsset, name)

            elif namekind in [CONSTANT_MultinameL, CONSTANT_MultinameLA]:
                nsset = self.nssets[self.data.readU30()]
                self.names[i] = Multiname(nsset, None)

            elif namekind in [CONSTANT_TypeName]:
                name = self.names[self.data.readU30()];
                count = self.data.readU30();
                types = []
                for j in range(0, count):
                    types.append(self.names[self.data.readU30()]);
                self.names[i] = TypeName(name, types);
            else:
                raise Error("Bad Kind")

    def parseMethodInfos(self):
        self.names[0] = QName(self.publicNs,"*")
        method_count = self.data.readU30()
        self.methods = [ None ] * method_count
        for i in range(0, method_count):
            m = MethodInfo()
            self.methods[i] = m
            param_count = self.data.readU30()
            m.returnType = self.names[self.data.readU30()]
            m.paramTypes = [ None ] * param_count
            m.paramNames = [ "" ] * param_count
            m.optional_count = 0
            for j in range(0, param_count):
                m.paramTypes[j] = self.names[self.data.readU30()]
            m.debugName = self.strings[self.data.readU30()]
            m.flags = self.data.readU8()
            if m.hasOptional():
                m.optional_count = self.data.readU30();
                m.optionalValues = [ (-1, -1) ] * param_count
                for k in range(param_count-m.optional_count, param_count):
                    index = self.data.readU30()
                    kind = self.data.readU8()
                    m.optionalValues[k] = (kind, index)
            if (m.flags & HAS_ParamNames) != 0:
                for j in range(0, param_count):
                    m.paramNames[j] = self.strings[self.data.readU30()]

    def parseMetadataInfos(self):
        count = self.data.readU30()
        self.metadata = [ None ] * count
        for i in range (0, count):
            mname = self.strings[self.data.readU30()]
            m = MetaData(mname)
            self.metadata[i] = m
            values_count = self.data.readU30()
            names = [ None ] * values_count
            for q in range(0, values_count):
                names[q] = self.strings[self.data.readU30()]
            for q in range(0, values_count):
                m.attrs[names[q]] = self.strings[self.data.readU30()]

    def parseInstanceInfos(self):
        count = self.data.readU30()
        self.instances = [ None ] * count
        instancesDict = {}
        for i in range (0, count):
            tname = self.names[self.data.readU30()]
            t = Traits(tname)
            self.instances[i] = t
            instancesDict[id(tname)] = t
            t.base = self.names[self.data.readU30()]
            t.flags = self.data.readU8()
            if (t.flags & 1) != 0:
                t.is_sealed = True
            if (t.flags & 2) != 0:
                t.is_final = True
            if (t.flags & 4) != 0:
                t.is_interface = True
            if (t.flags & 8) != 0:
                t.protectedNs = self.namespaces[self.data.readU30()]
            interface_count = self.data.readU30()
            t.interfaces = [None] * interface_count
            for j in range(0, interface_count):
                t.interfaces[j] = self.names[self.data.readU30()]
            methid = self.data.readU30()
            t.init = self.methods[methid]
            t.init.name = t.name
            t.init.kind = TRAIT_Method
            t.init.id = methid
            self.parseTraits(t, instancesDict.get(id(t.base), None))

    @staticmethod
    def __qname(name):
        if isinstance(name, QName):
            return name
        if len(name.nsset) == 0:
            return QName(Namespace("", CONSTANT_Namespace), name.name)
        return QName(name.nsset[0].stripVersion(), name.name)

    def qname(self, name):
        if (not self.nameToQName.has_key(id(name))):
            try:
                result = self.__qname(name)
            except:
                print dir(name)
                raise
            self.qnameToName[id(result)] = name
            self.nameToQName[id(name)] = result
            return result
        return self.nameToQName[id(name)]

    def parseTraits(self, t, baseTraits=None):
        lastBaseTraitsSlotId = 0 if baseTraits is None else baseTraits.nextSlotId
        namecount = self.data.readU30()
        t.members = [ None ] * namecount
        for i in range(0, namecount):
            name_index = self.data.readU30()
            name = self.names[name_index]
            name = self.qname(name)
            bindingOffset = self.data.pos
            tag = self.data.readU8()
            kind = tag & 0xf
            member = None
            if kind in [TRAIT_Slot, TRAIT_Const, TRAIT_Class]:
                member = SlotInfo()
                member.ns = name.ns
                member.fileOffset = bindingOffset
                memberId = self.data.readU30()
                member.id = (memberId - 1) if memberId != 0 else (len(t.slots) + lastBaseTraitsSlotId)
                memberIndex = member.id - lastBaseTraitsSlotId
                while len(t.slots) <= memberIndex:
                    t.slots.append(None)
                t.slots[member.id - lastBaseTraitsSlotId] = member
                t.nextSlotId = max(t.nextSlotId, member.id + 1)
                if kind in [TRAIT_Slot, TRAIT_Const]:
                    member.type = self.names[self.data.readU30()]
                    index = self.data.readU30()
                    if index:
                        deftable = self.defaults[self.data.readU8()]
                        member.value = deftable[0][index]
                        if deftable[1] == CTYPE_NAMESPACE:
                            assert(isinstance(member.value, Namespace))
                            member.value.srcname = name.name
                else:
                    member.value = self.classes[self.data.readU30()]
                    member.value.qname = name
            elif kind in [TRAIT_Method, TRAIT_Getter, TRAIT_Setter]:
                self.data.readU30() # disp_id, ignored
                methid = self.data.readU30()
                member = self.methods[methid]
                t.tmethods.append(member)
                member.id = methid
                member.final = (tag & ATTR_final) != 0
                member.override = (tag & ATTR_override) != 0
            member.kind = kind
            member.name = name
            member.name_index = name_index
            t.members[i] = member
            t.names[str(name)] = member

            if (tag & ATTR_metadata) != 0:
                mdCount = self.data.readU30()
                member.metadata = [ None ] * mdCount
                for j in range(0, mdCount):
                    member.metadata[j] = self.metadata[self.data.readU30()]
                # stash class metadata in the ctraits and itraits too, makes it much easier later
                if kind == TRAIT_Class:
                    member.value.metadata = member.metadata
                    member.value.itraits.metadata = member.metadata

    def parseClassInfos(self):
        count = len(self.instances)
        self.classes = [ None ] * count
        for i in range(0, count):
            itraits = self.instances[i]
            tname = QName(itraits.name.ns, (str(itraits.name.name) + "$"))
            if str(tname) == "Object$":
                self.is_vm_builtin = True
            t = Traits(tname)
            self.classes[i] = t
            t.init = self.methods[self.data.readU30()]
            t.base = "Class"
            t.itraits = itraits
            itraits.ctraits = t
            t.init.name = str(t.itraits.name) + "$cinit"
            t.init.kind = TRAIT_Method
            self.parseTraits(t)

    def parseScriptInfos(self):
        count = self.data.readU30()
        self.scripts = [ None ] * count
        for i in range(0, count):
            tname = self.scriptName + "_script_" + str(i)
            t = Traits(tname)
            self.scripts[i] = t
            t.init = self.methods[self.data.readU30()]
            t.base = self.names[0]
            t.itraits = None
            t.init.name = t.name + "$init"
            t.init.kind = TRAIT_Method
            self.parseTraits(t)
            # this is a little hacky, but it allows us to assume that all traits have a valid CPP name.
            t.cpp_name_comps = BASE_INSTANCE_NAME.split('::')
            assert t.itraits == None

    def parseMethodBodies(self):
        count = self.data.readU30()
        for i in range(0, count):
            m = self.methods[self.data.readU30()]
            m.max_stack = self.data.readU30()
            m.local_count = self.data.readU30()
            initScopeDepth = self.data.readU30()
            maxScopeDepth = self.data.readU30()
            m.max_scope = maxScopeDepth - initScopeDepth
            code_length = self.data.readU30()
            m.code = self.data.readBytes(code_length)
            ex_count = self.data.readU30()
            for j in range(0, ex_count):
                frm = self.data.readU30()
                to = self.data.readU30()
                target = self.data.readU30()
                type = self.names[self.data.readU30()]
                name = self.names[self.data.readU30()];
            m.activation = Traits(None)
            self.parseTraits(m.activation)


class IndentingPrintWriter:
    f = None
    indent = 0
    backslash = 0
    do_indent = True

    def __init__(self, file):
        self.f = file

    def dent(self):
        if self.do_indent:
            for i in range(0, self.indent):
                self.f.write("    ")
            self.do_indent = False

    def prnt(self, s):
        self.dent();
        self.f.write(s)

    def println(self, s):
        assert self.indent >= 0
        if s != "":
            self.dent();
            self.f.write(s)
        if self.backslash > 0:
            self.f.write(" \\")
        self.f.write("\n")
        self.do_indent = True


NON_POINTER_4_BYTE_SLOT_BUCKET = 0
POINTER_SLOT_BUCKET = 1
NON_POINTER_8_BYTE_SLOT_BUCKET = 2

CTYPE_TO_SLOT_SORT_BUCKET = {
    # following types are 4 bytes
    CTYPE_INT : NON_POINTER_4_BYTE_SLOT_BUCKET,
    CTYPE_UINT : NON_POINTER_4_BYTE_SLOT_BUCKET,
    CTYPE_BOOLEAN : NON_POINTER_4_BYTE_SLOT_BUCKET,
    CTYPE_FLOAT : NON_POINTER_4_BYTE_SLOT_BUCKET,
    # following types are pointer size ( either 4 or 8 bytes )
    CTYPE_OBJECT : POINTER_SLOT_BUCKET,
    CTYPE_ATOM : POINTER_SLOT_BUCKET,
    CTYPE_STRING : POINTER_SLOT_BUCKET,
    CTYPE_NAMESPACE : POINTER_SLOT_BUCKET,
    # doubles are 8 bytes
    CTYPE_DOUBLE : NON_POINTER_8_BYTE_SLOT_BUCKET,
    # slots should never be of type void
    CTYPE_VOID : -1
}

CTYPE_TO_NEED_FORWARD_DECL = {
    CTYPE_INT : False,
    CTYPE_UINT : False,
    CTYPE_BOOLEAN : False,
    CTYPE_OBJECT : True,
    CTYPE_ATOM : False,
    CTYPE_STRING : True,
    CTYPE_NAMESPACE : True,
    CTYPE_DOUBLE : False,
    CTYPE_FLOAT : False,
    CTYPE_VOID : False
}

GLUECLASSES_WITHOUT_SLOTS = frozenset((
    'bool',
    'double',
    'float',
    'int32_t',
    'avmplus::Namespace',
    'avmplus::String',
    'uint32_t'))

GLUECLASSES_WITHOUT_CONSTRUCT_WRAPPERS = frozenset((
    'bool',
    'double',
    'int32_t',
    'uint32_t'))

class AbcThunkGen:
    abc = None
    abcs = []
    all_thunks = []
    lookup_traits = None
    namesDict = None

    def addAbc(self, a):
        self.abcs.append(a)
        self.lookup_traits = None

    def class_native_name(self, c):
        return ns_prefix(c.qname.ns, True) + to_cname(c.qname.name)

    def class_id_name(self, c):
        return "abcclass_" + self.class_native_name(c)

    def emitAOT(self, out, name):
        out.println('#ifdef VMCFG_AOT')
        
        traits = filter(lambda t: (not (t.itraits != None and t.itraits.is_interface)) and \
                                    (not t.is_interface) and \
                                    (not t.is_synthetic) and \
                                    (t.fqcppname() != "double") and \
                                    ((t.ctype == CTYPE_OBJECT) or (t.fqcppname() == BASE_INSTANCE_NAME) or (t.fqcppname() == BASE_CLASS_NAME)), \
                        self.abc.classes + self.abc.instances)
        glueClasses = sorted(set(map(lambda t: tuple([t.fqcppname(), t.niname()]), traits)))
        
        out.println('extern "C" const struct {')
        out.indent += 1
        
        i = 0
        for i in range(0, len(glueClasses)):
            out.println('const char* const n_%(count)u; %(glueClass)s* const m_%(count)u;' % { 'count' : i, 'glueClass': glueClasses[i][0]})
        
        out.indent -= 1
        out.println('} aotABCTypes_%s = {' % name)
        out.indent += 1
        
        i = 0
        for i in range(0, len(glueClasses)):
            out.println('"%(glueClass)s", 0,' % { 'count' : i, 'glueClass': glueClasses[i][1]})
        
        out.indent -= 1
        out.println('};')
        
        out.println('#endif')

    def emit_h(self, out, name):

        out.println(MPL_HEADER);
        out.println('')
        out.println("/* machine generated file -- do not edit */");
        out.println('')

        out.println("#ifndef _H_nativegen_header_%s" % name);
        out.println("#define _H_nativegen_header_%s" % name);
        out.println('')

        self.forwardDeclareGlueClasses(out)

        nativeIDNamespaces = opts.nativeIDNS.split('::')
        out.println(' '.join(map(lambda ns: 'namespace %s {' % ns, nativeIDNamespaces)))
        out.println('')

        out.println('extern const uint32_t '+name+"_abc_class_count;")
        out.println('extern const uint32_t '+name+"_abc_script_count;")
        out.println('extern const uint32_t '+name+"_abc_method_count;")
        out.println('extern const uint32_t '+name+"_abc_length;")
        out.println('extern const uint8_t '+name+"_abc_data[];");
        out.println('extern const char* const '+name+"_versioned_uris[];");

        out.println("AVMTHUNK_DECLARE_NATIVE_INITIALIZER(%s)" % (name));

        out.println('')
        out.println("/* classes */");
        for i in range(0, len(self.abc.classes)):
            c = self.abc.classes[i]
            out.println("const uint32_t " + self.class_id_name(c) + " = " + str(c.class_id) + ";");

        out.println('')
        out.println("/* methods */");
        for i in range(0, len(self.abc.methods)):
            m = self.abc.methods[i]
            if m.native_id_name != None:
                assert(m.id == i)
                if m.isNative():
                    out.println("const uint32_t "+m.native_id_name+" = "+str(m.id)+";");
                else:
                    # not sure if we want to expose method id's for non-native methods; emit as comments for now
                    out.println("/* const uint32_t "+m.native_id_name+" = "+str(m.id)+"; */");
        out.println('')

        for receiver,m in self.all_thunks:
            self.emitThunkProto(out, receiver, m);

        out.println('class SlotOffsetsAndAsserts;')
        self.emitStructDeclarations(out)

        out.println(' '.join(('}',) * len(nativeIDNamespaces)))

        out.println('namespace %s {' % opts.rootImplNS)
        self.emitGlueClassManifest(out)
        out.println('}')

        out.println("#endif // _H_nativegen_header_%s" % name);


    def emit_cls(self, out, name):

        out.println(MPL_HEADER);
        out.println('')
        out.println("/* machine generated file -- do not edit */");
        out.println('')
        out.println("#ifndef _H_nativegen_classes_%s" % name);
        out.println("#define _H_nativegen_classes_%s" % name);
        out.println('')
        
        rootNS = opts.rootImplNS.split('::')
        out.println(' '.join(map(lambda ns: 'namespace %s {' % ns, rootNS)))
        out.println('')

        self.emitSyntheticClasses(out)
        
        out.println(' '.join(('}',) * len(rootNS)))

        out.println('')
        out.println("#endif // _H_nativegen_classes_%s" % name);

    def emit_cpp(self, out, name):

        out.println(MPL_HEADER);
        out.println('')
        out.println("/* machine generated file -- do not edit */");
        out.println('')

        nativeIDNamespaces = opts.nativeIDNS.split('::')
        out.println(' '.join(map(lambda ns: 'namespace %s {' % ns, nativeIDNamespaces)))
        out.println('')

        out.println("const uint32_t "+name+"_abc_class_count = "+str(len(self.abc.classes))+";");
        out.println("const uint32_t "+name+"_abc_script_count = "+str(len(self.abc.scripts))+";");
        out.println("const uint32_t "+name+"_abc_method_count = "+str(len(self.abc.methods))+";");
        out.println("const uint32_t "+name+"_abc_length = "+str(len(self.abc.data.data))+";");

        out.println("");
        out.println("/* thunks (%d total) */" % len(self.all_thunks));
        if opts.thunkvprof:
            out.println("#define DOPROF")
            out.println('#include "../vprof/vprof.h"')

        for receiver,m in self.all_thunks:
            self.emitThunkBody(out, receiver, m);

        out.println("")
        self.printStructAsserts(out, self.abc)
        out.println("")

        out.println("");
        out.println("AVMTHUNK_BEGIN_NATIVE_TABLES(%s)" % self.abc.scriptName)
        out.indent += 1

        out.println("");
        out.println("AVMTHUNK_BEGIN_NATIVE_METHODS(%s)" % self.abc.scriptName)
        out.indent += 1
        for i in range(0, len(self.abc.methods)):
            m = self.abc.methods[i]
            if m.isNative() and (m.receiver == None or not m.receiver.is_interface):
                assert(m.native_method_name != None)
                assert(m.native_id_name != None)
                if m.receiver == None:
                    out.println("AVMTHUNK_NATIVE_FUNCTION(%s, %s)" % (m.native_id_name, m.native_method_name))
                else:
                    # special-case the two oddballs of the group: String and Namespace
                    # don't descend from ScriptObject and so need a little extra love.
                    if str(m.receiver.name) == "String":
                        nmout = "AVMTHUNK_NATIVE_METHOD_STRING"
                    elif str(m.receiver.name) == "Namespace":
                        nmout = "AVMTHUNK_NATIVE_METHOD_NAMESPACE"
                    else:
                        nmout = "AVMTHUNK_NATIVE_METHOD"
                    out.println("%s(%s, %s::%s)" % (nmout, m.native_id_name, m.receiver.method_map_name, m.native_method_name))
        out.indent -= 1
        out.println("AVMTHUNK_END_NATIVE_METHODS()")

        out.println("");
        out.println("AVMTHUNK_BEGIN_NATIVE_CLASSES(%s)" % self.abc.scriptName)
        out.indent += 1
        for i in range(0, len(self.abc.classes)):
            c = self.abc.classes[i]
            if c.itraits.is_interface:
                continue
            offsetOfSlotsClass = "SlotOffsetsAndAsserts::kSlotsOffset_%s" % to_cname(c.fqcppname())
            offsetOfSlotsInstance = "SlotOffsetsAndAsserts::kSlotsOffset_%s" % to_cname(c.itraits.fqcppname())
            out.println("AVMTHUNK_NATIVE_CLASS(%s, %s, %s, %s, %s, %s, %s, %s, %s)" %\
                (self.class_id_name(c),\
                c.cppname(),\
                c.fqcppname(),\
                offsetOfSlotsClass,\
                c.itraits.fqcppname(),\
                offsetOfSlotsInstance,\
                str(c.has_construct_method_override).lower(),
                str(c.is_restricted_inheritance).lower(),
                str(c.is_abstract_base).lower()))
        out.indent -= 1
        out.println("AVMTHUNK_END_NATIVE_CLASSES()")

        out.println("");
        out.indent -= 1
        out.println("AVMTHUNK_END_NATIVE_TABLES()")

        out.println("");
        out.println("AVMTHUNK_DEFINE_NATIVE_INITIALIZER(%s)" % (name));

        if opts.externmethodandclassetables:
            out.println("");
            out.println('extern const NativeClassInfo* '+name+"_classEntriesExtern = "+name+"_classEntries;");
            out.println('extern const NativeMethodInfo* '+name+"_methodEntriesExtern = "+name+"_methodEntries;");

        out.println("");
        out.println("/* abc */");
        n = len(self.abc.data.data)
        out.println("const uint8_t "+name+"_abc_data["+str(n)+"] = {");
        for i in range(0, n):
            x = ord(self.abc.data.data[i]) & 255;
            out.prnt("%4d" % x)
            if i+1 < n:
                out.prnt(",")
            if i%16 == 15:
                out.println("");
        out.println("};");
        out.println('')

        out.println("");
        out.println("/* versioned_uris */");
        out.println("const char* const "+name+"_versioned_uris[] = {");
        out.indent += 1
        # don't really need to sort 'em, but helps keep output stable
        sorted_keys = sorted(self.abc.versioned_uris.keys())
        for i in sorted_keys:
            # The empty URI (aka "public") is always versioned 
            # (and special-cased by the versioning code in AvmCore)
            # so don't bother emitting it.
            if len(i) > 0:
                out.println('"%s", // %s' % (i, str(sorted(self.abc.versioned_uris[i]))))
        out.println('NULL')
        out.indent -= 1
        out.println("};");
        out.println('')

        self.emitAOT(out, name)

        out.println(' '.join(('}',) * len(nativeIDNamespaces)))

        # note, these are emitted *after* closing the namespaces
        self.emitMethodBodies(out)

    def emit(self, abc, name, out_h, out_cls, out_c):
        self.abc = abc;
        self.all_thunks = []
        self.lookup_traits = None
        self.namesDict = {}
        for i in range(1, len(abc.names)):
            if (not isinstance(abc.names[i], TypeName)):
                self.namesDict[id(abc.qname(abc.names[i]))] = i

        for i in range(0, len(abc.scripts)):
            script = abc.scripts[i]
            if script != None:
                self.processTraits(script)
        
        self.emit_h(out_h, name)
        self.emit_cls(out_cls, name)
        self.emit_cpp(out_c, name)

    def forwardDeclareGlueClasses(self, out_h):
        # find all the native glue classes and write forward declarations for them
        cppNamespaceToGlueClasses = {}
        traitsSet = set()
        for i in range(0, len(self.abc.classes)):
            c = self.abc.classes[i]
            traitsSet.add(c)
            traitsSet.add(c.itraits)
            if c.itraits.is_gc_exact:
                base = self.lookupTraits(c.itraits.base)
                # ScriptObject isn't marked as gc_exact, but that's ok here
                while base != None and base.fqcppname() != BASE_INSTANCE_NAME:
                    if not base.is_gc_exact:
                        if c.itraits.is_synthetic:
                            raise Error("class %s is implicitly assumed to be exact-gc but its ancestor %s is not; this is not supported" % (c.itraits.fqcppname(), base.fqcppname()))
                        else:
                            raise Error("class %s is explicitly marked as exact-gc but its ancestor %s is not; this is not supported" % (c.itraits.fqcppname(), base.fqcppname()))
                    base = self.lookupTraits(base.base)
                    

        for t in frozenset(traitsSet):
            filteredSlots = filter(lambda s: s is not None, t.slots)
            for s in filteredSlots:
                slotTraits = self.lookupTraits(s.type)
                traitsSet.add(slotTraits)

        glueClassToTraits = {}
        for t in sorted(traitsSet):
            if CTYPE_TO_NEED_FORWARD_DECL[t.ctype]:
                classNS = t.cppns()
                glueClassName = t.cppname()
                # special hack because the metadata for the class Math says its instance data is of type double
                if glueClassName != "double" and glueClassName != "float" :
                    cppNamespaceToGlueClasses.setdefault(classNS, set()).add(glueClassName)
                    key = classNS + '::' + glueClassName
                    if not key in glueClassToTraits:
                        glueClassToTraits[key] = []
                    glueClassToTraits[key].append(t)
        for (nsStr, glueClasses) in sorted(cppNamespaceToGlueClasses.iteritems()):
            # turn list of namespaces [foo, bar, baz] into "namespace foo { namespace bar { namespace baz {"
            nsList = nsStr.split('::')
            out_h.println(' '.join(map(lambda ns: 'namespace %s {' % ns, nsList)))
            out_h.indent += 1
            
            # this can emit the same class multiple times; that's by design,
            # for clarity & stability of output
            for glueClass in sorted(glueClasses):
                traitsList = glueClassToTraits[nsStr + '::' + glueClass]
                for traits in sorted(traitsList):
                    out_h.println('class %s; // %s' % (glueClass, traits))
            out_h.indent -= 1
            out_h.println(' '.join(('}',) * len(nsList)))
            out_h.println('')

    def emitGlueClassManifest(self, out):
        names = []
        for i in range(0, len(self.abc.classes)):
            c = self.abc.classes[i]
            as3name = to_cname(c.itraits.name.name) + "Class"
            cppname = c.fqcppname()
            clsid = "%s::%s" % (opts.nativeIDNS, self.class_id_name(c))
            names.append((as3name,cppname,clsid))
        names = sorted(names)
        man_name = "%sClassManifest" % self.abc.scriptName
        out.println("");
        out.println("class %s : public avmplus::ClassManifestBase" % man_name)
        out.println("{")
        out.indent += 1
        out.println("friend class avmplus::AvmCore;")
        out.println("friend class avmplus::IntVectorClass;")
        out.println("friend class avmplus::UIntVectorClass;")
        out.println("// friend class avmplus::FloatVectorClass;") # remove comment when adding FloatVectorClass in Tamarin
        out.println("friend class avmplus::DoubleVectorClass;")
        out.println("friend class avmplus::ObjectVectorClass;")
        out.indent -= 1
        out.println("private:")
        out.indent += 1
        out.println("REALLY_INLINE %s(avmplus::ScriptEnv* e) : ClassManifestBase(%d, e) { }" % (man_name, len(names)))
        out.println("REALLY_INLINE static %s* create(avmplus::ScriptEnv* e) { return new (MMgc::GC::GetGC(e), MMgc::kExact, sizeof(ClassClosure*)*%d) %s(e); }" % (man_name, len(names)-1, man_name))
        out.indent -= 1
        out.println("public:")
        out.indent += 1
        for as3name,cppname,clsid in names:
            # We can't use static_cast<> because the subclass is only forward-declared at this point
            out.println("REALLY_INLINE GCRef<%s> get_%s() { return (%s*)(lazyInitClass(%s)); }" % (cppname, as3name, cppname, clsid))
        out.indent -= 1
        out.println("};")

    @staticmethod
    def cmpSlots(slotA, slotB, slotsTypeInfo):
        if (slotA is slotB):
            return 0;

        # slotA or slotB could be None, which means they are an anonymous slot.
        # Anonymous slots should be at the end of the pointer slots.
        ctype_b = slotsTypeInfo[id(slotB)][0]
        slotBBucket = CTYPE_TO_SLOT_SORT_BUCKET[ctype_b] if (slotB is not None) else POINTER_SLOT_BUCKET
        if (slotA is None):
            if (slotBBucket <= POINTER_SLOT_BUCKET):
                return 1
            else:
                return -1

        assert slotA is not None
        ctype_a = slotsTypeInfo[id(slotA)][0]
        slotABucket = CTYPE_TO_SLOT_SORT_BUCKET[ctype_a]
        if (slotB is None):
            if (slotBBucket <= POINTER_SLOT_BUCKET):
                return -1
            else:
                return 1

        assert slotB is not None
        slotBucketCmp = cmp(slotABucket, slotBBucket)
        if (slotBucketCmp != 0):
            return slotBucketCmp
        return cmp(slotA.fileOffset, slotB.fileOffset)

    @staticmethod
    def needsInstanceSlotsStruct(c):
        return c.itraits.fqcppname() != BASE_INSTANCE_NAME

    def emitStructDeclarations(self, out):
        visitedGlueClasses = set()
        for i in range(0, len(self.abc.classes)):
            c = self.abc.classes[i]
            if c.itraits.is_interface:
                continue
            self.emitStructDeclarationsForTraits(out, c, visitedGlueClasses)
            if (self.needsInstanceSlotsStruct(c)):
                self.emitStructDeclarationsForTraits(out, c.itraits, visitedGlueClasses)

    def emitMethodBodies(self, out):
        visitedGlueClasses = set()
        for i in range(0, len(self.abc.classes)):
            c = self.abc.classes[i]
            if c.itraits.is_interface:
                continue
            self.emitMethodBodiesForTraits(out, c, visitedGlueClasses)
            if (self.needsInstanceSlotsStruct(c)):
                self.emitMethodBodiesForTraits(out, c.itraits, visitedGlueClasses)

    def sortSlots(self, t):
        filteredSlots = filter(lambda s: s is not None, t.slots)

        slotsTypeInfo = {}
        for slot in filteredSlots:
            slotTraits = self.lookupTraits(slot.type)
            slotCType = slotTraits.ctype
            if slotCType == CTYPE_VOID:
                raise Error("A slot should never be CTYPE_VOID")
            slotArgType = slotTraits.cpp_argument_name()
            slotRetType = slotTraits.cpp_return_name()
            slotMemberType = slotTraits.cpp_gcmember_name()
            slotsTypeInfo[id(slot)] = (slotCType, slotArgType, slotRetType, slotMemberType)

        sortedSlots = sorted(t.slots, lambda x,y: self.cmpSlots(x, y, slotsTypeInfo))
        return sortedSlots, slotsTypeInfo

    def emitDeclareSlotClass(self, out, t, sortedSlots, slotsTypeInfo):
        memberVars = []
        out.println('//-----------------------------------------------------------')
        out.println('// %s' % str(t.name))
        out.println('//-----------------------------------------------------------')
        out.println('class %s' % t.slotsStructName)
        out.println('{')
        out.indent += 1
        out.println('friend class SlotOffsetsAndAsserts;')
        out.println('friend class %s;' % t.fqcppname())
        if len(sortedSlots) > 0:
            out.indent -= 1
            out.println('private:')
            out.indent += 1
        anonCount = 0
        for slot in sortedSlots:
            if (slot is not None):
                assert slot.kind in (TRAIT_Slot, TRAIT_Const)
                (slotCType, slotArgType, slotRetType, slotMemberType) = slotsTypeInfo[id(slot)]
                out.println('%s m_%s;' % (slotMemberType, to_cname(slot.name)))
            else:
                out.println('Atom __anonymous_slot_%u;' % (anonCount))
                anonCount = anonCount + 1

        if t.is_gc_exact:
            numTracedSlots = self.countTracedSlots(sortedSlots)
            if numTracedSlots > 0:
                out.indent -= 1
                out.println('public:');
                out.indent += 1
                out.println('REALLY_INLINE void gcTracePrivateProperties(MMgc::GC* gc)')
                out.println('{')
                out.indent += 1
                anonCount = 0
                for slot in sortedSlots:
                    if (slot is not None):
                        slotTraits = self.lookupTraits(slot.type)
                        if slotTraits.ctype == CTYPE_ATOM:
                            out.println('gc->TraceAtom(&m_%s);' % to_cname(slot.name))
                        elif (slotTraits.ctype == CTYPE_STRING) or (slotTraits.ctype == CTYPE_NAMESPACE) or (slotTraits.ctype == CTYPE_OBJECT):
                            out.println('gc->TraceLocation(&m_%s);' % to_cname(slot.name))
                    else:
                        out.println('gc->TraceAtom(&__anonymous_slot_%u);' % (anonCount,))
                        anonCount = anonCount + 1
                out.indent -= 1
                out.println('}')
            else:
                out.indent -= 1
                out.println('#define GC_TRIVIAL_TRACER_' + t.cppname())
                out.indent += 1

        out.indent -= 1
        out.println('};')

    def emitConstructDeclarations(self, out, t, sortedSlots, slotsTypeInfo):
        
        if t.itraits != None:
            # FIXME: make these non-public, friend access only
            out.println("public:")
            out.indent += 1
            out.println("static %s* FASTCALL createClassClosure(avmplus::VTable* cvtable);" % BASE_CLASS_NAME)
            out.indent -= 1

            if t.has_custom_createInstanceProc:
                out.println("public:") 
                out.indent += 1
                if t.has_pre_create_check: 
                    out.println("static void FASTCALL preCreateInstanceCheck(%s*);" % BASE_CLASS_NAME);
                out.println("static avmplus::ScriptObject* FASTCALL createInstanceProc(%s*);" % BASE_CLASS_NAME)
                out.indent -= 1

        if t.has_construct_method_override:
            # emit the construct declaration to ensure that construct is defined for this class
            out.println("public:")
            out.indent += 1
            out.println("virtual avmplus::Atom construct(int argc, avmplus::Atom* argv);")
            out.indent -= 1
        else:
            # TEMPORARY: emit a (debug-only) stub to ensure that construct is NOT defined for this class
            out.println("public:")
            out.indent += 1
            out.println("AvmThunk_DEBUG_ONLY( virtual avmplus::Atom construct(int argc, avmplus::Atom* argv); )")
            out.indent -= 1
            
        # createInstance() is no longer supported; emit a (debug-only) stub to generate compile errors for any dangling usages
        out.println("private:")
        out.indent += 1
        out.println("AvmThunk_DEBUG_ONLY( virtual void createInstance() { AvmAssert(0); } )")
        out.indent -= 1

    def emitConstructStubs(self, out, t, sortedSlots, slotsTypeInfo):

        if t.itraits != None:
            out.println("/*static*/ %s* FASTCALL %s::createClassClosure(avmplus::VTable* cvtable)" % (BASE_CLASS_NAME,t.fqcppname()))
            out.println("{")
            out.indent += 1
            if t.is_gc_exact:
                out.println("cvtable->ivtable->createInstanceProc = %s;" % (t.createInstanceProcName));
                out.println("ClassClosure* const cc = new (cvtable->gc(), MMgc::kExact, cvtable->getExtraSize()) %s(cvtable);" % t.fqcppname())
            else:
                out.println("cvtable->ivtable->createInstanceProc = %s;" % (t.createInstanceProcName));
                out.println("ClassClosure* const cc = new (cvtable->gc(), cvtable->getExtraSize()) %s(cvtable);" % t.fqcppname())
            out.println("AvmThunk_DEBUG_ONLY( %s::SlotOffsetsAndAsserts::check_%s(cc->traits(), cc->traits()->itraits); )" % (opts.nativeIDNS, to_cname(t.fqcppname())))
            out.println("return cc;")
            out.indent -= 1
            out.println("}")

            if t.has_custom_createInstanceProc:
                out.println("/*static*/ avmplus::ScriptObject* FASTCALL %s::createInstanceProc(%s* cls)" % (t.fqcppname(), BASE_CLASS_NAME));
                out.println("{");
                out.indent += 1
                if t.has_pre_create_check: 
                    out.println("%s::preCreateInstanceCheck(cls);" % (t.fqcppname()));
                if t.itraits.is_gc_exact:
                    out.println("return new (cls->gc(), MMgc::kExact, cls->getExtraSize()) %s(cls->ivtable(), cls->prototypePtr());" % (t.itraits.fqcppname()))
                else:
                    out.println("return new (cls->gc(), cls->getExtraSize()) %s(cls->ivtable(), cls->prototypePtr());" % (t.itraits.fqcppname()))
                out.indent -= 1
                out.println("}");

        if not t.has_construct_method_override:
            if t.fqinstancebase_name != None:
                baseclassname = t.fqinstancebase_name
            else:
                baseclassname = self.lookupTraits(t.base).fqcppname()
            out.println("AvmThunk_DEBUG_ONLY( avmplus::Atom %s::construct(int argc, avmplus::Atom* argv) { return %s::construct(argc, argv); } )" % (t.fqcppname(), baseclassname))

    def get_args_info(self, t, m):
        argtraits = self.argTraits(t, m)
        args = []
        for i in range(0, len(argtraits)):
            argt = argtraits[i]
            if i == 0:
                argname = "thisRef"
            else:
                argname = "arg%d" % i
            arg_typedef = TYPEMAP_RETTYPE_GCREF[argt.ctype](argt)
            args.append((argt, arg_typedef, argname))
        return args

    def shouldEmitConstructObject(self, t):
        # note, we don't support needRest() methods; we probably could with some effort,
        # but there's currently no need, so we haven't bothered
        return t.itraits != None and \
                t.construct != "none" and \
                not t.is_abstract_base and \
                not t.itraits.is_interface and \
                not t.itraits.fqcppname() in GLUECLASSES_WITHOUT_CONSTRUCT_WRAPPERS and \
                not t.itraits.init.needRest()

    def emitConstructObjectDeclaration(self, out, t, args):
        ret_typedef = TYPEMAP_RETTYPE_GCREF[t.itraits.ctype](t.itraits)
        arglist = ', '.join(map(lambda (argt, arg_typedef, argname): "%s %s" % (arg_typedef, argname), args[1:]))
        out.println("%s constructObject(%s);" % (ret_typedef, arglist))

    def emitMethodWrappers(self, out, t):
        if self.shouldEmitConstructObject(t):
            out.println("public:")
            out.indent += 1
            args = self.get_args_info(t,t.itraits.init)
            ctype = t.itraits.ctype
            # t.itraits is a pure-AS3 class, so it has no C++ class;
            # the most closest native ancestor and use that.
            fqcppname = t.itraits.fqcppname()
            ret_typedef = TYPEMAP_RETTYPE_GCREF[ctype](t.itraits)
            for i in range(0, t.itraits.init.optional_count+1):
                arglist = ', '.join(map(lambda (argt, arg_typedef, argname): "%s %s" % (arg_typedef, argname), args[1:]))
                # "inline" rather than "REALLY_INLINE" -- it's not essential to inline
                # this, so let the compiler decide to deinline if it so chooses
                out.println("inline %s constructObject(%s)" % (ret_typedef, arglist))
                out.println("{")
                out.indent += 1
                # bah, does Python not have an equivalent to JS Array.some?
                needcore = False
                for (argt, arg_typedef, argname) in args[1:]:
                    needcore = needcore or TYPEMAP_TO_ATOM_NEEDS_CORE[argt.ctype]
                if needcore:
                    # explicitly cast to AvmCore* because it might be an only-forward-declared subclass
                    out.println("avmplus::AvmCore* const core = ((avmplus::AvmCore*)(this->core()));")
                arglist = ', '.join(map(lambda (argt, arg_typedef, argname): TYPEMAP_TO_ATOM[argt.ctype](argname), args))
                out.println("avmplus::Atom args[%d] = { %s };" % (len(args), arglist))
                if t.construct == "native":
                    out.println("avmplus::Atom const result = this->construct_native(%s::createInstanceProc, %d, args);" % (t.fqcppname(), len(args)-1))
                else:
                    out.println("avmplus::Atom const result = this->construct(%d, args);" % (len(args)-1))
                out.println("return %s;" % TYPEMAP_ATOM_TO_GCREF[ctype]("result",t.itraits))
                out.indent -= 1
                out.println("}")
                args.pop()
            out.indent -= 1
        if t.itraits != None:
            out.println("public:")
            out.indent += 1
            ctype = t.itraits.ctype
            ifqcppname = t.itraits.fqcppname()
            ret_typedef = TYPEMAP_RETTYPE_GCREF[ctype](t.itraits)
            out.println("REALLY_INLINE bool isType(avmplus::Atom value)")
            out.println("{")
            out.indent += 1
            out.println("return isTypeImpl(value);")
            out.indent -= 1
            out.println("}")
            out.println("REALLY_INLINE bool isType(GCRef<avmplus::ScriptObject> value)")
            out.println("{")
            out.indent += 1
            out.println("return isTypeImpl(value->atom());")
            out.indent -= 1
            out.println("}")
            out.println("REALLY_INLINE %s asType(avmplus::Atom value)" % ret_typedef)
            out.println("{")
            out.indent += 1
            out.println("avmplus::Atom const result = asTypeImpl(value);")
            out.println("return %s;" % TYPEMAP_ATOM_TO_GCREF[ctype]("result",t.itraits))
            out.indent -= 1
            out.println("}")
            out.println("REALLY_INLINE %s asType(GCRef<avmplus::ScriptObject> value)" % ret_typedef)
            out.println("{")
            out.indent += 1
            out.println("avmplus::Atom const result = asTypeImpl(value->atom());")
            out.println("return %s;" % TYPEMAP_ATOM_TO_GCREF[ctype]("result",t.itraits))
            out.indent -= 1
            out.println("}")
            out.println("REALLY_INLINE %s coerceToType(avmplus::Atom value)" % ret_typedef)
            out.println("{")
            out.indent += 1
            out.println("avmplus::Atom const result = coerceToTypeImpl(value);")
            out.println("return %s;" % TYPEMAP_ATOM_TO_GCREF[ctype]("result",t.itraits))
            out.indent -= 1
            out.println("}")
            out.println("REALLY_INLINE %s coerceToType(GCRef<avmplus::ScriptObject> value)" % ret_typedef)
            out.println("{")
            out.indent += 1
            out.println("avmplus::Atom const result = coerceToTypeImpl(value->atom());")
            out.println("return %s;" % TYPEMAP_ATOM_TO_GCREF[ctype]("result",t.itraits))
            out.indent -= 1
            out.println("}")
            out.indent -= 1

        # "cpp_vis" is used to sort the visibility of the cpp wrapper functions.
        # Note that everything other than private/protected currently gets lumped
        # into C++ "public"; this is suboptimal but hard to improve on easily, and
        # is probably adequate for our purposes.
        cpp_vis = {}
        cpp_vis["public"] = []
        cpp_vis["protected"] = []
        cpp_vis["private"] = []
        for mi in t.tmethods:
            if mi.metadata == None:
                continue

            include = False
            for md in mi.metadata:
                if md.name == "cppcall":
                    include = True

            if not include:
                continue

            if mi.needRest():
                raise Error("You cannot specify [cppcall] for a method with rest args (%s)" % str(mi.name))
                
            if mi.name.ns.isPrivate():
                cpp_vis["private"].append(mi)
            elif mi.name.ns.isProtected():
                cpp_vis["protected"].append(mi)
            else:
                # FIXME: what about internal, etc?
                cpp_vis["public"].append(mi)

        prefixes = { TRAIT_Setter : "call_set_", TRAIT_Getter: "call_get_", TRAIT_Method: "call_" }
        for v in ["public", "protected", "private"]:
            if len(cpp_vis[v]) == 0:
                continue
            out.println("%s:" % v)
            out.indent += 1
            # sort by methodname, for a predictable output
            cpp_vis[v].sort(None, lambda mi: "%s%s" % (prefixes[mi.kind], mi.name.name))
            for mi in cpp_vis[v]:
                args = self.get_args_info(t,mi)
                ret_traits = self.lookupTraits(mi.returnType)
                ret_ctype = ret_traits.ctype
                if mi.kind == TRAIT_Setter:
                    ret_ctype = CTYPE_VOID
                ret_typedef = TYPEMAP_RETTYPE_GCREF[ret_ctype](ret_traits)
                for i in range(0, mi.optional_count+1):
                    arglist = ', '.join(map(lambda (argt, arg_typedef, argname): "%s %s" % (arg_typedef, argname), args[1:]))
                    # "inline" rather than "REALLY_INLINE" -- it's not essential to inline
                    # this, so let the compiler decide to deinline if it so chooses
                    out.println("inline %s %s%s(%s)" % (ret_typedef, prefixes[mi.kind], mi.name.name, arglist))
                    out.println("{")
                    out.indent += 1
                    # bah, does Python not have an equivalent to JS Array.some?
                    needcore = False
                    for (argt, arg_typedef, argname) in args[1:]:
                        needcore = needcore or TYPEMAP_TO_ATOM_NEEDS_CORE[argt.ctype]
                    if needcore:
                        # explicitly cast to AvmCore* because it might be an only-forward-declared subclass
                        out.println("avmplus::AvmCore* const core = ((avmplus::AvmCore*)(this->core()));")
                    arglist = ', '.join(map(lambda (argt, arg_typedef, argname): TYPEMAP_TO_ATOM[argt.ctype](argname), args))
                    argc = len(args)-1
                    if t.is_interface:
                        # for interfaces, must look up by name. we could probably improve this 
                        # by hooking into the IMT cache, but I'm skeptical it's necessary.
                        out.println("const avmplus::Multiname* const mn = this->traits()->pool->precomputedMultiname(%d);" % mi.name_index)
                        if mi.kind == TRAIT_Getter:
                            out.println("avmplus::Atom const result = this->toplevel()->getproperty(this->atom(), mn, this->vtable);")
                        elif mi.kind == TRAIT_Setter:
                            argt, arg_typedef, argname = args[1]
                            out.println("this->toplevel()->setproperty(this->atom(), mn, %s, this->vtable);" % (TYPEMAP_TO_ATOM[argt.ctype](argname)))
                            out.println("avmplus::Atom const result = undefinedAtom;")
                        else:
                            out.println("avmplus::Atom args[%d] = { %s };" % (len(args), arglist))
                            out.println("avmplus::Atom const result = this->toplevel()->callproperty(this->atom(), mn, %d, args, this->vtable);" % (argc))
                    else:
                        idx = mi.vtable_index
                        if mi.kind == TRAIT_Setter:
                            idx += 1
                        out.println("avmplus::MethodEnv* const method = vtable->methods[%d];" % idx)
                        if argc == 0:
                            out.println("avmplus::Atom const result = method->coerceEnter(%s);" % arglist)
                        else:
                            out.println("avmplus::Atom args[%d] = { %s };" % (len(args), arglist))
                            out.println("avmplus::Atom const result = method->coerceEnter(%d, args);" % (argc))
                    if ret_ctype == CTYPE_VOID:
                        out.println("AvmAssert(result == undefinedAtom); (void)result;")
                    else:
                        out.println("return %s;" % TYPEMAP_ATOM_TO_GCREF[ret_ctype]("result",ret_traits))
                    out.indent -= 1
                    out.println("}")
                    args.pop()
                    # Note the "break" here: if you remove it, we'll generate additional stubs, one for each 'default' argument.
                    # I'm omitting these because IMHO it's not necessary; only select C++ -> AS3 call stubs will be generated,
                    # and the need for supporting all default arguments should be effectively nil.
                    break
            out.indent -= 1

        if t.interfaces != None and len(t.interfaces) > 0:
            out.println("public:")
            out.indent += 1
            for i in t.interfaces:
                if isinstance(i, Multiname):
                    for ns in i.nsset:
                        if str(ns) == "" or str(ns) == "private":
                            continue
                        intf = self.lookupTraits(str(ns) + "::" + i.name)
                else:
                    intf = self.lookupTraits(i)
                intf_typedef = TYPEMAP_RETTYPE_GCREF[intf.ctype](intf)
                out.println("REALLY_INLINE %s as_%s()" % (intf_typedef, intf.cppname()))
                out.println("{")
                out.indent += 1
                # We can't use static_cast<> because the subclass might be only forward-declared at this point;
                out.println("return GCRef<%s>((%s*)this);" % (intf.fqcppname(),intf.fqcppname()))
                out.indent -= 1
                out.println("}")
            out.indent -= 1


    def emitSlotDeclarations(self, out, t, sortedSlots, slotsTypeInfo, closingSemi):
        out.println("private:")
        out.indent += 1
        out.println("friend class %s::SlotOffsetsAndAsserts;" % opts.nativeIDNS)
        if t.cpp_friend_classes != None:
            for f in t.cpp_friend_classes:
                out.println("friend class %s;" % f)
        out.indent -= 1
        if (len(t.slots) > 0):
            for slot in sortedSlots:
                assert slot.kind in (TRAIT_Slot, TRAIT_Const)
                (slotCType, slotArgType, slotRetType, slotMemberType) = slotsTypeInfo[id(slot)]
                slotMemberName = to_cname(slot.name)
                slotAccessorName = to_cname(slot.name.name) # omit the ns
                if slot.ns.isPublic():
                    out.println("public:")
                else:
                    out.println("protected:")
                out.indent += 1
                suffix = ""
                if slotCType == CTYPE_BOOLEAN:
                    suffix = " != 0"
                out.println("REALLY_INLINE %s get_%s() const { return %s.m_%s%s; }" % (slotRetType, slotAccessorName, t.slotsInstanceName, slotMemberName, suffix));
                if slot.kind == TRAIT_Slot:
                    out.println("REALLY_INLINE void set_%s(%s newVal) { %s.m_%s = newVal; }" % (slotAccessorName, slotArgType, t.slotsInstanceName, slotMemberName))
                elif slot.kind == TRAIT_Const:
                    out.println("REALLY_INLINE void setconst_%s(%s newVal) { %s.m_%s = newVal; }" % (slotAccessorName, slotArgType, t.slotsInstanceName, slotMemberName))
                out.indent -= 1
            out.println("private:")
            out.indent += 1
            out.println("%s::%s %s%s" % (opts.nativeIDNS, t.slotsStructName, t.slotsInstanceName,closingSemi) )
            out.indent -= 1

    def emitStructDeclarationsForTraits(self, out, t, visitedGlueClasses):
        
        if (t.fqcppname() in visitedGlueClasses):
            if (len(t.slots) == 0):
                return
                raise Error('C++ glue classes for AS3 classes that have slots may only be referenced by metadata for one AS3 class: %s(%s)' % (t.name, t.fqcppname()))

        visitedGlueClasses.add(t.fqcppname())
        if (t.fqcppname() in GLUECLASSES_WITHOUT_SLOTS):
            return
        
        sortedSlots,slotsTypeInfo = self.sortSlots(t)
        self.emitDeclareSlotClass(out, t, sortedSlots, slotsTypeInfo)
        
        if not t.is_synthetic:
            out.backslash += 1
            out.println('#define DECLARE_SLOTS_%s' % t.cppname())
            out.indent += 1
            self.emitConstructDeclarations(out, t, sortedSlots, slotsTypeInfo)
            self.emitMethodWrappers(out, t)
            self.emitSlotDeclarations(out, t, sortedSlots, slotsTypeInfo, "")
            out.indent -= 1
            out.backslash -= 1
            out.println('')

        out.println('//-----------------------------------------------------------')
        out.println('')

    def emitOneSyntheticClass(self, out, t):
        sortedSlots,slotsTypeInfo = self.sortSlots(t)
        out.println('//-----------------------------------------------------------')
        out.println('// %s' % str(t.name))
        out.println('//-----------------------------------------------------------')
        base = self.lookupTraits(t.base)
        baseclassname = base.fqcppname()
        either_is_interface = (t.is_interface or (t.itraits != None and t.itraits.is_interface))
        out.println("class %s : public %s" % (t.cppname(), baseclassname))
        out.println("{")
        if t.is_gc_exact:
            out.indent += 1
            out.println("GC_DECLARE_EXACT_METHODS")
            out.indent -= 1
        if not either_is_interface:
            self.emitConstructDeclarations(out, t, sortedSlots, slotsTypeInfo)
        self.emitMethodWrappers(out, t)
        self.emitSlotDeclarations(out, t, sortedSlots, slotsTypeInfo, ";")
        out.println("protected:")
        out.indent += 1
        if t.itraits != None:
            out.println("inline explicit %s(VTable* cvtable) : %s(cvtable) { createVanillaPrototype(); }" % (t.cppname(), baseclassname))
        else:
            out.println("friend class %s;" % t.ctraits.fqcppname())
            out.println("REALLY_INLINE explicit %s(VTable* ivtable, ScriptObject* delegate) : %s(ivtable, delegate) {}" % (t.cppname(), baseclassname))
        out.indent -= 1
        out.println("private:")
        out.indent += 1
        out.println("explicit %s(const %s&); // unimplemented" % (t.cppname(), t.cppname()))
        out.println("void operator=(const %s&); // unimplemented" % t.cppname())
        out.indent -= 1
        out.println("};")
        out.println('')
        if t.is_gc_exact:
            out.println("#define %s_isExactInterlock 1" % to_cname(t.fqcppname()))

    def emitSyntheticClasses(self, out):
        for i in range(0, len(self.abc.classes)):
            t = self.abc.classes[i]
            # emit Object first, since Class is likely to reference it
            if t.itraits.is_synthetic:
                self.emitOneSyntheticClass(out, t.itraits)
            if t.is_synthetic:
                self.emitOneSyntheticClass(out, t)

    def countTracedSlots(self, sortedSlots):
        numTracedSlots = 0
        for slot in sortedSlots:
            if (slot is not None):
                slotTraits = self.lookupTraits(slot.type)
                if slotTraits.ctype in [CTYPE_ATOM, CTYPE_STRING, CTYPE_NAMESPACE, CTYPE_OBJECT]:
                    numTracedSlots += 1
            else:
                numTracedSlots += 1
        return numTracedSlots
    
    # IMPORTANT NOTE: The code for emitting gcTrace() bodies is essentially
    # replicated in exactgc.as (for handwritten C++ classes) and in nativegen.py 
    # (for synthetically-generated C++ classes representing pure AS3 builtins).
    # This is a highly undesirable situation which is intended to be temporary;
    # we should really unify this code into a single place. At the time of
    # this writing, however (May 2011) it's an expediency we are willing to live with.
    # Until these are unified, please keep in mind that any changes to one may
    # necessitate similar changes to the other.
    def emitSyntheticClassGcTrace(self, out, t, sortedSlots,slotsTypeInfo):
        base = self.lookupTraits(t.base)
        out.println("bool %s::gcTrace(MMgc::GC* gc, size_t _xact_cursor)" % t.fqcppname())
        out.println("{")
        out.indent += 1
        out.println("(void)gc;")
        out.println("(void)_xact_cursor;")
        out.indent -= 1
        out.println("#ifndef GC_TRIVIAL_TRACER_%s" % t.cppname())
        out.indent += 1
        out.println("m_slots_%s.gcTracePrivateProperties(gc);" % t.cppname())
        out.indent -= 1
        out.println("#endif")
        out.indent += 1
        if base != None:
            out.println("%s::gcTrace(gc, 0);" % base.fqcppname())
            out.println("(void)(%s_isExactInterlock != 0);" % to_cname(base.fqcppname()))
        out.println("return false;")
        out.indent -= 1
        out.println("}")

        numTracedSlots = self.countTracedSlots(sortedSlots)

        out.println("#ifdef DEBUG")
        if numTracedSlots > 0:
            out.println("const uint32_t %s::gcTracePointerOffsets[] = {" % t.fqcppname())
            out.indent += 1
            anonCount = 0
            for slot in sortedSlots:
                if (slot is not None):
                    slotTraits = self.lookupTraits(slot.type)
                    if slotTraits.ctype in [CTYPE_ATOM, CTYPE_STRING, CTYPE_NAMESPACE, CTYPE_OBJECT]:
                        out.println('offsetof(%s, m_slots_%s.m_%s),' % (t.fqcppname(), t.cppname(), to_cname(slot.name)))
                else:
                    out.println('offsetof(%s, m_slots_%s.__anonymous_slot_%u),' % (t.fqcppname(), t.cppname(), anonCount))
                    anonCount = anonCount + 1
            out.println("0};")
            out.indent -= 1

        out.println("MMgc::GCTracerCheckResult %s::gcTraceOffsetIsTraced(uint32_t off) const" % t.fqcppname())
        out.println("{")
        out.indent += 1
        out.println("MMgc::GCTracerCheckResult result;")
        out.println("(void)off;")
        out.println("(void)result;")
        if base != None:
            out.println("if ((result = %s::gcTraceOffsetIsTraced(off)) != MMgc::kOffsetNotFound) return result;" % base.fqcppname())
        if numTracedSlots > 0:
            out.println("return MMgc::GC::CheckOffsetIsInList(off,gcTracePointerOffsets,%d);" % numTracedSlots)
        else:
            out.println("return MMgc::kOffsetNotFound;")
        out.indent -= 1
        out.println("}")
        out.println("#endif // DEBUG")

    def emitMethodBodiesForTraits(self, out, t, visitedGlueClasses):
        
        if (t.fqcppname() in visitedGlueClasses):
            if (len(t.slots) == 0):
                return
                raise Error('C++ glue classes for AS3 classes that have slots may only be referenced by metadata for one AS3 class: %s(%s)' % (t.name, t.fqcppname()))

        visitedGlueClasses.add(t.fqcppname())
        if (t.fqcppname() in GLUECLASSES_WITHOUT_SLOTS):
            return
        
        sortedSlots,slotsTypeInfo = self.sortSlots(t)
        self.emitConstructStubs(out, t, sortedSlots, slotsTypeInfo)
        if t.is_synthetic and t.is_gc_exact:
            self.emitSyntheticClassGcTrace(out, t, sortedSlots, slotsTypeInfo)

    def printStructAsserts(self, out, abc):
        out.println('class SlotOffsetsAndAsserts')
        out.println('{')
        out.println('public:')
        out.indent += 1
        out.println('#ifdef DEBUG')
        out.println('static uint32_t getSlotOffset(Traits* t, int nameId);')
        out.println('static uint32_t getMethodIndex(Traits* t, int nameId);')
        out.println('static uint32_t getGetterIndex(Traits* t, int nameId);')
        out.println('static uint32_t getSetterIndex(Traits* t, int nameId);')
        out.println('#endif // DEBUG');
        out.println('enum {')
        out.indent += 1
        visitedNativeClasses = set()
        for i in range(0, len(abc.classes)):
            c = abc.classes[i]
            if c.itraits.is_interface:
                continue
            if (c.fqcppname() not in visitedNativeClasses):
                visitedNativeClasses.add(c.fqcppname())
                out.println('kSlotsOffset_%s = %s,' % (to_cname(c.fqcppname()), c.cpp_offsetof_slots()))
            if (c.itraits.fqcppname() not in visitedNativeClasses):
                visitedNativeClasses.add(c.itraits.fqcppname())
                out.println('kSlotsOffset_%s = %s,' % (to_cname(c.itraits.fqcppname()), c.itraits.cpp_offsetof_slots()))
        out.println('kSlotsOffset_fnord')
        out.indent -= 1
        out.println('};')

        out.println('#ifdef DEBUG')
        for i in range(0, len(abc.classes)):
            c = abc.classes[i]
            if c.itraits.is_interface:
                continue
            out.println('static void check_%s(Traits* ctraits, Traits* itraits);' % to_cname(c.fqcppname()))
        out.println('#endif');

        out.indent -= 1
        out.println('};')
        out.println('#ifdef DEBUG');
        for i in range(0, len(abc.classes)):
            c = abc.classes[i]
            if c.itraits.is_interface:
                continue;
            out.println('REALLY_INLINE void SlotOffsetsAndAsserts::check_%s(Traits* ctraits, Traits* itraits)' % to_cname(c.fqcppname()))
            out.println('{')
            out.indent += 1
            out.println('(void)ctraits; (void)itraits;')
            self.printStructAssertsForTraits(out, c, 'ctraits')
            if self.needsInstanceSlotsStruct(c):
                self.printStructAssertsForTraits(out, c.itraits, 'itraits')
            out.indent -= 1
            out.println('}')
        out.println('#endif // DEBUG')

    def printStructAssertsForTraits(self, out, t, traitsVarName):
        if (len(t.slots) > 0):
            out.println('MMGC_STATIC_ASSERT(offsetof(%s, %s) == kSlotsOffset_%s);' % (t.fqcppname(), t.slotsInstanceName, to_cname(t.fqcppname())))
            out.println('MMGC_STATIC_ASSERT(offsetof(%s, %s) <= 0xFFFF);' % (t.fqcppname(), t.slotsInstanceName))
            out.println('MMGC_STATIC_ASSERT(sizeof(%s) <= 0xFFFF);' % (t.fqcppname()))
            for slot in t.slots:
                if slot != None:
                    out.println('AvmAssert(getSlotOffset(%s, %u) == (offsetof(%s, %s) + offsetof(%s, m_%s)));' 
                        % (traitsVarName, self.namesDict[id(slot.name)], t.fqcppname(), t.slotsInstanceName, t.slotsStructName, to_cname(slot.name)))
            for mi in t.tmethods:
                if mi.kind == TRAIT_Method:
                    out.println('AvmAssert(getMethodIndex(%s, %u) == %d); // %s' 
                        % (traitsVarName, self.namesDict[id(mi.name)], mi.vtable_index, str(mi.name)))
                elif mi.kind == TRAIT_Getter:
                    out.println('AvmAssert(getGetterIndex(%s, %u) == %d); // %s' 
                        % (traitsVarName, self.namesDict[id(mi.name)], mi.vtable_index, str(mi.name)))
                elif mi.kind == TRAIT_Setter:
                    # note the "+1" on vtable_index for Setters
                    out.println('AvmAssert(getSetterIndex(%s, %u) == %d); // %s' 
                        % (traitsVarName, self.namesDict[id(mi.name)], mi.vtable_index+1, str(mi.name)))


    def argTraits(self, receiver, m):
        argtraits = [ receiver ]
        for i in range(0, len(m.paramTypes)):
            argtraits.append(self.lookupTraits(m.paramTypes[i]))
        return argtraits

    def thunkInfo(self, m):
        ret_traits = self.lookupTraits(m.returnType)
        ret_ctype = ret_traits.ctype
        if m.kind == TRAIT_Setter:
            ret_ctype = CTYPE_VOID
        # for return types of thunks, everything but double maps to Atom
        if ret_ctype != CTYPE_DOUBLE and ret_ctype!= CTYPE_FLOAT :
            thunk_ret_ctype = CTYPE_ATOM
        else:
            thunk_ret_ctype = ret_ctype
        decl = "%s %s_thunk(MethodEnv* env, uint32_t argc, Atom* argv)" % (TYPEMAP_RETTYPE[thunk_ret_ctype], m.native_id_name)
        return ret_traits,ret_ctype,thunk_ret_ctype,decl

    def emitThunkProto(self, out, receiver, m):
        ret_traits,ret_ctype,thunk_ret_ctype,decl = self.thunkInfo(m)
        out.println('extern ' + decl + ";");

    def emitThunkBody(self, out, receiver, m):
        ret_traits,ret_ctype,thunk_ret_ctype,decl = self.thunkInfo(m)

        unbox_receiver = self.calc_unbox_this(m)

        out.println(decl);
        out.println("{");
        out.indent += 1;

        if opts.thunkvprof:
            out.println('_nvprof("%s", 1);' % name)

        param_count = len(m.paramTypes);
        optional_count = m.optional_count;

        argtraits = self.argTraits(receiver, m)

        argszprev = "0"
        out.println("enum {");
        out.indent += 1;
        for i in range(0, len(argtraits)):
            if i == 0:
                out.println("argoff0 = 0");
            else:
                out.println(", argoff%d = argoff%d + %s" % (i, i-1, argszprev));
            argszprev = "AvmThunkArgSize_%s" % TYPEMAP_ARGTYPE_SUFFIX[argtraits[i].ctype];
        out.indent -= 1;
        out.println("};");

        if m.needRest():
            out.println("const uint32_t argoffV = argoff"+str(len(argtraits)-1)+" + "+argszprev+";");

        args = []

        arg0_typedef = argtraits[0].cpp_argument_name()
        assert(argtraits[0].ctype in [CTYPE_OBJECT,CTYPE_STRING,CTYPE_NAMESPACE])
        if unbox_receiver:
            val = "AvmThunkUnbox_AvmAtomReceiver("+arg0_typedef+", argv[argoff0])";
        else:
            val = "AvmThunkUnbox_AvmReceiver("+arg0_typedef+", argv[argoff0])";
        args.append((val, arg0_typedef))

        for i in range(1, len(argtraits)):
            arg_ctype = argtraits[i].ctype
            arg_typedef = argtraits[i].cpp_unboxing_argument_name()
            val = "AvmThunkUnbox_%s(%s, argv[argoff%d])" % (TYPEMAP_ARGTYPE_SUFFIX[arg_ctype], arg_typedef, i)
            if arg_ctype == CTYPE_OBJECT:
                unboxname = argtraits[i].fqcppname()
                if unboxname != None:
                    val = "(%s*)%s" % (unboxname, val)
            # argtraits includes receiver at 0, optionalValues does not
            if i > param_count - optional_count:
                dct,defval,defvalraw = self.abc.default_ctype_and_value(m.optionalValues[i-1]);
                if dct != arg_ctype:
                    defval = "AvmThunkCoerce_%s_%s(%s)" % (TYPEMAP_ARGTYPE_SUFFIX[dct], TYPEMAP_ARGTYPE_SUFFIX[arg_ctype], defval)
                val = "(argc < "+str(i)+" ? "+defval+" : "+val+")";
                if arg_ctype == CTYPE_OBJECT:
                    coercename = argtraits[i].fqcppname()
                    if coercename != None:
                        val = "(%s*)%s" % (coercename, val)
            args.append((val, arg_typedef))

        if m.needRest():
            args.append(("(argc <= "+str(param_count)+" ? NULL : argv + argoffV)", "Atom*"))
            args.append(("(argc <= "+str(param_count)+" ? 0 : argc - "+str(param_count)+")", "uint32_t"))

        if not m.hasOptional() and not m.needRest():
            out.println("(void)argc;");

        out.println("(void)env;") # avoid "unreferenced formal parameter" in non-debugger builds
        if m.receiver == None:
            rec_type = BASE_INSTANCE_NAME+"*"
        else:
            rec_type = m.receiver.cpp_argument_name()
        out.println("%s const obj = %s;" % (rec_type, args[0][0]))

        if ret_ctype != CTYPE_VOID:
            out.prnt("%s const ret = " % ret_traits.cpp_return_name())

        if m.receiver == None:
            out.prnt("%s(obj" % m.native_method_name)
            need_comma = True
        else:
            if m.receiver.method_map_name != m.receiver.fqcppname():
                native_method_name = m.receiver.method_map_name + "::" + m.native_method_name
            else:
                native_method_name = m.native_method_name
            out.prnt("obj->%s(" % native_method_name)
            need_comma = False

        if len(args) > 1:
            out.println("")
            out.indent += 1
            for i in range(1, len(args)):
                if need_comma:
                    out.prnt(", ")
                out.println("%s" % args[i][0]);
                need_comma = True
            out.indent -= 1
        out.println(");")

        if ret_ctype != CTYPE_VOID:
            ret_result = TYPEMAP_THUNKRETTYPE[ret_ctype] % "ret";
        else:
            ret_result = "undefinedAtom";
        out.println("return %s;" % ret_result)
        out.indent -= 1
        out.println("}")

    # inefficient, but doesn't really matter
    def find_override_base(self, mi):
        if mi.override and mi.receiver.base != None:
            bt = self.lookupTraits(mi.receiver.base)
            for j in range(0, len(bt.tmethods)):
                bmi = bt.tmethods[j]
                if bmi.name.name == mi.name.name and bmi.name.ns == mi.name.ns and bmi != mi:
                    #print "OVER", str(mi.name), str(mi.receiver)
                    #print "BASE", str(bmi.name), str(bmi.receiver)
                    return bt,bmi
        return None,None

    def calc_unbox_this(self, mi):
        if mi.unbox_this < 0:
            bt,bmi = self.find_override_base(mi)
            if bmi == None:
                mi.unbox_this = 0 # no need to unbox
            elif bmi.unbox_this > 0:
                mi.unbox_this = 1 # unbox_this is sticky, down the inheritance tree
            elif len(bmi.paramTypes) > 0:
                param0 = self.lookupTraits(bmi.paramTypes[0])
                if mi.receiver.ctype in [CTYPE_OBJECT,CTYPE_STRING,CTYPE_NAMESPACE] and param0.ctype == CTYPE_ATOM:
                    mi.unbox_this = 1 # unbox_this is sticky, down the inheritance tree
        return mi.unbox_this > 0

    def lookupTraits(self, name):
        name = str(name)
        if self.lookup_traits == None:
            self.lookup_traits = {}
            self.lookup_traits["*"] = NULL
            self.lookup_traits["void"] = UNDEFINED
            for a in self.abcs:
                for t in a.scripts:
                    if self.lookup_traits.has_key(str(t)):
                        raise Error("duplicate name found: " + str(t))
                    self.lookup_traits[str(t)] = t
                for t in a.classes:
                    if self.lookup_traits.has_key(str(t)):
                        raise Error("duplicate name found: " + str(t))
                    self.lookup_traits[str(t)] = t
                for t in a.instances:
                    if self.lookup_traits.has_key(str(t)):
                        raise Error("duplicate name found: " + str(t))
                    self.lookup_traits[str(t)] = t
        if not self.lookup_traits.has_key(name):
            raise Error("name not found: " + name)
        return self.lookup_traits[name]

    def gatherThunk(self, receiver, m):
        if m.native_id_name == None:
            raise Error("name not specified for native method " + str(m.name))
        self.all_thunks.append((receiver, m))

    def processClass(self, b):
        c = b.value
        self.processTraits(c)
        self.processTraits(c.itraits)

    def processMethod(self, receiver, m):
        if m.isNative():
            self.gatherThunk(receiver, m)

    def assignMethodIndices(self, t):

        if t.tmethod_index_count >= 0:
            return

        methods = {}
        getters = {}
        setters = {}

        base = None
        t.tmethod_index_count = 0
        t.tmethods_name_map = {}
        t.tmethods_name_map[TRAIT_Method] = {}
        t.tmethods_name_map[TRAIT_Getter] = {}
        t.tmethods_name_map[TRAIT_Setter] = {}
        if t.base != None:
            base = self.lookupTraits(t.base)
            self.assignMethodIndices(base)
            t.tmethod_index_count = base.tmethod_index_count
            for kind in [TRAIT_Method,TRAIT_Getter,TRAIT_Setter]:
                for n in base.tmethods_name_map[kind].keys():
                    mi = base.tmethods_name_map[kind][n]
                    if not mi.name.ns.isPrivate():
                        t.tmethods_name_map[kind][n] = mi;

        # note: this doesn't do all the necessary is-this-a-legal-override
        # checking; that's going to be done at runtime anyway, so we won't
        # repeat it all here.
        #
        # overrides don't get new slotIDs, they re-use an existing one.
        #
        # the tricky part here is get/set; the first instance of get OR set
        # for a given name, in any of our ancestors *or* ourself, allocates
        # *two* slotids for both; when checking for override for these we
        # must only allocate new slots if *neither* get *nor* set is overridden.
        # but, they can arrive in any order, so we must be careful.
        getset_ids = {}
        for kind in [TRAIT_Method,TRAIT_Getter,TRAIT_Setter]:
            for mi in t.tmethods_name_map[kind].values():
                assert(isinstance(mi.name, QName))
                if mi.name.ns.isProtected():
                    n = "__protected__::" + mi.name.name
                else:
                    n = str(mi.name)
                getset_ids[n] = mi.vtable_index

        for i in range(0, len(t.tmethods)):
            mi = t.tmethods[i]
            assert(isinstance(mi.name, QName))
            if mi.name.ns.isProtected():
                n = "__protected__::" + mi.name.name
            else:
                n = str(mi.name)
            assert mi.override == t.tmethods_name_map[mi.kind].has_key(n)
            if mi.override:
                mi.vtable_index = getset_ids[n]
            else:
                if getset_ids.has_key(n):
                    assert mi.kind in [TRAIT_Getter, TRAIT_Setter]
                    mi.vtable_index = getset_ids[n]
                else:
                    mi.vtable_index = t.tmethod_index_count
                    getset_ids[n] = mi.vtable_index
                    if mi.kind in [TRAIT_Getter, TRAIT_Setter]:
                        t.tmethod_index_count += 2
                    else:
                        t.tmethod_index_count += 1
            t.tmethods_name_map[mi.kind][n] = mi

    def processTraits(self, s):
        self.assignMethodIndices(s)
        if s.init != None:
            if s.init.isNative():
                raise Error("native constructors are not allowed: " + str(s))
            self.processMethod(s, s.init)
        for i in range(0, len(s.members)):
            if s.members[i].kind in [TRAIT_Method,TRAIT_Getter,TRAIT_Setter]:
                self.processMethod(s, s.members[i])
            elif s.members[i].kind in [TRAIT_Class]:
                self.processClass(s.members[i]);

ngen = AbcThunkGen();
abcGenFor = None
abcGenName = ""
for file in args:
    try:
        f = open(file,"rb")
        data = f.read()
    finally:
        f.close()

    abcScriptName = os.path.splitext(os.path.split(file)[1])[0]
    #print "read %s" % abcScriptName
    abcGenFor = Abc(data, abcScriptName)
    ngen.addAbc(abcGenFor)
    abcGenName = os.path.splitext(file)[0]

if abcGenFor:
    hfile = None
    clsfile = None
    cppfile = None
    try:
        hfile = open(abcGenName+".h","w")
        clsfile = open(abcGenName+"-classes.hh","w")
        cppfile = open(abcGenName+".cpp","w")
        h = IndentingPrintWriter(hfile)
        cls = IndentingPrintWriter(clsfile)
        c = IndentingPrintWriter(cppfile)
        ngen.emit(abcGenFor, abcScriptName, h, cls, c);
#    except Exception, e:
#        sys.stderr.write("ERROR: "+str(e)+"\n")
#        exit(1)
    finally:
        if hfile != None:
            hfile.close()
        if clsfile != None:
            clsfile.close()
        if cppfile != None:
            cppfile.close()
