# usage: runpy.sh fixtures/connector-viewbox/make.py <port> <outdir>   (office saving ODF 1.2 Extended)
# Connectors of every kind, connected and free, axis-aligned and diagonal, in Draw and Impress.
import sys, os, uno
from unohelp import connect, desktop, pv
from com.sun.star.awt import Size, Point
from com.sun.star.drawing.ConnectorType import STANDARD, CURVE, LINE, LINES

port, outdir = int(sys.argv[1]), sys.argv[2]
d = desktop(connect(port))

def build(doc, page):
    def rect(x, y, w, h):
        s = doc.createInstance("com.sun.star.drawing.RectangleShape")
        page.add(s); s.Position = Point(x, y); s.Size = Size(w, h)
        return s
    def conn(kind, a, b, name):
        c = doc.createInstance("com.sun.star.drawing.ConnectorShape")
        page.add(c); c.EdgeKind = kind
        c.StartPosition = Point(*a); c.EndPosition = Point(*b)
        c.Name = name
        return c
    r1 = rect(1000, 12000, 3000, 2000); r2 = rect(12000, 16000, 3000, 2000)
    r3 = rect(1000, 20000, 3000, 2000); r4 = rect(9000, 24000, 3000, 2000)
    conn(STANDARD, (2000, 2000), (8000, 6000), "std_free")
    conn(CURVE, (10000, 2000), (16000, 6000), "curve_free")
    conn(LINE, (2000, 8000), (8000, 8000), "line_h")
    conn(LINE, (10000, 8000), (10000, 11000), "line_v")
    conn(LINE, (18000, 9000), (14000, 5000), "line_rev")
    conn(STANDARD, (2000, 28000), (8000, 28000), "std_h")
    conn(STANDARD, (16000, 28000), (16000, 32000), "std_v")
    c = conn(STANDARD, (0, 0), (0, 0), "std_conn"); c.StartShape = r1; c.EndShape = r2
    c = conn(CURVE, (0, 0), (0, 0), "curve_conn"); c.StartShape = r3; c.EndShape = r4
    c = conn(LINES, (2000, 34000), (9000, 38000), "lines_free")

def save(doc, name, flt):
    doc.storeToURL(uno.systemPathToFileUrl(os.path.join(outdir, name)), (pv("FilterName", flt),))
    doc.close(True)

for kind, name, flt in (("sdraw", "conn.odg", "draw8"), ("simpress", "conn.odp", "impress8")):
    doc = d.loadComponentFromURL("private:factory/" + kind, "_blank", 0, (pv("Hidden", True),))
    build(doc, doc.DrawPages.getByIndex(0))
    save(doc, name, flt)
