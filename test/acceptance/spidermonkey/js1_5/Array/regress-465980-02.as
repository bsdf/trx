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
 * The Original Code is JavaScript Engine testing utilities.
 *
 * The Initial Developer of the Original Code is
 * Mozilla Foundation.
 * Portions created by the Initial Developer are Copyright (C) 2008
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s): Jeff Walden
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

startTest();

var gTestfile = 'regress-465980-02.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 465980;
var summary = 'Do not crash @ InitArrayElements';
var actual = '';
var expect = '';


//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------

function addtestcases()
{

  printBugNumber(BUGNUMBER);
  printStatus (summary);

  function describe(name, startLength, pushArgs, expectThrow, expectLength)
  {
    return name + "(" + startLength + ", " +
      "[" + pushArgs.join(", ") + "], " +
      expectThrow + ", " +
      expectLength + ")";
  }

  function testArrayPush(startLength, pushArgs, expectThrow, expectLength)
  {
    /*
    print("running testArrayPush(" +
          startLength + ", " +
          "[" + pushArgs.join(", ") + "], " +
          expectThrow + ", " +
          expectLength + ")...");
    */
    var a = new Array(startLength);
    try
    {
      for (var i=0; i<pushArgs.length; i++) {
        a.push(pushArgs[i]);
      }
      if (expectThrow)
      {
        throw "expected to throw for " +
          describe("testArrayPush", startLength, pushArgs, expectThrow,
                   expectLength);
      }
    }
    catch (e)
    {
      if (!(e instanceof RangeError))
      {
        throw "unexpected exception type thrown: " + e + " for " +
          describe("testArrayPush", startLength, pushArgs, expectThrow,
                   expectLength);
      }
      if (!expectThrow)
      {
        throw "unexpected exception " + e + " for " +
          describe("testArrayPush", startLength, pushArgs, expectThrow,
                   expectLength);
      }
    }
    
    return a.length;
  }

  function testArrayUnshift(startLength, unshiftArgs, expectThrow, expectLength)
  {
    print("running testArrayUnshift(" +
          startLength + ", " +
          "[" + unshiftArgs.join(", ") + "], " +
          expectThrow + ", " +
          expectLength + ")...");
    
    print('initing array')
    var a = new Array(startLength);
    try
    {
      print('a.unshift(unshiftArgs);')
      a.unshift(unshiftArgs);
      if (expectThrow)
      {
        throw "expected to throw for " +
          describe("testArrayUnshift", startLength, unshiftArgs, expectThrow,
                   expectLength);
      }
    }
    catch (e)
    {
      print('catch e: '+e.toString());
      if (!(e instanceof RangeError))
      {
        throw "unexpected exception type thrown: " + e + " for " +
          describe("testArrayUnshift", startLength, unshiftArgs, expectThrow,
                   expectLength);
      }
      if (!expectThrow)
      {
        throw "unexpected exception " + e + " for " +
          describe("testArrayUnshift", startLength, unshiftArgs, expectThrow,
                   expectLength);
      }
    }

    print('checklength');
    if (a.length !== expectLength)
    {
      throw "unexpected modified-array length for " +
        describe("testArrayUnshift", startLength, unshiftArgs, expectThrow,
                 expectLength);
    }

    print('for')
    for (var i = 0, sz = unshiftArgs.length; i < sz; i++)
    {
      print('a[i] = '+a[i]+' unshiftArgs[i] = '+ unshiftArgs[i]);
      if (a[i] !== unshiftArgs[i])
      {
        throw "unexpected value at index " + index + " during " +
          describe("testArrayUnshift", startLength, unshiftArgs, expectThrow,
                   expectLength);
      }
    }
  }




var foo = "foo", bar = "bar", baz = "baz";
    

try {
     actual = testArrayPush(4294967294, [foo, bar], false, 4294967295);
 } catch (e) {
     actual = e + '';
 }
 AddTestCase('testArrayPush(4294967294, [foo, bar], false, 4294967295)', 4294967295, actual);

try {
     actual = testArrayPush(4294967294, [foo, bar, baz], false, 4294967295);
 } catch (e) {
     actual = e + '';
 }
 AddTestCase('testArrayPush(4294967294, [foo, bar, baz], false, 4294967295)', 4294967295, actual);

try {
     actual = testArrayPush(4294967295, [foo], false, 4294967295);
 } catch (e) {
     actual = e + '';
 }
 AddTestCase('testArrayPush(4294967295, [foo], false, 4294967295)', 4294967295, actual);

try {
     actual = testArrayPush(4294967295, [foo, bar], false, 4294967295);
 } catch (e) {
     actual = e + '';
 }
 AddTestCase('testArrayPush(4294967295, [foo, bar], false, 4294967295)', 4294967295, actual);

try {
     actual = testArrayPush(4294967295, [foo, bar, baz], false, 4294967295);
 } catch (e) {
     actual = e + '';
 }
 AddTestCase('testArrayPush(4294967295, [foo, bar, baz], false, 4294967295)', 4294967295, actual);


/* unshift is very slow for large arrays
  
actual = ''

try {
     testArrayUnshift(4294967294, [foo], false, 4294967295);
 } catch (e) {
     actual = e + '';
 }
 AddTestCase('testArrayUnshift(4294967294, [foo], false, 4294967295)', expect, actual);


actual = ''

try {
     testArrayUnshift(4294967294, [foo, bar], true, 4294967294);
 } catch (e) {
     actual = e + '';
 }
 AddTestCase('testArrayUnshift(4294967294, [foo, bar], true, 4294967294)', expect, actual);


actual = ''

try {
     testArrayUnshift(4294967294, [foo, bar, baz], true, 4294967294);
 } catch (e) {
     actual = e + '';
 }
 AddTestCase('testArrayUnshift(4294967294, [foo, bar, baz], true, 4294967294)', expect, actual);


actual = ''

try {
     testArrayUnshift(4294967295, [foo], true, 4294967295);
 } catch (e) {
     actual = e + '';
 }
 AddTestCase('testArrayUnshift(4294967295, [foo], true, 4294967295)', expect, actual);


actual = ''

try {
     testArrayUnshift(4294967295, [foo, bar], true, 4294967295);
 } catch (e) {
     actual = e + '';
 }
 AddTestCase('testArrayUnshift(4294967295, [foo, bar], true, 4294967295)', expect, actual);


actual = ''

try {
     testArrayUnshift(4294967295, [foo, bar, baz], true, 4294967295);
 } catch (e) {
     actual = e + '';
 }
 AddTestCase('testArrayUnshift(4294967295, [foo, bar, baz], true, 4294967295)', expect, actual);
*/

} // addtestcases

test();
