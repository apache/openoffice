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

"""formula-cases.tsv: every row gives exactly the expected findings from formulacheck.check_stream()."""
import collections
import functools
from xml.sax.saxutils import quoteattr

import pytest

from aootest.checkers import formulacheck
from aootest.paths import DATA

pytestmark = pytest.mark.selftest

CASES = [l.rstrip("\n").split("\t") for l in open(DATA / "formula-cases.tsv", encoding="utf-8") if not l.startswith("#") and l.strip()]
NSDECL = ('xmlns:office="urn:oasis:names:tc:opendocument:xmlns:office:1.0" xmlns:table="urn:oasis:names:tc:opendocument:xmlns:table:1.0" '
          'xmlns:style="urn:oasis:names:tc:opendocument:xmlns:style:1.0" xmlns:text="urn:oasis:names:tc:opendocument:xmlns:text:1.0" '
          'xmlns:of="urn:oasis:names:tc:opendocument:xmlns:of:1.2" xmlns:oooc="http://openoffice.org/2004/calc" xmlns:ooow="http://openoffice.org/2004/writer"')


@functools.cache
def findings():
    """All cases in one document; row n is on line n + 2."""
    lines = ["<office:document-content %s><office:body>" % NSDECL]
    for st, attr, val, msg in CASES:
        val = val.replace("\\n", "\n").replace("\\u00a0", " ")
        name, _, ctx = attr.partition("@")
        tag = "office:text" if ctx == "text" else "office:spreadsheet"
        lines.append("<%s><table:table-cell %s=%s/></%s>" % (tag, name, quoteattr(val, {"\n": "&#10;"}), tag))
    lines.append("</office:body></office:document-content>")
    got = collections.defaultdict(list)
    for line, col, level, rule, m in formulacheck.check_stream("\n".join(lines).encode("utf-8"), collections.Counter()):
        got[line - 2].append((level + ":" + rule, m))
    return got


@pytest.mark.parametrize("n", range(len(CASES)), ids=lambda n: "%03d %s %s" % (n, CASES[n][1], CASES[n][2][:40].replace("\\n", " ")))
def test_formula_case(n):
    st, attr, val, msg = CASES[n]
    want = [] if st == "ok" else st.split("+")
    got = findings()[n]
    assert sorted(r for r, _ in got) == sorted(want), (attr, val, got)
    assert not msg or any(msg in m for _, m in got), (attr, val, msg, got)


def test_formula_case_table_is_not_empty():
    assert len(CASES) > 100
