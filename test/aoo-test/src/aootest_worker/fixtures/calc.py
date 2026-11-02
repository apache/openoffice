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
# usage: runpy.sh fixtures/calc/make.py <port> <outdir>
# Calc documents for the Task 12d gap list: validations of every type and operator, and the other table: constructs.
# Expectations for fixtures/calc/verify.py go to <outdir>/../work/ (the checkers treat every file of <outdir> as a package).
import sys, os, json, uno
from unohelp import connect, desktop, pv

port, out = int(sys.argv[1]), os.path.abspath(sys.argv[2])
os.makedirs(out, exist_ok=True)
work = os.path.join(os.path.dirname(out), "work")
os.makedirs(work, exist_ok=True)
CTX = connect(port)
SM = CTX.ServiceManager
desk = desktop(CTX)
failed = []
expect = {}


def E(t, n):
    return uno.Enum("com.sun.star.sheet." + t, n)


def new():
    return desk.loadComponentFromURL("private:factory/scalc", "_blank", 0, (pv("Hidden", True),))


def save(doc, name, filt="calc8"):
    try:
        doc.storeToURL(uno.systemPathToFileUrl(os.path.join(out, name)), (pv("FilterName", filt),))
        print("saved", name, filt)
        return True
    except Exception as e:
        failed.append(name)
        print("FAIL", name, filt, repr(e)[:200])
        return False


def step(label, fn, *a, **kw):
    if os.environ.get("CALC_TRACE"):
        print("step", label, flush=True)
    try:
        return fn(*a, **kw)
    except Exception as e:
        failed.append(label)
        print("FAIL", label, type(e).__name__, str(e)[:300])


def cellrange(sh, addr):
    return sh.getCellRangeByName(addr)


NUMTYPES = ("WHOLE", "DECIMAL", "DATE", "TIME", "TEXT_LEN")
OPS = ("NONE", "EQUAL", "NOT_EQUAL", "GREATER", "GREATER_EQUAL", "LESS", "LESS_EQUAL", "BETWEEN", "NOT_BETWEEN", "FORMULA")
SAMPLE = {"WHOLE": ("5", "10"), "DECIMAL": ("1.5", "9.25"), "DATE": ("DATE(2020;1;31)", "43831"),
          "TIME": ("TIME(8;30;0)", "0.75"), "TEXT_LEN": ("3", "12"), "LIST": ('"a";"b";"c"', ""), "CUSTOM": ("$Sheet1.$B$1>0", "")}


def setval(cell, typ, op, f1, f2, alert="STOP", show_list=1, ignore=True, inp=("", ""), err=("", ""), show_inp=False, show_err=True):
    v = cell.Validation
    v.Type = E("ValidationType", typ)
    v.setOperator(E("ConditionOperator", op))
    v.setFormula1(f1)
    v.setFormula2(f2)
    v.IgnoreBlankCells = ignore
    v.ShowList = show_list
    v.ErrorAlertStyle = E("ValidationAlertStyle", alert)
    v.InputTitle, v.InputMessage = inp
    v.ErrorTitle, v.ErrorMessage = err
    v.ShowInputMessage = show_inp
    v.ShowErrorMessage = show_err
    cell.Validation = v
    r = cell.Validation
    return {"type": r.Type.value, "op": r.getOperator().value, "f1": r.getFormula1(), "f2": r.getFormula2(),
            "alert": r.ErrorAlertStyle.value, "list": r.ShowList, "ignore": r.IgnoreBlankCells,
            "inp": [r.InputTitle, r.InputMessage], "err": [r.ErrorTitle, r.ErrorMessage],
            "showinp": r.ShowInputMessage, "showerr": r.ShowErrorMessage}


def validation_doc(name, cases):
    doc = new()
    sh = doc.Sheets.getByIndex(0)
    for i in range(1, 6):
        sh.getCellByPosition(1, i - 1).Value = i
    rows = []
    for i, (label, args, kw) in enumerate(cases):
        c = sh.getCellByPosition(0, i)
        try:
            exp = setval(c, *args, **kw)
        except Exception as e:
            failed.append(name + ":" + label)
            print("FAIL", name, label, repr(e)[:200])
            continue
        c.Value = 1
        rows.append({"cell": "A%d" % (i + 1), "label": label, "set": [a for a in args], "kw": {k: (list(v) if isinstance(v, tuple) else v) for k, v in kw.items()}, "readback": exp})
    if save(doc, name):
        expect[name] = rows
    doc.close(True)


def validations():
    cases = []
    for t in NUMTYPES:
        for op in OPS:
            f1, f2 = SAMPLE[t]
            cases.append(("%s/%s" % (t, op), (t, op, f1, f2 if op in ("BETWEEN", "NOT_BETWEEN") else ""), {}))
    validation_doc("validation-types.ods", cases)
    cases = []
    for op in OPS:
        cases.append(("CUSTOM/%s" % op, ("CUSTOM", op, "$Sheet1.$B$1>0", ""), {}))
        cases.append(("LIST/%s" % op, ("LIST", op, '"a";"b";"c"', ""), {}))
        cases.append(("ANY/%s" % op, ("ANY", op, "", ""), {"err": ("t", "m")}))
    for t, f1, f2 in (("LIST", "$Sheet1.$B$1:$B$5", ""), ("LIST", '"one";"two"', ""), ("CUSTOM", "ISNUMBER(A1)", ""),
                      ("CUSTOM", "AND(A1>0;A1<10)", ""), ("CUSTOM", 'LEN(A1)=3', ""), ("CUSTOM", '"abc"', "")):
        cases.append(("%s/f1=%s" % (t, f1), (t, "FORMULA" if t == "CUSTOM" else "EQUAL", f1, f2), {}))
    validation_doc("validation-custom-list.ods", cases)
    cases = []
    for alert in ("STOP", "WARNING", "INFO", "MACRO"):
        for show_err in (True, False):
            for ign in (True, False):
                cases.append(("WHOLE/%s/err=%s/ign=%s" % (alert, show_err, ign),
                              ("WHOLE", "BETWEEN", "1", "10"),
                              {"alert": alert, "show_err": show_err, "ignore": ign,
                               "err": ("Macro.Lib.Mod.Fn" if alert == "MACRO" else "Err title", "Err line 1\nErr line 2"),
                               "inp": ("In title", "In text"), "show_inp": ign}))
    for sl in (0, 1, 2):
        for ign in (True, False):
            cases.append(("LIST/showlist=%d/ign=%s" % (sl, ign), ("LIST", "EQUAL", '"x";"y"', ""), {"show_list": sl, "ignore": ign}))
    for sl in (0, 1, 2):
        cases.append(("LIST/distinct/showlist=%d" % sl, ("LIST", "EQUAL", '"x";"y"' + ';"z"' * sl, ""), {"show_list": sl}))
    cases.append(("DATE/msg-only-input", ("DATE", "GREATER", "1", ""), {"inp": ("", "only text"), "show_inp": True, "show_err": False}))
    cases.append(("ANY/input-only", ("ANY", "NONE", "", ""), {"inp": ("Hint", "Type a number"), "show_inp": True, "show_err": False}))
    cases.append(("ANY/error-only", ("ANY", "NONE", "", ""), {"err": ("Bad", "No"), "show_inp": False, "show_err": True}))
    cases.append(("ANY/error-title-only", ("ANY", "NONE", "", ""), {"err": ("Only a title", ""), "alert": "WARNING"}))
    cases.append(("TEXT_LEN/utf8", ("TEXT_LEN", "LESS", "5", ""), {"err": ("Fehler äöü", "Zu lang – <&> \"q\""), "alert": "INFO"}))
    validation_doc("validation-alerts.ods", cases)


def validation_edges():
    cases = []
    for t in ("WHOLE", "DECIMAL", "TEXT_LEN", "LIST", "CUSTOM", "DATE", "TIME"):
        for op in ("EQUAL", "BETWEEN", "NOT_BETWEEN"):
            cases.append(("%s/%s/f1-empty" % (t, op), (t, op, "", "7"), {}))
            cases.append(("%s/%s/f2-empty" % (t, op), (t, op, "3", ""), {}))
            cases.append(("%s/%s/both-empty" % (t, op), (t, op, "", ""), {}))
    validation_doc("validation-edges.ods", cases)


