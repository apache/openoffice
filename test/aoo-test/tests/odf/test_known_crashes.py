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

from aootest import documents
from aootest.client import WorkerFailure
from aootest.odfversion import SAVE_MODES

pytestmark = [pytest.mark.slow, pytest.mark.crash]


def test_paragraph_bitmap_fill_mode_does_not_kill_the_office(aoo_offices, tmp_path):
    """Building para-fill-foreign sets FillBitmapMode on Writer paragraphs; the worker's generate op must come back."""
    session = aoo_offices.get(3)
    res = session.call("generate", {"builder": "para-fill-foreign", "outdir": str(tmp_path)}, timeout=300)
    assert res["files"] == ["aoo-form.odt"]


def test_draw_annotations_step_does_not_stall_the_office(aoo_offices, tmp_path):
    """Annotations on Impress and Draw pages: in a fresh trunk dev app the draw script's `annotations` step ran past its own 90 s limit."""
    session = aoo_offices.get(3, tag="build-draw-annotations")
    try:
        res = session.call("generate", {"builder": "draw-annotations", "outdir": str(tmp_path / "out")}, timeout=150)
    finally:
        session.close()
    assert res["files"] == ["annotations.odg", "annotations.odp"]


EXCLUDED = documents.script_excluded()


@pytest.fixture(scope="module")
def excluded_files(aoo_offices, tmp_path_factory):
    """{builder: output directory}, each builder run once in an office of its own."""
    made = {}

    def get(builder):
        if builder not in made:
            out = tmp_path_factory.mktemp(builder) / "out"
            session = aoo_offices.get(SAVE_MODES["ext"], tag="build-" + builder)
            try:
                session.call("generate", {"builder": builder, "outdir": str(out)}, timeout=documents.BUILD_TIMEOUTS.get(builder, documents.BUILD_TIMEOUT))
            finally:
                session.close()
            made[builder] = out
        return made[builder]
    return get


@pytest.mark.parametrize("builder,name,reason", EXCLUDED, ids=["%s/%s" % (b, f) for b, f, _ in EXCLUDED])
def test_excluded_document_survives_every_save_mode(builder, name, reason, excluded_files, aoo_offices, tmp_path):
    """Converts a document that data/built-excluded.tsv keeps out of the default run in each save mode, in a fresh office each time."""
    src = excluded_files(builder) / name
    kind = documents.BUILT_KIND[name.rsplit(".", 1)[1]]
    died = []
    for mode_name, mode in SAVE_MODES.items():
        session = aoo_offices.get(mode, tag="excluded-%s-%s-%s" % (builder, name, mode_name))
        try:
            session.call("convert", {"src": str(src), "dst": str(tmp_path / ("%s.%s" % (mode_name, kind))), "filter": documents.FILTERS[kind]},
                         timeout=120)
        except WorkerFailure as e:
            died.append("%s: %s" % (mode_name, e))
        finally:
            session.close()
    assert not died, "; ".join(died)
