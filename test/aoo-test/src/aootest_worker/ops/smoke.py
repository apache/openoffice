################################################################
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
################################################################

"""The scenarios of test/smoketestdoc in the openoffice tree (new, save and reopen per app, chart, Math, Java loader,
database, extension), as UNO steps. Each op returns {"steps": [{"name", "ok", "detail"}]} so the host can say which step failed."""
import os

import uno
import unohelper
from com.sun.star.lang import XServiceInfo
from com.sun.star.task import XInteractionHandler
from com.sun.star.ucb import XCommandEnvironment
from com.sun.star.util import XCloseListener

from aootest_worker.ops import core
from aootest_worker.ops.core import pv


class Steps:
    def __init__(self):
        self.steps = []

    def check(self, name, fn):
        """Runs fn; the step passes when it returns anything but False (or a (False, detail) pair) and raises nothing. Returns fn's value, or None if it failed."""
        try:
            value = fn()
        except Exception as e:
            self.steps.append({"name": name, "ok": False, "detail": "%s: %s" % (type(e).__name__, e)})
            return None
        detail = ""
        if isinstance(value, tuple) and len(value) == 2 and isinstance(value[0], bool):
            value, detail = value
        ok = value is not False
        self.steps.append({"name": name, "ok": ok, "detail": detail if detail or ok else "returned False"})
        return value if ok else None

    def result(self):
        return {"steps": self.steps}


class CloseListener(unohelper.Base, XCloseListener):
    def __init__(self):
        self.closed = False

    def queryClosing(self, source, gets_ownership):
        pass

    def notifyClosing(self, source):
        self.closed = True

    def disposing(self, source):
        pass


def _url(path):
    return uno.systemPathToFileUrl(os.path.abspath(path))


def _new(factory):
    doc = core._desktop().loadComponentFromURL(factory, "_blank", 0, (pv("Hidden", True),))
    if doc is None:
        raise RuntimeError("could not create %s" % factory)
    return doc


def _close_with_listener(doc):
    listener = CloseListener()
    doc.addCloseListener(listener)
    doc.close(True)
    return listener.closed


def _filter_exists(name):
    return core._create("com.sun.star.document.FilterFactory").hasByName(name)


def doc_cycle(factory, formats, workdir, base="smoketest_file"):
    """New document from `factory`, stored once per (label, filter, extension) in `formats`, closed (the close listener must
    be called), then every stored file loaded again and closed."""
    st = Steps()
    doc = st.check("new", lambda: _new(factory))
    if doc is None:
        return st.result()
    paths = {}
    for label, flt, ext in formats:
        path = os.path.join(workdir, "%s.%s" % (base, ext))
        paths[label] = path
        st.check("filter %s exists" % label, lambda: _filter_exists(flt))
        st.check("save %s" % label, lambda: doc.storeAsURL(_url(path), (pv("FilterName", flt),)) or True)
    st.check("close", lambda: _close_with_listener(doc))
    for label, flt, ext in formats:
        def reopen():
            d = core._desktop().loadComponentFromURL(_url(paths[label]), "_blank", 0, (pv("Hidden", True), pv("MacroExecutionMode", 0)))
            if d is None:
                return False
            d.close(True)
        st.check("open %s" % label, reopen)
    return st.result()


def chart():
    """A chart object added to the first sheet of a new spreadsheet."""
    st = Steps()
    doc = st.check("new", lambda: _new("private:factory/scalc"))
    if doc is None:
        return st.result()

    def add():
        charts = doc.Sheets.getByIndex(0).Charts
        charts.addNewByName("TestChart", uno.createUnoStruct("com.sun.star.awt.Rectangle"),
                            (uno.createUnoStruct("com.sun.star.table.CellRangeAddress"),), True, True)
        return charts.hasByName("TestChart")
    st.check("add chart", add)
    st.check("close", lambda: _close_with_listener(doc))
    return st.result()


def math():
    st = Steps()
    doc = st.check("new", lambda: _new("private:factory/smath"))
    if doc is not None:
        st.check("close", lambda: _close_with_listener(doc))
    return st.result()


def java_loader():
    """The Java loader service can be created. It does not start a JVM, so this holds without a JRE."""
    st = Steps()
    st.check("create com.sun.star.loader.Java", lambda: core._create("com.sun.star.loader.Java") is not None)
    return st.result()


