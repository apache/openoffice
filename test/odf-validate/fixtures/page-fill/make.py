# usage: runpy.sh fixtures/page-fill/make.py <port> <out.odt> <png>   (office saving ODF 1.2 Extended)
import sys, uno
from unohelp import connect, desktop, pv
from com.sun.star.awt import Gradient
from com.sun.star.awt.GradientStyle import LINEAR
from com.sun.star.drawing import Hatch
from com.sun.star.drawing.HatchStyle import SINGLE
from com.sun.star.drawing.FillStyle import SOLID, GRADIENT, HATCH, BITMAP

port, out, png = int(sys.argv[1]), sys.argv[2], uno.systemPathToFileUrl(sys.argv[3])
d = desktop(connect(port))
doc = d.loadComponentFromURL("private:factory/swriter", "_blank", 0, (pv("Hidden", True),))

def grad():
    g = Gradient(); g.Style = LINEAR; g.StartColor = 0xFF0000; g.EndColor = 0x0000FF
    g.Angle = 0; g.Border = 0; g.XOffset = 50; g.YOffset = 50
    g.StartIntensity = 100; g.EndIntensity = 100; g.StepCount = 0
    return g

def hatch():
    h = Hatch(); h.Style = SINGLE; h.Color = 0x00AA00; h.Distance = 200; h.Angle = 450
    return h

def fill(o, pre, kind, color=0xFFCC00):
    if kind == "solid":
        setattr(o, pre + "FillStyle", SOLID); setattr(o, pre + "FillColor", color)
    elif kind == "solid_transp":
        setattr(o, pre + "FillStyle", SOLID); setattr(o, pre + "FillColor", color); setattr(o, pre + "FillTransparence", 40)
    elif kind == "gradient":
        setattr(o, pre + "FillStyle", GRADIENT); setattr(o, pre + "FillGradient", grad())
    elif kind == "hatch":
        setattr(o, pre + "FillStyle", HATCH); setattr(o, pre + "FillHatch", hatch())
        setattr(o, pre + "FillColor", 0xFFFFCC); setattr(o, pre + "FillBackground", True)
    elif kind == "bitmap":
        setattr(o, pre + "FillStyle", BITMAP); setattr(o, pre + "FillBitmapURL", png)

ps = doc.StyleFamilies.getByName("PageStyles")
specs = [("P_solid", "solid", "solid", "solid"), ("P_transp", "solid_transp", "solid_transp", "solid_transp"),
         ("P_grad", "gradient", "gradient", "hatch"), ("P_bitmap", "bitmap", "bitmap", "bitmap")]
for name, pg, hd, ft in specs:
    s = doc.createInstance("com.sun.star.style.PageStyle")
    ps.insertByName(name, s)
    s.HeaderIsOn = True; s.FooterIsOn = True
    fill(s, "", pg, 0x66CCFF); fill(s, "Header", hd, 0xCCFF66); fill(s, "Footer", ft, 0xFF66CC)

text = doc.Text
cur = text.createTextCursor()
PB = uno.getConstantByName("com.sun.star.text.ControlCharacter.PARAGRAPH_BREAK")
paras = []
for name, *_ in specs:
    text.insertString(cur, "page " + name, False)
    text.insertControlCharacter(cur, PB, False)
    paras.append(name)
en = text.createEnumeration()
for name in paras:
    p = en.nextElement()
    p.PageDescName = name

doc.storeToURL(uno.systemPathToFileUrl(out), (pv("FilterName", "writer8"),))
doc.close(True)
