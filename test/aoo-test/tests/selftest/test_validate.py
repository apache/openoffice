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

"""validate.check() on the negative controls: each row must hold, so a validator that stops flagging a control fails here."""
import pytest

pytestmark = pytest.mark.selftest

BOGUS = '"text:bogus"'


def flags(E, versions, pkg, needle):
    return all(any(needle in m for m in E(v, pkg)) for v in versions)


ROWS = [
    ("good.odt clean in 1.2 and 1.4", lambda E, d: not E("12", "good.odt") and not E("14", "good.odt")),
    ("bad.odt flags text:bogus in 1.2 and 1.4", lambda E, d: flags(E, ("12", "14"), "bad.odt", BOGUS)),
    ("wrongver.odt flags office:version in 1.2 and 1.4", lambda E, d: flags(E, ("12", "14"), "wrongver.odt", "office:version")),
    ("13: good.odt clean in 1.3 and 1.3 extended", lambda E, d: not E("13", "good.odt") and not E("13x", "good.odt")),
    ("13: bad.odt flags text:bogus in 1.3 and 1.3 extended", lambda E, d: flags(E, ("13", "13x"), "bad.odt", BOGUS)),
    ("13: wrongver.odt flags office:version in 1.3 and 1.3 extended", lambda E, d: flags(E, ("13", "13x"), "wrongver.odt", "office:version")),
    ("13: new13.odt (text:drop-down) invalid in 1.2, valid in 1.3 and 1.3 extended",
     lambda E, d: bool(E("12", "new13.odt")) and not E("13", "new13.odt") and not E("13x", "new13.odt")),
    ("13: extforeign.odt invalid as written, valid once foreign content is removed",
     lambda E, d: bool(E("13", "extforeign.odt")) and not E("13x", "extforeign.odt")),
    ("13: meta:creator-initials invalid in 1.2, valid in 1.3 and 1.3 extended; the note without it is clean in all",
     lambda E, d: any('"meta:creator-initials"' in m for m in E("12", "newinit.odt")) and not E("13", "newinit.odt") and not E("13x", "newinit.odt")
     and not any(E(v, "baseinit.odt") for v in ("12", "13", "13x"))),
    ("13: chart:coordinate-region invalid in 1.2, valid in 1.3 and 1.3 extended; the chart without it is clean in all",
     lambda E, d: any('"chart:coordinate-region"' in m for m in E("12", "newregion.odc")) and not E("13", "newregion.odc") and not E("13x", "newregion.odc")
     and not any(E(v, "baseregion.odc") for v in ("12", "13", "13x"))),
    ("13: style:scale-to-X/Y together with style:scale-to invalid in 1.3 and 1.3 extended",
     lambda E, d: flags(E, ("13", "13x"), "scaleboth.odt", '"style:scale-to"')),
    ("13: style:scale-to-X/Y alone and style:scale-to alone valid in 1.3 and 1.3 extended; X/Y invalid in 1.2",
     lambda E, d: not any(E(v, p) for v in ("13", "13x") for p in ("scalexy.odt", "scaleone.odt"))
     and any('"style:scale-to-X"' in m for m in E("12", "scalexy.odt")) and not E("12", "scaleone.odt")),
    ("14: good.odt clean in 1.4 and 1.4 extended; a 1.3 version is rewritten like a 1.2 one", lambda E, d: not E("14", "good.odt") and not E("14x", "good.odt")),
    ("14: bad.odt flags text:bogus in 1.4 and 1.4 extended", lambda E, d: flags(E, ("14", "14x"), "bad.odt", BOGUS)),
    ("14: wrongver.odt (1.1) flags office:version in 1.4 and 1.4 extended", lambda E, d: flags(E, ("14", "14x"), "wrongver.odt", "office:version")),
    ("14: extforeign.odt invalid as written, valid in 1.4 extended once foreign content is removed",
     lambda E, d: bool(E("14", "extforeign.odt")) and not E("14x", "extforeign.odt")),
    ("14: draw:decorative invalid in 1.2 and 1.3, valid in 1.4 and 1.4 extended; the same style without it is clean in all",
     lambda E, d: all(any('"draw:decorative"' in m for m in E(v, "newdeco.odt")) for v in ("12", "13"))
     and not E("14", "newdeco.odt") and not E("14x", "newdeco.odt") and not any(E(v, "basedeco.odt") for v in ("12", "13", "14", "14x"))),
    ("14: draw:handle with only the old combined attribute valid in 1.2 and 1.3, invalid in 1.4 and 1.4 extended",
     lambda E, d: not any(E(v, "handleold.odt") for v in ("12", "13")) and all(E(v, "handleold.odt") for v in ("14", "14x"))),
    ("14: draw:handle with the new position or polar pair valid in 1.4 and 1.4 extended, invalid in 1.2 and 1.3",
     lambda E, d: not any(E(v, p) for v in ("14", "14x") for p in ("handlenew.odt", "handlepolar.odt"))
     and all(E(v, p) for v in ("12", "13") for p in ("handlenew.odt", "handlepolar.odt"))),
    ("mathobj.odt MathML object skipped", lambda E, d: d["mathml_skipped"] == ["mathobj.odt/Object 1/content.xml"] and not E("12", "mathobj.odt")),
    ("no unparsed jing output", lambda E, d: not any(e[0] == "?" for v in d["errors"] for e in d["errors"][v])),
    ("extended: extforeign.odt invalid as written, valid once foreign content is removed",
     lambda E, d: bool(E("12", "extforeign.odt")) and not E("12x", "extforeign.odt")),
    ("extended: good.odt, goodid.odt clean", lambda E, d: not E("12x", "good.odt") and not E("12x", "goodid.odt")),
    ("extended: bad.odt (unknown element in an ODF namespace) still flagged", lambda E, d: any(BOGUS in m for m in E("12x", "bad.odt"))),
    ("extended: wrongver.odt (attribute in an ODF namespace) still flagged", lambda E, d: any("office:version" in m for m in E("12x", "wrongver.odt"))),
    ("extended: badns.odt (unbound prefix) still flagged", lambda E, d: any("not bound" in m for m in E("12x", "badns.odt"))),
]


@pytest.mark.parametrize("row", [pytest.param(r[1], id=r[0]) for r in ROWS])
def test_validate_controls(row, errs, val):
    assert row(errs, val)
