# usage: runpy.sh fixtures/normal-values/make.py <port> <out.odt>
import sys, uno
from unohelp import connect, desktop, pv
from com.sun.star.text.TextContentAnchorType import AT_PAGE, AT_PARAGRAPH
from com.sun.star.style.BreakType import PAGE_BEFORE

port, out = int(sys.argv[1]), sys.argv[2]
d = desktop(connect(port))
doc = d.loadComponentFromURL("private:factory/swriter", "_blank", 0, (pv("Hidden", True),))
text = doc.Text

tbl = doc.createInstance("com.sun.star.text.TextTable")
tbl.initialize(2, 3)
text.insertTextContent(text.End, tbl, False)
c = tbl.createCursorByCellName("A1")
c.goRight(1, True)
c.mergeRange()

cur = text.createTextCursor(); cur.gotoEnd(False)
text.insertString(cur, "\nbody\n", False)
cur.gotoEnd(False)
cur.BreakType = PAGE_BEFORE
text.insertString(cur, "page two\n", False)

fn = doc.createInstance("com.sun.star.text.Footnote")
fn.Label = ""
cur.gotoEnd(False)
text.insertTextContent(cur, fn, False)
fn.String = "note text"
text.insertString(cur, " ", False)
for part in (0, 3, 2):
    f = doc.createInstance("com.sun.star.text.TextField.GetReference")
    f.ReferenceFieldSource = 3
    f.SourceName = ""
    f.SequenceNumber = fn.ReferenceId
    f.ReferenceFieldPart = part
    text.insertTextContent(cur, f, False)
    text.insertString(cur, " ", False)

for name, page in (("frame_p2", 2), ("frame_nopage", 0)):
    fr = doc.createInstance("com.sun.star.text.TextFrame")
    fr.AnchorType = AT_PAGE
    if page:
        fr.AnchorPageNo = page
    fr.Width = 3000; fr.Height = 1000
    text.insertTextContent(cur, fr, False)
    fr.setName(name)
    fr.Text.String = name

doc.storeToURL(uno.systemPathToFileUrl(out), (pv("FilterName", "writer8"),))
doc.close(True)
