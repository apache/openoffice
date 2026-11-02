################################################################
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

"""Builders for documents the tests start from: each creates one or more documents through the API and stores them. Ported from the
ODF harness's fixtures/*/make.py; the expected values read back from them are in data/checks/."""
import os
import re
import zipfile

import uno
from com.sun.star.awt import Gradient, Point, Size
from com.sun.star.awt.GradientStyle import LINEAR
from com.sun.star.drawing import Hatch
from com.sun.star.drawing.BitmapMode import NO_REPEAT, REPEAT, STRETCH
from com.sun.star.drawing.FillStyle import BITMAP, GRADIENT, HATCH, SOLID
from com.sun.star.drawing.HatchStyle import SINGLE

from aootest_worker.ops import core
from aootest_worker.ops.core import pv
from aootest_worker.ops.encrypted import _png


def _grad():
    g = Gradient()
    g.Style = LINEAR
    g.StartColor, g.EndColor = 0xFF0000, 0x0000FF
    g.Angle = g.Border = 0
    g.XOffset = g.YOffset = 50
    g.StartIntensity = g.EndIntensity = 100
    g.StepCount = 0
    return g


def _new(kind):
    doc = core._desktop().loadComponentFromURL("private:factory/" + kind, "_blank", 0, (pv("Hidden", True),))
    if doc is None:
        raise core.Fail("uno", "could not create a new %s document" % kind)
    return doc


def _save(doc, outdir, name, flt):
    try:
        doc.storeToURL(uno.systemPathToFileUrl(os.path.join(outdir, name)), (pv("FilterName", flt),))
    finally:
        core._close(doc)
    return name


def page_fill(outdir):
    """Writer page styles whose page, header and footer carry each kind of fill (fixtures/page-fill/make.py)."""
    png = os.path.join(outdir, "tile.png")
    _png(png)
    png_url = uno.systemPathToFileUrl(png)
    doc = _new("swriter")

    def hatch():
        h = Hatch()
        h.Style, h.Color, h.Distance, h.Angle = SINGLE, 0x00AA00, 200, 450
        return h

    def fill(o, pre, kind, color=0xFFCC00):
        if kind == "solid":
            setattr(o, pre + "FillStyle", SOLID)
            setattr(o, pre + "FillColor", color)
        elif kind == "solid_transp":
            setattr(o, pre + "FillStyle", SOLID)
            setattr(o, pre + "FillColor", color)
            setattr(o, pre + "FillTransparence", 40)
        elif kind == "gradient":
            setattr(o, pre + "FillStyle", GRADIENT)
            setattr(o, pre + "FillGradient", _grad())
        elif kind == "hatch":
            setattr(o, pre + "FillStyle", HATCH)
            setattr(o, pre + "FillHatch", hatch())
            setattr(o, pre + "FillColor", 0xFFFFCC)
            setattr(o, pre + "FillBackground", True)
        elif kind == "bitmap":
            setattr(o, pre + "FillStyle", BITMAP)
            setattr(o, pre + "FillBitmapURL", png_url)

    ps = doc.StyleFamilies.getByName("PageStyles")
    specs = [("P_solid", "solid", "solid", "solid"), ("P_transp", "solid_transp", "solid_transp", "solid_transp"),
             ("P_grad", "gradient", "gradient", "hatch"), ("P_bitmap", "bitmap", "bitmap", "bitmap")]
    for name, pg, hd, ft in specs:
        s = doc.createInstance("com.sun.star.style.PageStyle")
        ps.insertByName(name, s)
        s.HeaderIsOn = True
        s.FooterIsOn = True
        fill(s, "", pg, 0x66CCFF)
        fill(s, "Header", hd, 0xCCFF66)
        fill(s, "Footer", ft, 0xFF66CC)
    text = doc.Text
    cur = text.createTextCursor()
    pb = uno.getConstantByName("com.sun.star.text.ControlCharacter.PARAGRAPH_BREAK")
    for name, *_ in specs:
        text.insertString(cur, "page " + name, False)
        text.insertControlCharacter(cur, pb, False)
    en = text.createEnumeration()
    for name, *_ in specs:
        en.nextElement().PageDescName = name
    return [_save(doc, outdir, "page-fill.odt", "writer8")]


