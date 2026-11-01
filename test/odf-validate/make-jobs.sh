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
# usage: make-jobs.sh <outdir> > jobs.tsv -- the test-document corpus under main/, mapped to ODF outputs
source ${0:A:h}/env.sh
out=${1:?usage: make-jobs.sh <outdir>}
i=0
find $ROOT/main \( -path '*/unxmac*' -o -path '*/solver' \) -prune -o -type f \( -name '*.odt' -o -name '*.ods' -o -name '*.odp' -o -name '*.odg' -o -name '*.sxw' -o -name '*.sxc' -o -name '*.sxi' -o -name '*.doc' -o -name '*.xls' -o -name '*.ppt' -o -name '*.rtf' -o -name '*.docx' \) -print | sort |
while read -r f; do
  i=$((i+1))
  case ${f:e} in doc|docx|rtf|odt|sxw) o=odt;; xls|ods|sxc) o=ods;; ppt|odp|sxi) o=odp;; odg) o=odg;; esac
  printf '%s\t%s/%04d_%s.%s\n' "$f" "$out" $i "${f:t:r}" $o
done
