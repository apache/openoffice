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

"""aootest.odfversion: the declared versions of a package, and which packages are formula documents."""
import zipfile

import pytest

from aootest import odfversion

pytestmark = pytest.mark.selftest

MANIFEST = '<manifest:manifest xmlns:manifest="x" manifest:version="1.2"/>'


def package(path, mimetype, content):
    with zipfile.ZipFile(path, "w") as z:
        z.writestr("mimetype", mimetype)
        z.writestr("content.xml", content)
        z.writestr("META-INF/manifest.xml", MANIFEST)
    return str(path)


def test_a_text_document_declares_its_version_in_content_and_manifest(tmp_path):
    p = package(tmp_path / "a.odt", "application/vnd.oasis.opendocument.text", '<office:document-content office:version="1.2"/>')
    assert odfversion.odf_versions(p) == ("1.2", "1.2")
    assert not odfversion.is_formula(p)


def test_a_formula_document_has_a_mathml_root_without_a_version(tmp_path):
    p = package(tmp_path / "a.odf", "application/vnd.oasis.opendocument.formula", '<math xmlns="http://www.w3.org/1998/Math/MathML"/>')
    assert odfversion.odf_versions(p) == (None, "1.2")
    assert odfversion.is_formula(p)