def struct(name, *a):
    return uno.createUnoStruct(name, *a)


def rng_addr(sheet, c1, r1, c2, r2):
    return struct("com.sun.star.table.CellRangeAddress", sheet, c1, r1, c2, r2)


def cell_addr(sheet, c, r):
    return struct("com.sun.star.table.CellAddress", sheet, c, r)


def fill_table(sh, rows=12):
    heads = ("Region", "Product", "Qty", "Price", "Day")
    for c, h in enumerate(heads):
        sh.getCellByPosition(c, 0).String = h
    regions = ("North", "South", "East", "West")
    prods = ("Apple", "Pear", "Plum")
    for r in range(1, rows):
        sh.getCellByPosition(0, r).String = regions[r % 4]
        sh.getCellByPosition(1, r).String = prods[r % 3]
        sh.getCellByPosition(2, r).Value = r * 3 % 17
        sh.getCellByPosition(3, r).Value = 1.25 * r
        sh.getCellByPosition(4, r).Value = 43831 + r
    return sh


def pvs(**kw):
    return tuple(pv(k, v) for k, v in kw.items())


def sheets_doc(n):
    doc = new()
    for i in range(1, n):
        doc.Sheets.insertNewByName("Sh%d" % (i + 1), i)
    return doc


def filter_field(field, op, value, numeric, conn="AND"):
    f = struct("com.sun.star.sheet.TableFilterField2")
    f.Field = field
    f.IsNumeric = numeric
    f.NumericValue = value if numeric else 0.0
    f.StringValue = "" if numeric else value
    f.Operator = uno.getConstantByName("com.sun.star.sheet.FilterOperator2." + op)
    f.Connection = E("FilterConnection", conn)
    return f


def add_db(doc, i, name, rows=12):
    fill_table(doc.Sheets.getByIndex(i), rows)
    doc.DatabaseRanges.addNewByName(name, rng_addr(i, 0, 0, 4, rows - 1))
    return doc.DatabaseRanges.getByName(name)


def dbranges():
    ops = ["EQUAL", "NOT_EQUAL", "GREATER", "GREATER_EQUAL", "LESS", "LESS_EQUAL", "BOTTOM_PERCENT", "BOTTOM_VALUES", "EMPTY",
           "NOT_EMPTY", "TOP_PERCENT", "TOP_VALUES", "CONTAINS", "DOES_NOT_CONTAIN", "BEGINS_WITH", "DOES_NOT_BEGIN_WITH",
           "ENDS_WITH", "DOES_NOT_END_WITH"]
    patterns = {"single": [None], "and2": [None, "AND"], "or2": [None, "OR"], "and3": [None, "AND", "AND"],
                "or3": [None, "OR", "OR"], "and-or": [None, "AND", "OR"], "or-and": [None, "OR", "AND"],
                "and-or-and": [None, "AND", "OR", "AND"], "or-and-and": [None, "OR", "AND", "AND"],
                "and-and-or": [None, "AND", "AND", "OR"], "or-or-and": [None, "OR", "OR", "AND"]}
    doc = sheets_doc(len(patterns))

    def one(i, name, conns):
        db = add_db(doc, i, name)
        fd = db.getFilterDescriptor()
        fd.setFilterFields2(tuple(filter_field(k % 5, "EQUAL", 5.5 + k if k % 5 in (2, 3) else "North", k % 5 in (2, 3), cn or "AND")
                                  for k, cn in enumerate(conns)))
        fd.IsCaseSensitive = i % 2 == 0
        fd.SkipDuplicates = i % 3 == 0
        fd.UseRegularExpressions = i % 4 == 0
        if i % 5 == 0:
            fd.CopyOutputData = True
            fd.OutputPosition = cell_addr(i, 7, 14)

    for i, (name, conns) in enumerate(patterns.items()):
        step("filter-" + name, one, i, "f_" + name, conns)
    save(doc, "filter-connections.ods")
    doc.close(True)

    doc = sheets_doc(len(ops) * 2)
    for oi, o in enumerate(ops):
        for rx in (False, True):
            def mk(o=o, rx=rx, i=oi * 2 + rx):
                db = add_db(doc, i, "f_%s%s" % (o.lower(), "_rx" if rx else ""))
                num = o in ("BOTTOM_PERCENT", "BOTTOM_VALUES", "TOP_PERCENT", "TOP_VALUES", "GREATER", "GREATER_EQUAL", "LESS", "LESS_EQUAL")
                fd = db.getFilterDescriptor()
                fd.setFilterFields2((filter_field(2 if num else 0, o, 3.0 if num else "o", num),))
                fd.UseRegularExpressions = rx
            step("op-" + o, mk)
    save(doc, "filter-operators.ods")
    doc.close(True)

    doc = sheets_doc(8)
    sort_fields = lambda *fs: uno.Any("[]com.sun.star.table.TableSortField", tuple(
        struct("com.sun.star.table.TableSortField", f, asc, cs, uno.Enum("com.sun.star.table.TableSortFieldType", ft), struct("com.sun.star.lang.Locale", "en", "US", ""), "")
        for f, asc, cs, ft in fs))

    def sorts():
        for i, (cs, bind, copy, uls) in enumerate(((False, True, False, False), (True, False, True, False), (False, True, False, True), (True, True, True, False))):
            db = add_db(doc, i, "sort%d" % i)
            sd = db.getSortDescriptor()
            sd2 = []
            for p in sd:
                v = p.Value
                if p.Name == "SortFields":
                    v = sort_fields((0, True, cs, "ALPHANUMERIC"), (2, False, cs, "NUMERIC"), (3, True, cs, "AUTOMATIC"))
                elif p.Name == "BindFormatsToContent":
                    v = bind
                elif p.Name == "CopyOutputData":
                    v = copy
                elif p.Name == "OutputPosition":
                    v = cell_addr(i, 7, 14)
                elif p.Name == "IsUserListEnabled":
                    v = uls
                elif p.Name == "UserListIndex":
                    v = 1
                elif p.Name == "ContainsHeader":
                    v = i != 1
                sd2.append(pv(p.Name, v))
            doc.Sheets.getByIndex(i).getCellRangeByName("A1:E12").sort(tuple(sd2))
    step("sorts", sorts)

    def subtotals():
        for i in (4, 5):
            db = add_db(doc, i, "sub%d" % i)
            st = db.getSubTotalDescriptor()
            st.InsertPageBreaks = i == 5
            st.IsCaseSensitive = i == 5
            st.BindFormatsToContent = i == 4
            st.EnableSort = True
            st.SortAscending = i == 4
            if i == 5:
                st.EnableUserSortList = True
                st.UserSortListIndex = 1
            funcs = ["SUM", "COUNT", "AVERAGE", "MAX", "MIN", "PRODUCT", "COUNTNUMS", "STDEV", "STDEVP", "VAR", "VARP"]
            f0 = st.addNew((), 0)
            cols = tuple(struct("com.sun.star.sheet.SubTotalColumn", 2 + (k % 2), uno.Enum("com.sun.star.sheet.GeneralFunction", fn)) for k, fn in enumerate(funcs))
            sf = st.getByIndex(0)
            sf.setSubTotalColumns(cols)
            st.addNew((struct("com.sun.star.sheet.SubTotalColumn", 3, uno.Enum("com.sun.star.sheet.GeneralFunction", "SUM")),), 1)
    step("subtotals", subtotals)

    def attrs():
        for i, (keepf, mov, strip, auto, refresh) in enumerate(((True, True, True, True, 60), (False, False, False, False, 0))):
            db = add_db(doc, 6 + i, "attr%d" % i)
            db.KeepFormats, db.MoveCells, db.StripData, db.AutoFilter, db.RefreshPeriod = keepf, mov, strip, auto, refresh
            fd = db.getFilterDescriptor()
            fd.ContainsHeader = i == 1
            sd = tuple(pv(p.Name, (i == 0) if p.Name == "IsSortColumns" else p.Value) for p in db.getSortDescriptor())
            doc.Sheets.getByIndex(6 + i).getCellRangeByName("A1:E12").sort(sd)
    step("dbattrs", attrs)
    save(doc, "dbranges-sort-subtotal.ods")
    doc.close(True)


