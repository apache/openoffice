################################################################
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
################################################################

"""Negative-control documents for the checkers: ports of make-controls.py, make-pkg-controls.py and make-doctype-controls.py."""
import io, json, os, zipfile, warnings


def _prepare(path):
    """A control directory must be empty or absent: nothing is ever deleted here."""
    if path is None:
        return
    if os.path.isdir(path) and os.listdir(path):
        raise ValueError("control directory is not empty: " + path)
    os.makedirs(path, exist_ok=True)


def _target(path, name):
    """A file to write, or a throwaway buffer when only the expectations are wanted."""
    return io.BytesIO() if path is None else os.path.join(path, name)


def _write(path, name, text):
    if path is not None:
        open(os.path.join(path, name), "w").write(text)


def make_odf_controls(outdir):
    """Writes the negative-control packages for validate.check() and idcheck.check() into `outdir` (empty or absent)."""
    C = os.fspath(outdir)
    MANI = ('<?xml version="1.0" encoding="UTF-8"?>\n<manifest:manifest xmlns:manifest="urn:oasis:names:tc:opendocument:xmlns:manifest:1.0" manifest:version="1.2">'
            '<manifest:file-entry manifest:full-path="/" manifest:version="1.2" manifest:media-type="application/vnd.oasis.opendocument.text"/>'
            '<manifest:file-entry manifest:full-path="content.xml" manifest:media-type="text/xml"/></manifest:manifest>\n')
    MATH = ('<?xml version="1.0" encoding="UTF-8"?>\n<!DOCTYPE math:math PUBLIC "-//OpenOffice.org//DTD Modified W3C MathML 1.01//EN" "math.dtd">\n'
            '<math:math xmlns:math="http://www.w3.org/1998/Math/MathML"><math:semantics><math:mi>x</math:mi></math:semantics></math:math>\n')


    def mk(name, ver, body, extra=None):
        content = ('<?xml version="1.0" encoding="UTF-8"?>\n<office:document-content xmlns:office="urn:oasis:names:tc:opendocument:xmlns:office:1.0" '
                   'xmlns:text="urn:oasis:names:tc:opendocument:xmlns:text:1.0" office:version="%s"><office:body><office:text>%s</office:text></office:body></office:document-content>\n') % (ver, body)
        with zipfile.ZipFile(_target(C, name + ".odt"), "w") as z:
            z.writestr(zipfile.ZipInfo("mimetype"), "application/vnd.oasis.opendocument.text", compress_type=zipfile.ZIP_STORED)
            z.writestr("content.xml", content, compress_type=zipfile.ZIP_DEFLATED)
            z.writestr("META-INF/manifest.xml", MANI, compress_type=zipfile.ZIP_DEFLATED)
            for n, t in (extra or {}).items():
                z.writestr(n, t, compress_type=zipfile.ZIP_DEFLATED)


    OFFNS = ('xmlns:office="urn:oasis:names:tc:opendocument:xmlns:office:1.0" xmlns:text="urn:oasis:names:tc:opendocument:xmlns:text:1.0" '
             'xmlns:style="urn:oasis:names:tc:opendocument:xmlns:style:1.0" xmlns:chart="urn:oasis:names:tc:opendocument:xmlns:chart:1.0" '
             'xmlns:svg="urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0" xmlns:dc="http://purl.org/dc/elements/1.1/" '
             'xmlns:meta="urn:oasis:names:tc:opendocument:xmlns:meta:1.0" xmlns:draw="urn:oasis:names:tc:opendocument:xmlns:drawing:1.0"')


    def mkdoc(name, ext, mime, streams):
        """A package whose streams are given whole: {"content.xml": (root element, body)}, written with office:version 1.2."""
        mani = MANI.replace("application/vnd.oasis.opendocument.text", mime)
        with zipfile.ZipFile(_target(C, name + "." + ext), "w") as z:
            z.writestr(zipfile.ZipInfo("mimetype"), mime, compress_type=zipfile.ZIP_STORED)
            for n, (root, body) in streams.items():
                z.writestr(n, '<?xml version="1.0" encoding="UTF-8"?>\n<%s %s office:version="1.2">%s</%s>\n' % (root, OFFNS, body, root), compress_type=zipfile.ZIP_DEFLATED)
            z.writestr("META-INF/manifest.xml", mani, compress_type=zipfile.ZIP_DEFLATED)


    _prepare(C)
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
    # 1.4 (OFFICE-4142): draw:decorative on style:graphic-properties is new, invalid in 1.2 and 1.3; basedeco is the same style without it
    for n, attrs in (("basedeco", ""), ("newdeco", ' draw:decorative="true"')):
        mkdoc(n, "odt", TEXT, {"content.xml": ("office:document-content", "<office:body><office:text><text:p>hi</text:p></office:text></office:body>"),
                              "styles.xml": ("office:document-styles", '<office:automatic-styles><style:style style:name="gr1" style:family="graphic"><style:graphic-properties%s/></style:style></office:automatic-styles>' % attrs)})
    # 1.4 (OFFICE-4110): draw:handle needs the new position-x/-y or polar-pole pair; the old combined attribute alone is valid in 1.2 and 1.3 only.
    # The pair itself is invalid in 1.2 and 1.3.
    for n, attrs in (("handleold", 'draw:handle-position="0 0"'), ("handlenew", 'draw:handle-position-x="0" draw:handle-position-y="0"'),
                     ("handlepolar", 'draw:handle-polar-pole-x="0" draw:handle-polar-pole-y="0"')):
        mkdoc(n, "odt", TEXT, {"content.xml": ("office:document-content", '<office:body><office:text><text:p><draw:custom-shape><draw:enhanced-geometry><draw:handle %s/></draw:enhanced-geometry></draw:custom-shape></text:p></office:text></office:body>' % attrs)})


