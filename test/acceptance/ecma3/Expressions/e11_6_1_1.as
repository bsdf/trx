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
    var SECTION = "e11_6_1_1";
    var VERSION = "ECMA_1";
    startTest();

    var testcases = getTestCases();

    writeHeaderToLog( SECTION + " The Addition operator ( + )");
    test();

function getTestCases() {
    var array = new Array();
    var item = 0;

    // tests for boolean primitive, boolean object, Object object, a "MyObject" whose value is
    // a boolean primitive and a boolean object,
    var EXP_1 = true; var EXP_2 = false;
    array[item++] = new TestCase(   SECTION,
                                    "var EXP_1 = true; var EXP_2 = false; EXP_1 + EXP_2",
                                    1,
                                    EXP_1 + EXP_2 );
    var EXP_1 = new Boolean(true); var EXP_2 = new Boolean(false);
    array[item++] = new TestCase(   SECTION,
                                    "var EXP_1 = new Boolean(true); var EXP_2 = new Boolean(false); EXP_1 + EXP_2",
                                    1,
                                     EXP_1 + EXP_2 );
    var EXP_1 = new Object(true); var EXP_2 = new Object(false);
    array[item++] = new TestCase(   SECTION,
                                    "var EXP_1 = new Object(true); var EXP_2 = new Object(false); EXP_1 + EXP_2",
                                    1,
                                     EXP_1 + EXP_2 );
    var EXP_1 = new Object(new Boolean(true)); var EXP_2 = new Object(new Boolean(false));
    array[item++] = new TestCase(   SECTION,
                                    "var EXP_1 = new Object(new Boolean(true)); var EXP_2 = new Object(new Boolean(false)); EXP_1 + EXP_2",
                                    1,
                                     EXP_1 + EXP_2 );
    var EXP_1 = new MyObject(true); var EXP_2 = new MyObject(false);
    array[item++] = new TestCase(   SECTION,
                                    "var EXP_1 = new MyObject(true); var EXP_2 = new MyObject(false); EXP_1 + EXP_2",
                                    1,
                                     EXP_1 + EXP_2 );
    var EXP_1 = new MyObject(new Boolean(true)); var EXP_2 = new MyObject(new Boolean(false));
    array[item++] = new TestCase(   SECTION,
                                    "var EXP_1 = new MyObject(new Boolean(true)); var EXP_2 = new MyObject(new Boolean(false)); EXP_1 + EXP_2",
                                    1,
                                    EXP_1 + EXP_2 );
    //var EXP_1 = new MyValuelessObject(true); var EXP_2 = new MyValuelessObject(false);
    //array[item++] = new TestCase(   SECTION,
    //                                "var EXP_1 = new MyValuelessObject(true); var EXP_2 = new MyValuelessObject(false); EXP_1 + EXP_2",
    //                                1,
    //                                 EXP_1 + EXP_2 );
    //var EXP_1 = new MyValuelessObject(new Boolean(true)); var EXP_2 = new MyValuelessObject(new Boolean(false));
    //array[item++] = new TestCase(   SECTION,
    //                                "var EXP_1 = new MyValuelessObject(new Boolean(true)); var EXP_2 = new MyValuelessObject(new Boolean(false)); EXP_1 + EXP_2",
    //                                "truefalse",
    //                                 EXP_1 + EXP_2 );

    // tests for number primitive, number object, Object object, a "MyObject" whose value is
    // a number primitive and a number object
    var EXP_1 = 100; var EXP_2 = -1;
    array[item++] = new TestCase(   SECTION,
                                    "var EXP_1 = 100; var EXP_2 = -1; EXP_1 + EXP_2",
                                    99,
                                     EXP_1 + EXP_2 );
    var EXP_1 = new Number(100); var EXP_2 = new Number(-1);
    array[item++] = new TestCase(   SECTION,
                                    "var EXP_1 = new Number(100); var EXP_2 = new Number(-1); EXP_1 + EXP_2",
                                    99,
                                     EXP_1 + EXP_2 );
    var EXP_1 = new Object(100); var EXP_2 = new Object(-1);
    array[item++] = new TestCase(   SECTION,
                                    "var EXP_1 = new Object(100); var EXP_2 = new Object(-1); EXP_1 + EXP_2",
                                    99,
                                     EXP_1 + EXP_2 );
    var EXP_1 = new Object(new Number(100)); var EXP_2 = new Object(new Number(-1));
    array[item++] = new TestCase(   SECTION,
                                    "var EXP_1 = new Object(new Number(100)); var EXP_2 = new Object(new Number(-1)); EXP_1 + EXP_2",
                                    99,
                                     EXP_1 + EXP_2 );
    var EXP_1 = new MyObject(100); var EXP_2 = new MyObject(-1);
    array[item++] = new TestCase(   SECTION,
                                    "var EXP_1 = new MyObject(100); var EXP_2 = new MyObject(-1); EXP_1 + EXP_2",
                                    99,
                                     EXP_1 + EXP_2 );
    var EXP_1 = new MyObject(new Number(100)); var EXP_2 = new MyObject(new Number(-1));
    array[item++] = new TestCase(   SECTION,
                                    "var EXP_1 = new MyObject(new Number(100)); var EXP_2 = new MyObject(new Number(-1)); EXP_1 + EXP_2",
                                     99,
                                     EXP_1 + EXP_2 );

    //var EXP_1 = new MyValuelessObject(100); var EXP_2 = new MyValuelessObject(-1);
    //array[item++] = new TestCase(   SECTION,
    //                               "var EXP_1 = new MyValuelessObject(100); var EXP_2 = new MyValuelessObject(-1); EXP_1 + EXP_2",
    //                                99,
    //                                 EXP_1 + EXP_2 );
    //var EXP_1 = new MyValuelessObject(new Number(100)); var EXP_2 = new MyValuelessObject(new Number(-1));
    //array[item++] = new TestCase(   SECTION,
    //                                "var EXP_1 = new MyValuelessObject(new Number(100)); var EXP_2 = new MyValuelessObject(new Number(-1)); EXP_1 + EXP_2",
    //                                "100-1",
    //                                 EXP_1 + EXP_2 );
    //var EXP_1 = new MyValuelessObject( new MyValuelessObject( new Boolean(true) ) );
    //array[item++] = new TestCase(   SECTION,
    //                                "var EXP_1 = new MyValuelessObject( new MyValuelessObject( new Boolean(true) ) ); EXP_1 + EXP_1",
    //                                "truetrue",
    //                                 EXP_1 + EXP_1 );
    //
    return ( array );
}

function MyObject( value ) {
    this.toString = function(){return this.value + ''};
    this.valueOf = function(){return this.value};
    this.value = value;
}

