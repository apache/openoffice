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

"""Every source document (see aootest.documents: the templates and documents the office ships, a new document of each
application, and any --documents directory) saved in each ODF save mode, then checked: it saved, it declares the right
version, the package and document-type rules hold, ids and formulas parse, and it validates against the schema its mode
promises.

Strict ODF 1.2 (mode 4) must validate against 1.2 and 1.3; 1.3 (mode 5) against 1.3. The extended modes (3, 6, and the
product default) carry AOO extensions on purpose, so their schema errors are not asserted here."""
import pytest

from aootest import documents
from aootest.odfversion import SAVE_MODES, odf_versions

pytestmark = pytest.mark.slow

MODE_NAMES = list(SAVE_MODES)
DECLARED = {"strict": "1.2", "ext": "1.2", "v13": "1.3", "x13": "1.3"}
SCHEMAS = [("strict", "12"), ("strict", "13"), ("strict", "14"), ("v13", "13")]


def saved_copy(odf_runs, need_mode, mode, source):
    """The ModeRun and the saved result of `source`; skips when the build lacks the mode."""
    need_mode(mode)
    run = odf_runs(mode)
    return run, run.saved()[source.id]


def shall(entries, package):
    return [e[3] for e in entries if e[0].split("/", 1)[0] == package and e[3].startswith("shall:")]


def own(entries, package):
    return [e for e in entries if e[0].split("/", 1)[0] == package]


@pytest.mark.parametrize("mode", MODE_NAMES)
class TestSavedCopy:
    def test_saves(self, odf_runs, need_mode, mode, source):
        _, r = saved_copy(odf_runs, need_mode, mode, source)
        assert r["status"] == "ok", "%s: %s" % (r["status"], r["detail"])

    def test_declares_the_mode_version(self, odf_runs, need_mode, mode, source):
        if mode not in DECLARED:
            pytest.skip("the product default is not pinned to one ODF version")
        _, r = saved_copy(odf_runs, need_mode, mode, source)
        assert r["out"], "not saved: %s" % r["detail"]
        assert odf_versions(r["out"]) == (DECLARED[mode], DECLARED[mode])

    def test_package_rules(self, odf_runs, need_mode, mode, source):
        run, r = saved_copy(odf_runs, need_mode, mode, source)
        assert r["out"], "not saved: %s" % r["detail"]
        res = run.validation()["pkg"]
        if res is None:
            pytest.skip("needs java, jing and the ODF schemas (python -m aootest.fetch)")
        assert shall(res["errors"], documents.out_name(source)) == []

    def test_document_type(self, odf_runs, need_mode, mode, source):
        run, r = saved_copy(odf_runs, need_mode, mode, source)
        assert r["out"], "not saved: %s" % r["detail"]
        assert shall(run.validation()["doctype"]["errors"], documents.out_name(source)) == []

    def test_xml_ids(self, odf_runs, need_mode, mode, source):
        run, r = saved_copy(odf_runs, need_mode, mode, source)
        assert r["out"], "not saved: %s" % r["detail"]
        assert own(run.validation()["ids"]["errors"], documents.out_name(source)) == []

    def test_formula_syntax(self, odf_runs, need_mode, mode, source):
        run, r = saved_copy(odf_runs, need_mode, mode, source)
        assert r["out"], "not saved: %s" % r["detail"]
        assert shall(run.validation()["formulas"]["errors"], documents.out_name(source)) == []


@pytest.mark.parametrize("mode,version", SCHEMAS)
def test_valid_against_odf_schema(odf_runs, need_mode, mode, version, source):
    run, r = saved_copy(odf_runs, need_mode, mode, source)
    assert r["out"], "not saved: %s" % r["detail"]
    val = run.validation()["val"]
    if val is None:
        pytest.skip("needs java, jing and the ODF schemas (python -m aootest.fetch)")
    errors = own(val["errors"][version], documents.out_name(source))
    assert not errors, "%d errors against ODF 1.%s, first: %s" % (len(errors), version[1], errors[0][:4])
