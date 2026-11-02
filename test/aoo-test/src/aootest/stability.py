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

"""Comparing two saves of the same document. Date and time fields keep the value they had when the document was saved, so two
saves differ there by design; everything else in content.xml and styles.xml must be identical."""
import re
import zipfile

PARTS = ("content.xml", "styles.xml")
_FIELD = re.compile(r"(<text:(?:time|date)\b)([^>]*)>[^<]*(</text:(?:time|date)>)")
_VALUE = re.compile(r'\s+text:(?:time|date)-value="[^"]*"')


def normalize(xml):
    """`xml` with the cached text and the value attribute of text:date and text:time fields removed."""
    return _FIELD.sub(lambda m: m.group(1) + _VALUE.sub("", m.group(2)) + ">" + m.group(3), xml)


def differing_parts(first, second):
    """Names of the parts of two packages whose normalized content differs."""
    with zipfile.ZipFile(first) as a, zipfile.ZipFile(second) as b:
        return [p for p in PARTS if normalize(a.read(p).decode("utf-8")) != normalize(b.read(p).decode("utf-8"))]
