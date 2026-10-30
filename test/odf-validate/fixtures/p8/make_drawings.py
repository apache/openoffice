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

# usage: runpy.sh fixtures/p8/make_drawings.py <port> <outdir>
# Writes ten small ODF drawings (shapes, text, connectors, groups, gradient and hatch fills, polygons, custom shapes,
# measure lines, rotation/shadow/transparency, several pages) with the office, as input for the foreign 1.2/1.3 twins
# of tools13/make-foreign-corpus.py. Prints one line per drawing with its shape count; exit code 1 when one failed.
import os, sys
import uno
from com.sun.star.awt import Point, Size, Gradient
from com.sun.star.beans import PropertyValue
from com.sun.star.drawing import Hatch
from unohelp import connect, desktop, pv

port, out = int(sys.argv[1]), os.path.abspath(sys.argv[2])
os.makedirs(out, exist_ok=True)
ctx = connect(port)
desk = desktop(ctx)
E = lambda t, v: uno.Enum(t, v)


def new_doc():
    return desk.loadComponentFromURL("private:factory/sdraw", "_blank", 0, (pv("Hidden", True),))


def shape(doc, page, kind, x, y, w, h, **props):
    s = doc.createInstance("com.sun.star.drawing." + kind)
    page.add(s)
    s.Position = Point(x, y)
    s.Size = Size(w, h)
    for k, v in props.items():
        s.setPropertyValue(k, v)
    return s


def d01(doc):
    p = doc.DrawPages.getByIndex(0)
    shape(doc, p, "RectangleShape", 1000, 1000, 5000, 3000, FillColor=0xCC3333, LineColor=0x000000, LineWidth=100)
    shape(doc, p, "EllipseShape", 7000, 1000, 4000, 3000, FillColor=0x3366CC, LineStyle=E("com.sun.star.drawing.LineStyle", "DASH"))
    shape(doc, p, "LineShape", 1000, 5000, 10000, 0, LineColor=0x00AA00, LineWidth=250)
    shape(doc, p, "RectangleShape", 1000, 6000, 3000, 2000, CornerRadius=500, FillColor=0xFFCC00)


def d02(doc):
    p = doc.DrawPages.getByIndex(0)
    for i, (txt, size, adj) in enumerate((("Left aligned text", 18, "LEFT"), ("Centered heading", 28, "CENTER"), ("Right aligned small", 10, "RIGHT"))):
        s = shape(doc, p, "TextShape", 1000, 1000 + i * 2500, 12000, 2000)
        s.String = txt
        s.CharHeight = size
        s.CharFontName = "Helvetica"
        s.CharWeight = 150.0 if i == 1 else 100.0
        s.ParaAdjust = E("com.sun.star.style.ParagraphAdjust", adj)
    r = shape(doc, p, "RectangleShape", 1000, 9000, 8000, 2500, FillColor=0xEEEEEE)
    r.String = "Text inside a rectangle"
    r.CharColor = 0x0000CC


def d03(doc):
    p = doc.DrawPages.getByIndex(0)
    a = shape(doc, p, "RectangleShape", 1000, 1000, 3000, 2000, FillColor=0xCCCCFF)
    b = shape(doc, p, "RectangleShape", 8000, 5000, 3000, 2000, FillColor=0xCCFFCC)
    c = shape(doc, p, "EllipseShape", 1000, 9000, 3000, 2000, FillColor=0xFFCCCC)
    for kind, s, e in (("STANDARD", a, b), ("CURVE", b, c), ("LINE", c, a)):
        con = doc.createInstance("com.sun.star.drawing.ConnectorShape")
        p.add(con)
        con.StartShape, con.EndShape = s, e
        con.EdgeKind = E("com.sun.star.drawing.ConnectorType", kind)
        con.LineEndName = "Arrow"


def d04(doc):
    p = doc.DrawPages.getByIndex(0)
    inner = []
    for i in range(3):
        inner.append(shape(doc, p, "RectangleShape", 1000 + i * 1500, 1000, 1200, 1200, FillColor=0x336699 + i * 0x2020))
    col = ctx.ServiceManager.createInstanceWithContext("com.sun.star.drawing.ShapeCollection", ctx)
    for s in inner:
        col.add(s)
    g1 = p.group(col)
    e = shape(doc, p, "EllipseShape", 1000, 4000, 3000, 2000, FillColor=0xFFAA00)
    col2 = ctx.ServiceManager.createInstanceWithContext("com.sun.star.drawing.ShapeCollection", ctx)
    col2.add(g1)
    col2.add(e)
    p.group(col2)


