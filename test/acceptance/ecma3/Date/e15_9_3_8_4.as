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

    var VERSION = "ECMA_1";
    startTest();
    var SECTION = "15.9.3.8";
    var TYPEOF  = "object";

    var TIME        = 0;
    var UTC_YEAR    = 1;
    var UTC_MONTH   = 2;
    var UTC_DATE    = 3;
    var UTC_DAY     = 4;
    var UTC_HOURS   = 5;
    var UTC_MINUTES = 6;
    var UTC_SECONDS = 7;
    var UTC_MS      = 8;

    var YEAR        = 9;
    var MONTH       = 10;
    var DATE        = 11;
    var DAY         = 12;
    var HOURS       = 13;
    var MINUTES     = 14;
    var SECONDS     = 15;
    var MS          = 16;


//  for TCMS, the testcases array must be global.
    var tc= 0;
    var TITLE = "Date constructor:  new Date( value )";
    var SECTION = "15.9.3.8";
    var VERSION = "ECMA_1";
    startTest();

    writeHeaderToLog( SECTION +" " + TITLE );

    testcases = getTestCases();

//  all tests must call a function that returns a boolean value
    test();

function getTestCases( ) {
    var array = new Array();
    var item = 0;

    // all the "ResultArrays" below are hard-coded to Pacific Standard Time values -
    var TZ_ADJUST =  -TZ_PST * msPerHour;

    // Dates around Feb 29, 2000

    var UTC_FEB_29_2000 = TIME_2000 + 31*msPerDay + 28*msPerDay;
    var PST_FEB_29_2000 = UTC_FEB_29_2000 + TZ_ADJUST;

    addNewTestCase( new Date(UTC_FEB_29_2000),
                    "new Date("+UTC_FEB_29_2000+")",
                    [UTC_FEB_29_2000,2000,1,29,2,0,0,0,0,2000,1,28,1,16,0,0,0] );

    addNewTestCase( new Date(PST_FEB_29_2000),
                    "new Date("+PST_FEB_29_2000+")",
                    [PST_FEB_29_2000,2000,1,29,2,8,0,0,0,2000,1,29,2,0,0,0,0] );

    addNewTestCase( new Date( (new Date(UTC_FEB_29_2000)).toString() ),
                    "new Date(\+(new Date(UTC_FEB_29_2000)).toString()+\)",
                    [UTC_FEB_29_2000,2000,1,29,2,0,0,0,0,2000,1,28,1,16,0,0,0] );

    addNewTestCase( new Date( (new Date(PST_FEB_29_2000)).toString() ),
                    "new Date(\+(new Date(PST_FEB_29_2000)).toString()+\)",
                    [PST_FEB_29_2000,2000,1,29,2,8,0,0,0,2000,1,29,2,0,0,0,0] );


 //    addNewTestCase( new Date( (new Date(UTC_FEB_29_2000)).toGMTString() ),
 //                    "new Date(\""+(new Date(UTC_FEB_29_2000)).toGMTString()+"\")",
 //                   [UTC_FEB_29_2000,2000,1,29,2,0,0,0,0,2000,1,28,1,16,0,0,0] );

 //   addNewTestCase( new Date( (new Date(PST_FEB_29_2000)).toGMTString() ),
 //                   "new Date(\""+(new Date(PST_FEB_29_2000)).toGMTString()+"\")",
 //                    [PST_FEB_29_2000,2000,1,29,2,8,0,0,0,2000,1,29,2,0,0,0,0] );


/*
    // Dates around 1900

    var PST_1900 = TIME_1900 + 8*msPerHour;

    addNewTestCase( new Date( TIME_1900 ),
                    "new Date("+TIME_1900+")",
                    [TIME_1900,1900,0,1,1,0,0,0,0,1899,11,31,0,16,0,0,0] );

    addNewTestCase( new Date(PST_1900),
                    "new Date("+PST_1900+")",
                    [ PST_1900,1900,0,1,1,8,0,0,0,1900,0,1,1,0,0,0,0] );

    addNewTestCase( new Date( (new Date(TIME_1900)).toString() ),
                    "new Date(\""+(new Date(TIME_1900)).toString()+"\")",
                    [TIME_1900,1900,0,1,1,0,0,0,0,1899,11,31,0,16,0,0,0] );

    addNewTestCase( new Date( (new Date(PST_1900)).toString() ),
                    "new Date(\""+(new Date(PST_1900 )).toString()+"\")",
                    [ PST_1900,1900,0,1,1,8,0,0,0,1900,0,1,1,0,0,0,0] );

    addNewTestCase( new Date( (new Date(TIME_1900)).toUTCString() ),
                    "new Date(\""+(new Date(TIME_1900)).toUTCString()+"\")",
                    [TIME_1900,1900,0,1,1,0,0,0,0,1899,11,31,0,16,0,0,0] );

    addNewTestCase( new Date( (new Date(PST_1900)).toUTCString() ),
                    "new Date(\""+(new Date(PST_1900 )).toUTCString()+"\")",
                    [ PST_1900,1900,0,1,1,8,0,0,0,1900,0,1,1,0,0,0,0] );

//    addNewTestCase( "new Date(\""+(new Date(TIME_1900)).toLocaleString()+"\")",   [TIME_1900,1900,0,1,1,0,0,0,0,1899,11,31,0,16,0,0,0] );
//    addNewTestCase( "new Date(\""+(new Date(PST_1900 )).toLocaleString()+"\")",   [ PST_1900,1900,0,1,1,8,0,0,0,1900,0,1,1,0,0,0,0] );
*/
/*
   This test case is incorrect.  Need to fix the DaylightSavings functions in
   shell.js for this to work properly.

    var DST_START_1998 = UTC( GetFirstSundayInApril(TimeFromYear(1998)) + 2*msPerHour )

    addNewTestCase( new Date(DST_START_1998-1),
                    "new Date("+(DST_START_1998-1)+")",
                    [DST_START_1998-1,1998,3,5,0,9,59,59,999,1998,3,5,0,1,59,59,999] );

    addNewTestCase( new Date(DST_START_1998),
                    "new Date("+DST_START_1998+")",
                    [DST_START_1998,1998,3,5,0,10,0,0,0,1998,3,5,0,3,0,0,0]);

    var DST_END_1998 = UTC( GetLastSundayInOctober(TimeFromYear(1998)) + 2*msPerHour );

    addNewTestCase ( new Date(DST_END_1998-1),
                    "new Date("+(DST_END_1998-1)+")",
                    [DST_END_1998-1,1998,9,25,0,8,59,59,999,1998,9,25,0,1,59,59,999] );

    addNewTestCase ( new Date(DST_END_1998),
                    "new Date("+DST_END_1998+")",
                    [DST_END_1998,1998,9,25,0,9,0,0,0,1998,9,25,0,1,0,0,0] );
*/

    function addNewTestCase( DateCase, DateString, ResultArray ) {
        //adjust hard-coded ResultArray for tester's timezone instead of PST
        adjustResultArray(ResultArray, 'msMode');

        item = array.length;

        array[item++] = new TestCase( SECTION, DateString+".getTime()", ResultArray[TIME],       DateCase.getTime() );
        array[item++] = new TestCase( SECTION, DateString+".valueOf()", ResultArray[TIME],       DateCase.valueOf() );
        array[item++] = new TestCase( SECTION, DateString+".getUTCFullYear()",      ResultArray[UTC_YEAR], DateCase.getUTCFullYear() );
        array[item++] = new TestCase( SECTION, DateString+".getUTCMonth()",         ResultArray[UTC_MONTH],  DateCase.getUTCMonth() );
        array[item++] = new TestCase( SECTION, DateString+".getUTCDate()",          ResultArray[UTC_DATE],   DateCase.getUTCDate() );
        array[item++] = new TestCase( SECTION, DateString+".getUTCDay()",           ResultArray[UTC_DAY],    DateCase.getUTCDay() );
        array[item++] = new TestCase( SECTION, DateString+".getUTCHours()",         ResultArray[UTC_HOURS],  DateCase.getUTCHours() );
        array[item++] = new TestCase( SECTION, DateString+".getUTCMinutes()",       ResultArray[UTC_MINUTES],DateCase.getUTCMinutes() );
        array[item++] = new TestCase( SECTION, DateString+".getUTCSeconds()",       ResultArray[UTC_SECONDS],DateCase.getUTCSeconds() );
        array[item++] = new TestCase( SECTION, DateString+".getUTCMilliseconds()",  ResultArray[UTC_MS],     DateCase.getUTCMilliseconds() );
        array[item++] = new TestCase( SECTION, DateString+".getFullYear()",         ResultArray[YEAR],       DateCase.getFullYear() );
        array[item++] = new TestCase( SECTION, DateString+".getMonth()",            ResultArray[MONTH],      DateCase.getMonth() );
        array[item++] = new TestCase( SECTION, DateString+".getDate()",             ResultArray[DATE],       DateCase.getDate() );
        array[item++] = new TestCase( SECTION, DateString+".getDay()",              ResultArray[DAY],        DateCase.getDay() );
        array[item++] = new TestCase( SECTION, DateString+".getHours()",            ResultArray[HOURS],      DateCase.getHours() );
        array[item++] = new TestCase( SECTION, DateString+".getMinutes()",          ResultArray[MINUTES],    DateCase.getMinutes() );
        array[item++] = new TestCase( SECTION, DateString+".getSeconds()",          ResultArray[SECONDS],    DateCase.getSeconds() );
        array[item++] = new TestCase( SECTION, DateString+".getMilliseconds()",     ResultArray[MS],         DateCase.getMilliseconds() );
    }

    return ( array );
}
