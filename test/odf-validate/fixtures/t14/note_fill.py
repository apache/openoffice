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

# usage: runpy.sh fixtures/t14/note_fill.py <port> make|resave|edit <outdir>
# make:   writes <outdir>/note-src.ods, a sheet with a cell note, in the office's current save mode.
# resave: loads note-src.ods and stores it again as note-resaved-<tag>.ods WITHOUT changing the sheet (Calc then keeps the stream
#         of the loaded sheet and only registers the note styles from the source, see ScXMLExport::_ExportAutoStyles_).
# edit:   like resave but changes a cell first, as the control (the sheet is exported from the model).
import os, sys
import uno
from unohelp import FILTERS, connect, desktop, pv

port, phase, out = int(sys.argv[1]), sys.argv[2], os.path.abspath(sys.argv[3])
tag = sys.argv[4] if len(sys.argv) > 4 else "x"
os.makedirs(out, exist_ok=True)
desk = desktop(connect(port))
src = os.path.join(out, "note-src.ods")
url = uno.systemPathToFileUrl
if phase == "make":
    doc = desk.loadComponentFromURL("private:factory/scalc", "_blank", 0, (pv("Hidden", True),))
    sh = doc.Sheets.getByIndex(0)
    sh.getCellByPosition(0, 0).setString("with note")
    sh.getCellByPosition(1, 2).setString("second note")
    for col, row, text in ((0, 0, "First note"), (1, 2, "Second note")):
        addr = sh.getCellByPosition(col, row).CellAddress
        sh.Annotations.insertNew(addr, text)
    for i in range(sh.Annotations.Count):
        an = sh.Annotations.getByIndex(i)
        print("note", i, "visible", an.IsVisible, "caption fill", an.AnnotationShape.FillStyle.value, an.AnnotationShape.FillColor)
    if os.path.exists(src):
        os.remove(src)
    doc.storeToURL(url(src), (pv("FilterName", FILTERS["ods"]), pv("Overwrite", True)))
    doc.close(True)
else:
    doc = desk.loadComponentFromURL(url(src), "_blank", 0, (pv("Hidden", True),))
    if phase == "edit":
        doc.Sheets.getByIndex(0).getCellByPosition(3, 5).setString("changed")
    dst = os.path.join(out, "note-%s-%s.ods" % (phase, tag))
    if os.path.exists(dst):
        os.remove(dst)
    doc.storeToURL(url(dst), (pv("FilterName", FILTERS["ods"]), pv("Overwrite", True)))
    doc.close(True)
    print("stored", dst)
