/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
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
 * The Original Code is Mozilla Communicator client code, released
 * March 31, 1998.
 *
 * The Initial Developer of the Original Code is
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 1998
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
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

/**
   File Name:          sort.as
   ECMA Section:       Vector.sort(comparefn)
   Description:


   Author:             christine@netscape.com 12 Nov 1997
   Updated:            dschaffe@adobe.com 2 Nov 2007
*/


var SECTION = "";
var VERSION = "ECMA_1";
startTest();
var TITLE   = "Vector.sort(comparefn)";

writeHeaderToLog( SECTION + " "+ TITLE);

// sort int vector
var v1=new Vector.<int>();
for (var i=0;i<10;i++) v1[i]=9-i;
var v2=v1.sort(Compare);
CheckItems(v1,v2,"sort()");

// sort uint vector
var v1=new Vector.<uint>();
for (var i=0;i<10;i++) v1[i]=9-i;
var v2=v1.sort(Compare);
CheckItems(v1,v2,"sort()");

// sort string vector
var vs1=new Vector.<String>();
for (var i=0;i<10;i++) vs1[i]="string"+(9-i);
var vs2=vs1.sort(Compare);
CheckItems(vs1,vs2,"sort()");

var v1=new Vector.<int>();
for (var i=0;i<10;i++) v1[i]=9-i;
var errormsg="";
try {
  v1.sort()
} catch (e) {
  errormsg=e.toString();
}

if (as3Enabled) {
    AddTestCase(
   "sort vector without setting compare function throws exception",
   "ArgumentError: Error #1063",
   parseError(errormsg,"ArgumentError: Error #1063".length));
} else {
    AddTestCase(
   "sort vector without setting compare function throws exception",
   "TypeError: Error #1034",
   parseError(errormsg,"TypeError: Error #1034".length));
}

class TestClass {
    private var myVal:Object;
    public function TestClass(v:Object):void {
        myVal = v;
    }
    public function toString():String {
        return myVal.toString();
    }
    
    public function get val():Object {
        return myVal;
    }
}

function testClassReverseSort(x:TestClass, y:TestClass) {
    if (x.val < y.val)
        return 1
    if (x.val == y.val)
        return 0
    return -1
}

var testClassVector = new Vector.<TestClass>();
for (var i=0; i<20; i++) {
    testClassVector.push(new TestClass(i));
}
// push one duplicate value
testClassVector.push(new TestClass(12));

AddTestCase("Sort a custom vector",
            "19,18,17,16,15,14,13,12,12,11,10,9,8,7,6,5,4,3,2,1,0",
            testClassVector.sort(testClassReverseSort).toString()
            );

// Since we are not sorting on val, does an alpha sort
AddTestCase("Custom vector sort - object (alpha) sort",
            "0,1,10,11,12,12,13,14,15,16,17,18,19,2,3,4,5,6,7,8,9",
            testClassVector.sort(function(x,y) {if (x>y) return 1; if (x==y) return 0; return -1}).toString()
            );

// regular sort returning non-standard values
var mySortFunction:Function = function (x,y):Number {
    if (x.val < y.val)
        return -Infinity;
    if (x.val > y.val)
        return uint.MAX_VALUE;
    return undefined;
}

AddTestCase("Custom vector sort using sort function with non-standard values",
            "0,1,2,3,4,5,6,7,8,9,10,11,12,12,13,14,15,16,17,18,19",
            testClassVector.sort(mySortFunction).toString()
            );

test();

function CheckItems( A, E, desc) {
  AddTestCase(
          desc+" after sort, compare lengths",
          E.length,
          A.length);
  AddTestCase(
      desc+ " after sort, compare items",
      E.toString(),
      A.toString());

}
function Sort( a ) {
  for ( i = 0; i < a.length; i++ ) {
    for ( j = i+1; j < a.length; j++ ) {
      var lo = a[i];
      var hi = a[j];
      var c = Compare( lo, hi );
      if ( c == 1 ) {
    a[i] = hi;
    a[j] = lo;
      }
    }
  }
  return a;
}
function Compare( x, y ) {
  if ( x == void 0 && y == void 0  && typeof x == "undefined" && typeof y == "undefined" ) {
    return +0;
  }
  if ( x == void 0  && typeof x == "undefined" ) {
    return 1;
  }
  if ( y == void 0 && typeof y == "undefined" ) {
    return -1;
  }
  x = String(x);
  y = String(y);
  if ( x < y ) {
    return -1;
  }
  if ( x > y ) {
    return 1;
  }
  return 0;
}