def fill_others(outdir):
    """Other consumers of fills: Writer frame/table/section, Calc cell/page/shape, Draw shape/page, Impress shape/slide."""
    def shape(doc, page, x, y, kind, text, fillkind):
        s = doc.createInstance(kind)
        page.add(s)
        s.Position = Point(x, y)
        s.Size = Size(4000, 2000)
        s.String = text
        if fillkind == "solid":
            s.FillStyle = SOLID
            s.FillColor = 0x33AA55
        else:
            s.FillStyle = GRADIENT
            s.FillGradient = _grad()
        return s

    done = []
    doc = _new("swriter")
    text = doc.Text
    cur = text.createTextCursor()
    for fk in ("solid", "gradient"):
        fr = doc.createInstance("com.sun.star.text.TextFrame")
        fr.Width, fr.Height = 4000, 1500
        text.insertTextContent(cur, fr, False)
        fr.Text.String = "frame " + fk
        if fk == "solid":
            fr.FillStyle = SOLID
            fr.FillColor = 0x33AA55
        else:
            fr.FillStyle = GRADIENT
            fr.FillGradient = _grad()
    tbl = doc.createInstance("com.sun.star.text.TextTable")
    tbl.initialize(2, 2)
    text.insertTextContent(text.End, tbl, False)
    tbl.getCellByName("A1").BackColor = 0xFFCC00
    tbl.getCellByName("B1").BackColor = 0xFFAACC
    tbl.BackColor = 0xCCFFCC
    sec = doc.createInstance("com.sun.star.text.TextSection")
    text.insertTextContent(text.End, sec, False)
    sec.BackColor = 0xCCCCFF
    done.append(_save(doc, outdir, "others-writer.odt", "writer8"))

    doc = _new("scalc")
    sh = doc.Sheets.getByIndex(0)
    sh.getCellByPosition(0, 0).String = "cell"
    sh.getCellByPosition(0, 0).CellBackColor = 0xFFCC00
    sh.getCellByPosition(1, 0).String = "grad"
    doc.StyleFamilies.getByName("PageStyles").getByName("Default").BackColor = 0xDDEEFF
    shape(doc, sh.DrawPage, 1000, 3000, "com.sun.star.drawing.RectangleShape", "calc shape", "gradient")
    done.append(_save(doc, outdir, "others-calc.ods", "calc8"))

    doc = _new("sdraw")
    page = doc.DrawPages.getByIndex(0)
    shape(doc, page, 1000, 1000, "com.sun.star.drawing.RectangleShape", "draw solid", "solid")
    shape(doc, page, 6000, 1000, "com.sun.star.drawing.EllipseShape", "draw grad", "gradient")
    try:
        page.Background = None
        bg = doc.createInstance("com.sun.star.drawing.Background")
        bg.FillStyle = GRADIENT
        bg.FillGradient = _grad()
        page.Background = bg
    except Exception:
        pass
    done.append(_save(doc, outdir, "others-draw.odg", "draw8"))

    doc = _new("simpress")
    page = doc.DrawPages.getByIndex(0)
    shape(doc, page, 1000, 1000, "com.sun.star.drawing.RectangleShape", "impress solid", "solid")
    shape(doc, page, 6000, 1000, "com.sun.star.drawing.RectangleShape", "impress grad", "gradient")
    try:
        bg = doc.createInstance("com.sun.star.drawing.Background")
        bg.FillStyle = SOLID
        bg.FillColor = 0xCCEEFF
        page.Background = bg
    except Exception:
        pass
    done.append(_save(doc, outdir, "others-impress.odp", "impress8"))
    return done


