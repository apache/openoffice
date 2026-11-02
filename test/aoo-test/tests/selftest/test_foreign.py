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

"""foreign.strip_foreign(): removal of foreign elements and attributes (ODF 1.2 Part 1 2.2.2 B.1, 3.17)."""
import os

import pytest

from aootest.checkers import foreign

pytestmark = pytest.mark.selftest

HDR = ('<office:text xmlns:office="urn:oasis:names:tc:opendocument:xmlns:office:1.0" xmlns:text="urn:oasis:names:tc:opendocument:xmlns:text:1.0" '
       'xmlns:draw="urn:oasis:names:tc:opendocument:xmlns:drawing:1.0" xmlns:foo="urn:foo" xmlns:dc="http://purl.org/dc/elements/1.1/" '
       'xmlns:xlink="http://www.w3.org/1999/xlink" xmlns:xf="http://www.w3.org/2002/xforms"')
CASES = [
    ("foreign attribute removed", '<text:p foo:x="1" text:style-name="a">t</text:p>', '<text:p text:style-name="a">t</text:p>'),
    ("foreign element outside a paragraph dropped with its content", '<foo:x>stuff<text:p>q</text:p></foo:x><text:p>t</text:p>', '<text:p>t</text:p>'),
    ("foreign element in a paragraph: tags removed, content kept", '<text:p>a<foo:x>b<text:span>c</text:span></foo:x>d</text:p>', '<text:p>ab<text:span>c</text:span>d</text:p>'),
    ("foreign element in a span in a paragraph: tags removed", '<text:p><text:span>a<foo:x>b</foo:x></text:span></text:p>', '<text:p><text:span>ab</text:span></text:p>'),
    ("foreign element below a frame (no character data): dropped", '<text:p><draw:frame><foo:x>b</foo:x></draw:frame></text:p>', '<text:p><draw:frame></draw:frame></text:p>'),
    ("namespace declared on an unwrapped element is kept for its children",
     '<text:p><foo:x xmlns:t2="urn:oasis:names:tc:opendocument:xmlns:text:1.0"><t2:span>a</t2:span></foo:x></text:p>',
     '<text:p><t2:span xmlns:t2="urn:oasis:names:tc:opendocument:xmlns:text:1.0">a</t2:span></text:p>'),
    ("elements and attributes of table 4 namespaces kept", '<text:p xlink:href="u"><dc:title>t</dc:title></text:p>', '<text:p xlink:href="u"><dc:title>t</dc:title></text:p>'),
    ("content below a table 4 element kept (wildcard content)", '<xf:model><foo:data><bar/></foo:data></xf:model>', '<xf:model><foo:data><bar></bar></foo:data></xf:model>'),
    ("text is escaped again", '<text:p>a &amp; b &lt; c</text:p>', '<text:p>a &amp; b &lt; c</text:p>'),
]


@pytest.fixture(scope="module")
def mixed(schema_dir):
    return foreign.mixed_elements(os.path.join(schema_dir, "OpenDocument-v1.2-os-schema.rng"))


@pytest.mark.parametrize("src, want", [pytest.param(s, w, id=n) for n, s, w in CASES])
def test_strip_foreign(src, want, mixed):
    got = foreign.strip_foreign(HDR + ">" + src + "</office:text>", mixed).split("\n", 1)[1].strip()
    inner = got[got.index(">") + 1:-len("</office:text>")] if got.startswith("<office:text") else got
    assert inner == want
