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
# Use this script to fetch all a project's webcontent for the projects
# listed in the specified file (typically, webcontent-list.txt).
#
# See https://cwiki.apache.org/confluence/display/OOOUSERS/OOo-Sitemap
# for a note on the checkout from the Kenai svn repository.
#
# USAGE:
#   $ ./kenai2website.sh WEB-PROJECT SITE-DIR
#
#     WEB-LIST is a file containing the list of Projects to fetch
#       (see the file tools/dev/webcontent-list.txt)
#     WORK-DIR each project's webcontent will be created in a
#       subdirectory of WORK-DIR
#
#  Future steps will include scripts to transform the content for
#  the Apache CMS or a Confluence Wiki import
#

if test "$#" != 1; then
  echo "USAGE: $0 PROJECT"
  exit 1
fi

REPOS='https://svn.openoffice.org/svn/'
REPOS2='~webcontent'

webproject=$1

cd /tmp
rm -rf ${webproject}

webrepos=${REPOS}${webproject}${REPOS2}

echo "============ '$webproject' is being exported ..."
svn export $webrepos $webproject