def t13(outdir):
    """The features ODF 1.3 changes: drop-down fields, annotation initials, Calc tab colour, Calc page scale-to, chart inner
    plot position and legend size (fixtures/t13/make.py). Everything is fixed: no dates from the clock, nothing random."""
    def dt(y, mo, d, h, mi):
        return uno.createUnoStruct("com.sun.star.util.DateTime", 0, 0, mi, h, d, mo, y)

    def ensure_sheets(doc, n):
        sheets = doc.Sheets
        while sheets.Count > n:
            sheets.removeByName(sheets.getByIndex(sheets.Count - 1).Name)
        while sheets.Count < n:
            sheets.insertNewByName("Sheet%d" % (sheets.Count + 1), sheets.Count)
        return sheets

    def put_field(doc, cur, kind, **props):
        f = doc.createInstance("com.sun.star.text.TextField." + kind)
        for k, v in props.items():
            setattr(f, k, v)
        doc.Text.insertTextContent(cur, f, False)

    done = []
    doc = _new("swriter")
    cur = doc.Text.createTextCursor()
    doc.Text.insertString(cur, "First: ", False)
    put_field(doc, cur, "DropDown", Name="dd1", Items=("one", "two", "three"), SelectedItem="two", Help="help one", Tooltip="tip one")
    doc.Text.insertControlCharacter(cur, 0, False)
    doc.Text.insertString(cur, "Second: ", False)
    # SwDropDownField accepts only a listed SelectedItem: "zzz" reads back as "" (nothing selected, the field shows "a")
    put_field(doc, cur, "DropDown", Name="dd2", Items=("a", "b"), SelectedItem="zzz", Help="help two", Tooltip="tip two")
    done.append(_save(doc, outdir, "dropdown.odt", "writer8"))

    doc = _new("swriter")
    cur = doc.Text.createTextCursor()
    doc.Text.insertString(cur, "with initials ", False)
    put_field(doc, cur, "Annotation", Author="Ann Author", Initials="AB", Content="first note", DateTimeValue=dt(2024, 7, 8, 9, 10))
    doc.Text.insertControlCharacter(cur, 0, False)
    doc.Text.insertString(cur, "without initials ", False)
    put_field(doc, cur, "Annotation", Author="Bob Author", Content="second note", DateTimeValue=dt(2024, 7, 9, 9, 10))
    done.append(_save(doc, outdir, "annot.odt", "writer8"))

    doc = _new("scalc")
    sheets = ensure_sheets(doc, 3)
    for i, color in ((0, 0xFF0000), (2, 0x3366CC)):
        sheets.getByIndex(i).TabColor = color
    for i in range(3):
        sheets.getByIndex(i).getCellByPosition(0, 0).setString("sheet %d" % i)
    done.append(_save(doc, outdir, "tabcolor.ods", "calc8"))

    doc = _new("scalc")
    ps = doc.StyleFamilies.getByName("PageStyles")
    sheets = ensure_sheets(doc, 4)
    # setting ScaleToPagesX/Y clears ScaleToPages and PageScale in the style (sc/source/ui/unoobj/styleuno.cxx)
    for name, props in (("PS_xy", {"ScaleToPagesX": 2, "ScaleToPagesY": 3}), ("PS_x", {"ScaleToPagesX": 2}),
                        ("PS_pages", {"ScaleToPages": 4}), ("PS_scale", {"PageScale": 80})):
        ps.insertByName(name, doc.createInstance("com.sun.star.style.PageStyle"))
        st = ps.getByName(name)
        for k, v in props.items():
            setattr(st, k, v)
    for i, name in enumerate(("PS_xy", "PS_x", "PS_pages", "PS_scale")):
        sh = sheets.getByIndex(i)
        sh.PageStyle = name
        sh.getCellByPosition(0, 0).setString(name)
    done.append(_save(doc, outdir, "scaleto.ods", "calc8"))

    doc = _new("scalc")
    sh = doc.Sheets.getByIndex(0)
    for r in range(3):
        sh.getCellByPosition(0, r).setString("c%d" % r)
        sh.getCellByPosition(1, r).setValue(r + 1)
    rect = uno.createUnoStruct("com.sun.star.awt.Rectangle", 1000, 1000, 12000, 9000)
    sh.Charts.addNewByName("C1", rect, (sh.getCellRangeByName("A1:B3").RangeAddress,), True, True)
    ch = sh.Charts.getByName("C1").EmbeddedObject
    ch.HasLegend = True
    dg = ch.Diagram
    # inner plot position in 1/100 mm; the legend is positioned and given a custom size
    dg.setDiagramPositionExcludingAxes(uno.createUnoStruct("com.sun.star.awt.Rectangle", 1500, 1200, 6000, 4500))
    lg = ch.Legend
    lg.Expansion = uno.Enum("com.sun.star.chart.ChartLegendExpansion", "CUSTOM")
    lg.setSize(uno.createUnoStruct("com.sun.star.awt.Size", 2500, 1800))
    lg.setPosition(uno.createUnoStruct("com.sun.star.awt.Point", 8800, 1500))
    done.append(_save(doc, outdir, "chart.ods", "calc8"))
    return done


