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

"""The worker's dump_document op: it reports the document type and the content that the twin tests compare, and leaves no
document open."""
import pytest

pytestmark = pytest.mark.smoke


@pytest.fixture
def office(aoo_offices):
    return aoo_offices.get(None)


def stored(office, tmp_path, app, filter, name, text):
    path = tmp_path / name
    office.call("new_and_store", {"app": app, "dst": str(path), "filter": filter, "text": text}, timeout=120)
    return path


def test_writer_dump_has_the_text(office, tmp_path):
    d = office.call("dump_document", {"path": str(stored(office, tmp_path, "writer", "writer8", "d.odt", "Hello Dump"))}, timeout=120)
    assert d["type"] == "writer" and d["body"]["chars"] == len("Hello Dump") and "body_error" not in d


def test_calc_dump_has_the_cell(office, tmp_path):
    d = office.call("dump_document", {"path": str(stored(office, tmp_path, "calc", "calc8", "d.ods", "Hello Dump"))}, timeout=120)
    assert d["type"] == "calc" and d["body"]["sheets"][0]["ncells"] == 1


def test_different_text_gives_a_different_dump(office, tmp_path):
    a = office.call("dump_document", {"path": str(stored(office, tmp_path, "writer", "writer8", "a.odt", "Hello Dump"))}, timeout=120)
    b = office.call("dump_document", {"path": str(stored(office, tmp_path, "writer", "writer8", "b.odt", "Hello Dumps"))}, timeout=120)
    assert a["body"]["text_hash"] != b["body"]["text_hash"]


def test_dump_leaves_nothing_open(office, tmp_path):
    office.call("dump_document", {"path": str(stored(office, tmp_path, "writer", "writer8", "c.odt", "x"))}, timeout=120)
    assert office.call("open_documents")["documents"] == []
