#**************************************************************
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
# usage: derive-chart.py <package-with-chart.ods> <out.odc>
# Not AOO output as a standalone file: repackages the embedded chart object of a saved package as a chart document.
import sys, zipfile
import xml.etree.ElementTree as ET

src, out = sys.argv[1:3]
MIME = "application/vnd.oasis.opendocument.chart"
z = zipfile.ZipFile(src)
M = "{urn:oasis:names:tc:opendocument:xmlns:manifest:1.0}"
dirs = [e.get(M + "full-path") for e in ET.fromstring(z.read("META-INF/manifest.xml")) if e.get(M + "media-type") == MIME]
dirs = [d for d in dirs if d.endswith("/") and d != "/"]
assert len(dirs) == 1, dirs
obj = dirs[0][:-1]
mime = MIME
entries = [n for n in z.namelist() if n.startswith(obj + "/") and not n.endswith("/")]
rows = ['<manifest:file-entry manifest:media-type="%s" manifest:full-path="/"/>' % mime]
rows += ['<manifest:file-entry manifest:media-type="text/xml" manifest:full-path="%s"/>' % n[len(obj) + 1:]
         for n in entries if n[len(obj) + 1:] in ("content.xml", "styles.xml", "meta.xml", "settings.xml")]
manifest = ('<?xml version="1.0" encoding="UTF-8"?>\n<manifest:manifest xmlns:manifest="urn:oasis:names:tc:opendocument:xmlns:manifest:1.0" '
            'manifest:version="1.2">\n %s\n</manifest:manifest>\n' % "\n ".join(rows))
with zipfile.ZipFile(out, "w") as o:
    o.writestr("mimetype", mime, zipfile.ZIP_STORED)
    for n in entries:
        if n[len(obj) + 1:] in ("content.xml", "styles.xml", "meta.xml", "settings.xml"):
            o.writestr(n[len(obj) + 1:], z.read(n), zipfile.ZIP_DEFLATED)
    o.writestr("META-INF/manifest.xml", manifest, zipfile.ZIP_DEFLATED)