def page_fill_13(outdir):
    """Page styles with each kind of fill, built in Writer (aoo-src.odt), and a Calc page style with a background image and
    colour (calc-src.ods) with a copy whose master page names a usable drawing page style (fixtures/page-fill-1_3)."""
    png = os.path.join(outdir, "tile.png")
    _png(png)
    png_url = uno.systemPathToFileUrl(png)
    kinds = "solid,solid_transp,gradient,gradient_transp,gradient_tgrad,hatch,hatch_nobg,bitmap_tile,bitmap_stretch,bitmap_once,none,shared".split(",")
    doc = _new("swriter")

    def gr(c1, c2, b):
        g = Gradient()
        g.Style, g.StartColor, g.EndColor = LINEAR, c1, c2
        g.Angle, g.Border, g.XOffset, g.YOffset = 0, b, 50, 50
        g.StartIntensity = g.EndIntensity = 100
        g.StepCount = 0
        return g

    def fill(o, pre, kind):
        def sp(n, v):
            o.setPropertyValue(pre + n, v)
        if kind == "solid":
            sp("FillStyle", SOLID)
            sp("FillColor", 0x33AA55)
        elif kind == "solid_transp":
            sp("FillStyle", SOLID)
            sp("FillColor", 0x3355AA)
            sp("FillTransparence", 40)
        elif kind in ("gradient", "shared"):
            sp("FillStyle", GRADIENT)
            sp("FillGradient", gr(0xFF0000, 0x0000FF, 10))
        elif kind == "gradient_transp":
            sp("FillStyle", GRADIENT)
            sp("FillGradient", gr(0xFF0000, 0x0000FF, 10))
            sp("FillTransparence", 30)
        elif kind == "gradient_tgrad":
            sp("FillStyle", GRADIENT)
            sp("FillGradient", gr(0x00FF00, 0xFF00FF, 0))
            tg = doc.createInstance("com.sun.star.drawing.TransparencyGradientTable")
            if not tg.hasByName("TG1"):
                tg.insertByName("TG1", gr(0x000000, 0xFFFFFF, 0))
            sp("FillTransparenceGradientName", "TG1")
        elif kind in ("hatch", "hatch_nobg"):
            h = Hatch()
            h.Style, h.Color, h.Distance, h.Angle = SINGLE, 0x00AA00, 200, 450
            sp("FillStyle", HATCH)
            sp("FillHatch", h)
            if kind == "hatch":
                sp("FillColor", 0xFFFFCC)
                sp("FillBackground", True)
            else:
                sp("FillBackground", False)
        elif kind.startswith("bitmap"):
            sp("FillStyle", BITMAP)
            sp("FillBitmapURL", png_url)
            sp("FillBitmapMode", {"bitmap_tile": REPEAT, "bitmap_stretch": STRETCH, "bitmap_once": NO_REPEAT}[kind])

    ps = doc.StyleFamilies.getByName("PageStyles")
    for k in kinds:
        ps.insertByName("P_" + k, doc.createInstance("com.sun.star.style.PageStyle"))
        if k != "none":
            fill(ps.getByName("P_" + k), "", k)
    ps.getByName("P_shared").FollowStyle = "P_shared"
    ps.insertByName("P_shared2", doc.createInstance("com.sun.star.style.PageStyle"))
    fill(ps.getByName("P_shared2"), "", "shared")
    s = ps.getByName("P_gradient")
    s.HeaderIsOn = True
    fill(s, "Header", "hatch")
    fill(ps.getByName("Standard"), "", "solid")
    fill(ps.getByName("First Page"), "", "hatch_nobg")
    text = doc.Text
    cur = text.createTextCursor()
    pb = uno.getConstantByName("com.sun.star.text.ControlCharacter.PARAGRAPH_BREAK")
    names = ["First Page"] + ["P_" + k for k in kinds] + ["P_shared2"]
    for i, n in enumerate(names):
        text.insertString(cur, "page " + n, False)
        if i < len(names) - 1:
            text.insertControlCharacter(cur, pb, False)
    en = text.createEnumeration()
    for n in names:
        en.nextElement().PageDescName = n
    done = [_save(doc, outdir, "aoo-src.odt", "writer8")]

    doc = _new("scalc")
    ps = doc.StyleFamilies.getByName("PageStyles")
    ps.insertByName("P_image", doc.createInstance("com.sun.star.style.PageStyle"))
    s = ps.getByName("P_image")
    s.BackColor = 0x66CCFF
    s.BackGraphicURL = png_url
    s.BackGraphicLocation = uno.Enum("com.sun.star.style.GraphicLocation", "TILED")
    sheet = doc.Sheets.getByIndex(0)
    sheet.PageStyle = "P_image"
    sheet.getCellByPosition(0, 0).setString("page P_image")
    done.append(_save(doc, outdir, "calc-src.ods", "calc8"))

    src = os.path.join(outdir, "calc-src.ods")
    with zipfile.ZipFile(src) as zi, zipfile.ZipFile(os.path.join(outdir, "calc-drawing-page.ods"), "w") as zo:
        for it in zi.infolist():
            data = zi.read(it.filename)
            if it.filename == "styles.xml":
                t = data.decode("utf-8")
                m = re.search(r'<style:master-page style:name="P_5f_image"[^>]*?(/?)>', t)
                if not (m and t.count("</office:automatic-styles>") == 1):
                    raise core.Fail("uno", "calc-src.ods has an unexpected styles.xml: no master page P_5f_image")
                t = t.replace(m.group(0), m.group(0)[:-len(m.group(1)) - 1] + ' draw:style-name="Mdp9"' + m.group(1) + ">", 1)
                t = t.replace("</office:automatic-styles>", '<style:style style:name="Mdp9" style:family="drawing-page"><style:drawing-page-properties '
                              'draw:fill="solid" draw:fill-color="#ff0000"/></style:style></office:automatic-styles>', 1)
                data = t.encode("utf-8")
            zo.writestr(it, data, compress_type=zipfile.ZIP_STORED if it.filename == "mimetype" else zipfile.ZIP_DEFLATED)
    done.append("calc-drawing-page.ods")
    return done


