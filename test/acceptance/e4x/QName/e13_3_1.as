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

START("13.3.1 - QName Constructor as a Function");

q = QName("foobar");
p = new QName("foobar");
TEST(1, typeof(p), typeof(q));
TEST(2, p.localName, q.localName);
TEST(3, p.uri, q.uri);

q = QName("http://foobar/", "foobar");
p = new QName("http://foobar/", "foobar");
TEST(4, typeof(p), typeof(q));
TEST(5, p.localName, q.localName);
TEST(6, p.uri, q.uri);

p1 = QName(q);
p2 = new QName(q);
TEST(7, typeof(p2), typeof(p1));
TEST(8, p2.localName, p1.localName);
TEST(9, p2.uri, p1.uri);

n = new Namespace("http://foobar/");
q = QName(n, "foobar");
p = QName(n, "foobar");
TEST(10, typeof(p), typeof(q));
TEST(11, p.localName, q.localName);
TEST(12, p.uri, q.uri);

p = QName(q);
TEST(13, p, q);

// One value is supplied
AddTestCase( "QName('name').valueOf().toString()", 'name', QName('name').valueOf().toString() );
AddTestCase( "QName('name').valueOf() == 'name'", true, QName('name').valueOf() == 'name' );
AddTestCase( "typeof QName('name')", "object", typeof QName('name') );
//AddTestCase( "QName('name').__proto__", Namespace.prototype, QName('name').__proto__ );

// If one parameter is QName, same value is returned
AddTestCase ("foo = QName('foo'), bar = Qname(foo), bar === foo", true,
    (foo = QName("foo"), bar = QName(foo), bar === foo));

// If one parameter is QName but there is a Namespace param, different object is returned
AddTestCase ("foo = QName('foo'), bar = Qname(\"\", foo), bar === foo", false,
    (foo = QName("foo"), bar = QName("", foo), bar === foo));


//Two values are supplied
AddTestCase( "ns = new Namespace('duh'), QName(ns, 'name')",
    "duh::name",
    (ns = new Namespace('duh'), QName(ns, 'name').toString() ));
    
AddTestCase( "ns = new Namespace('duh'), typeof QName(ns, 'name')",
    "object",
    ( ns = new Namespace('duh'), typeof QName(ns, 'name') ));

AddTestCase( "ns = new Namespace('duh'), typeof QName(ns, 'name')",
    true,
    ( ns = new Namespace('duh'), QName(ns, 'name') instanceof QName));


END();