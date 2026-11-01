# usage: runpy.sh fixtures/boolean-writer/make.py <port> <out.odt>
import sys, uno
from unohelp import connect, desktop, pv
from com.sun.star.lang import Locale
from com.sun.star.util.NumberFormat import LOGICAL

port, out = int(sys.argv[1]), sys.argv[2]
d = desktop(connect(port))
doc = d.loadComponentFromURL("private:factory/swriter", "_blank", 0, (pv("Hidden", True),))
key = doc.NumberFormats.getStandardFormat(LOGICAL, Locale("en", "US", ""))
tbl = doc.createInstance("com.sun.star.text.TextTable")
tbl.initialize(1, 3)
doc.Text.insertTextContent(doc.Text.End, tbl, False)
for i, v in enumerate((12345, 1, 0)):
    c = tbl.getCellByPosition(i, 0)
    c.NumberFormat = key
    c.setValue(v)
cur = doc.Text.createTextCursor(); cur.gotoEnd(False)
for i, v in enumerate((12345.0, 1.0, 0.0)):
    f = doc.createInstance("com.sun.star.text.TextField.SetExpression")
    mst = doc.createInstance("com.sun.star.text.FieldMaster.SetExpression")
    mst.Name = "bv%d" % i
    mst.SubType = 0
    f.Content = ""
    f.NumberFormat = key
    f.SubType = 0
    f.IsShowFormula = False
    f.attachTextFieldMaster(mst)
    doc.Text.insertTextContent(cur, f, False)
    f.Value = v
    doc.Text.insertString(cur, " ", False)
doc.storeToURL(uno.systemPathToFileUrl(out), (pv("FilterName", "writer8"),))
doc.close(True)
