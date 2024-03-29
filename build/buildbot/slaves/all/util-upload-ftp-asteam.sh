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


source=$1
test "$source" = "" && {
    echo "usage: util-upload-ftp-asteam.sh <source> <dest>"
    exit 1
}

dest=$2
test "$dest" = "" && {
    echo "usage: util-upload-ftp-asteam.sh <source> <dest>"
    exit 1
}


##
# Upload the requested file
##

echo "Uploading $source to $dest"

## Just quickly make sure that the permisions are fully open on the file
chmod 777 $source

maxtries=5
x=1

while [ $x -le $maxtries ]
do
    curl --fail --ftp-create-dirs -T $source ftp://${dest}
    res=$?
    if [ "$res" != "0" ]; then
	echo "Upload appears to have failed, exitcode: $res"
        if [ $x -lt $maxtries ]; then
            x=$(( $x + 1 ))
            echo "Attempting upload again.  Attempt $x of $maxtries."
            sleep 5
        else 
            echo "Upload failed after $maxtries tries.  Aborting upload."
            exit 1
        fi
    else
	echo "Upload successful"
	exit 0
    fi
done

exit 0
