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
# usage: fixtures/gen-run.sh <area> <strict|ext|v13|x13> <port> <outdir>
# Starts a headless office saving in the given ODF mode (profile created as drive.sh does), runs fixtures/<area>/make.py <port> <outdir>, stops it.
source ${0:A:h:h}/env.sh
area=$1; mode=$2; port=$3; out=$4
case $mode in ext) v=3;; strict) v=4;; v13) v=5;; x13) v=6;; *) echo "gen-run.sh: unknown mode $mode" >&2; exit 2;; esac
[[ -f $D/fixtures/$area/make.py ]] || { echo "gen-run.sh: no fixtures/$area/make.py" >&2; exit 2; }
prof=t12_${area}_$mode
/bin/rm -rf $D/runs/profile_$prof
mkdir -p $D/runs/profile_$prof/user
cat > $D/runs/profile_$prof/user/registrymodifications.xcu <<XCU
<?xml version="1.0" encoding="UTF-8"?>
<oor:items xmlns:oor="http://openoffice.org/2001/registry" xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
<item oor:path="/org.openoffice.Office.Common/Save/ODF"><prop oor:name="DefaultVersion" oor:op="fuse"><value>$v</value></prop></item>
</oor:items>
XCU
$D/start.sh $prof $port || exit 2
$D/runpy.sh $D/fixtures/$area/make.py $port $out
rc=$?
pkill -f "[p]rofile_$prof"
exit $rc
