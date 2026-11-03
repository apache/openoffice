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

"""Calc formula cases (data/calcval-cases.tsv) that need no office to read and judge. Shared by the host and the worker.
Ported from the ODF harness's calcval.py."""
import re

from aootest.paths import DATA

CASES = DATA / "calcval-cases.tsv"
NOT_AVAILABLE = 32767
# Analysis add-in functions are not known by their English names to setFormula; the cases keep the ODF names and
# are rewritten to the programmatic names.
ADDIN = {"RECEIVED": "getReceived", "BESSELK": "getBesselk", "BESSELY": "getBessely", "CONVERT": "getConvert"}


def parse_mode(mode):
    """(kind, columns, rows, scratch row) of a case mode: cell, cell@N, array or array:CxR."""
    m = re.fullmatch(r"cell(?:@(\d+))?", mode)
    if m:
        return "cell", 1, 1, int(m.group(1) or 1)
    m = re.fullmatch(r"array(?::(\d+)x(\d+))?", mode)
    if m:
        return "array", int(m.group(1) or 1), int(m.group(2) or 1), 1
    raise ValueError("bad mode %r" % mode)


def read_cases(path=CASES):
    cases = []
    for ln, line in enumerate(open(path, encoding="utf-8"), 1):
        line = line.rstrip("\n")
        if not line or line.startswith("#"):
            continue
        f = line.split("\t")
        if len(f) != 5:
            raise ValueError("%s:%d: expected 5 tab separated columns, got %d" % (path, ln, len(f)))
        cases.append(dict(zip(("id", "formula", "mode", "expected", "note"), f)))
    ids = [c["id"] for c in cases]
    if len(set(ids)) != len(ids):
        raise ValueError("duplicate case ids in %s" % path)
    return cases


def api_formula(f):
    for name, prog in ADDIN.items():
        f = re.sub(r"(?<![A-Za-z0-9_.])%s\(" % name, "com.sun.star.sheet.addin.Analysis.%s(" % prog, f)
    return f


def elem_match(exp, cell):
    """Whether one result cell ({"rt", "err", "str", "val"}) equals the expected element."""
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
    """record (no expectation), match or mismatch."""
    if expected == "-":
        return "record"
    exps = expected.split("|")
    if len(exps) != len(cells):
        return "mismatch"
    return "match" if all(elem_match(e, c) for e, c in zip(exps, cells)) else "mismatch"


def summary(cells):
    """The result cells as short text, for failure messages."""
    return " | ".join("err=%d" % c["err"] if c["err"] else ("%r" % c["val"] if c["rt"] == "VALUE" else "%r" % c["str"]) for c in cells)
