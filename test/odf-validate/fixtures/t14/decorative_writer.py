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


# usage: run-in-mode.sh <mode> <port> <outdir> fixtures/t14/decorative_writer.py [file]
# Without a file: makes decorative-writer-<tag>.odt with a text frame, a graphic and a drawing shape, some of them decorative,
# and prints the flag and property state of each. With a file: loads it and prints the same lines.
import os, struct, sys, zlib
import uno
from unohelp import FILTERS, connect, desktop, pv

port, out, tag = int(sys.argv[1]), os.path.abspath(sys.argv[2]), sys.argv[3]
os.makedirs(out, exist_ok=True)
desk = desktop(connect(port))
url = uno.systemPathToFileUrl
reload_file = os.path.abspath(sys.argv[4]) if len(sys.argv) > 4 else None


def png(path):
    def chunk(t, d):
        c = struct.pack(">I", len(d)) + t + d
        return c + struct.pack(">I", zlib.crc32(t + d) & 0xffffffff)
    raw = b"".join(b"\x00" + b"\xff\x00\x00" * 2 for _ in range(2))
    with open(path, "wb") as f:
        f.write(b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", 2, 2, 8, 2, 0, 0, 0)) + chunk(b"IDAT", zlib.compress(raw)) + chunk(b"IEND", b""))


def describe(doc):
    for kind, coll in (("frame", doc.TextFrames), ("graphic", doc.GraphicObjects)):
        for n in coll.ElementNames:
            o = coll.getByName(n)
            print(kind, n, o.Decorative, o.getPropertyState("Decorative").value)
    dp = doc.DrawPage
    for i in range(dp.Count):
        s = dp.getByIndex(i)
        if s.ShapeType.endswith("RectangleShape"):
            print("shape", s.ShapeType.split(".")[-1], s.Decorative, s.getPropertyState("Decorative").value)


if reload_file is None:
    img = os.path.join(out, "decorative-pixel.png")
    png(img)
    doc = desk.loadComponentFromURL("private:factory/swriter", "_blank", 0, (pv("Hidden", True),))
    text = doc.Text
    cur = text.createTextCursor()
    text.insertString(cur, "anchor paragraph", False)

    def add(kind, name, decorative):
        o = doc.createInstance("com.sun.star.text." + kind)
        o.AnchorType = uno.Enum("com.sun.star.text.TextContentAnchorType", "AS_CHARACTER")
        o.Width, o.Height = 3000, 2000
        if decorative is not None:
            o.Decorative = decorative   # set on the descriptor, before insertion
        text.insertTextContent(text.createTextCursor(), o, False)
        o.Name = name
        return o

    add("TextFrame", "frame-dec", True)
    add("TextFrame", "frame-plain", None)
    g = doc.createInstance("com.sun.star.text.GraphicObject")
    g.GraphicURL = url(img)
    g.AnchorType = uno.Enum("com.sun.star.text.TextContentAnchorType", "AS_CHARACTER")
    g.Width, g.Height = 2000, 2000
    g.Decorative = True
    text.insertTextContent(text.createTextCursor(), g, False)
    g.Name = "graphic-dec"
    g2 = doc.createInstance("com.sun.star.text.GraphicObject")
    g2.GraphicURL = url(img)
    g2.AnchorType = uno.Enum("com.sun.star.text.TextContentAnchorType", "AS_CHARACTER")
    g2.Width, g2.Height = 2000, 2000
    text.insertTextContent(text.createTextCursor(), g2, False)
    g2.Name = "graphic-plain"
    # set on an inserted frame, and reset to default
    f = doc.TextFrames.getByName("frame-plain")
    f.Decorative = True
    assert f.Decorative is True and f.getPropertyState("Decorative").value == "DIRECT_VALUE"
    f.setPropertyToDefault("Decorative")
    assert f.Decorative is False and f.getPropertyState("Decorative").value == "DEFAULT_VALUE", "reset failed"
    # a drawing shape anchored in the text
    r = doc.createInstance("com.sun.star.drawing.RectangleShape")
    r.Size = uno.createUnoStruct("com.sun.star.awt.Size", 2000, 1000)
    r.AnchorType = uno.Enum("com.sun.star.text.TextContentAnchorType", "AT_PARAGRAPH")
    text.insertTextContent(text.createTextCursor(), r, False)
    r.Decorative = True
    print("== make")
    describe(doc)
    dst = os.path.join(out, "decorative-writer-%s.odt" % tag)
    if os.path.exists(dst):
        os.remove(dst)
    doc.storeToURL(url(dst), (pv("FilterName", FILTERS["odt"]), pv("Overwrite", True)))
    doc.close(True)
    print("stored", dst)
else:
    doc = desk.loadComponentFromURL(url(reload_file), "_blank", 0, (pv("Hidden", True),))
    print("== reload")
    describe(doc)
    doc.close(True)
