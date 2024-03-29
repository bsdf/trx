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
 * Netscape Communications Corp.
 * Portions created by the Initial Developer are Copyright (C) 2002
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   igor@icesoft.no, pschwartau@netscape.com
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

/*
 *
 * Date:    09 November 2002
 * SUMMARY: JS should treat --> as a single-line comment indicator.
 *          Whitespace may occur before the --> on the same line.
 *
 * See http://bugzilla.mozilla.org/show_bug.cgi?id=31255
 * and http://bugzilla.mozilla.org/show_bug.cgi?id=179366 (Rhino version)
 *
 * Note: <!--, --> are the HTML multi-line comment opener, closer.
 * JS already accepted <!-- as a single-line comment indicator.
 *
 */
//-----------------------------------------------------------------------------
var gTestfile = 'regress-31255.js';
var UBound = 0;
var BUGNUMBER = 31255;
var summary = 'JS should treat --> as a single-line comment indicator';
var status = '';
var statusitems = [];
var actual = '';
var actualvalues = [];
var expect= '';
var expectedvalues = [];


<!-- HTML comment start is already a single-line JS comment indicator
 var x = 1; <!-- until end-of-line

         status = inSection(1);
actual = (x == 1);
expect = true;
addThis();

--> HTML comment end is JS comments until end-of-line
--> but only if it follows a possible whitespace after line start
--> so in the following --> should not be treated as comments
if (x-->0)
  x = 2;

status = inSection(2);
actual = (x == 2);
expect = true;
addThis();



//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------



function addThis()
{
  statusitems[UBound] = status;
  actualvalues[UBound] = actual;
  expectedvalues[UBound] = expect;
  UBound++;
}


function addtestcases()
{

  printBugNumber(BUGNUMBER);
  printStatus(summary);

  for (var i=0; i<UBound; i++)
  {
    AddTestCase(statusitems[i], expectedvalues[i], actualvalues[i]);
  }


}

test();
