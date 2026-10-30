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

# usage: runpy.sh fixtures/p8/gridsnap_check.py <port> <outdir> <mode name> <strict: 1|0>
# The office runs with the save mode of <mode name>. Sets a character grid on the default Writer page style, saves, and checks
# which spelling of the snap-to attribute styles.xml carries: `style:layout-grid-snap-to` in the ODF 1.2 and 1.3 modes,
# `style:layout-grid-snap-to-characters` (the name released versions read) in every other mode, never both.
# Exit code 1 when a check fails.
import os, re, sys, zipfile
import uno
from unohelp import connect, desktop, pv

port, out, name, strict = int(sys.argv[1]), os.path.abspath(sys.argv[2]), sys.argv[3], sys.argv[4] == "1"
os.makedirs(out, exist_ok=True)
desk = desktop(connect(port))
doc = desk.loadComponentFromURL("private:factory/swriter", "_blank", 0, (pv("Hidden", True),))
ps = doc.StyleFamilies.getByName("PageStyles").getByName("Standard")
ps.setPropertyValue("GridMode", 2)                  # lines and characters
ps.setPropertyValue("GridSnapToChars", True)
ps.setPropertyValue("StandardPageMode", True)       # the exporter drops the grid unless this is set
path = os.path.join(out, "gridsnap-%s.odt" % name)
doc.storeToURL(uno.systemPathToFileUrl(path), (pv("FilterName", "writer8"),))
doc.close(True)
styles = zipfile.ZipFile(path).read("styles.xml").decode("utf-8")
new = len(re.findall(r'style:layout-grid-snap-to="', styles))
old = len(re.findall(r'style:layout-grid-snap-to-characters="', styles))
print("%s: layout-grid-snap-to x%d, layout-grid-snap-to-characters x%d, layout-grid-mode: %s" % (
    name, new, old, re.findall(r'style:layout-grid-mode="[^"]*"', styles)[:1]))
checks = []
if strict:
    checks.append(("the new spelling (snap-to) only", new > 0 and old == 0))
else:
    checks.append(("the old spelling (snap-to-characters) only", old > 0 and new == 0))
fails = 0
for label, ok in checks:
    fails += not ok
    print("%-4s %-7s %s" % ("ok" if ok else "FAIL", name, label))
sys.exit(1 if fails else 0)
