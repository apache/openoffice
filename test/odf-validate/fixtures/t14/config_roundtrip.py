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

# usage: runpy.sh fixtures/t14/config_roundtrip.py <port> <outdir> <value>   (run for the values 0..8 by config_roundtrip.sh)
# Saves a Writer, a Calc, an Impress and a Draw document (writer8, calc8, impress8, draw8) with the office's Save/ODF/DefaultVersion
# set in its profile to <value> and reads office:version of every XML stream and manifest:version back from the package.
# Expected: 1 = no version, 2 = 1.1, 3 and 4 = 1.2, 5 and 6 = 1.3, 7 and 8 = 1.4; 0 (unknown) is only reported.
import os, re, sys, zipfile
import uno
from unohelp import FILTERS, connect, desktop, pv

port, out, n = int(sys.argv[1]), os.path.abspath(sys.argv[2]), int(sys.argv[3])
os.makedirs(out, exist_ok=True)
EXPECT = {1: None, 2: "1.1", 3: "1.2", 4: "1.2", 5: "1.3", 6: "1.3", 7: "1.4", 8: "1.4"}
ctx = connect(port)
sm = ctx.ServiceManager
desk = desktop(ctx)
def build(doc, ext):
    if ext == "odt":
        doc.Text.insertString(doc.Text.createTextCursor(), "text", False)
    elif ext == "ods":
        doc.Sheets.getByIndex(0).getCellByPosition(0, 0).Value = 1
    else:
        sh = doc.createInstance("com.sun.star.drawing.TextShape")
        doc.DrawPages.getByIndex(0).add(sh)
        sh.String = "shape"


URL = {"odt": "private:factory/swriter", "ods": "private:factory/scalc", "odp": "private:factory/simpress", "odg": "private:factory/sdraw"}
bad = 0
for ext in ("odt", "ods", "odp", "odg"):
    doc = desk.loadComponentFromURL(URL[ext], "_blank", 0, (pv("Hidden", True),))
    build(doc, ext)
    path = os.path.join(out, "c%d.%s" % (n, ext))
    if os.path.exists(path):
        os.remove(path)
    doc.storeToURL(uno.systemPathToFileUrl(path), (pv("FilterName", FILTERS[ext]), pv("Overwrite", True)))
    doc.close(True)
    z = zipfile.ZipFile(path)
    vers = {}
    for name in ("content.xml", "styles.xml", "meta.xml", "settings.xml"):
        if name in z.namelist():
            m = re.search(r'<office:document-[a-z]+ [^>]*office:version="([0-9.]+)"', z.read(name).decode("utf-8"))
            vers[name] = m.group(1) if m else None
    mm = re.search(r'<manifest:manifest [^>]*manifest:version="([0-9.]+)"', z.read("META-INF/manifest.xml").decode("utf-8"))
    vers["manifest"] = mm.group(1) if mm else None
    want = EXPECT.get(n, "skip")
    if n == 0:
        ok = True
    else:
        ok = all(v == want for k, v in vers.items() if k != "manifest") and (vers["manifest"] == want if want and want >= "1.2" else True)
    bad += 0 if ok else 1
    print("%s config %d %s: want %s got %s" % ("PASS" if ok else "FAIL", n, ext, want, vers))
print("config_roundtrip %d: %d failures" % (n, bad))
sys.exit(1 if bad else 0)
