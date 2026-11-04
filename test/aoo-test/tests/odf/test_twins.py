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

"""Import twins (see aootest.twins): documents saved as ODF 1.2 and as ODF 1.3 from the same source by another producer, in the
plain and the extended flavour. Every document must load, and the two members of a pair must import alike. Two controls show
that the comparison can fail: an unchanged copy compares equal and a copy with one word changed compares different.

Needs a corpus (--twin-corpus or AOO_TWIN_CORPUS); without one the items skip. Differences that are understood are rows
in data/expect/twins.tsv."""
import os

import pytest

from aootest import twins

pytestmark = pytest.mark.slow


def _root(config):
    return config.getoption("--twin-corpus") or os.environ.get("AOO_TWIN_CORPUS")


def pytest_generate_tests(metafunc):
    root = _root(metafunc.config)
    usable = root and twins.corpus_problem(root) is None
    names = twins.document_names(root) if usable else ["-"]
    if "name" in metafunc.fixturenames:
        if "pair" in metafunc.fixturenames:
            params = [(p, n) for p in twins.PAIRS for n in names]
            ids = ["%s-%s|%s" % (p[0], p[1], n) for p, n in params]
            metafunc.parametrize("pair,name", params, ids=ids)
        elif "mode" in metafunc.fixturenames:
            params = [(m, n) for m in twins.MODES for n in names]
            metafunc.parametrize("mode,name", params, ids=["%s|%s" % x for x in params])


def test_document_loads(twin_run, mode, name):
    d = twin_run.load(mode, name)
    assert not d.get("error") and not d.get("body_error"), d.get("error") or d.get("body_error")


def test_twins_import_alike(twin_run, pair, name):
    r = twin_run.diff(pair)[name]
    assert r["ndiff"] == 0, "%d paths differ, first: %s" % (r["ndiff"], r["diffs"][0][:3])


def test_control_unchanged_copy_compares_equal(twin_run):
    res, what = twin_run.controls()
    assert res is not None, what
    assert res["same"] == 0, "an unchanged copy of %s compares different (%d paths)" % (what, res["same"])


def test_control_changed_word_compares_different(twin_run):
    res, what = twin_run.controls()
    assert res is not None, what
    assert res["mutated"] > 0, "a copy of %s with a changed word compares equal: the comparison cannot fail" % what
