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


# usage: run-in-mode.sh <mode> <port> <outdir> fixtures/t14/table_cell_props.py [<file.odp>]
# no file: writes <outdir>/table-cell-props-<mode>.odp with an Impress table whose four cells carry the non-fill cell
#          properties: text alignment, text distances, word wrap and writing mode, borders, paragraph and character
#          properties, in the office's current save mode.
# file:    loads that file and prints the same properties.
import os, sys
import uno
from unohelp import FILTERS, connect, desktop, pv

port, out, tag = int(sys.argv[1]), os.path.abspath(sys.argv[2]), sys.argv[3]
os.makedirs(out, exist_ok=True)
desk = desktop(connect(port))
dst = os.path.abspath(sys.argv[4]) if len(sys.argv) > 4 else os.path.join(out, "table-cell-props-%s.odp" % tag)
phase = "reload" if len(sys.argv) > 4 else "make"
url = uno.systemPathToFileUrl

NAMES = ["TextVerticalAdjust", "TextHorizontalAdjust", "TextLeftDistance", "TextRightDistance", "TextUpperDistance",
         "TextLowerDistance", "TextWordWrap", "TextWritingMode", "ParaAdjust", "ParaTopMargin", "CharWeight", "CharHeight"]


def val(v):
    return v.value if hasattr(v, "value") and not isinstance(v, (int, float, str)) else v


def border(c):
    b = uno.createUnoStruct("com.sun.star.table.BorderLine")
    b.Color, b.OuterLineWidth, b.InnerLineWidth, b.LineDistance = c, 35, 0, 0
    return b


def show(label, cell):
    items = [label]
    for n in NAMES:
        items.append("%s=%s" % (n, val(cell.getPropertyValue(n))))
    for n in ("TopBorder", "BottomBorder", "LeftBorder", "RightBorder"):
        b = cell.getPropertyValue(n)
        items.append("%s=0x%06x/%d" % (n, b.Color, b.OuterLineWidth))
    print(*items)


def cells(shape):
    tbl = shape.Model
    return [("c%d" % i, tbl.getCellByPosition(i % 2, i // 2)) for i in range(4)]


def adj(t, n):
    return uno.Enum("com.sun.star.drawing." + t, n)


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
    cs = cells(shape)
    c0, c1, c2, c3 = [c for _, c in cs]
    c0.TextVerticalAdjust = adj("TextVerticalAdjust", "CENTER")
    c0.TextHorizontalAdjust = adj("TextHorizontalAdjust", "RIGHT")
    c1.TextLeftDistance, c1.TextRightDistance, c1.TextUpperDistance, c1.TextLowerDistance = 500, 600, 300, 400
    c2.TextWordWrap = True
    c2.TextWritingMode = uno.Enum("com.sun.star.text.WritingMode", "RL_TB")
    c3.TopBorder, c3.BottomBorder = border(0xff0000), border(0x00ff00)
    c3.LeftBorder, c3.RightBorder = border(0x0000ff), border(0x808080)
    c3.ParaAdjust = 1
    c3.ParaTopMargin = 250
    c3.CharWeight = 150.0
    c3.CharHeight = 18.0
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
