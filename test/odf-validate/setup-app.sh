#!/bin/zsh
#**************************************************************
#
#  Licensed to the Apache Software Foundation (ASF) under one
#  or more contributor license agreements.  See the NOTICE file
#  distributed with this work for additional information
#  regarding copyright ownership.  The ASF licenses this file
#  to you under the Apache License, Version 2.0 (the
#  "License"); you may not use this file except in compliance
#  with the License.  You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing,
#  software distributed under the License is distributed on an
#  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
#  KIND, either express or implied.  See the License for the
#  specific language governing permissions and limitations
#  under the License.
#
#**************************************************************

# usage: setup-app.sh <AOO macOS .dmg> -- copy OpenOffice.app out of a build's dmg into app/
source ${0:A:h}/env.sh
dmg=${1:?usage: setup-app.sh <dmg>}
mnt=$(mktemp -d ${TMPDIR:-/tmp}/odfv-mnt.XXXXXX)
: ${mnt:?}
hdiutil attach -nobrowse -readonly -mountpoint $mnt $dmg </dev/null >/dev/null || exit 1
if [[ -d $D/app/OpenOffice.app ]]; then
  /bin/mv $D/app/OpenOffice.app $D/app/OpenOffice.app.old.$(date +%s)
fi
mkdir -p $D/app && ditto $mnt/OpenOffice.app $D/app/OpenOffice.app
rc=$?
hdiutil detach $mnt -quiet; rmdir $mnt
(( rc == 0 )) && echo "app: $D/app/OpenOffice.app ($(defaults read $D/app/OpenOffice.app/Contents/Info.plist CFBundleShortVersionString))"
exit $rc
