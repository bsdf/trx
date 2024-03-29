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

package foo {

    public namespace fletch;
    
    public class A {

        fletch var lives = 'PASSED';
        public function getFletch() {
            return fletch::lives;
        }
    }
    
    public function fletchFunc(){ return 'PASSED'; }
    
    public class fletch2{
        public function lives(){return 'PASSED';}
    }
    
    public interface fletch3 {
        function lives();
    }

    public class erwin implements fletch3 {
        public function lives(){return 'PASSED';}
    }
}

package test{
    public var a = 'PASSED';
}

package foo2{
    import test.*;
    public var b = a;
}

package P
{
    class A
    {
        public function f() { return "original f"}
    }
}


package P
{
    public class B extends A
    {
        override public function f() { return "override f"}
    }
}


import foo2.*;
import P.*;
import foo.*;
use namespace fletch;

var SECTION = "Definitions";       // provide a document reference (ie, Actionscript section)
var VERSION = "AS 3.0";        // Version of ECMAScript or ActionScript
var TITLE   = "PackageDefinition" //Proved ECMA section titile or a description
var BUGNUMBER = "";

startTest();                // leave this alone


AddTestCase( "namespace inside of package", "PASSED", (a = new A(), a.getFletch()));
  
AddTestCase( "import inside of package", "PASSED",b );
AddTestCase( "function inside of package", "PASSED",fletchFunc() );

var FLETCHCLASS = new fletch2();
AddTestCase( "class inside of package", "PASSED",FLETCHCLASS.lives() );

var FLETCHCLASS2 = new erwin();
AddTestCase( "interface inside of package", "PASSED",FLETCHCLASS2.lives() );

b = new B();
AddTestCase("Overriding function within the same package", "override f", b.f());

test();       // leave this alone.  this executes the test cases and
              // displays results.
