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

"""Against a real office (AOO_OFFICE or --office). Starts one private-profile office per ODF mode."""
import pytest

from aootest.client import WorkerCrash, WorkerHang
from aootest.odfversion import MODES, odf_versions
from aootest.proc import kill_tree

pytestmark = pytest.mark.smoke

APPS = {"writer": ("writer8", "odt", "com.sun.star.text.TextDocument"),
        "calc": ("calc8", "ods", "com.sun.star.sheet.SpreadsheetDocument"),
        "impress": ("impress8", "odp", "com.sun.star.presentation.PresentationDocument"),
        "draw": ("draw8", "odg", "com.sun.star.drawing.DrawingDocument")}


def test_product_info_matches_the_install(aoo_offices, aoo_layout):
    info = aoo_offices.get(4).call("product_info")
    assert info["ooSetupVersion"] == aoo_layout.version.package
    assert info["ooName"] == "OpenOffice"


@pytest.mark.parametrize("name", [v[0] for v in APPS.values()])
def test_native_filters_import_and_export(aoo_offices, name):
    f = aoo_offices.get(4).call("filters", {"names": [name]})[name]
    assert f is not None and f["import"] and f["export"]


@pytest.mark.parametrize("mode", MODES)
def test_mode_is_applied_by_the_profile(aoo_offices, mode):
    value = aoo_offices.get(mode).call("config_value", {"path": "/org.openoffice.Office.Common/Save/ODF", "name": "DefaultVersion"})
    assert value == mode


@pytest.mark.parametrize("app", APPS)
@pytest.mark.parametrize("mode", MODES)
def test_new_store_version_reload(aoo_offices, need_mode, tmp_path, app, mode):
    need_mode(mode)
    filter_name, ext, service = APPS[app]
    dst = tmp_path / ("new." + ext)
    s = aoo_offices.get(mode)
    s.call("new_and_store", {"app": app, "dst": str(dst), "filter": filter_name}, timeout=120)
    assert odf_versions(dst) == (MODES[mode], MODES[mode])
    info = s.call("load_info", {"path": str(dst)}, timeout=120)
    assert info["loaded"] and service in info["services"]


def test_default_mode_writes_some_known_version(aoo_offices, tmp_path):
    dst = tmp_path / "new.odt"
    aoo_offices.get(None).call("new_and_store", {"app": "writer", "dst": str(dst), "filter": "writer8"}, timeout=120)
    content, manifest = odf_versions(dst)
    assert content == manifest and content in ("1.2", "1.3")


def test_hang_is_reported_and_the_next_call_works(aoo_offices):
    s = aoo_offices.get(4)
    with pytest.raises(WorkerHang):
        s.call("sleep", {"seconds": 60}, timeout=3)
    assert s.call("ping", timeout=120) == {"pong": True}


def test_worker_crash_is_reported_and_the_next_call_works(aoo_offices):
    s = aoo_offices.get(4)
    with pytest.raises(WorkerCrash):
        s.call("die", timeout=30)
    assert s.call("ping", timeout=120) == {"pong": True}


def test_office_killed_mid_session_is_recovered(aoo_offices):
    s = aoo_offices.get(5)
    s.call("ping", timeout=120)
    kill_tree(s.office._proc)
    with pytest.raises((WorkerCrash, WorkerHang)):
        s.call("filters", {"names": ["writer8"]}, timeout=30)
    assert s.call("filters", {"names": ["writer8"]}, timeout=120)["writer8"]["export"]
