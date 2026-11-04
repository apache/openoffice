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

# usage: python -m aootest.checkers.idcheck <outdir> <ids.json>   (tests call check(outdir))
# ODF 1.2 Part 1 2.2.1 D.1 and D.2, and Part 3 2.2.1 F, which `jing -i` does not check: each XML stream is
# parsed namespace-aware, and its xml:id values must be NCNames, unique within the stream.
import sys, os, re, zipfile, json
from xml.parsers import expat

STREAMS = {"content.xml", "styles.xml", "meta.xml", "settings.xml", "manifest.xml"}
XMLID = "http://www.w3.org/XML/1998/namespace id"
# XML 1.0 5th ed. NameStartChar/NameChar without ':'
_S = "A-Z_a-zÀ-ÖØ-öø-˿Ͱ-ͽͿ-῿‌-‍⁰-↏Ⰰ-⿯、-퟿豈-﷏ﷰ-�\U00010000-\U000EFFFF"
NCNAME = re.compile("[" + _S + "][" + _S + r"\-.0-9·̀-ͯ‿-⁀]*\Z")


def check_stream(data):
    """Returns [(line, col, message)]."""
    errs, seen = [], {}
    p = expat.ParserCreate(namespace_separator=" ")

    def start(name, attrs):
        if XMLID not in attrs:
            return
        # xml:id 1.0 sec. 4: normalize like an ID-typed attribute
        v = " ".join(attrs[XMLID].split())
        pos = (p.CurrentLineNumber, p.CurrentColumnNumber)
        if not NCNAME.match(v):
            errs.append(pos + ('xml:id "%s" is not an NCName' % v,))
        elif v in seen:
            errs.append(pos + ('xml:id "%s" duplicates line %d' % (v, seen[v]),))
        else:
            seen[v] = pos[0]
    p.StartElementHandler = start
    try:
        p.Parse(data, True)
    except expat.ExpatError as e:
        errs.append((e.lineno, e.offset, "not namespace-well-formed: " + expat.ErrorString(e.code)))
    return errs


def check(outdir):
    """Returns the result that main() writes as <ids.json>."""
    errors, bad_zip, nstreams = [], [], 0
    pkgs = sorted(p for p in os.listdir(outdir) if not p.startswith("."))
    for pkg in pkgs:
        try:
            z = zipfile.ZipFile(os.path.join(outdir, pkg))
        except Exception:
            bad_zip.append(pkg); continue
        for name in z.namelist():
            base = name.rsplit("/", 1)[-1]
            if base not in STREAMS and not (name.startswith("META-INF/") and "signatures" in base):
                continue
            nstreams += 1
            for line, col, msg in check_stream(z.read(name)):
                errors.append([pkg + "/" + name, line, col, msg])
    return {"packages": pkgs, "bad_zip": bad_zip, "nstreams": nstreams, "errors": errors}


def main(argv=None):
    outdir, idsjson = sys.argv[1:3] if argv is None else argv
    res = check(outdir)
    json.dump(res, open(idsjson, "w"))
    errors, pkgs, bad_zip, nstreams = res["errors"], res["packages"], res["bad_zip"], res["nstreams"]
    print(f"xml:id / namespace check: {len(errors)} errors in {len({e[0].split('/')[0] for e in errors})} of {len(pkgs)} packages, "
          f"streams checked: {nstreams}, bad zips: {bad_zip}")


if __name__ == "__main__":
    main()
