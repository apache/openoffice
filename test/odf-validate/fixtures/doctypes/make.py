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
# usage: runpy.sh fixtures/doctypes/make.py <port> <outdir>
# One document of each ODF type the corpus lacks (chart-host.ods feeds derive-chart.py). Saves with the office's configured ODF version: run it against the strict and extended profiles.
import sys, os, uno
from unohelp import connect, desktop, pv

port, out = int(sys.argv[1]), sys.argv[2]
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
        print("FAIL", name, filt, repr(e)[:200])


def writer_body(doc, text):
    c = doc.Text.createTextCursor()
    doc.Text.insertString(c, text, False)


def calc_body(doc):
    sh = doc.Sheets.getByIndex(0)
    for i in range(4):
        sh.getCellByPosition(0, i).Value = i + 1
        sh.getCellByPosition(1, i).Formula = "=A%d*2" % (i + 1)
    return sh


def draw_body(doc):
    page = doc.DrawPages.getByIndex(0)
    shape = doc.createInstance("com.sun.star.drawing.RectangleShape")
    page.add(shape)
    shape.Size = uno.createUnoStruct("com.sun.star.awt.Size", 4000, 2000)
    shape.String = "box"


doc = new("private:factory/swriter")
writer_body(doc, "writer template")
save(doc, "template-writer.ott", "writer8_template")
doc.close(True)

doc = new("private:factory/scalc")
calc_body(doc)
save(doc, "template-calc.ots", "calc8_template")
doc.close(True)

doc = new("private:factory/simpress")
draw_body(doc)
save(doc, "template-impress.otp", "impress8_template")
save(doc, "impress-as-draw.odg", "impress8_draw")
doc.close(True)

doc = new("private:factory/sdraw")
draw_body(doc)
save(doc, "template-draw.otg", "draw8_template")
doc.close(True)

doc = new("private:factory/swriter/GlobalDocument")
writer_body(doc, "master document")
save(doc, "master.odm", "writerglobal8")
save(doc, "master-as-text.odt", "writerglobal8_writer")
doc.close(True)

doc = new("private:factory/swriter/web")
writer_body(doc, "web document")
save(doc, "web.odt", "writerweb8_writer")
save(doc, "web-template.ott", "writerweb8_writer_template")
doc.close(True)

doc = new("private:factory/smath")
doc.Formula = "a over b + sqrt {x ^ 2}"
save(doc, "math.odf", "math8")
doc.close(True)

doc = new("private:factory/scalc")
sh = calc_body(doc)
rng = uno.createUnoStruct("com.sun.star.table.CellRangeAddress", 0, 0, 0, 1, 3)
sh.Charts.addNewByName("c1", uno.createUnoStruct("com.sun.star.awt.Rectangle", 1000, 1000, 8000, 6000), (rng,), False, False)
# storeToURL/storeAsURL of a chart model crashes the office, so chart.odc is cut out of this package by derive-chart.py
save(doc, "chart-host.ods", "calc8")
doc.close(True)

sys.exit(1 if failed else 0)