def row_split(outdir):
    doc = _new("swriter")
    tbl = doc.createInstance("com.sun.star.text.TextTable")
    tbl.initialize(3, 2)
    doc.Text.insertTextContent(doc.Text.End, tbl, False)
    rows = tbl.Rows
    rows.getByIndex(0).IsSplitAllowed = True
    rows.getByIndex(1).IsSplitAllowed = False
    rows.getByIndex(2).IsSplitAllowed = True
    rows.getByIndex(2).Height = 1000
    return [_save(doc, outdir, "row-split.odt", "writer8")]


def grid_snap(outdir):
    doc = _new("swriter")
    ps = doc.StyleFamilies.getByName("PageStyles")
    specs = [("G_snap_on", True, 2), ("G_snap_off", False, 2), ("G_lines_off", False, 1)]
    for name, snap, mode in specs:
        s = doc.createInstance("com.sun.star.style.PageStyle")
        ps.insertByName(name, s)
        s.GridMode = mode
        s.StandardPageMode = True
        s.GridSnapToChars = snap
    text = doc.Text
    cur = text.createTextCursor()
    pb = uno.getConstantByName("com.sun.star.text.ControlCharacter.PARAGRAPH_BREAK")
    for name, *_ in specs:
        text.insertString(cur, "grid " + name, False)
        text.insertControlCharacter(cur, pb, False)
    en = text.createEnumeration()
    for name, *_ in specs:
        en.nextElement().PageDescName = name
    out = os.path.join(outdir, "grid-snap.odt")
    _save(doc, outdir, "grid-snap.odt", "writer8")
    # older builds did not write standard-mode next to base-width and the importer does not infer it, so add it where it is missing
    tmp = out + ".tmp"
    tag = re.compile(rb"<[^<>]*style:layout-grid-base-width=[^<>]*>")
    with zipfile.ZipFile(out) as zi, zipfile.ZipFile(tmp, "w") as zo:
        for it in zi.infolist():
            data = zi.read(it.filename)
            if it.filename == "styles.xml":
                data = tag.sub(lambda m: m.group(0) if b"style:layout-grid-standard-mode=" in m.group(0)
                               else m.group(0).replace(b"style:layout-grid-base-width=", b'style:layout-grid-standard-mode="true" style:layout-grid-base-width=', 1), data)
            zo.writestr(it, data, compress_type=it.compress_type)
    os.replace(tmp, out)
    return ["grid-snap.odt"]


def ole_text(outdir):
    """A Draw page with an OLE object without text and one with text."""
    doc = _new("sdraw")
    page = doc.DrawPages.getByIndex(0)
    for i, text in enumerate(("", "OLE label")):
        s = doc.createInstance("com.sun.star.drawing.OLE2Shape")
        s.CLSID = "078B7ABA-54FC-457F-8551-6147e776a997"
        page.add(s)
        s.Position = Point(2000 + i * 8000, 2000)
        s.Size = Size(5000, 3000)
        if text:
            s.String = text
    return [_save(doc, outdir, "ole-text.odg", "draw8")]


def ole_text_edge(outdir):
    """OLE shapes whose text is: none, a label, one space, a line break only, a date field only."""
    doc = _new("sdraw")
    page = doc.DrawPages.getByIndex(0)
    for i, text in enumerate(("", "OLE label", " ", "\n", None)):
        s = doc.createInstance("com.sun.star.drawing.OLE2Shape")
        s.CLSID = "078B7ABA-54FC-457F-8551-6147e776a997"
        page.add(s)
        s.Position = Point(1000 + i * 5000, 2000)
        s.Size = Size(4000, 3000)
        if text is None:
            f = doc.createInstance("com.sun.star.text.TextField.DateTime")
            f.IsFixed = True
            f.IsDate = True
            s.insertTextContent(s.createTextCursor(), f, False)
        elif text:
            s.String = text
    return [_save(doc, outdir, "ole-text-edge.odg", "draw8")]


def chart_shapes(outdir):
    """A Calc chart with additional shapes (polyline, rectangle, text) in the chart draw page."""
    from com.sun.star.awt import Rectangle
    from com.sun.star.table import CellRangeAddress
    doc = _new("scalc")
    sh = doc.Sheets.getByIndex(0)
    for r, (a, b) in enumerate(((1, 2), (2, 4), (3, 3))):
        sh.getCellByPosition(0, r).Value = a
        sh.getCellByPosition(1, r).Value = b
    rng = CellRangeAddress()
    rng.Sheet, rng.StartColumn, rng.StartRow, rng.EndColumn, rng.EndRow = 0, 0, 0, 1, 2
    r = Rectangle()
    r.X, r.Y, r.Width, r.Height = 1000, 4000, 10000, 7000
    sh.Charts.addNewByName("c1", r, (rng,), False, False)
    ch = sh.Charts.getByName("c1").EmbeddedObject
    page = ch.DrawPage

    def mk(kind, x, y, w, h):
        s = ch.createInstance(kind)
        page.add(s)
        s.Position = Point(x, y)
        s.Size = Size(w, h)
        return s
    pl = mk("com.sun.star.drawing.PolyLineShape", 500, 500, 3000, 1500)
    pl.PolyPolygon = ((Point(0, 1500), Point(1000, 500), Point(3000, 0)),)
    mk("com.sun.star.drawing.RectangleShape", 4000, 500, 2000, 1000).String = "rect"
    mk("com.sun.star.drawing.TextShape", 6500, 500, 2000, 800).String = "note"
    return [_save(doc, outdir, "chart-shapes.ods", "calc8")]


