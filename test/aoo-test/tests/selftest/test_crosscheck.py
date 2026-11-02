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

"""crosscheck.check(): odfvalidator on the negative controls, compared with jing."""
import pytest

from aootest import tools
from aootest.checkers import crosscheck

pytestmark = [pytest.mark.selftest, pytest.mark.needs_java]


@pytest.fixture(scope="module")
def cross(val, odf_controls, schema_dir, tmp_path_factory):
    jar, java = tools.odfvalidator(), tools.java()
    if jar is None or java is None:
        pytest.skip("needs java and the odfvalidator jar (set AOO_ODFVALIDATOR or run: python -m aootest.fetch)")
    return crosscheck.check(odf_controls, val, tmp_path_factory.mktemp("cross"), jar, schema_dir, java)


ROWS = [
    ("odfvalidator flags bad.odt in both modes", lambda c, e: all("bad.odt/content.xml" in m["odfvalidator_error_streams"] for m in (c, e))),
    ("odfvalidator flags extforeign.odt as written but not as an extended document",
     lambda c, e: "extforeign.odt/content.xml" in c["odfvalidator_error_streams"] and "extforeign.odt/content.xml" not in e["odfvalidator_error_streams"]),
    ("odfvalidator finds the duplicate xml:id that jing -i cannot", lambda c, e: "dupid.odt/content.xml" in c["odfvalidator_error_streams"]),
    ("good.odt clean in odfvalidator", lambda c, e: not any("good.odt/" in k for m in (c, e) for k in m["odfvalidator_error_streams"])),
    ("no stream where only jing reports errors", lambda c, e: not c["only_jing"] and not e["only_jing"]),
]


@pytest.mark.parametrize("row", [pytest.param(r[1], id=r[0]) for r in ROWS])
def test_crosscheck_controls(row, cross):
    assert row(cross["conformance"], cross["extended"])
