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

"""Which ODF version a saved package declares."""
import re
import zipfile

# Save/ODF/DefaultVersion -> the office:version written (3 and 4 are ODF 1.2, 5 and 6 are 1.3, 7 and 8 are 1.4)
MODES = {3: "1.2", 4: "1.2", 5: "1.3", 6: "1.3", 7: "1.4", 8: "1.4"}


# mode name -> Save/ODF/DefaultVersion (None leaves the product default)
SAVE_MODES = {"strict": 4, "ext": 3, "v13": 5, "x13": 6, "v14": 7, "x14": 8, "default": None}


def odf_versions(path):
    """(office:version of the root element of content.xml, manifest:version of META-INF/manifest.xml)."""
    with zipfile.ZipFile(path) as z:
        content = z.read("content.xml").decode("utf-8")
        manifest = z.read("META-INF/manifest.xml").decode("utf-8")
    c = re.search(r'<office:document-content[^>]*\boffice:version="([^"]*)"', content)
    m = re.search(r'<manifest:manifest[^>]*\bmanifest:version="([^"]*)"', manifest)
    return (c.group(1) if c else None), (m.group(1) if m else None)


def is_formula(path):
    """True for a formula (Math) package: its content.xml is a MathML root that carries no office:version."""
    with zipfile.ZipFile(path) as z:
        return z.read("mimetype").decode("ascii").strip() == "application/vnd.oasis.opendocument.formula"
