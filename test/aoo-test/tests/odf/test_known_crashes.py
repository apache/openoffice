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

"""Known crashes of the office under test, kept out of the default run: a crash makes macOS write a report and slows every
office start that follows, so these run on request (python3 -m pytest -m crash). A row in data/expect/crash.tsv records each
one; with --strict-expect the item fails when the crash is gone, which is the cue to delete the row."""
import pytest

pytestmark = [pytest.mark.slow, pytest.mark.crash]


def test_paragraph_bitmap_fill_mode_does_not_kill_the_office(aoo_offices, tmp_path):
    """Building para-fill-foreign sets FillBitmapMode on Writer paragraphs; the worker's generate op must come back."""
    session = aoo_offices.get(3)
    res = session.call("generate", {"builder": "para-fill-foreign", "outdir": str(tmp_path)}, timeout=300)
    assert res["files"] == ["aoo-form.odt"]
