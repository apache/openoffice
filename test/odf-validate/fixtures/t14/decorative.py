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


# usage: run-in-mode.sh <mode> <port> <outdir> fixtures/t14/decorative.py [file]
# Without a file: makes decorative-<tag>.odp/.odg/.ods with shapes whose Decorative flag is set on some of them, and prints each
# shape's flag and state. With a file: loads it and prints the same lines, so the two outputs can be compared.
import os, struct, sys, zlib
import uno
from unohelp import FILTERS, connect, desktop, pv

port, out, tag = int(sys.argv[1]), os.path.abspath(sys.argv[2]), sys.argv[3]
os.makedirs(out, exist_ok=True)
desk = desktop(connect(port))
url = uno.systemPathToFileUrl
reload_file = os.path.abspath(sys.argv[4]) if len(sys.argv) > 4 else None

FACTORY = {"odp": "private:factory/simpress", "odg": "private:factory/sdraw", "ods": "private:factory/scalc"}
CHECKS = 0


def png(path):
    def chunk(t, d):
        c = struct.pack(">I", len(d)) + t + d
        return c + struct.pack(">I", zlib.crc32(t + d) & 0xffffffff)
    raw = b"".join(b"\x00" + b"\xff\x00\x00" * 2 for _ in range(2))
    with open(path, "wb") as f:
        f.write(b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", 2, 2, 8, 2, 0, 0, 0)) + chunk(b"IDAT", zlib.compress(raw)) + chunk(b"IEND", b""))


def page_of(doc, ext):
    if ext == "ods":
        return doc.Sheets.getByIndex(0).DrawPage
    return doc.DrawPages.getByIndex(0)


def mk(doc, page, kind, x, y, decorative):
    s = doc.createInstance("com.sun.star.drawing." + kind)
    page.add(s)
    s.Position = uno.createUnoStruct("com.sun.star.awt.Point", x, y)
    s.Size = uno.createUnoStruct("com.sun.star.awt.Size", 2500, 1500)
    if decorative is not None:
        s.Decorative = decorative
    return s


def describe(page):
    n = 0
    for i in range(page.Count):
        s = page.getByIndex(i)
        st = s.getPropertyState("Decorative").value
        print(s.ShapeType.split(".")[-1], s.Decorative, st)
        n += 1
        if s.ShapeType.endswith("GroupShape"):
            for j in range(s.Count):
                c = s.getByIndex(j)
                print("  in-group", c.ShapeType.split(".")[-1], c.Decorative, c.getPropertyState("Decorative").value)


if reload_file is None:
    img = os.path.join(out, "decorative-pixel.png")
    png(img)
    for ext, factory in FACTORY.items():
        doc = desk.loadComponentFromURL(factory, "_blank", 0, (pv("Hidden", True),))
        page = page_of(doc, ext)
        mk(doc, page, "RectangleShape", 1000, 1000, True)
        mk(doc, page, "EllipseShape", 4000, 1000, False)
        mk(doc, page, "TextShape", 7000, 1000, None)
        mk(doc, page, "LineShape", 10000, 1000, True)
        cs = mk(doc, page, "CustomShape", 1000, 4000, True)
        cs.CustomShapeGeometry = ()
        g = mk(doc, page, "GraphicObjectShape", 4000, 4000, True)
        g.GraphicURL = url(img)
        a, b = mk(doc, page, "RectangleShape", 7000, 4000, None), mk(doc, page, "RectangleShape", 7000, 5600, None)
        grp = doc.createInstance("com.sun.star.drawing.GroupShape")
        page.add(grp)
        page.remove(a)
        page.remove(b)
        grp.add(a)
        grp.add(b)
        a.Decorative = True
        grp.Decorative = True
        print("== make", ext)
        describe(page)
        # default handling: set, reset to default, read back
        r = [page.getByIndex(i) for i in range(page.Count) if page.getByIndex(i).ShapeType.endswith("RectangleShape") and page.getByIndex(i).Decorative][0]
        assert r.Decorative is True and r.getPropertyState("Decorative").value == "DIRECT_VALUE"
        r.setPropertyToDefault("Decorative")
        assert r.Decorative is False and r.getPropertyState("Decorative").value == "DEFAULT_VALUE", "reset failed"
        r.Decorative = True
        dst = os.path.join(out, "decorative-%s.%s" % (tag, ext))
        if os.path.exists(dst):
            os.remove(dst)
        doc.storeToURL(url(dst), (pv("FilterName", FILTERS[ext]), pv("Overwrite", True)))
        doc.close(True)
        print("stored", dst)
else:
    ext = os.path.splitext(reload_file)[1][1:]
    doc = desk.loadComponentFromURL(url(reload_file), "_blank", 0, (pv("Hidden", True),))
    print("== reload", ext)
    describe(page_of(doc, ext))
    doc.close(True)