step("dbranges", dbranges)


def pivot_source(doc, sh, rows=40):
    heads = ("Region", "Product", "Qty", "Price", "Day", "Hour")
    for c, h in enumerate(heads):
        sh.getCellByPosition(c, 0).String = h
    regions = ("North", "South", "East", "West")
    prods = ("Apple", "Pear", "Plum")
    nf = doc.NumberFormats
    loc = struct("com.sun.star.lang.Locale", "en", "US", "")
    dfmt = nf.queryKey("YYYY-MM-DD", loc, False)
    if dfmt == -1:
        dfmt = nf.addNew("YYYY-MM-DD", loc)
    tfmt = nf.queryKey("HH:MM:SS", loc, False)
    if tfmt == -1:
        tfmt = nf.addNew("HH:MM:SS", loc)
    for r in range(1, rows):
        sh.getCellByPosition(0, r).String = regions[r % 4]
        sh.getCellByPosition(1, r).String = prods[r % 3]
        sh.getCellByPosition(2, r).Value = (r * 7) % 23
        sh.getCellByPosition(3, r).Value = 1.25 * r
        c = sh.getCellByPosition(4, r)
        c.Value = 43831 + r * 37
        c.NumberFormat = dfmt
        c = sh.getCellByPosition(5, r)
        c.Value = (r * 3671 % 86400) / 86400.0
        c.NumberFormat = tfmt
    return rng_addr(sh.RangeAddress.Sheet, 0, 0, 5, rows - 1)


def new_pivot(doc, sheetname, name, src, at=(8, 2), fields=None, **props):
    sh = doc.Sheets.getByName(sheetname)
    dps = sh.DataPilotTables
    desc = dps.createDataPilotDescriptor()
    desc.setSourceRange(src)
    fl = desc.getDataPilotFields()
    for fname, fp in (fields or {}).items():
        f = fl.getByName(fname) if hasattr(fl, "getByName") else None
        for k, v in fp.items():
            setattr(f, k, v) if not isinstance(v, uno.Enum) else f.setPropertyValue(k, v)
    for k, v in props.items():
        desc.setPropertyValue(k, v)
    dps.insertNewByName(name, cell_addr(sh.RangeAddress.Sheet, at[0], at[1]), desc)
    return dps.getByName(name)


def orient(n):
    return uno.Enum("com.sun.star.sheet.DataPilotFieldOrientation", n)


def gf(n):
    return uno.Enum("com.sun.star.sheet.GeneralFunction", n)


