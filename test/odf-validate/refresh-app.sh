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
# usage: refresh-app.sh <lib.dylib>... -- copy rebuilt libs from the solver into the app copy
source ${0:A:h}/env.sh
: ${SOLVER:?} ${APP:?}
# overwriting a library an office has mapped makes macOS SIGKILL everything that later touches the file
pgrep -f "$APP/MacOS/soffice" > /dev/null && { echo "refresh-app.sh: an office from $APP is still running; stop it first" >&2; exit 3; }
rc=0
for l in "$@"; do [[ -n $l && $l != */* ]] || { echo "bad lib name: $l"; exit 2; }; done
for l in "$@"; do
  /bin/cp -f $SOLVER/$l $APP/program/$l && cmp -s $SOLVER/$l $APP/program/$l && echo "updated $l ($(stat -f %Sm $SOLVER/$l))" || { echo "FAILED $l"; rc=1; }
done
exit $rc
