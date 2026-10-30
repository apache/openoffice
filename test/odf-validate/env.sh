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

# Sourced by the harness scripts. Override any ODFV_* variable in the environment.
D=${0:A:h}
: ${D:?env.sh: cannot resolve harness dir}
ROOT=${D:h:h}
APP=${ODFV_APP:-$D/app/OpenOffice.app}/Contents
# must be the Python that the office's pyuno links against (otool -L $APP/program/libpyuno.dylib)
PY=${ODFV_PYTHON:-/usr/local/Library/Frameworks/Python.framework/Versions/3.14/bin/python3.14}
JING=${ODFV_JING:-/Applications/XMLauthor/lib/jing.jar}
SOLVER=${ODFV_SOLVER:-$(print -l $ROOT/main/solver/*/unxmac*/lib(N/) | head -1)}
