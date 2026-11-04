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

# usage: make-consumer.py                 -- (re)generate the ODF 1.4 consumer test packages in fixtures/t14/consumer/
#        make-consumer.py --report <val.json>  -- print the table of validate.py results for them
# Each CASES entry is one construct from the ODF 1.4 additions inside an otherwise plain document, written with
# office:version and manifest:version 1.4, plus (unless base is None) a "<name>-base" control without the construct.
# "fields" fills the ${...} placeholders of the case; "base" overrides them for the control (all others become empty).
import json, os, struct, sys, zipfile, zlib
from string import Template

OUT = os.path.join(os.path.dirname(os.path.abspath(__file__)), "consumer")
NS = {"office": "urn:oasis:names:tc:opendocument:xmlns:office:1.0", "style": "urn:oasis:names:tc:opendocument:xmlns:style:1.0",
      "text": "urn:oasis:names:tc:opendocument:xmlns:text:1.0", "table": "urn:oasis:names:tc:opendocument:xmlns:table:1.0",
      "draw": "urn:oasis:names:tc:opendocument:xmlns:drawing:1.0", "fo": "urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0",
      "svg": "urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0", "xlink": "http://www.w3.org/1999/xlink",
      "chart": "urn:oasis:names:tc:opendocument:xmlns:chart:1.0", "number": "urn:oasis:names:tc:opendocument:xmlns:datastyle:1.0",
      "dc": "http://purl.org/dc/elements/1.1/", "meta": "urn:oasis:names:tc:opendocument:xmlns:meta:1.0"}
NSDECL = " ".join('xmlns:%s="%s"' % kv for kv in NS.items())
MT = "application/vnd.oasis.opendocument."
# ext: (media type, body element, wrapper around the case's "content", has page layout and master page)
KINDS = {"odt": (MT + "text", "office:text", "${content}", True),
         "ods": (MT + "spreadsheet", "office:spreadsheet", "${content}", True),
         "odg": (MT + "graphics", "office:drawing", '<draw:page draw:name="page1" draw:master-page-name="Default">${content}</draw:page>', True),
         "odc": (MT + "chart", "office:chart", "${content}", False)}
TXT = "<text:p>Plain text.</text:p>"
CELLS = '<table:table table:name="Sheet1"><table:table-column/><table:table-row>%s</table:table-row></table:table>'
RECT = 'svg:x="1cm" svg:y="1cm" svg:width="6cm" svg:height="4cm"'
GEOM = '<draw:custom-shape %s><draw:enhanced-geometry svg:viewBox="0 0 21600 21600" draw:type="rectangle" ${eg}>${handles}</draw:enhanced-geometry></draw:custom-shape>' % RECT
FRAME = '<draw:frame draw:name="F1" text:anchor-type="paragraph" svg:width="4cm"%s><draw:text-box fo:min-height="2cm"%s><text:p>Box</text:p></draw:text-box></draw:frame>'
PSTYLE = '<style:style style:name="%s" style:family="paragraph">%s</style:style>'
GSTYLE = '<style:style style:name="%s" style:family="graphic">%s</style:style>'


def png():
    """A 1x1 RGB PNG."""
    def chunk(t, d):
        c = t + d
        return struct.pack(">I", len(d)) + c + struct.pack(">I", zlib.crc32(c) & 0xffffffff)
    return (b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", 1, 1, 8, 2, 0, 0, 0))
            + chunk(b"IDAT", zlib.compress(b"\x00\xff\x00\x00")) + chunk(b"IEND", b""))


def pstyle(n, tag):
    """Paragraph style n with the given properties element."""
    return PSTYLE % (n, tag)


