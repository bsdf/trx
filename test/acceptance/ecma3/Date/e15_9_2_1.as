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
    var VERSION =   "ECMA_1";
    startTest();
    var SECTION =   "15.9.2.1";
    var TITLE =     "Date Constructor used as a function";
    var TYPEOF  =   "string";
    var TOLERANCE = 1000;

    writeHeaderToLog("15.9.2.1 The Date Constructor Called as a Function:  " +
                     "Date( year, month, date, hours, minutes, seconds, ms )" );
    var testcases = getTestCases();

//  all tests must call a function that returns an array of TestCase objects.
    test();


function compareDate(d1, d2) {
    //Dates may be off by a second
    if (d1 == d2) {
        return true;
    } else if (Math.abs(new Date(d1) - new Date(d2)) <= 1000) {
        return true;
    } else {
        return false;
    }
}


function getTestCases() {
    var array = new Array();
    var item = 0;

    var TODAY = new Date();

    // Dates around 1970

    array[item++] = new TestCase( SECTION, "Date(1970,0,1,0,0,0,0)",            true, compareDate((new Date()).toString(), Date(1970,0,1,0,0,0,0) ));
    array[item++] = new TestCase( SECTION, "Date(1969,11,31,15,59,59,999)",     true, compareDate((new Date()).toString(), Date(1969,11,31,15,59,59,999)));
    array[item++] = new TestCase( SECTION, "Date(1969,11,31,16,0,0,0)",         true, compareDate((new Date()).toString(), Date(1969,11,31,16,0,0,0)));
    array[item++] = new TestCase( SECTION, "Date(1969,11,31,16,0,0,1)",         true, compareDate((new Date()).toString(), Date(1969,11,31,16,0,0,1)));

    // Dates around 2000
    array[item++] = new TestCase( SECTION, "Date(1999,11,15,59,59,999)",        true, compareDate((new Date()).toString(), Date(1999,11,15,59,59,999)));
    array[item++] = new TestCase( SECTION, "Date(1999,11,16,0,0,0,0)",          true, compareDate((new Date()).toString(), Date(1999,11,16,0,0,0,0)));
    array[item++] = new TestCase( SECTION, "Date(1999,11,31,23,59,59,999)",     true, compareDate((new Date()).toString(), Date(1999,11,31,23,59,59,999)) );
    array[item++] = new TestCase( SECTION, "Date(2000,0,1,0,0,0,0)",            true, compareDate((new Date()).toString(), Date(2000,0,0,0,0,0,0) ));
    array[item++] = new TestCase( SECTION, "Date(2000,0,1,0,0,0,1)",            true, compareDate((new Date()).toString(), Date(2000,0,0,0,0,0,1) ));

    // Dates around 1900

    array[item++] = new TestCase( SECTION, "Date(1899,11,31,23,59,59,999)",     true, compareDate((new Date()).toString(), Date(1899,11,31,23,59,59,999)));
    array[item++] = new TestCase( SECTION, "Date(1900,0,1,0,0,0,0)",            true, compareDate((new Date()).toString(), Date(1900,0,1,0,0,0,0)));
    array[item++] = new TestCase( SECTION, "Date(1900,0,1,0,0,0,1)",            true, compareDate((new Date()).toString(), Date(1900,0,1,0,0,0,1)));
    array[item++] = new TestCase( SECTION, "Date(1899,11,31,16,0,0,0,0)",       true, compareDate((new Date()).toString(), Date(1899,11,31,16,0,0,0,0)));

    // Dates around feb 29, 2000

    array[item++] = new TestCase( SECTION, "Date( 2000,1,29,0,0,0,0)",         true, compareDate((new Date()).toString(), Date(2000,1,29,0,0,0,0)));
    array[item++] = new TestCase( SECTION, "Date( 2000,1,28,23,59,59,999)",    true, compareDate((new Date()).toString(), Date( 2000,1,28,23,59,59,999)));
    array[item++] = new TestCase( SECTION, "Date( 2000,1,27,16,0,0,0)",        true, compareDate((new Date()).toString(), Date(2000,1,27,16,0,0,0)));

    // Dates around jan 1, 2005
    array[item++] = new TestCase( SECTION, "Date(2004,11,31,23,59,59,999)",     true, compareDate((new Date()).toString(), Date(2004,11,31,23,59,59,999)));
    array[item++] = new TestCase( SECTION, "Date(2005,0,1,0,0,0,0)",            true, compareDate((new Date()).toString(), Date(2005,0,1,0,0,0,0)));
    array[item++] = new TestCase( SECTION, "Date(2005,0,1,0,0,0,1)",            true, compareDate((new Date()).toString(), Date(2005,0,1,0,0,0,1)));
    array[item++] = new TestCase( SECTION, "Date(2004,11,31,16,0,0,0,0)",       true, compareDate((new Date()).toString(), Date(2004,11,31,16,0,0,0,0)));

    // Dates around jan 1, 2032
    array[item++] = new TestCase( SECTION, "Date(2031,11,31,23,59,59,999)",     true, compareDate((new Date()).toString(), Date(2031,11,31,23,59,59,999)));
    array[item++] = new TestCase( SECTION, "Date(2032,0,1,0,0,0,0)",            true, compareDate((new Date()).toString(), Date(2032,0,1,0,0,0,0)));
    array[item++] = new TestCase( SECTION, "Date(2032,0,1,0,0,0,1)",            true, compareDate((new Date()).toString(), Date(2032,0,1,0,0,0,1)));
    array[item++] = new TestCase( SECTION, "Date(2031,11,31,16,0,0,0,0)",       true, compareDate((new Date()).toString(), Date(2031,11,31,16,0,0,0,0)));

    return ( array );
}
