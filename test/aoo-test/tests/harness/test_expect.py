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

"""aootest.expect: reading expectation rows and matching test names."""
import pytest

from aootest import expect


def rows(tmp_path, text):
    (tmp_path / "x.tsv").write_text(text, encoding="utf-8")
    return expect.load(tmp_path)


def test_rows_of_each_kind_are_read(tmp_path):
    r = rows(tmp_path, "# comment\n\na[1]\txfail\twhy\nb\tflaky\tsometimes\nc\tskip\tnot here\n")
    assert [(x.pattern, x.kind, x.reason) for x in r] == [("a[1]", "xfail", "why"), ("b", "flaky", "sometimes"), ("c", "skip", "not here")]


@pytest.mark.parametrize("line", ["a\tmaybe\twhy\n", "a\txfail\t\n", "a\txfail\n", "a\txfail\twhy\textra\n"])
def test_malformed_rows_are_refused(tmp_path, line):
    with pytest.raises(ValueError):
        rows(tmp_path, line)


def test_star_matches_any_characters_and_brackets_are_literal(tmp_path):
    r = rows(tmp_path, "test_x[*|name.ods]\txfail\twhy\n")
    assert expect.match(r, "test_x[v12-v13|name.ods]") is r[0]
    assert expect.match(r, "test_x[v12-v13|nameXods]") is None
    assert expect.match(r, "test_y[v12-v13|name.ods]") is None
