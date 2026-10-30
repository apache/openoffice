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
# usage: runpy.sh fixtures/calc/verify.py <port> <outdir>
# Reloads each document listed in <outdir>/../work/expect.json with validations and compares the Validation of every cell with what make.py read back before saving.
import sys, os, json, uno
from unohelp import connect, desktop, pv

port, out = int(sys.argv[1]), os.path.abspath(sys.argv[2])
work = os.path.join(os.path.dirname(out), "work")
expect = json.load(open(os.path.join(work, "expect.json")))
desk = desktop(connect(port))
result = {}
for name, rows in expect.items():
    if not rows or "readback" not in rows[0]:
        continue
    path = os.path.join(out, name)
    if not os.path.exists(path):
        continue
    doc = desk.loadComponentFromURL(uno.systemPathToFileUrl(path), "_blank", 0, (pv("Hidden", True), pv("MacroExecutionMode", 0), pv("ReadOnly", True)))
    sh = doc.Sheets.getByIndex(0)
    res = []
    for r in rows:
        v = sh.getCellRangeByName(r["cell"]).Validation
        got = {"type": v.Type.value, "op": v.getOperator().value, "f1": v.getFormula1(), "f2": v.getFormula2(),
               "alert": v.ErrorAlertStyle.value, "list": v.ShowList, "ignore": v.IgnoreBlankCells,
               "inp": [v.InputTitle, v.InputMessage], "err": [v.ErrorTitle, v.ErrorMessage],
               "showinp": v.ShowInputMessage, "showerr": v.ShowErrorMessage}
        diff = {k: [r["readback"][k], got[k]] for k in got if got[k] != r["readback"][k]}
        res.append({"cell": r["cell"], "label": r["label"], "got": got, "diff": diff})
    doc.close(True)
    result[name] = res
    print(name, "cells", len(res), "with differences", sum(1 for x in res if x["diff"]))
json.dump(result, open(os.path.join(work, "verify.json"), "w"), indent=1)
