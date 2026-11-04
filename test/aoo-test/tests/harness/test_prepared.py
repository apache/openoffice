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

"""aootest.documents.Prepared: a script builder runs in an office of its own that is closed afterwards, and one that stalls is run
once more in a directory of its own; other builders and other failures are not retried."""
import pytest

from aootest import documents
from aootest.client import WorkerCrash, WorkerError, WorkerHang

pytestmark = pytest.mark.selftest

SCRIPT = documents.Source("built/base/base-hsqldb.odb", None, "odb", None, ("base", "base-hsqldb.odb"))
SMALL = documents.Source("built/page-fill/page-fill.odt", None, "odt", None, ("page-fill", "page-fill.odt"))


class FakeSession:
    def __init__(self, outcomes):
        self.outcomes, self.calls, self.closed = list(outcomes), [], 0

    def call(self, op, args, timeout):
        self.calls.append((op, args["builder"], args["outdir"], timeout))
        outcome = self.outcomes.pop(0)
        if isinstance(outcome, Exception):
            raise outcome
        return {"files": []}

    def close(self):
        self.closed += 1


def prepared(tmp_path, shared, own):
    return documents.Prepared(shared, tmp_path, lambda builder: own)


def test_a_script_builder_runs_in_its_own_office_which_is_closed_afterwards(tmp_path):
    shared, own = FakeSession([]), FakeSession([None])
    path = prepared(tmp_path, shared, own).file(SCRIPT)
    assert shared.calls == [] and len(own.calls) == 1 and own.closed == 1
    assert path == str(tmp_path / "base" / "out" / "base-hsqldb.odb")


def test_a_stalled_script_builder_is_run_once_more_in_another_directory(tmp_path):
    own = FakeSession([WorkerHang("no answer"), None])
    path = prepared(tmp_path, FakeSession([]), own).file(SCRIPT)
    assert [c[2] for c in own.calls] == [str(tmp_path / "base" / "out"), str(tmp_path / "base" / "out-retry")]
    assert own.closed == 2
    assert path == str(tmp_path / "base" / "out-retry" / "base-hsqldb.odb")


def test_a_script_builder_that_stalls_twice_fails_every_time_without_a_third_try(tmp_path):
    own = FakeSession([WorkerHang("first"), WorkerHang("second")])
    prep = prepared(tmp_path, FakeSession([]), own)
    for _ in range(2):
        with pytest.raises(WorkerHang):
            prep.file(SCRIPT)
    assert len(own.calls) == 2


@pytest.mark.parametrize("error", [WorkerCrash("died"), WorkerError("uno", "bad")])
def test_a_crash_or_an_error_is_not_retried(tmp_path, error):
    own = FakeSession([error])
    with pytest.raises(type(error)):
        prepared(tmp_path, FakeSession([]), own).file(SCRIPT)
    assert len(own.calls) == 1


def test_a_small_builder_runs_in_the_shared_office_and_is_not_retried(tmp_path):
    shared, own = FakeSession([WorkerHang("no answer")]), FakeSession([])
    with pytest.raises(WorkerHang):
        prepared(tmp_path, shared, own).file(SMALL)
    assert len(shared.calls) == 1 and own.calls == []
