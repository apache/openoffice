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
# usage: runpy.sh fixtures/base/make.py <port> <outdir> [<workdir>]
# Writes base-<variant>.odb data source documents into <outdir>; dbf, csv and hsqldb files go to <workdir>.
import os, shutil, sys, traceback, uno
from unohelp import connect, pv
from com.sun.star.beans import NamedValue
from com.sun.star.awt import Size, Point
from com.sun.star.lang import Locale

port, out = int(sys.argv[1]), os.path.abspath(sys.argv[2])
work = os.path.abspath(sys.argv[3]) if len(sys.argv) > 3 else os.path.join(os.path.dirname(out), "work")
ctx = connect(port)
sm = ctx.ServiceManager
dbc = sm.createInstanceWithContext("com.sun.star.sdb.DatabaseContext", ctx)
os.makedirs(out, exist_ok=True)
os.makedirs(work, exist_ok=True)
FURL = uno.systemPathToFileUrl
ANCHOR = uno.Enum("com.sun.star.text.TextContentAnchorType", "AT_PARAGRAPH")


def nv(n, v):
    return NamedValue(n, v)


def set_info(ds, **kw):
    # Info= with several settings crashes the office in comphelper::OPropertyBag; set them one by one
    bag = ds.Settings
    for k, v in kw.items():
        if bag.getPropertySetInfo().hasPropertyByName(k):
            uno.invoke(bag, "setPropertyValue", (k, v))
        else:
            uno.invoke(bag, "addProperty", (k, 128 | 1, v))


def new_source(url, **props):
    ds = dbc.createInstance()
    ds.URL = url
    for k, v in props.items():
        setattr(ds, k, v)
    return ds


def sql(conn, stmts):
    st = conn.createStatement()
    for s in stmts:
        st.executeUpdate(s)
    st.close()


def hsql_connect(ds):
    # the first statement on a fresh embedded database fails on a null context class loader; a retry works
    conn = ds.getConnection("", "")
    st = conn.createStatement()
    for _ in range(3):
        try:
            st.executeUpdate('CREATE MEMORY TABLE "PROBE" ("ID" INTEGER)')
            st.executeUpdate('DROP TABLE "PROBE"')
            break
        except Exception:
            pass
    st.close()
    return conn


def numkey(ds, code):
    nf = ds.NumberFormatsSupplier.NumberFormats
    loc = Locale("en", "US", "")
    k = nf.queryKey(code, loc, False)
    return k if k != -1 else nf.addNew(code, loc)


def make_query(ds, name, command, **props):
    q = ds.QueryDefinitions.createInstance()
    q.Command = command
    for k, v in props.items():
        setattr(q, k, v)
    ds.QueryDefinitions.insertByName(name, q)
    return ds.QueryDefinitions.getByName(name)


def add_columns(owner, specs):
    cols = owner.Columns
    for name, props in specs:
        d = cols.createDataDescriptor()
        d.Name = name
        for k, v in props.items():
            d.setPropertyValue(k, v)
        cols.appendByDescriptor(d)


def make_table_def(ds, name, columns=(), **props):
    tables = ds.Tables
    if tables.hasByName(name):
        t = tables.getByName(name)
        for k, v in props.items():
            t.setPropertyValue(k, v)
    else:
        t = tables.createInstance()
        for k, v in props.items():
            t.setPropertyValue(k, v)
        tables.insertByName(name, t)
        t = tables.getByName(name)
    add_columns(t, columns)
    return t


def new_doc_def(container, name, service, parent=None):
    args = (nv("Name", name), nv("DocumentServiceName", service))
    d = container.createInstanceWithArguments("com.sun.star.sdb.DocumentDefinition", args)
    (parent or container).insertByName(name, d)
    return d


def open_def(container, name, conn):
    return container.loadComponentFromURL(name, "", 0, (pv("OpenMode", "openDesign"), pv("ActiveConnection", conn)))


def def_command(container, name, command):
    cmd = uno.createUnoStruct("com.sun.star.ucb.Command")
    cmd.Name = command
    cmd.Handle = -1
    container.getByName(name).execute(cmd, 0, None)


def store_def(container, name):
    def_command(container, name, "store")
    def_command(container, name, "close")


def add_control(doc, form, service, name, x, y, w=4000, h=700, **props):
    m = doc.createInstance("com.sun.star.form.component." + service)
    for k, v in props.items():
        setattr(m, k, v)
    form.insertByName(name, m)
    shape = doc.createInstance("com.sun.star.drawing.ControlShape")
    shape.AnchorType = ANCHOR
    shape.Size = Size(w, h)
    shape.Position = Point(x, y)
    shape.Control = m
    doc.DrawPage.add(shape)
    return m


