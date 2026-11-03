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

# usage: runpy.sh fixtures/t14/chart.py <port> <outdir> <tag>
# A Calc document with a chart over A1:C4 (data in the sheet, so the chart uses the sheet's data provider), saved with the
# office's configured ODF version and reloaded. Writes <outdir>/<tag>.json: whether the reloaded chart has an internal data
# provider, the source range of every data sequence, and whether the chart still follows a changed cell.
import json, os, sys
import uno
from unohelp import connect, desktop, pv

port, out, tag = int(sys.argv[1]), os.path.abspath(sys.argv[2]), sys.argv[3]
os.makedirs(out, exist_ok=True)
ctx = connect(port)
desk = desktop(ctx)


def info(doc):
    sh = doc.Sheets.getByIndex(0)
    cd = sh.Charts.getByName("chart1").EmbeddedObject
    seqs = []
    cs = cd.FirstDiagram.CoordinateSystems[0]
    for ct in cs.ChartTypes:
        for ser in ct.DataSeries:
            for seq in ser.DataSequences:
                seqs.append((seq.Role if hasattr(seq, "Role") else seq.Values.Role, seq.Values.SourceRangeRepresentation))
    first = cd.DataProvider if hasattr(cd, "DataProvider") else cd.getDataProvider()
    before = [seq.Values.getNumericalData() for ct in cs.ChartTypes for ser in ct.DataSeries for seq in ser.DataSequences]
    sh.getCellByPosition(1, 1).Value = 12345
    after = [seq.Values.getNumericalData() for ct in cs.ChartTypes for ser in ct.DataSeries for seq in ser.DataSequences]
    return {"internal_data_provider": bool(cd.hasInternalDataProvider()), "ranges": seqs,
            "follows_cell": before != after, "label_sequences": [s.Label.SourceRangeRepresentation for ct in cs.ChartTypes for ser in ct.DataSeries for s in [ser.DataSequences[0]]] if False else None}


doc = desk.loadComponentFromURL("private:factory/scalc", "_blank", 0, (pv("Hidden", True),))
sh = doc.Sheets.getByIndex(0)
for r, row in enumerate((("", "a", "b"), ("x", 1, 2), ("y", 3, 4), ("z", 5, 6))):
    for c, v in enumerate(row):
        cell = sh.getCellByPosition(c, r)
        if isinstance(v, str):
            cell.String = v
        else:
            cell.Value = v
addr = uno.createUnoStruct("com.sun.star.table.CellRangeAddress")
addr.Sheet, addr.StartColumn, addr.StartRow, addr.EndColumn, addr.EndRow = 0, 0, 0, 2, 3
rect = uno.createUnoStruct("com.sun.star.awt.Rectangle", 6000, 500, 8000, 6000)
sh.Charts.addNewByName("chart1", rect, (addr,), True, True)
path = os.path.join(out, tag + ".ods")
if os.path.exists(path):
    os.remove(path)
doc.storeToURL(uno.systemPathToFileUrl(path), (pv("FilterName", "calc8"), pv("Overwrite", True)))
doc.close(True)
d2 = desk.loadComponentFromURL(uno.systemPathToFileUrl(path), "_blank", 0, (pv("Hidden", True),))
res = info(d2)
d2.close(True)
json.dump(res, open(os.path.join(out, "chart-" + tag + ".json"), "w"), indent=1, sort_keys=True)
print(tag, res)
