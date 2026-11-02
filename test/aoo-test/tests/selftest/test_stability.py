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

"""aootest.stability: two saves that differ only in the cached value of a date or time field compare equal, and any other
difference does not."""
import zipfile

import pytest

from aootest import stability

pytestmark = pytest.mark.selftest

BODY = '<office:text><text:p>Hello <text:time text:time-value="2026-11-02T07:47:54" text:fixed="false">07:47:54</text:time></text:p></office:text>'


def package(path, content, styles="<office:document-styles/>"):
    with zipfile.ZipFile(path, "w") as z:
        z.writestr("content.xml", content)
        if styles is not None:
            z.writestr("styles.xml", styles)
    return str(path)


def test_normalize_drops_the_cached_time_and_the_value_attribute():
    other = BODY.replace("07:47:54", "07:48:00")
    assert stability.normalize(BODY) == stability.normalize(other)
    assert "07:47:54" not in stability.normalize(BODY)


def test_two_packages_differing_only_in_a_time_field_compare_equal(tmp_path):
    a = package(tmp_path / "a.zip", BODY)
    b = package(tmp_path / "b.zip", BODY.replace("07:47:54", "09:00:00"))
    assert stability.differing_parts(a, b) == []


@pytest.mark.parametrize("change", [lambda t: t.replace("Hello", "Hullo"), lambda t: t.replace('text:fixed="false"', 'text:fixed="true"'),
                                    lambda t: t.replace("<text:p>", "<text:p text:style-name=\"P1\">")])
def test_any_other_difference_is_reported(tmp_path, change):
    a = package(tmp_path / "a.zip", BODY)
    b = package(tmp_path / "b.zip", change(BODY))
    assert stability.differing_parts(a, b) == ["content.xml"]


def test_a_difference_in_styles_xml_is_reported(tmp_path):
    a = package(tmp_path / "a.zip", BODY)
    b = package(tmp_path / "b.zip", BODY, "<office:document-styles>x</office:document-styles>")
    assert stability.differing_parts(a, b) == ["styles.xml"]


def test_a_part_neither_package_has_is_not_compared(tmp_path):
    a = package(tmp_path / "a.zip", BODY, None)
    b = package(tmp_path / "b.zip", BODY, None)
    assert stability.differing_parts(a, b) == []
    assert stability.differing_parts(a, package(tmp_path / "c.zip", BODY.replace("Hello", "Hullo"), None)) == ["content.xml"]


def test_a_part_only_one_package_has_is_reported(tmp_path):
    a = package(tmp_path / "a.zip", BODY, None)
    b = package(tmp_path / "b.zip", BODY)
    assert stability.differing_parts(a, b) == ["styles.xml"]
    assert stability.differing_parts(b, a) == ["styles.xml"]