# name, ext, construct, content, auto (content.xml automatic styles), styles (styles.xml office:styles), fields, base
CASES = [
    dict(name="value-type-error", ext="ods", construct='office:value-type="error" with office:string-value, text:p child',
         content=CELLS % '<table:table-cell ${c}><text:p>#DIV/0!</text:p></table:table-cell>',
         fields={"c": 'office:value-type="error" office:string-value="#DIV/0!"'}, base={"c": 'office:value-type="string"'}),
    dict(name="value-type-error-nostring", ext="ods", construct='office:value-type="error" without office:string-value, text:p child',
         content=CELLS % '<table:table-cell ${c}><text:p>#N/A</text:p></table:table-cell>',
         fields={"c": 'office:value-type="error"'}, base=None),
    dict(name="extrusion-metal-type", ext="odg", construct='draw:extrusion-metal-type="draw:default" on draw:enhanced-geometry (value arbitrary: the RNG type is namespacedToken with a declared prefix)',
         content=GEOM, fields={"eg": 'draw:extrusion="true" ${m}', "m": 'draw:extrusion-metal="true" draw:extrusion-metal-type="draw:default"', "handles": ""},
         base={"eg": 'draw:extrusion="true" draw:extrusion-metal="true"', "handles": ""}),
    dict(name="extrusion-specularity", ext="odg", construct='draw:extrusion-specularity="150%" on draw:enhanced-geometry',
         content=GEOM, fields={"eg": 'draw:extrusion="true" draw:extrusion-specularity="${v}"', "v": "150%", "handles": ""},
         base={"eg": 'draw:extrusion="true" draw:extrusion-specularity="80%"', "handles": ""}),
    dict(name="writing-mode-sideways", ext="odt", construct='style:writing-mode sideways-rl / sideways-lr on paragraph styles, sideways-rl on graphic-properties',
         auto=pstyle("P1", '<style:paragraph-properties style:writing-mode="${w1}"/>') + pstyle("P2", '<style:paragraph-properties style:writing-mode="${w2}"/>')
              + GSTYLE % ("G1", '<style:graphic-properties style:writing-mode="${w3}"/>'),
         content='<text:p text:style-name="P1">One</text:p><text:p text:style-name="P2">Two</text:p><text:p>' + FRAME % (' draw:style-name="G1"', "") + '</text:p>',
         fields={"w1": "sideways-rl", "w2": "sideways-lr", "w3": "sideways-rl"}, base={"w1": "lr-tb", "w2": "rl-tb", "w3": "lr-tb"}),
    dict(name="break-even-odd-page", ext="odt", construct='fo:break-before="even-page" / "odd-page" on paragraph styles',
         auto=pstyle("P1", '<style:paragraph-properties fo:break-before="${b1}"/>') + pstyle("P2", '<style:paragraph-properties fo:break-before="${b2}"/>'),
         content='<text:p text:style-name="P1">Even</text:p><text:p text:style-name="P2">Odd</text:p>',
         fields={"b1": "even-page", "b2": "odd-page"}, base={"b1": "page", "b2": "auto"}),
    dict(name="vertical-rel-page-content", ext="odt", construct='style:vertical-rel="page-content-bottom" / "page-content-top" on graphic-properties of anchored text-box frames',
         auto=GSTYLE % ("G1", '<style:graphic-properties style:vertical-pos="bottom" style:vertical-rel="${r1}"/>')
              + GSTYLE % ("G2", '<style:graphic-properties style:vertical-pos="top" style:vertical-rel="${r2}"/>'),
         content='<text:p>' + FRAME % (' draw:style-name="G1"', "") + FRAME.replace("F1", "F2") % (' draw:style-name="G2"', "") + '</text:p>',
         fields={"r1": "page-content-bottom", "r2": "page-content-top"}, base={"r1": "page-content", "r2": "page-content"}),
    dict(name="repeat-scale", ext="odg", construct='style:repeat="scale" on a graphic style with a bitmap fill',
         auto=GSTYLE % ("gr1", '<style:graphic-properties draw:fill="bitmap" draw:fill-image-name="Img" style:repeat="${r}"/>'),
         styles='<draw:fill-image draw:name="Img" xlink:href="Pictures/p.png" xlink:type="simple" xlink:show="embed" xlink:actuate="onLoad"/>',
         content='<draw:rect draw:style-name="gr1" %s/>' % RECT, fields={"r": "scale"}, base={"r": "stretch"}, extra={"Pictures/p.png": png()}),
    dict(name="svg-rx-ry", ext="odt", construct='svg:rx / svg:ry on a draw:text-box in a draw:frame and on an office:annotation',
         content='<text:p>' + FRAME % ("", ' ${r}') + 'x<office:annotation ${r}><dc:creator>A</dc:creator><text:p>Note</text:p></office:annotation></text:p>',
         fields={"r": 'svg:rx="0.3cm" svg:ry="0.2cm"'}, base={}),
    dict(name="table-in-shape", ext="odg", construct='table:table as a child of draw:rect',
         content='<draw:rect %s>${t}</draw:rect>' % RECT,
         fields={"t": '<table:table table:name="T1"><table:table-column/><table:table-row><table:table-cell office:value-type="string"><text:p>cell</text:p></table:table-cell></table:table-row></table:table>'},
         base={"t": "<text:p>text</text:p>"}),
    dict(name="annotation-in-page", ext="odg", construct='office:annotation as a child of draw:page',
         content='<draw:rect %s><text:p>shape</text:p></draw:rect>${a}' % RECT,
         fields={"a": '<office:annotation svg:x="8cm" svg:y="1cm" svg:width="4cm" svg:height="2cm"><dc:creator>A</dc:creator><dc:date>2026-11-03T10:00:00</dc:date><text:p>Note</text:p></office:annotation>'}, base={}),
    dict(name="background-fill-paragraph", ext="odt", construct='style:background-fill (solid, #ff0000) in style:paragraph-properties',
         auto=pstyle("P1", '<style:paragraph-properties>${bf}</style:paragraph-properties>'),
         content='<text:p text:style-name="P1">Red</text:p>', fields={"bf": '<style:background-fill draw:fill="solid" draw:fill-color="#ff0000"/>'}, base={}),
    dict(name="background-fill-cell", ext="ods", construct='style:background-fill (solid, #ff0000) in style:table-cell-properties',
         auto='<style:style style:name="ce1" style:family="table-cell"><style:table-cell-properties>${bf}</style:table-cell-properties></style:style>',
         content=CELLS % '<table:table-cell table:style-name="ce1" office:value-type="string"><text:p>Red</text:p></table:table-cell>',
         fields={"bf": '<style:background-fill draw:fill="solid" draw:fill-color="#ff0000"/>'}, base={}),
    dict(name="num-list-format", ext="odt", construct='number:num-list-format / number:num-list-label in office:styles, style:num-list-format-name on text:list-level-style-number',
         styles='${nlf}<text:list-style style:name="L1"><text:list-level-style-number text:level="1" style:num-format="1" ${nm}/></text:list-style>',
         content='<text:list text:style-name="L1"><text:list-item><text:p>One</text:p></text:list-item></text:list>',
         fields={"nlf": '<number:num-list-format style:name="NLF1"><number:text>(</number:text><number:num-list-label text:level="1"/><number:text>)</number:text></number:num-list-format>',
                 "nm": 'style:num-list-format-name="NLF1"'}, base={}),
    dict(name="filter-color-condition", ext="ods", construct='table:filter-condition with table:data-type="background-color" and "text-color", table:value a colour',
         content=CELLS % '<table:table-cell office:value-type="string"><text:p>a</text:p></table:table-cell>'
                 + '<table:database-ranges><table:database-range table:name="DB1" table:target-range-address="Sheet1.A1:Sheet1.A3">'
                   '<table:filter><table:filter-and>${f}</table:filter-and></table:filter></table:database-range></table:database-ranges>',
         fields={"f": '<table:filter-condition table:field-number="0" table:operator="=" table:data-type="background-color" table:value="#ff0000"/>'
                      '<table:filter-condition table:field-number="0" table:operator="=" table:data-type="text-color" table:value="#0000ff"/>'
                      '<table:filter-condition table:field-number="0" table:operator="!=" table:value="b"/>'},
         base={"f": '<table:filter-condition table:field-number="0" table:operator="!=" table:value="b"/>'}),
    dict(name="chart-major-origin", ext="odc", construct='chart:major-origin and chart:minor-logarithmic on style:chart-properties',
         auto='<style:style style:name="ch1" style:family="chart"><style:chart-properties ${a}/></style:style>',
         content='<chart:chart chart:class="chart:bar"><chart:plot-area><chart:axis chart:dimension="y" chart:name="primary-y" chart:style-name="ch1"/></chart:plot-area></chart:chart>',
         fields={"a": 'chart:major-origin="0" chart:minor-logarithmic="true"'}, base={}),
    dict(name="allow-overlap-decorative", ext="odt", construct='draw:allow-overlap and draw:decorative on style:graphic-properties',
         auto=GSTYLE % ("G1", '<style:graphic-properties ${a}/>'), content='<text:p>' + FRAME % (' draw:style-name="G1"', "") + '</text:p>',
         fields={"a": 'draw:allow-overlap="true" draw:decorative="true"'}, base={}),
    dict(name="margin-gutter", ext="odt", construct='style:margin-gutter on style:page-layout-properties',
         content=TXT, fields={"pl": 'fo:margin-left="2cm" style:margin-gutter="1cm"'}, base={"pl": 'fo:margin-left="2cm"'}),
    dict(name="relative-tab-stop-position", ext="odt", construct='text:relative-tab-stop-position on style:paragraph-properties',
         auto=pstyle("P1", '<style:paragraph-properties ${a}><style:tab-stops><style:tab-stop style:position="5cm"/></style:tab-stops></style:paragraph-properties>'),
         content='<text:p text:style-name="P1">A<text:tab/>B</text:p>', fields={"a": 'text:relative-tab-stop-position="false"'}, base={}),
    dict(name="start-value-zero", ext="odt", construct='text:start-value="0" on text:list-level-style-number',
         auto='<text:list-style style:name="L1"><text:list-level-style-number text:level="1" style:num-format="1" text:start-value="${v}"/></text:list-style>',
         content='<text:list text:style-name="L1"><text:list-item><text:p>Zero</text:p></text:list-item><text:list-item><text:p>One</text:p></text:list-item></text:list>',
         fields={"v": "0"}, base={"v": "1"}),
    dict(name="handle-new-attributes", ext="odg", construct='draw:handle with only draw:handle-position-x/-y, and a polar one with -polar-pole-x/-y, -polar-radius/-angle',
         content=GEOM, fields={"eg": "", "handles": '<draw:handle draw:handle-position-x="10800" draw:handle-position-y="0"/>'
                               '<draw:handle draw:handle-polar-pole-x="10800" draw:handle-polar-pole-y="10800" draw:handle-polar-radius="5000" draw:handle-polar-angle="0"/>'},
         base={"handles": ""}),
]


