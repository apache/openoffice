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

# usage: fetch-odfvalidator.sh -- download the Apache ODF Toolkit odfvalidator 1.1.8-incubating into tools/
# Apache License 2.0, archived project, used by crosscheck.py. Not committed.
source ${0:A:h}/env.sh
mkdir -p $D/tools
url=https://repo1.maven.org/maven2/org/apache/odftoolkit/odfvalidator/1.1.8-incubating/odfvalidator-1.1.8-incubating-jar-with-dependencies.jar
sum=d362272a7ed04dcc8627fe5210b073f5e53f875820a2522c617ac8073d1d595e
f=$D/tools/${url:t}
[[ -f $f ]] || curl -sfL -o $f $url || { echo "download failed: $url"; exit 1; }
[[ $(shasum -a 256 $f | cut -d' ' -f1) == $sum ]] && echo "ok ${url:t}" || { echo "CHECKSUM MISMATCH ${url:t}"; exit 1; }
