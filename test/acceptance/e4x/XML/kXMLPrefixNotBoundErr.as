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

START("13.4 XML Object - XML prefix not bound error");

var xml;
expectedStr = "TypeError: Error #1083: The prefix \"ns\" for element \"ns:x\" is not bound.";
expected = "Error #1083";
result = "error, exception not thrown";

try{

xml = new XML("<ns:x><a>1</a><a>2</a><a>3</a></ns:x>");
throw new Error("kXMLPrefixNotBound error not thrown");

} catch( e1 ){

result = grabError(e1, e1.toString());

}

AddTestCase( "x = new XML(var xml = new XML(\"<ns:x><a>1</a><a>2</a><a>3</a></ns:x>\")", expected, result );



expectedStr = "TypeError: Error #1083: The prefix \"ns\" for element \"ns:x\" is not bound.";
expected = "Error #1083";
result = "error, exception not thrown";

try{

xml = new XML("<ns:x xmlns:as=\"http://foo.bar\"><a>1</a><a>2</a><a>3</a></ns:x>");
throw new Error("kXMLPrefixNotBound error not thrown");

} catch( e2 ){

result = grabError(e2, e2.toString());

}

AddTestCase( "x = new XML(var xml = new XML(\"<ns:x xmlns:as=\"http://foo.bar\"><a>1</a><a>2</a><a>3</a></ns:x>\")", expected, result );



expectedStr = "TypeError: Error #1083: The prefix \"as\" for element \"as:a\" is not bound.";
expected = "Error #1083";
result = "error, exception not thrown";

try{

xml = new XML("<ns:b xmlns:ns=\"http://foo.bar\"><as:a>1</as:a></ns:b>");
throw new Error("kXMLPrefixNotBound error not thrown");

} catch( e3 ){

result = grabError(e3, e3.toString());

}

AddTestCase( "x = new XML(var xml = new XML(\"<ns:b xmlns:ns=\"http://foo.bar\"><as:a>1</as:a></ns:b>\")", expected, result );


END();

