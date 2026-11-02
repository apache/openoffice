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

"""aootest.kinds: grouping of schema errors into kinds, and what counts as worse than a baseline."""
import pytest

from aootest import kinds

pytestmark = pytest.mark.selftest


def entry(package, message, part="content.xml"):
    return ["%s/%s" % (package, part), 2, 10, message]


VAL = {"errors": {"12": [entry("a.odt", 'element "text:bogus" not allowed here; expected element "text:p"'),
                         entry("b.odt", 'element "text:bogus" not allowed here; expected element "text:h"'),
                         entry("b.odt", 'attribute "x:y" not allowed here'),
                         entry("other.odt", 'attribute "x:y" not allowed here')]}}


def test_errors_are_grouped_by_message_without_the_expected_tail_and_limited_to_the_packages():
    got = kinds.kinds_of(VAL, "12", {"a.odt", "b.odt"})
    assert got == {'element "text:bogus" not allowed here': {"errors": 2, "packages": ["a.odt", "b.odt"]},
                   'attribute "x:y" not allowed here': {"errors": 1, "packages": ["b.odt"]}}


def test_jing_failures_that_name_the_work_directory_are_one_kind_whatever_the_directory():
    a = {"errors": {"12": [["?", 0, 0, "file not found: /tmp/one/w-validate/12/a.odt/META-INF/Manifest.dtd"]]}}
    b = {"errors": {"12": [["?", 0, 0, "file not found: /var/x/two/w-validate/12/a.odt/META-INF/Manifest.dtd"]]}}
    # the package of such an entry is "?", so name it
    assert kinds.kinds_of(a, "12", {"?"}) == kinds.kinds_of(b, "12", {"?"})
    assert list(kinds.kinds_of(a, "12", {"?"})) == ["UNPARSED: file not found: <workdir>/12/a.odt/META-INF/Manifest.dtd"]


BASE = {"documents": ["a.odt", "b.odt"], "kinds": {"k1": {"errors": 3, "packages": ["a.odt", "b.odt"]}, "k2": {"errors": 1, "packages": ["b.odt"]}}}


def test_the_same_kinds_are_not_worse():
    assert kinds.judge(BASE, ["a.odt", "b.odt"], BASE["kinds"]) == ([], False)


@pytest.mark.parametrize("change", [
    lambda k: {**k, "k3": {"errors": 1, "packages": ["a.odt"]}},
    lambda k: {**k, "k1": {"errors": 4, "packages": ["a.odt", "b.odt"]}},
    lambda k: {**k, "k2": {"errors": 1, "packages": ["a.odt", "b.odt"]}},
])
def test_a_new_kind_or_a_higher_count_or_another_package_is_worse(change):
    lines, bad = kinds.judge(BASE, ["a.odt", "b.odt"], change(BASE["kinds"]))
    assert bad and any(l.startswith("WORSE") for l in lines)


def test_fewer_errors_are_reported_but_not_worse():
    lines, bad = kinds.judge(BASE, ["a.odt", "b.odt"], {"k1": {"errors": 2, "packages": ["a.odt"]}})
    assert not bad and lines and all(l.startswith("better") for l in lines)


def test_a_baseline_document_that_is_no_longer_covered_is_worse():
    lines, bad = kinds.judge(BASE, ["a.odt"], BASE["kinds"])
    assert bad and lines == ["NOT COVERED  b.odt"]


def test_a_written_baseline_reads_back(tmp_path, monkeypatch):
    monkeypatch.setattr(kinds, "BASELINE", tmp_path / "baseline")
    kinds.write("ext", "12", {"b.odt", "a.odt"}, BASE["kinds"])
    assert kinds.load("ext", "12") == BASE
    assert kinds.load("ext", "13") is None
