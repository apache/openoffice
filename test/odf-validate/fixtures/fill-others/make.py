# usage: runpy.sh fixtures/fill-others/make.py <port> <outdir>   (office saving ODF 1.2 Extended)
# Other consumers of fills: Writer frame/table/section, Calc cell/page/shape, Draw shape/page, Impress shape/slide.
import sys, os, uno
from unohelp import connect, desktop, pv
from com.sun.star.awt import Gradient, Rectangle
from com.sun.star.awt.GradientStyle import LINEAR
from com.sun.star.drawing.FillStyle import SOLID, GRADIENT
from com.sun.star.awt import Size, Point

port, outdir = int(sys.argv[1]), sys.argv[2]
d = desktop(connect(port))

def grad():
    g = Gradient(); g.Style = LINEAR; g.StartColor = 0xFF0000; g.EndColor = 0x0000FF
    g.Angle = 0; g.Border = 0; g.XOffset = 50; g.YOffset = 50
    g.StartIntensity = 100; g.EndIntensity = 100; g.StepCount = 0
    return g

def new(kind):
    return d.loadComponentFromURL("private:factory/" + kind, "_blank", 0, (pv("Hidden", True),))

def save(doc, name, flt):
    doc.storeToURL(uno.systemPathToFileUrl(os.path.join(outdir, name)), (pv("FilterName", flt),))
    doc.close(True)

def shape(doc, page, x, y, kind, text, fillkind):
    s = doc.createInstance(kind)
    page.add(s)
    s.Position = Point(x, y); s.Size = Size(4000, 2000)
    s.String = text
    if fillkind == "solid":
        s.FillStyle = SOLID; s.FillColor = 0x33AA55
    else:
        s.FillStyle = GRADIENT; s.FillGradient = grad()
    return s

# Writer: frame, table cell, section, with solid and gradient fills
doc = new("swriter")
text = doc.Text
cur = text.createTextCursor()
for i, fk in enumerate(("solid", "gradient")):
    fr = doc.createInstance("com.sun.star.text.TextFrame")
    fr.Width = 4000; fr.Height = 1500
    text.insertTextContent(cur, fr, False)
    fr.Text.String = "frame " + fk
    if fk == "solid":
        fr.FillStyle = SOLID; fr.FillColor = 0x33AA55
    else:
        fr.FillStyle = GRADIENT; fr.FillGradient = grad()
tbl = doc.createInstance("com.sun.star.text.TextTable")
tbl.initialize(2, 2)
text.insertTextContent(text.End, tbl, False)
tbl.getCellByName("A1").BackColor = 0xFFCC00
tbl.getCellByName("B1").BackColor = 0xFFAACC
tbl.BackColor = 0xCCFFCC
sec = doc.createInstance("com.sun.star.text.TextSection")
text.insertTextContent(text.End, sec, False)
sec.BackColor = 0xCCCCFF
save(doc, "others-writer.odt", "writer8")

# Calc: cell back colour, page style back colour, shape with gradient and with text
doc = new("scalc")
sh = doc.Sheets.getByIndex(0)
sh.getCellByPosition(0, 0).String = "cell"
sh.getCellByPosition(0, 0).CellBackColor = 0xFFCC00
sh.getCellByPosition(1, 0).String = "grad"
pst = doc.StyleFamilies.getByName("PageStyles").getByName("Default")
pst.BackColor = 0xDDEEFF
dp = sh.DrawPage
shape(doc, dp, 1000, 3000, "com.sun.star.drawing.RectangleShape", "calc shape", "gradient")
save(doc, "others-calc.ods", "calc8")

# Draw: shapes (solid and gradient), page background
doc = new("sdraw")
page = doc.DrawPages.getByIndex(0)
shape(doc, page, 1000, 1000, "com.sun.star.drawing.RectangleShape", "draw solid", "solid")
shape(doc, page, 6000, 1000, "com.sun.star.drawing.EllipseShape", "draw grad", "gradient")
try:
    page.Background = None
    bg = doc.createInstance("com.sun.star.drawing.Background")
    bg.FillStyle = GRADIENT; bg.FillGradient = grad()
    page.Background = bg
except Exception as e:
    print("draw background:", repr(e))
save(doc, "others-draw.odg", "draw8")

# Impress: shapes with text, slide background
doc = new("simpress")
page = doc.DrawPages.getByIndex(0)
shape(doc, page, 1000, 1000, "com.sun.star.drawing.RectangleShape", "impress solid", "solid")
shape(doc, page, 6000, 1000, "com.sun.star.drawing.RectangleShape", "impress grad", "gradient")
try:
    bg = doc.createInstance("com.sun.star.drawing.Background")
    bg.FillStyle = SOLID; bg.FillColor = 0xCCEEFF
    page.Background = bg
except Exception as e:
    print("impress background:", repr(e))
save(doc, "others-impress.odp", "impress8")
