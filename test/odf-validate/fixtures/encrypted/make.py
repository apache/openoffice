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

# usage: runpy.sh fixtures/encrypted/make.py <port> <outdir> [<twindir>]   (fixtures/encrypted/run.sh drives it)
# Saves password-protected packages with the office's configured ODF version and UseSHA1InODF12 /
# UseBlowfishInODF12 values; <twindir> gets the same documents without a password.
import sys, os, zlib, struct, hashlib, uno
from com.sun.star.awt import Point, Size
from com.sun.star.table import CellRangeAddress
from unohelp import connect, desktop, pv, FILTERS

PASSWORD = "t12-Password"
port, outdir = int(sys.argv[1]), os.path.abspath(sys.argv[2])
twindir = os.path.abspath(sys.argv[3]) if len(sys.argv) > 3 else None
ctx = connect(port)
d = desktop(ctx)
os.makedirs(outdir, exist_ok=True)
if twindir:
    os.makedirs(twindir, exist_ok=True)


def png(path):
    raw = b"".join(b"\x00" + b"".join(bytes((x * 16, y * 16, 128)) for x in range(16)) for y in range(16))

    def chunk(t, b):
        c = struct.pack(">I", len(b)) + t + b
        return c + struct.pack(">I", zlib.crc32(t + b) & 0xFFFFFFFF)
    with open(path, "wb") as f:
        f.write(b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", 16, 16, 8, 2, 0, 0, 0))
                + chunk(b"IDAT", zlib.compress(raw)) + chunk(b"IEND", b""))


def algs_in_use():
    cp = ctx.ServiceManager.createInstanceWithContext("com.sun.star.configuration.ConfigurationProvider", ctx)
    ca = cp.createInstanceWithArguments("com.sun.star.configuration.ConfigurationAccess",
                                        (pv("nodepath", "/org.openoffice.Office.Common/Save/ODF"),))
    return {(True, True): "", (False, False): "-aes", (True, False): "-sha1-aes",
            (False, True): "-sha256-blowfish"}[(ca.getByName("UseSHA1InODF12"), ca.getByName("UseBlowfishInODF12"))]


def graphic(url):
    gp = ctx.ServiceManager.createInstanceWithContext("com.sun.star.graphic.GraphicProvider", ctx)
    return gp.queryGraphic((pv("URL", url),))


def new(kind):
    url = {"odt": "swriter", "ods": "scalc", "odp": "simpress", "odg": "sdraw"}[kind]
    return d.loadComponentFromURL("private:factory/" + url, "_blank", 0, (pv("Hidden", True),))


def pic(doc, page, url, x, y):
    g = doc.createInstance("com.sun.star.drawing.GraphicObjectShape")
    page.add(g)
    g.Graphic = graphic(url)
    g.Position = Point(x, y)
    g.Size = Size(4000, 4000)


def shape(doc, page, svc, x, y, w, h, text=None):
    s = doc.createInstance("com.sun.star.drawing." + svc)
    page.add(s)
    s.Position = Point(x, y)
    s.Size = Size(w, h)
    if text:
        s.String = text
    return s


def writer(doc, img, objects):
    text = doc.Text
    cur = text.createTextCursor()
    text.insertString(cur, "Encrypted text document. \u00c4\u00f6\u00fc \u20ac \u4e2d\u6587\n", False)
    text.insertString(cur, "Second paragraph with enough words to make the content stream longer. " * 40, False)
    if img:
        g = doc.createInstance("com.sun.star.text.TextGraphicObject")
        g.Graphic = graphic(img)
        g.Width, g.Height = 4000, 4000
        text.insertTextContent(cur, g, False)
    if objects:
        for clsid, name in (("12dcae26-281f-416f-a234-c3086127382e", "Chart1"),
                            ("078B7ABA-54FC-457F-8551-6147e776a997", "Formula1")):
            o = doc.createInstance("com.sun.star.text.TextEmbeddedObject")
            o.CLSID = clsid
            o.Name = name
            o.Width, o.Height = 8000, 6000
            text.insertTextContent(cur, o, False)
            if name == "Formula1":
                o.Model.Formula = "a + b over c"


