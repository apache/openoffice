################################################################
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

"""Evaluates formulas in a new Calc document. The data area the cases refer to is described at the top of
data/calcval-cases.tsv. Ported from the ODF harness's calcval.py."""
import math

from aootest.calcval import api_formula, parse_mode
from aootest_worker.ops import core


def _num(v):
    return v if math.isfinite(v) else repr(v)


def cell_result(cell):
    try:
        rt = cell.getPropertyValue("FormulaResultType").value
    except Exception:
        rt = None
    return {"rt": rt, "err": cell.getError(), "str": cell.getString(), "val": _num(float(cell.getValue()))}


def setup_data(doc):
    sheets = doc.getSheets()
    sheets.insertNewByName("Other", 1)
    sh = sheets.getByIndex(0)

    def put(addr, vals):
        a0 = sh.getCellRangeByName(addr).getRangeAddress()
        for r, row in enumerate(vals):
            for c, v in enumerate(row):
                if v is None:
                    continue
                cell = sh.getCellByPosition(a0.StartColumn + c, a0.StartRow + r)
                if isinstance(v, str):
                    cell.setString(v)
                else:
                    cell.setValue(v)

    put("A1", [[v] for v in "yxxyxyxxy"])
    put("B1", [[v] for v in "yxyyyyxyx"])
    put("D1", [[10 * i, 11 * i, 101 * i] for i in range(1, 6)])
    put("H1", [[10 * i for i in range(1, 6)], [11 * i for i in range(1, 6)], [101 * i for i in range(1, 6)]])
    put("N1", [[v] for v in (3, 5, 8, 9, 13, 14)])
    put("O1", [[v] for v in (1, 2, 3, 4, 5, 6)])
    put("P1", [[v] for v in (2, 4, 7, 15, 30, 58)])
    put("R1", [["Name", "Age", "Score"], ["A", 20, 5], ["B", 30, "n/a"], ["C", 40, 15], ["D", 50, None], ["E", 60, 25]])
    put("V1", [["Age"], [">=30"]])
    put("W1", [["Age"], [None]])
    put("Y1", [["Name"], ["A"]])
    put("AH1", [[v] for v in (6, 6, 6, 6, 3, 2, 1, 2, 6, 4, 2)])
    put("AI1", [[v] for v in (1, 3, 5)])
    put("AJ1", [[v] for v in (2, 4, 7, 9)])
    put("AK1", [[v] for v in (2, 4, 7, 9)])
    return sh


def calc_eval(cases):
    """Sets each case's formula in a fresh Calc document and returns {"cases": [{"id", "result", "readback"} or
    {"id", "exception"}], "aborted_at": id or None}. The scratch area AA1:AF12 is cleared after every case."""
    doc = core._desktop().loadComponentFromURL("private:factory/scalc", "_blank", 0, (core.pv("Hidden", True),))
    if doc is None:
        raise core.Fail("uno", "could not create a Calc document")
    results, aborted = [], None
    try:
        sh = setup_data(doc)
        scratch = sh.getCellRangeByName("AA1:AF12")
        for case in cases:
            kind, cols, rows, row = parse_mode(case["mode"])
            rec = {"id": case["id"]}
            try:
                formula = api_formula(case["formula"])
                if kind == "cell":
                    cell = sh.getCellByPosition(26, row - 1)
                    cell.setFormula(formula)
                    cells = [cell]
                else:
                    rng = sh.getCellRangeByPosition(26, 0, 26 + cols - 1, rows - 1)
                    rng.setArrayFormula(formula)
                    cells = [sh.getCellByPosition(26 + c, r) for r in range(rows) for c in range(cols)]
                doc.calculateAll()
                rec["result"] = [cell_result(c) for c in cells]
                rec["readback"] = cells[0].getFormula()
            except Exception as e:
                rec["exception"] = "%s: %s" % (type(e).__name__, e)
                if "Disposed" in type(e).__name__ or "connect" in str(e).lower():
                    aborted = case["id"]
            results.append(rec)
            try:
                scratch.clearContents(1 | 2 | 4 | 8 | 16 | 32)
            except Exception as e:
                aborted = aborted or case["id"]
                rec["exception"] = rec.get("exception", "") + " clear: %s" % e
            if aborted:
                break
    finally:
        core._close(doc)
    return {"cases": results, "aborted_at": aborted}


OPS = {"calc_eval": core.guarded(calc_eval)}
