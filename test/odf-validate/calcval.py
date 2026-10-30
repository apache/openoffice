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
# usage: runpy.sh calcval.py <port> <cases.tsv> <out.json>
#        python3 -I calcval.py --compare old.json new.json
#
# Evaluates the formulas of a cases TSV (see calcval-cases.tsv for the columns
# and the data area) in a new Calc document and records, per case, the compile
# and evaluation result of the current build, and whether it equals the value
# that ODF 1.3 Part 4 requires. --compare lists the cases whose result changed.

import json, math, re, sys


def parse_mode(mode):
    m = re.fullmatch(r"cell(?:@(\d+))?", mode)
    if m:
        return "cell", 1, 1, int(m.group(1) or 1)
    m = re.fullmatch(r"array(?::(\d+)x(\d+))?", mode)
    if m:
        return "array", int(m.group(1) or 1), int(m.group(2) or 1), 1
    raise ValueError("bad mode %r" % mode)


def read_cases(path):
    cases = []
    for ln, line in enumerate(open(path, encoding="utf-8"), 1):
        line = line.rstrip("\n")
        if not line or line.startswith("#"):
            continue
        f = line.split("\t")
        if len(f) != 5:
            raise SystemExit("%s:%d: expected 5 tab separated columns, got %d" % (path, ln, len(f)))
        cases.append(dict(zip(("id", "formula", "mode", "expected", "note"), f)))
    ids = [c["id"] for c in cases]
    if len(set(ids)) != len(ids):
        raise SystemExit("duplicate case ids in " + path)
    return cases


# Analysis add-in functions are not known by their English names to setFormula;
# the case formulas keep the ODF names and are rewritten to the programmatic names.
ADDIN = {"RECEIVED": "getReceived", "BESSELK": "getBesselk", "BESSELY": "getBessely"}


def api_formula(f):
    for name, prog in ADDIN.items():
        f = re.sub(r"(?<![A-Za-z0-9_.])%s\(" % name, "com.sun.star.sheet.addin.Analysis.%s(" % prog, f)
    return f


NOT_AVAILABLE = 32767


def elem_match(exp, cell):
    # cell: dict with rt (CellContentType name of the formula result; errors report VALUE), err, str, val
    if exp == "#N/A":
        return cell["err"] == NOT_AVAILABLE
    if exp == "#ERR":
        return cell["err"] != 0
    if cell["err"] != 0:
        return False
    if exp.startswith("s:"):
        return cell["rt"] == "TEXT" and cell["str"] == exp[2:]
    tol = 1e-9
    if exp.startswith("~"):
        exp, tol = exp[1:], 1e-6
    e = float(exp)
    return cell["rt"] == "VALUE" and isinstance(cell["val"], float) and abs(cell["val"] - e) <= tol * max(1.0, abs(e))


def judge(expected, cells):
    if expected == "-":
        return "record"
    exps = expected.split("|")
    if len(exps) != len(cells):
        return "mismatch"
    return "match" if all(elem_match(e, c) for e, c in zip(exps, cells)) else "mismatch"


def num(v):
    return v if math.isfinite(v) else repr(v)


def cell_result(cell):
    try:
        rt = cell.getPropertyValue("FormulaResultType").value
    except Exception:
        rt = None
    return {"rt": rt, "err": cell.getError(), "str": cell.getString(), "val": num(float(cell.getValue()))}


def setup_data(doc):
    sheets = doc.getSheets()
    sheets.insertNewByName("Other", 1)
    sh = sheets.getByIndex(0)

    def put(addr, vals):
        # vals: list of rows; None leaves the cell empty; str is text
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


def office_version(ctx):
    try:
        from unohelp import pv
        cp = ctx.ServiceManager.createInstanceWithContext("com.sun.star.configuration.ConfigurationProvider", ctx)
        ca = cp.createInstanceWithArguments("com.sun.star.configuration.ConfigurationAccess",
                                            (pv("nodepath", "/org.openoffice.Setup/Product"),))
        return "%s %s" % (ca.getByName("ooName"), ca.getByName("ooSetupVersion"))
    except Exception as e:
        return "unknown (%s)" % e


