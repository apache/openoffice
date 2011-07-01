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
# you should have a local Hg repository of the DEV300 repository.
# See http://wiki.services.openoffice.org/wiki/Getting_It.
#
# USAGE:
#   $ ./fetch-all-cws.sh DEV300 CWS-LIST WORK-DIR
#
#     DEV300 is the path to the DEV300 repository
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

if test "$#" != 3; then
  echo "USAGE: $0 DEV300 CWS-LIST WORK-DIR"
  exit 1
fi

REPOS='http://hg.services.openoffice.org'

# Make the work directory, in case it does not exist
if test ! -e "$3"; then
  mkdir "$3"
fi

# Turn the parameters into absolute paths
dev300=`(cd "$1" ; pwd)`
work=`(cd "$3" ; pwd)`

cwsdir=`dirname "$2"`
cwsfile=`basename "$2"`
cwslist=`(cd "$cwsdir" ; pwd)`/$cwsfile


for cws in `sed -n '/^cws\//s#^cws/##p' $cwslist` ; do
  cd "$work"

  if test -d "$cws" ; then
    echo "============ '$cws' exists. Pulling ..."
    cd "$cws"
    hg pull "$REPOS/cws/$cws"
  elif test -e "$cws" ; then
    echo "ERROR: '$cws' exists and is not a directory."
    exit 1
  else
    # filter out empty CWS: hg incoming returns 1 if there's nothing to pull
    if hg -R "$dev300" incoming "$REPOS/cws/$cws" >/dev/null; then
        echo "============ '$cws' is being created ..."
        hg clone -U "$dev300" "$cws"
        cd "$cws"
        hg pull "$REPOS/cws/$cws"
    else
        echo "============ '$cws' skipped: nothing to pull ..."
    fi
  fi

done
