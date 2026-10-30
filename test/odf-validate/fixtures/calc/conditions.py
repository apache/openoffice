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
# usage: python3 -I fixtures/calc/conditions.py <outdir> [<verify.json>]
# Prints every table:condition of the validations in <outdir>/validation-*.ods next to the cell it applies to, the formulacheck.py verdict
# and, when verify.json (written by verify.py) is given, whether the reloaded Validation equals what make.py read back before saving.
import sys, os, zipfile, json, importlib.util
import xml.etree.ElementTree as ET

D = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
spec = importlib.util.spec_from_file_location("formulacheck", os.path.join(D, "formulacheck.py"))
fc = importlib.util.module_from_spec(spec)
spec.loader.exec_module(fc)
T = "{urn:oasis:names:tc:opendocument:xmlns:table:1.0}"
out = sys.argv[1]
ver = json.load(open(sys.argv[2])) if len(sys.argv) > 2 else {}
expect = json.load(open(os.path.join(os.path.dirname(os.path.abspath(out)), "work", "expect.json")))
total = bad = 0
for name in sorted(expect):
    if not name.startswith("validation-"):
        continue
    root = ET.fromstring(zipfile.ZipFile(os.path.join(out, name)).read("content.xml"))
    vals = {v.get(T + "name"): v for v in root.iter(T + "content-validation")}
    cellval = {}
    for table in root.iter(T + "table"):
        row = 0
        for tr in table.iter(T + "table-row"):
            cells = [c for c in tr if c.tag in (T + "table-cell", T + "covered-table-cell")]
            if cells:
                cellval.setdefault(row + 1, cells[0].get(T + "content-validation-name"))
            row += int(tr.get(T + "number-rows-repeated", "1"))
    print("==", name)
    vrows = {x["cell"]: x for x in ver.get(name, [])}
    for r in expect[name]:
        n = int(r["cell"][1:])
        vn = cellval.get(n)
        v = vals.get(vn)
        cond = v.get(T + "condition") if v is not None else None
        if cond is None:
            verdict = "(no table:condition)"
        else:
            body = cond.split(":", 1)[1] if cond.startswith("of:") else cond
            err, funcs, ragged = fc.check_value("table:condition", body, cond.startswith("of:"))
            total += 1
            verdict = "conformant" if err is None else "NOT conformant: %s at character %d" % (err[1], err[0] + (len(cond) - len(body)) + 1)
            bad += err is not None
        rt = ""
        if vrows:
            d = vrows[r["cell"]]["diff"]
            rt = "round-trip OK" if not d else "round-trip DIFF " + json.dumps(d)
        print("%-34s %-5s %-6s %s | %s | %s" % (r["label"], r["cell"], vn, cond, verdict, rt))
print("conditions checked: %d, not conformant: %d" % (total, bad))
