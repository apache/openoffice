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
"""The extended save modes must not get worse: the error kinds against the ODF 1.2 and 1.3 schemas (and the same with foreign
content removed) are compared with data/baseline/. Regenerate the baseline with --write-kinds after a change that is meant to
change them, and read the diff of data/baseline/ before committing it."""
import pytest

from aootest import documents, kinds

pytestmark = pytest.mark.slow


@pytest.mark.parametrize("mode,version", kinds.PAIRS)
def test_error_kinds_do_not_get_worse(odf_runs, need_mode, pytestconfig, mode, version):
    need_mode(mode)
    sources = kinds.stable(documents.from_config(pytestconfig))
    run = odf_runs(mode)
    saved = run.saved()
    val = run.validation()["val"]
    if val is None:
        pytest.skip("needs java, jing and the ODF schemas (python -m aootest.fetch)")
    covered = {documents.out_name(s) for s in sources if saved[s.id]["out"]}
    now = kinds.kinds_of(val, version, covered)
    if pytestconfig.getoption("--write-kinds"):
        unsaved = [s.id for s in sources if not saved[s.id]["out"]]
        kinds.write(mode, version, covered, now)
        pytest.skip("baseline written: %s%s" % (kinds.path(mode, version).name, "; not saved, so not in it: " + ", ".join(unsaved) if unsaved else ""))
    base = kinds.load(mode, version)
    if base is None:
        pytest.skip("no baseline for %s against ODF %s.%s (python3 -m pytest tests/odf/test_kinds.py --write-kinds)" % (mode, version[0], version[1:]))
    lines, bad = kinds.judge(base, covered, now)
    assert not bad, "\n".join(lines)
