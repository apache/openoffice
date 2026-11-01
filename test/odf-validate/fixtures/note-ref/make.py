# usage: runpy.sh fixtures/note-ref/make.py <port> <out.odt> <expected.json>
# Saves with the office's configured ODF version, so run it against the strict profile (drive.sh strict).
import sys, json, uno
from unohelp import connect, desktop, pv

port, out, exp = int(sys.argv[1]), sys.argv[2], sys.argv[3]
d = desktop(connect(port))
doc = d.loadComponentFromURL("private:factory/swriter", "_blank", 0, (pv("Hidden", True),))
text = doc.Text
cur = text.createTextCursor()
text.insertString(cur, "body", False)
fn = doc.createInstance("com.sun.star.text.Footnote")
text.insertTextContent(cur, fn, False)
fn.String = "note text"
en = doc.createInstance("com.sun.star.text.Endnote")
text.insertTextContent(cur, en, False)
en.String = "end text"
text.insertString(cur, " ", False)
# (source: 3 footnote, 4 endnote), part: 0 page, 1 chapter, 2 text, 3 direction, 5/6/7 sequence-only
for src, note in ((3, fn), (4, en)):
    for part in (0, 3, 2, 5, 6, 7):
        f = doc.createInstance("com.sun.star.text.TextField.GetReference")
        f.ReferenceFieldSource = src
        f.SourceName = ""
        f.SequenceNumber = note.ReferenceId
        f.ReferenceFieldPart = part
        text.insertTextContent(cur, f, False)
        text.insertString(cur, " ", False)
doc.TextFields.refresh()
e = doc.TextFields.createEnumeration()
rows = []
while e.hasMoreElements():
    f = e.nextElement()
    if f.supportsService("com.sun.star.text.TextField.GetReference"):
        rows.append([f.ReferenceFieldSource, f.ReferenceFieldPart, f.CurrentPresentation])
json.dump(rows, open(exp, "w"))
doc.storeToURL(uno.systemPathToFileUrl(out), (pv("FilterName", "writer8"),))
doc.close(True)
