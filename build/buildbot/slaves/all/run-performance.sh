#!/bin/bash
#  ***** BEGIN LICENSE BLOCK *****
#  Version: MPL 1.1/GPL 2.0/LGPL 2.1
# 
#  The contents of this file are subject to the Mozilla Public License Version
#  1.1 (the "License"); you may not use this file except in compliance with
#  the License. You may obtain a copy of the License at
#  http://www.mozilla.org/MPL/
# 
#  Software distributed under the License is distributed on an "AS IS" basis,
#  WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
#  for the specific language governing rights and limitations under the
#  License.
# 
#  The Original Code is [Open Source Virtual Machine.].
# 
#  The Initial Developer of the Original Code is
#  Adobe System Incorporated.
#  Portions created by the Initial Developer are Copyright (C) 2009
#  the Initial Developer. All Rights Reserved.
# 
#  Contributor(s):
#    Adobe AS3 Team
# 
#  Alternatively, the contents of this file may be used under the terms of
#  either the GNU General Public License Version 2 or later (the "GPL"), or
#  the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
#  in which case the provisions of the GPL or the LGPL are applicable instead
#  of those above. If you wish to allow use of your version of this file only
#  under the terms of either the GPL or the LGPL, and not to allow others to
#  use your version of this file under the terms of the MPL, indicate your
#  decision by deleting the provisions above and replace them with the notice
#  and other provisions required by the GPL or the LGPL. If you do not delete
#  the provisions above, a recipient may use your version of this file under
#  the terms of any one of the MPL, the GPL or the LGPL.
# 
#  ***** END LICENSE BLOCK ****
(set -o igncr) 2>/dev/null && set -o igncr; # comment is needed


### PARAMS ###
# $1 = revision or revision:hash
# $2 = avmshell to run
# $3 = vmargs
# $4 = config (aka testsuite)
# $5 = iterations
# $6 = android-run?  (True or '', optional)

shellname=$2
vmargs=$3
config=$4
iterations=$5
android=$6

##
# Bring in the environment variables
##
. ./environment.sh


##
# Calculate the change number and change id
##
. ../all/util-calculate-change.sh $1

##
# Run any slave specific code PRE performance
##
. ./run-performance-pre.sh

if [ $android ]; then
    ##
    # Test for adb in path, if not found choosecombo must be run
    ##
    adb=`which adb`
    test "$adb" == "" && {
        echo "ERROR: android sdk is not setup.  Please see buildbot machines wiki for setup instructions."
        exit 1
    }
fi

##
# Download the AVMSHELL if it does not exist
##
download_shell $shellname


##
# Download the latest asc.jar if it does not exist
##
download_asc

echo ""
echo "Building ABC files using the following ASC version:"
echo "`java -jar $ASC`"
echo ""


export AVM=$buildsdir/$change-${changeid}/$platform/$shellname

if [ $android ]; then
    echo "setup $branch/${change}-${changeid}"
    ../all/adb-shell-deployer.sh ${change} ${buildsdir}/${change}-${changeid}/${platform}/$shellname
    res=$?
    test "$res" = "0" || {
        echo "message: setup failed"
        exit 1
    }
    export AVM=$basedir/platform/android/android_shell.py
fi

echo ""
echo AVM=$AVM
echo "`$AVM`"
echo; echo "AVM built with the following options:"
echo "`$AVM -Dversion`"
echo ""

##
# Ensure that the system is clean and ready
##
cd $basedir/build/buildbot/slaves/scripts
../all/util-acceptance-clean.sh

cd $basedir/test/performance

measurememory="false"
$AVM | grep "\-memstats" > /dev/null
test "$?" = "0" && {
  measurememory="true"
}


result="0"
resultmessage=""

echo ""
echo "===========   $config   ==========="
python ./runtests.py --config=$config -r $branch -k -f -i $iterations --vmargs="$vmargs" --repo="$repo"
test "$?" = "0" || { 
    result="1"; 
    resultmessage="$config time test run failed. " 
}

test "$measurememory" = "true" && {
    python ./runtests.py --config=$config --memory -r $branch -k -f -i 1 --vmargs="$vmargs" --repo="$repo"
    test "$?" = "0" || { 
        result="1"; 
        resultmessage="$resultmessage \n$config memory test run failed. "
    }
}
echo ""
  

export memory_url="http://tamarin-builds.mozilla.org/report/index.cfm?hostip=${hostip}&config1=${configDefault}&config2=${configDefault}&configbaseline=${baselineConfigMemory}&baselineBuild=${baselineBuildMemory}&topBuild=${change}&product1=${branch}&product2=${branch}&productBaseline=${baselineBranchMemory}&raw=false&ratio=false&verbose=false&group=Desktop&metricsChecked=memory"
export time_url="http://tamarin-builds.mozilla.org/report/index.cfm?hostip=${hostip}&config1=${configDefault}&config2=${configDefault}&configbaseline=${baselineConfigTime}&baselineBuild=${baselineBuildTime}&topBuild=${change}&product1=${branch}&product2=${branch}&productBaseline=${baselineBranchTime}&raw=false&ratio=true&verbose=false&group=Desktop&metricsChecked=time"

echo ""
echo ""
echo "url: ${time_url} time report"
if [ "$measurememory" == "true" ]
then
    echo "url: ${memory_url} memory report"
fi

test -f results.log && rm results.log
wget -O results.log -q ${time_url}
retry="0"
perfchg=""
while true
 do
  test -f results.log && {
    sleep 10
    perfchg=`cat results.log | grep -m1 "perfchange_ratio_time:" | awk -F"perfchange_ratio_time:" '{print $2}' | awk '{print $1}'`
    break
  }
  retry=`echo $retry | awk '{ print $1+1 }'`
  test "$retry" = "20" && break
  sleep 1
done
echo "perfchange: ${perfchg}%"


##
# Run any slave specific code POST performance
##
cd $basedir/build/buildbot/slaves/scripts/
. ./run-performance-post.sh

##
# Ensure that the system is torn down and clean
# Skip on android so that other avmshell processes on android host do not get
# killed.
##
if [ ! $android ]; then
    cd $basedir/build/buildbot/slaves/scripts
    ../all/util-acceptance-teardown.sh
fi

test "$result" = "0" && resultmessage="performance tests passed"
echo -e "message: $resultmessage"

exit $result