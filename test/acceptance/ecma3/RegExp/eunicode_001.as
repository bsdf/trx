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
    var SECTION = "RegExp/unicode-001.js";
    var VERSION = "ECMA_2";
    var TITLE   = "new RegExp( pattern, flags )";

startTest();
writeHeaderToLog(SECTION + " " + TITLE);
var testcases = getTestCases();
test();

function getTestCases() {
    var array = new Array();
    var item = 0;

    // These examples come from 15.7.1, UnicodeEscapeSequence

    AddRegExpCases( /\u0041/, "/\\u0041/",   "A", "A", 1, 0, ["A"] );
    AddRegExpCases( /\u00412/, "/\\u00412/", "A2", "A2", 1, 0, ["A2"] );
    AddRegExpCases( /\u00412/, "/\\u00412/", "A2", "A2", 1, 0, ["A2"] );
    AddRegExpCases( /\u001g/, "/\\u001g/", "u001g", "u001g", 1, 0, ["u001g"] );

    AddRegExpCases( /A/,  "/A/",  "\u0041", "\\u0041",   1, 0, ["A"] );
    AddRegExpCases( /A/,  "/A/",  "\u00412", "\\u00412", 1, 0, ["A"] );
    AddRegExpCases( /A2/, "/A2/", "\u00412", "\\u00412", 1, 0, ["A2"]);
    AddRegExpCases( /A/,  "/A/",  "A2",      "A2",       1, 0, ["A"] );

    function AddRegExpCases(
        regexp, str_regexp, pattern, str_pattern, length, index, matches_array ) {

        array[item++] = new TestCase(SECTION,
            str_regexp + " .exec(" + str_pattern +").length",
            length,
            regexp.exec(pattern).length );

        array[item++] = new TestCase(SECTION,
            str_regexp + " .exec(" + str_pattern +").index",
            index,
            regexp.exec(pattern).index );

        array[item++] = new TestCase(SECTION,
            str_regexp + " .exec(" + str_pattern +").input",
            pattern,
            regexp.exec(pattern).input );

        for ( var matches = 0; matches < matches_array.length; matches++ ) {
            array[item++] = new TestCase(SECTION,
                str_regexp + " .exec(" + str_pattern +")[" + matches +"]",
                matches_array[matches],
                regexp.exec(pattern)[matches] );
        }
    }

    return array;
}
