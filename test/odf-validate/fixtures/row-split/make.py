# usage: runpy.sh fixtures/row-split/make.py <port> <out.odt>   (office saving ODF 1.2 Extended)
import sys, uno
from unohelp import connect, desktop, pv

port, out = int(sys.argv[1]), sys.argv[2]
d = desktop(connect(port))
doc = d.loadComponentFromURL("private:factory/swriter", "_blank", 0, (pv("Hidden", True),))
tbl = doc.createInstance("com.sun.star.text.TextTable")
tbl.initialize(3, 2)
doc.Text.insertTextContent(doc.Text.End, tbl, False)
rows = tbl.Rows
rows.getByIndex(0).IsSplitAllowed = True
rows.getByIndex(1).IsSplitAllowed = False
rows.getByIndex(2).IsSplitAllowed = True
rows.getByIndex(2).Height = 1000
doc.storeToURL(uno.systemPathToFileUrl(out), (pv("FilterName", "writer8"),))
doc.close(True)
