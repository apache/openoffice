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

"""doctypecheck.check() must report exactly the (level, rule) pairs each control document was built to trigger."""
import pytest

from aootest import controls

pytestmark = pytest.mark.selftest

EXPECTED = controls.make_doctype_controls(None)


def found(result, doc):
    msgs = (e[3] for e in result["errors"] if e[0].startswith(doc + "/"))
    return sorted({tuple(m.split("]")[0].split(": [")) for m in msgs})


@pytest.mark.parametrize("doc", sorted(EXPECTED), ids=lambda d: "%s reports exactly %s" % (d, ", ".join(r for _, r in EXPECTED[d]) or "nothing"))
def test_doctypecheck_control(doc, doctype_result):
    assert found(doctype_result, doc) == [tuple(x) for x in EXPECTED[doc]]


def test_doctypecheck_table_is_not_empty():
    assert len(EXPECTED) > 30
