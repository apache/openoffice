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

# usage: selftest.py -- validator negative controls; exits 1 if any control misbehaves
import collections, json, os, subprocess, sys
D = os.path.dirname(os.path.abspath(__file__))
val = os.path.join(D, "runs", "control", "val.json")
os.makedirs(os.path.dirname(val), exist_ok=True)
subprocess.run([sys.executable, "-I", os.path.join(D, "make-controls.py")], check=True)
subprocess.run([sys.executable, "-I", os.path.join(D, "validate.py"), os.path.join(D, "fixtures", "control"), val], check=True, capture_output=True)
d = json.load(open(val))
def errs(v, pkg): return [e[3] for e in d["errors"][v] if e[0].startswith(pkg + "/")]
checks = {
    "good.odt clean in 1.2 and 1.4": not errs("12", "good.odt") and not errs("14", "good.odt"),
    "bad.odt flags text:bogus in 1.2 and 1.4": all(any('"text:bogus"' in m for m in errs(v, "bad.odt")) for v in ("12", "14")),
    "wrongver.odt flags office:version in 1.2 and 1.4": all(any('office:version' in m for m in errs(v, "wrongver.odt")) for v in ("12", "14")),
    "mathobj.odt MathML object skipped": d["mathml_skipped"] == ["mathobj.odt/Object 1/content.xml"] and not errs("12", "mathobj.odt"),
    "no unparsed jing output": not any(e[0] == "?" for v in d["errors"] for e in d["errors"][v]),
}
ids = os.path.join(D, "runs", "control", "ids.json")
subprocess.run([sys.executable, "-I", os.path.join(D, "idcheck.py"), os.path.join(D, "fixtures", "control"), ids], check=True, capture_output=True)
i = json.load(open(ids))
def ierrs(pkg): return [e[3] for e in i["errors"] if e[0].startswith(pkg + "/")]
checks.update({
    "idcheck: good.odt, goodid.odt (normalized ids) clean": not ierrs("good.odt") and not ierrs("goodid.odt"),
    "idcheck: dupid.odt flags the duplicate after normalization": any("duplicates" in m for m in ierrs("dupid.odt")),
    "idcheck: badid.odt, badid2.odt flag non-NCName ids": all(any("not an NCName" in m for m in ierrs(p)) for p in ("badid.odt", "badid2.odt")),
    "idcheck: badns.odt flags the unbound prefix": any("unbound prefix" in m for m in ierrs("badns.odt")),
    "idcheck: badsig.odt flags a duplicate id in a signature file": any("duplicates" in m for m in ierrs("badsig.odt")),
    "idcheck: no errors in other packages": {e[0].split("/")[0] for e in i["errors"]} == {"dupid.odt", "badid.odt", "badid2.odt", "badns.odt", "badsig.odt"},
})
subprocess.run([sys.executable, "-I", os.path.join(D, "make-pkg-controls.py")], check=True)
pk = os.path.join(D, "runs", "control", "pkg.json")
pdir = os.path.join(D, "fixtures", "control-pkg")
subprocess.run([sys.executable, "-I", os.path.join(D, "pkgcheck.py"), pdir, pk], check=True, capture_output=True)
pe = json.load(open(pk))["errors"]
expected = json.load(open(os.path.join(pdir, ".expected.json")))
def found(pkg): return sorted({tuple(m.split("]")[0].split(": [")) for m in (e[3] for e in pe if e[0].startswith(pkg + "/"))})
for pkg in sorted(expected):
    checks["pkgcheck: " + pkg + " reports exactly " + (", ".join(r for _, r in map(tuple, expected[pkg])) or "nothing")] = found(pkg) == [tuple(x) for x in expected[pkg]]
subprocess.run([sys.executable, "-I", os.path.join(D, "make-doctype-controls.py")], check=True)
dt = os.path.join(D, "runs", "control", "dt.json")
ddir = os.path.join(D, "fixtures", "control-doctype")
subprocess.run([sys.executable, "-I", os.path.join(D, "doctypecheck.py"), ddir, dt], check=True, capture_output=True)
de = json.load(open(dt))["errors"]
dexpected = json.load(open(os.path.join(ddir, ".expected.json")))
def dfound(doc): return sorted({tuple(m.split("]")[0].split(": [")) for m in (e[3] for e in de if e[0].startswith(doc + "/"))})
for doc in sorted(dexpected):
    checks["doctypecheck: " + doc + " reports exactly " + (", ".join(r for _, r in dexpected[doc]) or "nothing")] = dfound(doc) == [tuple(x) for x in dexpected[doc]]
