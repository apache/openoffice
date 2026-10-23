# usage: runpy.sh fixtures/alien-attrs/make.py <port> <outdir>   (office saving ODF 1.2 Extended)
# Preserved alien attributes in ODF namespaces and in a foreign namespace on the carriers that keep them:
# Writer paragraph, character, section, table and table cell (item-set export path), Draw shape, Calc cell.
import sys, os, uno
from unohelp import connect, desktop, pv
from com.sun.star.awt import Size, Point

port, outdir = int(sys.argv[1]), sys.argv[2]
d = desktop(connect(port))
NS = {"text": "urn:oasis:names:tc:opendocument:xmlns:text:1.0",
      "style": "urn:oasis:names:tc:opendocument:xmlns:style:1.0",
      "draw": "urn:oasis:names:tc:opendocument:xmlns:drawing:1.0",
      "table": "urn:oasis:names:tc:opendocument:xmlns:table:1.0",
      "office": "urn:oasis:names:tc:opendocument:xmlns:office:1.0",
      "svg": "urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0",
      "fo": "http://www.w3.org/1999/XSL/Format",
      "ex": "urn:example:foreign"}
ATTRS = (("text", "enable-numbering"), ("style", "alien-style"), ("draw", "alien-draw"), ("table", "alien-table"),
         ("office", "alien-office"), ("svg", "alien-svg"), ("fo", "alien-fo"), ("ex", "alien-foreign"))

def put(container):
    for p, n in ATTRS:
        a = uno.createUnoStruct("com.sun.star.xml.AttributeData")
        a.Namespace = NS[p]; a.Type = "CDATA"; a.Value = "v-" + p
        container.insertByName(p + ":" + n, a)

def new(kind):
    return d.loadComponentFromURL("private:factory/" + kind, "_blank", 0, (pv("Hidden", True),))

def save(doc, name, flt):
    doc.storeToURL(uno.systemPathToFileUrl(os.path.join(outdir, name)), (pv("FilterName", flt),))
    doc.close(True)

doc = new("swriter")
text = doc.Text
cur = text.createTextCursor()
text.insertString(cur, "para text", False)
text.insertControlCharacter(cur, 0, False)
cur.gotoStart(False); cur.goRight(4, True)
cont = cur.ParaUserDefinedAttributes
put(cont); cur.ParaUserDefinedAttributes = cont
cont = cur.TextUserDefinedAttributes
put(cont); cur.TextUserDefinedAttributes = cont
cur.gotoEnd(False)
tbl = doc.createInstance("com.sun.star.text.TextTable")
tbl.initialize(2, 2)
text.insertTextContent(cur, tbl, False)
for tgt in (tbl, tbl.getCellByName("A1")):
    cont = tgt.UserDefinedAttributes
    put(cont); tgt.UserDefinedAttributes = cont
sec = doc.createInstance("com.sun.star.text.TextSection")
text.insertTextContent(text.End, sec, False)
cont = sec.UserDefinedAttributes
put(cont); sec.UserDefinedAttributes = cont
save(doc, "alien-writer.odt", "writer8")

doc = new("sdraw")
page = doc.DrawPages.getByIndex(0)
s = doc.createInstance("com.sun.star.drawing.RectangleShape")
page.add(s); s.Position = Point(2000, 2000); s.Size = Size(4000, 2000); s.String = "alien"
cont = s.UserDefinedAttributes
put(cont); s.UserDefinedAttributes = cont
save(doc, "alien-draw.odg", "draw8")

doc = new("scalc")
c = doc.Sheets.getByIndex(0).getCellByPosition(0, 0)
c.Value = 1
cont = c.UserDefinedAttributes
put(cont); c.UserDefinedAttributes = cont
save(doc, "alien-calc.ods", "calc8")
