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

"""idcheck.check() on the negative controls."""
import pytest

pytestmark = pytest.mark.selftest


def ierrs(ids, pkg):
    return [e[3] for e in ids["errors"] if e[0].startswith(pkg + "/")]


ROWS = [
    ("good.odt, goodid.odt (normalized ids) clean", lambda I: not I("good.odt") and not I("goodid.odt")),
    ("dupid.odt flags the duplicate after normalization", lambda I: any("duplicates" in m for m in I("dupid.odt"))),
    ("badid.odt, badid2.odt flag non-NCName ids", lambda I: all(any("not an NCName" in m for m in I(p)) for p in ("badid.odt", "badid2.odt"))),
    ("badns.odt flags the unbound prefix", lambda I: any("unbound prefix" in m for m in I("badns.odt"))),
    ("badsig.odt flags a duplicate id in a signature file", lambda I: any("duplicates" in m for m in I("badsig.odt"))),
]


@pytest.mark.parametrize("row", [pytest.param(r[1], id=r[0]) for r in ROWS])
def test_idcheck_controls(row, ids):
    assert row(lambda pkg: ierrs(ids, pkg))


def test_idcheck_no_errors_in_other_packages(ids):
    assert {e[0].split("/")[0] for e in ids["errors"]} == {"dupid.odt", "badid.odt", "badid2.odt", "badns.odt", "badsig.odt"}
