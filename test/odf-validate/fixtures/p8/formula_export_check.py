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

# usage: runpy.sh fixtures/p8/formula_export_check.py <port> <outdir> <mode name> <office:version>
# The office runs with the save mode of <mode name> (DefaultVersion in its profile). Builds a sheet with a named expression
# that is a list of ranges (as the Excel import creates it) and with formulas whose references were broken by deleting a column
# (one single reference, one range, one range with a function), saves it, and checks the formulas in content.xml:
#  - the range list is joined with the union operator `~`, never with `;`;
#  - broken references are written as [#REF!], never with #REF! inside a reference such as [.#REF!1];
#  - a function with several parameters keeps `;` between its parameters;
# then reloads the file: the named expression keeps both ranges and the broken formulas still give #REF! (error 524); a second
# save of the reloaded file keeps both spellings.
# Exit code 1 when any check fails.
import os, re, sys, zipfile
import uno
from com.sun.star.table import CellAddress
from unohelp import connect, desktop, load, pv

port, out, name, want_version = int(sys.argv[1]), os.path.abspath(sys.argv[2]), sys.argv[3], sys.argv[4]
os.makedirs(out, exist_ok=True)
desk = desktop(connect(port))
doc = desk.loadComponentFromURL("private:factory/scalc", "_blank", 0, (pv("Hidden", True),))
sh = doc.Sheets.getByIndex(0)
cell = lambda a: sh.getCellRangeByName(a)
for r in range(1, 4):
    cell("A%d" % r).Value = r
    cell("B%d" % r).Value = 10 * r
cell("E1").setFormula("=A1+1")
cell("E2").setFormula("=SUM(A1:A3)")
cell("E3").setFormula("=IF(A1>0;SUM(A1:A3);0)")
cell("E4").setFormula("=SUM(B1:B3;C1)")
doc.NamedRanges.addNewByName("rangelist", "$Sheet1.$H$1:$I$2;$Sheet1.$K$1:$L$2", CellAddress(0, 0, 0), 0)
sh.Columns.removeByIndex(0, 1)          # column A: the references to it are now broken

path = os.path.join(out, "formulas-%s.ods" % name)
doc.storeToURL(uno.systemPathToFileUrl(path), (pv("FilterName", "calc8"),))
doc.close(True)

z = zipfile.ZipFile(path)
content = z.read("content.xml").decode("utf-8")
version = re.search(r'office:version="([^"]+)"', content).group(1)
formulas = re.findall(r'table:formula="([^"]*)"', content)
exprs = re.findall(r'table:expression="([^"]*)"', content)
print("version", version, "| formulas", formulas, "| expressions", exprs)

checks = [
    ("office:version of the mode", version == want_version),
    ("a named expression was written", len(exprs) == 1),
    ("range list joined with ~", bool(exprs) and "~" in exprs[0] and "];[" not in exprs[0]),
    ("#REF! only as a whole reference [#REF!]", all(re.sub(r"\[#REF!\]", "", f).find("#REF!") < 0 for f in formulas + exprs)),
    ("broken references are [#REF!]", sum("[#REF!]" in f for f in formulas) == 3),
    ("parameters keep ;", any(f.startswith("of:=SUM(") and ";" in f for f in formulas)
                          or any(";" in f for f in formulas if "SUM" in f)),
]

ctx_doc = load(desk, path)
sh2 = ctx_doc.Sheets.getByIndex(0)
names = ctx_doc.NamedRanges
checks.append(("the named expression survives a reload", names.hasByName("rangelist")))
if names.hasByName("rangelist"):
    content2 = names.getByName("rangelist").Content
    print("reloaded content:", content2)
    checks.append(("the reloaded list has both ranges", content2.count("$G$1") == 1 and content2.count("$J$1") == 1))
errs = [sh2.getCellRangeByName(a).getError() for a in ("D1", "D2", "D3")]
print("reloaded errors of the broken formulas (D1..D3):", errs)
checks.append(("broken formulas still give #REF! (524)", errs == [524, 524, 524]))
path2 = os.path.join(out, "formulas2-%s.ods" % name)
ctx_doc.storeToURL(uno.systemPathToFileUrl(path2), (pv("FilterName", "calc8"),))
ctx_doc.close(True)
content3 = zipfile.ZipFile(path2).read("content.xml").decode("utf-8")
f3 = re.findall(r'table:formula="([^"]*)"', content3)
e3 = re.findall(r'table:expression="([^"]*)"', content3)
print("second save:", f3, e3)
checks.append(("a second save keeps [#REF!] and ~", sum("[#REF!]" in f for f in f3) == 3 and len(e3) == 1 and "~" in e3[0]
               and all(re.sub(r"\[#REF!\]", "", f).find("#REF!") < 0 for f in f3 + e3)))

fails = 0
for label, ok in checks:
    fails += not ok
    print("%-4s %s" % ("ok" if ok else "FAIL", label))
sys.exit(1 if fails else 0)
