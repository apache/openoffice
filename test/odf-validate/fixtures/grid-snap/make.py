# usage: runpy.sh fixtures/grid-snap/make.py <port> <out.odt>   (office saving ODF 1.2 Extended)
import sys, uno
from unohelp import connect, desktop, pv

port, out = int(sys.argv[1]), sys.argv[2]
d = desktop(connect(port))
doc = d.loadComponentFromURL("private:factory/swriter", "_blank", 0, (pv("Hidden", True),))
ps = doc.StyleFamilies.getByName("PageStyles")
specs = [("G_snap_on", True, 2), ("G_snap_off", False, 2), ("G_lines_off", False, 1)]
for name, snap, mode in specs:
    s = doc.createInstance("com.sun.star.style.PageStyle")
    ps.insertByName(name, s)
    s.GridMode = mode; s.StandardPageMode = True; s.GridSnapToChars = snap
text = doc.Text
cur = text.createTextCursor()
PB = uno.getConstantByName("com.sun.star.text.ControlCharacter.PARAGRAPH_BREAK")
for name, *_ in specs:
    text.insertString(cur, "grid " + name, False)
    text.insertControlCharacter(cur, PB, False)
en = text.createEnumeration()
for name, *_ in specs:
    en.nextElement().PageDescName = name
doc.storeToURL(uno.systemPathToFileUrl(out), (pv("FilterName", "writer8"),))
doc.close(True)

# base-width suppresses standard-mode on export and the importer does not infer it, so add it by hand
import zipfile, shutil
tmp = out + ".tmp"
with zipfile.ZipFile(out) as zi, zipfile.ZipFile(tmp, "w") as zo:
    for it in zi.infolist():
        data = zi.read(it.filename)
        if it.filename == "styles.xml":
            data = data.replace(b"style:layout-grid-base-width=", b'style:layout-grid-standard-mode="true" style:layout-grid-base-width=')
        zo.writestr(it, data, compress_type=it.compress_type)
shutil.move(tmp, out)
