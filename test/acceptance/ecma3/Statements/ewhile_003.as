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
    var SECTION = "while-003";
    var VERSION = "ECMA_2";
    var TITLE   = "while statement";

    startTest();
    writeHeaderToLog( SECTION + " "+ TITLE);

    var testcases = getTestCases();
    test();
    
function getTestCases(){
    var array = new Array();
    var item = 0;

    DoWhile( new DoWhileObject(
                "while expression is true",
                true));

    DoWhile( new DoWhileObject(
             "while expression is 1",
             1));

    DoWhile( new DoWhileObject(
             "while expression is new Boolean(true)",
             new Boolean(true)));

    DoWhile( new DoWhileObject(
             "while expression is new Object()",
             new Object()));

    DoWhile( new DoWhileObject(
             "while expression is \"hi\"",
             "hi"));
/*
    DoWhile( new DoWhileObject(
             "while expression has a continue in it",
             "true",
             "if ( i == void 0 ) i = 0; result=\"pass\"; if ( ++i == 1 ) {continue;} else {break;} result=\"fail\";"
             ));
*/


    function DoWhileObject( d, e ) {
        this.description = d;
        this.whileExpression = e;
    }

    function DoWhile( object ) {
        result = "fail: statements in while block were not evaluated";

        while ( expression = object.whileExpression ) {
            result = "pass";
            break;
        }

        // verify that the while expression was evaluated
        array[item++] = new TestCase(
            SECTION,
            "verify that while expression was evaluated (should be "+
                object.whileExpression +")",
            "pass",
            (object.whileExpression == expression ||
               ( isNaN(object.whileExpression) && isNaN(expression) )
             ) ? "pass" : "fail" );

        array[item++] = new TestCase(
            SECTION,
            object.description,
            "pass",
            result );
    }
    return array;
}