def calc(doc, img, objects):
    sh = doc.Sheets.getByIndex(0)
    for r in range(10):
        sh.getCellByPosition(0, r).Value = r + 1
        sh.getCellByPosition(1, r).Value = (r + 1) * 3.5
        sh.getCellByPosition(2, r).String = "row %d \u00e9" % r
    sh.getCellByPosition(3, 0).Formula = "=SUM(A1:A10)"
    if objects:
        ra = CellRangeAddress()
        ra.Sheet, ra.StartColumn, ra.StartRow, ra.EndColumn, ra.EndRow = 0, 0, 0, 1, 9
        sh.Charts.addNewByName("C1", uno.createUnoStruct("com.sun.star.awt.Rectangle", 8000, 1000, 8000, 6000), (ra,), True, True)
    if img:
        pic(doc, sh.DrawPage, img, 1000, 12000)


def impress(doc, img, objects):
    p = doc.DrawPages.getByIndex(0)
    shape(doc, p, "TextShape", 2000, 2000, 10000, 2000, "Encrypted slide one")
    shape(doc, p, "RectangleShape", 2000, 6000, 5000, 3000, "box")
    if img:
        pic(doc, p, img, 14000, 6000)
    p2 = doc.DrawPages.insertNewByIndex(1)
    shape(doc, p2, "EllipseShape", 3000, 3000, 6000, 4000, "slide two")


def draw(doc, img, objects):
    p = doc.DrawPages.getByIndex(0)
    shape(doc, p, "RectangleShape", 2000, 2000, 6000, 3000, "rect")
    shape(doc, p, "TextShape", 2000, 7000, 8000, 2000, "Encrypted drawing \u00e4")
    if img:
        pic(doc, p, img, 12000, 2000)


FILL = {"odt": writer, "ods": calc, "odp": impress, "odg": draw}
TAG = {"odt": "text", "ods": "calc", "odp": "impress", "odg": "draw"}


def modify_info(password):
    salt = os.urandom(16)
    h = hashlib.pbkdf2_hmac("sha1", password.encode("utf-8"), salt, 1024, 16)
    return uno.Any("[]com.sun.star.beans.PropertyValue",
                   (pv("algorithm-name", "PBKDF2"), pv("salt", uno.ByteSequence(salt)),
                    pv("iteration-count", 1024), pv("hash", uno.ByteSequence(h))))


def make(kind, suffix, img, objects, tag, encrypt=True, modify=False):
    doc = new(kind)
    FILL[kind](doc, img, objects)
    name = "encrypted-%s%s.%s" % (tag, suffix, kind)
    props = [pv("FilterName", FILTERS[kind])]
    if encrypt:
        props.append(pv("Password", PASSWORD))
    if modify:
        props.append(pv("ModifyPasswordInfo", modify_info(PASSWORD)))
    doc.storeToURL(uno.systemPathToFileUrl(os.path.join(outdir, name)), tuple(props))
    if twindir:
        doc.storeToURL(uno.systemPathToFileUrl(os.path.join(twindir, name)), (pv("FilterName", FILTERS[kind]),))
    doc.close(True)
    print(name)


img = os.path.join(os.path.dirname(outdir), "pic.png")
png(img)
imgurl = uno.systemPathToFileUrl(img)
suffix = algs_in_use()
print("algorithms:", suffix or "default")
for kind in FILL if suffix in ("", "-aes") else ("odt",):
    make(kind, suffix, None, False, TAG[kind])
    make(kind, suffix, imgurl, kind in ("odt", "ods"), TAG[kind] + "-objects")
if not suffix:
    for kind in ("odt", "ods"):
        make(kind, suffix, None, False, TAG[kind] + "-modifyonly", encrypt=False, modify=True)
        make(kind, suffix, None, False, TAG[kind] + "-modify", modify=True)