CUSTOMER_CONTROLS = (
    ("FixedText", "Title", dict(Label="Customers")),
    ("TextField", "NameBox", dict(DataField="NAME", MaxTextLen=40)),
    ("NumericField", "IdBox", dict(DataField="ID", DecimalAccuracy=0, ValueMin=0.0, ValueMax=9999.0)),
    ("CheckBox", "ActiveBox", dict(DataField="ACTIVE", Label="Active", TriState=True)),
    ("DateField", "BornBox", dict(DataField="BORN", DateFormat=4)),
    ("CurrencyField", "BalBox", dict(DataField="BALANCE", CurrencySymbol="$", PrependCurrencySymbol=True)),
    ("ListBox", "CityList", dict(DataField="CITY", ListSourceType=2, ListSource=('SELECT DISTINCT "CITY" FROM "CUSTOMERS"',),
                                 Dropdown=True)),
    ("ComboBox", "CityCombo", dict(DataField="CITY", ListSourceType=0, StringItemList=("Berlin", "Paris"))),
    ("CommandButton", "Ok", dict(Label="Save", ButtonType=2)),
)


def fill_form(doc, command, controls=CUSTOMER_CONTROLS, sub=True):
    forms = doc.DrawPage.Forms
    main = doc.createInstance("com.sun.star.form.component.DataForm")
    main.Command = command
    main.CommandType = 0
    main.Filter = '"ID" > 0'
    main.ApplyFilter = True
    main.Order = '"NAME"'
    forms.insertByName("MainForm", main)
    y = 300
    for service, name, props in controls:
        add_control(doc, main, service, name, 500, y, **props)
        y += 900
    if sub:
        child = doc.createInstance("com.sun.star.form.component.DataForm")
        child.Command = command
        child.CommandType = 0
        child.MasterFields = ("ID",)
        child.DetailFields = ("ID",)
        main.insertByName("SubForm", child)
        add_control(doc, child, "GridControl", "Grid", 500, y, w=6000, h=2500)
        add_control(doc, child, "RadioButton", "R1", 7000, 300, Label="One", RefValue="1", DataField="ID")
        add_control(doc, child, "RadioButton", "R2", 7000, 1200, Label="Two", RefValue="2", DataField="ID")


def populate(ds, conn, command="CUSTOMERS", folder=True):
    doc = ds.DatabaseDocument
    fd = doc.FormDocuments
    new_doc_def(fd, "Form1", "com.sun.star.text.TextDocument")
    f = open_def(fd, "Form1", conn)
    fill_form(f, command)
    store_def(fd, "Form1")
    if folder:
        grp = fd.createInstanceWithArguments("com.sun.star.sdb.Forms", (pv("Name", "Folder"),))
        fd.insertByName("Folder", grp)
        new_doc_def(fd, "Form2", "com.sun.star.text.TextDocument", parent=grp)
        f = open_def(grp, "Form2", conn)
        fill_form(f, command, controls=CUSTOMER_CONTROLS[:3], sub=False)
        store_def(grp, "Form2")
    rd = doc.ReportDocuments
    new_doc_def(rd, "Report1", "com.sun.star.text.TextDocument")
    r = open_def(rd, "Report1", conn)
    fill_form(r, command, controls=CUSTOMER_CONTROLS[:2], sub=False)
    store_def(rd, "Report1")


def new_hsqldb(name, **props):
    path = os.path.join(out, name)
    for p in (path, path + ".lck"):
        if os.path.exists(p):
            os.remove(p)
    ds = new_source("sdbc:embedded:hsqldb", **props)
    ds.DatabaseDocument.storeAsURL(FURL(path), ())
    return ds


def new_dir(name):
    d = os.path.join(work, name)
    shutil.rmtree(d, ignore_errors=True)
    os.makedirs(d)
    return d


def finish(ds, name, conn=None):
    doc = ds.DatabaseDocument
    doc.store()
    if conn is not None:
        conn.close()
    doc.close(True)
    lck = os.path.join(out, name + ".lck")
    if os.path.exists(lck):
        os.remove(lck)


def save_new(ds, name):
    path = os.path.join(out, name)
    if os.path.exists(path):
        os.remove(path)
    ds.DatabaseDocument.storeAsURL(FURL(path), ())


