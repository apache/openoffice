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
# usage: runpy.sh fixtures/leak/make.py <port> <outdir>
# Documents for names that AOO's export code can emit but no ODF 1.2 schema pattern defines (runs/task12/gaps/strict-leak.txt).
# Saves with the office's configured ODF version. ODF packages go to <outdir>; the report-definition package and the
# old-format chart copies go to <outdir>/../other and <outdir>/../oldfmt so the ODF checkers do not see them.
import os, struct, sys, traceback, zlib, uno
from unohelp import connect, desktop, pv
from com.sun.star.awt import Rectangle, Size, Point
from com.sun.star.table import CellAddress, CellRangeAddress, BorderLine

port, out = int(sys.argv[1]), os.path.abspath(sys.argv[2])
base = os.path.dirname(out)
other, oldfmt, work = (os.path.join(base, d) for d in ("other", "oldfmt", "work"))
for d in (out, other, oldfmt, work):
    os.makedirs(d, exist_ok=True)
ctx = connect(port)
sm = ctx.ServiceManager
desk = desktop(ctx)
failed = []
URL = uno.systemPathToFileUrl
ANCHOR = uno.Enum("com.sun.star.text.TextContentAnchorType", "AS_CHARACTER")
TPL = "com.sun.star.chart2.template."


def new(factory):
    return desk.loadComponentFromURL(factory, "_blank", 0, (pv("Hidden", True),))


def save(doc, path, filt):
    doc.storeToURL(URL(path), (pv("FilterName", filt),))
    print("saved", os.path.basename(path), filt)


def step(name, fn):
    try:
        fn()
    except Exception as e:
        failed.append(name)
        print("FAIL", name, repr(e)[:300])
        traceback.print_exc(limit=3)


def make_png(path):
    w = h = 8
    raw = b"".join(b"\x00" + bytes([200, 30, 30] * w) for _ in range(h))

    def chunk(t, b):
        return struct.pack(">I", len(b)) + t + b + struct.pack(">I", zlib.crc32(t + b) & 0xFFFFFFFF)

    with open(path, "wb") as f:
        f.write(b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", w, h, 8, 2, 0, 0, 0))
                + chunk(b"IDAT", zlib.compress(raw)) + chunk(b"IEND", b""))


def set_all(obj, props):
    for k, v in props:
        try:
            uno.invoke(obj, "setPropertyValue", (k, v)) if isinstance(v, uno.Any) else obj.setPropertyValue(k, v)
        except Exception as e:
            print("note: cannot set", k, repr(e)[:80])


def writer_fields():
    doc = new("private:factory/swriter")
    try:
        text = doc.Text
        cur = text.createTextCursor()
        text.insertString(cur, "fields: ", False)

        def field(svc, **kw):
            f = doc.createInstance("com.sun.star.text.TextField." + svc)
            for k, v in kw.items():
                setattr(f, k, v)
            text.insertTextContent(cur, f, False)
            text.insertString(cur, " ", False)

        field("Input", Content="x", Hint="hint", Help="help text", Tooltip="tooltip text")
        field("DropDown", Name="dd", Items=("a", "b", "c"), SelectedItem="b", Help="help dd", Tooltip="tip dd")
        pn = uno.Enum("com.sun.star.text.PageNumberType", "NEXT")
        field("PageNumber", SubType=pn, UserText="next page", NumberingType=6)
        pn = uno.Enum("com.sun.star.text.PageNumberType", "PREV")
        field("PageNumber", SubType=pn, UserText="prev page", NumberingType=6)
        fn = doc.createInstance("com.sun.star.text.Footnote")
        text.insertTextContent(cur, fn, False)
        fn.String = "note"
        fs = doc.FootnoteSettings
        fs.EndNotice = "continued on next page"
        fs.BeginNotice = "continued from previous page"
        save(doc, os.path.join(out, "writer-fields.odt"), "writer8")
    finally:
        doc.close(True)


