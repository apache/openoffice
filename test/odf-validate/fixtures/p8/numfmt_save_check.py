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

# usage: runpy.sh fixtures/p8/numfmt_save_check.py <port> <outdir> <mode name> <DefaultVersion> <office:version>
# The office must run with Save/ODF DefaultVersion set to <DefaultVersion> in its profile (4 = 1.2, 3 = 1.2 Extended, 5 = 1.3,
# 6 = 1.3 Extended); the setting is read when the office starts. Creates number formats with optional decimals, saves the sheet
# and reads number:decimal-places, number:min-decimal-places and number:decimal-replacement back from the number style of
# each cell. In the 1.3 modes the minimum is written when some decimals are optional; the 1.2 modes must not write it. The saved
# file must carry <office:version> (control that the mode was really in force). Exit code 1 when any row differs.
import os, re, sys, zipfile
import uno
from com.sun.star.lang import Locale
from unohelp import connect, desktop, pv

port, out = int(sys.argv[1]), os.path.abspath(sys.argv[2])
name, want_version = sys.argv[3], sys.argv[5]
os.makedirs(out, exist_ok=True)

# format code, expected (decimal-places, min-decimal-places, decimal-replacement) in the 1.3 modes; the 1.2 modes never get a min
CASES = [
    ("0.0##",        (3, 1, None)),
    ("0.0#",         (2, 1, None)),
    ("0.00#",        (3, 2, None)),
    ("0.000",        (3, None, None)),
    ("0.###",        (3, None, "")),
    ("0.00",         (2, None, None)),
    ("0",            (0, None, None)),
    ("#,##0.0#",     (2, 1, None)),
    ("0.0#%",        (2, 1, None)),
    ("0.0##E+00",    (3, 1, None)),
    ("0.###E+00",    (3, 0, None)),
    ("0.000E+00",    (3, None, None)),
    ('0.0#" kg"',    (2, 1, None)),
    ("[RED]0.0##",   (3, 1, None)),
]

ctx = connect(port)
desk = desktop(ctx)


def attrs(tag):
    return dict(re.findall(r'number:([a-z-]+)="([^"]*)"', tag))


def styles_of(path):
    z = zipfile.ZipFile(path)
    parts = {n: z.read(n).decode("utf-8") for n in ("content.xml", "styles.xml")}
    version = re.search(r'office:version="([^"]+)"', parts["content.xml"]).group(1)
    allx = parts["content.xml"] + parts["styles.xml"]
    datastyle = dict(re.findall(r'<style:style style:name="([^"]+)"[^>]*style:data-style-name="([^"]+)"', allx))
    numstyle = {}
    for m in re.finditer(r'<number:(?:number|percentage|currency)-style style:name="([^"]+)"[^>]*>(.*?)</number:(?:number|percentage|currency)-style>', allx, re.S):
        e = re.search(r'<number:(?:number|scientific-number)\b([^>]*)/?>', m.group(2))
        numstyle[m.group(1)] = attrs(e.group(1)) if e else {}
    # a cell whose style is the default of its column carries no table:style-name
    coldef = re.search(r'<table:table-column[^>]*table:default-cell-style-name="([^"]+)"', parts["content.xml"])
    cells = []
    for row in re.findall(r"<table:table-row[^>]*>(.*?)</table:table-row>", parts["content.xml"], re.S):
        m = re.match(r"<table:table-cell([^>]*)>", row)
        sn = re.search(r'table:style-name="([^"]+)"', m.group(1)) if m else None
        cells.append(sn.group(1) if sn else (coldef.group(1) if coldef else ""))
    return version, [numstyle.get(datastyle.get(c, ""), {}) for c in cells]


doc = desk.loadComponentFromURL("private:factory/scalc", "_blank", 0, (pv("Hidden", True),))
sheet = doc.Sheets.getByIndex(0)
formats = doc.NumberFormats
loc = Locale("en", "US", "")
for i, (code, _) in enumerate(CASES):
    key = formats.queryKey(code, loc, False)
    if key == -1:
        key = formats.addNew(code, loc)
    cell = sheet.getCellByPosition(0, i)
    cell.Value = 1.5
    cell.NumberFormat = key

fails = 0
for i, (code, _) in enumerate(CASES):
    fmt = formats.getByKey(sheet.getCellByPosition(0, i).NumberFormat).FormatString
    if fmt != code:
        print("FAIL setup %r: the cell has format %r" % (code, fmt))
        fails += 1
path = os.path.join(out, "save-%s.ods" % name)
doc.storeToURL(uno.systemPathToFileUrl(path), (pv("FilterName", "calc8"),))
version, cells = styles_of(path)
ok_v = version == want_version
print("%-4s %-7s office:version=%s (want %s)" % ("ok" if ok_v else "FAIL", name, version, want_version))
fails += not ok_v
for i, (code, (places, minimum, repl)) in enumerate(CASES):
    a = cells[i] if i < len(cells) else {}
    want_min = minimum if want_version == "1.3" else None
    got = (int(a["decimal-places"]) if "decimal-places" in a else None,
           int(a["min-decimal-places"]) if "min-decimal-places" in a else None,
           a.get("decimal-replacement"))
    want = (places, want_min, repl)
    ok = got == want
    fails += not ok
    print("%-4s %-7s %-14s got %r want %r" % ("ok" if ok else "FAIL", name, code, got, want))
doc.close(True)
print("%d failing rows" % fails)
sys.exit(1 if fails else 0)