def pivots():
    doc = new()
    sheets = doc.Sheets
    names = ["Src", "Funcs", "Refs", "Sorts", "AutoShow", "Layout", "Groups", "Totals", "Filtered", "Pages"]
    for i, n in enumerate(names[1:], 1):
        sheets.insertNewByName(n, i)
    sheets.getByIndex(0).Name = "Src"
    src = pivot_source(doc, sheets.getByName("Src"))
    funcs = ["SUM", "COUNT", "AVERAGE", "MAX", "MIN", "PRODUCT", "COUNTNUMS", "STDEV", "STDEVP", "VAR", "VARP"]
    FN = "Funcs"

    def func_pivot(k, fn):
        t = new_pivot(doc, FN, "pf_" + fn.lower(), src, at=(k * 4, 1 + 0), fields={
            "Region": {"Orientation": orient("ROW")}, "Qty": {"Orientation": orient("DATA"), "Function": gf(fn)}})
    for k, fn in enumerate(funcs):
        step("func-" + fn, func_pivot, k, fn)

    RT = "com.sun.star.sheet.DataPilotFieldReferenceType."
    IT = "com.sun.star.sheet.DataPilotFieldReferenceItemType."

    def ref_pivot(k, rtype, itype):
        t = new_pivot(doc, "Refs", "pr_%s_%s" % (rtype.lower(), itype.lower()), src, at=(k * 4, 1), fields={
            "Region": {"Orientation": orient("ROW")}, "Product": {"Orientation": orient("COLUMN")},
            "Qty": {"Orientation": orient("DATA")}})
        f = t.getDataPilotFields().getByName("Qty")
        r = struct("com.sun.star.sheet.DataPilotFieldReference")
        r.ReferenceType = uno.getConstantByName(RT + rtype)
        r.ReferenceField = "Region"
        r.ReferenceItemType = uno.getConstantByName(IT + itype)
        r.ReferenceItemName = "North" if itype == "NAMED" else ""
        f.setPropertyValue("Reference", r)
    k = 0
    for rtype in ("NONE", "ITEM_DIFFERENCE", "ITEM_PERCENTAGE", "ITEM_PERCENTAGE_DIFFERENCE", "RUNNING_TOTAL", "ROW_PERCENTAGE",
                  "COLUMN_PERCENTAGE", "TOTAL_PERCENTAGE", "INDEX"):
        for itype in (("NAMED", "PREVIOUS", "NEXT") if rtype.startswith("ITEM") else ("NAMED",)):
            step("ref-%s-%s" % (rtype, itype), ref_pivot, k, rtype, itype)
            k += 1

    SM = "com.sun.star.sheet.DataPilotFieldSortMode."

    def sort_pivot(k, mode, asc):
        t = new_pivot(doc, "Sorts", "ps_%d" % k, src, at=(k * 4, 1), fields={
            "Region": {"Orientation": orient("ROW")}, "Qty": {"Orientation": orient("DATA")}})
        f = t.getDataPilotFields().getByName("Region")
        si = struct("com.sun.star.sheet.DataPilotFieldSortInfo")
        si.Field = "Qty" if mode == "DATA" else ""
        si.IsAscending = asc
        si.Mode = uno.getConstantByName(SM + mode)
        f.setPropertyValue("SortInfo", si)
    k = 0
    for mode in ("NONE", "MANUAL", "NAME", "DATA"):
        for asc in (True, False):
            step("sort-%s-%s" % (mode, asc), sort_pivot, k, mode, asc)
            k += 1

    def auto_pivot(k, mode, enabled):
        t = new_pivot(doc, "AutoShow", "pa_%d" % k, src, at=(k * 4, 1), fields={
            "Region": {"Orientation": orient("ROW")}, "Qty": {"Orientation": orient("DATA")}})
        f = t.getDataPilotFields().getByName("Region")
        ai = struct("com.sun.star.sheet.DataPilotFieldAutoShowInfo")
        ai.IsEnabled = enabled
        ai.ShowItemsMode = uno.getConstantByName("com.sun.star.sheet.DataPilotFieldShowItemsMode." + mode)
        ai.ItemCount = 2
        ai.DataField = "Qty"
        f.setPropertyValue("AutoShowInfo", ai)
    k = 0
    for mode in ("FROM_TOP", "FROM_BOTTOM"):
        for en in (True, False):
            step("autoshow-%s-%s" % (mode, en), auto_pivot, k, mode, en)
            k += 1

    def layout_pivot(k, mode, empty):
        t = new_pivot(doc, "Layout", "pl_%d" % k, src, at=(k * 5, 1), fields={
            "Region": {"Orientation": orient("ROW")}, "Product": {"Orientation": orient("ROW")}, "Qty": {"Orientation": orient("DATA")}})
        f = t.getDataPilotFields().getByName("Region")
        li = struct("com.sun.star.sheet.DataPilotFieldLayoutInfo")
        li.AddEmptyLines = empty
        li.LayoutMode = uno.getConstantByName("com.sun.star.sheet.DataPilotFieldLayoutMode." + mode)
        f.setPropertyValue("LayoutInfo", li)
    k = 0
    for mode in ("TABULAR_LAYOUT", "OUTLINE_SUBTOTALS_TOP", "OUTLINE_SUBTOTALS_BOTTOM"):
        for empty in (True, False):
            step("layout-%s-%s" % (mode, empty), layout_pivot, k, mode, empty)
            k += 1

    def totals_pivot(k, row, col, **kw):
        new_pivot(doc, "Totals", "pt_%d" % k, src, at=(k * 4, 1), fields={
            "Region": {"Orientation": orient("ROW")}, "Product": {"Orientation": orient("COLUMN")},
            "Qty": {"Orientation": orient("DATA")}}, RowGrand=row, ColumnGrand=col, **kw)
    for k, (row, col, kw) in enumerate(((True, True, {}), (True, False, {}), (False, True, {}), (False, False, {}),
                                        (True, True, {"IgnoreEmptyRows": True}), (True, True, {"RepeatIfEmpty": True}),
                                        (True, True, {"ShowFilterButton": False}), (True, True, {"DrillDownOnDoubleClick": False}))):
        step("totals-%d" % k, totals_pivot, k, row, col, **kw)

    def sub_pivot():
        t = new_pivot(doc, "Totals", "pt_sub", src, at=(0, 12), fields={
            "Region": {"Orientation": orient("ROW")}, "Product": {"Orientation": orient("ROW")}, "Qty": {"Orientation": orient("DATA")}})
        f = t.getDataPilotFields().getByName("Region")
        uno.invoke(f, "setPropertyValue", ("Subtotals", uno.Any("[]com.sun.star.sheet.GeneralFunction", tuple(gf(n) for n in ("SUM", "COUNT", "AVERAGE", "MAX")))))
        f.setPropertyValue("ShowEmpty", True)
        f2 = t.getDataPilotFields().getByName("Product")
        uno.invoke(f2, "setPropertyValue", ("Subtotals", uno.Any("[]com.sun.star.sheet.GeneralFunction", (gf("AUTO"),))))
        items = f.getItems()
        for n in items.ElementNames[:2]:
            it = items.getByName(n)
            it.IsHidden = n == items.ElementNames[0]
            it.ShowDetail = False
    step("subtotals", sub_pivot)

    def page_pivot():
        t = new_pivot(doc, "Pages", "pg_1", src, at=(0, 3), fields={
            "Region": {"Orientation": orient("PAGE")}, "Product": {"Orientation": orient("ROW")}, "Qty": {"Orientation": orient("DATA")}})
        f = t.getDataPilotFields().getByName("Region")
        f.setPropertyValue("CurrentPage", "South")
    step("page", page_pivot)

    def hidden_pivot():
        t = new_pivot(doc, "Pages", "pg_2", src, at=(6, 3), fields={
            "Region": {"Orientation": orient("HIDDEN")}, "Product": {"Orientation": orient("COLUMN")},
            "Qty": {"Orientation": orient("DATA")}, "Price": {"Orientation": orient("DATA")}})
    step("two-data-fields", hidden_pivot)

    def filtered_pivot():
        sh = doc.Sheets.getByName("Filtered")
        dps = sh.DataPilotTables
        desc = dps.createDataPilotDescriptor()
        desc.setSourceRange(src)
        fl = desc.getDataPilotFields()
        fl.getByName("Region").Orientation = orient("ROW")
        fl.getByName("Qty").Orientation = orient("DATA")
        fd = desc.getFilterDescriptor()
        fd.setFilterFields2((filter_field(1, "EQUAL", "Apple", False, "AND"), filter_field(2, "GREATER", 2.0, True, "AND"),
                             filter_field(0, "NOT_EQUAL", "East", False, "OR")))
        fd.IsCaseSensitive = True
        fd.SkipDuplicates = True
        fd.UseRegularExpressions = False
        dps.insertNewByName("pfil_1", cell_addr(doc.Sheets.getByName("Filtered").RangeAddress.Sheet, 0, 0), desc)
        desc2 = dps.createDataPilotDescriptor()
        desc2.setSourceRange(src)
        desc2.getDataPilotFields().getByName("Product").Orientation = orient("ROW")
        desc2.getDataPilotFields().getByName("Price").Orientation = orient("DATA")
        fd = desc2.getFilterDescriptor()
        fd.setFilterFields2((filter_field(0, "EQUAL", "N.*", False, "OR"), filter_field(1, "CONTAINS", "ea", False, "OR")))
        fd.UseRegularExpressions = True
        dps.insertNewByName("pfil_2", cell_addr(doc.Sheets.getByName("Filtered").RangeAddress.Sheet, 6, 0), desc2)
    step("filtered", filtered_pivot)

    GB = "com.sun.star.sheet.DataPilotFieldGroupBy."

    def date_group(k, part, auto, fname="Day"):
        t = new_pivot(doc, "Groups", "pd_%d" % k, src, at=(k * 4, 1), fields={
            fname: {"Orientation": orient("ROW")}, "Qty": {"Orientation": orient("DATA")}})
        f = t.getDataPilotFields().getByName(fname)
        gi = struct("com.sun.star.sheet.DataPilotFieldGroupInfo")
        gi.HasAutoStart = auto
        gi.HasAutoEnd = auto
        gi.HasDateValues = True
        gi.Start = 43831.0 if fname == "Day" else 0.0
        gi.End = 50000.0 if fname == "Day" else 0.99
        gi.Step = 1.0 if part == "DAYS" else 0.0
        gi.GroupBy = uno.getConstantByName(GB + part)
        f.createDateGroup(gi)
    k = 0
    for part, fname in (("YEARS", "Day"), ("QUARTERS", "Day"), ("MONTHS", "Day"), ("DAYS", "Day"), ("HOURS", "Hour"),
                        ("MINUTES", "Hour"), ("SECONDS", "Hour")):
        for auto in (True, False):
            step("dategroup-%s-%s" % (part, auto), date_group, k, part, auto, fname)
            k += 1

    def num_group(k, auto):
        t = new_pivot(doc, "Groups", "pn_%d" % k, src, at=(60 + k * 4, 1), fields={
            "Day": {"Orientation": orient("ROW")}, "Price": {"Orientation": orient("DATA")}})
        f = t.getDataPilotFields().getByName("Day")
        gi = struct("com.sun.star.sheet.DataPilotFieldGroupInfo")
        gi.HasAutoStart = auto
        gi.HasAutoEnd = auto
        gi.HasDateValues = True
        gi.Start = 43831.0
        gi.End = 45000.0
        gi.Step = 7.0
        gi.GroupBy = uno.getConstantByName(GB + "DAYS")
        f.createDateGroup(gi)
    for k, auto in enumerate((True, False)):
        step("dayrange-%s" % auto, num_group, k, auto)

    def name_group():
        t = new_pivot(doc, "Groups", "pg_names", src, at=(80, 1), fields={
            "Region": {"Orientation": orient("ROW")}, "Qty": {"Orientation": orient("DATA")}})
        f = t.getDataPilotFields().getByName("Region")
        f.createNameGroup(("North", "South"))
        f.createNameGroup(("East",))
    step("namegroup", name_group)
    save(doc, "pivot-tables.ods")
    doc.close(True)


step("pivots", pivots)


def tracked_doc(name, ops):
    doc = new()
    sh = doc.Sheets.getByIndex(0)
    doc.Sheets.insertNewByName("Other", 1)
    for r in range(8):
        for c in range(4):
            sh.getCellByPosition(c, r).Value = r * 10 + c
        sh.getCellByPosition(4, r).Formula = "=SUM(A%d:D%d)" % (r + 1, r + 1)
    fr = doc.CurrentController.Frame
    disp = SM.createInstanceWithContext("com.sun.star.frame.DispatchHelper", CTX)
    disp.executeDispatch(fr, ".uno:TraceChangeMode", "", 0, (pv("TraceChangeMode", True),))
    for label, fn in ops:
        step(name + ":" + label, fn, doc, sh)
    save(doc, name)
    doc.close(True)


