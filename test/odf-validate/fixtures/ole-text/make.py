# usage: runpy.sh fixtures/ole-text/make.py <port> <out.odg>   (office saving ODF 1.2 Extended)
# Draw page with an OLE object without text and one with text.
import sys, uno
from unohelp import connect, desktop, pv
from com.sun.star.awt import Size, Point

port, out = int(sys.argv[1]), sys.argv[2]
d = desktop(connect(port))
doc = d.loadComponentFromURL("private:factory/sdraw", "_blank", 0, (pv("Hidden", True),))
page = doc.DrawPages.getByIndex(0)
for i, text in enumerate(("", "OLE label")):
    s = doc.createInstance("com.sun.star.drawing.OLE2Shape")
    s.CLSID = "078B7ABA-54FC-457F-8551-6147e776a997"
    page.add(s)
    s.Position = Point(2000 + i * 8000, 2000); s.Size = Size(5000, 3000)
    if text:
        s.String = text
doc.storeToURL(uno.systemPathToFileUrl(out), (pv("FilterName", "draw8"),))
doc.close(True)
