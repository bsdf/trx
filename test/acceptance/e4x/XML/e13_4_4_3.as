/* -*- Mode: java; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 *
 * ***** BEGIN LICENSE BLOCK *****
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
 * The Original Code is Rhino code, released
 * May 6, 1999.
 *
 * The Initial Developer of the Original Code is
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 1997-2000
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Igor Bukanov
 *   Ethan Hugg
 *   Milen Nankov
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

import avmplus.System

START("13.4.4.3 - XML appendChild()");

//TEST(1, true, XML.prototype.hasOwnProperty("appendChild"));

// Add new employee to list
emps =
<employees>
    <employee id="0"><name>Jim</name><age>25</age></employee>
    <employee id="1"><name>Joe</name><age>20</age></employee>
</employees>;

correct =
<employees>
    <employee id="0"><name>Jim</name><age>25</age></employee>
    <employee id="1"><name>Joe</name><age>20</age></employee>
    <employee id="2"><name>Sue</name><age>30</age></employee>
</employees>;

newEmp = <employee id="2"><name>Sue</name><age>30</age></employee>;

emps.appendChild(newEmp);
TEST(2, correct, emps);

// Add a new child element to the end of Jim's employee element
emps =
<employees>
    <employee id="0"><name>Jim</name><age>25</age></employee>
    <employee id="1"><name>Joe</name><age>20</age></employee>
</employees>;

correct =
<employees>
    <employee id="0"><name>Jim</name><age>25</age><hobby>snorkeling</hobby></employee>
    <employee id="1"><name>Joe</name><age>20</age></employee>
</employees>;

emps.employee.(name == "Jim").appendChild(<hobby>snorkeling</hobby>);
TEST(3, correct, emps);

XML.prettyPrinting = false;
var xmlDoc = "<XML><TEAM>Giants</TEAM><TEAM>Padres</TEAM></XML>";

var expectedResult;
if (System.swfVersion < 10)
    expectedResult = '<XML><TEAM>Giants</TEAM><TEAM>Padres</TEAM><TEAM>Red Sox</TEAM></XML>';
else
    expectedResult = '<XML><TEAM>Giants</TEAM><TEAM>Padres</TEAM><TEAM>&lt;TEAM&gt;Red Sox&lt;/TEAM&gt;</TEAM></XML>';

AddTestCase( "MYXML = new XML(xmlDoc), MYXML.appendChild('<TEAM>Red Sox</TEAM>'), MYXML.toXMLString()",
             expectedResult,
             (MYXML = new XML(xmlDoc), MYXML.appendChild('<TEAM>Red Sox</TEAM>'), MYXML.toXMLString()) );

if (System.swfVersion < 10)
    expectedResult = '<XML><TEAM>Giants<City>San Francisco</City></TEAM><TEAM>Padres</TEAM></XML>';
else
    expectedResult = '<XML><TEAM>Giants&lt;City&gt;San Francisco&lt;/City&gt;</TEAM><TEAM>Padres</TEAM></XML>';
AddTestCase( "MYXML = new XML(xmlDoc), MYXML.TEAM[0].appendChild ('<City>San Francisco</City>')), MYXML.toXMLString()",
             expectedResult,
             (MYXML = new XML(xmlDoc), MYXML.TEAM[0].appendChild ('<City>San Francisco</City>'), MYXML.toXMLString()) );


// Weird behavior of new XML(null) vs XML()

var child = new XML("<TEAM>Giants</TEAM>");
var xml = new XML("foo");

AddTestCase( "MYXML = new XML(null), MYXML.appendChild(new XML('<TEAM>Giants</TEAM>')), MYXML.nodeKind()",
            "text",
            (MYXML = new XML(null), MYXML.appendChild(new XML("<TEAM>Giants</TEAM>")), MYXML.nodeKind()) );

AddTestCase( "MYXML = new XML(null), MYXML.appendChild(new XML('<TEAM>Giants</TEAM>')), MYXML.toString()",
            "",
            (MYXML = new XML(null), MYXML.appendChild(new XML("<TEAM>Giants</TEAM>")), MYXML.toString()) );

// This has weird behavior of seemingly replacing the XML() node with the appended child.  It somehow
// converts the "text" (really "empty") node into a element node
// 03/07/05 [vfleisch] NOT ANYMORE. Updated test case to assert for text, instead of element.
AddTestCase( "MYXML = new XML(), MYXML.appendChild(new XML('<TEAM>Giants</TEAM>')), MYXML.nodeKind()",
            "text",
            (MYXML = new XML(), MYXML.appendChild(new XML("<TEAM>Giants</TEAM>")), MYXML.nodeKind()) );
            
MYXML = new XML();
MYXML.appendChild(new XML("<TEAM>Giants</TEAM>"));

var MYXML = new XML('<LEAGUE></LEAGUE>');
var x1 = new XML('<TEAM>Giants</TEAM>');
MYXML.appendChild(x1);
            
AddTestCase( "duplicate child node - MYXML.appendChild(new XML('<TEAM>Giants</TEAM>')), MYXML.toString()",
            "<LEAGUE><TEAM>Giants</TEAM><TEAM>Giants</TEAM></LEAGUE>",
            (MYXML.appendChild(x1), MYXML.toString()) );

MYXML = new XML('<LEAGUE></LEAGUE>');
x1 = new XML('<TEAM>Giants</TEAM>');
MYXML.appendChild(x1);
            
AddTestCase( "true duplicate child node - MYXML.appendChild(MYXML.child(0)[0]), MYXML.toString()",
            "<LEAGUE><TEAM>Giants</TEAM><TEAM>Giants</TEAM></LEAGUE>",
            (MYXML.appendChild(MYXML.child(0)[0]), MYXML.toString()) );
            
if (System.swfVersion < 10)
    expectedResult = '<root><b>a</b></root>';
else
    expectedResult = '<b>a</b>';

MYXML = new XML('<?xml version="1.0"?><root></root>');
AddTestCase( "MYXML = new XML('<?xml version=\"1.0\"?><root></root>'); MYXML.appendChild(\"<b>a</b>\"), MYXML.toString()",
            expectedResult,
            (MYXML.appendChild("<b>a</b>"), MYXML.toString()));
            
MYXML = new XML('<LEAGUE></LEAGUE>');
x1 = new XMLList('<TEAM t="a">Giants</TEAM><TEAM t="b">Robots</TEAM>');
MYXML.appendChild(x1);
            
AddTestCase( "Append XMLList",
            '<LEAGUE><TEAM t="a">Giants</TEAM><TEAM t="b">Robots</TEAM></LEAGUE>',
            (MYXML.toString()) );
            
MYXML = new XML('<SCARY><MOVIE></MOVIE></SCARY>');
x1 = "poltergeist";
MYXML.MOVIE.appendChild(x1);
            
AddTestCase( "Append a string to child node",
            '<SCARY><MOVIE>poltergeist</MOVIE></SCARY>',
            (MYXML.toString()) );


MYXML = new XML('<SCARY><MOVIE></MOVIE></SCARY>');
x1 = "poltergeist";
MYXML.appendChild(x1);
            
if (System.swfVersion < 10)
    expectedResult = '<SCARY><MOVIE/>poltergeist</SCARY>';
else
    expectedResult = '<SCARY><MOVIE/><MOVIE>poltergeist</MOVIE></SCARY>';

AddTestCase( "Append a string to top node",
            expectedResult,
            (MYXML.toString()) );
            
MYXML = new XML('<SCARY><MOVIE></MOVIE></SCARY>');
x1 = new XML("<the>poltergeist</the>");
MYXML.appendChild(x1);
            
AddTestCase( "Append a node to child node",
            '<SCARY><MOVIE/><the>poltergeist</the></SCARY>',
            (MYXML.toString()) );

var a = <a><b><c/></b></a>;

try {
    a.appendChild (a);
    result = a;
} catch (e1) {
    result = typeError(e1.toString());
}

AddTestCase("a = <a><b><c/></b></a>, a.appendChild(a)", "TypeError: Error #1118", result);


END();
