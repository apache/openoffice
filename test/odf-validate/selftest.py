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
import json, os, subprocess, sys
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
for k, ok in checks.items():
    print(("PASS " if ok else "FAIL ") + k)
sys.exit(0 if all(checks.values()) else 1)
