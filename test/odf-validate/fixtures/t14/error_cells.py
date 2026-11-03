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

# usage: runpy.sh fixtures/t14/error_cells.py <port> <outdir>
# Builds an ODS with office:value-type="error" cells (with and without office:string-value, error code only in the text:p, a
# repeated cell, a string cell as control), loads it, compares each cell's error code with the expected one, saves it again and
# compares the reloaded file. Exit status 1 when a row fails.
import os, sys, zipfile
import uno
from unohelp import FILTERS, connect, desktop, pv

port, out = int(sys.argv[1]), os.path.abspath(sys.argv[2])
os.makedirs(out, exist_ok=True)

# text, expected error code (519 = #VALUE!, the fallback)
CASES = [("#NULL!", 521), ("#DIV/0!", 532), ("#VALUE!", 519), ("#REF!", 524), ("#NAME?", 525), ("#NUM!", 503),
         ("#N/A", 32767), ("Err:504", 504), ("Err:99999", 519), ("Err:12x", 519), ("#BOGUS", 519), ("", 519)]

NS = ('xmlns:office="urn:oasis:names:tc:opendocument:xmlns:office:1.0" xmlns:table="urn:oasis:names:tc:opendocument:xmlns:table:1.0" '
      'xmlns:text="urn:oasis:names:tc:opendocument:xmlns:text:1.0"')
rows = []
for t, _ in CASES:
    p = "<text:p>%s</text:p>" % t if t else ""
    rows.append('<table:table-row><table:table-cell office:value-type="error" office:string-value="%s">%s</table:table-cell>'
                '<table:table-cell office:value-type="error">%s</table:table-cell></table:table-row>' % (t, p, p))
rows.append('<table:table-row><table:table-cell office:value-type="error" office:string-value="#DIV/0!" table:number-columns-repeated="3"/></table:table-row>')
rows.append('<table:table-row><table:table-cell office:value-type="string" office:string-value="#DIV/0!"><text:p>#DIV/0!</text:p></table:table-cell></table:table-row>')
rows.append('<table:table-row><table:table-cell office:value-type="error"><text:p><text:span>#DIV/</text:span><text:span>0!</text:span></text:p></table:table-cell></table:table-row>')
content = ('<?xml version="1.0" encoding="UTF-8"?><office:document-content %s office:version="1.4"><office:body><office:spreadsheet>'
           '<table:table table:name="Sheet1"><table:table-column table:number-columns-repeated="3"/>%s</table:table></office:spreadsheet>'
           '</office:body></office:document-content>') % (NS, "".join(rows))
manifest = ('<?xml version="1.0" encoding="UTF-8"?><manifest:manifest xmlns:manifest="urn:oasis:names:tc:opendocument:xmlns:manifest:1.0" '
            'manifest:version="1.4"><manifest:file-entry manifest:full-path="/" manifest:version="1.4" '
            'manifest:media-type="application/vnd.oasis.opendocument.spreadsheet"/>'
            '<manifest:file-entry manifest:full-path="content.xml" manifest:media-type="text/xml"/></manifest:manifest>')
src = os.path.join(out, "error-cells.ods")
with zipfile.ZipFile(src, "w") as z:
    z.writestr(zipfile.ZipInfo("mimetype"), "application/vnd.oasis.opendocument.spreadsheet")
    z.writestr("META-INF/manifest.xml", manifest, zipfile.ZIP_DEFLATED)
    z.writestr("content.xml", content, zipfile.ZIP_DEFLATED)

fails = 0


TEXT = {521: "#NULL!", 532: "#DIV/0!", 519: "#VALUE!", 524: "#REF!", 525: "#NAME?", 503: "#NUM!", 32767: "#N/A", 504: "Err:504"}


def check(label, doc, asstring):
    # asstring: the saved file holds error cells as value-type="string" (no formula), so the reload is a text cell
    global fails
    sh = doc.Sheets.getByIndex(0)

    def row(what, c, code):
        global fails
        if asstring:
            ok = c.getError() == 0 and c.getString() == TEXT[code] and c.getFormula() == TEXT[code]
            got = "error %d string %r formula %r" % (c.getError(), c.getString(), c.getFormula())
        else:
            ok = c.getError() == code
            got = "error %d" % c.getError()
        fails += not ok
        print("%-7s %-26s expect %-7s got %s  %s" % (label, what, TEXT[code] if asstring else code, got, "ok" if ok else "FAIL"))

    for i, (t, code) in enumerate(CASES):
        for col, kind in ((0, "string-value"), (1, "text only")):
            row("%r %s" % (t, kind), sh.getCellByPosition(col, i), code)
    for col in range(3):
        row("repeated col %d" % col, sh.getCellByPosition(col, len(CASES)), 532)
    c = sh.getCellByPosition(0, len(CASES) + 1)
    ok = c.getError() == 0 and c.getString() == "#DIV/0!"
    fails += not ok
    print("%-7s string control: error %d string %r  %s" % (label, c.getError(), c.getString(), "ok" if ok else "FAIL"))
    row("text in text:span", sh.getCellByPosition(0, len(CASES) + 2), 532)


ctx = connect(port)
desk = desktop(ctx)
load = lambda p: desk.loadComponentFromURL(uno.systemPathToFileUrl(p), "_blank", 0, (pv("Hidden", True),))
doc = load(src)
if doc is None:
    sys.exit("could not load " + src)
check("load", doc, False)
# an unchanged sheet is copied verbatim in some modes, so touch it to force the real export
doc.Sheets.getByIndex(0).getCellByPosition(5, 0).setValue(1)
dst = os.path.join(out, "error-cells-resaved.ods")
if os.path.exists(dst):
    os.remove(dst)
doc.storeToURL(uno.systemPathToFileUrl(dst), (pv("FilterName", FILTERS["ods"]), pv("Overwrite", True)))
doc.close(True)
doc = load(dst)
check("reload", doc, True)
doc.close(True)
print("FAILS", fails)
sys.exit(1 if fails else 0)
