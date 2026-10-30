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
# usage: run.sh gen|verify   -- builds or reloads the base-*.odb corpus in strict and ext mode (ports 2130-2133)
# A fresh profile is made for every office start: a profile left by a crashed office does not come up again.
source ${0:A:h:h:h}/env.sh
G=$D/runs/task12/gen-base
what=$1

fresh() {  # fresh <profile name> <DefaultVersion> <port>
  [[ $1 == t12_base_* ]] || { echo "run.sh: bad profile $1" >&2; exit 2; }
  pkill -f "[p]rofile_$1"; sleep 1
  /bin/rm -rf "${D:?}/runs/profile_${1:?}"
  mkdir -p $D/runs/profile_$1/user
  cat > $D/runs/profile_$1/user/registrymodifications.xcu <<XCU
<?xml version="1.0" encoding="UTF-8"?>
<oor:items xmlns:oor="http://openoffice.org/2001/registry" xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
<item oor:path="/org.openoffice.Office.Common/Save/ODF"><prop oor:name="DefaultVersion" oor:op="fuse"><value>$2</value></prop></item>
</oor:items>
XCU
  dismiss &
  dp=$!
  $D/start.sh $1 $3; rc=$?
  kill $dp 2>/dev/null
  return $rc
}

dismiss() {  # after a crash macOS asks whether to reopen windows; clicks the first soffice window, which may be another session's
  for i in {1..90}; do
    osascript -e 'tell application "System Events" to tell process "soffice" to click button "Don’t Reopen" of window 1' >/dev/null 2>&1 && return
    sleep 1
  done
}

if [[ $what == gen ]]; then
  for m in strict:4:2130 ext:3:2131; do
    n=${m%%:*}; rest=${m#*:}; v=${rest%%:*}; port=${rest##*:}
    /bin/rm -rf "${G:?}/${n:?}/out" "${G:?}/work/${n:?}"
    mkdir -p $G/$n/out
    fresh t12_base_$n $v $port || exit 1
    gtimeout 600 $D/runpy.sh fixtures/base/make.py $port $G/$n/out $G/work/$n
    pkill -f "[p]rofile_t12_base_$n"
  done
elif [[ $what == verify ]]; then
  for m in strict:4:2132 ext:3:2133; do
    n=${m%%:*}; rest=${m#*:}; v=${rest%%:*}; port=${rest##*:}
    for f in $G/$n/out/base-*.odb; do
      for try in 1 2 3; do
        fresh t12_base_verify_$n $v $port || continue
        out=$(gtimeout 120 $D/runpy.sh fixtures/base/verify.py $port $G/$n/out ${f:t} 2>&1 | tail -1)
        pkill -f "[p]rofile_t12_base_verify_$n"
        [[ $out == base-* ]] && { echo "[$n] try=$try $out"; break; }
        echo "[$n] try=$try ${f:t} FAILED: ${out:0:120}"
      done
    done
  done
else
  echo "usage: run.sh gen|verify" >&2; exit 2
fi