def writer_toc_image():
    png = os.path.join(work, "red.png")
    make_png(png)
    doc = new("private:factory/swriter")
    try:
        text = doc.Text
        cur = text.createTextCursor()
        text.insertString(cur, "Heading one", False)
        cur.gotoStart(False)
        cur.gotoEndOfParagraph(True)
        cur.ParaStyleName = "Heading 1"
        cur.gotoEnd(False)
        text.insertControlCharacter(cur, uno.getConstantByName("com.sun.star.text.ControlCharacter.PARAGRAPH_BREAK"), False)
        g = doc.createInstance("com.sun.star.text.GraphicObject")
        g.GraphicURL = URL(png)
        g.Width = g.Height = 3000
        g.AnchorType = ANCHOR
        text.insertTextContent(cur, g, False)
        g.GraphicRotation = 900
        toc = doc.createInstance("com.sun.star.text.ContentIndex")
        toc.CreateFromOutline = True
        lf = toc.LevelFormat
        new_level = []
        for tok in lf.getByIndex(1):
            tok = list(tok)
            for p in tok:
                if p.Name == "WithTab":
                    p.Value = False
            new_level.append(tuple(tok))
        uno.invoke(lf, "replaceByIndex", (1, uno.Any("[][]com.sun.star.beans.PropertyValue", tuple(new_level))))
        text.insertTextContent(cur, toc, False)
        toc.update()
        save(doc, os.path.join(out, "writer-toc-image.odt"), "writer8")
    finally:
        doc.close(True)


def writer_forms():
    doc = new("private:factory/swriter")
    try:
        dp = doc.DrawPage
        dp.Forms.insertByName("F1", doc.createInstance("com.sun.star.form.component.Form"))
        form = dp.Forms.getByName("F1")
        y = 300
        for kind in ("TextField", "FormattedField", "NumericField", "DateField", "TimeField", "CurrencyField", "PatternField",
                     "CheckBox", "ComboBox", "ListBox", "RadioButton"):
            m = doc.createInstance("com.sun.star.form.component." + kind)
            set_all(m, (("InputRequired", False), ("EnableVisible", False)))
            m.Name = kind
            form.insertByName(kind, m)
            s = doc.createInstance("com.sun.star.drawing.ControlShape")
            s.Size = Size(3000, 600)
            s.Position = Point(500, y)
            y += 800
            s.setControl(m)
            dp.add(s)
        for i, (n, v) in enumerate((("FontWidth", uno.Any("short", 20)), ("FontCharWidth", uno.Any("float", 90.0)),
                                    ("FontWordLineMode", True))):
            m = doc.createInstance("com.sun.star.form.component.TextField")
            set_all(m, ((n, v),))
            m.Name = "font_" + n
            form.insertByName(m.Name, m)
            s = doc.createInstance("com.sun.star.drawing.ControlShape")
            s.Size = Size(3000, 600)
            s.Position = Point(4500, 300 + i * 800)
            s.setControl(m)
            dp.add(s)
        save(doc, os.path.join(out, "writer-forms.odt"), "writer8")
    finally:
        doc.close(True)


def writer_redline():
    doc = new("private:factory/swriter")
    try:
        doc.Text.insertString(doc.Text.createTextCursor(), "first para\nsecond para\nthird para", False)
        t = doc.createInstance("com.sun.star.text.TextTable")
        t.initialize(2, 2)
        c = doc.Text.createTextCursor()
        c.gotoEnd(False)
        doc.Text.insertTextContent(c, t, False)
        doc.RecordChanges = True
        c = doc.Text.createTextCursor()
        c.gotoStart(False)
        c.goRight(3, False)
        c.gotoEndOfParagraph(True)
        c.goRight(1, True)
        c.setString("")
        c = doc.Text.createTextCursor()
        c.gotoEnd(False)
        c.goLeft(4, True)
        c.setString("")
        e = doc.Redlines.createEnumeration()
        while e.hasMoreElements():
            r = e.nextElement()
            print("redline", r.RedlineType, "MergeLastPara", r.getPropertyValue("MergeLastPara"))
        save(doc, os.path.join(out, "writer-redline.odt"), "writer8")
    finally:
        doc.close(True)


def writer_pagegrid():
    doc = new("private:factory/swriter")
    try:
        doc.Text.insertString(doc.Text.createTextCursor(), "grid", False)
        ps = doc.StyleFamilies.getByName("PageStyles").getByName("Standard")
        set_all(ps, (("GridMode", 2), ("GridLines", 20), ("GridBaseHeight", 600), ("GridBaseWidth", 500),
                     ("GridSnapToChars", True), ("StandardPageMode", True), ("GridDisplay", True), ("GridPrint", True)))
        save(doc, os.path.join(out, "writer-pagegrid.odt"), "writer8")
    finally:
        doc.close(True)


def fill_rows(sh, rows):
    for r, row in enumerate(rows):
        for c, v in enumerate(row):
            cell = sh.getCellByPosition(c, r)
            if isinstance(v, str):
                cell.String = v
            else:
                cell.Value = v


