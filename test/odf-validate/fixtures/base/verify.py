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
# usage: runpy.sh fixtures/base/verify.py <port> <outdir> [<name>.odb ...]
# Reloads base-*.odb files of <outdir> read-only and prints what the data source holds.
import glob, os, sys, uno
from unohelp import connect, desktop, pv

port, out = int(sys.argv[1]), os.path.abspath(sys.argv[2])
d = desktop(connect(port))
NO_CONNECT = ("mysql", "oracle", "odbc", "ldap", "adabas", "settings")


def names(c):
    return list(c.ElementNames) if c is not None else []


def walk(c, prefix=""):
    for n in names(c):
        e = c.getByName(n)
        if hasattr(e, "ElementNames"):
            yield from walk(e, prefix + n + "/")
        else:
            yield prefix + n


paths = [os.path.join(out, n) for n in sys.argv[3:]] or sorted(glob.glob(os.path.join(out, "base-*.odb")))
for path in paths:
    base = os.path.basename(path)
    doc = d.loadComponentFromURL(uno.systemPathToFileUrl(path), "_blank", 0,
                                 (pv("Hidden", True), pv("MacroExecutionMode", 0), pv("ReadOnly", True)))
    ds = doc.DataSource
    row = {"url": ds.URL.split("/work/")[-1] if "file:" in ds.URL else ds.URL,
           "user": ds.User, "pwreq": ds.IsPasswordRequired,
           "queries": names(ds.QueryDefinitions), "tabledefs": names(ds.Tables),
           "forms": list(walk(doc.FormDocuments)), "reports": list(walk(doc.ReportDocuments)),
           "filter": list(ds.TableFilter), "typefilter": list(ds.TableTypeFilter)}
    q = ds.QueryDefinitions
    if "Q_Cols" in names(q):
        row["Q_Cols.columns"] = names(q.getByName("Q_Cols").Columns)
    if not any(base.startswith("base-" + n) for n in NO_CONNECT):
        try:
            conn = ds.getConnection("", "")
            row["tables"] = names(conn.Tables)
            st = conn.createStatement()
            counts = {}
            for t in names(conn.Tables):
                rs = st.executeQuery('SELECT COUNT(*) FROM "%s"' % t)
                rs.next()
                counts[t] = rs.getInt(1)
            row["rows"] = counts
            conn.close()
        except Exception as e:
            row["connect"] = "FAIL " + str(e)[:150]
    for fdoc in ("Form1",):
        fd = doc.FormDocuments
        if fd.hasByName(fdoc):
            f = fd.loadComponentFromURL(fdoc, "", 0, (pv("OpenMode", "open"),))
            forms = f.DrawPage.Forms
            row["Form1.controls"] = sum(len(names(forms.getByName(n))) for n in names(forms))
            row["Form1.forms"] = names(forms)
            cmd = uno.createUnoStruct("com.sun.star.ucb.Command")
            cmd.Name, cmd.Handle = "close", -1
            fd.getByName(fdoc).execute(cmd, 0, None)
    row["settings"] = sorted(p.Name for p in doc.DataSource.Info if p.Name.startswith("My"))
    print(base, row)
    doc.close(True)
