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
#
# usage: runpy.sh fixtures/t13/make.py <port> <outdir>
# Documents for the features ODF 1.3 changes in the writer: drop-down fields, annotation initials, Calc tab colour,
# Calc page scale-to, chart inner plot position and legend size. Saves with the office's configured ODF version: run
# it with fixtures/gen-run.sh (strict, ext, v13, x13). The committed copies in this folder were generated in ext mode,
# which keeps all of the features. Everything is fixed (no dates taken from the clock, nothing random).
import sys, os, traceback, uno
from unohelp import connect, desktop, pv

port, out = int(sys.argv[1]), os.path.abspath(sys.argv[2])
os.makedirs(out, exist_ok=True)
desk = desktop(connect(port))
failed = []


def new(factory):
    return desk.loadComponentFromURL(factory, "_blank", 0, (pv("Hidden", True),))


def save(doc, name, filt):
    try:
        doc.storeToURL(uno.systemPathToFileUrl(os.path.join(out, name)), (pv("FilterName", filt),))
        print("saved", name, filt)
    except Exception as e:
        failed.append(name)
        print("FAIL", name, filt, type(e).__name__, str(e)[:200], repr(e)[:200])


def attempt(label, fn):
    try:
        fn()
    except Exception as e:
        failed.append(label)
        print("FAIL", label, type(e).__name__, str(e)[:100], repr(e)[:200], "line", traceback.extract_tb(e.__traceback__)[-1].lineno)


def dt(y, mo, d, h, mi):
    return uno.createUnoStruct("com.sun.star.util.DateTime", 0, 0, mi, h, d, mo, y)


def ensure_sheets(doc, n):
    sheets = doc.Sheets
    while sheets.Count > n:
        sheets.removeByName(sheets.getByIndex(sheets.Count - 1).Name)
    while sheets.Count < n:
        sheets.insertNewByName("Sheet%d" % (sheets.Count + 1), sheets.Count)
    return sheets


def put_text(doc, cur, s):
    doc.Text.insertString(cur, s, False)


def put_field(doc, cur, kind, **props):
    f = doc.createInstance("com.sun.star.text.TextField." + kind)
    for k, v in props.items():
        setattr(f, k, v)
    doc.Text.insertTextContent(cur, f, False)
    return f


def doc_dropdown():
    doc = new("private:factory/swriter")
    cur = doc.Text.createTextCursor()
    put_text(doc, cur, "First: ")
    put_field(doc, cur, "DropDown", Name="dd1", Items=("one", "two", "three"), SelectedItem="two", Help="help one", Tooltip="tip one")
    doc.Text.insertControlCharacter(cur, 0, False)
    put_text(doc, cur, "Second: ")
    # SwDropDownField accepts only a listed SelectedItem: "zzz" reads back as "" (nothing selected, the field shows "a")
    put_field(doc, cur, "DropDown", Name="dd2", Items=("a", "b"), SelectedItem="zzz", Help="help two", Tooltip="tip two")
    save(doc, "dropdown.odt", "writer8")
    doc.close(True)


def doc_annot():
    doc = new("private:factory/swriter")
    cur = doc.Text.createTextCursor()
    put_text(doc, cur, "with initials ")
    put_field(doc, cur, "Annotation", Author="Ann Author", Initials="AB", Content="first note", DateTimeValue=dt(2024, 7, 8, 9, 10))
    doc.Text.insertControlCharacter(cur, 0, False)
    put_text(doc, cur, "without initials ")
    put_field(doc, cur, "Annotation", Author="Bob Author", Content="second note", DateTimeValue=dt(2024, 7, 9, 9, 10))
    save(doc, "annot.odt", "writer8")
    doc.close(True)


def doc_tabcolor():
    doc = new("private:factory/scalc")
    sheets = ensure_sheets(doc, 3)
    for i, color in ((0, 0xFF0000), (2, 0x3366CC)):
        sheets.getByIndex(i).TabColor = color
    for i in range(3):
        sheets.getByIndex(i).getCellByPosition(0, 0).setString("sheet %d" % i)
    save(doc, "tabcolor.ods", "calc8")
    doc.close(True)


def doc_scaleto():
    doc = new("private:factory/scalc")
    ps = doc.StyleFamilies.getByName("PageStyles")
    sheets = ensure_sheets(doc, 4)
    # setting ScaleToPagesX/Y clears ScaleToPages and PageScale in the style (sc/source/ui/unoobj/styleuno.cxx)
    for name, props in (("PS_xy", {"ScaleToPagesX": 2, "ScaleToPagesY": 3}),
                        ("PS_x", {"ScaleToPagesX": 2}),
                        ("PS_pages", {"ScaleToPages": 4}),
                        ("PS_scale", {"PageScale": 80})):
        ps.insertByName(name, doc.createInstance("com.sun.star.style.PageStyle"))
        st = ps.getByName(name)
        for k, v in props.items():
            setattr(st, k, v)
    for i, name in enumerate(("PS_xy", "PS_x", "PS_pages", "PS_scale")):
        sh = sheets.getByIndex(i)
        sh.PageStyle = name
        sh.getCellByPosition(0, 0).setString(name)
    save(doc, "scaleto.ods", "calc8")
    doc.close(True)


def doc_chart():
    doc = new("private:factory/scalc")
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
    r = dg.calculateDiagramPositionExcludingAxes()
    print("diagram excl. axes", r.X, r.Y, r.Width, r.Height, "auto", dg.isAutomaticDiagramPositioning(), "excl", dg.isExcludingDiagramPositioning())
    print("legend", lg.Expansion.value, lg.getPosition().X, lg.getPosition().Y, lg.getSize().Width, lg.getSize().Height)
    save(doc, "chart.ods", "calc8")
    doc.close(True)


for n, fn in (("dropdown", doc_dropdown), ("annot", doc_annot), ("tabcolor", doc_tabcolor), ("scaleto", doc_scaleto), ("chart", doc_chart)):
    attempt(n, fn)
sys.exit(1 if failed else 0)
