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

"""Documents with content in each application, saved with a password (and as a plain twin), for the encryption tests.
Ported from the ODF harness's fixtures/encrypted/make.py."""
import os
import struct
import zlib

import uno
from com.sun.star.awt import Point, Size
from com.sun.star.table import CellRangeAddress

from aootest_worker.ops import core
from aootest_worker.ops.core import pv

FILTERS = {"odt": "writer8", "ods": "calc8", "odp": "impress8", "odg": "draw8"}
FACTORY = {"odt": "swriter", "ods": "scalc", "odp": "simpress", "odg": "sdraw"}


def _png(path):
    raw = b"".join(b"\x00" + b"".join(bytes((x * 16, y * 16, 128)) for x in range(16)) for y in range(16))

    def chunk(t, b):
        c = struct.pack(">I", len(b)) + t + b
        return c + struct.pack(">I", zlib.crc32(t + b) & 0xFFFFFFFF)
    with open(path, "wb") as f:
        f.write(b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", 16, 16, 8, 2, 0, 0, 0))
                + chunk(b"IDAT", zlib.compress(raw)) + chunk(b"IEND", b""))


def _graphic(url):
    gp = core._create("com.sun.star.graphic.GraphicProvider")
    return gp.queryGraphic((pv("URL", url),))


def _pic(doc, page, url, x, y):
    g = doc.createInstance("com.sun.star.drawing.GraphicObjectShape")
    page.add(g)
    g.Graphic = _graphic(url)
    g.Position = Point(x, y)
    g.Size = Size(4000, 4000)


def _shape(doc, page, svc, x, y, w, h, text=None):
    s = doc.createInstance("com.sun.star.drawing." + svc)
    page.add(s)
    s.Position = Point(x, y)
    s.Size = Size(w, h)
    if text:
        s.String = text
    return s


def _writer(doc, img, objects):
    text = doc.Text
    cur = text.createTextCursor()
    text.insertString(cur, "Encrypted text document. Äöü € 中文\n", False)
    text.insertString(cur, "Second paragraph with enough words to make the content stream longer. " * 40, False)
    if img:
        g = doc.createInstance("com.sun.star.text.TextGraphicObject")
        g.Graphic = _graphic(img)
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


def _calc(doc, img, objects):
    sh = doc.Sheets.getByIndex(0)
    for r in range(10):
        sh.getCellByPosition(0, r).Value = r + 1
        sh.getCellByPosition(1, r).Value = (r + 1) * 3.5
        sh.getCellByPosition(2, r).String = "row %d é" % r
    sh.getCellByPosition(3, 0).Formula = "=SUM(A1:A10)"
    if objects:
        ra = CellRangeAddress()
        ra.Sheet, ra.StartColumn, ra.StartRow, ra.EndColumn, ra.EndRow = 0, 0, 0, 1, 9
        sh.Charts.addNewByName("C1", uno.createUnoStruct("com.sun.star.awt.Rectangle", 8000, 1000, 8000, 6000), (ra,), True, True)
    if img:
        _pic(doc, sh.DrawPage, img, 1000, 12000)


def _impress(doc, img, objects):
    p = doc.DrawPages.getByIndex(0)
    _shape(doc, p, "TextShape", 2000, 2000, 10000, 2000, "Encrypted slide one")
    _shape(doc, p, "RectangleShape", 2000, 6000, 5000, 3000, "box")
    if img:
        _pic(doc, p, img, 14000, 6000)
    p2 = doc.DrawPages.insertNewByIndex(1)
    _shape(doc, p2, "EllipseShape", 3000, 3000, 6000, 4000, "slide two")


def _draw(doc, img, objects):
    p = doc.DrawPages.getByIndex(0)
    _shape(doc, p, "RectangleShape", 2000, 2000, 6000, 3000, "rect")
    _shape(doc, p, "TextShape", 2000, 7000, 8000, 2000, "Encrypted drawing ä")
    if img:
        _pic(doc, p, img, 12000, 2000)


FILL = {"odt": _writer, "ods": _calc, "odp": _impress, "odg": _draw}


def make_encrypted(kind, dst, twin, password, with_objects=False):
    """Creates a `kind` (odt, ods, odp, odg) document with content, stores it with `password` as `dst` and without as `twin`.
    `with_objects` adds a picture (all kinds) and charts or formula objects (odt, ods)."""
    if kind not in FILL:
        raise core.Fail("bad_args", "unknown kind %r" % kind)
    img = None
    if with_objects:
        png = os.path.join(os.path.dirname(os.path.abspath(dst)), "pic.png")
        _png(png)
        img = uno.systemPathToFileUrl(png)
    doc = core._desktop().loadComponentFromURL("private:factory/" + FACTORY[kind], "_blank", 0, (pv("Hidden", True),))
    if doc is None:
        raise core.Fail("uno", "could not create a new %s document" % kind)
    try:
        FILL[kind](doc, img, with_objects and kind in ("odt", "ods"))
        doc.storeToURL(uno.systemPathToFileUrl(os.path.abspath(dst)), (pv("FilterName", FILTERS[kind]), pv("Password", password)))
        doc.storeToURL(uno.systemPathToFileUrl(os.path.abspath(twin)), (pv("FilterName", FILTERS[kind]),))
    finally:
        core._close(doc)
    return {"dst": os.path.abspath(dst), "twin": os.path.abspath(twin)}


OPS = {"make_encrypted": core.guarded(make_encrypted)}
