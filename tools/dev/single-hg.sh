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
#   $ ./single-hg.sh path/to/bundle/DEV300.hg single
#
# You need to separately fetch the 1.6 Gb bundle from:
#   http://hg.services.openoffice.org/bundle/DEV300.hg
#
# (see http://wiki.services.openoffice.org/wiki/Getting_It)
#
# See <TBD> for converting the resulting repository to Subversion, for
# loading into the ASF repository.
#

if test "$#" != 2; then
  echo "USAGE: $0 path/to/bundle/DEV300.hg WORKING_DIR"
  exit 1
fi

bundle="$1"
work="$2"

if test -e "$work"; then
  echo "ERROR: working directory ('$work') exists. Please remove it first."
  exit 1
fi

mkdir "$work"
cd "$work"

hg init
hg unbundle "../$bundle" || exit 1
hg pull http://hg.services.openoffice.org/DEV300
hg update

### now we need to deal with the CWS's
