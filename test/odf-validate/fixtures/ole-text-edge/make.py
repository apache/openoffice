# usage: runpy.sh fixtures/ole-text-edge/make.py <port> <out.odg>   (office saving ODF 1.2 Extended)
# OLE shapes whose text is: none, a label, one space, a line break only, a date field only.
import sys, uno
from unohelp import connect, desktop, pv
from com.sun.star.awt import Size, Point

port, out = int(sys.argv[1]), sys.argv[2]
d = desktop(connect(port))
doc = d.loadComponentFromURL("private:factory/sdraw", "_blank", 0, (pv("Hidden", True),))
page = doc.DrawPages.getByIndex(0)
for i, text in enumerate(("", "OLE label", " ", "\n", None)):
    s = doc.createInstance("com.sun.star.drawing.OLE2Shape")
    s.CLSID = "078B7ABA-54FC-457F-8551-6147e776a997"
    page.add(s)
    s.Position = Point(1000 + i * 5000, 2000); s.Size = Size(4000, 3000)
    if text is None:
        f = doc.createInstance("com.sun.star.text.TextField.DateTime")
        f.IsFixed = True
        f.IsDate = True
        s.insertTextContent(s.createTextCursor(), f, False)
    elif text:
        s.String = text
doc.storeToURL(uno.systemPathToFileUrl(out), (pv("FilterName", "draw8"),))
doc.close(True)