def tracked_changes():
    def a(doc, sh):
        sh.getCellByPosition(0, 0).Value = 111
        sh.getCellByPosition(0, 0).Value = 112
        sh.getCellByPosition(1, 0).String = "text"
        sh.getCellByPosition(5, 5).Formula = "=A1+1"
        sh.getCellByPosition(5, 5).Value = 3
        sh.getCellRangeByName("A2:B3").clearContents(1 | 2 | 4 | 16)
    def b(doc, sh):
        sh.Rows.insertByIndex(2, 2)
        sh.Rows.insertByIndex(3, 1)
        sh.Rows.removeByIndex(2, 3)
        sh.Columns.insertByIndex(1, 2)
        sh.Columns.removeByIndex(2, 1)
        sh.getCellByPosition(0, 0).Value = 7
    def c(doc, sh):
        sh.Rows.insertByIndex(4, 3)
        sh.getCellByPosition(0, 5).Value = 1
        sh.Rows.removeByIndex(5, 2)
        sh.Rows.removeByIndex(3, 1)
    def d(doc, sh):
        sh.moveRange(sh.getCellByPosition(7, 0).CellAddress, sh.getCellRangeByName("A1:B2").RangeAddress)
        sh.getCellByPosition(7, 0).Value = 5
        sh.moveRange(sh.getCellByPosition(9, 3).CellAddress, sh.getCellRangeByName("H1:I2").RangeAddress)
        sh.copyRange(sh.getCellByPosition(0, 12).CellAddress, sh.getCellRangeByName("C1:D4").RangeAddress)
        sh.Rows.removeByIndex(0, 1)
    def e(doc, sh):
        o = doc.Sheets.getByName("Other")
        o.getCellByPosition(0, 0).Value = 4
        o.getCellByPosition(0, 1).Formula = "=$Sheet1.A1+A1"
        sh.getCellByPosition(0, 0).Value = 8
        sh.Columns.removeByIndex(0, 1)
        doc.Sheets.removeByName("Other")
    def f(doc, sh):
        sh.getCellByPosition(0, 0).String = "multi\nline"
        sh.getCellByPosition(1, 0).Value = 1.5
        sh.getCellByPosition(2, 0).Formula = "=1+1"
        sh.getCellByPosition(3, 0).Formula = '="s"&"t"'
        sh.getCellByPosition(0, 0).Value = 3
        sh.Rows.removeByIndex(0, 1)
        sh.Rows.removeByIndex(0, 4)
    tracked_doc("tracked-content.ods", [("a", a)])
    tracked_doc("tracked-insdel.ods", [("b", b)])
    tracked_doc("tracked-cutoffs.ods", [("c", c)])
    tracked_doc("tracked-move.ods", [("d", d)])
    tracked_doc("tracked-sheets.ods", [("e", e)])
    tracked_doc("tracked-deleted-content.ods", [("f", f)])
    tracked_doc("tracked-all.ods", [("a", a), ("b", b), ("c", c), ("d", d), ("f", f)])


step("tracked", tracked_changes)


def link_source():
    path = os.path.join(work, "link-source.ods")
    doc = new()
    sh = doc.Sheets.getByIndex(0)
    sh.Name = "SrcSheet"
    for r in range(5):
        for c in range(4):
            sh.getCellByPosition(c, r).Value = r * 4 + c
    doc.storeToURL(uno.systemPathToFileUrl(path), (pv("FilterName", "calc8"),))
    doc.close(True)
    return uno.systemPathToFileUrl(path)


def links():
    url = link_source()
    doc = sheets_doc(4)

    def arealinks():
        sh = doc.Sheets.getByIndex(0)
        doc.AreaLinks.insertAtPosition(cell_addr(0, 0, 0), url, "SrcSheet.A1:D5", "calc8", "")
        doc.AreaLinks.insertAtPosition(cell_addr(0, 6, 0), url, "SrcSheet.B2:C4", "calc8", "")
        al = doc.AreaLinks.getByIndex(1)
        al.RefreshDelay = 90
    step("arealinks", arealinks)

    def sheetlinks():
        for i, mode in ((1, "NORMAL"), (2, "VALUE")):
            sh = doc.Sheets.getByIndex(i)
            sh.link(url, "SrcSheet", "calc8", "", E("SheetLinkMode", mode))
        sl = doc.SheetLinks
        for n in sl.ElementNames:
            l = sl.getByName(n)
            l.RefreshDelay = 120
            break
    step("sheetlinks", sheetlinks)
    save(doc, "links.ods")
    doc.close(True)


def scenarios():
    doc = sheets_doc(1)
    sh = doc.Sheets.getByIndex(0)
    for r in range(6):
        for c in range(4):
            sh.getCellByPosition(c, r).Value = r * 4 + c

    def mk(name, ranges, comment):
        doc.Sheets.getByIndex(0).Scenarios.addNewByName(name, ranges, comment)
        scn = doc.Sheets.getByName(name)
        return scn

    def sc1():
        scn = mk("scn_one", (rng_addr(0, 0, 0, 1, 1),), "first scenario")
        scn.getCellByPosition(0, 0).Value = 100
        scn.ShowBorder = True
        scn.PrintBorder = True
        scn.CopyBack = True
        scn.CopyStyles = True
        scn.CopyFormulas = True
        scn.BorderColor = 0x00FF8000
        scn.IsActive = True
    step("scenario-1", sc1)

    def sc2():
        scn = mk("scn_two", (rng_addr(0, 2, 2, 3, 3), rng_addr(0, 0, 5, 0, 5)), "")
        scn.ShowBorder = False
        scn.PrintBorder = False
        scn.CopyBack = False
        scn.CopyStyles = False
        scn.CopyFormulas = False
        scn.Protected = True
        scn.IsActive = False
    step("scenario-2", sc2)

    def sc3():
        scn = mk("scn_three", (rng_addr(0, 1, 1, 2, 2),), "third \u00e4 & <b>")
        scn.CopyFormulas = True
    step("scenario-3", sc3)
    save(doc, "scenarios.ods")
    doc.close(True)


def consolidation():
    doc = sheets_doc(3)
    for i in range(2):
        sh = doc.Sheets.getByIndex(i)
        for r in range(4):
            sh.getCellByPosition(0, r).String = "k%d" % r
            for c in range(1, 3):
                sh.getCellByPosition(c, r).Value = r + c + i
        sh.getCellByPosition(1, 0).String = "h1"
        sh.getCellByPosition(2, 0).String = "h2"
    fns = ["SUM", "COUNT", "AVERAGE", "MAX", "MIN", "PRODUCT", "COUNTNUMS", "STDEV", "STDEVP", "VAR", "VARP"]
    for k, fn in enumerate(fns[:6]):
        def one(k=k, fn=fn):
            cd = doc.createConsolidationDescriptor(True)
            cd.Function = gf(fn)
            cd.UseColumnHeaders = k % 2 == 0
            cd.UseRowHeaders = k % 3 != 0
            cd.Sources = (rng_addr(0, 0, 0, 2, 3), rng_addr(1, 0, 0, 2, 3))
            cd.StartOutputPosition = cell_addr(2, 0, k * 6)
            cd.InsertLinks = k % 2 == 1
            doc.consolidate(cd)
        step("consolidate-" + fn, one)
        if k == 5:
            break
    save(doc, "consolidation.ods")
    doc.close(True)

    for fn, rows, cols, links in (("SUM", False, False, False), ("MAX", True, True, True), ("MIN", False, True, False)):
        doc = sheets_doc(3)
        for i in range(2):
            sh = doc.Sheets.getByIndex(i)
            for r in range(4):
                sh.getCellByPosition(0, r).String = "k%d" % r
                for c in range(1, 3):
                    sh.getCellByPosition(c, r).Value = r + c + i

        def one():
            cd = doc.createConsolidationDescriptor(True)
            cd.Function = gf(fn)
            cd.UseColumnHeaders = cols
            cd.UseRowHeaders = rows
            cd.Sources = (rng_addr(0, 0, 0, 2, 3), rng_addr(1, 0, 0, 2, 3))
            cd.StartOutputPosition = cell_addr(2, 0, 0)
            cd.InsertLinks = links
            doc.consolidate(cd)
        step("consolidate-%s-%s%s" % (fn, rows, cols), one)
        save(doc, "consolidation-%s.ods" % fn.lower())
        doc.close(True)


