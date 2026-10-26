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

# usage: doctypecheck.py <outdir> <dt.json>
# ODF 1.2 Part 1 2.2.1 A.2/E and 2.2.3-2.2.10 on every package (or flat XML file) in <outdir>:
# the mimetype file (or office:mimetype), the root element and the office:body child must agree
# on one document type. Findings are "shall: [rule] ..." or "should: [rule] ...".
import sys, os, io, json, zipfile, collections
from xml.etree import ElementTree as ET

D = os.path.dirname(os.path.abspath(__file__))
OFFICE = "urn:oasis:names:tc:opendocument:xmlns:office:1.0"
MATH = "http://www.w3.org/1998/Math/MathML"
P = "application/vnd.oasis.opendocument."
# type -> (mimetype values, office:body child). 2.2.5 spells the drawing values "application.vnd...graphics"
# and "...graphic-template"; the registered values in Part 1 appendix C are used instead.
TYPES = {
    "text": ([P + "text", P + "text-template", P + "text-master"], "text"),
    "spreadsheet": ([P + "spreadsheet", P + "spreadsheet-template"], "spreadsheet"),
    "drawing": ([P + "graphics", P + "graphics-template"], "drawing"),
    "presentation": ([P + "presentation", P + "presentation-template"], "presentation"),
    "chart": ([P + "chart", P + "chart-template"], "chart"),
    "image": ([P + "image", P + "image-template"], "image"),
    "formula": ([P + "formula"], None),
    "database": ([P + "base"], "database"),
}
BY_MIME = {m: t for t, (ms, _) in TYPES.items() for m in ms}
# registered in Part 1 appendix C, but not a value any of 2.2.3-2.2.10 allows
UNLISTED = {P + "text-web", P + "formula-template"}


def local(tag):
    if not tag.startswith("{"):
        return tag
    ns, name = tag[1:].split("}", 1)
    return {OFFICE: "office:", MATH: "math:"}.get(ns, "?:") + name


def scan(data):
    """Returns (root tag, [office:body child tags]); stops reading after office:body."""
    root, body, depth, in_body = None, [], 0, False
    for ev, el in ET.iterparse(io.BytesIO(data), events=("start", "end")):
        if ev == "start":
            depth += 1
            if depth == 1:
                root = el.tag
            elif depth == 2 and el.tag == "{%s}body" % OFFICE:
                in_body = True
            elif depth == 3 and in_body:
                body.append(el.tag)
        else:
            depth -= 1
            if depth == 1 and in_body:
                break
    return root, body


def check_content(add, entry, data, T, flat_mime=None):
    """Checks one content.xml (or a flat file) against document type T."""
    try:
        root, body = scan(data)
    except ET.ParseError as e:
        add(entry, "shall", "doctype-malformed", "not well-formed: %s" % e)
        return
    if T == "formula":
        if root != "{%s}math" % MATH:
            add(entry, "shall", "doctype-root", "formula document whose root is %s, not math:math (2.2.9 B)" % local(root))
        return
    want = "{%s}document%s" % (OFFICE, "" if flat_mime is not None else "-content")
    if root != want:
        add(entry, "shall", "doctype-root", "root is %s, not %s (2.2.1 B.2.1, C.2)" % (local(root), local(want)))
        return
    child = TYPES[T][1]
    if "{%s}%s" % (OFFICE, child) not in body:
        add(entry, "shall", "doctype-body", "%s document without office:%s in office:body; has %s (2.2.x C)" %
            (T, child, ", ".join(local(b) for b in body) or "no body child"))


