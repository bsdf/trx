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

   var SECTION = "15.9.5.5";
   var VERSION = "ECMA_3";
   var TITLE   = "Date.prototype.toLocaleString()";
   
   var status = '';
   var actual = '';
   var expect = '';


   startTest();
   writeHeaderToLog( SECTION + " "+ TITLE);


//-----------------------------------------------------------------------------------------------------
   var testcases = getTestCases();
//-----------------------------------------------------------------------------------------------------
    test();

function getTestCases() {
    var array = new Array();
    var item = 0;
   // first, some generic tests -

    status = "typeof (now.toLocaleString())";
    actual =   typeof (now.toLocaleString());
    expect = "string";
    array[item++] = new TestCase(SECTION, status, expect, actual);

    status = "Date.prototype.toLocaleString.length";
    actual =  Date.prototype.toLocaleString.length;
    expect =  0;
    array[item++] = new TestCase(SECTION, status, expect, actual);
    
    // Date.parse is accurate to the second;  valueOf() to the millisecond  -
    status = "Math.abs(Date.parse(now.toLocaleString()) - now.valueOf()) < 1000";
    actual =   Math.abs(Date.parse(now.toLocaleString()) -  now.valueOf()) < 1000;
    expect = true;
    array[item++] = new TestCase(SECTION, status, expect, actual);


    // 1970
    addDateTestCase(0);
    addDateTestCase(TZ_ADJUST);

   
    // 1900
    addDateTestCase(TIME_1900);
    addDateTestCase(TIME_1900 -TZ_ADJUST);

   
    // 2000
    addDateTestCase(TIME_2000);
    addDateTestCase(TIME_2000 -TZ_ADJUST);

    
    // 29 Feb 2000
    addDateTestCase(UTC_29_FEB_2000);
    addDateTestCase(UTC_29_FEB_2000 - 1000);
    addDateTestCase(UTC_29_FEB_2000 - TZ_ADJUST);
 

    // 2005
    addDateTestCase(UTC_1_JAN_2005);
    addDateTestCase(UTC_1_JAN_2005 - 1000);
    addDateTestCase(UTC_1_JAN_2005-TZ_ADJUST);
   
    
    function addTestCase()
    {
      array[item++] = new TestCase( SECTION, status, expect, actual);
    }
    
    
    function addDateTestCase(date_given_in_milliseconds)
    {
      var givenDate = new Date(date_given_in_milliseconds);
    
      status = 'Date.parse(   +   givenDate   +   ).toLocaleString())';
      actual =  Date.parse(givenDate.toLocaleString());
      expect = date_given_in_milliseconds;
      addTestCase();
    }
    return array;
}