def normal_values_5b(outdir):
    """Two charts, one with a graphic as the series symbol, and a hidden column."""
    from com.sun.star.awt import Rectangle
    png = os.path.join(outdir, "symbol.png")
    _png(png)
    doc = _new("scalc")
    sh = doc.Sheets.getByIndex(0)
    for r, row in enumerate([["", "a", "b"], ["x", 1, 2], ["y", 3, 5], ["z", 4, 8]]):
        for c, v in enumerate(row):
            cell = sh.getCellByPosition(c, r)
            if isinstance(v, str):
                cell.String = v
            else:
                cell.Value = v
    rng = sh.getCellRangeByName("A1:C4").getRangeAddress()
    r = Rectangle()
    r.X, r.Y, r.Width, r.Height = 8000, 1000, 9000, 7000
    sh.Charts.addNewByName("c1", r, (rng,), True, True)
    m = sh.Charts.getByName("c1").EmbeddedObject
    gp = core._create("com.sun.star.graphic.GraphicProvider")
    g = gp.queryGraphic((pv("URL", uno.systemPathToFileUrl(png)),))
    s = m.getFirstDiagram().getCoordinateSystems()[0].getChartTypes()[0].getDataSeries()[0]
    sym = s.Symbol
    sym.Style = uno.Enum("com.sun.star.chart2.SymbolStyle", "GRAPHIC")
    sym.Graphic = g
    s.Symbol = sym
    sh.getColumns().getByIndex(2).IsVisible = False
    r2 = Rectangle()
    r2.X, r2.Y, r2.Width, r2.Height = 8000, 9000, 9000, 7000
    sh.Charts.addNewByName("c2", r2, (sh.getCellRangeByName("A1:C4").getRangeAddress(),), True, True)
    return [_save(doc, outdir, "normal5b.ods", "calc8")]


def connector_viewbox(outdir):
    """Connectors of every kind, connected and free, axis-aligned and diagonal, in Draw and Impress."""
    from com.sun.star.drawing.ConnectorType import CURVE, LINE, LINES, STANDARD

    def build(doc, page):
        def rect(x, y, w, h):
            s = doc.createInstance("com.sun.star.drawing.RectangleShape")
            page.add(s)
            s.Position = Point(x, y)
            s.Size = Size(w, h)
            return s

        def conn(kind, a, b, name):
            c = doc.createInstance("com.sun.star.drawing.ConnectorShape")
            page.add(c)
            c.EdgeKind = kind
            c.StartPosition = Point(*a)
            c.EndPosition = Point(*b)
            c.Name = name
            return c
        r1, r2 = rect(1000, 12000, 3000, 2000), rect(12000, 16000, 3000, 2000)
        r3, r4 = rect(1000, 20000, 3000, 2000), rect(9000, 24000, 3000, 2000)
        conn(STANDARD, (2000, 2000), (8000, 6000), "std_free")
        conn(CURVE, (10000, 2000), (16000, 6000), "curve_free")
        conn(LINE, (2000, 8000), (8000, 8000), "line_h")
        conn(LINE, (10000, 8000), (10000, 11000), "line_v")
        conn(LINE, (18000, 9000), (14000, 5000), "line_rev")
        conn(STANDARD, (2000, 28000), (8000, 28000), "std_h")
        conn(STANDARD, (16000, 28000), (16000, 32000), "std_v")
        c = conn(STANDARD, (0, 0), (0, 0), "std_conn")
        c.StartShape, c.EndShape = r1, r2
        c = conn(CURVE, (0, 0), (0, 0), "curve_conn")
        c.StartShape, c.EndShape = r3, r4
        conn(LINES, (2000, 34000), (9000, 38000), "lines_free")

    done = []
    for kind, name, flt in (("sdraw", "conn.odg", "draw8"), ("simpress", "conn.odp", "impress8")):
        doc = _new(kind)
        build(doc, doc.DrawPages.getByIndex(0))
        done.append(_save(doc, outdir, name, flt))
    return done