def detective():
    doc = new()
    sh = doc.Sheets.getByIndex(0)
    for r in range(4):
        sh.getCellByPosition(0, r).Value = r + 1
    sh.getCellByPosition(1, 0).Formula = "=SUM(A1:A4)"
    sh.getCellByPosition(1, 1).Formula = "=B1*2"
    sh.getCellByPosition(1, 2).Formula = "=1/0"
    sh.getCellByPosition(1, 3).Formula = "=B3+1"
    v = sh.getCellByPosition(2, 0).Validation
    v.Type = E("ValidationType", "WHOLE")
    v.setOperator(E("ConditionOperator", "BETWEEN"))
    v.setFormula1("1")
    v.setFormula2("10")
    sh.getCellByPosition(2, 0).Validation = v
    sh.getCellByPosition(2, 0).Value = 99
    ctrl = doc.CurrentController
    disp = SM.createInstanceWithContext("com.sun.star.frame.DispatchHelper", CTX)

    def do(cmd, addr):
        ctrl.select(sh.getCellRangeByName(addr))
        disp.executeDispatch(ctrl.Frame, ".uno:" + cmd, "", 0, ())
    for cmd, addr in (("ShowPrecedents", "B1"), ("ShowDependents", "A2"), ("ShowDependents", "B2"), ("ShowPrecedents", "B2"), ("ShowErrors", "B4"),
                      ("MarkErrors", "B3"), ("ShowInvalid", "C1"), ("AddPrecedents", "B1"), ("AddDependents", "A1"), ("ShowDependents", "A4"),
                      ("ShowPrecedents", "B4")):
        step("detective-" + cmd, do, cmd, addr)
    save(doc, "detective.ods")
    doc.close(True)


def shapes_doc():
    doc = sheets_doc(2)
    sh = doc.Sheets.getByIndex(0)
    for r in range(5):
        sh.getCellByPosition(0, r).Value = r
    dp = sh.DrawPage

    def add(kind, x, y, w, h, anchor_cell=None, text=""):
        shp = doc.createInstance("com.sun.star.drawing." + kind)
        dp.add(shp)
        shp.Position = struct("com.sun.star.awt.Point", x, y)
        shp.Size = struct("com.sun.star.awt.Size", w, h)
        if text:
            shp.String = text
        shp.Anchor = sh.getCellRangeByName(anchor_cell) if anchor_cell is not None else sh
        return shp
    step("shape-page-rect", add, "RectangleShape", 3000, 1000, 3000, 2000, None, "page anchored")
    step("shape-page-ellipse", add, "EllipseShape", 7000, 1000, 2000, 2000)
    step("shape-cell-rect", add, "RectangleShape", 3000, 4000, 3000, 2000, "B3", "cell anchored")
    step("shape-cell-line", add, "LineShape", 3000, 7000, 3000, 1000, "C5")
    step("shape-cell-text", add, "TextShape", 8000, 5000, 3000, 1000, "D2", "text")
    step("shape-conn", add, "ConnectorShape", 1000, 9000, 3000, 1000)
    sh2 = doc.Sheets.getByIndex(1)
    shp = doc.createInstance("com.sun.star.drawing.RectangleShape")
    sh2.DrawPage.add(shp)
    shp.Size = struct("com.sun.star.awt.Size", 2000, 1000)
    shp.Position = struct("com.sun.star.awt.Point", 500, 500)
    save(doc, "shapes.ods")
    doc.close(True)


def calc_settings():
    docs = {"calc-settings-a.ods": dict(CalcAsShown=True, IgnoreCase=True, LookUpLabels=False, MatchWholeCell=False, RegularExpressions=False,
                                        IsIterationEnabled=True, IterationCount=250, IterationEpsilon=0.0005, NullDate=struct("com.sun.star.util.Date", 1, 1, 1904)),
            "calc-settings-b.ods": dict(IterationCount=50, NullDate=struct("com.sun.star.util.Date", 31, 12, 1899)),
            "calc-settings-c.ods": dict(IsIterationEnabled=True, IterationEpsilon=0.01),
            "calc-settings-d.ods": dict(CalcAsShown=True),
            "calc-settings-e.ods": dict(MatchWholeCell=False),
            "calc-settings-f.ods": dict(LookUpLabels=False),
            "calc-settings-g.ods": dict(RegularExpressions=False),
            "calc-settings-h.ods": dict(IgnoreCase=True)}
    for name, props in docs.items():
        doc = new()
        for k, v in props.items():
            step(name + ":" + k, doc.setPropertyValue, k, v)
        doc.Sheets.getByIndex(0).getCellByPosition(0, 0).Value = 1.234
        save(doc, name)
        doc.close(True)


def null_year():
    cp = SM.createInstanceWithContext("com.sun.star.configuration.ConfigurationProvider", CTX)
    ca = cp.createInstanceWithArguments("com.sun.star.configuration.ConfigurationUpdateAccess", (pv("nodepath", "/org.openoffice.Office.Common/DateFormat"),))
    old = ca.getByName("TwoDigitYear")
    try:
        ca.setPropertyValue("TwoDigitYear", 1950)
        ca.commitChanges()
        doc = new()
        doc.Sheets.getByIndex(0).getCellByPosition(0, 0).Value = 1.5
        save(doc, "calc-settings-null-year.ods")
        doc.close(True)
    finally:
        ca2 = cp.createInstanceWithArguments("com.sun.star.configuration.ConfigurationUpdateAccess", (pv("nodepath", "/org.openoffice.Office.Common/DateFormat"),))
        ca2.setPropertyValue("TwoDigitYear", old)
        ca2.commitChanges()


def db_imports():
    dbc = SM.createInstance("com.sun.star.sdb.DatabaseContext")
    if "Bibliography" not in dbc.ElementNames:
        print("FAIL db_imports: no Bibliography data source")
        return
    ds = dbc.getByName("Bibliography")
    qd = ds.QueryDefinitions
    if not qd.hasByName("t12q"):
        q = qd.createInstance()
        q.Command = "SELECT * FROM biblio"
        qd.insertByName("t12q", q)
    doc = sheets_doc(6)
    DIM = "com.sun.star.sheet.DataImportMode"
    cases = [("TABLE", "biblio", False, {}), ("SQL", "SELECT * FROM biblio", False, {}), ("SQL", "SELECT * FROM biblio", True, {}),
             ("QUERY", "t12q", False, {}), ("TABLE", "biblio", False, {"StripData": True}), ("TABLE", "biblio", False, {"FromSelection": True})]
    for i, (t, obj, native, extra) in enumerate(cases):
        def one(i=i, t=t, obj=obj, native=native, extra=extra):
            doc.DatabaseRanges.addNewByName("imp%d" % i, rng_addr(i, 0, 0, 2, 3))
            doc.Sheets.getByIndex(i).getCellRangeByPosition(0, 0, 2, 3).doImport(
                (pv("DatabaseName", "Bibliography"), pv("SourceType", uno.Enum(DIM, t)), pv("SourceObject", obj), pv("IsNative", native)))
            db = doc.DatabaseRanges.getByName("imp%d" % i)
            for k, v in extra.items():
                db.setPropertyValue(k, v)
        step("import-%s-%d" % (t, i), one)
    save(doc, "dbranges-import.ods")
    doc.close(True)


step("null_year", null_year)
step("db_imports", db_imports)
step("links", links)
step("scenarios", scenarios)
step("consolidation", consolidation)
step("detective", detective)
step("shapes", shapes_doc)
step("calc_settings", calc_settings)


CATEGORIES = {1: "database", 2: "datetime", 3: "financial", 4: "information", 5: "logical", 6: "math", 7: "matrix", 8: "statistical",
              9: "spreadsheet", 10: "text", 11: "addin"}


