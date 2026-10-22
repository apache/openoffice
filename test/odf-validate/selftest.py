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
for k, ok in checks.items():
    print(("PASS " if ok else "FAIL ") + k)
sys.exit(0 if all(checks.values()) else 1)