def calc_misc():
    doc = new("private:factory/scalc")
    try:
        sh = doc.Sheets.getByIndex(0)
        sh.TabColor = 0xFF8000
        for i, mode in enumerate((0, 1, 2)):
            sh.getCellByPosition(12, i).Formula = '=DDE("soffice";"/tmp/x.ods";"Sheet1.A%d";%d)' % (i + 1, mode)
        fill_rows(sh, [("", "", "", "L0", 1, ""), ("", "", "", "L1", 2, ""), ("", "", "", "L2", 3, "")])
        for r in range(3):
            sh.getCellByPosition(1, r + 4).Value = r + 5
            sh.getCellByPosition(0, r + 4).String = "L%d" % r
        cd = doc.createConsolidationDescriptor(False)
        cd.Function = uno.Enum("com.sun.star.sheet.GeneralFunction", "SUM")
        cd.Sources = (CellRangeAddress(0, 0, 4, 1, 6), CellRangeAddress(0, 3, 0, 4, 2))
        cd.StartOutputPosition = CellAddress(0, 7, 7)
        cd.UseColumnHeaders = True
        cd.UseRowHeaders = True
        cd.InsertLinks = False
        doc.consolidate(cd)
        c = sh.getCellByPosition(10, 10)
        c.String = "diagonal"
        bl = BorderLine()
        bl.OuterLineWidth = 50
        bl.Color = 0xFF0000
        c.DiagonalBLTR = bl
        c.DiagonalTLBR = bl
        ps = doc.StyleFamilies.getByName("PageStyles").getByName(sh.PageStyle)
        set_all(ps, (("ScaleToPagesX", 2), ("ScaleToPagesY", 3)))
        save(doc, os.path.join(out, "calc-misc.ods"), "calc8")
    finally:
        doc.close(True)


def calc_pivot_group():
    doc = new("private:factory/scalc")
    try:
        sh = doc.Sheets.getByIndex(0)
        fill_rows(sh, [("Region", "Qty")] + [(r, i + 1) for i, r in enumerate(["North", "South", "East", "West", "North", "East"])])
        dps = sh.DataPilotTables
        desc = dps.createDataPilotDescriptor()
        desc.setSourceRange(CellRangeAddress(0, 0, 0, 1, 6))
        fl = desc.getDataPilotFields()
        ori = lambda n: uno.Enum("com.sun.star.sheet.DataPilotFieldOrientation", n)
        fl.getByName("Region").Orientation = ori("ROW")
        fl.getByName("Qty").Orientation = ori("DATA")
        dps.insertNewByName("pv", CellAddress(0, 4, 1), desc)
        f = dps.getByName("pv").getDataPilotFields().getByName("Region")
        f.createNameGroup(("North", "South"))
        save(doc, os.path.join(out, "calc-pivot-group.ods"), "calc8")
    finally:
        doc.close(True)


def calc_tracked():
    doc = new("private:factory/scalc")
    try:
        dh = sm.createInstanceWithContext("com.sun.star.frame.DispatchHelper", ctx)
        dh.executeDispatch(doc.CurrentController.Frame, ".uno:TraceChangeMode", "", 0, (pv("TraceChangeMode", True),))
        sh = doc.Sheets.getByIndex(0)
        for r in range(6):
            sh.getCellByPosition(0, r).Value = r
        for r in range(6):
            sh.getCellByPosition(0, r).Value = r * 10
        sh.Rows.insertByIndex(2, 2)
        sh.Rows.removeByIndex(3, 2)
        save(doc, os.path.join(out, "calc-tracked.ods"), "calc8")
    finally:
        doc.close(True)