import importlib.util
from xml.sax.saxutils import quoteattr
spec = importlib.util.spec_from_file_location("formulacheck", os.path.join(D, "formulacheck.py"))
fc = importlib.util.module_from_spec(spec)
spec.loader.exec_module(fc)
cases = [l.rstrip("\n").split("\t") for l in open(os.path.join(D, "formula-cases.tsv"), encoding="utf-8") if not l.startswith("#") and l.strip()]
NSDECL = ('xmlns:office="urn:oasis:names:tc:opendocument:xmlns:office:1.0" xmlns:table="urn:oasis:names:tc:opendocument:xmlns:table:1.0" '
          'xmlns:style="urn:oasis:names:tc:opendocument:xmlns:style:1.0" xmlns:text="urn:oasis:names:tc:opendocument:xmlns:text:1.0" '
          'xmlns:of="urn:oasis:names:tc:opendocument:xmlns:of:1.2" xmlns:oooc="http://openoffice.org/2004/calc" xmlns:ooow="http://openoffice.org/2004/writer"')
lines = ["<office:document-content %s><office:body>" % NSDECL]
for st, attr, val, msg in cases:
    val = val.replace("\\n", "\n").replace("\\u00a0", "\u00a0")
    name, _, ctx = attr.partition("@")
    tag = "office:text" if ctx == "text" else "office:spreadsheet"
    lines.append("<%s><table:table-cell %s=%s/></%s>" % (tag, name, quoteattr(val, {"\n": "&#10;"}), tag))
lines.append("</office:body></office:document-content>")
got = collections.defaultdict(list)
for line, col, level, rule, m in fc.check_stream("\n".join(lines).encode("utf-8"), collections.Counter()):
    got[line - 2].append((level + ":" + rule, m))
bad_cases = []
for n, (st, attr, val, msg) in enumerate(cases):
    want = [] if st == "ok" else st.split("+")
    if sorted(r for r, _ in got[n]) != sorted(want) or (msg and not any(msg in m for _, m in got[n])):
        bad_cases.append((n, st, attr, val, got[n]))
for c in bad_cases[:10]:
    print("CASE", *c)
checks["formulacheck: all %d cases in formula-cases.tsv give exactly the expected findings" % len(cases)] = not bad_cases and len(cases) > 100
checks.update({
    "extended: extforeign.odt invalid as written, valid once foreign content is removed": bool(errs("12", "extforeign.odt")) and not errs("12x", "extforeign.odt"),
    "extended: good.odt, goodid.odt clean": not errs("12x", "good.odt") and not errs("12x", "goodid.odt"),
    "extended: bad.odt (unknown element in an ODF namespace) still flagged": any('"text:bogus"' in m for m in errs("12x", "bad.odt")),
    "extended: wrongver.odt (attribute in an ODF namespace) still flagged": any("office:version" in m for m in errs("12x", "wrongver.odt")),
    "extended: badns.odt (unbound prefix) still flagged": any("not bound" in m for m in errs("12x", "badns.odt")),
})
fspec = importlib.util.spec_from_file_location("foreign", os.path.join(D, "foreign.py"))
fg = importlib.util.module_from_spec(fspec)
fspec.loader.exec_module(fg)
MIXED = fg.mixed_elements(os.path.join(D, "schema", "OpenDocument-v1.2-os-schema.rng"))
HDR = ('<office:text xmlns:office="urn:oasis:names:tc:opendocument:xmlns:office:1.0" xmlns:text="urn:oasis:names:tc:opendocument:xmlns:text:1.0" '
       'xmlns:draw="urn:oasis:names:tc:opendocument:xmlns:drawing:1.0" xmlns:foo="urn:foo" xmlns:dc="http://purl.org/dc/elements/1.1/" '
       'xmlns:xlink="http://www.w3.org/1999/xlink" xmlns:xf="http://www.w3.org/2002/xforms"')
