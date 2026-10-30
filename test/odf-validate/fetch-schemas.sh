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

# usage: fetch-schemas.sh -- download the OASIS ODF 1.2, 1.3 and 1.4 RELAX NG schemas into schema/
# They are not committed: they carry OASIS copyright terms.
source ${0:A:h}/env.sh
mkdir -p $D/schema
typeset -A SUMS
SUMS=(
  https://docs.oasis-open.org/office/v1.2/os/OpenDocument-v1.2-os-schema.rng ca5d2510858c6f922b20cd27dc76d34959c1a976b9dfc6b5caeadbebfc83dbbb
  https://docs.oasis-open.org/office/v1.2/os/OpenDocument-v1.2-os-manifest-schema.rng 75f972902bc785c15e51fb4e112e2f1b24e2327829eb54786cf8687ee39c7631
  https://docs.oasis-open.org/office/v1.2/os/OpenDocument-v1.2-os-dsig-schema.rng 52fbf196dc11fcaac41ff4b320f005daf419794c2f343a26ecab05778d7e7f6f
  https://docs.oasis-open.org/office/OpenDocument/v1.3/os/schemas/OpenDocument-v1.3-schema.rng 40bad03efdbb02825230d357da0aa6ac679934c5bf56c6281752c0c24d58e4e6
  https://docs.oasis-open.org/office/OpenDocument/v1.3/os/schemas/OpenDocument-v1.3-manifest-schema.rng 8aee71f03484be112af972d622cc9031280c007b0a454ae8815e8e333c9bdd17
  https://docs.oasis-open.org/office/OpenDocument/v1.3/os/schemas/OpenDocument-v1.3-dsig-schema.rng 1760eab7fba2394b6b9d2c77b4ed2187ed01ff8029f8538ba7bff3b834866573
  https://docs.oasis-open.org/office/OpenDocument/v1.4/os/schemas/OpenDocument-v1.4-schema.rng 4034ec6be29205d5fc1ee5f42468ac6ef824287b3aba6d9289032af4fafbda7f
  https://docs.oasis-open.org/office/OpenDocument/v1.4/os/schemas/OpenDocument-v1.4-manifest-schema.rng f96e1fe95f4e3609717a307c28e2fe0654c890fe70ea03eb8a53caa20b684782
)
rc=0
for url sum in ${(kv)SUMS}; do
  f=$D/schema/${url:t}
  [[ -f $f ]] || curl -sfL -o $f $url || { echo "download failed: $url"; rc=1; continue; }
  [[ $(shasum -a 256 $f | cut -d' ' -f1) == $sum ]] && echo "ok ${url:t}" || { echo "CHECKSUM MISMATCH ${url:t}"; rc=1; }
done
exit $rc
