/* -*- Mode: js; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
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

var SECTION = "15.1.2.2 parseInt()";
var VERSION = "";
startTest();
var TITLE = "Tests based on code coverage";

writeHeaderToLog( SECTION + " "+ TITLE);

var testcases = getTestCases();
test();

function getTestCases() {
    var array = new Array();
    var item = 0;


    // other js engines produce 1228503247364293600 for this testcase, and c produces 1228503247364293699
    // Bug 602912: https://bugzilla.mozilla.org/show_bug.cgi?id=602912
    array[item++] = new TestCase( SECTION,  "radix 32 > 53bits of precisions", 1228503247364293699, parseInt("1234567890123", 32) );
    array[item++] = new TestCase( SECTION,  "radix 8 > 53bits of precisions", 23528930381028800, parseInt("1234567000123456700", 8) ); // other engines 188231443048230400
    array[item++] = new TestCase( SECTION,  "radix 4 > 53bits of precisions", 5.120083167990638e+23, parseInt("1230123000123012300012301230001230123000", 4) );
    array[item++] = new TestCase( SECTION,  "radix 2 > 53bits of precisions", 1.1805916207174113e+21, parseInt("1111111111111111111111111111111111111111111111111111111111111111111111", 2) );


    return ( array );
}
