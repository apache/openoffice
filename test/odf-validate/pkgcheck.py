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

# usage: pkgcheck.py <outdir> <pkg.json>
# ODF 1.2 Part 3 2.2.1, 3.2, 3.3 and 3.4.1 on every package in <outdir>. Each finding is
# "shall: [rule] ..." or "should: [rule] ...". Schema validity of the manifest is validate.py's job.
import sys, os, re, zipfile, json, struct, subprocess, shutil, warnings
from xml.etree import ElementTree as ET

D = os.path.dirname(os.path.abspath(__file__))
JING = ["java", "-jar", os.environ.get("ODFV_JING", "/Applications/XMLauthor/lib/jing.jar"), "-i"]
DSIG_SCHEMA = D + "/schema/OpenDocument-v1.2-os-dsig-schema.rng"
MANI = "{urn:oasis:names:tc:opendocument:xmlns:manifest:1.0}"
DSIG = "{urn:oasis:names:tc:opendocument:xmlns:digitalsignature:1.0}"
ERR = re.compile(r"^(.*?):(\d+):(\d+): (error|fatal|warning): (.*)$")


def check_package(path, work):
    """Returns [(entry, level, rule, message)] for one package."""
    out = []

    def add(entry, level, rule, msg):
        out.append((entry, level, rule, msg))
    z = zipfile.ZipFile(path)
    infos = z.infolist()
    names = [i.filename for i in infos]
    for n in sorted({n for n in names if names.count(n) > 1}):
        add(n, "shall", "zip-duplicate", "zip entry name occurs more than once")
    for i in infos:
        if i.compress_type not in (zipfile.ZIP_STORED, zipfile.ZIP_DEFLATED):
            add(i.filename, "shall", "zip-method", "compression method %d is neither STORED nor DEFLATED (2.2.1 A)" % i.compress_type)
        if i.filename.endswith("/"):
            add(i.filename, "should", "zip-dir-entry", "directories have no zip entries (4.3)")
    files = {n for n in names if not n.endswith("/")}

    # mimetype (3.3)
    mime = None
    if "mimetype" not in files:
        add("mimetype", "should", "mimetype-missing", "no mimetype file (2.2.1 C)")
    else:
        mi = z.getinfo("mimetype")
        first = min(infos, key=lambda i: i.header_offset)
        if first.filename != "mimetype" or mi.header_offset != 0:
            add("mimetype", "shall", "mimetype-first", "mimetype is not the first file of the zip (3.3)")
        with open(path, "rb") as f:
            f.seek(mi.header_offset)
            hdr = f.read(30)
        if len(hdr) == 30 and hdr[:4] == b"PK\x03\x04":
            method, extralen = struct.unpack("<H", hdr[8:10])[0], struct.unpack("<H", hdr[28:30])[0]
            if method != zipfile.ZIP_STORED:
                add("mimetype", "shall", "mimetype-compressed", "mimetype is compressed, local header method %d (3.3)" % method)
            if extralen:
                add("mimetype", "shall", "mimetype-extra", "mimetype has a %d byte extra field in its local header (3.3)" % extralen)
        else:
            add("mimetype", "shall", "mimetype-first", "no local file header at the recorded offset")
        if mi.compress_type != zipfile.ZIP_STORED:
            add("mimetype", "shall", "mimetype-compressed", "mimetype is compressed, central directory method %d (3.3)" % mi.compress_type)
        raw = z.read("mimetype")
        try:
            mime = raw.decode("ascii")
        except UnicodeDecodeError:
            add("mimetype", "shall", "mimetype-ascii", "mimetype content is not ASCII (3.3)")

    # META-INF contents (2.2.1 D, E)
    for n in sorted(files):
        if n.startswith("META-INF/") and n != "META-INF/manifest.xml" and "signatures" not in n.rsplit("/", 1)[-1]:
            add(n, "shall", "meta-inf-file", "file other than manifest.xml and signature files in META-INF (2.2.1 E)")

    # manifest (2.2.1 B, 3.2)
    entries = None
    if "META-INF/manifest.xml" not in files:
        add("META-INF/manifest.xml", "shall", "manifest-missing", "no META-INF/manifest.xml (2.2.1 B)")
    else:
        try:
            root = ET.fromstring(z.read("META-INF/manifest.xml"))
            if root.tag != MANI + "manifest":
                add("META-INF/manifest.xml", "shall", "manifest-root", "root element is not manifest:manifest (2.2.1 B.2)")
            else:
                entries = [e for e in root if e.tag == MANI + "file-entry"]
        except ET.ParseError as e:
            add("META-INF/manifest.xml", "shall", "manifest-malformed", "not well-formed: %s (2.2.1 B.1)" % e)
    if entries is not None:
        paths = [e.get(MANI + "full-path") for e in entries]
        for p in sorted({p for p in paths if paths.count(p) > 1}, key=str):
            add("META-INF/manifest.xml", "shall", "manifest-duplicate", "more than one file-entry for %s (3.2)" % p)
        for p in paths:
            if p in ("META-INF/manifest.xml", "mimetype"):
                add("META-INF/manifest.xml", "shall", "manifest-self", "file-entry for %s (3.2)" % p)
            elif p and p.endswith("/") and p != "/":
                if p + "content.xml" not in files and p + "styles.xml" not in files:
                    add("META-INF/manifest.xml", "should", "manifest-dir-entry", "file-entry for %s, a directory without a document (4.3)" % p)
            elif p not in files and p != "/":
                add("META-INF/manifest.xml", "should", "manifest-dangling", "file-entry for %s, which is not in the package (3.2)" % p)
        for n in sorted(files):
            if n != "mimetype" and not n.startswith("META-INF/") and n not in paths:
                add(n, "shall", "manifest-coverage", "no file-entry in the manifest (3.2)")
        root_entry = [e for e in entries if e.get(MANI + "full-path") == "/"]
        if "mimetype" in files and not root_entry:
            add("META-INF/manifest.xml", "shall", "manifest-root-entry", 'no file-entry for "/" although the package has a mimetype file (3.2)')
        for e in root_entry:
            mt = e.get(MANI + "media-type")
            if "mimetype" not in files:
                add("mimetype", "shall", "mimetype-missing", 'file-entry for "/" but no mimetype file (3.3)')
            elif mime is not None and mt != mime:
                add("mimetype", "shall", "mimetype-mismatch", 'mimetype is "%s" but the "/" media-type is "%s" (3.3)' % (mime, mt))
        # encrypted entries (3.4.1)
        for e in entries:
            if e.find(MANI + "encryption-data") is None:
                continue
            p = e.get(MANI + "full-path")
            size = e.get(MANI + "size")
            if size is None or not re.fullmatch(r"[0-9]+", size):
                add(p, "shall", "encrypted-size", "encrypted entry without a valid manifest:size (3.4.1, 4.8.13)")
            if p in files and z.getinfo(p).compress_type != zipfile.ZIP_STORED:
                add(p, "shall", "encrypted-method", "encrypted entry is not flagged STORED in the zip (3.4.1)")

    # signature files (2.2.1 D)
    sigs = sorted(n for n in files if n.startswith("META-INF/") and "signatures" in n.rsplit("/", 1)[-1])
    for n in sigs:
        try:
            root = ET.fromstring(z.read(n))
        except ET.ParseError as e:
            add(n, "shall", "signature-malformed", "not well-formed: %s (2.2.1 D.1)" % e)
            continue
        if root.tag != DSIG + "document-signatures":
            add(n, "shall", "signature-root", "root element is not dsig:document-signatures (2.2.1 D.2)")
            continue
        p = os.path.join(work, os.path.basename(path), n)
        os.makedirs(os.path.dirname(p), exist_ok=True)
        open(p, "wb").write(z.read(n))
        r = subprocess.run(JING + [DSIG_SCHEMA, p], capture_output=True, text=True)
        for line in (r.stdout + r.stderr).splitlines():
            m = ERR.match(line)
            if m:
                add(n, "shall", "signature-schema", "%s:%s %s (2.2.1 D.3)" % (m.group(2), m.group(3), m.group(5)))
            elif line.strip():
                add(n, "shall", "signature-schema", line.strip())
    return out


