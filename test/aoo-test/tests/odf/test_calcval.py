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

"""Calc formula results against what ODF 1.3 Part 4 (OpenFormula) requires: one test per case in data/calcval-cases.tsv.
A case with no expected value ('-') only has to evaluate. Deviations that are known are rows in data/expect/calcval.tsv."""
import pytest

from aootest import calcval
from aootest.client import WorkerFailure

CASES = calcval.read_cases()
CHUNK = 25


@pytest.fixture(scope="module")
def results(aoo_offices):
    """{case id: record}, from the worker in chunks, so a crash costs one chunk and the office is restarted for the next."""
    session, out = aoo_offices.get(None), {}
    for i in range(0, len(CASES), CHUNK):
        chunk = CASES[i:i + CHUNK]
        try:
            res = session.call("calc_eval", {"cases": [{k: c[k] for k in ("id", "formula", "mode")} for c in chunk]}, timeout=300)
        except WorkerFailure as e:
            out.update({c["id"]: {"id": c["id"], "exception": "%s: %s" % (type(e).__name__, e)} for c in chunk})
            continue
        out.update({r["id"]: r for r in res["cases"]})
        out.update({c["id"]: {"id": c["id"], "exception": "not evaluated: the batch stopped at %s" % res["aborted_at"]}
                    for c in chunk if c["id"] not in out})
    return out


@pytest.mark.parametrize("case", CASES, ids=[c["id"] for c in CASES])
def test_formula_result(results, case):
    rec = results[case["id"]]
    assert "exception" not in rec, rec["exception"]
    assert calcval.judge(case["expected"], rec["result"]) in ("match", "record"), \
        "%s: expected %s, got %s" % (case["formula"], case["expected"], calcval.summary(rec["result"]))