def _scan(conn, table, text):
    """(number of rows, number of rows whose 5th column is `text`) by reading the whole table through a fresh RowSet."""
    rs = core._create("com.sun.star.sdb.RowSet")
    rs.ActiveConnection = conn
    rs.CommandType = uno.getConstantByName("com.sun.star.sdb.CommandType.COMMAND")
    rs.Command = "SELECT * FROM " + table
    rs.execute()
    rows = found = 0
    while rs.next():
        rows += 1
        found += rs.getString(5) == text
    rs.dispose()
    return rows, found


def database(text="Automatical Test"):
    """Open the first registered data source, insert a row through a RowSet, read it back, delete it, read another row.
    Row counts come from a full scan: RowCount of this driver counts what has been fetched so far."""
    st = Steps()
    ctx = st.check("create DatabaseContext", lambda: core._create("com.sun.star.sdb.DatabaseContext"))
    if ctx is None:
        return st.result()
    names = ctx.getElementNames()
    if not st.check("a data source is registered", lambda: len(names) > 0):
        return st.result()
    source = ctx.getByName(names[0])
    table = st.check("a table exists", lambda: source.Tables.getElementNames()[0])
    conn = st.check("open connection", lambda: source.getConnection("", ""))
    if table is None or conn is None:
        return st.result()
    try:
        rs = st.check("create RowSet", lambda: core._create("com.sun.star.sdb.RowSet"))
        if rs is None:
            return st.result()
        rs.ActiveConnection = conn
        rs.CommandType = uno.getConstantByName("com.sun.star.sdb.CommandType.COMMAND")
        rs.Command = "SELECT * FROM " + table
        if st.check("run SELECT * FROM " + table, lambda: rs.execute() or True) is None:
            return st.result()
        before = _scan(conn, table, text)
        st.check("table has no leftover test row", lambda: (before[1] == 0, "rows %d, leftovers %d" % before))

        def insert():
            rs.moveToInsertRow()
            rs.updateString(5, text)
            rs.insertRow()
            rs.moveToCurrentRow()
            after = _scan(conn, table, text)
            return (rs.getString(5) == text and after == (before[0] + 1, before[1] + 1)), "rows %d, matches %d" % after
        inserted = st.check("insert row", insert)
        if inserted:
            def delete():
                rs.deleteRow()
                after = _scan(conn, table, text)
                return after == before, "rows %d, matches %d (expected %d, %d)" % (after + before)
            st.check("delete row", delete)

        def seek():
            rs.first()
            return rs.getString(5) != text
        st.check("read another row", seek)
    finally:
        conn.dispose()
    return st.result()


class ApprovingHandler(unohelper.Base, XInteractionHandler):
    def handle(self, request):
        approve = uno.getTypeByName("com.sun.star.task.XInteractionApprove")
        for c in request.getContinuations():
            a = c.queryInterface(approve)
            if a is not None:
                a.select()
                return


class CommandEnvironment(unohelper.Base, XCommandEnvironment):
    def getInteractionHandler(self):
        return ApprovingHandler()

    def getProgressHandler(self):
        return None


def extension(path, identifier, service, implementation):
    """Add the extension at `path` for the user, check that its service exists, remove it, check that the service is gone."""
    st = Steps()
    ctx = core._ctx()
    mgr = st.check("get ExtensionManager", lambda: ctx.getValueByName("/singletons/com.sun.star.deployment.ExtensionManager"))
    if mgr is None:
        return st.result()
    env = CommandEnvironment()
    name = os.path.basename(path)
    if st.check("add extension", lambda: mgr.addExtension(_url(path), (), "user", None, env) is not None) is None:
        return st.result()
    obj = st.check("service created", lambda: core._create(service))
    st.check("implementation name", lambda: obj is not None and obj.getImplementationName() == implementation)
    st.check("remove extension", lambda: mgr.removeExtension(identifier, name, "user", None, env) or True)
    st.check("service gone", lambda: core._create(service) is None)
    return st.result()


OPS = {name: core.guarded(fn) for name, fn in {"doc_cycle": doc_cycle, "chart": chart, "math": math, "java_loader": java_loader,
                                               "database": database, "extension": extension}.items()}
