#**************************************************************
#
#  Licensed to the Apache Software Foundation (ASF) under one
#  or more contributor license agreements.  See the NOTICE file
#  distributed with this work for additional information
#  regarding copyright ownership.  The ASF licenses this file
#  to you under the Apache License, Version 2.0 (the
#  "License"); you may not use this file except in compliance
#  with the License.  You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing,
#  software distributed under the License is distributed on an
#  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
#  KIND, either express or implied.  See the License for the
#  specific language governing permissions and limitations
#  under the License.
#
#**************************************************************
# usage: runpy.sh fixtures/chart/make.py <port> <outdir>
# Charts inside Calc, Writer and Impress/Draw hosts (a standalone chart model crashes the office on store). Saves with the office's configured ODF version.
import sys, os, uno
from unohelp import connect, desktop, pv
from com.sun.star.awt import Rectangle, Size, Point
from com.sun.star.table import CellRangeAddress
from com.sun.star.chart.ChartDataRowSource import COLUMNS, ROWS

port, out = int(sys.argv[1]), sys.argv[2]
os.makedirs(out, exist_ok=True)
ctx = connect(port)
desk = desktop(ctx)
failed = []
FILTERS = {"ods": "calc8", "odt": "writer8", "odp": "impress8", "odg": "draw8"}
TPL = "com.sun.star.chart2.template."
C2 = "com.sun.star.chart2."


def new(factory):
    return desk.loadComponentFromURL(factory, "_blank", 0, (pv("Hidden", True),))


def save(doc, name):
    try:
        doc.storeToURL(uno.systemPathToFileUrl(os.path.join(out, name)),
                       (pv("FilterName", FILTERS[name.rsplit(".", 1)[1]]),))
        print("saved", name)
    except Exception as e:
        failed.append(name)
        print("FAIL save", name, repr(e)[:200])


def attempt(label, fn, *a, **kw):
    try:
        return fn(*a, **kw)
    except Exception as e:
        failed.append(label)
        print("FAIL", label, repr(e)[:300])


def setp(obj, label="", **kw):
    for k, v in kw.items():
        try:
            try:
                obj.setPropertyValue(k, v)
            except AttributeError:
                setattr(obj, k, v)
        except Exception as e:
            print("NOPROP", label, k, repr(e)[:120])


def const(name):
    return uno.getConstantByName(name)


def enum(typ, val):
    return uno.Enum(typ, val)


CATS = ["Q1", "Q2", "Q3", "Q4", "Q5", "Q6"]
BLOCKS = {}


def fill_data(sh):
    def put(col, row, v):
        c = sh.getCellByPosition(col, row)
        if isinstance(v, str):
            c.String = v
        else:
            c.Value = v

    def cats(col, hdr=""):
        put(col, 0, hdr)
        for r, t in enumerate(CATS):
            put(col, r + 1, t)

    def block(col, names, gen):
        for j, n in enumerate(names):
            put(col + j, 0, n)
            for r in range(6):
                put(col + j, r + 1, gen(j, r))

    cats(0)
    block(1, ["north", "south", "east", "west"], lambda j, r: 3 + ((r * (j + 2) * 5 + j * 3) % 9))
    block(6, ["x", "y1", "y2"], lambda j, r: (r + 1) * 1.5 if j == 0 else (r + 1) * (j + 1) + (r % 3))
    block(10, ["bx", "by", "size"], lambda j, r: (r + 1) if j == 0 else ((r * 3 + 2) % 7 + 1 if j == 1 else 2 + (r * 5) % 6))
    cats(15)
    for j, n in enumerate(["open", "low", "high", "close"]):
        put(16 + j, 0, n)
    for r in range(6):
        o = 20 + (r * 3) % 5
        lo, hi = o - 3 - r % 2, o + 4 + r % 3
        cl = o + (2 if r % 2 else -1)
        for j, v in enumerate([o, lo, hi, cl]):
            put(16 + j, r + 1, v)
    cats(21)
    for j, n in enumerate(["volume", "open", "low", "high", "close"]):
        put(22 + j, 0, n)
    for r in range(6):
        o = 20 + (r * 3) % 5
        lo, hi = o - 3 - r % 2, o + 4 + r % 3
        cl = o + (2 if r % 2 else -1)
        for j, v in enumerate([100 + 30 * r, o, lo, hi, cl]):
            put(22 + j, r + 1, v)
    cats(46)
    for j, n in enumerate(["volume", "low", "high", "close"]):
        put(47 + j, 0, n)
    for r in range(6):
        o = 20 + (r * 3) % 5
        for j, v in enumerate([100 + 30 * r, o - 3, o + 4, o + 1 + r % 2]):
            put(47 + j, r + 1, v)
    cats(28)
    for j, n in enumerate(["low", "high", "close"]):
        put(29 + j, 0, n)
    for r in range(6):
        o = 20 + (r * 3) % 5
        for j, v in enumerate([o - 3, o + 4, o + 1 + r % 2]):
            put(29 + j, r + 1, v)
    put(33, 0, "date")
    for j, n in enumerate(["d1", "d2"]):
        put(34 + j, 0, n)
    for r in range(6):
        c = sh.getCellByPosition(33, r + 1)
        c.Value = 40000 + (0, 1, 3, 4, 9, 15)[r]
        put(34, r + 1, 5 + (r * 3) % 7)
        put(35, r + 1, 9 - (r * 2) % 5)
    sh.getCellRangeByName("AH2:AH7").NumberFormat = date_format(sh)
    put(37, 0, "gaps")
    for j, n in enumerate(["g1", "g2"]):
        put(38 + j, 0, n)
    for r in range(6):
        put(37, r + 1, CATS[r])
        if r != 2:
            put(38, r + 1, 4 + r)
        if r != 4:
            put(39, r + 1, 9 - r)
    put(41, 0, "err")
    for j, n in enumerate(["e1", "ep", "en"]):
        put(42 + j, 0, n)
    for r in range(6):
        put(41, r + 1, CATS[r])
        put(42, r + 1, 6 + (r * 4) % 7)
        put(43, r + 1, 0.5 + r * 0.25)
        put(44, r + 1, 0.3 + r * 0.1)


def date_format(sh):
    fm = CURRENT[0].NumberFormats
    loc = uno.createUnoStruct("com.sun.star.lang.Locale", "en", "US", "")
    k = fm.queryKey("YYYY-MM-DD", loc, False)
    return fm.addNew("YYYY-MM-DD", loc) if k == -1 else k


CURRENT = [None]
CHART_CLSID = "12dcae26-281f-416f-a234-c3086127382e"


def set_data(ch, data):
    d = ch.getData()
    d.setData(tuple(tuple(float(v) for v in row) for row in data["rows"]))
    d.setRowDescriptions(tuple(data["cats"]))
    d.setColumnDescriptions(tuple(data["names"]))
