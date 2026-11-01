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

# usage: runpy.sh fixtures/p8/numfmt_check.py <port> <outdir>
# Loads an ODS whose cells use number styles with number:min-decimal-places and number:fill-character (ODF 1.3) and
# compares the format code and the displayed text of each cell with the expected ones. Rows marked "control" must give the
# same result before and after the reader change. Exit code 1 when any row differs.
import os, sys, zipfile
import uno
from unohelp import connect, desktop, load

port, out = int(sys.argv[1]), os.path.abspath(sys.argv[2])
os.makedirs(out, exist_ok=True)

NS = ('xmlns:office="urn:oasis:names:tc:opendocument:xmlns:office:1.0" '
      'xmlns:style="urn:oasis:names:tc:opendocument:xmlns:style:1.0" '
      'xmlns:table="urn:oasis:names:tc:opendocument:xmlns:table:1.0" '
      'xmlns:number="urn:oasis:names:tc:opendocument:xmlns:datastyle:1.0"')

# name, number-style content, cell value, expected format code (None: only checked by the regex below), expected text
# (suffix match); the number-style is always an en-US one
# sci-min1-of-3-display: the format code is right (ODF wants 1.5E+05), but the scientific output of SvNumberformat shows the optional
# digits as zeros; the row pins that known limit
CASES = [
    ("min1-of-3",          '<number:number number:decimal-places="3" number:min-decimal-places="1" number:min-integer-digits="1"/>', 1.5,    "0.0##",     "1.5"),
    ("min1-of-3-integer",  '<number:number number:decimal-places="3" number:min-decimal-places="1" number:min-integer-digits="1"/>', 2,      "0.0##",     "2.0"),
    ("min1-of-3-full",     '<number:number number:decimal-places="3" number:min-decimal-places="1" number:min-integer-digits="1"/>', 1.2345, "0.0##",     "1.235"),
    ("min0-of-3",          '<number:number number:decimal-places="3" number:min-decimal-places="0" number:min-integer-digits="1"/>', 1.5,    "0.###",     "1.5"),
    ("min-equals-places",  '<number:number number:decimal-places="2" number:min-decimal-places="2" number:min-integer-digits="1"/>', 1.5,    "0.00",      "1.50"),
    ("control-no-min",     '<number:number number:decimal-places="2" number:min-integer-digits="1"/>',                               1.5,    "0.00",      "1.50"),
    ("control-var-dec",    '<number:number number:decimal-places="2" number:decimal-replacement="" number:min-integer-digits="1"/>', 1.5,    "0.##",      "1.5"),
    ("var-dec-and-min1",   '<number:number number:decimal-places="3" number:decimal-replacement="" number:min-decimal-places="1" number:min-integer-digits="1"/>', 1.5, "0.0##", "1.5"),
    ("min-above-places",   '<number:number number:decimal-places="2" number:min-decimal-places="5" number:min-integer-digits="1"/>', 1.5,    "0.00",      "1.50"),
    ("control-dashes",     '<number:number number:decimal-places="2" number:decimal-replacement="-" number:min-integer-digits="1"/>', 2,     "0.--",      "2.--"),
    ("dashes-and-min1",    '<number:number number:decimal-places="2" number:decimal-replacement="-" number:min-decimal-places="1" number:min-integer-digits="1"/>', 2, "0.--", "2.--"),
    ("sci-min1-of-3-display",      '<number:scientific-number number:decimal-places="3" number:min-decimal-places="1" number:min-integer-digits="1" number:min-exponent-digits="2"/>', 150000, "0.0##E+00", "1.500E+05"),
    ("control-sci",        '<number:scientific-number number:decimal-places="3" number:min-integer-digits="1" number:min-exponent-digits="2"/>', 150000, "0.000E+00", "1.500E+05"),
    ("fill-before",        '<number:fill-character>.</number:fill-character><number:number number:decimal-places="2" number:min-integer-digits="1"/>', 1.5, "*.0.00", "1.50"),
    ("fill-after-text",    '<number:text>$</number:text><number:fill-character>-</number:fill-character><number:number number:decimal-places="2" number:min-integer-digits="1"/>', 1.5, '"$"*-0.00', "1.50"),
    ("fill-twice",         '<number:fill-character>.</number:fill-character><number:fill-character>-</number:fill-character><number:number number:decimal-places="2" number:min-integer-digits="1"/>', 1.5, "*.0.00", "1.50"),
    ("fill-two-chars",     '<number:fill-character>ab</number:fill-character><number:number number:decimal-places="2" number:min-integer-digits="1"/>', 1.5, "0.00", "1.50"),
    ("fill-empty",         '<number:fill-character/><number:number number:decimal-places="2" number:min-integer-digits="1"/>', 1.5, "0.00", "1.50"),
]

styles, cells = [], []
for i, (name, content, value, _, _) in enumerate(CASES):
    styles.append('<number:number-style style:name="N%d" number:language="en" number:country="US">%s</number:number-style>' % (i, content))
    styles.append('<style:style style:name="ce%d" style:family="table-cell" style:data-style-name="N%d"/>' % (i, i))
    cells.append('<table:table-row><table:table-cell table:style-name="ce%d" office:value-type="float" office:value="%s"/></table:table-row>' % (i, value))

content = ('<?xml version="1.0" encoding="UTF-8"?>\n<office:document-content %s office:version="1.3"><office:automatic-styles>%s'
           '</office:automatic-styles><office:body><office:spreadsheet><table:table table:name="T">%s</table:table>'
           '</office:spreadsheet></office:body></office:document-content>' % (NS, "".join(styles), "".join(cells)))
manifest = ('<?xml version="1.0" encoding="UTF-8"?>\n<manifest:manifest xmlns:manifest="urn:oasis:names:tc:opendocument:xmlns:manifest:1.0" '
            'manifest:version="1.3"><manifest:file-entry manifest:full-path="/" manifest:version="1.3" '
            'manifest:media-type="application/vnd.oasis.opendocument.spreadsheet"/>'
            '<manifest:file-entry manifest:full-path="content.xml" manifest:media-type="text/xml"/></manifest:manifest>')
path = os.path.join(out, "numfmt.ods")
with zipfile.ZipFile(path, "w") as z:
    z.writestr("mimetype", "application/vnd.oasis.opendocument.spreadsheet", zipfile.ZIP_STORED)
    z.writestr("META-INF/manifest.xml", manifest, zipfile.ZIP_DEFLATED)
    z.writestr("content.xml", content, zipfile.ZIP_DEFLATED)

ctx = connect(port)
doc = load(desktop(ctx), path)
sheet = doc.Sheets.getByIndex(0)
formats = doc.NumberFormats
fails = 0
for i, (name, _, _, want_fmt, want_txt) in enumerate(CASES):
    cell = sheet.getCellByPosition(0, i)
    fmt = formats.getByKey(cell.NumberFormat).FormatString
    txt = cell.String
    ok = (fmt == want_fmt) and txt.endswith(want_txt)
    fails += not ok
    print("%-4s %-20s format=%-12r text=%-12r want %r / %r" % ("ok" if ok else "FAIL", name, fmt, txt, want_fmt, want_txt))
doc.close(True)
print("%d of %d rows ok" % (len(CASES) - fails, len(CASES)))
sys.exit(1 if fails else 0)