def functions():
    fdesc = SM.createInstance("com.sun.star.sheet.FunctionDescriptions")
    by_cat = {}
    for i in range(fdesc.getCount()):
        d = {p.Name: p.Value for p in fdesc.getByIndex(i)}
        by_cat.setdefault(d["Category"], []).append(d)
    print("function descriptions:", {CATEGORIES.get(k, k): len(v) for k, v in sorted(by_cat.items())})
    for cat, items in sorted(by_cat.items()):
        doc = new()
        sh = doc.Sheets.getByIndex(0)
        for r in range(1, 8):
            for c in range(4):
                sh.getCellByPosition(c, r + 20).Value = r + c
        failed_fns = []
        for k, d in enumerate(items):
            args = d.get("Arguments") or ()
            n = sum(1 for a in args if not a.IsOptional)
            if n == 0 and args:
                n = 1
            if cat == 10:
                vals = ['"abc"', "2", "1", '"b"']
            elif cat == 1:
                vals = ["$A$21:$D$27", "2", "$A$21:$A$22"]
            elif cat in (7, 9, 8):
                vals = ["$A$21:$B$24", "$C$21:$D$24", "2", "1"]
            else:
                vals = ["2", "1", "3", "4", "5", "6", "7"]
            text = "=%s(%s)" % (d["Name"], ";".join((vals + ["1"] * 8)[:n])) if n else "=%s()" % d["Name"]
            cell = sh.getCellByPosition((k // 100) * 3, k % 100)
            try:
                cell.Formula = text
            except Exception as e:
                failed_fns.append(d["Name"])
        print("functions", CATEGORIES.get(cat, cat), len(items), "unparsed:", failed_fns[:10])
        save(doc, "formulas-%02d-%s.ods" % (cat, CATEGORIES.get(cat, cat)))
        doc.close(True)


def formula_forms():
    doc = sheets_doc(3)
    sh = doc.Sheets.getByIndex(0)
    sh.Name = "Data sheet"
    doc.Sheets.getByIndex(1).Name = "it's"
    doc.Sheets.getByIndex(2).Name = "Sheet 3"
    for r in range(10):
        sh.getCellByPosition(0, r).Value = r
        sh.getCellByPosition(1, r).String = "t%d" % r
    forms = ["=1+2*3-4/5^2", "=-A1", "=A1%", "=+A2", "=A1&B1&\"x\"", "=A1=A2", "=A1<>A2", "=A1<=A2", "=A1>=A2", "=A1<A2", "=A1>A2",
             "=SUM(A1:A5 A3:A7)", "=SUM(A1:A3~A5:A7)", "=SUM($A$1:$A$3)", "=SUM(A:A)", "=SUM(1:3)", "=SUM($'Data sheet'.A1:A3)",
             "=SUM($'it''s'.A1:$'Sheet 3'.A3)", "=$'it''s'.A1", "=SUM(A1:A3,B1)" , "={1;2;3}", "={1;2|3;4}", "={\"a\";\"b\"}", "=SUM({1;2|3;4})",
             "=#N/A", "=ERROR.TYPE(#DIV/0!)", "=ISERROR(#VALUE!)", "=IF(A1;\"y\";\"n\")", "=TRUE()", "=FALSE()", "=TRUE", "=PI()", "=1E3+1.5E-3",
             "=0.5+.5", "=\"he said \"\"hi\"\"\"", "=(1+2)*(3+4)", "=SUM((A1:A3;A5:A7))", "=A1:A3*2", "=ROW(A1:A3)", "=SUM(IF(A1:A5>2;1;0))",
             "=INDIRECT(\"A1\")", "=OFFSET(A1;1;1)", "=HYPERLINK(\"http://example.org\";\"x\")", "=VLOOKUP(2;A1:B5;2;0)", "=MULTIPLE.OPERATIONS(A1;A2;A3)",
             "=TEXT(A1;\"0.00\")", "=DDE(\"soffice\";\"x\";\"y\")", "=A1+$Data sheet.A2" ]
    for i, f in enumerate(forms):
        def put(i=i, f=f):
            sh.getCellByPosition(3, i).Formula = f
        step("form-%d" % i, put)
    step("array-1", sh.getCellRangeByName("F1:G2").setArrayFormula, "=A1:B2*2")
    step("array-2", sh.getCellRangeByName("H1:H3").setArrayFormula, "=A1:A3+1")
    step("array-3", sh.getCellRangeByName("J1:K1").setArrayFormula, "=TRANSPOSE(A1:A2)")
    sh.getCellByPosition(14, 0).Formula = "=A5*2+A6"
    # setTableOperation on the Spreadsheet object itself fills the whole sheet and keeps the office busy for minutes
    step("tableop-col", sh.getCellRangeByName("O2:O5").setTableOperation, sh.getCellRangeByName("N1:O1").RangeAddress, E("TableOperationMode", "COLUMN"), cell_addr(0, 0, 4), cell_addr(0, 0, 5))
    step("tableop-row", sh.getCellRangeByName("Q2:R2").setTableOperation, sh.getCellRangeByName("P1:P2").RangeAddress, E("TableOperationMode", "ROW"), cell_addr(0, 0, 4), cell_addr(0, 0, 5))
    step("tableop-both", sh.getCellRangeByName("U2:W4").setTableOperation, sh.getCellRangeByName("T1:T1").RangeAddress, E("TableOperationMode", "BOTH"), cell_addr(0, 0, 4), cell_addr(0, 0, 5))
    save(doc, "formulas-forms.ods")
    doc.close(True)


def names_and_ranges():
    doc = sheets_doc(2)
    sh = doc.Sheets.getByIndex(0)
    fill_table(sh)
    nr = doc.NamedRanges
    NF = "com.sun.star.sheet.NamedRangeFlag."
    spec = [("rng_abs", "$Sheet1.$A$1:$C$5", 0), ("rng_rel", "A1:B2", 0), ("expr_const", "0.19", 0), ("expr_formula", "SUM($Sheet1.$C$2:$C$9)*2", 0),
            ("expr_rel", "$Sheet1.A2*2", 0), ("expr_text", "\"abc\"", 0), ("expr_func", "TODAY()+1", 0),
            ("print_ar", "$Sheet1.$A$1:$E$12", 2), ("crit", "$Sheet1.$A$1:$B$2", 1), ("colhdr", "$Sheet1.$A$1:$E$1", 4), ("rowhdr", "$Sheet1.$A$1:$A$12", 8),
            ("all_flags", "$Sheet1.$A$1:$E$12", 1 | 2 | 4 | 8), ("na\u00efve", "$Sheet1.$B$2", 0), ("nested", "rng_abs", 0), ("sheet2ref", "$Sheet2.$A$1", 0),
            ("array_c", "{1;2|3;4}", 0), ("err_ref", "#REF!", 0)]
    for n, c, fl in spec:
        step("name-" + n, nr.addNewByName, n, c, cell_addr(0, 0, 0), fl)
    for i, rng in enumerate(("A2:A12", "B2:B12")):
        step("collabel-%d" % i, doc.ColumnLabelRanges.addNew, rng_addr(0, i, 0, i, 0), rng_addr(0, i, 1, i, 11))
    step("rowlabel-1", doc.RowLabelRanges.addNew, rng_addr(0, 0, 1, 0, 11), rng_addr(0, 1, 1, 4, 11))
    step("rowlabel-2", doc.RowLabelRanges.addNew, rng_addr(1, 0, 0, 0, 3), rng_addr(1, 1, 0, 3, 3))
    step("print-areas", sh.setPrintAreas, (rng_addr(0, 0, 0, 4, 5), rng_addr(0, 0, 8, 4, 11)))
    step("title-rows", sh.setPrintTitleRows, True)
    step("title-rows-r", sh.setTitleRows, rng_addr(0, 0, 0, 0, 0))
    step("title-cols", sh.setPrintTitleColumns, True)
    step("title-cols-r", sh.setTitleColumns, rng_addr(0, 0, 0, 0, 0))
    save(doc, "named-ranges.ods")
    doc.close(True)


def conditional_formats():
    doc = sheets_doc(1)
    sh = doc.Sheets.getByIndex(0)
    fill_table(sh)
    styles = doc.StyleFamilies.getByName("CellStyles")
    for n in ("cf_a", "cf_b", "cf_c"):
        st = doc.createInstance("com.sun.star.style.CellStyle")
        styles.insertByName(n, st)
        st.CellBackColor = {"cf_a": 0xFF0000, "cf_b": 0x00FF00, "cf_c": 0x0000FF}[n]
    ops = ["EQUAL", "NOT_EQUAL", "GREATER", "GREATER_EQUAL", "LESS", "LESS_EQUAL", "BETWEEN", "NOT_BETWEEN", "FORMULA"]
    for i, op in enumerate(ops):
        def one(i=i, op=op):
            rg = sh.getCellRangeByName("%s2:%s12" % ("ABCDEFGHI"[i], "ABCDEFGHI"[i]))
            cf = rg.ConditionalFormat
            f1, f2 = ("5", "9") if op in ("BETWEEN", "NOT_BETWEEN") else (("$A$2>0", "") if op == "FORMULA" else ("5", ""))
            cf.addNew((pv("Operator", E("ConditionOperator", op)), pv("Formula1", f1), pv("Formula2", f2),
                       pv("SourcePosition", cell_addr(0, i, 1)), pv("StyleName", "cf_a")))
            cf.addNew((pv("Operator", E("ConditionOperator", "GREATER")), pv("Formula1", '"text"'), pv("Formula2", ""),
                       pv("SourcePosition", cell_addr(0, i, 1)), pv("StyleName", "cf_b")))
            cf.addNew((pv("Operator", E("ConditionOperator", "FORMULA")), pv("Formula1", "ISEVEN(A2)"), pv("Formula2", ""),
                       pv("SourcePosition", cell_addr(0, i, 1)), pv("StyleName", "cf_c")))
            rg.ConditionalFormat = cf
        step("cf-" + op, one)
    save(doc, "conditional-formats.ods")
    doc.close(True)


def table_attrs():
    doc = sheets_doc(4)
    sh = doc.Sheets.getByIndex(0)
    fill_table(sh)
    for r in range(8):
        sh.getCellByPosition(8, r).Value = r / 24.0 + 0.1
    nf = doc.NumberFormats
    loc = struct("com.sun.star.lang.Locale", "en", "US", "")
    tk = nf.queryKey("HH:MM:SS", loc, False)
    if tk == -1:
        tk = nf.addNew("HH:MM:SS", loc)
    for r in range(8):
        sh.getCellByPosition(8, r).NumberFormat = tk
    dk = nf.queryKey("[HH]:MM:SS", loc, False)
    if dk == -1:
        dk = nf.addNew("[HH]:MM:SS", loc)
    sh.getCellByPosition(9, 0).Value = 2.5
    sh.getCellByPosition(9, 0).NumberFormat = dk
    sh.getCellByPosition(9, 1).Formula = "=TIME(1;2;3)"
    sh.getCellByPosition(9, 1).NumberFormat = tk
    sh.getCellByPosition(9, 2).Formula = '=TIME(1;2;3)+DATE(2020;1;1)'
    ck = nf.queryKey('#,##0.00 [$EUR]', loc, False)
    if ck == -1:
        ck = nf.addNew('#,##0.00 [$EUR]', loc)
    sh.getCellByPosition(9, 3).Value = 12.5
    sh.getCellByPosition(9, 3).NumberFormat = ck
    sh.getCellByPosition(9, 5).Formula = "=TRUE()"
    sh.getCellByPosition(9, 6).Formula = '="s"'
    sh.getCellByPosition(9, 7).Formula = "=1/0"
    sh.getCellByPosition(9, 8).Formula = "=NA()"
    sh.getCellByPosition(9, 9).String = "multi\nline  two  spaces\ttab"
    sh.Columns.getByIndex(5).IsVisible = False
    sh.Columns.getByIndex(7).IsVisible = False
    sh.Rows.getByIndex(3).IsVisible = False
    sh.Rows.getByIndex(4).IsVisible = False
    sh.Rows.getByIndex(6).Height = 1500
    sh.Columns.getByIndex(2).Width = 4000
    sh.Columns.getByIndex(3).OptimalWidth = True
    sh.Rows.getByIndex(7).OptimalHeight = True
    sh.Columns.getByIndex(4).IsStartOfNewPage = True
    sh.Rows.getByIndex(9).IsStartOfNewPage = True
    step("group-rows", sh.group, rng_addr(0, 0, 14, 0, 18), uno.Enum("com.sun.star.table.TableOrientation", "ROWS"))
    step("group-rows2", sh.group, rng_addr(0, 0, 15, 0, 16), uno.Enum("com.sun.star.table.TableOrientation", "ROWS"))
    step("group-cols", sh.group, rng_addr(0, 11, 0, 13, 0), uno.Enum("com.sun.star.table.TableOrientation", "COLUMNS"))
    step("group-cols2", sh.group, rng_addr(0, 20, 0, 22, 0), uno.Enum("com.sun.star.table.TableOrientation", "COLUMNS"))
    step("hide-grp-rows", sh.hideDetail, rng_addr(0, 0, 15, 0, 16))
    step("hide-grp-cols", sh.hideDetail, rng_addr(0, 20, 0, 22, 0))
    m = sh.getCellRangeByName("L5:N7")
    for c in range(11, 14):
        for r in range(4, 7):
            cell = sh.getCellByPosition(c, r)
            if (c, r) == (11, 4):
                cell.String = "top left"
            elif (c, r) == (12, 4):
                cell.Value = 7
            elif (c, r) == (13, 4):
                cell.Formula = "=1+1"
            elif (c, r) == (11, 5):
                cell.String = "covered text"
                cell.NumberFormat = nf.getStandardFormat(1, loc)
            elif (c, r) == (12, 5):
                cell.Value = 43831
                cell.NumberFormat = nf.getStandardFormat(2, loc)
            elif (c, r) == (13, 5):
                cell.Value = 0.25
                cell.NumberFormat = tk
            elif (c, r) == (11, 6):
                cell.Formula = "=TRUE()"
            elif (c, r) == (12, 6):
                cell.Value = 5
                cell.NumberFormat = ck
            else:
                cell.Formula = '="f"&"x"'
    v = sh.getCellByPosition(12, 4).Validation
    v.Type = E("ValidationType", "WHOLE")
    v.setOperator(E("ConditionOperator", "GREATER"))
    v.setFormula1("1")
    sh.getCellByPosition(12, 4).Validation = v
    step("merge", m.merge, True)
    sh.getCellRangeByName("P1:Q1").merge(True)
    sh.getCellRangeByName("P3:P5").merge(True)
    sh.getCellRangeByName("S1:T2").setArrayFormula("=A2:B3+1")
    sh.getCellRangeByName("V1:V3").setArrayFormula("=A2:A4*2")
    sh.getCellRangeByName("A1:B2").CellProtection = struct("com.sun.star.util.CellProtection", True, True, True, True)
    sh.getCellRangeByName("C1:C2").CellProtection = struct("com.sun.star.util.CellProtection", False, False, False, False)
    doc.Sheets.getByIndex(1).protect("pw")
    doc.Sheets.getByIndex(2).protect("")
    doc.Sheets.getByIndex(3).setPrintAreas(())
    sh2 = doc.Sheets.getByIndex(1)
    sh2.getCellByPosition(0, 0).Value = 1
    sh2.setPrintAreas((rng_addr(1, 0, 0, 2, 2),))
    doc.Sheets.getByIndex(3).getCellByPosition(0, 0).Value = 1
    doc.Sheets.getByIndex(3).IsVisible = False
    sh.TabColor = 0x00FF00
    doc.Sheets.getByIndex(1).TabColor = 0xFF0000
    save(doc, "table-attrs.ods")
    doc.close(True)


def filtered_rows():
    doc = sheets_doc(1)
    sh = doc.Sheets.getByIndex(0)
    fill_table(sh, 20)
    rg = sh.getCellRangeByName("A1:E20")
    doc.DatabaseRanges.addNewByName("flt", rng_addr(0, 0, 0, 4, 19))
    fd = rg.createFilterDescriptor(True)
    fd.setFilterFields2((filter_field(0, "EQUAL", "North", False, "AND"),))
    rg.filter(fd)
    sh.Rows.getByIndex(18).IsVisible = False
    save(doc, "filtered-rows.ods")
    doc.close(True)


step("functions", functions)
step("formula_forms", formula_forms)
step("names", names_and_ranges)
step("condfmt", conditional_formats)
step("table_attrs", table_attrs)
step("filtered", filtered_rows)


step("validations", validations)
step("validation_edges", validation_edges)

json.dump(expect, open(os.path.join(work, "expect.json"), "w"), indent=1)
sys.exit(1 if failed else 0)