def para_fill(outdir):
    """Paragraphs and paragraph styles with each kind of fill, direct and through a style (fixtures/para-fill/make.py)."""
    png = os.path.join(outdir, "tile.png")
    _png(png)
    png_url = uno.systemPathToFileUrl(png)
    doc = _new("swriter")
    text = doc.Text

    def hatch():
        h = Hatch()
        h.Style, h.Color, h.Distance, h.Angle = SINGLE, 0x00AA00, 200, 450
        return h

    def fill(o, kind):
        if kind == "solid":
            o.ParaBackColor = 0xFFCC00
            o.ParaBackTransparent = False
        elif kind == "solid_fill":
            o.FillStyle = SOLID
            o.FillColor = 0x33AA55
        elif kind == "solid_transp":
            o.FillStyle = SOLID
            o.FillColor = 0x3355AA
            o.FillTransparence = 40
        elif kind == "gradient":
            o.FillStyle = GRADIENT
            o.FillGradient = _grad()
        elif kind == "hatch":
            o.FillStyle = HATCH
            o.FillHatch = hatch()
            o.FillColor = 0xFFFFCC
            o.FillBackground = True
        elif kind == "bitmap":
            o.FillStyle = BITMAP
            o.FillBitmapURL = png_url

    kinds = ["solid", "solid_fill", "solid_transp", "gradient", "hatch", "bitmap"]
    styles = doc.StyleFamilies.getByName("ParagraphStyles")
    for k in kinds:
        s = doc.createInstance("com.sun.star.style.ParagraphStyle")
        s.ParentStyle = "Standard"
        styles.insertByName("S_" + k, s)
        fill(s, k)
    pb = uno.getConstantByName("com.sun.star.text.ControlCharacter.PARAGRAPH_BREAK")
    cur = text.createTextCursor()
    labels = []
    for k in kinds:
        for kind in ("direct", "styled"):
            text.insertString(cur, kind + " " + k, False)
            text.insertControlCharacter(cur, pb, False)
            labels.append((kind, k))
    paras = doc.Text.createEnumeration()
    for kind, k in labels:
        p = paras.nextElement()
        if kind == "direct":
            fill(p, k)
        else:
            p.ParaStyleName = "S_" + k
    return [_save(doc, outdir, "para-fill.odt", "writer8")]


def alien_attrs(outdir):
    """Preserved alien attributes in ODF namespaces and in a foreign namespace on the carriers that keep them: Writer paragraph,
    character, section, table and table cell, a Draw shape, a Calc cell."""
    ns = {"text": "urn:oasis:names:tc:opendocument:xmlns:text:1.0", "style": "urn:oasis:names:tc:opendocument:xmlns:style:1.0",
          "draw": "urn:oasis:names:tc:opendocument:xmlns:drawing:1.0", "table": "urn:oasis:names:tc:opendocument:xmlns:table:1.0",
          "office": "urn:oasis:names:tc:opendocument:xmlns:office:1.0", "svg": "urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0",
          "fo": "http://www.w3.org/1999/XSL/Format", "ex": "urn:example:foreign"}
    attrs = (("text", "enable-numbering"), ("style", "alien-style"), ("draw", "alien-draw"), ("table", "alien-table"),
             ("office", "alien-office"), ("svg", "alien-svg"), ("fo", "alien-fo"), ("ex", "alien-foreign"))

    def put(container):
        for p, n in attrs:
            a = uno.createUnoStruct("com.sun.star.xml.AttributeData")
            a.Namespace, a.Type, a.Value = ns[p], "CDATA", "v-" + p
            container.insertByName(p + ":" + n, a)

    done = []
    doc = _new("swriter")
    text = doc.Text
    cur = text.createTextCursor()
    text.insertString(cur, "para text", False)
    text.insertControlCharacter(cur, 0, False)
    cur.gotoStart(False)
    cur.goRight(4, True)
    cont = cur.ParaUserDefinedAttributes
    put(cont)
    cur.ParaUserDefinedAttributes = cont
    cont = cur.TextUserDefinedAttributes
    put(cont)
    cur.TextUserDefinedAttributes = cont
    cur.gotoEnd(False)
    tbl = doc.createInstance("com.sun.star.text.TextTable")
    tbl.initialize(2, 2)
    text.insertTextContent(cur, tbl, False)
    for tgt in (tbl, tbl.getCellByName("A1")):
        cont = tgt.UserDefinedAttributes
        put(cont)
        tgt.UserDefinedAttributes = cont
    sec = doc.createInstance("com.sun.star.text.TextSection")
    text.insertTextContent(text.End, sec, False)
    cont = sec.UserDefinedAttributes
    put(cont)
    sec.UserDefinedAttributes = cont
    done.append(_save(doc, outdir, "alien-writer.odt", "writer8"))

    doc = _new("sdraw")
    page = doc.DrawPages.getByIndex(0)
    s = doc.createInstance("com.sun.star.drawing.RectangleShape")
    page.add(s)
    s.Position = Point(2000, 2000)
    s.Size = Size(4000, 2000)
    s.String = "alien"
    cont = s.UserDefinedAttributes
    put(cont)
    s.UserDefinedAttributes = cont
    done.append(_save(doc, outdir, "alien-draw.odg", "draw8"))

    doc = _new("scalc")
    c = doc.Sheets.getByIndex(0).getCellByPosition(0, 0)
    c.Value = 1
    cont = c.UserDefinedAttributes
    put(cont)
    c.UserDefinedAttributes = cont
    done.append(_save(doc, outdir, "alien-calc.ods", "calc8"))
    return done


