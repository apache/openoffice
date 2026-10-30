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
# usage: setup-cert.sh -- creates the throw-away NSS database runs/task12/gen-signed/nssdb with one self-signed
# signing certificate (empty password). Only that directory is written.
source ${0:A:h}/../../env.sh
db=$D/runs/task12/gen-signed/nssdb
[[ -e $db/cert9.db ]] && { echo "setup-cert.sh: $db exists"; exit 0; }
mkdir -p $db
certutil -N -d sql:$db --empty-password || exit 1
head -c 64 /dev/urandom > $db/noise.bin
certutil -S -d sql:$db -n "ODF Test Signer" -s "CN=ODF Test Signer,O=ODF Validation Harness" -x -t "u,u,u" \
  -g 2048 -m 1 -v 120 -z $db/noise.bin --keyUsage digitalSignature,nonRepudiation || exit 1
/bin/rm -f $db/noise.bin
certutil -L -d sql:$db
