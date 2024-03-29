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
 * Portions created by the Initial Developer are Copyright (C) 2005-2006
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

import EmptyFunctionBody.*;

class EmptyFunctionBodyClass {
    function EmptyFunctionBodyClass() {}
    function noReturnNoParams() { return "noReturnNoParams"; }
    function noReturnParams(s:String, b:Boolean) { return s; }
    function noReturnCustomParam(c:Custom) { return new Custom(); }
    function returnNoParams():String { return "returnNoParams"; }
    function returnParams(s:String, b:Boolean):String { return s; }
    function returnCustomNoParams():Custom { return new Custom(); }
}

function noReturnNoParamsNoPackage() { return "noReturnNoParams"; }
function noReturnParamsNoPackage(s:String, b:Boolean) { return s; }
function noReturnCustomParamNoPackage(c:Custom) { return new Custom(); }
function returnNoParamsNoPackage():String { return "returnNoParams"; }
function returnParamsNoPackage(s:String, b:Boolean):String { return s; }
function returnCustomNoParamsNoPackage():Custom { return new Custom(); }


var SECTION = "Definitions";       // provide a document reference (ie, ECMA section)
var VERSION = "AS3";  // Version of JavaScript or ECMA
var TITLE   = "Function Body Parameter/Result Type";       // Provide ECMA section title or a description
var BUGNUMBER = "";

startTest();                // leave this alone

var TESTOBJ;
var s:String = new String("this is a test");
var b:Boolean = new Boolean(true);
var c:Custom = new Custom();

// inside class inside package
TESTOBJ = new TestObj();
AddTestCase( "TESTOBJ.noReturnNoParams()", "noReturnNoParams", TESTOBJ.noReturnNoParams() );
AddTestCase( "TESTOBJ.noReturnParams(s,b)", "this is a test", TESTOBJ.noReturnParams(s,b) );
//Function returns a new Custom object therefore must be compared using strings.
AddTestCase( "TESTOBJ.noReturnCustomParams(c)", "[object Custom]", String(TESTOBJ.noReturnCustomParam(c)) );
AddTestCase( "TESTOBJ.returnNoParams()", "returnNoParams", TESTOBJ.returnNoParams() );
AddTestCase( "TESTOBJ.returnParams(s,b)", "this is a test", TESTOBJ.returnParams(s,b) );
AddTestCase( "TESTOBJ.returnCustomNoParams()", "[object Custom]", String(TESTOBJ.returnCustomNoParams()) );

// inside package outside of class
AddTestCase( "noReturnNoParams()", "noReturnNoParams", noReturnNoParams() );
AddTestCase( "noReturnParams(s,b)", "this is a test", noReturnParams(s,b) );
AddTestCase( "noReturnCustomParams()", "[object Custom]", String(noReturnCustomParam(c)) );
AddTestCase( "returnNoParams()", "returnNoParams", returnNoParams() );
AddTestCase( "returnParams(s,b)", "this is a test", returnParams(s,b) );
AddTestCase( "returnCustomNoParams()", "[object Custom]", String(returnCustomNoParams()) );

// outside package inside class
TESTOBJ = new EmptyFunctionBodyClass();
AddTestCase( "TESTOBJ.noReturnNoParams()", "noReturnNoParams", TESTOBJ.noReturnNoParams() );
AddTestCase( "TESTOBJ.noReturnParams(s,b)", "this is a test", TESTOBJ.noReturnParams(s,b) );
AddTestCase( "TESTOBJ.noReturnCustomParams()", "[object Custom]", String(TESTOBJ.noReturnCustomParam(c)) );
AddTestCase( "TESTOBJ.returnNoParams()", "returnNoParams", TESTOBJ.returnNoParams() );
AddTestCase( "TESTOBJ.returnParams(s,b)", "this is a test", TESTOBJ.returnParams(s,b) );
AddTestCase( "TESTOBJ.returnCustomNoParams()", "[object Custom]", String(TESTOBJ.returnCustomNoParams()) );

// outside package and outside class
AddTestCase( "noReturnNoParamsNoPackage()", "noReturnNoParams", noReturnNoParamsNoPackage() );
AddTestCase( "noReturnParamsNoPackage(s,b)", "this is a test", noReturnParamsNoPackage(s,b) );
AddTestCase( "noReturnCustomParamsNoPackage()", "[object Custom]", String(noReturnCustomParamNoPackage(c)) );
AddTestCase( "returnNoParamsNoPackage()", "returnNoParams", returnNoParamsNoPackage() );
AddTestCase( "returnParamsNoPackage(s,b)", "this is a test", returnParamsNoPackage(s,b) );
AddTestCase( "returnCustomNoParamsNoPackage()", "[object Custom]", String(returnCustomNoParamsNoPackage()) );

test();       // leave this alone.  this executes the test cases and
              // displays results.