def check_package(path):
    out = []

    def add(entry, level, rule, msg):
        out.append((entry, level, rule, msg))
    with open(path, "rb") as f:
        magic = f.read(4)
    if magic[:2] != b"PK":
        return check_flat(path, out, add)
    z = zipfile.ZipFile(path)
    files = set(z.namelist())
    if "content.xml" not in files and "styles.xml" not in files:
        add("", "shall", "doctype-no-content", "neither content.xml nor styles.xml (2.2.1 A.2)")
    mime = z.read("mimetype").decode("utf-8", "replace") if "mimetype" in files else None
    T = None
    if mime is None:
        add("mimetype", "shall", "doctype-no-mimetype", "no mimetype file (2.2.3-2.2.10 B)")
    elif mime in UNLISTED:
        add("mimetype", "shall", "doctype-unlisted-mimetype", '"%s" is registered but no document type in 2.2.3-2.2.10 allows it' % mime)
    elif mime not in BY_MIME:
        add("mimetype", "shall", "doctype-unknown-mimetype", 'mimetype "%s" is not a value of any document type (2.2.1 E)' % mime)
    else:
        T = BY_MIME[mime]
    if T and "content.xml" not in files:
        add("content.xml", "shall", "doctype-no-body", "no content.xml, so no office:body child (2.2.x C)")
    elif T:
        check_content(add, "content.xml", z.read("content.xml"), T)
    # sub documents: the manifest media type of a directory against that directory's content.xml
    if "META-INF/manifest.xml" in files:
        try:
            root = ET.fromstring(z.read("META-INF/manifest.xml"))
        except ET.ParseError:
            root = []
        mns = "{urn:oasis:names:tc:opendocument:xmlns:manifest:1.0}"
        for e in root:
            p, mt = e.get(mns + "full-path"), e.get(mns + "media-type")
            if p and p.endswith("/") and p != "/" and p + "content.xml" in files and mt in BY_MIME:
                sub = []
                check_content(lambda en, lv, ru, ms: sub.append((en, ru, ms)), p + "content.xml", z.read(p + "content.xml"), BY_MIME[mt])
                for en, ru, ms in sub:
                    add(en, "should", "sub-" + ru, "sub document typed %s: %s" % (mt, ms))
    return out, mime or "(none)"


def check_flat(path, out, add):
    data = open(path, "rb").read()

    try:
        root = ET.fromstring(data)
    except ET.ParseError as e:
        raise zipfile.BadZipFile(str(e))
    mime = root.get("{%s}mimetype" % OFFICE)
    if root.tag == "{%s}math" % MATH:
        return out, "(flat math)"
    if mime is None:
        add("", "shall", "doctype-no-mimetype", "no office:mimetype attribute (2.2.3-2.2.10 A)")
        return out, "(none)"
    T = BY_MIME.get(mime)
    if mime in UNLISTED:
        add("", "shall", "doctype-unlisted-mimetype", '"%s" is registered but no document type in 2.2.3-2.2.10 allows it' % mime)
    elif T is None or T == "formula":
        add("", "shall", "doctype-unknown-mimetype", 'office:mimetype "%s" is not a value of any flat document type (2.2.1 E)' % mime)
    else:
        check_content(add, "", data, T, flat_mime=mime)
    return out, mime


def main():
    outdir, dtjson = sys.argv[1], sys.argv[2]
    if not os.path.abspath(dtjson).startswith(os.path.join(D, "runs") + os.sep):
        sys.exit("doctypecheck.py: <dt.json> must live under " + os.path.join(D, "runs"))
    errors, bad, types = [], [], collections.Counter()
    pkgs = sorted(p for p in os.listdir(outdir) if not p.startswith("."))
    for pkg in pkgs:
        try:
            found, mime = check_package(os.path.join(outdir, pkg))
        except zipfile.BadZipFile:
            bad.append(pkg); continue
        types[mime] += 1
        for entry, level, rule, msg in found:
            errors.append([pkg + "/" + entry, 0, 0, "%s: [%s] %s" % (level, rule, msg)])
    json.dump({"packages": pkgs, "bad": bad, "types": types, "errors": errors}, open(dtjson, "w"))
    shall = [e for e in errors if e[3].startswith("shall:")]
    print(f"document type check: {len(shall)} shall and {len(errors) - len(shall)} should findings in "
          f"{len({e[0].split('/')[0] for e in errors})} of {len(pkgs)} documents, unreadable: {bad}")
    for k, v in sorted(types.items()):
        print(f"  {v:6d} {k}")
    rules = collections.Counter(e[3].split("]")[0] + "]" for e in errors)
    for k, v in sorted(rules.items()):
        print(f"  {v:6d} {k}")


main()