def run(port, cases_path, out_path):
    from unohelp import connect, desktop, pv
    cases = read_cases(cases_path)
    ctx = connect(port)
    desk = desktop(ctx)
    doc = desk.loadComponentFromURL("private:factory/scalc", "_blank", 0, (pv("Hidden", True),))
    if doc is None:
        raise SystemExit("could not create a Calc document")
    results = []
    aborted = None
    try:
        sh = setup_data(doc)
        scratch = sh.getCellRangeByName("AA1:AF12")
        for case in cases:
            kind, cols, rows, row = parse_mode(case["mode"])
            rec = dict(case)
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
                rec["formula_used"] = formula
                res = [cell_result(c) for c in cells]
                rec["readback"] = cells[0].getFormula()
                rec["result"] = res
                rec["status"] = judge(case["expected"], res)
            except Exception as e:
                rec["exception"] = "%s: %s" % (type(e).__name__, e)
                rec["status"] = "exception"
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
        try:
            doc.close(True)
        except Exception:
            pass
    out = {"office": office_version(ctx) if not aborted else "unknown", "cases_file": cases_path,
           "aborted_at": aborted, "cases": results}
    with open(out_path, "w", encoding="utf-8") as f:
        json.dump(out, f, indent=1, ensure_ascii=False)
        f.write("\n")
    n = len(results)
    st = {}
    for r in results:
        st[r["status"]] = st.get(r["status"], 0) + 1
    nerr = sum(1 for r in results if r["status"] != "exception" and any(c["err"] for c in r["result"]))
    print("cases run %d of %d: %s; cases with an error value in the result: %d%s" % (
        n, len(cases), ", ".join("%s=%d" % kv for kv in sorted(st.items())), nerr,
        "; ABORTED at " + aborted if aborted else ""))
    for r in results:
        if r["status"] == "mismatch":
            print("MISMATCH %-28s %-12s expected %s got %s" % (r["id"], r["mode"], r["expected"], summary(r)))
    return 1 if aborted else 0


def summary(rec):
    if "result" not in rec:
        return rec.get("exception", "?")
    return " | ".join("err=%d" % c["err"] if c["err"] else
                      ("%r" % c["val"] if c["rt"] == "VALUE" else "%r" % c["str"]) for c in rec["result"])


def same(a, b):
    if "result" not in a or "result" not in b:
        return a.get("exception") == b.get("exception") and ("result" in a) == ("result" in b)
    if a["readback"] != b["readback"] or len(a["result"]) != len(b["result"]):
        return False
    for x, y in zip(a["result"], b["result"]):
        if (x["rt"], x["err"], x["str"]) != (y["rt"], y["err"], y["str"]):
            return False
        vx, vy = x["val"], y["val"]
        if isinstance(vx, float) and isinstance(vy, float):
            if abs(vx - vy) > 1e-12 * max(1.0, abs(vx)):
                return False
        elif vx != vy:
            return False
    return True


def compare(old_path, new_path):
    old = {r["id"]: r for r in json.load(open(old_path, encoding="utf-8"))["cases"]}
    new = {r["id"]: r for r in json.load(open(new_path, encoding="utf-8"))["cases"]}
    changed = 0
    for i in old:
        if i not in new:
            print("REMOVED %s" % i); changed += 1
        elif not same(old[i], new[i]):
            changed += 1
            print("CHANGED %-28s %-12s %s\n    old: %s [%s]\n    new: %s [%s]" % (
                i, new[i]["mode"], new[i]["formula"], summary(old[i]), old[i]["status"],
                summary(new[i]), new[i]["status"]))
    for i in new:
        if i not in old:
            print("ADDED   %s" % i); changed += 1
    print("%d cases compared, %d changed" % (len(set(old) & set(new)), changed))
    return 1 if changed else 0


def main(argv):
    if len(argv) == 4 and argv[1] == "--compare":
        return compare(argv[2], argv[3])
    if len(argv) == 4:
        return run(int(argv[1]), argv[2], argv[3])
    print("usage: runpy.sh calcval.py <port> <cases.tsv> <out.json>\n"
          "       python3 -I calcval.py --compare old.json new.json", file=sys.stderr)
    return 2


sys.exit(main(sys.argv))
