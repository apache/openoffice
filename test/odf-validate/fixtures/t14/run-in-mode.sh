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
# usage: run-in-mode.sh <strict|ext|v13|x13|v14|x14> <port> <outdir> <script.py> [args...]
# Starts an office whose profile (runs/profile_t14_<mode>) sets Save/ODF/DefaultVersion for the mode, runs the pyuno script in it
# and stops the office. Use a port that no gate is using.
source ${0:A:h}/../../env.sh
mode=$1; port=$2; out=$3; script=$4; shift 4
case $mode in ext) v=3;; strict) v=4;; v13) v=5;; x13) v=6;; v14) v=7;; x14) v=8;; *) echo "run-in-mode.sh: unknown mode $mode" >&2; exit 2;; esac
prof=t14_$mode
lsof -nP -iTCP:$port -sTCP:LISTEN >/dev/null 2>&1 && { echo "run-in-mode.sh: port $port is in use" >&2; exit 2; }
mkdir -p $D/runs/profile_$prof/user
cat > $D/runs/profile_$prof/user/registrymodifications.xcu <<XCU
<?xml version="1.0" encoding="UTF-8"?>
<oor:items xmlns:oor="http://openoffice.org/2001/registry" xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
<item oor:path="/org.openoffice.Office.Common/Save/ODF"><prop oor:name="DefaultVersion" oor:op="fuse"><value>$v</value></prop></item>
</oor:items>
XCU
$D/start.sh $prof $port || exit 2
(cd $D && gtimeout 180 $D/runpy.sh $script $port $out $mode "$@")
rc=$?
pkill -f "profile_$prof"; sleep 2
exit $rc
