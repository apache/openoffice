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
# usage: start.sh <profile> <port> -- headless office on a socket, profile under runs/
source ${0:A:h}/env.sh
mkdir -p $D/runs
nohup $APP/MacOS/soffice -headless -nofirststartwizard -norestore -nologo -nodefault \
  -env:UserInstallation=file://$D/runs/profile_$1 "-accept=socket,host=localhost,port=$2;urp;" \
  >>$D/runs/soffice_$1.log 2>&1 &
for i in {1..180}; do lsof -nP -iTCP:$2 -sTCP:LISTEN >/dev/null 2>&1 && exit 0; sleep 1; done
echo "start.sh: office on port $2 did not come up" >&2; exit 1
