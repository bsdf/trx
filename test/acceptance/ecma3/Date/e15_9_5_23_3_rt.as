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

var SECTION = "15.9.5.23-3-n";
var VERSION = "ECMA_1";
var TITLE   = "Date.prototype.setTime()";


startTest();
writeHeaderToLog( SECTION + " "+ TITLE);

var testcases = getTestCases();
test();

function getTestCases() {
    var array = new Array();
    var item = 0;
    
    var MYDATE = new MyDate(TIME_1970);
    
    if (as3Enabled) {
        MYDATE.setTime(TIME_2000)
        array[item++] = new TestCase(SECTION,
                                "MYDATE.setTime(TIME_2000)",
                                TIME_2000,
                                MYDATE.getTime());
    } else {
        thisError = "no error thrown";
        try {
            MYDATE.setTime(TIME_2000);
        }
        catch(e){
            thisError=e.toString();
        }
        finally {
            trace(thisError)
            array[item++] = new TestCase(SECTION,
                                "MYDATE.setTime(TIME_2000)",
                                "TypeError: Error #1034",
                                typeError(thisError));
        }
    }
    
    return array;
}

function MyDate(value) {
    this.value = value;
    this.setTime = Date.prototype.setTime;
    this.getTime = Date.prototype.getTime;
    return this;
}
