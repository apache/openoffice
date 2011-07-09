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
# Use this script to combine "all" of the OpenOffice.org source code
# into a single Mercurial repository.
#
# USAGE:
#   $ ./single-hg.sh OOO340 L10N CWS-DIR SINGLE
#
#     OOO340 is the path to the OOO340 repository
#     L10N is the path to the master_l10n/OOO340 repository
#     CWS-DIR is the directory containing all the CWS repositories
#       to integrate into the output SINGLE repository
#     SINGLE will be created, containing the merge of all the
#       repositories
#
# See <TBD> for converting the resulting repository to Subversion, for
# loading into the ASF repository.
#

if test "$#" != 4; then
  echo "USAGE: $0 OOO340 L10N CWS-DIR SINGLE"
  exit 1
fi

# Turn the parameters into absolute paths
ooo340=`(cd "$1" ; pwd)`
l10n=`(cd "$2" ; pwd)`
cwsdir=`(cd "$3" ; pwd)`

single="$4"

if test -e "$single"; then
  echo "ERROR: working directory ('$single') exists. Please remove it first."
  exit 1
fi

echo "============ cloning to '$single' ..."
hg clone -U "$ooo340" "$single"
cd "$single"

for cws in `ls "$cwsdir"` ; do

  fullpath="$cwsdir/$cws"
  echo "============ pulling from '$fullpath' ..."
  hg pull "$fullpath"

done
