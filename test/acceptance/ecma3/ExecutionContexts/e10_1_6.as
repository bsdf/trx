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

    var SECTION = "10.1.6";
    var VERSION = "ECMA_1";
    startTest();
    var TITLE   = "Activation Object";

    writeHeaderToLog( SECTION + " "+ TITLE);

    //    The current object has an arguments property.
    var arguments = "FAILED!";

    var testcases = getTestCases();

    test();

function getTestCases() {
    var array = new Array();
    var item = 0;


    var ARG_STRING = "value of the argument property";

    array[item++] = new TestCase( SECTION,
                                    "(new TestObject(0,1,2,3,4,5)).length",
                                    6,
                                    (new TestObject(0,1,2,3,4,5)).length );

    for ( i = 0; i < 6; i++ ) {

        array[item++] = new TestCase( SECTION,
                                        "(new TestObject(0,1,2,3,4,5))["+i+"]",
                                        i,
                                        (new TestObject(0,1,2,3,4,5))[i]);
    }


    //    The current object already has an arguments property.

    array[item++] = new TestCase( SECTION,
                                    "(new AnotherTestObject(1,2,3)).arguments",
                                    ARG_STRING,
                                    (new AnotherTestObject(1,2,3)).arguments );

    //  The function invoked with [[Call]]

    array[item++] = new TestCase( SECTION,
                                    "TestFunction(1,2,3)",
                                    ARG_STRING,
                                    TestFunction() + '' );


    function TestObject() {
        // cn:  __proto__ not ecma3 compliant
        //this.__proto__ = new Prototype();
        this.constructor.prototype.arguments = ARG_STRING;
        return arguments;
    }
    function AnotherTestObject() {
        // cn:  __proto__ not ecma3 compliant
        //this.__proto__ = new Prototype();
        this.constructor.prototype.arguments = ARG_STRING;
        return this;
    }
    function TestFunction() {
        arguments[0] = ARG_STRING;
        return arguments;
    }
    function AnotherTestFunction() {
        // cn:  __proto__ not ecma3 compliant
        //this.__proto__ = new Prototype();
        this.constructor.prototype.arguments = ARG_STRING;
        return this;
    }

    return ( array );
}
