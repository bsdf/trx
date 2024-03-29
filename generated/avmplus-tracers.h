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
 * Portions created by the Initial Developer are Copyright (C) 2010
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

/* machine generated file via utils/exactgc.as -- do not edit */

#define avmplus_AbcEnv_isExactInterlock 1
#define avmplus_AbcFile_isExactInterlock 1
#define avmplus_AbcInfo_isExactInterlock 1
#define avmplus_ArgumentErrorClass_isExactInterlock 1
#define avmplus_ArgumentErrorObject_isExactInterlock 1
#define avmplus_ArrayClass_isExactInterlock 1
#define avmplus_ArrayObject_isExactInterlock 1
#define avmplus_AtomContainer_isExactInterlock 1
#define avmplus_AttributeE4XNode_isExactInterlock 1
#define avmplus_BooleanClass_isExactInterlock 1
#define avmplus_ByteArrayClass_isExactInterlock 1
#define avmplus_ByteArrayObject_isExactInterlock 1
#define avmplus_CDATAE4XNode_isExactInterlock 1
#define avmplus_ClassClass_isExactInterlock 1
#define avmplus_ClassClosure_isExactInterlock 1
#define avmplus_CommentE4XNode_isExactInterlock 1
#define avmplus_DateClass_isExactInterlock 1
#define avmplus_DateObject_isExactInterlock 1
#define avmplus_Debugger_isExactInterlock 1
#define avmplus_DebuggerMethodInfo_isExactInterlock 1
#define avmplus_DefinitionErrorClass_isExactInterlock 1
#define avmplus_DefinitionErrorObject_isExactInterlock 1
#define avmplus_Domain_isExactInterlock 1
#define avmplus_DomainEnv_isExactInterlock 1
#define avmplus_DoubleVectorClass_isExactInterlock 1
#define avmplus_DoubleVectorObject_isExactInterlock 1
#define avmplus_E4XNode_isExactInterlock 1
#define avmplus_E4XNodeAux_isExactInterlock 1
#define avmplus_ElementE4XNode_isExactInterlock 1
#define avmplus_ErrorClass_isExactInterlock 1
#define avmplus_ErrorObject_isExactInterlock 1
#define avmplus_EvalErrorClass_isExactInterlock 1
#define avmplus_EvalErrorObject_isExactInterlock 1
#define avmplus_ExactGCTest_isExactInterlock 1
#define avmplus_ExceptionHandlerTable_isExactInterlock 1
#define avmplus_FunctionClass_isExactInterlock 1
#define avmplus_FunctionEnv_isExactInterlock 1
#define avmplus_FunctionObject_isExactInterlock 1
#define avmplus_HeapHashtable_isExactInterlock 1
#define avmplus_HeapHashtableRC_isExactInterlock 1
#define avmplus_InlineHashtable_isExactInterlock 1
#define avmplus_IntClass_isExactInterlock 1
#define avmplus_IntVectorClass_isExactInterlock 1
#define avmplus_IntVectorObject_isExactInterlock 1
#define avmplus_JSONClass_isExactInterlock 1
#define avmplus_LivePoolNode_isExactInterlock 1
#define avmplus_MathClass_isExactInterlock 1
#define avmplus_MethodClosure_isExactInterlock 1
#define avmplus_MethodClosureClass_isExactInterlock 1
#define avmplus_MethodEnv_isExactInterlock 1
#define avmplus_MethodEnvProcHolder_isExactInterlock 1
#define avmplus_MethodInfo_isExactInterlock 1
#define avmplus_MethodInfoProcHolder_isExactInterlock 1
#define avmplus_MethodSignature_isExactInterlock 1
#define avmplus_Namespace_isExactInterlock 1
#define avmplus_NamespaceClass_isExactInterlock 1
#define avmplus_NamespaceSet_isExactInterlock 1
#define avmplus_NativeErrorClass_isExactInterlock 1
#define avmplus_NumberClass_isExactInterlock 1
#define avmplus_ObjectClass_isExactInterlock 1
#define avmplus_ObjectVectorClass_isExactInterlock 1
#define avmplus_ObjectVectorObject_isExactInterlock 1
#define avmplus_PIE4XNode_isExactInterlock 1
#define avmplus_PoolObject_isExactInterlock 1
#define avmplus_ProxyClass_isExactInterlock 1
#define avmplus_ProxyObject_isExactInterlock 1
#define avmplus_QCache_isExactInterlock 1
#define avmplus_QCachedItem_isExactInterlock 1
#define avmplus_QNameClass_isExactInterlock 1
#define avmplus_QNameObject_isExactInterlock 1
#define avmplus_RangeErrorClass_isExactInterlock 1
#define avmplus_RangeErrorObject_isExactInterlock 1
#define avmplus_ReferenceErrorClass_isExactInterlock 1
#define avmplus_ReferenceErrorObject_isExactInterlock 1
#define avmplus_RegExpClass_isExactInterlock 1
#define avmplus_RegExpObject_isExactInterlock 1
#define avmplus_ScopeChain_isExactInterlock 1
#define avmplus_ScopeTypeChain_isExactInterlock 1
#define avmplus_ScriptEnv_isExactInterlock 1
#define avmplus_ScriptEnvMap_isExactInterlock 1
#define avmplus_SecurityErrorClass_isExactInterlock 1
#define avmplus_SecurityErrorObject_isExactInterlock 1
#define avmplus_SourceFile_isExactInterlock 1
#define avmplus_SourceInfo_isExactInterlock 1
#define avmplus_StackTrace_isExactInterlock 1
#define avmplus_StringClass_isExactInterlock 1
#define avmplus_SyntaxErrorClass_isExactInterlock 1
#define avmplus_SyntaxErrorObject_isExactInterlock 1
#define avmplus_TextE4XNode_isExactInterlock 1
#define avmplus_Toplevel_isExactInterlock 1
#define avmplus_Traits_isExactInterlock 1
#define avmplus_TraitsBindings_isExactInterlock 1
#define avmplus_TraitsMetadata_isExactInterlock 1
#define avmplus_TypeErrorClass_isExactInterlock 1
#define avmplus_TypeErrorObject_isExactInterlock 1
#define avmplus_TypedVectorClassBase_isExactInterlock 1
#define avmplus_UIntClass_isExactInterlock 1
#define avmplus_UIntVectorClass_isExactInterlock 1
#define avmplus_UIntVectorObject_isExactInterlock 1
#define avmplus_URIErrorClass_isExactInterlock 1
#define avmplus_URIErrorObject_isExactInterlock 1
#define avmplus_UninitializedErrorClass_isExactInterlock 1
#define avmplus_UninitializedErrorObject_isExactInterlock 1
#define avmplus_VTable_isExactInterlock 1
#define avmplus_VectorBaseObject_isExactInterlock 1
#define avmplus_VectorClass_isExactInterlock 1
#define avmplus_VerifyErrorClass_isExactInterlock 1
#define avmplus_VerifyErrorObject_isExactInterlock 1
#define avmplus_WeakKeyHashtable_isExactInterlock 1
#define avmplus_WeakMethodClosure_isExactInterlock 1
#define avmplus_WeakValueHashtable_isExactInterlock 1
#define avmplus_XMLClass_isExactInterlock 1
#define avmplus_XMLListClass_isExactInterlock 1
#define avmplus_XMLListObject_isExactInterlock 1
#define avmplus_XMLObject_isExactInterlock 1

