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

package {
    import avmplus.Domain
    import avmplus.System

    var SECTION = "BugCompatibility";
    var VERSION = "BugCompatibility";
    startTest();
    var TITLE   = "Bug 535770";

    writeHeaderToLog( SECTION + " "+ TITLE );

    var leaf_path:String = System.argv[0];
    var test_path:String = System.argv[1];

    var SWF10_domain = new Domain(Domain.currentDomain);
    SWF10_domain.load(test_path, 10);
    var SWF10_cl = SWF10_domain.getClass("testclass");
    var SWF10_helper = new SWF10_cl;

    var SWF11_domain = new Domain(Domain.currentDomain);
    SWF11_domain.load(test_path, 11);
    var SWF11_cl = SWF11_domain.getClass("testclass");
    var SWF11_helper = new SWF11_cl;

    var result;

    result = SWF10_helper.vtest(leaf_path, SWF10_domain, 10);
    AddTestCase("Test Vector.concat with SWF10 loading SWF10 behavior",
      "1,4,3,2",
      result.toString());

    result = SWF11_helper.vtest(leaf_path, SWF11_domain, 11);
    AddTestCase("Test Vector.concat with SWF11 loading SWF11 behavior",
      "1,2,3,4",
      result.toString());

    result = SWF10_helper.vtest(leaf_path, SWF10_domain, 11);
    AddTestCase("Test Vector.concat with SWF10 loading SWF11 behavior",
      "1,2,3,4",
      result.toString());

    result = SWF11_helper.vtest(leaf_path, SWF11_domain, 10);
    AddTestCase("Test Vector.concat with SWF11 loading SWF10 behavior",
      "1,4,3,2",
      result.toString());

    test();

}