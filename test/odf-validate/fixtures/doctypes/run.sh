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
# usage: fixtures/doctypes/run.sh <strict|ext> <port> <outdir>
# Starts a headless office saving in the given ODF mode (profile created as drive.sh does), runs make.py, stops it.
source ${0:A:h:h:h}/env.sh
mode=$1; port=$2; out=$3
case $mode in ext) v=3;; strict) v=4;; *) echo "run.sh: unknown mode $mode" >&2; exit 2;; esac
prof=t12_main_$mode
mkdir -p $D/runs/profile_$prof/user
cat > $D/runs/profile_$prof/user/registrymodifications.xcu <<XCU
<?xml version="1.0" encoding="UTF-8"?>
<oor:items xmlns:oor="http://openoffice.org/2001/registry" xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
<item oor:path="/org.openoffice.Office.Common/Save/ODF"><prop oor:name="DefaultVersion" oor:op="fuse"><value>$v</value></prop></item>
</oor:items>
XCU
$D/start.sh $prof $port || exit 2
$D/runpy.sh $D/fixtures/doctypes/make.py $port $out
rc=$?
pkill -f "profile_$prof"
exit $rc
