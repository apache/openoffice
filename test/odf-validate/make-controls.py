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

# usage: make-controls.py -- (re)generate the validator's negative-control packages in fixtures/control/
import os, zipfile

C = os.path.join(os.path.dirname(os.path.abspath(__file__)), "fixtures", "control")
MANI = ('<?xml version="1.0" encoding="UTF-8"?>\n<manifest:manifest xmlns:manifest="urn:oasis:names:tc:opendocument:xmlns:manifest:1.0" manifest:version="1.2">'
        '<manifest:file-entry manifest:full-path="/" manifest:version="1.2" manifest:media-type="application/vnd.oasis.opendocument.text"/>'
        '<manifest:file-entry manifest:full-path="content.xml" manifest:media-type="text/xml"/></manifest:manifest>\n')
MATH = ('<?xml version="1.0" encoding="UTF-8"?>\n<!DOCTYPE math:math PUBLIC "-//OpenOffice.org//DTD Modified W3C MathML 1.01//EN" "math.dtd">\n'
        '<math:math xmlns:math="http://www.w3.org/1998/Math/MathML"><math:semantics><math:mi>x</math:mi></math:semantics></math:math>\n')


def mk(name, ver, body, extra=None):
    content = ('<?xml version="1.0" encoding="UTF-8"?>\n<office:document-content xmlns:office="urn:oasis:names:tc:opendocument:xmlns:office:1.0" '
               'xmlns:text="urn:oasis:names:tc:opendocument:xmlns:text:1.0" office:version="%s"><office:body><office:text>%s</office:text></office:body></office:document-content>\n') % (ver, body)
    with zipfile.ZipFile(os.path.join(C, name + ".odt"), "w") as z:
        z.writestr(zipfile.ZipInfo("mimetype"), "application/vnd.oasis.opendocument.text", compress_type=zipfile.ZIP_STORED)
        z.writestr("content.xml", content, compress_type=zipfile.ZIP_DEFLATED)
        z.writestr("META-INF/manifest.xml", MANI, compress_type=zipfile.ZIP_DEFLATED)
        for n, t in (extra or {}).items():
            z.writestr(n, t, compress_type=zipfile.ZIP_DEFLATED)


OFFNS = ('xmlns:office="urn:oasis:names:tc:opendocument:xmlns:office:1.0" xmlns:text="urn:oasis:names:tc:opendocument:xmlns:text:1.0" '
         'xmlns:style="urn:oasis:names:tc:opendocument:xmlns:style:1.0" xmlns:chart="urn:oasis:names:tc:opendocument:xmlns:chart:1.0" '
         'xmlns:svg="urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0" xmlns:dc="http://purl.org/dc/elements/1.1/" '
         'xmlns:meta="urn:oasis:names:tc:opendocument:xmlns:meta:1.0"')


def mkdoc(name, ext, mime, streams):
    """A package whose streams are given whole: {"content.xml": (root element, body)}, written with office:version 1.2."""
    mani = MANI.replace("application/vnd.oasis.opendocument.text", mime)
    with zipfile.ZipFile(os.path.join(C, name + "." + ext), "w") as z:
        z.writestr(zipfile.ZipInfo("mimetype"), mime, compress_type=zipfile.ZIP_STORED)
        for n, (root, body) in streams.items():
            z.writestr(n, '<?xml version="1.0" encoding="UTF-8"?>\n<%s %s office:version="1.2">%s</%s>\n' % (root, OFFNS, body, root), compress_type=zipfile.ZIP_DEFLATED)
        z.writestr("META-INF/manifest.xml", mani, compress_type=zipfile.ZIP_DEFLATED)


os.makedirs(C, exist_ok=True)
mk("good", "1.2", "<text:p>hi</text:p>")
mk("bad", "1.2", "<text:bogus/>")
mk("wrongver", "1.1", "<text:p>hi</text:p>")
# text:drop-down is new in ODF 1.3 (OFFICE-3881): invalid in 1.2, valid in 1.3 and 1.4
mk("new13", "1.2", '<text:p><text:drop-down text:name="d"><text:label text:value="a"/></text:drop-down></text:p>')
# ODF 1.2 Part 1 2.2.1 B.4: an embedded object whose content.xml is MathML
mk("mathobj", "1.2", "<text:p>hi</text:p>", {"Object 1/content.xml": MATH})
# ODF 1.2 Part 1 2.2.1 D.1/D.2: checked by idcheck.py, not by the schema
mk("goodid", "1.2", '<text:p xml:id="a1">x</text:p><text:p xml:id=" b-2.c ">y</text:p>')
mk("dupid", "1.2", '<text:p xml:id="a1">x</text:p><text:p xml:id=" a1 ">y</text:p>')
mk("badid", "1.2", '<text:p xml:id="1abc">x</text:p>')
mk("badid2", "1.2", '<text:p xml:id="a b">x</text:p>')
mk("badns", "1.2", "<text:p><zz:span/></text:p>")
# Part 3 2.2.1 F: signature files are held to the same rules (validate.py does not look at them)
mk("badsig", "1.2", "<text:p>hi</text:p>", {"META-INF/documentsignatures.xml": '<a><b xml:id="x"/><b xml:id="x"/></a>'})
# Part 1 2.2.2 B.1: valid as an extended document only (foreign attribute, foreign element in a paragraph, foreign element elsewhere)
mk("extforeign", "1.2", '<text:p xmlns:foo="urn:foo" foo:x="1">a<foo:y>b</foo:y>c</text:p><foo:w xmlns:foo="urn:foo">z<text:p>q</text:p></foo:w>')
TEXT = "application/vnd.oasis.opendocument.text"
# meta:creator-initials in office:annotation is new in ODF 1.3; baseinit is the same note without it
ANN = '<office:body><office:text><text:p><office:annotation><dc:creator>A</dc:creator>%s<text:p>n</text:p></office:annotation>x</text:p></office:text></office:body>'
for n, ini in (("baseinit", ""), ("newinit", "<meta:creator-initials>AB</meta:creator-initials>")):
    mkdoc(n, "odt", TEXT, {"content.xml": ("office:document-content", ANN % ini)})
# chart:coordinate-region (inner plot position) is new in 1.3; baseregion is the same chart without it
CHART = "application/vnd.oasis.opendocument.chart"
for n, region in (("baseregion", ""), ("newregion", '<chart:coordinate-region svg:x="1cm" svg:y="1cm" svg:width="5cm" svg:height="4cm"/>')):
    mkdoc(n, "odc", CHART, {"content.xml": ("office:document-content", '<office:body><office:chart><chart:chart chart:class="chart:bar"><chart:plot-area>%s</chart:plot-area></chart:chart></office:chart></office:body>' % region)})
# 1.3 makes style:scale-to, style:scale-to-pages and the pair scale-to-X/scale-to-Y mutually exclusive (OFFICE-3857); 1.2 has no scale-to-X/Y
for n, attrs in (("scaleone", 'style:scale-to="80%"'), ("scalexy", 'style:scale-to-X="2" style:scale-to-Y="3"'), ("scaleboth", 'style:scale-to-X="2" style:scale-to-Y="3" style:scale-to="80%"')):
    mkdoc(n, "odt", TEXT, {"content.xml": ("office:document-content", "<office:body><office:text><text:p>hi</text:p></office:text></office:body>"),
                          "styles.xml": ("office:document-styles", '<office:automatic-styles><style:page-layout style:name="pm1"><style:page-layout-properties %s/></style:page-layout></office:automatic-styles>' % attrs)})
