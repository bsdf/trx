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

var SECTION = "RegExp/exec-002";
var VERSION = "ECMA_2";
var TITLE   = "RegExp.prototype.exec(string)";

startTest();
writeHeaderToLog(SECTION + " " + TITLE);
var testcases = getTestCases();
test();

function getTestCases() {
    var array = new Array();
    var item = 0;

    /*
     * for each test case, verify:
     * - type of object returned
     * - length of the returned array
     * - value of lastIndex
     * - value of index
     * - value of input
     * - value of the array indices
     */

    AddRegExpCases(
        /(a|d|q|)x/i,
        "bcaDxqy",
        3,
        ["Dx", "D"] );

    AddRegExpCases(
        /(a|(e|q))(x|y)/,
        "bcaddxqy",
        6,
        ["qy","q","q","y"] );


    AddRegExpCases(
        /a+b+d/,
        "aabbeeaabbs",
        0,
        null );

    AddRegExpCases(
        /a*b/,
        "aaadaabaaa",
        4,
        ["aab"] );

    AddRegExpCases(
        /a*b/,
        "dddb",
        3,
        ["b"] );

    AddRegExpCases(
        /a*b/,
        "xxx",
        0,
        null );

    AddRegExpCases(
        /x\d\dy/,
        "abcx45ysss235",
        3,
        ["x45y"] );

    AddRegExpCases(
        /[^abc]def[abc]+/,
        "abxdefbb",
        2,
        ["xdefbb"] );

    AddRegExpCases(
        /(a*)baa/,
        "ccdaaabaxaabaa",
        9,
        ["aabaa", "aa"] );

    AddRegExpCases(
        /(a*)baa/,
        "aabaa",
        0,
        ["aabaa", "aa"] );

    AddRegExpCases(
        /q(a|b)*q/,
        "xxqababqyy",
        2,
        ["qababq", "b"] );

    AddRegExpCases(
        /(a(.|[^d])c)*/,
        "adcaxc",
        0,
        ["adcaxc", "axc", "x"] );

    AddRegExpCases(
        /(a*)b\1/,
        "abaaaxaabaayy",
        0,
        ["aba", "a"] );

    AddRegExpCases(
        /(a*)b\1/,
        "abaaaxaabaayy",
        0,
        ["aba", "a"] );

    AddRegExpCases(
        /(a*)b\1/,
        "cccdaaabaxaabaayy",
        6,
        ["aba", "a"] );

    AddRegExpCases(
        /(a*)b\1/,
        "cccdaaabqxaabaayy",
        7,
        ["b", ""] );

    AddRegExpCases(
        /"(.|[^"\\\\])*"/,
        'xx\"makudonarudo\"yy',
        2,
        ["\"makudonarudo\"", "o"] );

    AddRegExpCases(
        /"(.|[^"\\\\])*"/,
        "xx\"ma\"yy",
        2,
        ["\"ma\"", "a"] );


    function AddRegExpCases(
        regexp, pattern, index, matches_array ) {

        // prevent a runtime error

        if ( regexp.exec(pattern) == null || matches_array == null ) {
            array[item++] = new TestCase(SECTION,
                regexp + ".exec(" + pattern +")",
                matches_array,
                regexp.exec(pattern) );

            return;
        }
        array[item++] = new TestCase(SECTION,
            regexp + ".exec(" + pattern +").length",
            matches_array.length,
            regexp.exec(pattern).length );

        array[item++] = new TestCase(SECTION,
            regexp + ".exec(" + pattern +").index",
            index,
            regexp.exec(pattern).index );

        array[item++] = new TestCase(SECTION,
            regexp + ".exec(" + pattern +").input",
            pattern,
            regexp.exec(pattern).input );

        array[item++] = new TestCase(SECTION,
            regexp + ".exec(" + pattern +").toString()",
            matches_array.toString(),
            regexp.exec(pattern).toString() );
    /*
        var limit = matches_array.length > regexp.exec(pattern).length
                    ? matches_array.length
                    : regexp.exec(pattern).length;

        for ( var matches = 0; matches < limit; matches++ ) {
            array[item++] = new TestCase(SECTION,
                regexp + ".exec(" + pattern +")[" + matches +"]",
                matches_array[matches],
                regexp.exec(pattern)[matches] );
        }
    */
    }

    return array;
}