def main():
    outdir, pkgjson = sys.argv[1], sys.argv[2]
    if not os.path.abspath(pkgjson).startswith(os.path.join(D, "runs") + os.sep):
        sys.exit("pkgcheck.py: <pkg.json> must live under " + os.path.join(D, "runs"))
    work = os.path.abspath(pkgjson[:-5] + ".xml")
    shutil.rmtree(work, ignore_errors=True)
    errors, bad_zip = [], []
    pkgs = sorted(p for p in os.listdir(outdir) if not p.startswith("."))
    for pkg in pkgs:
        try:
            found = check_package(os.path.join(outdir, pkg), work)
        except zipfile.BadZipFile:
            bad_zip.append(pkg); continue
        for entry, level, rule, msg in found:
            errors.append([pkg + "/" + entry, 0, 0, "%s: [%s] %s" % (level, rule, msg)])
    json.dump({"packages": pkgs, "bad_zip": bad_zip, "errors": errors}, open(pkgjson, "w"))
    shall = [e for e in errors if e[3].startswith("shall:")]
    print(f"package check: {len(shall)} shall and {len(errors) - len(shall)} should findings in "
          f"{len({e[0].split('/')[0] for e in errors})} of {len(pkgs)} packages, bad zips: {bad_zip}")
    rules = {}
    for e in errors:
        k = e[3].split("]")[0] + "]"
        rules[k] = rules.get(k, 0) + 1
    for k in sorted(rules):
        print(f"  {rules[k]:6d} {k}")


main()
