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
# usage: run.sh <port-base>   -- writes runs/task12/gen-encrypted/<strict|ext>/{out,twin,plain} for the four
# UseSHA1InODF12/UseBlowfishInODF12 combinations, using profiles t12_enc_<mode>[_variant] on ports base..base+7
source ${0:A:h}/../../env.sh
base=${1:?port base}
G=$D/runs/task12/gen-encrypted
port=$base
for mode in strict ext; do
  case $mode in ext) v=3;; strict) v=4;; esac
  mkdir -p $G/$mode/out $G/$mode/twin $G/$mode/plain
  for var in default aes sha1aes sha256bf; do
    case $var in default) sha1=true; bf=true; p=t12_enc_$mode;; aes) sha1=false; bf=false; p=t12_enc_${mode}_aes;;
      sha1aes) sha1=true; bf=false; p=t12_enc_${mode}_sha1aes;; sha256bf) sha1=false; bf=true; p=t12_enc_${mode}_sha256bf;; esac
    /bin/rm -rf $D/runs/profile_$p
    mkdir -p $D/runs/profile_$p/user
    cat > $D/runs/profile_$p/user/registrymodifications.xcu <<XCU
<?xml version="1.0" encoding="UTF-8"?>
<oor:items xmlns:oor="http://openoffice.org/2001/registry" xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
<item oor:path="/org.openoffice.Office.Common/Save/ODF"><prop oor:name="DefaultVersion" oor:op="fuse"><value>$v</value></prop></item>
<item oor:path="/org.openoffice.Office.Common/Save/ODF"><prop oor:name="UseSHA1InODF12" oor:op="fuse"><value>$sha1</value></prop></item>
<item oor:path="/org.openoffice.Office.Common/Save/ODF"><prop oor:name="UseBlowfishInODF12" oor:op="fuse"><value>$bf</value></prop></item>
</oor:items>
XCU
    $D/start.sh $p $port || exit 2
    (cd $D && ./runpy.sh fixtures/encrypted/make.py $port $G/$mode/out $G/$mode/twin) || exit 3
    pkill -f "profile_$p"
    (( port++ ))
  done
done