def variant_hsqldb():
    ds = new_hsqldb("base-hsqldb.odb", TableFilter=("%",), TableTypeFilter=("TABLE", "VIEW"), User="", IsPasswordRequired=False)
    conn = hsql_connect(ds)
    sql(conn, ['CREATE MEMORY TABLE "CUSTOMERS" ("ID" INTEGER NOT NULL PRIMARY KEY, "NAME" VARCHAR(50), "CITY" VARCHAR(30), '
               '"ACTIVE" BOOLEAN, "BORN" DATE, "BALANCE" DECIMAL(10,2))',
               'CREATE MEMORY TABLE "ORDERS" ("OID" INTEGER NOT NULL PRIMARY KEY, "CID" INTEGER, "TOTAL" DOUBLE, '
               'FOREIGN KEY ("CID") REFERENCES "CUSTOMERS"("ID"))',
               'CREATE VIEW "BIGORDERS" AS SELECT * FROM "ORDERS" WHERE "TOTAL" > 100'])
    set_info(ds, IsAutoRetrievingEnabled=True, AutoRetrievingStatement="CALL IDENTITY()",
                   AutoIncrementCreation="GENERATED BY DEFAULT AS IDENTITY", BooleanComparisonMode=1,
                   SuppressVersionColumns=False)
    make_query(ds, "Q_ByCity", 'SELECT "ID", "NAME", "CITY" FROM "CUSTOMERS"',
               Filter='"CITY" = \'Berlin\'', ApplyFilter=True, Order='"NAME"',
               UpdateTableName="CUSTOMERS", UpdateSchemaName="PUBLIC", UpdateCatalogName="")
    make_query(ds, "Q_Native", 'SELECT * FROM "ORDERS"', EscapeProcessing=False)
    q = make_query(ds, "Q_Cols", 'SELECT "OID", "TOTAL" FROM "ORDERS"', RowHeight=800, TextColor=0xFF0000, FontName="Arial")
    add_columns(q, [("OID", dict(Hidden=True, HelpText="order id")),
                    ("TOTAL", dict(HelpText="the total", ControlDefault="1.5", Width=4000, Align=2,
                                   FormatKey=numkey(ds, "#,##0.00")))])
    make_table_def(ds, "CUSTOMERS", [("NAME", dict(Width=5000, HelpText="customer name", Align=1)),
                                      ("BALANCE", dict(Hidden=True, ControlDefault="0"))],
                   Filter='"ACTIVE" = TRUE', ApplyFilter=True, Order='"NAME" DESC',
                   SchemaName="PUBLIC", CatalogName="CAT", RowHeight=700, TextColor=0x0000FF, FontName="Courier")
    make_table_def(ds, "ORDERS")
    populate(ds, conn)
    finish(ds, "base-hsqldb.odb", conn)


def make_dbf(conn):
    tbls = conn.Tables
    t = tbls.createDataDescriptor()
    t.Name = "CUSTOMERS"
    cols = t.Columns
    for name, typ, prec, scale in (("ID", 4, 9, 0), ("NAME", 12, 40, 0), ("CITY", 12, 30, 0), ("ACTIVE", -7, 0, 0),
                                   ("BORN", 91, 0, 0), ("BALANCE", 3, 10, 2), ("NOTES", -1, 0, 0)):
        c = cols.createDataDescriptor()
        c.Name, c.Type, c.Precision, c.Scale = name, typ, prec, scale
        cols.appendByDescriptor(c)
    tbls.appendByDescriptor(t)
    ps = conn.prepareStatement('INSERT INTO "CUSTOMERS" VALUES (?,?,?,?,?,?,?)')
    for i, name, city, active, born, bal, note in ((1, "Ada", "Berlin", True, (31, 1, 1990), 12.5, "first"),
                                                   (2, "Bob", "Paris", False, (4, 7, 1985), 0.0, "second")):
        ps.setInt(1, i)
        ps.setString(2, name)
        ps.setString(3, city)
        ps.setBoolean(4, active)
        ps.setDate(5, uno.createUnoStruct("com.sun.star.util.Date", *born))
        ps.setDouble(6, bal)
        ps.setString(7, note)
        ps.executeUpdate()
    ps.close()


def variant_dbase():
    d = new_dir("dbase")
    ds = new_source("sdbc:dbase:" + FURL(d) + "/", TableFilter=("CUSTOMERS",), TableTypeFilter=("TABLE",))
    set_info(ds, Extension="dbf", ShowDeleted=True, CharSet="utf-8")
    save_new(ds, "base-dbase.odb")
    conn = ds.getConnection("", "")
    make_dbf(conn)
    make_query(ds, "Q_Active", 'SELECT * FROM "CUSTOMERS"', Filter='"ACTIVE" = TRUE', ApplyFilter=True, Order='"NAME"',
               UpdateTableName="CUSTOMERS")
    make_table_def(ds, "CUSTOMERS", [("NAME", dict(Width=4000))], Order='"NAME"')
    populate(ds, conn)
    finish(ds, "base-dbase.odb", conn)


