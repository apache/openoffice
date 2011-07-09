#!/bin/sh
#
# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.
#

#
# Use this script to fetch all the CWS repositories listed in the
# specified file (typically, cws-list.txt). Before using this script,
# you should have a local Hg repository of the OOO340 repository. This
# can be quickly done by fetching the bundle from:
#   http://hg.services.openoffice.org/bundle/OOO340.hg
#
# USAGE:
#   $ ./fetch-all-cws.sh OOO340 CWS-LIST WORK-DIR
#
#     OOO340 is the path to the OOO340 repository
#     L10N is the path to the master_l10n/OOO340 repository
#     CWS-LIST is a file containing the list of CWSs to fetch
#       (see the file tools/dev/cws-list.txt)
#     WORK-DIR each CWS will be created in a subdirectory of WORK-DIR
#
# The script may be restarted. Each existing CWS will be refreshed,
# and the remaining CWSs will be fetched.
#
# Once the repositories are all fetched, see tools/dev/single-hg.sh to
# combine them into a single repository.
#

if test "$#" != 4; then
  echo "USAGE: $0 OOO340 L10N CWS-LIST WORK-DIR"
  exit 1
fi

REPOS='http://hg.services.openoffice.org'

# Make the work directory, in case it does not exist
if test ! -e "$3"; then
  mkdir "$3"
fi

# Turn the parameters into absolute paths
ooo340=`(cd "$1" ; pwd)`
l10n=`(cd "$2" ; pwd)`
work=`(cd "$4" ; pwd)`

cwsdir=`dirname "$3"`
cwsfile=`basename "$3"`
cwslist=`(cd "$cwsdir" ; pwd)`/$cwsfile


for cwsrepos in `grep '^cws' $cwslist` ; do
  cd "$work"

  master="$ooo340"

  # Construct a name for our working directory
  cws=`echo $cwsrepos | sed 's#.*/##'`
  if test "`echo $cwsrepos | sed -n '/cws_l10n/p'`" ; then
    master="$l10n"
    cws="l10n.$cws"
  fi

  if test -d "$cws" ; then
    echo "============ '$cws' exists. Pulling ..."
    cd "$cws"
    hg pull "$REPOS/$cwsrepos"

  elif test -e "$cws" ; then
    echo "ERROR: '$cws' exists and is not a directory."
    exit 1

  # filter out empty CWS: hg incoming returns 1 if there's nothing to pull
  elif hg -R "$master" incoming "$REPOS/$cwsrepos" >/dev/null; then
    echo "============ '$cws' is being created ..."
    hg clone -U "$master" "$cws"
    cd "$cws"
    hg pull "$REPOS/$cwsrepos"

  else
    echo "============ '$cws' skipped: nothing to pull ..."
  fi

done
