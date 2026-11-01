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

# usage: make-pkg-controls.py -- (re)generate pkgcheck.py's negative-control packages in fixtures/control-pkg/
# .expected.json maps each package to the (level, rule) pairs pkgcheck.py must report, exactly.
import json, os, shutil, warnings, zipfile

C = os.path.join(os.path.dirname(os.path.abspath(__file__)), "fixtures", "control-pkg")
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
    with zipfile.ZipFile(os.path.join(C, name + ".odt"), "w") as z, warnings.catch_warnings():
        warnings.simplefilter("ignore")
        for f in files:
            zi = zipfile.ZipInfo(f[0])
            if len(f) > 3:
                zi.extra = f[3]
            z.writestr(zi, f[1], compress_type=f[2])
    EXPECTED[name + ".odt"] = sorted(expect)


def std(mani=GOOD_MANI, mime=MIME, extra=()):
    return [("mimetype", mime, S), ("content.xml", "<c/>", Z)] + list(extra) + [("META-INF/manifest.xml", mani, Z)]


shutil.rmtree(C, ignore_errors=True)
os.makedirs(C)
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
json.dump(EXPECTED, open(os.path.join(C, ".expected.json"), "w"), indent=1)
