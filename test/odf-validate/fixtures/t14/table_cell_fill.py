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

# usage: run-in-mode.sh <mode> <port> <outdir> fixtures/t14/table_cell_fill.py [<file.odp>]
# no file: writes <outdir>/table-cell-fill-<mode>.odp with an Impress table whose cells carry a solid fill with
#          transparency, a gradient, a hatch and no fill, in the office's current save mode.
# file:    loads that file and prints the fill of those cells.
import os, sys
import uno
from unohelp import FILTERS, connect, desktop, pv

port, out, tag = int(sys.argv[1]), os.path.abspath(sys.argv[2]), sys.argv[3]
os.makedirs(out, exist_ok=True)
desk = desktop(connect(port))
dst = os.path.abspath(sys.argv[4]) if len(sys.argv) > 4 else os.path.join(out, "table-cell-fill-%s.odp" % tag)
phase = "reload" if len(sys.argv) > 4 else "make"
url = uno.systemPathToFileUrl


def fe(name):
    return uno.Enum("com.sun.star.drawing.FillStyle", name)


def show(label, cell):
    kind = cell.FillStyle.value
    line = [label, kind]
    if kind == "SOLID":
        line += ["0x%06x" % cell.FillColor, cell.FillTransparence]
    elif kind == "GRADIENT":
        g = cell.FillGradient
        line += [g.Style.value, hex(g.StartColor), hex(g.EndColor), g.Angle]
    elif kind == "HATCH":
        h = cell.FillHatch
        line += [h.Style.value, hex(h.Color), h.Distance, h.Angle]
    print(*line)


def cells(shape):
    tbl = shape.Model
    return [("c%d" % i, tbl.getCellByPosition(i % 2, i // 2)) for i in range(4)]


if phase == "make":
    doc = desk.loadComponentFromURL("private:factory/simpress", "_blank", 0, (pv("Hidden", True),))
    page = doc.DrawPages.getByIndex(0)
    shape = doc.createInstance("com.sun.star.drawing.TableShape")
    page.add(shape)
    shape.Position = uno.createUnoStruct("com.sun.star.awt.Point", 2000, 2000)
    shape.Size = uno.createUnoStruct("com.sun.star.awt.Size", 12000, 6000)
    tbl = shape.Model
    tbl.Columns.insertByIndex(1, 1)
    tbl.Rows.insertByIndex(1, 1)
    print("table", tbl.Columns.Count, "x", tbl.Rows.Count)
    cs = cells(shape)
    c0, c1, c2, c3 = [c for _, c in cs]
    c0.FillStyle = fe("SOLID"); c0.FillColor = 0x3355aa; c0.FillTransparence = 40
    g = uno.createUnoStruct("com.sun.star.awt.Gradient")
    g.Style = uno.Enum("com.sun.star.awt.GradientStyle", "LINEAR")
    g.StartColor, g.EndColor, g.Angle = 0xff0000, 0x0000ff, 300
    g.Border, g.XOffset, g.YOffset, g.StartIntensity, g.EndIntensity, g.StepCount = 5, 50, 50, 100, 100, 0
    c1.FillStyle = fe("GRADIENT"); c1.FillGradient = g
    h = uno.createUnoStruct("com.sun.star.drawing.Hatch")
    h.Style = uno.Enum("com.sun.star.drawing.HatchStyle", "SINGLE")
    h.Color, h.Distance, h.Angle = 0x00aa00, 200, 450
    c2.FillStyle = fe("HATCH"); c2.FillHatch = h
    c3.FillStyle = fe("NONE")
    for n, c in cs:
        show(n, c)
    if os.path.exists(dst):
        os.remove(dst)
    doc.storeToURL(url(dst), (pv("FilterName", FILTERS["odp"]), pv("Overwrite", True)))
    doc.close(True)
    print("stored", dst)
else:
    doc = desk.loadComponentFromURL(url(dst), "_blank", 0, (pv("Hidden", True),))
    page = doc.DrawPages.getByIndex(0)
    shape = [page.getByIndex(i) for i in range(page.Count) if page.getByIndex(i).ShapeType.endswith("TableShape")][0]
    for n, c in cells(shape):
        show(n, c)
    doc.close(True)
