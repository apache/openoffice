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

# usage: runpy.sh fixtures/p8/calc_spelling_check.py <port> <outdir> <mode name> <strict: 1|0>
# The office runs with the save mode of <mode name> (DefaultVersion in its profile). Builds a pivot table with a named group,
# saves the sheet and checks the element that holds the group members in content.xml: the ODF 1.2 and 1.3 modes write
# `table:data-pilot-group-member`, every other mode writes `table:data-pilot-member`, which older versions read. The same
# condition guards `table:database-table-name` (database ranges and pivot sources); no database was available to check those two.
# Exit code 1 when a check fails.
import os, re, sys, zipfile
import uno
from com.sun.star.table import CellAddress
from com.sun.star.table import CellRangeAddress
from unohelp import connect, desktop, pv

port, out, name, strict = int(sys.argv[1]), os.path.abspath(sys.argv[2]), sys.argv[3], sys.argv[4] == "1"
os.makedirs(out, exist_ok=True)
desk = desktop(connect(port))
doc = desk.loadComponentFromURL("private:factory/scalc", "_blank", 0, (pv("Hidden", True),))
sh = doc.Sheets.getByIndex(0)
rows = (("Cat", "Val"), ("x", 1), ("y", 2), ("z", 3), ("x", 4))
for r, row in enumerate(rows):
    for c, v in enumerate(row):
        cell = sh.getCellByPosition(c, r)
        if isinstance(v, str): cell.String = v
        else: cell.Value = v

dp = sh.DataPilotTables
desc = dp.createDataPilotDescriptor()
desc.setSourceRange(CellRangeAddress(0, 0, 0, 1, 4))
fields = desc.DataPilotFields
fields.getByIndex(0).Orientation = uno.Enum("com.sun.star.sheet.DataPilotFieldOrientation", "ROW")
fields.getByIndex(1).Orientation = uno.Enum("com.sun.star.sheet.DataPilotFieldOrientation", "DATA")
dp.insertNewByName("pv", CellAddress(0, 4, 0), desc)
table = dp.getByName("pv")
table.DataPilotFields.getByIndex(0).createNameGroup(("x", "y"))

path = os.path.join(out, "spelling-%s.ods" % name)
doc.storeToURL(uno.systemPathToFileUrl(path), (pv("FilterName", "calc8"),))
doc.close(True)

content = zipfile.ZipFile(path).read("content.xml").decode("utf-8")
group = re.search(r"<table:data-pilot-group\b[^>]*>(.*?)</table:data-pilot-group>", content, re.S)
inner = group.group(1) if group else ""
print("group content:", re.findall(r"<table:data-pilot-[a-z-]*", inner))

checks = [
    ("a named group was written", bool(group)),
    ("group members use the %s element" % ("data-pilot-group-member" if strict else "data-pilot-member"),
     ("<table:data-pilot-group-member" in inner) == strict and ("<table:data-pilot-member" in inner) == (not strict)),
]
fails = 0
for label, ok in checks:
    fails += not ok
    print("%-4s %-7s %s" % ("ok" if ok else "FAIL", name, label))
sys.exit(1 if fails else 0)
