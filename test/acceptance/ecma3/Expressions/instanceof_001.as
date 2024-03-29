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
    var SECTION = "instanceof-001";
    var VERSION = "ECMA_2";
    var TITLE   = "instanceof"

    startTest();
    writeHeaderToLog( SECTION + " "+ TITLE);

    var testcases = getTestCases();
    test();
    
    function InstanceOf( object_1, object_2, expect, array, item ) {
        result = object_1 instanceof object_2;

        array[item] = new TestCase(
            SECTION,
            "(" + object_1 + ") instanceof " + object_2,
            expect,
            result );
    }

function getTestCases() {
    var array = new Array();
    var item = 0;
    
    function Gen3(value) {
        this.value = value;
        this.generation = 3;
        this.toString = function (){return "Gen" + this.generation+ " instance"};
    }
    Gen3.name = 3;
    var origPrototypeToString = Gen3.constructor.prototype.toString;
    Gen3.constructor.prototype.toString = function(){return this.name+" object"};

    function Gen2(value) {
        this.value = value;
        this.generation = 2;
    }
    Gen2.name = 2;
    Gen2.prototype = new Gen3();

    function Gen1(value) {
        this.value = value;
        this.generation = 1;
    }
    Gen1.name = 1;
    Gen1.prototype = new Gen2();

    function Gen0(value) {
        this.value = value;
        this.generation = 0;
    }
    Gen0.name = 0;
    Gen0.prototype = new Gen1();


    function GenA(value) {
        this.value = value;
        this.generation = "A";
        this.toString = function (){return "instance of Gen" +this.generation};

    }
    GenA.prototype = new Gen0();
    GenA.name = "A";

    function GenB(value) {
        this.value = value;
        this.generation = "B";
        this.toString = function (){ return "instance of Gen"+this.generation};
    }
    GenB.name = "B"
    //GenB.prototype = void 0;

    // RelationalExpression is not an object.

    InstanceOf( true, Boolean, true, array, item++ );
    InstanceOf( new Boolean(false), Boolean, true, array, item++ );

    // Identifier is a function, prototype of Identifier is not an object

    // RelationalExpression.__proto__ ==  (but not ===) Identifier.prototype
    InstanceOf( new Gen2(), Gen0, false, array, item++ );
    InstanceOf( new Gen2(), Gen1, false, array, item++ );
    InstanceOf( new Gen2(), Gen2, true, array, item++ );
    InstanceOf( new Gen2(), Gen3, true, array, item++ );

    // RelationalExpression.__proto__.__proto__ === Identifier.prototype
    InstanceOf( new Gen0(), Gen0, true, array, item++ );
    InstanceOf( new Gen0(), Gen1, true, array, item++ );
    InstanceOf( new Gen0(), Gen2, true, array, item++ );
    InstanceOf( new Gen0(), Gen3, true, array, item++ );

    InstanceOf( new Gen0(), Object, true, array, item++ );
    InstanceOf( new Gen0(), Function, false, array, item++ );

    InstanceOf( Gen0, Function, true, array, item++ );
    InstanceOf( Gen0, Object, true, array, item++ );

    //restore
    Gen3.constructor.prototype.toString = origPrototypeToString;
    return array;
}