def chart_doc(name, charts, post=None):
    doc = new("private:factory/scalc")
    try:
        sh = doc.Sheets.getByIndex(0)
        loc = uno.createUnoStruct("com.sun.star.lang.Locale", "en", "US", "")
        key = doc.NumberFormats.queryKey("YYYY-MM-DD", loc, False)
        if key == -1:
            key = doc.NumberFormats.addNew("YYYY-MM-DD", loc)
        for r in range(6):
            for c in range(5):
                if r == 0:
                    sh.getCellByPosition(c, r).String = "s%d" % c if c else "cat"
                else:
                    sh.getCellByPosition(c, r).Value = 40000 + r * 30 if c == 0 else r * 2 + c
        sh.getCellRangeByName("A2:A6").NumberFormat = key
        for i, (cname, ncols, tpl) in enumerate(charts):
            rng = CellRangeAddress(0, 0, 0, ncols, 5)
            sh.Charts.addNewByName(cname, Rectangle(500 + i * 9500, 8000, 9000, 6800), (rng,), True, True)
            ch = sh.Charts.getByName(cname).EmbeddedObject
            if tpl:
                dp = ch.getDataProvider()
                args = (pv("CellRangeRepresentation", "$Sheet1.$A$1:$%s$6" % "ABCDE"[ncols]),
                        pv("DataRowSource", uno.Enum("com.sun.star.chart.ChartDataRowSource", "COLUMNS")),
                        pv("FirstCellAsLabel", True), pv("HasCategories", True))
                t = ch.ChartTypeManager.createInstance(TPL + tpl)
                ch.setFirstDiagram(t.createDiagramByDataSource(dp.createDataSource(args), args))
            if post:
                post(ch)
        save(doc, os.path.join(out, name + ".ods"), "calc8")
        try:
            save(doc, os.path.join(oldfmt, name + ".sxc"), "StarOffice XML (Calc)")
        except Exception as e:
            print("note: old format save failed", repr(e)[:100])
    finally:
        doc.close(True)


def label_and_date_axis(ch):
    cs = ch.getFirstDiagram().getCoordinateSystems()[0]
    for s in cs.getChartTypes()[0].getDataSeries():
        s.setPropertyValue("LabelSeparator", "a\tb\nc")
        s.setPropertyValue("Label", uno.createUnoStruct("com.sun.star.chart2.DataPointLabel", True, False, False, False))
    ax = cs.getAxisByDimension(0, 0)
    sd = ax.getScaleData()
    sd.AxisType = 4
    ti = uno.createUnoStruct("com.sun.star.chart.TimeIncrement")
    ti.TimeResolution = 1
    ti.MajorTimeInterval = uno.createUnoStruct("com.sun.star.chart.TimeInterval", 2, 1)
    ti.MinorTimeInterval = uno.createUnoStruct("com.sun.star.chart.TimeInterval", 1, 0)
    sd.TimeIncrement = ti
    ax.setScaleData(sd)


def draw_fontwork():
    doc = new("private:factory/sdraw")
    try:
        page = doc.DrawPages.getByIndex(0)
        s = doc.createInstance("com.sun.star.drawing.RectangleShape")
        page.add(s)
        s.Size = Size(5000, 3000)
        s.Position = Point(500, 500)
        s.String = "fontwork"
        set_all(s, (("FontWorkStyle", 0), ("FontWorkAdjust", 1), ("FontWorkDistance", 100), ("FontWorkStart", 50),
                    ("FontWorkMirror", True), ("FontWorkOutline", True), ("FontWorkShadow", 1), ("FontWorkShadowColor", 0xFF0000),
                    ("FontWorkShadowOffsetX", 100), ("FontWorkShadowOffsetY", 100), ("FontWorkHideForm", True),
                    ("FontWorkShadowTransparence", 50)))
        for svc in ("RectangleShape", "TextShape", "PolyPolygonShape", "CustomShape"):
            o = doc.createInstance("com.sun.star.drawing." + svc)
            print("FontWorkForm on", svc, o.getPropertySetInfo().hasPropertyByName("FontWorkForm"))
        a = doc.createInstance("com.sun.star.drawing.AppletShape")
        page.add(a)
        a.Size = Size(3000, 2000)
        a.Position = Point(6000, 500)
        set_all(a, (("AppletName", "myapplet"), ("AppletCode", "Foo.class"), ("AppletCodeBase", "file:///tmp/"), ("AppletIsScript", True)))
        print("applet AppletName reads back", repr(a.getPropertyValue("AppletName")))
        t = doc.createInstance("com.sun.star.drawing.TextShape")
        page.add(t)
        t.Size = Size(5000, 3000)
        t.Position = Point(500, 4500)
        t.String = "one\ntwo"
        nr = t.getPropertyValue("NumberingRules")
        lvl = list(nr.getByIndex(0))
        for p in lvl:
            if p.Name == "NumberingType":
                p.Value = 4
        uno.invoke(nr, "replaceByIndex", (0, uno.Any("[]com.sun.star.beans.PropertyValue", tuple(lvl))))
        t.setPropertyValue("NumberingRules", nr)
        save(doc, os.path.join(out, "draw-fontwork.odg"), "draw8")
    finally:
        doc.close(True)