def build(case, base):
    mime, body, wrap, paged = KINDS[case["ext"]]
    f = {k: "" for k in case["fields"]}
    f.update(case["base"] if base else case["fields"])
    f = {k: Template(v).substitute(f) for k, v in f.items()}  # fields may refer to other fields, one level
    sub = lambda s: Template(s).substitute(f)
    content = sub(Template(wrap).substitute(content=case["content"]))
    head = '<?xml version="1.0" encoding="UTF-8"?>\n<%s ' + NSDECL + ' office:version="1.4">%s</%s>\n'
    cbody = '<office:automatic-styles>%s</office:automatic-styles><office:body><%s>%s</%s></office:body>' % (sub(case.get("auto", "")), body, content, body)
    sbody = '<office:styles>%s</office:styles>' % sub(case.get("styles", ""))
    if paged:
        sbody += ('<office:automatic-styles><style:page-layout style:name="PM1"><style:page-layout-properties %s/></style:page-layout></office:automatic-styles>'
                  '<office:master-styles><style:master-page style:name="Default" style:page-layout-name="PM1"/></office:master-styles>') % f.get("pl", "")
    streams = {"content.xml": head % ("office:document-content", cbody, "office:document-content"),
               "styles.xml": head % ("office:document-styles", sbody, "office:document-styles")}
    entries = "".join('<manifest:file-entry manifest:full-path="%s" manifest:media-type="%s"/>' % (n, "image/png" if n.endswith(".png") else "text/xml")
                      for n in list(streams) + list(case.get("extra", {})))
    mani = ('<?xml version="1.0" encoding="UTF-8"?>\n<manifest:manifest xmlns:manifest="urn:oasis:names:tc:opendocument:xmlns:manifest:1.0" manifest:version="1.4">'
            '<manifest:file-entry manifest:full-path="/" manifest:version="1.4" manifest:media-type="%s"/>%s</manifest:manifest>\n') % (mime, entries)
    path = os.path.join(OUT, case["name"] + ("-base" if base else "") + "." + case["ext"])
    with zipfile.ZipFile(path, "w") as z:
        def put(n, data, comp):
            z.writestr(zipfile.ZipInfo(n, (2026, 11, 3, 0, 0, 0)), data, compress_type=comp)
        put("mimetype", mime, zipfile.ZIP_STORED)
        for n, t in streams.items():
            put(n, t, zipfile.ZIP_DEFLATED)
        for n, d in case.get("extra", {}).items():
            put(n, d, zipfile.ZIP_STORED)
        put("META-INF/manifest.xml", mani, zipfile.ZIP_DEFLATED)
    return path


