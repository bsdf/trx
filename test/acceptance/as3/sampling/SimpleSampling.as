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

import flash.sampler.*;
import avmplus.*;

var SECTION = "Sampling";
var VERSION = "AS3";
var TITLE   = "Sampling Basic Usage";

startTest();
writeHeaderToLog("Sampling api");

var isdebugger=System.isDebugger();

var objs:Array=new Array();
class simpleobject {
    var str:String;
    function simpleobject(s:String) {
        this.str=s;
    }
}

sampleInternalAllocs(false);
startSampling();
simpleLoop();
pauseSampling();

if (isdebugger) {
    AddTestCase(
      "test getSampleCount on simple loop internalAllocs=false",
      true,
      getSampleCount()>20
    );
} else {
    AddTestCase(
      "test getSampleCount on simple loop internalAllocs=false",
      false,
      getSampleCount()>20
    );
}
var len:int=0;
for each (var sample in getSamples()) {
  len++;
}
if (isdebugger) {
    AddTestCase(
      "test getSamples length on simple loop internalAllocs=false",
      true,
      len>20
    );

    clearSamples();

    AddTestCase(
      "after clearSamples() sample count is 0, internalAllocs=false",
      0,
      getSampleCount()
    );
}
len=0;
for each (var sample in getSamples()) {
    len++;
}
if (isdebugger) {
    AddTestCase(
      "after clearSamples() getSamples() returns no samples, internalAllocs=false",
      0,
      len
    );
}
sampleInternalAllocs(true);
startSampling();
simpleLoop();
pauseSampling();

if (isdebugger) {
    AddTestCase(
      "test getSampleCount on simple loop internalAllocs=true",
      true,
      getSampleCount()>50
    );
}
var len:int=0;
for each (var sample in getSamples()) {
  len++;
}

if (isdebugger) {
    AddTestCase(
      "test getSamples length on simple loop internalAllocs=true",
      true,
      len>20
    );
}

clearSamples();
    var len:int=0;
    for (sample in getSamples()) {
        if ((sample as DeleteObjectSample)==null) {
            len++;
        }
    }

if (isdebugger) {
    AddTestCase(
      "after clearSamples() sample count is 0, internalAllocs=true",
      0,
      len
    );
}

len=0;
for each (var sample in getSamples()) {
    if ((sample as DeleteObjectSample)==null) {
        len++;
    }
}
if (isdebugger) {
    AddTestCase(
      "after clearSamples() getSamples() returns no samples, internalAllocs=true",
      0,
      len
    );
}

test();

function simpleLoop() {
    for (var i:int=0;i<100;i++) {
       objs.push(new simpleobject(""+i));
    }
}
