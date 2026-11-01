# usage: runpy.sh fixtures/para-fill-foreign/make-aoo.py <port> <out.odt> <tile.png>   (office saving ODF 1.2 Extended)
# Builds the same paragraphs and paragraph styles as gen.bas, with AOO's own fill properties.
import sys, uno
from unohelp import connect, desktop, pv
from com.sun.star.awt import Gradient
from com.sun.star.awt.GradientStyle import LINEAR
from com.sun.star.drawing import Hatch
from com.sun.star.drawing.HatchStyle import SINGLE
from com.sun.star.drawing.FillStyle import SOLID, GRADIENT, HATCH, BITMAP
from com.sun.star.drawing.BitmapMode import REPEAT, STRETCH, NO_REPEAT

port, out, png = int(sys.argv[1]), sys.argv[2], uno.systemPathToFileUrl(sys.argv[3])
d = desktop(connect(port))
doc = d.loadComponentFromURL("private:factory/swriter", "_blank", 0, (pv("Hidden", True),))

def grad(c1, c2):
    g = Gradient(); g.Style = LINEAR; g.StartColor = c1; g.EndColor = c2
    g.Angle = 300; g.Border = 10; g.XOffset = 50; g.YOffset = 50
    g.StartIntensity = 100; g.EndIntensity = 100; g.StepCount = 0
    return g

def hatch():
    h = Hatch(); h.Style = SINGLE; h.Color = 0xFFAA00; h.Distance = 200; h.Angle = 450
    return h

def fill(o, kind):
    if kind == "solid":
        o.FillStyle = SOLID; o.FillColor = 0x33AA55
    elif kind == "solid_transp":
        o.FillStyle = SOLID; o.FillColor = 0x3355AA; o.FillTransparence = 40
    elif kind == "gradient":
        o.FillStyle = GRADIENT; o.FillGradient = grad(0xFF0000, 0x0000FF)
    elif kind == "gradient_transp":
        o.FillStyle = GRADIENT; o.FillGradient = grad(0xFF0000, 0x0000FF); o.FillTransparence = 30
    elif kind == "hatch":
        o.FillStyle = HATCH; o.FillHatch = hatch(); o.FillColor = 0xFFFFCC; o.FillBackground = True
    elif kind == "hatch_nobg":
        o.FillStyle = HATCH; o.FillHatch = hatch(); o.FillBackground = False
    elif kind.startswith("bitmap"):
        o.FillStyle = BITMAP; o.FillBitmapURL = png
        o.FillBitmapMode = {"bitmap_stretch": STRETCH, "bitmap_once": NO_REPEAT}.get(kind, REPEAT)
        if kind == "bitmap_transp":
            o.FillTransparence = 50

kinds = ["solid", "solid_transp", "gradient", "gradient_transp", "hatch", "hatch_nobg", "bitmap_tile", "bitmap_stretch", "bitmap_once", "bitmap_transp"]
styles = doc.StyleFamilies.getByName("ParagraphStyles")
for k in kinds:
    s = doc.createInstance("com.sun.star.style.ParagraphStyle")
    s.ParentStyle = "Standard"
    styles.insertByName("S_" + k, s)
    fill(s, k)
PB = uno.getConstantByName("com.sun.star.text.ControlCharacter.PARAGRAPH_BREAK")
cur = doc.Text.createTextCursor()
for k in kinds:
    for kind in ("direct", "styled"):
        doc.Text.insertString(cur, kind + " " + k, False)
        doc.Text.insertControlCharacter(cur, PB, False)
paras = doc.Text.createEnumeration()
for k in kinds:
    fill(paras.nextElement(), k)
    paras.nextElement().ParaStyleName = "S_" + k
doc.storeToURL(uno.systemPathToFileUrl(out), (pv("FilterName", "writer8"),))
doc.close(True)
print("saved", out)
