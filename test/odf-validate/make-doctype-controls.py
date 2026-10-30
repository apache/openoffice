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

# usage: make-doctype-controls.py -- (re)generate doctypecheck.py's negative-control documents in fixtures/control-doctype/
# .expected.json maps each document to the (level, rule) pairs doctypecheck.py must report, exactly.
import json, os, shutil, zipfile

C = os.path.join(os.path.dirname(os.path.abspath(__file__)), "fixtures", "control-doctype")
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
    with zipfile.ZipFile(os.path.join(C, name + ".odt"), "w") as z:
        if mime is not None:
            z.writestr(zipfile.ZipInfo("mimetype"), mime, compress_type=S)
        for n, d in (files if files is not None else {"content.xml": body}).items():
            z.writestr(n, d, compress_type=Z)
        z.writestr("META-INF/manifest.xml", '<?xml version="1.0" encoding="UTF-8"?>\n<manifest:manifest xmlns:manifest='
                   '"urn:oasis:names:tc:opendocument:xmlns:manifest:1.0" manifest:version="1.2">%s</manifest:manifest>\n' % manifest, compress_type=Z)
    EXPECTED[name + ".odt"] = sorted(expect)


def flat(name, mime, child="text", expect=()):
    attr = "" if mime is None else ' office:mimetype="%s"' % mime
    open(os.path.join(C, name + ".fodt"), "w").write(content(child, "office:document", attr))
    EXPECTED[name + ".fodt"] = sorted(expect)


shutil.rmtree(C, ignore_errors=True)
os.makedirs(C)
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
json.dump(EXPECTED, open(os.path.join(C, ".expected.json"), "w"), indent=1)
