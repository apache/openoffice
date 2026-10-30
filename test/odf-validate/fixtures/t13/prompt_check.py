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
#**************************************************************

# usage: runpy.sh fixtures/t13/prompt_check.py <port> <package.odt|ods|odp|odg> <outdir>
# Loads copies of one ODF 1.2 package whose office:version and manifest:version are rewritten to 1.2, 1.3 and 1.4, with an
# interaction handler, and reports whether the "created with a newer version" request (FutureDocumentVersionProductUpdateRequest)
# was raised, counted as a call of the handler (the documents used raise no other request). Expected: none for 1.2 and 1.3, one for
# 1.4 (the negative control). Needs Load/ShowOfficeUpdateDialog = true.
import os, re, sys, zipfile
import uno, unohelper
from com.sun.star.task import XInteractionHandler
from unohelp import connect, desktop, pv

port, src, out = int(sys.argv[1]), os.path.abspath(sys.argv[2]), os.path.abspath(sys.argv[3])
os.makedirs(out, exist_ok=True)


class Handler(unohelper.Base, XInteractionHandler):
    # getRequest() cannot be used from pyuno here (it raises, the request being an exception value), so a call counts
    def __init__(self):
        self.calls = 0

    def handle(self, request):
        self.calls += 1
        for c in request.getContinuations():
            if c.queryInterface(uno.getTypeByName("com.sun.star.task.XInteractionApprove")):
                c.select()
                return


def rewrite(version, dst):
    zi, zo = zipfile.ZipFile(src), zipfile.ZipFile(dst, "w")
    for it in zi.infolist():
        d = zi.read(it.filename)
        if it.filename.endswith(".xml") and (it.filename == "META-INF/manifest.xml" or it.filename.rsplit("/", 1)[-1] in ("content.xml", "styles.xml", "meta.xml", "settings.xml")):
            t = d.decode("utf-8")
            attr = "manifest:version" if it.filename == "META-INF/manifest.xml" else "office:version"
            t = re.sub(r'(%s=")[0-9.]+(")' % attr, r"\g<1>%s\g<2>" % version, t)
            d = t.encode("utf-8")
        zo.writestr(it, d, compress_type=it.compress_type)
    zo.close()


ctx = connect(port)
desk = desktop(ctx)
ext = os.path.splitext(src)[1]
results = {}
for version, want in (("1.2", False), ("1.3", False), ("1.4", True)):
    dst = os.path.join(out, "v" + version.replace(".", "") + ext)
    rewrite(version, dst)
    h = Handler()
    doc = desk.loadComponentFromURL(uno.systemPathToFileUrl(dst), "_blank", 0,
            (pv("Hidden", True), pv("MacroExecutionMode", 0), pv("ReadOnly", True), pv("UpdateDocMode", 0), pv("InteractionHandler", h)))
    loaded = doc is not None
    if loaded:
        doc.close(True)
    got = h.calls > 0
    results[version] = (loaded, got, want)
    print("%s: version %s, loaded=%s, update request raised=%s (handler calls: %d)" % ("PASS" if loaded and got == want else "FAIL", version, loaded, got, h.calls))
print("prompt_check: %d/%d pass" % (sum(1 for l, g, w in results.values() if l and g == w), len(results)))
sys.exit(0 if all(l and g == w for l, g, w in results.values()) else 1)