def make_pkg_controls(outdir):
    """Writes pkgcheck's negative-control packages and .expected.json into `outdir` (empty or absent), or only builds the table when
    `outdir` is None. Returns {package: sorted [(level, rule)]}: the findings pkgcheck.check() must report, exactly."""
    C = None if outdir is None else os.fspath(outdir)
    MIME = "application/vnd.oasis.opendocument.text"
    S, Z = zipfile.ZIP_STORED, zipfile.ZIP_DEFLATED
    SIG = ('<?xml version="1.0" encoding="UTF-8"?>\n<dsig:document-signatures xmlns:dsig="urn:oasis:names:tc:opendocument:xmlns:digitalsignature:1.0" '
           'dsig:version="%s" xmlns:ds="http://www.w3.org/2000/09/xmldsig#"><ds:Signature Id="s1"/></dsig:document-signatures>\n')
    ENC = ('<manifest:encryption-data manifest:checksum-type="SHA1/1K" manifest:checksum="AAAA">'
           '<manifest:algorithm manifest:algorithm-name="Blowfish CFB" manifest:initialisation-vector="AAAA"/>'
           '<manifest:key-derivation manifest:key-derivation-name="PBKDF2" manifest:key-size="16" manifest:iteration-count="1024" manifest:salt="AAAA"/>'
           '</manifest:encryption-data>')


    def entry(path, mt="text/xml", inner="", **attrs):
        a = "".join(' manifest:%s="%s"' % kv for kv in attrs.items())
        return '<manifest:file-entry manifest:full-path="%s" manifest:media-type="%s"%s>%s</manifest:file-entry>' % (path, mt, a, inner)


    def manifest(*entries, root="manifest:manifest"):
        return ('<?xml version="1.0" encoding="UTF-8"?>\n<%s xmlns:manifest="urn:oasis:names:tc:opendocument:xmlns:manifest:1.0" manifest:version="1.2">%s</%s>\n'
                % (root, "".join(entries), root))


    ROOT = entry("/", MIME, version="1.2")
    CONTENT = entry("content.xml")
    GOOD_MANI = manifest(ROOT, CONTENT)
    EXPECTED = {}


    def build(name, files, expect=()):
        """files: (zip name, data, method[, extra]) in physical order."""
        with zipfile.ZipFile(_target(C, name + ".odt"), "w") as z, warnings.catch_warnings():
            warnings.simplefilter("ignore")
            for f in files:
                zi = zipfile.ZipInfo(f[0])
                if len(f) > 3:
                    zi.extra = f[3]
                z.writestr(zi, f[1], compress_type=f[2])
        EXPECTED[name + ".odt"] = sorted(expect)


    def std(mani=GOOD_MANI, mime=MIME, extra=()):
        return [("mimetype", mime, S), ("content.xml", "<c/>", Z)] + list(extra) + [("META-INF/manifest.xml", mani, Z)]


    _prepare(C)
    build("good", std())
    sub = [("Object 1/content.xml", "<c/>", Z), ("Pictures/a.png", "x", S), ("secret.xml", "0123456789", S),
           ("META-INF/documentsignatures.xml", SIG % "1.2", Z)]
    build("good-full", std(manifest(ROOT, CONTENT, entry("Object 1/", MIME, version="1.2"), entry("Object 1/content.xml"),
                                    entry("Pictures/a.png", "image/png"), entry("secret.xml", inner=ENC, size="10")), extra=sub))
    build("mime-deflated", [("mimetype", MIME, Z), ("content.xml", "<c/>", Z), ("META-INF/manifest.xml", GOOD_MANI, Z)], [("shall", "mimetype-compressed")])
    build("mime-extra", [("mimetype", MIME, S, b"\xfe\xca\x00\x00"), ("content.xml", "<c/>", Z), ("META-INF/manifest.xml", GOOD_MANI, Z)], [("shall", "mimetype-extra")])
    build("mime-notfirst", [("content.xml", "<c/>", Z), ("mimetype", MIME, S), ("META-INF/manifest.xml", GOOD_MANI, Z)], [("shall", "mimetype-first")])
    build("mime-mismatch", std(mime="application/x-bogus"), [("shall", "mimetype-mismatch")])
    build("mime-nonascii", std(mime=MIME + "é"), [("shall", "mimetype-ascii")])
    build("mime-missing", std()[1:], [("should", "mimetype-missing"), ("shall", "mimetype-missing")])
    build("mime-missing-noroot", std(manifest(CONTENT))[1:], [("should", "mimetype-missing")])
    build("no-root-entry", std(manifest(CONTENT)), [("shall", "manifest-root-entry")])
    build("uncovered", std(extra=[("extra.xml", "<e/>", Z)]), [("shall", "manifest-coverage")])
    build("dup-entry", std(manifest(ROOT, CONTENT, CONTENT)), [("shall", "manifest-duplicate")])
    build("self-entry", std(manifest(ROOT, CONTENT, entry("mimetype", "text/plain"))), [("shall", "manifest-self")])
    build("dangling", std(manifest(ROOT, CONTENT, entry("gone.xml"))), [("should", "manifest-dangling")])
    build("dir-entry-manifest", std(manifest(ROOT, CONTENT, entry("Configurations2/", "application/vnd.sun.xml.ui.configuration"))), [("should", "manifest-dir-entry")])
    build("stray-meta-inf", std(extra=[("META-INF/foo.xml", "<f/>", Z)]), [("shall", "meta-inf-file")])
    build("no-manifest", [("mimetype", MIME, S), ("content.xml", "<c/>", Z)], [("shall", "manifest-missing")])
    build("manifest-malformed", std("<manifest:manifest"), [("shall", "manifest-malformed")])
    build("manifest-wrongroot", std(manifest(root="manifest:other")), [("shall", "manifest-root")])
    build("zip-bzip2", std(extra=[("b.xml", "<b/>", zipfile.ZIP_BZIP2)], mani=manifest(ROOT, CONTENT, entry("b.xml"))), [("shall", "zip-method")])
    build("zip-dup-name", std(extra=[("content.xml", "<d/>", Z)]), [("shall", "zip-duplicate")])
    build("zip-dir-entry", std(extra=[("Pictures/", "", S)]), [("should", "zip-dir-entry")])
    build("enc-nosize", std(manifest(ROOT, CONTENT, entry("secret.xml", inner=ENC)), extra=[("secret.xml", "0123456789", S)]), [("shall", "encrypted-size")])
    build("enc-deflated", std(manifest(ROOT, CONTENT, entry("secret.xml", inner=ENC, size="10")), extra=[("secret.xml", "0123456789", Z)]), [("shall", "encrypted-method")])
    build("sig-malformed", std(extra=[("META-INF/documentsignatures.xml", "<dsig", Z)]), [("shall", "signature-malformed")])
    build("sig-wrongroot", std(extra=[("META-INF/documentsignatures.xml", "<a/>", Z)]), [("shall", "signature-root")])
    build("sig-badversion", std(extra=[("META-INF/documentsignatures.xml", SIG % "1.1", Z)]), [("shall", "signature-schema")])
    if C:
        json.dump(EXPECTED, open(os.path.join(C, ".expected.json"), "w"), indent=1)
    return EXPECTED


