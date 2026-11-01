# usage: runpy.sh fixtures/para-fill/make.py <port> <out.odt>   (run against an office saving ODF 1.2 Extended)
import sys, uno
from unohelp import connect, desktop, pv
from com.sun.star.awt import Gradient
from com.sun.star.awt.GradientStyle import LINEAR
from com.sun.star.drawing import Hatch
from com.sun.star.drawing.HatchStyle import SINGLE
from com.sun.star.drawing.FillStyle import SOLID, GRADIENT, HATCH, BITMAP

port, out = int(sys.argv[1]), sys.argv[2]
d = desktop(connect(port))
doc = d.loadComponentFromURL("private:factory/swriter", "_blank", 0, (pv("Hidden", True),))
text = doc.Text
png = uno.systemPathToFileUrl(sys.argv[3]) if len(sys.argv) > 3 else None

def grad():
    g = Gradient(); g.Style = LINEAR; g.StartColor = 0xFF0000; g.EndColor = 0x0000FF
    g.Angle = 0; g.Border = 0; g.XOffset = 50; g.YOffset = 50
    g.StartIntensity = 100; g.EndIntensity = 100; g.StepCount = 0
    return g

def hatch():
    h = Hatch(); h.Style = SINGLE; h.Color = 0x00AA00; h.Distance = 200; h.Angle = 450
    return h

def fill(o, kind):
    if kind == "solid":
        o.ParaBackColor = 0xFFCC00; o.ParaBackTransparent = False
    elif kind == "solid_fill":
        o.FillStyle = SOLID; o.FillColor = 0x33AA55
    elif kind == "solid_transp":
        o.FillStyle = SOLID; o.FillColor = 0x3355AA; o.FillTransparence = 40
    elif kind == "gradient":
        o.FillStyle = GRADIENT; o.FillGradient = grad()
    elif kind == "hatch":
        o.FillStyle = HATCH; o.FillHatch = hatch(); o.FillColor = 0xFFFFCC; o.FillBackground = True
    elif kind == "bitmap":
        o.FillStyle = BITMAP; o.FillBitmapURL = png

kinds = ["solid", "solid_fill", "solid_transp", "gradient", "hatch"] + (["bitmap"] if png else [])
styles = doc.StyleFamilies.getByName("ParagraphStyles")
for k in kinds:
    s = doc.createInstance("com.sun.star.style.ParagraphStyle")
    s.ParentStyle = "Standard"
    styles.insertByName("S_" + k, s)
    fill(s, k)

PB = uno.getConstantByName("com.sun.star.text.ControlCharacter.PARAGRAPH_BREAK")
cur = text.createTextCursor()
labels = []
for k in kinds:
    for kind in ("direct", "styled"):
        text.insertString(cur, kind + " " + k, False)
        text.insertControlCharacter(cur, PB, False)
        labels.append((kind, k))
paras = doc.Text.createEnumeration()
for kind, k in labels:
    p = paras.nextElement()
    if kind == "direct":
        fill(p, k)
    else:
        p.ParaStyleName = "S_" + k

doc.storeToURL(uno.systemPathToFileUrl(out), (pv("FilterName", "writer8"),))
doc.close(True)
