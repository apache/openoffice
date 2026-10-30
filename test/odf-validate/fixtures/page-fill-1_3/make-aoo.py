# usage: runpy.sh fixtures/page-fill-1_3/make-aoo.py <port> <out.odt>   (office saving in its default mode)
# Same page styles and fills as gen.bas, built through UNO in AOO.
import os, sys, uno
from unohelp import connect, desktop, pv
from com.sun.star.awt import Gradient
from com.sun.star.awt.GradientStyle import LINEAR
from com.sun.star.drawing import Hatch
from com.sun.star.drawing.HatchStyle import SINGLE
from com.sun.star.drawing.FillStyle import SOLID, GRADIENT, HATCH, BITMAP
from com.sun.star.drawing.BitmapMode import REPEAT, STRETCH, NO_REPEAT

port, out = int(sys.argv[1]), sys.argv[2]
png = uno.systemPathToFileUrl(os.path.join(os.path.dirname(os.path.abspath(sys.argv[0])), "tile.png"))
KINDS = "solid,solid_transp,gradient,gradient_transp,gradient_tgrad,hatch,hatch_nobg,bitmap_tile,bitmap_stretch,bitmap_once,none,shared".split(",")
d = desktop(connect(port))
doc = d.loadComponentFromURL("private:factory/swriter", "_blank", 0, (pv("Hidden", True),))

def gr(c1, c2, b):
    g = Gradient(); g.Style = LINEAR; g.StartColor = c1; g.EndColor = c2
    g.Angle = 0; g.Border = b; g.XOffset = 50; g.YOffset = 50
    g.StartIntensity = 100; g.EndIntensity = 100; g.StepCount = 0
    return g

def fill(o, pre, kind):
    sp = lambda n, v: o.setPropertyValue(pre + n, v)
    if kind == "solid":
        sp("FillStyle", SOLID); sp("FillColor", 0x33AA55)
    elif kind == "solid_transp":
        sp("FillStyle", SOLID); sp("FillColor", 0x3355AA); sp("FillTransparence", 40)
    elif kind in ("gradient", "shared"):
        sp("FillStyle", GRADIENT); sp("FillGradient", gr(0xFF0000, 0x0000FF, 10))
    elif kind == "gradient_transp":
        sp("FillStyle", GRADIENT); sp("FillGradient", gr(0xFF0000, 0x0000FF, 10)); sp("FillTransparence", 30)
    elif kind == "gradient_tgrad":
        sp("FillStyle", GRADIENT); sp("FillGradient", gr(0x00FF00, 0xFF00FF, 0))
        tg = doc.createInstance("com.sun.star.drawing.TransparencyGradientTable")
        if not tg.hasByName("TG1"):
            tg.insertByName("TG1", gr(0x000000, 0xFFFFFF, 0))
        sp("FillTransparenceGradientName", "TG1")
    elif kind in ("hatch", "hatch_nobg"):
        h = Hatch(); h.Style = SINGLE; h.Color = 0x00AA00; h.Distance = 200; h.Angle = 450
        sp("FillStyle", HATCH); sp("FillHatch", h)
        if kind == "hatch":
            sp("FillColor", 0xFFFFCC); sp("FillBackground", True)
        else:
            sp("FillBackground", False)
    elif kind.startswith("bitmap"):
        sp("FillStyle", BITMAP); sp("FillBitmapURL", png)
        sp("FillBitmapMode", {"bitmap_tile": REPEAT, "bitmap_stretch": STRETCH, "bitmap_once": NO_REPEAT}[kind])

ps = doc.StyleFamilies.getByName("PageStyles")
for k in KINDS:
    ps.insertByName("P_" + k, doc.createInstance("com.sun.star.style.PageStyle"))
    if k != "none":
        fill(ps.getByName("P_" + k), "", k)
ps.getByName("P_shared").FollowStyle = "P_shared"
ps.insertByName("P_shared2", doc.createInstance("com.sun.star.style.PageStyle"))
fill(ps.getByName("P_shared2"), "", "shared")
s = ps.getByName("P_gradient"); s.HeaderIsOn = True; fill(s, "Header", "hatch")
fill(ps.getByName("Standard"), "", "solid")
fill(ps.getByName("First Page"), "", "hatch_nobg")

text = doc.Text
cur = text.createTextCursor()
PB = uno.getConstantByName("com.sun.star.text.ControlCharacter.PARAGRAPH_BREAK")
names = ["First Page"] + ["P_" + k for k in KINDS] + ["P_shared2"]
for i, n in enumerate(names):
    text.insertString(cur, "page " + n, False)
    if i < len(names) - 1:
        text.insertControlCharacter(cur, PB, False)
en = text.createEnumeration()
for n in names:
    en.nextElement().PageDescName = n

doc.storeToURL(uno.systemPathToFileUrl(out), (pv("FilterName", "writer8"),))
doc.close(True)
