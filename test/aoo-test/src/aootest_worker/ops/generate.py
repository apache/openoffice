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


BUILDERS = {"page-fill": page_fill, "fill-others": fill_others, "t13": t13, "page-fill-1_3": page_fill_13}


def generate(builder, outdir):
    """Runs the named builder; the documents it makes are stored in `outdir`. Returns {"files": [names]}."""
    if builder not in BUILDERS:
        raise core.Fail("bad_args", "unknown builder %r" % builder)
    os.makedirs(outdir, exist_ok=True)
    return {"files": BUILDERS[builder](os.path.abspath(outdir))}


OPS = {"generate": core.guarded(generate)}
