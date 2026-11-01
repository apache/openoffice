# usage: runpy.sh fixtures/normal-values5b/make.py <port> <out.ods> <symbol.png>
import sys, uno
from unohelp import connect, desktop, pv
from com.sun.star.awt import Rectangle
from com.sun.star.table import CellRangeAddress

port, out, png = int(sys.argv[1]), sys.argv[2], sys.argv[3]
ctx = connect(port)
d = desktop(ctx)
doc = d.loadComponentFromURL("private:factory/scalc", "_blank", 0, (pv("Hidden", True),))
sh = doc.Sheets.getByIndex(0)
for r, row in enumerate([["", "a", "b"], ["x", 1, 2], ["y", 3, 5], ["z", 4, 8]]):
    for c, v in enumerate(row):
        cell = sh.getCellByPosition(c, r)
        if isinstance(v, str): cell.String = v
        else: cell.Value = v
rng = sh.getCellRangeByName("A1:C4").getRangeAddress()
r = Rectangle(); r.X, r.Y, r.Width, r.Height = 8000, 1000, 9000, 7000
sh.Charts.addNewByName("c1", r, (rng,), True, True)
m = sh.Charts.getByName("c1").EmbeddedObject
gp = ctx.ServiceManager.createInstanceWithContext("com.sun.star.graphic.GraphicProvider", ctx)
g = gp.queryGraphic((pv("URL", uno.systemPathToFileUrl(png)),))
series = m.getFirstDiagram().getCoordinateSystems()[0].getChartTypes()[0].getDataSeries()
s = series[0]
sym = s.Symbol
sym.Style = uno.Enum("com.sun.star.chart2.SymbolStyle", "GRAPHIC")
sym.Graphic = g
s.Symbol = sym
print("readback", s.Symbol.Style, s.Symbol.Graphic, g, s.SymbolBitmapURL if hasattr(s,"SymbolBitmapURL") else None)
sh.getColumns().getByIndex(2).IsVisible = False
rng2 = sh.getCellRangeByName("A1:C4").getRangeAddress()
r2 = Rectangle(); r2.X, r2.Y, r2.Width, r2.Height = 8000, 9000, 9000, 7000
sh.Charts.addNewByName("c2", r2, (rng2,), True, True)
doc.storeToURL(uno.systemPathToFileUrl(out), (pv("FilterName", "calc8"),))
doc.close(True)
