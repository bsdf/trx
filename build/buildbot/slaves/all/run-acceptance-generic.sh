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

##
# Bring in the environment variables
##
. ./environment.sh


##
# Calculate the change number and change id
##
. ../all/util-calculate-change.sh $1

showhelp ()
{
    echo ""
    echo "usage: run-acceptance-generic.sh <change> <filename> <vmargs> <config> <scriptargs>"
    echo "  <change>     build number of shell to test"
    echo "  <filename>   name of the shell, do not include file extenstion"
    echo "  <vmargs>     vmargs to be passed or empty quoted string"
    echo "  <config>     custom config string to be passed to runtests.py"
    echo "               or an empty string"
    echo "  <scriptargs> (optional) additional runtests.py args to be passed, can be"
    echo "               args and test/directories to run, value is appended to call"
    exit 1
}

if [ "$#" -lt 4 ]
then
    echo "not enough args passed"
    showhelp
fi

filename=$2
vmargs=$3
config=$4

# assign the remaining positional params to scriptargs
shift 4
scriptargs=$*


export shell=$filename$shell_extension


##
# Download the AVMSHELL if it does not exist
##
download_shell $shell


##
# Download the latest asc.jar if it does not exist
##
download_asc


echo ""
echo "Missing media will be compiled using the following ASC version:"
echo "`java -jar $ASC`"
echo ""



export AVM=$buildsdir/$change-${changeid}/$platform/$shell

echo AVM=$AVM
echo "`$AVM`"
test -f $AVM || {
  echo "ERROR: $AVM not found"
  exit 1
}
echo; echo "AVM built with the following options:"
echo "`$AVM -Dversion`"


##
# Ensure that the system is clean and ready
##
cd $basedir/build/buildbot/slaves/scripts
../all/util-acceptance-clean.sh

cd $basedir/test/acceptance



if [ "$silent" == "true" ] && [ "$internal_repo" == "true" ]; then
    logfile=`echo acceptance-$shell$vmargs.log | sed 's/ //g' | sed 's/\.exe//g'`
    silentoptions="-l $logfile --summaryonly"
fi

if [ "$config" != "" ]
then
    echo "message: $PYTHON_RUNTESTS ./runtests.py --vmargs=\"${vmargs}\" --config=${config} --notimecheck ${scriptargs} ${silentoptions}"
    $PYTHON_RUNTESTS ./runtests.py  --vmargs="${vmargs}" --config=${config} --notimecheck ${scriptargs} ${silentoptions}
else
    echo "message: $PYTHON_RUNTESTS ./runtests.py --vmargs=\"${vmargs}\" --notimecheck ${scriptargs} ${silentoptions}"
    $PYTHON_RUNTESTS ./runtests.py  --vmargs="${vmargs}" --notimecheck ${scriptargs} ${silentoptions}
fi

if [ "$silent" == "true" ] && [ "$internal_repo" == "true" ]; then
    # upload log to asteam
    $basedir/build/buildbot/slaves/all/util-upload-ftp-asteam.sh $logfile $ftp_asteam/$branch/${change}-${changeid}/$platform/
    ret=$?
    if [ "$ret" != "0" ]; then
	echo "Uploading of $logfile failed"
	exit 1
    fi
    echo "Acceptance logfile can be found here: http://asteam.corp.adobe.com/builds/$branch/${change}-${changeid}/$platform/$logfile"
    echo "url: http://asteam.corp.adobe.com/builds/$branch/${change}-${changeid}/$platform/$logfile logfile"
fi

##
# Ensure that the system is torn down and clean
##
cd $basedir/build/buildbot/slaves/scripts
../all/util-acceptance-teardown.sh

exit 0


