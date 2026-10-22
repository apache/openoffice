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
# usage: drive.sh <ext|strict> <port> <jobs.tsv> <done.tsv>
# Converts jobs in batches of 20. Stops on the first soffice crash; kills and resumes on a hang.
source ${0:A:h}/env.sh
mode=$1; port=$2; jobs=$3; done_=$4; log=${done_:r}.log
prof=$mode
if [[ ! -e $D/runs/profile_$prof/user/registrymodifications.xcu ]]; then
  mkdir -p $D/runs/profile_$prof/user
  v=3; [[ $mode == strict ]] && v=4
  cat > $D/runs/profile_$prof/user/registrymodifications.xcu <<XCU
<?xml version="1.0" encoding="UTF-8"?>
<oor:items xmlns:oor="http://openoffice.org/2001/registry" xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
<item oor:path="/org.openoffice.Office.Common/Save/ODF"><prop oor:name="DefaultVersion" oor:op="fuse"><value>$v</value></prop></item>
</oor:items>
XCU
fi
total=$(wc -l < $jobs)
(( total > 0 )) || { echo "drive.sh: no jobs in $jobs" >&2; exit 2; }
while true; do
  started=$(cut -f1 $done_ 2>/dev/null | sort -u | wc -l)
  (( started >= total )) && break
  lsof -nP -iTCP:$port -sTCP:LISTEN >/dev/null 2>&1 || $D/start.sh $prof $port || exit 2
  gtimeout 300 $D/runpy.sh $D/convert.py $port $jobs $done_ 20 >> $log 2>&1
  rc=$?; echo "rc=$rc" >> $log
  if (( rc == 124 )); then echo "HANG: $(tail -1 $done_)" >> $log; pkill -f "profile_$prof"; sleep 3
  elif (( rc != 0 )); then echo "CRASH: $(tail -2 $done_ | head -1)" >> $log; exit 1; fi
done
pkill -f "profile_$prof"
echo DONE >> $log
