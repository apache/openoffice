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

# usage: runpy.sh fixtures/t14/handles.py <port> <outdir> <tag> [file.odg]
# With a file, nothing is drawn: the file is loaded and the handles of its shapes (in the order of the types below) are written.
# Draws custom shapes with polar handles (circular-arrow, block-arc, ring), with XY handles (sun, round-rectangle) and a ribbon
# in a Draw document, saves it with the office's configured ODF version, reloads the file and writes <outdir>/<tag>.json with the
# handles of every shape after the reload; the test compares the lists between the modes (v13 is the control).
import json, os, sys
import uno
from unohelp import connect, desktop, pv

port, out, tag = int(sys.argv[1]), os.path.abspath(sys.argv[2]), sys.argv[3]
os.makedirs(out, exist_ok=True)
ctx = connect(port)
desk = desktop(ctx)
TYPES = ["circular-arrow", "block-arc", "ring", "sun", "round-rectangle", "ribbon", "mso-spt163", "smiley", "heart"]


def handles(shape):
    geo = shape.CustomShapeGeometry
    res = []
    for prop in geo:
        if prop.Name == "Handles":
            for h in prop.Value:
                d = {}
                for p in h:
                    v = p.Value
                    d[p.Name] = [(x.Value, x.Type) for x in (v.First, v.Second)] if hasattr(v, "First") else ((v.Value, v.Type) if hasattr(v, "Type") and hasattr(v, "Value") else v)
                res.append(d)
    return res


doc = None if len(sys.argv) > 4 else desk.loadComponentFromURL("private:factory/sdraw", "_blank", 0, (pv("Hidden", True),))
page = doc.DrawPages.getByIndex(0) if doc else None
for i, t in enumerate(TYPES if doc else []):
    sh = doc.createInstance("com.sun.star.drawing.CustomShape")
    page.add(sh)
    sh.Size = uno.createUnoStruct("com.sun.star.awt.Size", 4000, 3000)
    sh.Position = uno.createUnoStruct("com.sun.star.awt.Point", 500 + (i % 3) * 5000, 500 + (i // 3) * 4000)
    sh.CustomShapeGeometry = (pv("Type", t),)
if doc:
    path = os.path.join(out, tag + ".odg")
    if os.path.exists(path):
        os.remove(path)
    doc.storeToURL(uno.systemPathToFileUrl(path), (pv("FilterName", "draw8"), pv("Overwrite", True)))
    doc.close(True)
else:
    path = os.path.abspath(sys.argv[4])
d2 = desk.loadComponentFromURL(uno.systemPathToFileUrl(path), "_blank", 0, (pv("Hidden", True),))
page2 = d2.DrawPages.getByIndex(0)
res = {}
for i, t in enumerate(TYPES):
    res[t] = handles(page2.getByIndex(i))
d2.close(True)
json.dump(res, open(os.path.join(out, "handles-" + tag + ".json"), "w"), indent=1, sort_keys=True, default=str)
print(tag, {t: len(v) for t, v in res.items()})
