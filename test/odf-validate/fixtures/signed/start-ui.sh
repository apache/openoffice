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
# usage: start-ui.sh <strict|ext> <port> -- like start.sh but -invisible instead of -headless, because -headless
# cancels every modal dialog and the signing dialog is the only way to sign. Uses the throw-away NSS database
# (MOZILLA_CERTIFICATE_FOLDER, which also stops AOO from looking at any Mozilla profile) and creates the profile.
source ${0:A:h}/../../env.sh
mode=$1; port=$2
case $mode in ext) v=3;; strict) v=4;; *) echo "start-ui.sh: mode must be ext or strict" >&2; exit 2;; esac
db=$D/runs/task12/gen-signed/nssdb
[[ -e $db/cert9.db ]] || { echo "start-ui.sh: run setup-cert.sh first" >&2; exit 2; }
prof=$D/runs/profile_t12_sign_$mode
if [[ ! -e $prof/user/registrymodifications.xcu ]]; then
  mkdir -p $prof/user
  cat > $prof/user/registrymodifications.xcu <<XCU
<?xml version="1.0" encoding="UTF-8"?>
<oor:items xmlns:oor="http://openoffice.org/2001/registry" xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
<item oor:path="/org.openoffice.Office.Common/Save/ODF"><prop oor:name="DefaultVersion" oor:op="fuse"><value>$v</value></prop></item>
</oor:items>
XCU
fi
export MOZILLA_CERTIFICATE_FOLDER=sql:$db
nohup $APP/MacOS/soffice -invisible -nofirststartwizard -norestore -nologo -nodefault \
  -env:UserInstallation=file://$prof "-accept=socket,host=localhost,port=$port;urp;" \
  >>$D/runs/soffice_t12_sign_$mode.log 2>&1 &
for i in {1..180}; do lsof -nP -iTCP:$port -sTCP:LISTEN >/dev/null 2>&1 && exit 0; sleep 1; done
echo "start-ui.sh: office on port $port did not come up" >&2; exit 1
