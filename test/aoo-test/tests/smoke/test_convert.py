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

"""The worker's convert op: ODF and foreign formats out, OOXML in, properties passed through, and the document always closed."""
import zipfile

import pytest

from aootest.client import WorkerError
from aootest.odfversion import MODES, odf_versions

pytestmark = pytest.mark.smoke

DOCX = {
    "[Content_Types].xml": '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>'
    '<Types xmlns="http://schemas.openxmlformats.org/package/2006/content-types">'
    '<Default Extension="rels" ContentType="application/vnd.openxmlformats-package.relationships+xml"/>'
    '<Default Extension="xml" ContentType="application/xml"/>'
    '<Override PartName="/word/document.xml" ContentType="application/vnd.openxmlformats-officedocument.wordprocessingml.document.main+xml"/>'
    '</Types>',
    "_rels/.rels": '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>'
    '<Relationships xmlns="http://schemas.openxmlformats.org/package/2006/relationships">'
    '<Relationship Id="rId1" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument" Target="word/document.xml"/>'
    '</Relationships>',
    "word/document.xml": '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>'
    '<w:document xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main"><w:body>'
    '<w:p><w:r><w:t>Imported from OOXML</w:t></w:r></w:p></w:body></w:document>',
}


@pytest.fixture
def office(aoo_offices):
    return aoo_offices.get(None)


@pytest.fixture
def source(office, tmp_path):
    """A small Writer document with known text."""
    path = tmp_path / "source.odt"
    office.call("new_and_store", {"app": "writer", "dst": str(path), "filter": "writer8", "text": "Hello Convert"}, timeout=120)
    return path


def open_documents(office):
    return office.call("open_documents")["documents"]


def test_odf_to_odf_keeps_the_text_and_leaves_nothing_open(office, source, tmp_path):
    dst = tmp_path / "out.odt"
    assert office.call("convert", {"src": str(source), "dst": str(dst), "filter": "writer8"}, timeout=120) == {"dst": str(dst)}
    with zipfile.ZipFile(dst) as z:
        assert "Hello Convert" in z.read("content.xml").decode("utf-8")
    assert open_documents(office) == []


def test_text_export(office, source, tmp_path):
    dst = tmp_path / "out.txt"
    office.call("convert", {"src": str(source), "dst": str(dst), "filter": "Text"}, timeout=120)
    assert "Hello Convert" in dst.read_text(encoding="utf-8", errors="replace")


def test_pdf_export(office, source, tmp_path):
    dst = tmp_path / "out.pdf"
    office.call("convert", {"src": str(source), "dst": str(dst), "filter": "writer_pdf_Export"}, timeout=180)
    data = dst.read_bytes()
    assert data.startswith(b"%PDF-") and b"%%EOF" in data[-1024:]


def test_binary_word_export(office, source, tmp_path):
    dst = tmp_path / "out.doc"
    office.call("convert", {"src": str(source), "dst": str(dst), "filter": "MS Word 97"}, timeout=180)
    assert dst.read_bytes().startswith(bytes.fromhex("D0CF11E0A1B11AE1"))


def test_spreadsheet_to_csv(office, tmp_path):
    ods = tmp_path / "s.ods"
    office.call("new_and_store", {"app": "calc", "dst": str(ods), "filter": "calc8", "text": "cell one"}, timeout=120)
    csv = tmp_path / "s.csv"
    office.call("convert", {"src": str(ods), "dst": str(csv), "filter": "Text - txt - csv (StarCalc)"}, timeout=120)
    assert "cell one" in csv.read_text(encoding="utf-8", errors="replace")


def test_ooxml_is_imported_by_type_detection(office, tmp_path):
    docx = tmp_path / "in.docx"
    with zipfile.ZipFile(docx, "w", zipfile.ZIP_DEFLATED) as z:
        for name, text in DOCX.items():
            z.writestr(name, text)
    dst = tmp_path / "out.odt"
    office.call("convert", {"src": str(docx), "dst": str(dst), "filter": "writer8"}, timeout=180)
    with zipfile.ZipFile(dst) as z:
        assert "Imported from OOXML" in z.read("content.xml").decode("utf-8")


@pytest.mark.parametrize("mode", [3, 4])
def test_the_save_mode_applies_to_a_conversion(aoo_offices, source, tmp_path, mode):
    dst = tmp_path / "out.odt"
    aoo_offices.get(mode).call("convert", {"src": str(source), "dst": str(dst), "filter": "writer8"}, timeout=120)
    assert odf_versions(dst) == (MODES[mode], MODES[mode])


def test_store_properties_reach_the_filter(office, source, tmp_path):
    enc = tmp_path / "enc.odt"
    office.call("convert", {"src": str(source), "dst": str(enc), "filter": "writer8", "store_props": {"Password": "secret"}}, timeout=120)
    with zipfile.ZipFile(enc) as z:
        assert b"encryption-data" in z.read("META-INF/manifest.xml")
        assert "Hello Convert" not in z.read("content.xml").decode("latin-1")
    # without the password the load fails cleanly; with it the text comes back
    with pytest.raises(WorkerError) as e:
        office.call("convert", {"src": str(enc), "dst": str(tmp_path / "nopw.odt"), "filter": "writer8"}, timeout=120)
    assert e.value.kind == "uno"
    back = tmp_path / "back.odt"
    office.call("convert", {"src": str(enc), "dst": str(back), "filter": "writer8", "load_props": {"Password": "secret"}}, timeout=120)
    with zipfile.ZipFile(back) as z:
        assert "Hello Convert" in z.read("content.xml").decode("utf-8")
    assert open_documents(office) == []


def test_a_missing_source_is_a_uno_error_and_the_office_stays_usable(office, tmp_path):
    with pytest.raises(WorkerError) as e:
        office.call("convert", {"src": str(tmp_path / "nothing.odt"), "dst": str(tmp_path / "o.odt"), "filter": "writer8"}, timeout=60)
    assert e.value.kind == "uno"
    assert office.call("ping") == {"pong": True} and open_documents(office) == []


def test_an_unknown_filter_closes_the_document_it_loaded(office, source, tmp_path):
    with pytest.raises(WorkerError) as e:
        office.call("convert", {"src": str(source), "dst": str(tmp_path / "o.x"), "filter": "no such filter"}, timeout=60)
    assert e.value.kind == "uno"
    assert open_documents(office) == []


@pytest.mark.parametrize("content", [b"", b"this is not a zip file at all\n"], ids=["empty", "garbage"])
def test_a_damaged_input_ends_cleanly(office, tmp_path, content):
    """Either a uno error or a converted document is fine; a hang, a crash, a leaked document or a dead office is not."""
    src = tmp_path / "damaged.odt"
    src.write_bytes(content)
    dst = tmp_path / "out.odt"
    try:
        office.call("convert", {"src": str(src), "dst": str(dst), "filter": "writer8"}, timeout=60)
    except WorkerError as e:
        assert e.kind == "uno"
    else:
        assert zipfile.is_zipfile(dst)
    assert office.call("ping") == {"pong": True} and open_documents(office) == []