def variant_flat(name, ext, header, field, text, dec, thousand, charset):
    d = new_dir(name)
    sep = field
    rows = [["ID", "NAME", "CITY", "BALANCE"], ["1", "Ada", "Berlin", "1,250.50"], ["2", "Bob", "Paris", "0.00"]]
    with open(os.path.join(d, "CUSTOMERS." + ext), "w", encoding="utf-8") as fh:
        for i, r in enumerate(rows):
            if i == 0 and not header:
                continue
            fh.write(sep.join(text + c + text if c in ("Ada", "Bob", "Berlin", "Paris", "NAME", "CITY") else c for c in r) + "\n")
    ds = new_source("sdbc:flat:" + FURL(d) + "/", TableFilter=("CUSTOMERS",))
    set_info(ds, Extension=ext, CharSet=charset, FieldDelimiter=field, StringDelimiter=text, DecimalDelimiter=dec,
                   ThousandDelimiter=thousand, HeaderLine=header)
    save_new(ds, "base-%s.odb" % name)
    make_query(ds, "Q_All", 'SELECT * FROM "CUSTOMERS"')
    finish(ds, "base-%s.odb" % name)


def variant_flat_csv():
    variant_flat("flat", "csv", True, ",", '"', ".", ",", "UTF-8")


def variant_flat_txt():
    variant_flat("flat-txt", "txt", False, ";", "'", ",", ".", "ISO-8859-1")


def variant_mysql():
    ds = new_source("sdbc:mysql:jdbc:db.example.invalid:3306/shop", User="shopuser", IsPasswordRequired=True,
                    TableFilter=("shop.%", "!shop.tmp%"), TableTypeFilter=("TABLE", "VIEW", "SYSTEM TABLE"))
    set_info(ds, JavaDriverClass="org.example.jdbc.Driver", LocalSocket="/var/run/mysqld/mysqld.sock",
                   SuppressVersionColumns=True, IsAutoRetrievingEnabled=True,
                   AutoRetrievingStatement="SELECT LAST_INSERT_ID()", AutoIncrementCreation="AUTO_INCREMENT")
    set_info(ds, ServerTimeout=uno.Any("long", 30))
    save_new(ds, "base-mysql.odb")
    make_query(ds, "Q_Orders", "SELECT * FROM `orders` ORDER BY `id`", UpdateTableName="orders", UpdateCatalogName="shop")
    finish(ds, "base-mysql.odb")


def variant_settings():
    ds = new_source("sdbc:odbc:SettingsDSN")
    set_info(ds, MyStringSetting="text", MyBoolSetting=uno.Any("boolean", True), MyIntSetting=uno.Any("long", 42),
                 MyDoubleSetting=2.5, MyListSetting=("a", "b", "c"))
    save_new(ds, "base-settings.odb")
    finish(ds, "base-settings.odb")


def variant_oracle():
    ds = new_source("jdbc:oracle:thin:@db.example.invalid:1521:XE", User="scott", IsPasswordRequired=True)
    save_new(ds, "base-oracle.odb")
    finish(ds, "base-oracle.odb")


def variant_odbc():
    ds = new_source("sdbc:odbc:ShopDSN", User="odbcuser", IsPasswordRequired=False, TableFilter=("%",))
    set_info(ds, SystemDriverSettings="DSN=ShopDSN;Timeout=5", UseCatalog=True, EnableSQL92Check=True, AppendTableAliasName=True,
                   IgnoreDriverPrivileges=False, ParameterNameSubstitution=True, NoNameLengthLimit=True,
                   BooleanComparisonMode=3)
    save_new(ds, "base-odbc.odb")
    make_query(ds, "Q_Odbc", "SELECT 1 FROM x")
    finish(ds, "base-odbc.odb")


def variant_ldap():
    ds = new_source("sdbc:address:ldap:ldap.example.invalid:389")
    set_info(ds, BaseDN="dc=example,dc=invalid", MaxRowCount=uno.Any("long", 25))
    save_new(ds, "base-ldap.odb")
    finish(ds, "base-ldap.odb")


def variant_adabas():
    ds = new_source("sdbc:adabas:dbhost.example.invalid:MYDB", User="control", IsPasswordRequired=True)
    save_new(ds, "base-adabas.odb")
    finish(ds, "base-adabas.odb")


VARIANTS = [("hsqldb", variant_hsqldb), ("dbase", variant_dbase), ("flat", variant_flat_csv), ("flat-txt", variant_flat_txt),
            ("mysql", variant_mysql), ("settings", variant_settings), ("oracle", variant_oracle), ("odbc", variant_odbc), ("ldap", variant_ldap),
            ("adabas", variant_adabas)]

failed = []
for name, fn in VARIANTS:
    if len(sys.argv) > 4 and name not in sys.argv[4:]:
        continue
    try:
        fn()
        print("OK", name)
    except Exception:
        traceback.print_exc()
        print("FAIL", name)
        failed.append(name)
sys.exit(1 if failed else 0)