def report(valjson):
    val = json.load(open(valjson))
    count = {v: {} for v in ("13", "14")}
    for v in count:
        for e in val["errors"][v]:
            # validate.py only rewrites a 1.2/1.3 version to the target, so every 1.4 file draws its three version errors under 1.3
            if v == "13" and e[3].startswith(('value of attribute "office:version"', 'value of attribute "manifest:version"')):
                continue
            count[v][e[0].split("/")[0]] = count[v].get(e[0].split("/")[0], 0) + 1
    rows = []
    for c in CASES:
        for base in (False, True):
            if base and c["base"] is None:
                continue
            n = c["name"] + ("-base" if base else "") + "." + c["ext"]
            if n in val["packages"]:
                rows.append((n, "(control without the construct)" if base else c["construct"], count["14"].get(n, 0), count["13"].get(n, 0)))
            else:
                rows.append((n, "NOT VALIDATED", -1, -1))
    w = max(len(r[0]) for r in rows)
    print("%-*s  %4s  %4s  construct  (1.3 count leaves out the office:version/manifest:version errors)" % (w, "package", "1.4", "1.3"))
    for n, k, e14, e13 in rows:
        print("%-*s  %4d  %4d  %s" % (w, n, e14, e13, k))


if len(sys.argv) == 3 and sys.argv[1] == "--report":
    report(sys.argv[2])
else:
    os.makedirs(OUT, exist_ok=True)
    for c in CASES:
        print(build(c, False))
        if c["base"] is not None:
            print(build(c, True))