FOREIGN_CASES = [
    ("foreign attribute removed", '<text:p foo:x="1" text:style-name="a">t</text:p>', '<text:p text:style-name="a">t</text:p>'),
    ("foreign element outside a paragraph dropped with its content", '<foo:x>stuff<text:p>q</text:p></foo:x><text:p>t</text:p>', '<text:p>t</text:p>'),
    ("foreign element in a paragraph: tags removed, content kept", '<text:p>a<foo:x>b<text:span>c</text:span></foo:x>d</text:p>', '<text:p>a<text:span>c</text:span>bd</text:p>'.replace("<text:span>c</text:span>bd", "b<text:span>c</text:span>d")),
    ("foreign element in a span in a paragraph: tags removed", '<text:p><text:span>a<foo:x>b</foo:x></text:span></text:p>', '<text:p><text:span>ab</text:span></text:p>'),
    ("foreign element below a frame (no character data): dropped", '<text:p><draw:frame><foo:x>b</foo:x></draw:frame></text:p>', '<text:p><draw:frame></draw:frame></text:p>'),
    ("namespace declared on an unwrapped element is kept for its children", '<text:p><foo:x xmlns:t2="urn:oasis:names:tc:opendocument:xmlns:text:1.0"><t2:span>a</t2:span></foo:x></text:p>',
     '<text:p><t2:span xmlns:t2="urn:oasis:names:tc:opendocument:xmlns:text:1.0">a</t2:span></text:p>'),
    ("elements and attributes of table 4 namespaces kept", '<text:p xlink:href="u"><dc:title>t</dc:title></text:p>', '<text:p xlink:href="u"><dc:title>t</dc:title></text:p>'),
    ("content below a table 4 element kept (wildcard content)", '<xf:model><foo:data><bar/></foo:data></xf:model>', '<xf:model><foo:data><bar></bar></foo:data></xf:model>'),
    ("text is escaped again", '<text:p>a &amp; b &lt; c</text:p>', '<text:p>a &amp; b &lt; c</text:p>'),
]
for name, src, want in FOREIGN_CASES:
    got = fg.strip_foreign(HDR + ">" + src + "</office:text>", MIXED).split("\n", 1)[1].strip()
    inner = got[got.index(">") + 1:-len("</office:text>")] if got.startswith("<office:text") else got
    checks["foreign.py: " + name] = inner == want
ODFV_JAR = os.environ.get("ODFV_ODFVALIDATOR", os.path.join(D, "tools", "odfvalidator-1.1.8-incubating-jar-with-dependencies.jar"))
if os.path.isfile(ODFV_JAR):
    cc = os.path.join(D, "runs", "control", "cross.json")
    subprocess.run([sys.executable, "-I", os.path.join(D, "crosscheck.py"), os.path.join(D, "fixtures", "control"), os.path.join(D, "runs", "control", "val.json"), cc], check=True, capture_output=True)
    x = json.load(open(cc))
    c, e = x["conformance"], x["extended"]
    checks.update({
        "crosscheck: odfvalidator flags bad.odt in both modes": all("bad.odt/content.xml" in m["odfvalidator_error_streams"] for m in (c, e)),
        "crosscheck: odfvalidator flags extforeign.odt as written but not as an extended document": "extforeign.odt/content.xml" in c["odfvalidator_error_streams"] and "extforeign.odt/content.xml" not in e["odfvalidator_error_streams"],
        "crosscheck: odfvalidator finds the duplicate xml:id that jing -i cannot": "dupid.odt/content.xml" in c["odfvalidator_error_streams"],
        "crosscheck: good.odt clean in odfvalidator": not any("good.odt/" in k for m in (c, e) for k in m["odfvalidator_error_streams"]),
        "crosscheck: no stream where only jing reports errors": not c["only_jing"] and not e["only_jing"],
    })
else:
    print("SKIP crosscheck controls: odfvalidator not installed (fetch-odfvalidator.sh)")
for k, ok in checks.items():
    print(("PASS " if ok else "FAIL ") + k)
sys.exit(0 if all(checks.values()) else 1)