RANGES = {
    "cat": "Sheet1.A1:E7", "cat1": "Sheet1.A1:B7", "cat2": "Sheet1.A1:C7", "xy": "Sheet1.G1:I7",
    "bub": "Sheet1.K1:M7", "olhc": "Sheet1.P1:T7", "vol": "Sheet1.V1:AA7", "lhc": "Sheet1.AC1:AF7",
    "vlhc": "Sheet1.AU1:AY7", "date": "Sheet1.AH1:AJ7", "gap": "Sheet1.AL1:AN7", "err": "Sheet1.AP1:AS7",
}


def a1(sh, ref):
    return sh.getCellRangeByName(ref.split(".", 1)[1]).RangeAddress


class Host:
    def __init__(self, name, kind="ods"):
        self.name, self.kind, self.n = name, kind, 0
        self.doc = new({"ods": "private:factory/scalc", "odt": "private:factory/swriter",
                        "odp": "private:factory/simpress", "odg": "private:factory/sdraw"}[kind])
        CURRENT[0] = self.doc
        if kind == "ods":
            self.sh = self.doc.Sheets.getByIndex(0)
            fill_data(self.sh)

    def chart(self, cname, rng, tpl=None, cfg=None, rows=False, cats=True, labels=True, data=None):
        i = self.n
        self.n += 1
        label = "%s/%s" % (self.name, cname)
        if self.kind == "ods":
            rect = Rectangle(500 + (i % 3) * 9500, 8000 + (i // 3) * 7200, 9000, 6800)
            self.sh.Charts.addNewByName(cname, rect, (a1(self.sh, RANGES[rng]),), cats, labels)
            ch = self.sh.Charts.getByName(cname).EmbeddedObject
            if tpl:
                dp = ch.getDataProvider()
                args = (pv("CellRangeRepresentation", RANGES[rng]), pv("DataRowSource", ROWS if rows else COLUMNS),
                        pv("FirstCellAsLabel", labels), pv("HasCategories", cats))
                t = ch.ChartTypeManager.createInstance(TPL + tpl)
                dia = t.createDiagramByDataSource(dp.createDataSource(args), args)
                ch.setFirstDiagram(dia)
        else:
            ch = self.embed(cname, i)
            if data:
                set_data(ch, data)
            if tpl:
                t = ch.ChartTypeManager.createInstance(TPL + tpl)
                t.changeDiagram(ch.getFirstDiagram())
        if cfg:
            cfg(ch)
        return ch

    def embed(self, cname, i):
        if self.kind == "odt":
            shape = self.doc.createInstance("com.sun.star.text.TextEmbeddedObject")
            shape.CLSID = CHART_CLSID
            shape.Name = cname
            self.doc.Text.insertTextContent(self.doc.Text.getEnd(), shape, False)
            return shape.getEmbeddedObject()
        shape = self.doc.createInstance("com.sun.star.drawing.OLE2Shape")
        shape.CLSID = CHART_CLSID
        shape.Position = Point(500 + (i % 2) * 11000, 500 + (i // 2) * 8000)
        shape.Size = Size(10000, 7000)
        page = self.doc.DrawPages.getByIndex(0)
        page.add(shape)
        return shape.Model

    def done(self):
        save(self.doc, "%s.%s" % (self.name, self.kind))
        self.doc.close(True)


def series_of(ch):
    res = []
    for cs in ch.getFirstDiagram().getCoordinateSystems():
        for ct in cs.getChartTypes():
            res.extend(ct.getDataSeries())
    return res


def title(text, sub=None):
    def f(ch):
        ch.HasMainTitle = True
        ch.Title.String = text
        if sub:
            ch.HasSubTitle = True
            ch.SubTitle.String = sub
    return f


def combine(*fs):
    def f(ch):
        for g in fs:
            if g:
                attempt("cfg " + getattr(g, "__name__", "?"), g, ch)
    return f


def legacy(**kw):
    def f(ch):
        setp(ch.getDiagram(), "diagram", **kw)
    return f


def axis(which, **kw):
    def f(ch):
        d = ch.getDiagram()
        setp(getattr(d, which), which, **kw)
    return f


def sprops(idx=None, **kw):
    def f(ch):
        for i, s in enumerate(series_of(ch)):
            if idx is None or i == idx:
                setp(s, "series%d" % i, **kw)
    return f


def types_doc(name, rng, names, extra=None, kind="ods", data=None):
    h = Host(name, kind)
    for n in names:
        attempt("%s/%s" % (name, n), h.chart, n, rng, n, combine(title(n), extra), False, True, True, data)
    h.done()


def topic_types():
    types_doc("types-column", "cat", ["Column", "StackedColumn", "PercentStackedColumn", "Bar", "StackedBar",
              "PercentStackedBar", "ThreeDColumnDeep", "ThreeDColumnFlat", "StackedThreeDColumnFlat",
              "PercentStackedThreeDColumnFlat", "ThreeDBarDeep", "ThreeDBarFlat", "StackedThreeDBarFlat",
              "PercentStackedThreeDBarFlat", "ColumnWithLine", "StackedColumnWithLine"])
    types_doc("types-line", "cat", ["Symbol", "StackedSymbol", "PercentStackedSymbol", "Line", "StackedLine",
              "PercentStackedLine", "LineSymbol", "StackedLineSymbol", "PercentStackedLineSymbol", "ThreeDLine",
              "StackedThreeDLine", "PercentStackedThreeDLine", "ThreeDLineDeep"])
    types_doc("types-area", "cat", ["Area", "StackedArea", "PercentStackedArea", "ThreeDArea", "StackedThreeDArea",
              "PercentStackedThreeDArea"])
    types_doc("types-net", "cat", ["Net", "NetSymbol", "NetLine", "StackedNet", "StackedNetSymbol", "StackedNetLine",
              "PercentStackedNet", "PercentStackedNetSymbol", "PercentStackedNetLine", "FilledNet",
              "StackedFilledNet", "PercentStackedFilledNet"])
    types_doc("types-pie", "cat1", ["Pie", "PieAllExploded", "Donut", "DonutAllExploded", "ThreeDPie",
              "ThreeDPieAllExploded", "ThreeDDonut", "ThreeDDonutAllExploded"])
    h = Host("types-xy")
    for n in ["ScatterLineSymbol", "ScatterLine", "ScatterSymbol", "ThreeDScatter"]:
        attempt("types-xy/" + n, h.chart, n, "xy", n, title(n))
    attempt("types-xy/Bubble", h.chart, "Bubble", "bub", "Bubble", title("Bubble"))
    h.done()
    h = Host("types-stock")
    attempt("stock/lhc", h.chart, "lhc", "lhc", "StockLowHighClose", title("lhc"))
    attempt("stock/olhc", h.chart, "olhc", "olhc", "StockOpenLowHighClose", title("olhc"))
    attempt("stock/vlhc", h.chart, "vlhc", "vlhc", "StockVolumeLowHighClose", title("vlhc"))
    attempt("stock/volhc", h.chart, "volhc", "vol", "StockVolumeOpenLowHighClose", title("volhc"))
    h.done()


def topic_stock():
    def markers(ch):
        d = ch.getDiagram()
        setp(d.getUpBar(), "upbar", FillColor=0x00ff00, LineColor=0x006600, LineWidth=30, FillStyle=enum("com.sun.star.drawing.FillStyle", "SOLID"))
        setp(d.getDownBar(), "downbar", FillColor=0xff0000, LineColor=0x660000, LineStyle=enum("com.sun.star.drawing.LineStyle", "DASH"))
        setp(d.getMinMaxLine(), "minmax", LineColor=0x0000ff, LineWidth=50)
    h = Host("stock-variants")
    attempt("stock/markers", h.chart, "m1", "olhc", "StockOpenLowHighClose", combine(title("markers"), markers))
    attempt("stock/lhc-style", h.chart, "m2", "lhc", "StockLowHighClose", combine(title("lhc styled"), markers))
    attempt("stock/vol-style", h.chart, "m3", "vol", "StockVolumeOpenLowHighClose", combine(title("vol styled"), markers))
    attempt("stock/vol2", h.chart, "m4", "vlhc", "StockVolumeLowHighClose", combine(title("vlhc styled"), markers, axis("YAxis", Logarithmic=True)))
    attempt("stock/updown-off", h.chart, "m5", "olhc", "StockOpenLowHighClose", combine(title("updown off"), legacy(UpDown=False)))
    h.done()


def topic_threed():
    PM = "com.sun.star.drawing.ProjectionMode"
    SM = "com.sun.star.drawing.ShadeMode"
    h = Host("three-d")
    cases = [
        ("flat", dict(D3DSceneShadeMode=enum(SM, "FLAT"), D3DScenePerspective=enum(PM, "PARALLEL"))),
        ("phong", dict(D3DSceneShadeMode=enum(SM, "PHONG"), D3DSceneTwoSidedLighting=True)),
        ("smooth", dict(D3DSceneShadeMode=enum(SM, "SMOOTH"), D3DSceneTwoSidedLighting=False)),
        ("draft", dict(D3DSceneShadeMode=enum(SM, "DRAFT"), D3DSceneTwoSidedLighting=True)),
        ("rot", dict(RightAngledAxes=False, RotationHorizontal=30, RotationVertical=-20, Perspective=40)),
        ("rot2", dict(RightAngledAxes=True, RotationHorizontal=-45, RotationVertical=10, Perspective=80)),
    ]
    for n, kw in cases:
        attempt("3d/" + n, h.chart, n, "cat", "ThreeDColumnDeep", combine(title("3d " + n), legacy(**kw)))
    attempt("3d/lights", h.chart, "lights", "cat", "ThreeDColumnFlat", combine(title("lights"), legacy(
        D3DSceneAmbientColor=0x404040, D3DSceneLightOn1=True, D3DSceneLightColor1=0xffffff, D3DSceneLightOn2=True,
        D3DSceneLightColor2=0xff0000, D3DSceneLightOn3=True, D3DSceneLightColor3=0x00ff00, D3DSceneLightOn4=True,
        D3DSceneLightColor4=0x0000ff)))
    h.done()
    h = Host("solid-types")
    for n, v in (("cylinder", 0), ("cone", 1), ("cuboid", 2), ("pyramid", 3)):
        attempt("solid/" + n, h.chart, n, "cat", "ThreeDColumnDeep", combine(title(n), legacy(SolidType=v)))
        attempt("solid/bar-" + n, h.chart, "b" + n, "cat", "ThreeDBarDeep", combine(title("bar " + n), legacy(SolidType=v)))
    attempt("solid/flat-cone", h.chart, "flatcone", "cat", "ThreeDColumnFlat", combine(title("flat cone"), legacy(SolidType=1)))
    attempt("solid/stacked-cyl", h.chart, "stcyl", "cat", "StackedThreeDColumnFlat", combine(title("stacked cyl"), legacy(SolidType=0)))
    h.done()


def topic_pie():
    h = Host("pie-variants")
    attempt("pie/offset", h.chart, "off", "cat1", "Pie", combine(title("offset"), sprops(0, Offset=0.3)))
    attempt("pie/angle", h.chart, "ang", "cat1", "Pie", combine(title("angle"), legacy(StartingAngle=90)))
    attempt("pie/donut-angle", h.chart, "dang", "cat1", "Donut", combine(title("donut angle"), legacy(StartingAngle=270)))
    attempt("pie/2series-donut", h.chart, "d2", "cat", "Donut", title("two rings"))

    def pt(ch):
        s = series_of(ch)[0]
        for i in range(3):
            p = s.getDataPointByIndex(i)
            setp(p, "pt%d" % i, Offset=0.1 * (i + 1), Color=0x336699 + i * 0x2000)
    attempt("pie/point-offset", h.chart, "ptoff", "cat1", "Pie", combine(title("point offset"), pt))
    h.done()


AX = "com.sun.star.chart.ChartAxis"


def axis_titles(ch):
    d = ch.getDiagram()
    for flag, getter, text in (("HasXAxisTitle", d.getXAxisTitle, "x title"), ("HasYAxisTitle", d.getYAxisTitle, "y title")):
        setp(d, "diagram", **{flag: True})
        getter().String = text


def topic_axes():
    CP = "com.sun.star.chart.ChartAxisPosition"
    LP = "com.sun.star.chart.ChartAxisLabelPosition"
    MP = "com.sun.star.chart.ChartAxisMarkPosition"
    AO = "com.sun.star.chart.ChartAxisArrangeOrderType"
    h = Host("axes")
    A = lambda n, rng, tpl, *fs: attempt("axes/" + n, h.chart, n, rng, tpl, combine(title(n), *fs))
    A("scale", "cat", "Column", axis("YAxis", Min=1.0, Max=14.0, StepMain=2.0, StepHelpCount=4, Origin=1.0,
                                       Marks=3, HelpMarks=3))
    A("marks-none", "cat", "Column", axis("YAxis", Marks=0, HelpMarks=0), axis("XAxis", Marks=1, HelpMarks=2))
    A("marks-in", "cat", "Column", axis("YAxis", Marks=1, HelpMarks=1), axis("XAxis", Marks=2, HelpMarks=0))
    A("log", "cat", "Line", axis("YAxis", Logarithmic=True))
    A("reverse", "cat", "Column", axis("YAxis", ReverseDirection=True), axis("XAxis", ReverseDirection=True))
    A("grids", "cat", "Column", legacy(HasXAxisGrid=True, HasXAxisHelpGrid=True, HasYAxisGrid=True, HasYAxisHelpGrid=True))
    A("nogrid", "cat", "Column", legacy(HasYAxisGrid=False, HasXAxisGrid=False))
    A("hidden-labels", "cat", "Column", axis("XAxis", DisplayLabels=False), axis("YAxis", DisplayLabels=False))
    A("hidden-axes", "cat", "Column", legacy(HasXAxis=False, HasYAxis=False))
    A("titles", "cat", "Column", axis_titles)
    A("rotation", "cat", "Column", axis("XAxis", TextRotation=4500), axis("YAxis", TextRotation=9000))
    A("stacked-text", "cat", "Column", axis("XAxis", StackedText=True))
    A("text-break", "cat", "Column", axis("XAxis", TextBreak=True, TextOverlap=True))
    A("arrange-odd", "cat", "Column", axis("XAxis", ArrangeOrder=enum(AO, "STAGGER_ODD")))
    A("arrange-even", "cat", "Column", axis("XAxis", ArrangeOrder=enum(AO, "STAGGER_EVEN")))
    A("arrange-side", "cat", "Column", axis("XAxis", ArrangeOrder=enum(AO, "SIDE_BY_SIDE")))
    for p in ("START", "END", "VALUE", "ZERO"):
        A("cross-" + p, "cat", "Column", axis("XAxis", CrossoverPosition=enum(CP, p), CrossoverValue=3.0),
          axis("YAxis", CrossoverPosition=enum(CP, p), CrossoverValue=2.0))
    for p in ("NEAR_AXIS", "NEAR_AXIS_OTHER_SIDE", "OUTSIDE_START", "OUTSIDE_END"):
        A("labelpos-" + p, "cat", "Column", axis("XAxis", LabelPosition=enum(LP, p)), axis("YAxis", LabelPosition=enum(LP, p)))
    for p in ("AT_LABELS", "AT_AXIS", "AT_LABELS_AND_AXIS"):
        A("markpos-" + p, "cat", "Column", axis("XAxis", MarkPosition=enum(MP, p)), axis("YAxis", MarkPosition=enum(MP, p)))
    A("numfmt-link", "cat", "Column", axis("YAxis", LinkNumberFormatToSource=True))
    A("numfmt-own", "cat", "Column", axis("YAxis", LinkNumberFormatToSource=False, NumberFormat=1))
    h.done()

    h = Host("axes2")
    A = lambda n, rng, tpl, *fs: attempt("axes2/" + n, h.chart, n, rng, tpl, combine(title(n), *fs))

    def second(ch):
        d = ch.getDiagram()
        d.HasSecondaryYAxis = True
        d.HasSecondaryXAxis = True
        series_of(ch)[1].setPropertyValue("AttachedAxisIndex", 1)
        series_of(ch)[3].setPropertyValue("AttachedAxisIndex", 1)
        setp(d.SecondaryYAxis, "sec", Min=0.0, Max=20.0)
    A("secondary", "cat", "Column", second)
    A("secondary-line", "cat", "Line", second)
    A("secondary-bar", "cat", "Bar", second)
    A("xy-axes", "xy", "ScatterLineSymbol", axis("XAxis", Min=0.0, Max=10.0, StepMain=2.0, Logarithmic=False),
      axis("YAxis", Min=0.0, Max=30.0, StepMain=5.0))
    A("xy-log", "xy", "ScatterSymbol", axis("XAxis", Logarithmic=True), axis("YAxis", Logarithmic=True))
    A("zaxis", "cat", "ThreeDColumnDeep", axis("ZAxis", DisplayLabels=True, Marks=3))
    A("zaxis-title", "cat", "ThreeDColumnDeep", legacy(HasZAxisTitle=True, HasZAxisGrid=True, HasZAxisHelpGrid=True))
    A("net-axes", "cat", "Net", axis("YAxis", Min=0.0, Max=12.0, StepMain=3.0))
    A("hidden-cat", "cat", "Column", legacy(HasXAxis=False))

    def dates(ch):
        ax = ch.getFirstDiagram().getCoordinateSystems()[0].getAxisByDimension(0, 0)
        sd = ax.getScaleData()
        sd.AxisType = const("com.sun.star.chart2.AxisType.DATE")
        sd.AutoDateAxis = False
        ti = uno.createUnoStruct("com.sun.star.chart.TimeIncrement")
        ti.TimeResolution = const("com.sun.star.chart.TimeUnit.DAY")
        ti.MajorTimeInterval = uno.createUnoStruct("com.sun.star.chart.TimeInterval", 2, const("com.sun.star.chart.TimeUnit.DAY"))
        ti.MinorTimeInterval = uno.createUnoStruct("com.sun.star.chart.TimeInterval", 1, const("com.sun.star.chart.TimeUnit.DAY"))
        sd.TimeIncrement = ti
        ax.setScaleData(sd)

    def dates_month(ch):
        ax = ch.getFirstDiagram().getCoordinateSystems()[0].getAxisByDimension(0, 0)
        sd = ax.getScaleData()
        sd.AxisType = const("com.sun.star.chart2.AxisType.DATE")
        sd.AutoDateAxis = True
        ti = uno.createUnoStruct("com.sun.star.chart.TimeIncrement")
        ti.TimeResolution = const("com.sun.star.chart.TimeUnit.MONTH")
        ti.MajorTimeInterval = uno.createUnoStruct("com.sun.star.chart.TimeInterval", 1, const("com.sun.star.chart.TimeUnit.YEAR"))
        ti.MinorTimeInterval = uno.createUnoStruct("com.sun.star.chart.TimeInterval", 3, const("com.sun.star.chart.TimeUnit.MONTH"))
        sd.TimeIncrement = ti
        ax.setScaleData(sd)

    def real_axis(ch):
        ax = ch.getFirstDiagram().getCoordinateSystems()[0].getAxisByDimension(0, 0)
        sd = ax.getScaleData()
        sd.AxisType = const("com.sun.star.chart2.AxisType.REALNUMBER")
        ax.setScaleData(sd)
    A("date-day", "date", "Line", dates)
    A("date-month", "date", "LineSymbol", dates_month)
    A("date-col", "date", "Column", dates)
    A("date-auto", "date", "Line")
    A("xy-date", "date", "ScatterLineSymbol", dates)
    A("cat-as-real", "cat", "Line", real_axis)
    h.done()


def label(num=False, pct=False, cat=False, sym=False):
    return uno.createUnoStruct("com.sun.star.chart2.DataPointLabel", num, pct, cat, sym)


def topic_labels():
    h = Host("labels")
    combos = [("value", label(num=True)), ("pct", label(pct=True)), ("value-pct", label(num=True, pct=True)),
              ("cat", label(cat=True)), ("sym", label(sym=True)), ("all", label(True, True, True, True)),
              ("cat-value", label(num=True, cat=True)), ("sym-value", label(num=True, sym=True)),
              ("pct-cat-sym", label(pct=True, cat=True, sym=True)), ("none", label())]
    for n, lab in combos:
        for tpl, rng in (("Column", "cat"), ("Pie", "cat1")):
            attempt("labels/%s-%s" % (n, tpl), h.chart, n + tpl, rng, tpl, combine(title(n + " " + tpl), sprops(None, Label=lab)))
    h.done()

    h = Host("label-opts")
    seps = [" ", "; ", "\n", ", "]
    for i, sp in enumerate(seps):
        attempt("labsep/%d" % i, h.chart, "sep%d" % i, "cat", "Column",
                combine(title("sep %d" % i), sprops(None, Label=label(num=True, cat=True, pct=True), LabelSeparator=sp)))
    PL = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12]
    for k in range(0, len(PL), 4):
        vals = PL[k:k + 4]

        def f(ch, vals=vals):
            for i, s in enumerate(series_of(ch)):
                if i < len(vals):
                    setp(s, "place", Label=label(num=True), LabelPlacement=vals[i])
        attempt("labplace/%d" % k, h.chart, "place%d" % k, "cat", "LineSymbol", combine(title("placement %d" % k), f))
    attempt("labfmt/own", h.chart, "fmt", "cat", "Column", sprops(None, Label=label(num=True, pct=True), NumberFormat=3, PercentageNumberFormat=10))
    attempt("labrot", h.chart, "rot", "cat", "Column", sprops(None, Label=label(num=True), TextRotation=90.0))

    def pointlabels(ch):
        s = series_of(ch)[0]
        for i in range(3):
            p = ch.getDiagram().getDataPointProperties(i, 0)
            setp(p, "pt%d" % i, DataCaption=(1, 2, 3)[i])
    attempt("labpoint", h.chart, "pt", "cat", "Column", combine(title("point labels"), pointlabels))
    h.done()


def topic_stats():
    EI = "com.sun.star.chart.ChartErrorIndicatorType"
    RC = "com.sun.star.chart.ChartRegressionCurveType"
    h = Host("stats-error")
    styles = [(1, "variance"), (2, "stddev"), (3, "constant"), (4, "percent"), (5, "margin"), (6, "stderr"), (7, "range")]
    inds = ["TOP_AND_BOTTOM", "UPPER", "LOWER"]
    for st, n in styles:
        for ind in inds:
            def f(ch, st=st, ind=ind):
                d = ch.getDiagram()
                for i in range(2):
                    p = d.getDataRowProperties(i)
                    setp(p, "row", ErrorBarStyle=st, ErrorIndicator=enum(EI, ind), ConstantErrorHigh=1.5,
                         ConstantErrorLow=0.5, PercentageError=12.0, ErrorMargin=8.0)
                    if st == 7:
                        setp(p, "range", ErrorBarRangePositive="Sheet1.AR2:AR7", ErrorBarRangeNegative="Sheet1.AS2:AS7")
            kind = "Column" if ind != "LOWER" else "LineSymbol"
            attempt("err/%s-%s" % (n, ind), h.chart, "e%d%s" % (st, ind), "cat", kind, combine(title("%s %s" % (n, ind)), f))
    h.done()

    h = Host("stats-regression")
    for rc in ("LINEAR", "LOGARITHM", "EXPONENTIAL", "POWER"):
        def f(ch, rc=rc):
            d = ch.getDiagram()
            for i in range(2):
                setp(d.getDataRowProperties(i), "row", RegressionCurves=enum(RC, rc))
        attempt("reg/" + rc, h.chart, "r" + rc, "xy", "ScatterSymbol", combine(title(rc), f))
        attempt("reg/line/" + rc, h.chart, "l" + rc, "cat", "LineSymbol", combine(title("line " + rc), f))

    def equation(ch, rc, show_eq, show_r2, pos):
        for s in series_of(ch)[:2]:
            for c in s.getRegressionCurves():
                e = c.getEquationProperties()
                setp(e, "eq", ShowEquation=show_eq, ShowCorrelationCoefficient=show_r2,
                     RelativePosition=uno.createUnoStruct("com.sun.star.chart2.RelativePosition", pos, 0.2,
                                                           uno.Enum("com.sun.star.drawing.Alignment", "TOP_LEFT")),
                     NumberFormat=1)
    for rc in ("LINEAR", "LOGARITHM", "EXPONENTIAL", "POWER"):
        for sh_eq, sh_r in ((True, False), (False, True), (True, True)):
            def f(ch, rc=rc, a=sh_eq, b=sh_r):
                d = ch.getDiagram()
                for i in range(2):
                    setp(d.getDataRowProperties(i), "row", RegressionCurves=enum(RC, rc))
                equation(ch, rc, a, b, 0.3)
            attempt("eq/%s%s%s" % (rc, sh_eq, sh_r), h.chart, "q%s%d%d" % (rc, sh_eq, sh_r), "xy", "ScatterSymbol",
                    combine(title("%s eq=%s r2=%s" % (rc, sh_eq, sh_r)), f))

    def curve_style(ch):
        for s in series_of(ch)[:2]:
            for c in s.getRegressionCurves():
                setp(c, "curve", LineColor=0xff0000, LineWidth=60, LineStyle=enum("com.sun.star.drawing.LineStyle", "DASH"))
        setp(ch.getDiagram().getDataRowProperties(0), "row", RegressionCurves=enum(RC, "LINEAR"))
    attempt("reg/style", h.chart, "style", "xy", "ScatterLineSymbol", combine(title("curve style"), curve_style))
    h.done()

    h = Host("stats-mean")
    for tpl, rng in (("Column", "cat"), ("Line", "cat"), ("ScatterSymbol", "xy"), ("Bar", "cat"), ("Area", "cat")):
        def f(ch):
            d = ch.getDiagram()
            for i in range(2):
                setp(d.getDataRowProperties(i), "row", MeanValue=True)
        attempt("mean/" + tpl, h.chart, "m" + tpl, rng, tpl, combine(title("mean " + tpl), f))

    def mean_style(ch):
        d = ch.getDiagram()
        for i in range(3):
            p = d.getDataRowProperties(i)
            setp(p, "row", MeanValue=True)
            mp = p.getPropertyValue("DataMeanValueProperties")
            setp(mp, "mean", LineColor=0x00aa00 + i * 0x1000, LineWidth=40 * (i + 1))
    attempt("mean/style", h.chart, "mstyle", "cat", "Column", combine(title("mean styled"), mean_style))

    def all_stats(ch):
        d = ch.getDiagram()
        p = d.getDataRowProperties(0)
        setp(p, "row", MeanValue=True, RegressionCurves=enum(RC, "LINEAR"), ErrorBarStyle=3,
             ErrorIndicator=enum(EI, "TOP_AND_BOTTOM"), ConstantErrorHigh=1.0, ConstantErrorLow=1.0)
        equation(ch, "LINEAR", True, True, 0.5)
    attempt("stats/all", h.chart, "all", "xy", "ScatterLineSymbol", combine(title("all stats"), all_stats))
    h.done()


def chart_types(ch):
    return [ct for cs in ch.getFirstDiagram().getCoordinateSystems() for ct in cs.getChartTypes()]


def topic_legend():
    LP = "com.sun.star.chart.ChartLegendPosition"
    LE = "com.sun.star.chart.ChartLegendExpansion"
    h = Host("legend")
    for p in ("LEFT", "TOP", "RIGHT", "BOTTOM", "NONE"):
        def f(ch, p=p):
            ch.HasLegend = True
            setp(ch.Legend, "legend", Alignment=enum(LP, p))
        attempt("legend/" + p, h.chart, "L" + p, "cat", "Column", combine(title("legend " + p, "sub " + p), f))
    for e in ("WIDE", "HIGH", "CUSTOM"):
        def f(ch, e=e):
            ch.HasLegend = True
            setp(ch.Legend, "legend", Expansion=enum(LE, e), Alignment=enum(LP, "TOP"))
            if e == "CUSTOM":
                ch.Legend.Size = Size(5000, 2500)
        attempt("legend/exp" + e, h.chart, "E" + e, "cat", "Line", combine(title("expansion " + e), f))

    def nolegend(ch):
        ch.HasLegend = False
    attempt("legend/off", h.chart, "off", "cat", "Column", combine(title("no legend"), nolegend))

    def legend_style(ch):
        ch.HasLegend = True
        setp(ch.Legend, "legend", FillStyle=enum("com.sun.star.drawing.FillStyle", "SOLID"), FillColor=0xffffcc,
             LineStyle=enum("com.sun.star.drawing.LineStyle", "SOLID"), LineColor=0x333333, CharHeight=14.0,
             CharColor=0x990000, CharWeight=150.0, CharPosture=enum("com.sun.star.awt.FontSlant", "ITALIC"))
    attempt("legend/style", h.chart, "style", "cat", "Column", combine(title("legend style"), legend_style))

    def titles(ch):
        ch.HasMainTitle = True
        ch.HasSubTitle = True
        ch.Title.String = "Main\nTwo lines"
        ch.SubTitle.String = "Subtitle text"
        setp(ch.Title, "title", CharHeight=18.0, CharColor=0x0000ff, CharFontName="Arial", CharWeight=150.0,
             TextRotation=0, CharUnderline=1)
        setp(ch.SubTitle, "sub", CharHeight=11.0, CharPosture=enum("com.sun.star.awt.FontSlant", "ITALIC"), TextRotation=90)
        ch.Title.Position = Point(2000, 100)
        ch.SubTitle.Position = Point(2000, 1200)
    attempt("legend/titles", h.chart, "titles", "cat", "Column", titles)

    def rotated(ch):
        ch.HasMainTitle = True
        ch.Title.String = "rot"
        setp(ch.Title, "title", TextRotation=45)
        axis_titles(ch)
        d = ch.getDiagram()
        setp(d.getXAxisTitle(), "xt", TextRotation=90)
        setp(d.getYAxisTitle(), "yt", TextRotation=270)
    attempt("legend/rot", h.chart, "rot", "cat", "Column", rotated)

    def wall(ch):
        d = ch.getDiagram()
        setp(d.getWall(), "wall", FillStyle=enum("com.sun.star.drawing.FillStyle", "SOLID"), FillColor=0xeeeeff,
             LineStyle=enum("com.sun.star.drawing.LineStyle", "SOLID"), LineColor=0x888888, LineWidth=30)
        setp(d.getFloor(), "floor", FillStyle=enum("com.sun.star.drawing.FillStyle", "SOLID"), FillColor=0xccffcc)
        setp(ch.getPageBackground(), "page", FillStyle=enum("com.sun.star.drawing.FillStyle", "SOLID"), FillColor=0xffeeee)
    attempt("legend/wall", h.chart, "wall", "cat", "Column", combine(title("wall floor"), wall))
    attempt("legend/wall3d", h.chart, "wall3d", "cat", "ThreeDColumnDeep", combine(title("3d wall floor"), wall))
    h.done()


def png_file():
    import zlib, struct, tempfile
    w = h = 8
    raw = b"".join(b"\x00" + b"".join(bytes((30 * x, 30 * y, 160)) for x in range(w)) for y in range(h))

    def chunk(t, d):
        c = struct.pack(">I", len(d)) + t + d
        return c + struct.pack(">I", zlib.crc32(t + d) & 0xffffffff)
    data = b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", w, h, 8, 2, 0, 0, 0)) + chunk(b"IDAT", zlib.compress(raw)) + chunk(b"IEND", b"")
    fd, path = tempfile.mkstemp(suffix=".png")
    os.write(fd, data)
    os.close(fd)
    return path


def topic_symbols():
    global PNG
    PNG = png_file()
    SS = "com.sun.star.chart2.SymbolStyle"
    h = Host("symbols")

    def sym(style, n=0, size=300, fill=0xff6600, border=0x003366):
        s = uno.createUnoStruct("com.sun.star.chart2.Symbol")
        s.Style = enum(SS, style)
        s.StandardSymbol = n
        s.Size = Size(size, size)
        s.FillColor = fill
        s.BorderColor = border
        return s

    for k in range(0, 15, 4):
        def f(ch, k=k):
            for i, s in enumerate(series_of(ch)):
                setp(s, "sym", Symbol=sym("STANDARD", k + i if k + i < 15 else 0, 200 + 100 * i))
        attempt("sym/%d" % k, h.chart, "s%d" % k, "cat", "LineSymbol", combine(title("symbols %d.." % k), f))

    def none_auto(ch):
        ss = series_of(ch)
        setp(ss[0], "s", Symbol=sym("NONE"))
        setp(ss[1], "s", Symbol=sym("AUTO"))
    attempt("sym/none-auto", h.chart, "na", "cat", "LineSymbol", combine(title("none auto"), none_auto))

    def graphic(ch):
        gp = ctx.ServiceManager.createInstanceWithContext("com.sun.star.graphic.GraphicProvider", ctx)
        g = gp.queryGraphic((pv("URL", uno.systemPathToFileUrl(PNG)),))
        s = sym("GRAPHIC")
        s.Graphic = g
        setp(series_of(ch)[0], "s", Symbol=s)
    attempt("sym/graphic", h.chart, "gr", "cat", "LineSymbol", combine(title("graphic"), graphic))
    attempt("sym/scatter", h.chart, "sc", "xy", "ScatterSymbol", combine(title("scatter sym"), sprops(0, Symbol=sym("STANDARD", 5, 400))))
    for n, v in (("none", 0), ("cubic", 1), ("bspline", 2)):
        def f(ch, v=v):
            setp(ch.getDiagram(), "d", SplineType=v, SplineOrder=3 + v, SplineResolution=20 + 10 * v)
        attempt("interp/" + n, h.chart, "i" + n, "cat", "LineSymbol", combine(title("spline " + n), f))
        attempt("interp/xy-" + n, h.chart, "x" + n, "xy", "ScatterLineSymbol", combine(title("xy spline " + n), f))
    attempt("sym/lines-off", h.chart, "nolines", "xy", "ScatterSymbol", combine(title("lines off"), legacy(Lines=False)))
    attempt("sym/lines-on", h.chart, "lines", "cat", "Symbol", combine(title("lines on"), legacy(Lines=True)))
    h.done()


def topic_data():
    h = Host("data")
    A = lambda n, rng, tpl, cats, labels, *fs, rows=False: attempt("data/" + n, h.chart, n, rng, tpl,
        combine(title(n), *fs), rows, cats, labels)
    A("no-labels", "cat", "Column", False, False)
    A("col-labels", "cat", "Column", False, True)
    A("row-labels", "cat", "Column", True, False)
    A("both", "cat", "Column", True, True)
    A("rows", "cat", "Column", True, True, rows=True)
    A("rows-nolabels", "cat", "Line", False, False, rows=True)
    for n, v in (("gap", 0), ("zero", 1), ("cont", 2)):
        A("missing-" + n, "gap", "LineSymbol", True, True, legacy(MissingValueTreatment=v))
        A("missing-col-" + n, "gap", "Column", True, True, legacy(MissingValueTreatment=v))
        A("missing-area-" + n, "gap", "Area", True, True, legacy(MissingValueTreatment=v))
    A("hidden-on", "cat", "Column", True, True, legacy(IncludeHiddenCells=True))
    A("hidden-off", "cat", "Column", True, True, legacy(IncludeHiddenCells=False))
    A("sort-x", "xy", "ScatterLineSymbol", True, True, legacy(SortByXValues=True))
    A("gapwidth", "cat", "Column", True, True, axis("YAxis", GapWidth=150, Overlap=-30))
    A("gapwidth-bar", "cat", "Bar", True, True, axis("YAxis", GapWidth=20, Overlap=100))
    A("gapwidth-2axes", "cat", "Column", True, True, lambda ch: [
        setp(ch.getDiagram(), "d", HasSecondaryYAxis=True), series_of(ch)[1].setPropertyValue("AttachedAxisIndex", 1),
        setp(ch.getDiagram().SecondaryYAxis, "y2", GapWidth=70, Overlap=20)])
    A("connect", "cat", "StackedColumn", True, True, legacy(StackedBarsConnected=True))
    A("connect-bar", "cat", "StackedBar", True, True, legacy(StackedBarsConnected=True))

    def group(ch):
        d = ch.getDiagram()
        d.HasSecondaryYAxis = True
        series_of(ch)[1].setPropertyValue("AttachedAxisIndex", 1)
        setp(d, "d", GroupBarsPerAxis=False)
    A("group-bars", "cat", "Column", True, True, group)
    A("group-bars-on", "cat", "Column", True, True, lambda ch: [setp(ch.getDiagram(), "d", GroupBarsPerAxis=True), group(ch)])
    A("scale-text", "cat", "Column", True, True, axis("XAxis", ScaleText=True), axis("YAxis", ScaleText=True),
      lambda ch: setp(ch.Title, "t", ScaleText=True))
    A("lineseries", "cat", "ColumnWithLine", True, True, legacy(NumberOfLines=2))
    A("lineseries-stacked", "cat", "StackedColumnWithLine", True, True, legacy(NumberOfLines=3))
    A("percent-off", "cat", "StackedColumn", True, True, legacy(Percent=False, Stacked=True))
    A("stack-off", "cat", "Column", True, True, legacy(Stacked=False, Percent=False))
    A("vertical-on", "cat", "Column", True, True, legacy(Vertical=True))
    A("area-deep", "cat", "ThreeDArea", True, True, legacy(Deep=True))
    A("area-flat", "cat", "ThreeDArea", True, True, legacy(Deep=False))
    A("line3d-flat", "cat", "ThreeDLine", True, True, legacy(Deep=False))
    A("dim3d-off", "cat", "ThreeDColumnDeep", True, True, legacy(Dim3D=False))
    A("bubble-hdr", "bub", "Bubble", True, False)
    A("scatter-nocats", "xy", "ScatterSymbol", False, True)
    A("pie-rows", "cat", "Pie", True, True, rows=True)
    h.done()

    h = Host("point-props")

    def points(ch):
        d = ch.getDiagram()
        for i in range(4):
            p = d.getDataPointProperties(i, 0)
            setp(p, "pt", FillColor=0x203060 + i * 0x302010, LineColor=0x101010, LineWidth=20 * i,
                 FillStyle=enum("com.sun.star.drawing.FillStyle", "SOLID"), DataCaption=i % 4,
                 LineStyle=enum("com.sun.star.drawing.LineStyle", ("SOLID", "DASH", "NONE", "SOLID")[i]),
                 FillTransparence=10 * i, LineTransparence=5 * i)
    for t in ("Column", "Line", "Area", "Bar", "StackedColumn"):
        attempt("pts/" + t, h.chart, "pts" + t, "cat", t, combine(title("points " + t), points))

    def series_style(ch):
        for i, s in enumerate(series_of(ch)):
            setp(s, "s", Color=0x113355 + i * 0x303030, FillStyle=enum("com.sun.star.drawing.FillStyle", ("SOLID", "GRADIENT", "HATCH", "SOLID")[i]),
                 LineStyle=enum("com.sun.star.drawing.LineStyle", "SOLID"), LineWidth=30 * (i + 1), Transparency=20 * i,
                 CharHeight=9.0 + i, CharColor=0x000000 + i * 0x101010, CharFontName="Helvetica",
                 CharWeight=100.0 + 20 * i, CharUnderline=i)
    attempt("series/style", h.chart, "ss", "cat", "Column", combine(title("series style"), series_style))
    attempt("series/style-line", h.chart, "ssl", "cat", "LineSymbol", combine(title("series style line"), series_style))

    def fills(ch):
        ss = series_of(ch)
        g = uno.createUnoStruct("com.sun.star.awt.Gradient")
        g.Style = enum("com.sun.star.awt.GradientStyle", "LINEAR")
        g.StartColor = 0xff0000
        g.EndColor = 0x0000ff
        g.Angle = 300
        g.Border = 5
        g.XOffset = 50
        g.YOffset = 50
        g.StartIntensity = 100
        g.EndIntensity = 100
        g.StepCount = 0
        ch.createInstance("com.sun.star.drawing.GradientTable").insertByName("grad1", g)
        setp(ss[0], "g", FillStyle=enum("com.sun.star.drawing.FillStyle", "GRADIENT"), GradientName="grad1")
        hs = uno.createUnoStruct("com.sun.star.drawing.Hatch")
        hs.Style = enum("com.sun.star.drawing.HatchStyle", "DOUBLE")
        hs.Color = 0x00aa00
        hs.Distance = 100
        hs.Angle = 450
        ch.createInstance("com.sun.star.drawing.HatchTable").insertByName("hatch1", hs)
        setp(ss[1], "h", FillStyle=enum("com.sun.star.drawing.FillStyle", "HATCH"), HatchName="hatch1", FillBackground=True,
             Color=0xffffaa)
    attempt("series/gradient-hatch", h.chart, "grad", "cat", "Column", combine(title("gradient hatch"), fills))
    h.done()


D5 = {"cats": CATS[:5], "names": ["north", "south", "east"],
      "rows": [(4, 7, 5), (6, 3, 8), (5, 9, 4), (8, 6, 7), (3, 5, 9)]}
DX = {"cats": ["1", "2", "3", "4", "5"], "names": ["y1", "y2"], "rows": [(2, 3), (4, 3.5), (5, 6), (7, 5.5), (9, 8)]}
DS = {"cats": CATS[:5], "names": ["open", "low", "high", "close"],
      "rows": [(20, 17, 24, 22), (21, 18, 26, 20), (22, 19, 25, 24), (23, 18, 27, 25), (21, 17, 26, 19)]}
DV = {"cats": CATS[:5], "names": ["volume", "open", "low", "high", "close"],
      "rows": [(120, 20, 17, 24, 22), (90, 21, 18, 26, 20), (150, 22, 19, 25, 24), (80, 23, 18, 27, 25), (110, 21, 17, 26, 19)]}
EMBED = [
    ("Column", D5), ("StackedColumn", D5), ("PercentStackedBar", D5), ("Line", D5), ("LineSymbol", D5), ("Area", D5),
    ("StackedArea", D5), ("Pie", {"cats": CATS[:5], "names": ["share"], "rows": [(4,), (6,), (5,), (8,), (3,)]}),
    ("Donut", D5), ("ScatterLineSymbol", DX), ("Net", D5), ("FilledNet", D5), ("StockOpenLowHighClose", DS),
    ("StockVolumeOpenLowHighClose", DV), ("StockLowHighClose", {"cats": CATS[:5], "names": ["low", "high", "close"],
        "rows": [(17, 24, 22), (18, 26, 20), (19, 25, 24), (18, 27, 25), (17, 26, 19)]}),
    ("ThreeDColumnDeep", D5), ("ThreeDPie", {"cats": CATS[:5], "names": ["share"], "rows": [(4,), (6,), (5,), (8,), (3,)]}),
    ("ThreeDArea", D5), ("ColumnWithLine", D5), ("Bar", D5),
]


def embedded_doc(name, kind, items, extra=None):
    h = Host(name, kind)
    for i, (tpl, data) in enumerate(items):
        attempt("%s/%s" % (name, tpl), h.chart, "%s%d" % (tpl, i), None, tpl, combine(title(tpl, "sub " + tpl), extra), data=data)
    h.done()


def topic_hosts():
    def legend_labels(ch):
        ch.HasLegend = True
        sprops(None, Label=label(num=True))(ch)
    embedded_doc("embed-writer", "odt", EMBED[:10], legend_labels)
    embedded_doc("embed-writer2", "odt", EMBED[10:], axis_titles)
    embedded_doc("embed-impress", "odp", EMBED[:10], legend_labels)
    embedded_doc("embed-impress2", "odp", EMBED[10:], None)
    embedded_doc("embed-draw", "odg", EMBED[:8])
    embedded_doc("embed-draw2", "odg", EMBED[8:])


def topic_writer_table():
    doc = new("private:factory/swriter")
    CURRENT[0] = doc
    try:
        for rows in (5, 5):
            t = doc.createInstance("com.sun.star.text.TextTable")
            t.initialize(rows, 4)
            doc.Text.insertTextContent(doc.Text.getEnd(), t, False)
            for c in "BCD":
                t.getCellByName(c + "1").String = "s" + c
            for r in range(2, rows + 1):
                t.getCellByName("A%d" % r).String = "c%d" % r
                for j, c in enumerate("BCD"):
                    t.getCellByName("%s%d" % (c, r)).Value = (r * (j + 2) * 3) % 9 + 1
            doc.Text.insertControlCharacter(doc.Text.getEnd(), uno.getConstantByName("com.sun.star.text.ControlCharacter.PARAGRAPH_BREAK"), False)
    except Exception as e:
        failed.append("writer-table")
        import traceback
        traceback.print_exc()
        print("FAIL writer table", repr(e)[:200])
    dp = doc.createInstance("com.sun.star.chart2.data.DataProvider")
    print("writer dp", dp is not None)
    for i, (tn, tpl) in enumerate((("Table1", "Column"), ("Table1", "Line"), ("Table2", "Pie"), ("Table2", "StockOpenLowHighClose"),
                                   ("Table1", "Bar"), ("Table2", "Area"))):
        def mk(tn=tn, tpl=tpl, i=i):
            shape = doc.createInstance("com.sun.star.text.TextEmbeddedObject")
            shape.CLSID = CHART_CLSID
            shape.Name = "wc%d" % i
            doc.Text.insertTextContent(doc.Text.getEnd(), shape, False)
            ch = shape.getEmbeddedObject()
            ch.attachDataProvider(doc.createInstance("com.sun.star.chart2.data.DataProvider"))
            rng = tn + ".A1:D5" if tpl != "Pie" else tn + ".A1:B5"
            args = (pv("CellRangeRepresentation", rng), pv("DataRowSource", COLUMNS), pv("FirstCellAsLabel", True), pv("HasCategories", True))
            ch.setArguments(args)
            t = ch.ChartTypeManager.createInstance(TPL + tpl)
            ch.setFirstDiagram(t.createDiagramByDataSource(ch.getDataProvider().createDataSource(args), args))
            title("table chart %s %s" % (tn, tpl))(ch)
            ch.HasLegend = True
        attempt("writer-table/%s-%s" % (tn, tpl), mk)
    save(doc, "embed-writer-table.odt")
    doc.close(True)


for _t in (topic_types, topic_stock, topic_threed, topic_pie, topic_axes, topic_labels, topic_stats, topic_legend,
           topic_symbols, topic_data, topic_hosts, topic_writer_table):
    attempt(_t.__name__, _t)
try:
    os.remove(PNG)
except Exception:
    pass
print("failed:", failed)
sys.exit(1 if failed else 0)
