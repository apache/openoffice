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
# usage: config_roundtrip.sh <outdir>
# Starts an office per Save/ODF/DefaultVersion value 0..8 (profile runs/profile_cfgN, port 2310+N) and runs config_roundtrip.py in it.
source ${0:A:h}/../../env.sh
out=${1:?outdir}
rc=0
for n in {0..8}; do
  prof=cfg$n
  mkdir -p $D/runs/profile_$prof/user
  cat > $D/runs/profile_$prof/user/registrymodifications.xcu <<XCU
<?xml version="1.0" encoding="UTF-8"?>
<oor:items xmlns:oor="http://openoffice.org/2001/registry" xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
<item oor:path="/org.openoffice.Office.Common/Save/ODF"><prop oor:name="DefaultVersion" oor:op="fuse"><value>$n</value></prop></item>
</oor:items>
XCU
  $D/start.sh $prof $((2310 + n)) || { rc=1; continue; }
  (cd $D && gtimeout 120 $D/runpy.sh fixtures/t14/config_roundtrip.py $((2310 + n)) $out $n) || rc=1
  pkill -f "profile_$prof"; sleep 2
done
exit $rc