def make_doctype_controls(outdir):
    """Writes doctypecheck's negative-control documents and .expected.json into `outdir` (empty or absent), or only builds the table
    when `outdir` is None. Returns {document: sorted [(level, rule)]}."""
    C = None if outdir is None else os.fspath(outdir)
    P = "application/vnd.oasis.opendocument."
    NS = ('xmlns:office="urn:oasis:names:tc:opendocument:xmlns:office:1.0" xmlns:math="http://www.w3.org/1998/Math/MathML"')
    S, Z = zipfile.ZIP_STORED, zipfile.ZIP_DEFLATED
    EXPECTED = {}


    def content(child="text", root="office:document-content", extra=""):
        body = "<office:body>%s</office:body>" % ("<office:%s/>" % child if child else "")
        return '<?xml version="1.0" encoding="UTF-8"?>\n<%s %s office:version="1.2"%s>%s</%s>\n' % (root, NS, extra, body, root)


    MATHDOC = '<?xml version="1.0" encoding="UTF-8"?>\n<math:math %s><math:mi>x</math:mi></math:math>\n' % NS


    def pkg(name, mime, body, expect=(), files=None, manifest=""):
        """files: replaces the default content.xml; manifest: extra file-entry elements."""
        with zipfile.ZipFile(_target(C, name + ".odt"), "w") as z:
            if mime is not None:
                z.writestr(zipfile.ZipInfo("mimetype"), mime, compress_type=S)
            for n, d in (files if files is not None else {"content.xml": body}).items():
                z.writestr(n, d, compress_type=Z)
            z.writestr("META-INF/manifest.xml", '<?xml version="1.0" encoding="UTF-8"?>\n<manifest:manifest xmlns:manifest='
                       '"urn:oasis:names:tc:opendocument:xmlns:manifest:1.0" manifest:version="1.2">%s</manifest:manifest>\n' % manifest, compress_type=Z)
        EXPECTED[name + ".odt"] = sorted(expect)


    def flat(name, mime, child="text", expect=()):
        attr = "" if mime is None else ' office:mimetype="%s"' % mime
        _write(C, name + ".fodt", content(child, "office:document", attr))
        EXPECTED[name + ".fodt"] = sorted(expect)


    _prepare(C)
    for t, ms, child in (("text", ["text", "text-template", "text-master"], "text"), ("spreadsheet", ["spreadsheet", "spreadsheet-template"], "spreadsheet"),
                         ("drawing", ["graphics", "graphics-template"], "drawing"), ("presentation", ["presentation", "presentation-template"], "presentation"),
                         ("chart", ["chart", "chart-template"], "chart"), ("image", ["image", "image-template"], "image"), ("database", ["base"], "database")):
        for m in ms:
            pkg("good-" + m, P + m, content(child))
    pkg("good-formula", P + "formula", MATHDOC)
    sub = lambda mt, c: {"content.xml": content(), "Object 1/content.xml": c}
    ent = lambda mt: '<manifest:file-entry manifest:full-path="Object 1/" manifest:media-type="%s"/>' % mt
    pkg("good-embedded", P + "text", None, files=sub(P + "chart", content("chart")), manifest=ent(P + "chart"))
    pkg("good-embedded-math", P + "text", None, files=sub("", MATHDOC), manifest=ent(P + "formula"))
    flat("good-flat", P + "text")
    for m, ru in (("application/x-bogus", "doctype-unknown-mimetype"), (P + "text\n", "doctype-unknown-mimetype"),
                  ("application.vnd.oasis.opendocument.graphics", "doctype-unknown-mimetype"),
                  (P + "text-web", "doctype-unlisted-mimetype"), (P + "formula-template", "doctype-unlisted-mimetype")):
        pkg("bad-mime-" + str(len(EXPECTED)), m, content(), [("shall", ru)])
    pkg("no-mimetype", None, content(), [("shall", "doctype-no-mimetype")])
    pkg("body-mismatch", P + "text", content("spreadsheet"), [("shall", "doctype-body")])
    pkg("body-empty", P + "text", content(None), [("shall", "doctype-body")])
    pkg("root-styles", P + "text", content(root="office:document-styles"), [("shall", "doctype-root")])
    pkg("formula-wrong-root", P + "formula", content(), [("shall", "doctype-root")])
    pkg("math-in-text", P + "text", MATHDOC, [("shall", "doctype-root")])
    pkg("only-styles", P + "text", None, [("shall", "doctype-no-body")], files={"styles.xml": content(None, "office:document-styles")})
    pkg("only-meta", P + "text", None, [("shall", "doctype-no-content"), ("shall", "doctype-no-body")], files={"meta.xml": "<a/>"})
    pkg("malformed", P + "text", "<office:document-content", [("shall", "doctype-malformed")])
    pkg("sub-body", P + "text", None, [("should", "sub-doctype-body")], files=sub(P + "chart", content("spreadsheet")), manifest=ent(P + "chart"))
    pkg("sub-root", P + "text", None, [("should", "sub-doctype-root")], files=sub(P + "formula", content()), manifest=ent(P + "formula"))
    flat("flat-no-mimetype", None, expect=[("shall", "doctype-no-mimetype")])
    flat("flat-unknown", "application/x-bogus", expect=[("shall", "doctype-unknown-mimetype")])
    flat("flat-formula-mime", P + "formula", expect=[("shall", "doctype-unknown-mimetype")])
    flat("flat-body", P + "text", "presentation", [("shall", "doctype-body")])
    if C:
        json.dump(EXPECTED, open(os.path.join(C, ".expected.json"), "w"), indent=1)
    return EXPECTED
