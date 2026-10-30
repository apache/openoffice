#!/usr/bin/env python3
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
#
# usage: runpy.sh fixtures/t16/check.py <port> <outdir>
# A chart title whose text sits in text:span elements must be read like the same text without them. Prints PASS/FAIL lines.
import sys, os, re, zipfile, uno
from unohelp import connect, desktop, pv

port, out = int(sys.argv[1]), os.path.abspath(sys.argv[2])
os.makedirs(out, exist_ok=True)
desk = desktop(connect(port))

def check(name, ok, detail=""):
    print(("PASS " + name) if ok else ("FAIL %s %s" % (name, detail)), flush=True)

doc = desk.loadComponentFromURL("private:factory/scalc", "_blank", 0, (pv("Hidden", True),))
sh = doc.Sheets.getByIndex(0)
for r, row in enumerate(((1, 2), (3, 4), (5, 6))):
    for c, v in enumerate(row):
        sh.getCellByPosition(c, r).Value = v
rect = uno.createUnoStruct("com.sun.star.awt.Rectangle"); rect.X, rect.Y, rect.Width, rect.Height = 1000, 4000, 8000, 6000
rng = uno.createUnoStruct("com.sun.star.table.CellRangeAddress"); rng.Sheet, rng.StartColumn, rng.StartRow, rng.EndColumn, rng.EndRow = 0, 0, 0, 1, 2
sh.Charts.addNewByName("c1", rect, (rng,), False, False)
chart = sh.Charts.getByName("c1").EmbeddedObject
chart.HasMainTitle = True
chart.Title.String = "My Title"
base = os.path.join(out, "chart-title.ods")
doc.storeToURL(uno.systemPathToFileUrl(base), (pv("FilterName", "calc8"),)); doc.close(True)

with zipfile.ZipFile(base) as zin:
    entries = [(i.filename, zin.read(i.filename)) for i in zin.infolist()]
part = [n for n, d in entries if n.endswith("/content.xml") and b"<chart:title" in d][0]
orig = dict(entries)[part].decode("utf8")
m = re.search(r"(<chart:title\b[^>]*>)<text:p>My Title</text:p>(</chart:title>)", orig)
check("chart title written as plain text:p", m is not None)

def title_of(markup, name):
    p = os.path.join(out, name + ".ods")
    with zipfile.ZipFile(p, "w") as zo:
        for n, d in entries:
            if n == part:
                d = orig.replace(m.group(0), m.group(1) + markup + m.group(2)).encode("utf8")
            zo.writestr(n, d, compress_type=zipfile.ZIP_STORED if n == "mimetype" else zipfile.ZIP_DEFLATED)
    d = desk.loadComponentFromURL(uno.systemPathToFileUrl(p), "_blank", 0, (pv("Hidden", True),))
    c = d.Sheets.getByIndex(0).Charts.getByIndex(0).EmbeddedObject
    t = c.Title.String if c.HasMainTitle else None
    d.close(True)
    return t

check("plain paragraph", title_of("<text:p>My Title</text:p>", "v0") == "My Title")
check("one span", title_of('<text:p><text:span text:style-name="T1">My Title</text:span></text:p>', "v1") == "My Title")
check("two spans", title_of("<text:p><text:span>My </text:span><text:span>Title</text:span></text:p>", "v2") == "My Title")
plain = title_of("<text:p>ABC<text:tab/>D<text:line-break/>E</text:p>", "v4")
check("nested spans match the same text without spans", title_of("<text:p>A<text:span>B<text:span>C</text:span></text:span><text:tab/>D<text:span><text:line-break/>E</text:span></text:p>", "v3") == plain and plain is not None, plain)
