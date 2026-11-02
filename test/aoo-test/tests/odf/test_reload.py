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

"""Documents the worker builds (aootest_worker.ops.generate) saved in each ODF save mode and loaded again: the values the
checks in data/checks/ read from the saved copy must be the ones recorded for them. Differences between save modes that are
known are rows in data/expect/reload.tsv."""
import pytest

from aootest import documents
from aootest.odfversion import SAVE_MODES

pytestmark = pytest.mark.slow

BUILT = [s for s in documents.generated() if s.checks]
RELOAD = [(m, s.id, f, c["name"]) for m in SAVE_MODES for s in BUILT for f in s.checks for c in documents.load_checks(f)]


@pytest.mark.parametrize("mode,source_id,checks,name", RELOAD, ids=["%s|%s|%s" % (m, i, n) for m, i, f, n in RELOAD])
def test_reload_check(odf_runs, need_mode, mode, source_id, checks, name):
    need_mode(mode)
    run = odf_runs(mode)
    saved = run.saved()[source_id]
    assert saved["out"], "not saved: %s" % saved["detail"]
    got = run.reload_checks()[source_id]
    assert "__error__" not in got, got.get("__error__")
    expected = {c["name"]: c["expect"] for c in documents.load_checks(checks)}[name]
    assert got[name] == expected
