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

"""Against a real install, when AOO_OFFICE names one. Nothing here starts the office: it reads files and imports uno in a child Python."""
import os

import pytest

from aootest import install

pytestmark = [pytest.mark.smoke, pytest.mark.skipif(not os.environ.get("AOO_OFFICE"), reason="set AOO_OFFICE to an installed AOO 5 to run this")]


@pytest.fixture(scope="module")
def layout():
    return install.discover()


def test_layout_matches_the_version_file(layout):
    text = layout.version_file.read_text()
    assert "OOOBaseVersion=%s" % layout.version.base in text
    assert layout.soffice.is_file() and layout.fundamental.is_file() and layout.pyuno_module is not None
    assert layout.version.tuple >= install.MIN_VERSION


def test_a_python_can_import_uno(layout, tmp_path):
    try:
        py = install.find_uno_python(layout, cache_file=tmp_path / "uno.json")
    except install.NoUnoPython as e:
        pytest.skip("no UNO Python for this install: %s; tried %s" % (e, e.tried))
    assert os.path.exists(py.path) and tuple(int(x) for x in py.version.split(".")[:2]) >= install.MIN_WORKER_PYTHON