def para_fill_foreign(outdir):
    """The paragraphs and paragraph styles another producer writes with fill properties, built with AOO's own (aoo-form.odt)."""
    png = os.path.join(outdir, "tile.png")
    _png(png)
    png_url = uno.systemPathToFileUrl(png)
    doc = _new("swriter")

    def grad(c1, c2):
        g = Gradient()
        g.Style, g.StartColor, g.EndColor = LINEAR, c1, c2
        g.Angle, g.Border, g.XOffset, g.YOffset = 300, 10, 50, 50
        g.StartIntensity = g.EndIntensity = 100
        g.StepCount = 0
        return g

    def hatch():
        h = Hatch()
        h.Style, h.Color, h.Distance, h.Angle = SINGLE, 0xFFAA00, 200, 450
        return h

    def fill(o, kind):
        if kind == "solid":
            o.FillStyle = SOLID
            o.FillColor = 0x33AA55
        elif kind == "solid_transp":
            o.FillStyle = SOLID
            o.FillColor = 0x3355AA
            o.FillTransparence = 40
        elif kind == "gradient":
            o.FillStyle = GRADIENT
            o.FillGradient = grad(0xFF0000, 0x0000FF)
        elif kind == "gradient_transp":
            o.FillStyle = GRADIENT
            o.FillGradient = grad(0xFF0000, 0x0000FF)
            o.FillTransparence = 30
        elif kind == "hatch":
            o.FillStyle = HATCH
            o.FillHatch = hatch()
            o.FillColor = 0xFFFFCC
            o.FillBackground = True
        elif kind == "hatch_nobg":
            o.FillStyle = HATCH
            o.FillHatch = hatch()
            o.FillBackground = False
        elif kind.startswith("bitmap"):
            o.FillStyle = BITMAP
            o.FillBitmapURL = png_url
            o.FillBitmapMode = {"bitmap_stretch": STRETCH, "bitmap_once": NO_REPEAT}.get(kind, REPEAT)
            if kind == "bitmap_transp":
                o.FillTransparence = 50

    kinds = ["solid", "solid_transp", "gradient", "gradient_transp", "hatch", "hatch_nobg", "bitmap_tile", "bitmap_stretch", "bitmap_once",
             "bitmap_transp"]
    styles = doc.StyleFamilies.getByName("ParagraphStyles")
    for k in kinds:
        s = doc.createInstance("com.sun.star.style.ParagraphStyle")
        s.ParentStyle = "Standard"
        styles.insertByName("S_" + k, s)
        fill(s, k)
    pb = uno.getConstantByName("com.sun.star.text.ControlCharacter.PARAGRAPH_BREAK")
    cur = doc.Text.createTextCursor()
    for k in kinds:
        for kind in ("direct", "styled"):
            doc.Text.insertString(cur, kind + " " + k, False)
            doc.Text.insertControlCharacter(cur, pb, False)
    paras = doc.Text.createEnumeration()
    for k in kinds:
        fill(paras.nextElement(), k)
        paras.nextElement().ParaStyleName = "S_" + k
    return [_save(doc, outdir, "aoo-form.odt", "writer8")]


BUILDERS = {"page-fill": page_fill, "fill-others": fill_others, "t13": t13, "page-fill-1_3": page_fill_13, "row-split": row_split,
            "grid-snap": grid_snap, "ole-text": ole_text, "ole-text-edge": ole_text_edge, "chart-shapes": chart_shapes,
            "normal-values5b": normal_values_5b, "connector-viewbox": connector_viewbox, "para-fill": para_fill,
            "alien-attrs": alien_attrs, "para-fill-foreign": para_fill_foreign}


def generate(builder, outdir):
    """Runs the named builder; the documents it makes are stored in `outdir`. Returns {"files": [names]}."""
    if builder not in BUILDERS:
        raise core.Fail("bad_args", "unknown builder %r" % builder)
    os.makedirs(outdir, exist_ok=True)
    return {"files": BUILDERS[builder](os.path.abspath(outdir))}


OPS = {"generate": core.guarded(generate)}
