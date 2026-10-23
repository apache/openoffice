# usage: runpy.sh fixtures/page-fill-1_3/make-calc.py <port> <out-dir>
# Writes calc-src.ods (a Calc page style with a background image and colour) and calc-drawing-page.ods, the same
# file with a usable drawing page style named by its master page. Calc page styles have no fill properties, so the
# drawing page style must not take the page layout's background away.
import os, re, sys, zipfile, uno
from unohelp import connect, desktop, pv

port, outdir = int(sys.argv[1]), sys.argv[2]
png = uno.systemPathToFileUrl(os.path.join(os.path.dirname(os.path.abspath(sys.argv[0])), "tile.png"))
d = desktop(connect(port))
doc = d.loadComponentFromURL("private:factory/scalc", "_blank", 0, (pv("Hidden", True),))
ps = doc.StyleFamilies.getByName("PageStyles")
ps.insertByName("P_image", doc.createInstance("com.sun.star.style.PageStyle"))
s = ps.getByName("P_image")
s.BackColor = 0x66CCFF
s.BackGraphicURL = png
s.BackGraphicLocation = uno.Enum("com.sun.star.style.GraphicLocation", "TILED")
sheet = doc.Sheets.getByIndex(0)
sheet.PageStyle = "P_image"
sheet.getCellByPosition(0, 0).setString("page P_image")
src = os.path.join(outdir, "calc-src.ods")
doc.storeToURL(uno.systemPathToFileUrl(src), (pv("FilterName", "calc8"),))
doc.close(True)

zi = zipfile.ZipFile(src); zo = zipfile.ZipFile(os.path.join(outdir, "calc-drawing-page.ods"), "w")
for it in zi.infolist():
    data = zi.read(it.filename)
    if it.filename == "styles.xml":
        t = data.decode("utf-8")
        m = re.search(r'<style:master-page style:name="P_5f_image"[^>]*?(/?)>', t)
        assert m and t.count("</office:automatic-styles>") == 1
        t = t.replace(m.group(0), m.group(0)[:-len(m.group(1)) - 1] + ' draw:style-name="Mdp9"' + m.group(1) + ">", 1)
        t = t.replace("</office:automatic-styles>", '<style:style style:name="Mdp9" style:family="drawing-page"><style:drawing-page-properties draw:fill="solid" draw:fill-color="#ff0000"/></style:style></office:automatic-styles>', 1)
        data = t.encode("utf-8")
    zo.writestr(it, data, compress_type=zipfile.ZIP_STORED if it.filename == "mimetype" else zipfile.ZIP_DEFLATED)
zo.close()
