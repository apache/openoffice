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

# usage: runpy.sh isoweeknum_rt.py <port> <outdir>
# ISOWEEKNUM(d) and WEEKNUM(d;mode) share the ODF name ISOWEEKNUM. Saves both to ODS,
# checks the table:formula strings, reloads and checks that each formula comes back as
# the same function with the same value. ISOWEEKNUM is checked against Python's ISO
# calendar; WEEKNUM values are only compared before and after the round trip.
import sys, os, re, zipfile, datetime as dt
import uno
from unohelp import connect, desktop, load, pv

port, outdir = int(sys.argv[1]), os.path.abspath(sys.argv[2])
os.makedirs(outdir, exist_ok=True)
DATES = [dt.date(2015, 12, 31), dt.date(2021, 1, 3), dt.date(2018, 12, 31),
         dt.date(2024, 2, 29), dt.date(2026, 1, 1), dt.date(2020, 12, 28)]


# per row: (column, API formula, ODF formula)
def formulas(row):
    r = row + 1
    return [("B", "=ISOWEEKNUM(A%d)" % r, "of:=ISOWEEKNUM([.A%d])" % r),
            ("C", "=WEEKNUM(A%d;1)" % r, "of:=ISOWEEKNUM([.A%d];1)" % r),
            ("D", "=WEEKNUM(A%d;2)" % r, "of:=ISOWEEKNUM([.A%d];2)" % r)]


total = fails = 0


def check(what, ok, detail=""):
    global total, fails
    total += 1
    if not ok:
        fails += 1
        print("MISMATCH %s %s" % (what, detail))


def check_sheet(doc, tag, ref):
    sh = doc.getSheets().getByIndex(0)
    for row, d in enumerate(DATES):
        for col, api, odf in formulas(row):
            c = sh.getCellRangeByName("%s%d" % (col, row + 1))
            check("%s %s%d formula" % (tag, col, row + 1), c.getFormula() == api, "expected %s got %s" % (api, c.getFormula()))
            key = (col, row)
            if col == "B":
                ref[key] = d.isocalendar()[1]
            elif key not in ref:
                ref[key] = c.getValue()
            check("%s %s%d value" % (tag, col, row + 1), c.getError() == 0 and c.getValue() == ref[key],
                  "%s expected %s got %s err %d" % (d, ref[key], c.getValue(), c.getError()))


desk = desktop(connect(port))
doc = desk.loadComponentFromURL("private:factory/scalc", "_blank", 0, (pv("Hidden", True),))
sh = doc.getSheets().getByIndex(0)
for row, d in enumerate(DATES):
    sh.getCellRangeByName("A%d" % (row + 1)).setValue((d - dt.date(1899, 12, 30)).days)
    for col, api, odf in formulas(row):
        sh.getCellRangeByName("%s%d" % (col, row + 1)).setFormula(api)
doc.calculateAll()
ref = {}
check_sheet(doc, "new", ref)
path = os.path.join(outdir, "isoweeknum.ods")
doc.storeToURL(uno.systemPathToFileUrl(path), (pv("FilterName", "calc8"),))
doc.close(True)

xml = zipfile.ZipFile(path).read("content.xml").decode("utf-8")
saved = re.findall(r'table:formula="([^"]*)"', xml)
want = [odf for row in range(len(DATES)) for _, _, odf in formulas(row)]
check("content.xml formulas", saved == want, "expected %s got %s" % (want, saved))

doc = load(desk, path)
check_sheet(doc, "reload cached", ref)
doc.calculateAll()
check_sheet(doc, "reload recalculated", ref)
doc.close(True)

check("ran at least one check", total > 0)
print("isoweeknum_rt: %d/%d match" % (total - fails, total))
sys.exit(1 if fails else 0)
