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
#
################################################################

"""The scenarios of test/smoketestdoc in the openoffice tree, run as UNO steps in the office at its default save mode."""
import pytest

from aootest import oxt

pytestmark = pytest.mark.smoke

# app -> (factory, [(label, filter, extension)])
DOC_APPS = {
    "writer": ("private:factory/swriter", [("8.0", "writer8", "odt"), ("XML", "StarOffice XML (Writer)", "sxw")]),
    "calc": ("private:factory/scalc", [("8.0", "calc8", "ods"), ("XML", "StarOffice XML (Calc)", "sxc")]),
    "impress": ("private:factory/simpress", [("8.0", "impress8", "odp"), ("XML", "StarOffice XML (Impress)", "sxi")]),
    "draw": ("private:factory/sdraw", [("8.0", "draw8", "odg"), ("XML", "StarOffice XML (Draw)", "sxd")]),
    "html": ("private:factory/swriter/web", [("HTML", "HTML", "html")]),
}


def assert_steps(result):
    failed = ["%s (%s)" % (s["name"], s["detail"]) for s in result["steps"] if not s["ok"]]
    assert result["steps"], "no steps were run"
    assert not failed, "failed steps: " + "; ".join(failed)


@pytest.fixture
def office(aoo_offices):
    return aoo_offices.get(None)


@pytest.mark.parametrize("app", DOC_APPS)
def test_new_save_close_reopen(office, tmp_path, app):
    factory, formats = DOC_APPS[app]
    result = office.call("doc_cycle", {"factory": factory, "formats": formats, "workdir": str(tmp_path)}, timeout=180)
    assert_steps(result)
    names = [s["name"] for s in result["steps"]]
    assert names[0] == "new" and "close" in names and any(n.startswith("open ") for n in names)


def test_chart_in_a_spreadsheet(office):
    assert_steps(office.call("chart", timeout=120))


def test_math_new_and_close(office):
    assert_steps(office.call("math", timeout=120))


def test_java_loader_service(office):
    assert_steps(office.call("java_loader", timeout=120))


def test_database_insert_delete_seek(office):
    assert_steps(office.call("database", timeout=180))


def test_extension_install_and_uninstall(office, tmp_path):
    path = oxt.build(tmp_path)
    result = office.call("extension", {"path": str(path), "identifier": oxt.IDENTIFIER, "service": oxt.SERVICE,
                                       "implementation": oxt.IMPLEMENTATION}, timeout=240)
    assert_steps(result)
