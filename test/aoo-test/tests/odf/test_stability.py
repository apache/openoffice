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

"""Saving the same document twice in the same office must give the same document content: content.xml and styles.xml are
identical apart from the cached value of date and time fields. meta.xml (dates, editing duration) and the package bytes are
allowed to differ. Documents with other volatile content (NOW, TODAY or RAND formulas) are rows in data/expect/stability.tsv."""
import pytest

from aootest import stability

pytestmark = pytest.mark.slow


@pytest.mark.parametrize("mode", ["strict", "ext", "v13", "x13", "v14", "x14"])
def test_saving_twice_gives_the_same_content(saved, mode, source):
    run, first = saved(mode, source)
    assert first["out"], "not saved: %s" % first["detail"]
    second = run.saved_again()[source.id]
    assert second["out"], "not saved the second time: %s" % second["detail"]
    differ = stability.differing_parts(first["out"], second["out"])
    assert not differ, "differs between two saves: %s" % ", ".join(differ)