def set_info(ds, **kw):
    bag = ds.Settings
    for k, v in kw.items():
        if bag.getPropertySetInfo().hasPropertyByName(k):
            uno.invoke(bag, "setPropertyValue", (k, v))
        else:
            uno.invoke(bag, "addProperty", (k, 128 | 1, v))


def base_doc():
    dbc = sm.createInstanceWithContext("com.sun.star.sdb.DatabaseContext", ctx)
    path = os.path.join(out, "leak-base.odb")
    for p in (path, path + ".lck"):
        if os.path.exists(p):
            os.remove(p)
    ds = dbc.createInstance()
    ds.URL = "sdbc:embedded:hsqldb"
    ds.DatabaseDocument.storeAsURL(URL(path), ())
    try:
        q = ds.QueryDefinitions.createInstance()
        q.Command = 'SELECT "ID", "NAME" FROM "CUSTOMERS"'
        q.Filter = '"ID" > 0'
        q.ApplyFilter = True
        q.Order = '"NAME"'
        ds.QueryDefinitions.insertByName("Q", q)
        q = ds.QueryDefinitions.getByName("Q")
        print("ApplyOrder on query", q.getPropertySetInfo().hasPropertyByName("ApplyOrder"),
              "on table definition", ds.Tables.createInstance().getPropertySetInfo().hasPropertyByName("ApplyOrder"))
        set_all(q, (("FontWidth", uno.Any("short", 20)), ("FontCharWidth", uno.Any("float", 90.0))))
        for name, props in (("ID", dict(Hidden=True, HelpText="id column")), ("NAME", dict(HelpText="name", ControlDefault="x"))):
            d = q.Columns.createDataDescriptor()
            d.Name = name
            for k, v in props.items():
                d.setPropertyValue(k, v)
            q.Columns.appendByDescriptor(d)
        ds.DatabaseDocument.store()
    finally:
        ds.DatabaseDocument.close(True)
        if os.path.exists(path + ".lck"):
            os.remove(path + ".lck")
    print("saved", os.path.basename(path))


def base_dbase():
    dbc = sm.createInstanceWithContext("com.sun.star.sdb.DatabaseContext", ctx)
    d = os.path.join(work, "dbase")
    os.makedirs(d, exist_ok=True)
    path = os.path.join(out, "leak-base-dbase.odb")
    for p in (path, path + ".lck"):
        if os.path.exists(p):
            os.remove(p)
    ds = dbc.createInstance()
    ds.URL = "sdbc:dbase:" + URL(d) + "/"
    set_info(ds, Extension="dbf", CharSet="utf-8")
    ds.DatabaseDocument.storeAsURL(URL(path), ())
    try:
        ds.DatabaseDocument.store()
    finally:
        ds.DatabaseDocument.close(True)
        if os.path.exists(path + ".lck"):
            os.remove(path + ".lck")
    print("saved", os.path.basename(path))


def report_definition():
    r = sm.createInstanceWithContext("com.sun.star.report.ReportDefinition", ctx)
    r.Caption = "Leak report"
    r.Name = "rpt"
    r.ReportHeaderOn = True
    path = os.path.join(other, "report-definition.odr")
    stg = sm.createInstanceWithContext("com.sun.star.embed.StorageFactory", ctx).createInstanceWithArguments((URL(path), 7))
    r.storeToStorage(stg, ())
    stg.commit()
    stg.dispose()
    print("saved", os.path.basename(path), "(report definition)")


for name, fn in (("writer-fields", writer_fields), ("writer-toc-image", writer_toc_image), ("writer-forms", writer_forms),
                 ("writer-redline", writer_redline), ("writer-pagegrid", writer_pagegrid), ("calc-misc", calc_misc),
                 ("calc-pivot-group", calc_pivot_group), ("calc-tracked", calc_tracked),
                 ("calc-chart-axis", lambda: chart_doc("calc-chart-axis", (("c1", 3, None),), label_and_date_axis)),
                 ("calc-chart-types", lambda: chart_doc("calc-chart-types", (("cl", 3, "ColumnWithLine"), ("sv", 4, "StockVolumeLowHighClose")))),
                 ("draw-fontwork", draw_fontwork), ("base", base_doc), ("base-dbase", base_dbase), ("report", report_definition)):
    step(name, fn)
print("FAILED:", failed if failed else "none")
