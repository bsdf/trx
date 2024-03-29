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
    var SECTION = "15.8.2.16";
    var VERSION = "ECMA_1";
    startTest();
    var TITLE   = "Math.sin(x)";

    writeHeaderToLog( SECTION + " "+ TITLE);

    var testcases = getTestCases();
    test();

function getTestCases() {
    var array = new Array();
    var item = 0;

    array[item++] = new TestCase( SECTION,  "Math.sin.length",      1,              Math.sin.length );
  /*thisError="no error";
    try{
        Math.sin();
    }catch(e:Error){
        thisError=(e.toString()).substring(0,26);
    }finally{//print(thisError);
        array[item++] = new TestCase( SECTION,   "Math.sin()","ArgumentError: Error #1063",thisError);
    }
    array[item++] = new TestCase( SECTION,  "Math.sin()",           Number.NaN,     Math.sin() );*/
    array[item++] = new TestCase( SECTION,  "Math.sin(null)",       0,              Math.sin(null) );
    array[item++] = new TestCase( SECTION,  "Math.sin(void 0)",     Number.NaN,     Math.sin(void 0) );
    array[item++] = new TestCase( SECTION,  "Math.sin(false)",      0,              Math.sin(false) );
    array[item++] = new TestCase( SECTION,  "Math.sin('2.356194490192')",    0.7071067811867916,    Math.sin('2.356194490192') );

    array[item++] = new TestCase( SECTION,  "Math.sin(NaN)",        Number.NaN,     Math.sin(Number.NaN) );
    array[item++] = new TestCase( SECTION,  "Math.sin(0)",          0,              Math.sin(0) );
    array[item++] = new TestCase( SECTION,  "Math.sin(-0)",         -0,             Math.sin(-0));
    array[item++] = new TestCase( SECTION,  "Math.sin(Infinity)",   Number.NaN,     Math.sin(Number.POSITIVE_INFINITY));
    array[item++] = new TestCase( SECTION,  "Math.sin(-Infinity)",  Number.NaN,     Math.sin(Number.NEGATIVE_INFINITY));
    array[item++] = new TestCase( SECTION,  "Math.sin(0.7853981633974)",    0.7071067811865134,    Math.sin(0.7853981633974));
    array[item++] = new TestCase( SECTION,  "Math.sin(1.570796326795)",     1,                  Math.sin(1.570796326795));
    array[item++] = new TestCase( SECTION,  "Math.sin(2.356194490192)",     0.7071067811867916,    Math.sin(2.356194490192));
    array[item++] = new TestCase( SECTION,  "Math.sin(3.14159265359)",      -2.0682311115474694e-13,                  Math.sin(3.14159265359));

    return ( array );
}
