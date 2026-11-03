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

# usage: runpy.sh fixtures/t14/resave.py <port> <outdir> <tag> <input>...
# Loads each input hidden and saves it again (writer8, calc8, impress8, draw8 by extension) as <outdir>/<tag>-<name>, with whatever
# ODF version the running office is configured for. Used with an older office (ODFV_APP=/Applications/OpenOffice5.app) to see
# what it keeps of a file written in a newer ODF version.
import os, sys
import uno
from unohelp import FILTERS, connect, desktop, pv

port, out, tag = int(sys.argv[1]), os.path.abspath(sys.argv[2]), sys.argv[3]
os.makedirs(out, exist_ok=True)
ctx = connect(port)
desk = desktop(ctx)
for src in sys.argv[4:]:
    src = os.path.abspath(src)
    doc = desk.loadComponentFromURL(uno.systemPathToFileUrl(src), "_blank", 0, (pv("Hidden", True), pv("ReadOnly", True)))
    if doc is None:
        print("could not load", src)
        continue
    dst = os.path.join(out, tag + "-" + os.path.basename(src))
    if os.path.exists(dst):
        os.remove(dst)
    doc.storeToURL(uno.systemPathToFileUrl(dst), (pv("FilterName", FILTERS[os.path.splitext(src)[1][1:]]), pv("Overwrite", True)))
    doc.close(True)
    print("resaved", os.path.basename(src))
