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

gTestfile = 'asterisk.js';

/**
   Filename:     asterisk.js
   Description:  'Tests regular expressions containing *'

   Author:       Nick Lerissa
   Date:         March 10, 1998
*/

var SECTION = 'As described in Netscape doc "Whats new in JavaScript 1.2"';
var VERSION = 'no version';
startTest();  var testscases=[]; var index=0;
var TITLE   = 'RegExp: *';

writeHeaderToLog('Executing script: aterisk.js');
writeHeaderToLog( SECTION + " "+ TITLE);

// 'abcddddefg'.match(new RegExp('d*'))
testcases[index++] = new TestCase ( SECTION, "'abcddddefg'.match(new RegExp('d*'))",
           String([""]), String('abcddddefg'.match(new RegExp('d*'))));

// 'abcddddefg'.match(new RegExp('cd*'))
testcases[index++] = new TestCase ( SECTION, "'abcddddefg'.match(new RegExp('cd*'))",
           String(["cdddd"]), String('abcddddefg'.match(new RegExp('cd*'))));

// 'abcdefg'.match(new RegExp('cx*d'))
testcases[index++] = new TestCase ( SECTION, "'abcdefg'.match(new RegExp('cx*d'))",
           String(["cd"]), String('abcdefg'.match(new RegExp('cx*d'))));

// 'xxxxxxx'.match(new RegExp('(x*)(x+)'))
testcases[index++] = new TestCase ( SECTION, "'xxxxxxx'.match(new RegExp('(x*)(x+)'))",
           String(["xxxxxxx","xxxxxx","x"]), String('xxxxxxx'.match(new RegExp('(x*)(x+)'))));

// '1234567890'.match(new RegExp('(\\d*)(\\d+)'))
testcases[index++] = new TestCase ( SECTION, "'1234567890'.match(new RegExp('(\\d*)(\\d+)'))",
           String(["1234567890","123456789","0"]),
           String('1234567890'.match(new RegExp('(\\d*)(\\d+)'))));

// '1234567890'.match(new RegExp('(\\d*)\\d(\\d+)'))
testcases[index++] = new TestCase ( SECTION, "'1234567890'.match(new RegExp('(\\d*)\\d(\\d+)'))",
           String(["1234567890","12345678","0"]),
           String('1234567890'.match(new RegExp('(\\d*)\\d(\\d+)'))));

// 'xxxxxxx'.match(new RegExp('(x+)(x*)'))
testcases[index++] = new TestCase ( SECTION, "'xxxxxxx'.match(new RegExp('(x+)(x*)'))",
           String(["xxxxxxx","xxxxxxx",""]), String('xxxxxxx'.match(new RegExp('(x+)(x*)'))));

// 'xxxxxxyyyyyy'.match(new RegExp('x*y+$'))
testcases[index++] = new TestCase ( SECTION, "'xxxxxxyyyyyy'.match(new RegExp('x*y+$'))",
           String(["xxxxxxyyyyyy"]), String('xxxxxxyyyyyy'.match(new RegExp('x*y+$'))));

// 'abcdef'.match(/[\d]*[\s]*bc./)
testcases[index++] = new TestCase ( SECTION, "'abcdef'.match(/[\\d]*[\\s]*bc./)",
           String(["bcd"]), String('abcdef'.match(/[\d]*[\s]*bc./)));

// 'abcdef'.match(/bc..[\d]*[\s]*/)
testcases[index++] = new TestCase ( SECTION, "'abcdef'.match(/bc..[\\d]*[\\s]*/)",
           String(["bcde"]), String('abcdef'.match(/bc..[\d]*[\s]*/)));

// 'a1b2c3'.match(/.*/)
testcases[index++] = new TestCase ( SECTION, "'a1b2c3'.match(/.*/)",
           String(["a1b2c3"]), String('a1b2c3'.match(/.*/)));

// 'a0.b2.c3'.match(/[xyz]*1/)
testcases[index++] = new TestCase ( SECTION, "'a0.b2.c3'.match(/[xyz]*1/)",
           null, 'a0.b2.c3'.match(/[xyz]*1/));

test();