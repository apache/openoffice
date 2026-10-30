# usage: runpy.sh fixtures/chart-shapes/make.py <port> <out.ods>   (office saving ODF 1.2 Extended)
# Calc chart with additional shapes (polyline, rectangle, text) in the chart draw page.
import sys, uno
from unohelp import connect, desktop, pv
from com.sun.star.awt import Size, Point, Rectangle
from com.sun.star.table import CellRangeAddress

port, out = int(sys.argv[1]), sys.argv[2]
d = desktop(connect(port))
doc = d.loadComponentFromURL("private:factory/scalc", "_blank", 0, (pv("Hidden", True),))
sh = doc.Sheets.getByIndex(0)
for r, (a, b) in enumerate(((1, 2), (2, 4), (3, 3))):
    sh.getCellByPosition(0, r).Value = a
    sh.getCellByPosition(1, r).Value = b
rng = CellRangeAddress(); rng.Sheet = 0; rng.StartColumn = 0; rng.StartRow = 0; rng.EndColumn = 1; rng.EndRow = 2
r = Rectangle(); r.X = 1000; r.Y = 4000; r.Width = 10000; r.Height = 7000
sh.Charts.addNewByName("c1", r, (rng,), False, False)
ch = sh.Charts.getByName("c1").EmbeddedObject
page = ch.DrawPage
from com.sun.star.awt import Point as P
def mk(kind, x, y, w, h):
    s = ch.createInstance(kind)
    page.add(s)
    s.Position = Point(x, y); s.Size = Size(w, h)
    return s
pl = mk("com.sun.star.drawing.PolyLineShape", 500, 500, 3000, 1500)
pl.PolyPolygon = ((P(0, 1500), P(1000, 500), P(3000, 0)),)
mk("com.sun.star.drawing.RectangleShape", 4000, 500, 2000, 1000).String = "rect"
mk("com.sun.star.drawing.TextShape", 6500, 500, 2000, 800).String = "note"
doc.storeToURL(uno.systemPathToFileUrl(out), (pv("FilterName", "calc8"),))
doc.close(True)