def d05(doc):
    p = doc.DrawPages.getByIndex(0)
    s = shape(doc, p, "RectangleShape", 1000, 1000, 5000, 3000)
    g = Gradient()
    g.Style = E("com.sun.star.awt.GradientStyle", "LINEAR")
    g.StartColor, g.EndColor, g.Angle, g.Border = 0xFF0000, 0x0000FF, 450, 0
    g.XOffset = g.YOffset = 50
    g.StartIntensity = g.EndIntensity = 100
    s.FillStyle = E("com.sun.star.drawing.FillStyle", "GRADIENT")
    s.FillGradient = g
    h = shape(doc, p, "EllipseShape", 7000, 1000, 4000, 3000)
    hh = Hatch()
    hh.Style = E("com.sun.star.drawing.HatchStyle", "DOUBLE")
    hh.Color, hh.Distance, hh.Angle = 0x006600, 150, 300
    h.FillStyle = E("com.sun.star.drawing.FillStyle", "HATCH")
    h.FillHatch = hh
    h.FillBackground = True
    h.FillColor = 0xFFFFCC


def d06(doc):
    pages = doc.DrawPages
    pages.insertNewByIndex(1)
    pages.insertNewByIndex(2)
    for i in range(3):
        p = pages.getByIndex(i)
        p.Name = "Page %d" % (i + 1)
        for j in range(i + 1):
            s = shape(doc, p, "RectangleShape", 1000 + j * 3000, 1000, 2500, 2000, FillColor=0x80 * (j + 1) + i * 0x1000)
            s.String = "p%d s%d" % (i + 1, j + 1)


def d07(doc):
    p = doc.DrawPages.getByIndex(0)
    s = doc.createInstance("com.sun.star.drawing.PolyPolygonShape")
    p.add(s)
    s.PolyPolygon = ((Point(1000, 1000), Point(5000, 1000), Point(3000, 4000)),)
    s.FillColor = 0x99CC33
    t = doc.createInstance("com.sun.star.drawing.PolyLineShape")
    p.add(t)
    t.PolyPolygon = ((Point(7000, 1000), Point(9000, 3000), Point(7500, 5000), Point(10000, 6000)),)
    t.LineColor, t.LineWidth = 0xCC0066, 150
    q = doc.createInstance("com.sun.star.drawing.PolyPolygonShape")
    p.add(q)
    q.PolyPolygon = ((Point(1000, 7000), Point(4000, 7000), Point(4000, 9000), Point(1000, 9000)),
                     (Point(1800, 7500), Point(3200, 7500), Point(3200, 8500), Point(1800, 8500)))
    q.FillColor = 0x3399CC


def d08(doc):
    p = doc.DrawPages.getByIndex(0)
    for i, kind in enumerate(("diamond", "heart", "star5", "right-arrow", "cloud")):
        s = doc.createInstance("com.sun.star.drawing.CustomShape")
        p.add(s)
        s.Position = Point(1000 + (i % 3) * 4000, 1000 + (i // 3) * 4000)
        s.Size = Size(3000, 3000)
        s.CustomShapeGeometry = (PropertyValue("Type", 0, kind, 0),)
        s.FillColor = 0x6600CC + i * 0x1100
        s.String = kind


def d09(doc):
    p = doc.DrawPages.getByIndex(0)
    for i in range(3):
        m = doc.createInstance("com.sun.star.drawing.MeasureShape")
        p.add(m)
        m.StartPosition = Point(1000, 1500 + i * 2500)
        m.EndPosition = Point(6000 + i * 1500, 1500 + i * 2500)
    shape(doc, p, "RectangleShape", 1000, 9000, 6000, 1500, FillColor=0xDDDDDD)


def d10(doc):
    p = doc.DrawPages.getByIndex(0)
    a = shape(doc, p, "RectangleShape", 2000, 2000, 4000, 2500, FillColor=0xCC6600, Shadow=True, ShadowXDistance=300, ShadowYDistance=300)
    a.RotateAngle = 3000
    b = shape(doc, p, "EllipseShape", 8000, 2000, 4000, 2500, FillColor=0x0066CC, FillTransparence=50)
    c = shape(doc, p, "RectangleShape", 2000, 7000, 4000, 2000, FillColor=0x669900)
    c.ShearAngle = 1500
    d = shape(doc, p, "RectangleShape", 7000, 7000, 4000, 2000, FillColor=0x990066, LineTransparence=40, LineWidth=300)
    d.String = "transparent line"


fails = 0
for n, fn in enumerate((d01, d02, d03, d04, d05, d06, d07, d08, d09, d10), 1):
    name = "draw%02d" % n
    try:
        doc = new_doc()
        fn(doc)
        npages = doc.DrawPages.Count
        count = sum(doc.DrawPages.getByIndex(i).Count for i in range(npages))
        doc.storeToURL(uno.systemPathToFileUrl(os.path.join(out, name + ".odg")), (pv("FilterName", "draw8"),))
        doc.close(True)
        print("ok   %s shapes=%d pages=%d" % (name, count, npages))
    except Exception as e:
        fails += 1
        print("FAIL %s %s: %s" % (name, type(e).__name__, e))
sys.exit(1 if fails else 0)
