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
#
# usage: runpy.sh fixtures/writer/make.py <port> <outdir>
# Writer constructs the corpus lacks (fields, variables, references, indexes, forms, settings, tracked changes, ...). Saves with the office's configured ODF version: run it against the strict and extended profiles.
import sys, os, traceback, uno
from unohelp import connect, desktop, pv

port, out = int(sys.argv[1]), os.path.abspath(sys.argv[2])
os.makedirs(out, exist_ok=True)
desk_ctx = connect(port)
desk = desktop(desk_ctx)
failed = []
ODT = "writer8"


def new(factory="private:factory/swriter"):
    return desk.loadComponentFromURL(factory, "_blank", 0, (pv("Hidden", True),))


SUF = ""


def save(doc, name, filt=ODT):
    base, ext = os.path.splitext(name)
    name = base + SUF + ext
    try:
        doc.storeToURL(uno.systemPathToFileUrl(os.path.join(out, name)), (pv("FilterName", filt),))
        print("saved", name, filt)
    except Exception as e:
        failed.append(name)
        print("FAIL", name, filt, type(e).__name__, str(e)[:200], repr(e)[:200])


def attempt(label, fn, *a, **kw):
    try:
        return fn(*a, **kw)
    except Exception as e:
        failed.append(label)
        print("FAIL", label, type(e).__name__, str(e)[:100], repr(e)[:200], "line", traceback.extract_tb(e.__traceback__)[-1].lineno)


class W:
    def __init__(self, doc):
        self.doc = doc
        self.text = doc.Text
        self.cur = self.text.createTextCursor()
        self.cur.gotoEnd(False)

    def s(self, txt):
        self.text.insertString(self.cur, txt, False)

    def para(self, txt=""):
        self.text.insertControlCharacter(self.cur, 0, False)
        if txt:
            self.s(txt)

    def mk(self, service, **props):
        o = self.doc.createInstance(service)
        for k, v in props.items():
            setattr(o, k, v)
        return o

    def put(self, content, absorb=False):
        self.text.insertTextContent(self.cur, content, absorb)

    def field(self, kind, label=None, **props):
        lab = label or kind
        try:
            f = self.mk("com.sun.star.text.TextField." + kind, **props)
            self.put(f)
            self.s(" ")
            return f
        except Exception as e:
            failed.append("field " + lab)
            print("FAIL field", lab, type(e).__name__, str(e)[:100], repr(e)[:160])


def dt(y=2024, mo=3, d=15, h=10, mi=30, s=0):
    return uno.createUnoStruct("com.sun.star.util.DateTime", 0, s, mi, h, d, mo, y)


def date(y=2024, mo=3, d=15):
    return uno.createUnoStruct("com.sun.star.util.Date", d, mo, y)


def numfmt(doc, fmt, lang="en", country="US"):
    nf = doc.NumberFormats
    loc = uno.createUnoStruct("com.sun.star.lang.Locale", lang, country, "")
    k = nf.queryKey(fmt, loc, False)
    if k == -1:
        k = nf.addNew(fmt, loc)
    return k


def doc_fields_info():
    doc = new()
    w = W(doc)
    doc.DocumentProperties.Title = "Field title"
    doc.DocumentProperties.Subject = "Field subject"
    doc.DocumentProperties.Description = "Field description"
    doc.DocumentProperties.Keywords = ("one", "two")
    doc.DocumentProperties.Author = "Field Author"
    doc.DocumentProperties.PrintedBy = "Printer Person"
    doc.DocumentProperties.PrintDate = dt(2024, 4, 1)
    doc.DocumentProperties.ModifiedBy = "Modifier"
    doc.DocumentProperties.ModificationDate = dt(2024, 5, 2)
    doc.DocumentProperties.EditingCycles = 7
    doc.DocumentProperties.EditingDuration = 3661
    udp = doc.DocumentProperties.UserDefinedProperties
    doc.DocumentProperties.DefaultTarget = "_self"
    doc.DocumentProperties.AutoloadURL = "http://example.org/reload"
    doc.DocumentProperties.AutoloadSecs = 90
    doc.DocumentProperties.TemplateName = "tpl"
    doc.DocumentProperties.TemplateURL = "file:///tmp/tpl.ott"
    doc.DocumentProperties.Language = uno.createUnoStruct("com.sun.star.lang.Locale", "en", "US", "")
    for nm, val in (("cstr", "text"), ("cnum", 42.5), ("cbool", True), ("cdate", dt(2024, 6, 7)), ("cdate2", date(2024, 6, 8)),
                    ("ctime", uno.createUnoStruct("com.sun.star.util.Time", 0, 3, 2, 1)),
                    ("cdur", uno.createUnoStruct("com.sun.star.util.Duration", False, 0, 1, 2, 3, 4, 5, 6))):
        try:
            udp.addProperty(nm, 128, val)
        except Exception as e:
            print("udp", nm, repr(e)[:100])
    fid = 0
    for sub in ("DocInfo.Title", "DocInfo.Subject", "DocInfo.Description", "DocInfo.KeyWords",
                "DocInfo.CreateAuthor", "DocInfo.ChangeAuthor", "DocInfo.PrintAuthor"):
        for fixed in (True, False):
            w.field(sub, sub + str(fixed), IsFixed=fixed)
    w.para()
    for sub in ("DocInfo.CreateDateTime", "DocInfo.ChangeDateTime", "DocInfo.PrintDateTime"):
        for isdate in (True, False):
            for fixed in (True, False):
                w.field(sub, "%s %s %s" % (sub, isdate, fixed), IsDate=isdate, IsFixed=fixed,
                        NumberFormat=numfmt(doc, "YYYY-MM-DD" if isdate else "HH:MM:SS"))
        w.para()
    w.field("DocInfo.EditTime", NumberFormat=numfmt(doc, "[HH]:MM:SS"))
    w.field("DocInfo.Revision", IsFixed=True)
    w.field("DocInfo.Revision", "rev2", IsFixed=False)
    w.para()
    for nm in ("cstr", "cnum", "cbool", "cdate", "cdate2", "ctime", "cdur"):
        for fixed in (True, False):
            w.field("DocInfo.Custom", "custom " + nm, Name=nm, IsFixed=fixed, NumberFormat=numfmt(doc, "0.00"))
    w.para()
    save(doc, "fields-docinfo.odt")
    doc.close(True)



def doc_fields_misc():
    doc = new()
    w = W(doc)
    doc.DocumentProperties.Author = "Misc Author"
    for fixed in (True, False):
        for full in (True, False):
            w.field("Author", "author %s %s" % (fixed, full), IsFixed=fixed, FullName=full, Content="A. Person")
    w.para()
    for part in range(15):
        for fixed in (True, False):
            w.field("ExtendedUser", "sender %d %s" % (part, fixed), UserDataType=part, IsFixed=fixed,
                    Content="sender%d" % part)
    w.para()
    for isdate in (True, False):
        for fixed in (True, False):
            for adj in (0, 5, -3):
                w.field("DateTime", "datetime %s %s %d" % (isdate, fixed, adj), IsDate=isdate, IsFixed=fixed, Adjust=adj,
                        NumberFormat=numfmt(doc, "DD.MM.YYYY" if isdate else "HH:MM"),
                        DateTimeValue=dt())
        w.para()
    for sub in (0, 1, 2):
        for ntype in (4, 0, 1, 2, 3, 6, 9, 10):
            w.field("PageNumber", "pagenum %d %d" % (sub, ntype), SubType=sub, NumberingType=ntype, Offset=2 if sub == 1 else 0,
                    UserText="p")
    w.para()
    for kind in ("PageCount", "ParagraphCount", "WordCount", "CharacterCount", "TableCount",
                 "GraphicObjectCount", "EmbeddedObjectCount"):
        for ntype in (4, 0, 1, 2, 3, 9, 10):
            w.field(kind, "%s %d" % (kind, ntype), NumberingType=ntype)
    w.para()
    for fmt in range(4):
        w.field("FileName", "filename %d" % fmt, FileFormat=fmt, IsFixed=(fmt % 2 == 0))
    for fmt in range(6):
        w.field("TemplateName", "template %d" % fmt, FileFormat=fmt)
    w.para()
    for fmt in range(5):
        for lvl in (0, 3):
            w.field("Chapter", "chapter %d %d" % (fmt, lvl), ChapterFormat=fmt, Level=lvl)
    w.para()
    for ptype in range(5):
        w.field("JumpEdit", "placeholder %d" % ptype, PlaceHolderType=ptype, PlaceHolder="ph%d" % ptype, Hint="hint")
    w.para()
    w.field("Input", "input", Content="text", Hint="descr", Help="help text", Tooltip="tip text")
    w.field("Input", "input2", Content="x", Hint="d2")
    w.para()
    w.field("ConditionalText", "cond", Condition="1==1", TrueContent="yes", FalseContent="no")
    w.field("ConditionalText", "cond2", Condition="1==2", TrueContent="yes", FalseContent="no")
    w.field("HiddenText", "hidden", Condition="1==2", Content="secret")
    w.field("HiddenText", "hidden2", Condition="1==1", Content="secret2")
    w.field("HiddenParagraph", "hiddenpar", Condition="1==2")
    w.para("after hidden paragraph")
    w.field("HiddenParagraph", "hiddenpar2", Condition="1==1")
    w.para()
    w.field("DropDown", "dropdown", Name="dd", Items=("one", "two", "three"), SelectedItem="two", Help="dh", Tooltip="dt")
    w.field("DropDown", "dropdown2", Name="dd2", Items=("a",), SelectedItem="zzz")
    w.para()
    w.field("Script", "script1", ScriptType="text/javascript", Content="alert(1)", URLContent=False)
    w.field("Script", "script2", ScriptType="text/javascript", Content="http://example.org/s.js", URLContent=True)
    w.para()
    w.field("ReferencePageSet", "refpageset", On=True, Offset=3)
    w.field("ReferencePageSet", "refpageset0", On=False, Offset=0)
    w.field("ReferencePageGet", "refpageget", NumberingType=4)
    w.field("ReferencePageGet", "refpageget2", NumberingType=1)
    w.para()
    w.field("CombinedCharacters", "combined", Content="ab")
    w.para()
    w.field("Macro", "macro", Hint="macro hint", MacroLibrary="Standard", MacroName="Module1.Macro1")
    w.field("Macro", "macro2", Hint="h", ScriptURL="vnd.sun.star.script:Module1.Macro1?language=Basic&location=document",
            MacroLibrary="", MacroName="")
    w.para()
    save(doc, "fields-misc.odt")
    doc.close(True)



NF = {"date": 2, "time": 4, "datetime": 6, "currency": 8, "number": 16, "scientific": 32, "fraction": 64,
      "percent": 128, "text": 256, "boolean": 1024}


def stdfmt(doc, kind):
    loc = uno.createUnoStruct("com.sun.star.lang.Locale", "en", "US", "")
    return doc.NumberFormats.getStandardFormat(NF[kind], loc)


def doc_variables():
    doc = new()
    w = W(doc)

    def setvar(name, sub, content, fmt=None, visible=True, value=None, inp=False, hint=None, label=None):
        m = doc.createInstance("com.sun.star.text.FieldMaster.SetExpression")
        try:
            m.Name = name
        except Exception:
            pass
        m.SubType = sub
        f = doc.createInstance("com.sun.star.text.TextField.SetExpression")
        f.attachTextFieldMaster(m)
        f.SubType = sub
        f.Content = content
        if fmt is not None:
            f.NumberFormat = fmt
        f.IsVisible = visible
        if value is not None:
            f.Value = value
        if inp:
            f.IsInput = True
            if hint:
                f.Hint = hint
        w.put(f)
        w.s(" ")
        return f

    for kind in ("number", "date", "time", "datetime", "currency", "percent", "scientific", "fraction", "boolean"):
        attempt("setvar " + kind, setvar, "var_" + kind, 0, "1+2" if kind != "boolean" else "1", stdfmt(doc, kind), True, 3.5)
    w.para()
    attempt("setvar hidden", setvar, "var_hidden", 0, "5", stdfmt(doc, "number"), False)
    attempt("setvar string", setvar, "var_str", 3, "some text", stdfmt(doc, "text"))
    attempt("setvar string2", setvar, "var_str2", 3, "other", None, False)
    attempt("setvar input num", setvar, "var_in", 0, "7", stdfmt(doc, "number"), True, 7.0, True, "num hint")
    attempt("setvar input str", setvar, "var_in2", 3, "txt", None, True, None, True, "str hint")
    attempt("setvar input date", setvar, "var_in3", 0, "40000", stdfmt(doc, "date"), True, 40000.0, True, "date hint")
    w.para()
    for kind in ("number", "date", "currency", "percent", "boolean"):
        for show in (False, True):
            f = w.field("GetExpression", "getvar %s %s" % (kind, show), Content="var_" + kind, SubType=0,
                        NumberFormat=stdfmt(doc, kind), IsShowFormula=show)
    w.field("GetExpression", "getvar str", Content="var_str", SubType=3, NumberFormat=stdfmt(doc, "text"))
    w.para()
    for show in (False, True):
        for kind in ("number", "date", "percent"):
            w.field("GetExpression", "expr %s %s" % (kind, show), Content="1+2*3", SubType=2,
                    NumberFormat=stdfmt(doc, kind), IsShowFormula=show)
    w.field("GetExpression", "expr nofmt", Content="var_number*2", SubType=2)
    w.para()

    um = []
    for name, isexpr, val in (("usr_num", True, 12.5), ("usr_str", False, "hello"), ("usr_num2", True, 3.0)):
        m = doc.createInstance("com.sun.star.text.FieldMaster.User")
        m.Name = name
        m.IsExpression = isexpr
        if isexpr:
            m.Value = val
        else:
            m.Content = val
        um.append(m)
    for m in um:
        for kind in ("number", "date", "currency", "text"):
            for visible in (True, False):
                for show in (False, True):
                    f = doc.createInstance("com.sun.star.text.TextField.User")
                    try:
                        f.attachTextFieldMaster(m)
                        f.NumberFormat = stdfmt(doc, kind)
                        f.IsVisible = visible
                        f.IsShowFormula = show
                        w.put(f)
                        w.s(" ")
                    except Exception as e:
                        print("FAIL userfield", m.Name, kind, repr(e)[:100])
                        failed.append("userfield")
    w.para()
    f = doc.createInstance("com.sun.star.text.TextField.InputUser")
    f.Content = "usr_str"
    f.Hint = "user hint"
    w.put(f)
    w.s(" ")
    w.para()
    ddem = doc.createInstance("com.sun.star.text.FieldMaster.DDE")
    ddem.Name = "ddeconn"
    ddem.DDECommandType = "soffice"
    ddem.DDECommandFile = "file.ods"
    ddem.DDECommandElement = "Sheet1.A1"
    ddem.IsAutomaticUpdate = False
    f = doc.createInstance("com.sun.star.text.TextField.DDE")
    f.attachTextFieldMaster(ddem)
    w.put(f)
    w.s(" ")
    ddem2 = doc.createInstance("com.sun.star.text.FieldMaster.DDE")
    ddem2.Name = "ddeconn2"
    ddem2.DDECommandType = "soffice"
    ddem2.DDECommandFile = "file2.ods"
    ddem2.DDECommandElement = "Sheet1.B2"
    ddem2.IsAutomaticUpdate = True
    f = doc.createInstance("com.sun.star.text.TextField.DDE")
    f.attachTextFieldMaster(ddem2)
    w.put(f)
    w.s(" ")
    w.para()
    save(doc, "variables.odt")
    doc.close(True)



def tpl(**kw):
    return tuple(pv(k, v) for k, v in kw.items())


def mark(w, service, text, absorb_len=0, **props):
    m = w.mk("com.sun.star.text." + service, **props)
    if absorb_len:
        w.cur.goLeft(absorb_len, True)
        w.text.insertTextContent(w.cur, m, True)
        w.cur.gotoEnd(False)
    else:
        w.put(m)
    return m


def doc_indexes():
    doc = new()
    w = W(doc)
    w.cur.ParaStyleName = "Heading 1"
    w.s("Chapter one")
    w.para()
    w.cur.ParaStyleName = "Standard"
    w.s("Body paragraph with an ")
    w.s("alpha")
    attempt("docmark range", mark, w, "DocumentIndexMark", "alpha", 5, AlternativeText="", PrimaryKey="Greek", SecondaryKey="first",
            TextReading="a", PrimaryKeyReading="g", SecondaryKeyReading="f", IsMainEntry=True)
    w.s(" and ")
    attempt("docmark point", mark, w, "DocumentIndexMark", "", 0, AlternativeText="beta entry", PrimaryKey="Greek",
            SecondaryKey="second", IsMainEntry=False, TextReading="b", PrimaryKeyReading="g2", SecondaryKeyReading="s2")
    w.s(" and ")
    w.s("gamma")
    attempt("docmark range2", mark, w, "DocumentIndexMark", "gamma", 5, PrimaryKey="Greek", IsMainEntry=False)
    w.s(" and tocmark ")
    attempt("tocmark point", mark, w, "ContentIndexMark", "", 0, AlternativeText="toc point", Level=1)
    w.s(" tocrange")
    attempt("tocmark range", mark, w, "ContentIndexMark", "", 8, Level=2)
    w.s(" and usermark ")
    attempt("usermark point", mark, w, "UserIndexMark", "", 0, AlternativeText="user point", Level=0, UserIndexName="MyIndex")
    w.s(" userrange")
    attempt("usermark range", mark, w, "UserIndexMark", "", 9, Level=1, UserIndexName="MyIndex")
    w.s(".")
    w.para()
    w.cur.ParaStyleName = "Heading 2"
    w.s("Section 1.1")
    w.para()
    w.cur.ParaStyleName = "Standard"
    w.para("Another body.")
    w.cur.ParaStyleName = "Heading 3"
    w.para("Deep heading")
    w.cur.ParaStyleName = "Standard"
    w.para("")

    def seqfield(cat, desc):
        w.para("%s " % cat)
        try:
            mname = "com.sun.star.text.FieldMaster.SetExpression." + cat
            master = doc.TextFieldMasters.getByName(mname)
        except Exception:
            master = doc.createInstance("com.sun.star.text.FieldMaster.SetExpression")
            master.Name = cat
            master.SubType = 1
        f = doc.createInstance("com.sun.star.text.TextField.SetExpression")
        f.attachTextFieldMaster(master)
        f.SubType = 1
        f.NumberingType = 9 if cat == "Text" else 4
        f.Content = cat + "+1"
        w.put(f)
        w.s(": " + desc)

    for cat in ("Illustration", "Illustration", "Table", "Table", "Drawing", "Text"):
        attempt("seq " + cat, seqfield, cat, "description of " + cat)
    w.para()

    def add(label, service, props, levels=None, lps=None):
        w.para("")
        w.s("")
        idx = w.mk("com.sun.star.text." + service)
        for k, v in props.items():
            try:
                setattr(idx, k, v)
            except Exception as e:
                print("prop fail", label, k, repr(e)[:80])
                failed.append(label + " " + k)
        if lps:
            try:
                for lv, names in lps.items():
                    uno.invoke(idx.LevelParagraphStyles, "replaceByIndex", (lv, uno.Any("[]string", tuple(names))))
            except Exception as e:
                print("lps fail", label, repr(e)[:120])
                failed.append(label + " lps")
        if levels:
            try:
                lf = idx.LevelFormat
                for lv, toks in levels.items():
                    uno.invoke(lf, "replaceByIndex", (lv, uno.Any("[][]com.sun.star.beans.PropertyValue", tuple(tuple(t) for t in toks))))
            except Exception as e:
                print("levels fail", label, repr(e)[:120])
                failed.append(label + " levels")
        w.put(idx)
        w.cur.gotoEnd(False)
        try:
            idx.update()
        except Exception as e:
            print("update fail", label, repr(e)[:100])
        return idx

    toks_toc = [
        tpl(TokenType="TokenEntryNumber", CharacterStyleName="Strong Emphasis"),
        tpl(TokenType="TokenText", Text="> ", CharacterStyleName="Emphasis"),
        tpl(TokenType="TokenHyperlinkStart", CharacterStyleName="Internet link"),
        tpl(TokenType="TokenEntryText", CharacterStyleName="Emphasis"),
        tpl(TokenType="TokenHyperlinkEnd", CharacterStyleName="Internet link"),
        tpl(TokenType="TokenTabStop", TabStopRightAligned=False, TabStopPosition=3000, TabStopFillCharacter="-", WithTab=False,
            CharacterStyleName="Emphasis"),
        tpl(TokenType="TokenTabStop", TabStopRightAligned=True, TabStopFillCharacter=".", WithTab=True),
        tpl(TokenType="TokenPageNumber", CharacterStyleName="Strong Emphasis"),
        tpl(TokenType="TokenChapterInfo", ChapterFormat=1, ChapterLevel=2, CharacterStyleName="Emphasis"),
    ]
    attempt("toc1", add, "toc1", "ContentIndex", dict(Title="Contents", Level=10, CreateFromOutline=True, CreateFromMarks=False,
            CreateFromLevelParagraphStyles=True, IsRelativeTabstops=False, CreateFromChapter=True, IsProtected=True,
            ParaStyleHeading="Heading 2", ParaStyleLevel1="Heading 3", Name="TOC one"),
            {1: toks_toc, 2: toks_toc[:4]}, {0: ("Heading 4", "Title"), 2: ("Subtitle",)})
    attempt("toc2", add, "toc2", "ContentIndex", dict(Title="", Level=3, CreateFromOutline=False, CreateFromMarks=True,
            IsRelativeTabstops=True, Name="TOC two"))
    attempt("toc3", add, "toc3", "ContentIndex", dict(Title="Plain", Level=1, CreateFromOutline=True, CreateFromMarks=True))

    toks_alpha = [
        tpl(TokenType="TokenEntryText", CharacterStyleName="Emphasis"),
        tpl(TokenType="TokenTabStop", TabStopRightAligned=True, TabStopFillCharacter=" "),
        tpl(TokenType="TokenPageNumber"),
        tpl(TokenType="TokenChapterInfo", ChapterFormat=4, ChapterLevel=1),
    ]
    for n, props in enumerate((
            dict(Title="Alpha 1", UseAlphabeticalSeparators=True, UseCombinedEntries=False, UseDash=True, UseKeyAsEntry=True,
                 UsePP=False, UseUpperCase=True, IsCommaSeparated=True, IsCaseSensitive=True, CreateFromChapter=True,
                 IsRelativeTabstops=False, MainEntryCharacterStyleName="Strong Emphasis", SortAlgorithm="alphanumeric",
                 Locale=uno.createUnoStruct("com.sun.star.lang.Locale", "en", "US", "")),
            dict(Title="Alpha 2", UseAlphabeticalSeparators=False, UseCombinedEntries=True, UseDash=False, UseKeyAsEntry=False,
                 UsePP=True, UseUpperCase=False, IsCommaSeparated=False, IsCaseSensitive=False,
                 SortAlgorithm="", Locale=uno.createUnoStruct("com.sun.star.lang.Locale", "de", "DE", ""))), 1):
        attempt("alpha%d" % n, add, "alpha%d" % n, "DocumentIndex", props, {1: toks_alpha, 2: toks_alpha[:2], 3: toks_alpha[:3]})

    for n, (service, props) in enumerate((
            ("IllustrationsIndex", dict(Title="Figures", CreateFromLabels=True, LabelCategory="Illustration", LabelDisplayType=5,
                                         CreateFromChapter=True, IsRelativeTabstops=False)),
            ("IllustrationsIndex", dict(Title="Figures 2", CreateFromLabels=False, LabelCategory="Drawing", LabelDisplayType=5)),
            ("IllustrationsIndex", dict(Title="Figures 3", LabelCategory="Text", LabelDisplayType=6)),
            ("TableIndex", dict(Title="Tables", CreateFromLabels=True, LabelCategory="Table", LabelDisplayType=5,
                                CreateFromChapter=True, IsRelativeTabstops=False)),
            ("TableIndex", dict(Title="Tables 2", CreateFromLabels=False, LabelCategory="Table", LabelDisplayType=6)),
            ("TableIndex", dict(Title="Tables 3", LabelCategory="Table", LabelDisplayType=6)))):
        attempt("%s%d" % (service, n), add, "%s%d" % (service, n), service, props,
                {1: toks_alpha[:3]})

    for n, props in enumerate((
            dict(Title="Objects", CreateFromOtherEmbeddedObjects=True, CreateFromStarCalc=True, CreateFromStarChart=True,
                 CreateFromStarDraw=True, CreateFromStarMath=True, CreateFromChapter=True, IsRelativeTabstops=False),
            dict(Title="Objects 2", CreateFromOtherEmbeddedObjects=False, CreateFromStarCalc=False, CreateFromStarChart=False,
                 CreateFromStarDraw=False, CreateFromStarMath=False))):
        attempt("objidx%d" % n, add, "objidx%d" % n, "ObjectIndex", props, {1: toks_alpha[:3]})

    for n, props in enumerate((
            dict(Title="User", UserIndexName="MyIndex", CreateFromEmbeddedObjects=True, CreateFromGraphicObjects=True,
                 CreateFromMarks=True, CreateFromTables=True, CreateFromTextFrames=True, UseLevelFromSource=True,
                 CreateFromLevelParagraphStyles=True, CreateFromChapter=True, IsRelativeTabstops=False,
                 ParaStyleLevel1="Heading 1"),
            dict(Title="User 2", UserIndexName="Other", CreateFromEmbeddedObjects=False, CreateFromGraphicObjects=False,
                 CreateFromMarks=False, CreateFromTables=False, CreateFromTextFrames=False, UseLevelFromSource=False,
                 CreateFromLevelParagraphStyles=False))):
        attempt("userindex%d" % n, add, "userindex%d" % n, "UserIndex", props, {1: toks_toc[:5], 2: toks_toc[:3]}, {0: ("Heading 5",), 1: ("Heading 6", "Heading 7")})

    save(doc, "indexes.odt")
    doc.close(True)



def doc_objects():
    doc = new()
    w = W(doc)
    w.s("objects ")
    for name, clsid in (("math", "078B7ABA-54FC-457F-8551-6147e776a997"), ("calc", "47BBB4CB-CE4C-4E80-A591-42D9AE74950F"),
                        ("chart", "12DCAE26-281F-416F-a234-c3086127382e"), ("draw", "8BC6B165-B1B2-4EDD-aa47-dae2ee689dd6"),
                        ("impress", "9176E48A-637A-4D1F-803B-99D9BFAC1047")):
        try:
            o = doc.createInstance("com.sun.star.text.TextEmbeddedObject")
            o.CLSID = clsid
            o.Name = "obj_" + name
            w.put(o)
            w.para()
        except Exception as e:
            failed.append("embed " + name)
            print("FAIL embed", name, repr(e)[:100])
    for n, props in enumerate((
            dict(Title="Objects", CreateFromOtherEmbeddedObjects=True, CreateFromStarCalc=True, CreateFromStarChart=True,
                 CreateFromStarDraw=True, CreateFromStarMath=True, CreateFromChapter=True, IsRelativeTabstops=False),
            dict(Title="Objects 2", CreateFromOtherEmbeddedObjects=False, CreateFromStarCalc=False, CreateFromStarChart=False,
                 CreateFromStarDraw=False, CreateFromStarMath=False))):
        idx = w.mk("com.sun.star.text.ObjectIndex", **props)
        w.put(idx)
        w.cur.gotoEnd(False)
        idx.update()
    doc.IndexAutoMarkFileURL = "file:///tmp/automark.sdi"
    save(doc, "objects.odt")
    doc.close(True)


BIB_NAMES = ["Identifier", "BibiliographicType", "Address", "Annote", "Author", "Booktitle", "Chapter", "Edition", "Editor",
             "Howpublished", "Institution", "Journal", "Month", "Note", "Number", "Organizations", "Pages", "Publisher", "School",
             "Series", "Title", "Volume", "Year", "URL", "Custom1", "Custom2", "Custom3", "Custom4", "Custom5", "ISBN", "ReportType"]


def doc_bibliography():
    doc = new()
    w = W(doc)
    w.s("Citations: ")
    for t in range(22):
        fields = [pv(n, "%s-%d" % (n, t)) for n in BIB_NAMES if n != "BibiliographicType"]
        fields.append(pv("BibiliographicType", t))
        f = w.mk("com.sun.star.text.TextField.Bibliography")
        f.Fields = tuple(fields)
        w.put(f)
        w.s(" ")
    w.para()
    f = w.mk("com.sun.star.text.TextField.Bibliography")
    f.Fields = (pv("Identifier", "only-id"), pv("BibiliographicType", 1))
    w.put(f)
    w.para()
    master = doc.TextFieldMasters.getByName("com.sun.star.text.FieldMaster.Bibliography")
    master.BracketBefore = "("
    master.BracketAfter = ")"
    master.IsNumberEntries = True
    master.IsSortByPosition = False
    master.SortAlgorithm = "alphanumeric"
    master.Locale = uno.createUnoStruct("com.sun.star.lang.Locale", "en", "US", "")
    master.SortKeys = (tuple([pv("SortKey", 4), pv("IsSortAscending", True)]), tuple([pv("SortKey", 20), pv("IsSortAscending", False)]),
                       tuple([pv("SortKey", 22), pv("IsSortAscending", True)]))
    idx = w.mk("com.sun.star.text.Bibliography", Title="Bibliography", IsProtected=True, Name="Bib index")
    lf = idx.LevelFormat
    for lvl in range(1, 23):
        toks = [tpl(TokenType="TokenBibliographyDataField", BibliographyDataField=(lvl - 1 + k) % 31, CharacterStyleName="Emphasis")
                for k in range(3)] + [tpl(TokenType="TokenText", Text=" | ")]
        uno.invoke(lf, "replaceByIndex", (lvl, uno.Any("[][]com.sun.star.beans.PropertyValue", tuple(toks))))
    w.put(idx)
    w.cur.gotoEnd(False)
    idx.update()
    idx2 = w.mk("com.sun.star.text.Bibliography", Title="Bibliography 2")
    lf = idx2.LevelFormat
    toks = [tpl(TokenType="TokenBibliographyDataField", BibliographyDataField=k) for k in range(31)]
    uno.invoke(lf, "replaceByIndex", (1, uno.Any("[][]com.sun.star.beans.PropertyValue", tuple(toks))))
    w.put(idx2)
    w.cur.gotoEnd(False)
    idx2.update()
    save(doc, "bibliography.odt")
    doc.close(True)



def seqmaster(doc, name):
    try:
        return doc.TextFieldMasters.getByName("com.sun.star.text.FieldMaster.SetExpression." + name)
    except Exception:
        m = doc.createInstance("com.sun.star.text.FieldMaster.SetExpression")
        m.Name = name
        m.SubType = 1
        return m


def getref(w, source, part, name, seq=None, label=None):
    f = w.mk("com.sun.star.text.TextField.GetReference", ReferenceFieldSource=source, ReferenceFieldPart=part, SourceName=name)
    if seq is not None:
        f.SequenceNumber = seq
    w.put(f)
    w.s(" ")
    return f


def doc_refs_notes():
    doc = new()
    w = W(doc)
    fs = doc.FootnoteSettings
    fs.FootnoteCounting = 1
    fs.NumberingType = 9
    fs.StartAt = 2
    fs.Prefix = "["
    fs.Suffix = "]"
    fs.PositionEndOfDoc = True
    fs.BeginNotice = "continued from previous page"
    fs.EndNotice = "continued on next page"
    es = doc.EndnoteSettings
    es.NumberingType = 10
    es.StartAt = 3
    es.Prefix = "<"
    es.Suffix = ">"
    for st, nm in ((fs, "Footnote"), (es, "Endnote")):
        try:
            st.ParaStyleName = nm
            st.CharStyleName = "Footnote Characters" if nm == "Footnote" else "Endnote Characters"
            st.AnchorCharStyleName = "Footnote Anchor" if nm == "Footnote" else "Endnote Anchor"
            st.PageStyleName = "Footnote" if nm == "Footnote" else "Endnote"
        except Exception as e:
            print("notes style", nm, repr(e)[:100])
    w.s("Heading text with a note")
    for kind in ("Footnote", "Endnote", "Footnote", "Endnote"):
        n = w.mk("com.sun.star.text." + kind)
        w.put(n)
        n.String = "note text " + kind
        if kind == "Footnote" and doc.Footnotes.Count == 2:
            n.Label = "*"
    w.s(" ")
    w.para()
    w.s("bookmark target text")
    w.cur.goLeft(6, True)
    bm = w.mk("com.sun.star.text.Bookmark", Name="bm1")
    w.text.insertTextContent(w.cur, bm, True)
    w.cur.gotoEnd(False)
    bm2 = w.mk("com.sun.star.text.Bookmark", Name="bm2")
    w.put(bm2)
    w.s(" ")
    w.para("reference mark ")
    w.s("ranged")
    w.cur.goLeft(6, True)
    rm = w.mk("com.sun.star.text.ReferenceMark", Name="rm1")
    w.text.insertTextContent(w.cur, rm, True)
    w.cur.gotoEnd(False)
    rm2 = w.mk("com.sun.star.text.ReferenceMark", Name="rm2")
    w.put(rm2)
    w.s(" ")
    w.para()
    for part in range(0, 11):
        for src, name in ((2, "bm1"), (2, "bm2"), (0, "rm1"), (0, "rm2")):
            attempt("getref %d %d %s" % (src, part, name), getref, w, src, part, name)
    w.para()
    for part in (0, 1, 2, 3, 5, 6, 7, 8, 9, 10):
        for cat, num in (("Illustration", 0), ("Table", 1)):
            attempt("seqref", getref, w, 1, part, cat, num)
    w.para("Illustration ")
    m = seqmaster(doc, "Illustration")
    f = w.mk("com.sun.star.text.TextField.SetExpression")
    f.attachTextFieldMaster(m)
    f.SubType = 1
    f.NumberingType = 4
    f.Content = "Illustration+1"
    w.put(f)
    w.s(": a caption")
    w.para("Table ")
    m2 = seqmaster(doc, "Table")
    f = w.mk("com.sun.star.text.TextField.SetExpression")
    f.attachTextFieldMaster(m2)
    f.SubType = 1
    f.NumberingType = 4
    f.Content = "Table+1"
    w.put(f)
    w.s(": a table caption")
    w.para()
    ids = [doc.Footnotes.getByIndex(i).ReferenceId for i in range(doc.Footnotes.Count)]
    eids = [doc.Endnotes.getByIndex(i).ReferenceId for i in range(doc.Endnotes.Count)]
    for src, lst in ((3, ids), (4, eids)):
        for rid in lst:
            for part in (0, 1, 2, 3, 8, 9, 10):
                attempt("noteref", getref, w, src, part, "", rid)
    w.para()
    doc.TextFields.refresh()
    save(doc, "refs-notes.odt")
    doc.close(True)


def doc_notes_variants():
    doc = new()
    w = W(doc)
    fs = doc.FootnoteSettings
    for cnt, eod in ((0, False), (2, False)):
        fs.FootnoteCounting = cnt
        fs.PositionEndOfDoc = eod
    es = doc.EndnoteSettings
    es.NumberingType = 4
    w.s("one")
    n = w.mk("com.sun.star.text.Footnote")
    w.put(n)
    n.String = "fn"
    w.s(" two")
    n = w.mk("com.sun.star.text.Endnote")
    w.put(n)
    n.String = "en"
    save(doc, "notes-variants.odt")
    doc.close(True)


def doc_annotations():
    doc = new()
    w = W(doc)
    w.s("plain text with a ranged annotation here and a point annotation")
    w.cur.gotoStart(False)
    w.cur.goRight(5, False)
    w.cur.goRight(4, True)
    a = w.mk("com.sun.star.text.TextField.Annotation", Author="Ann Author", Content="ranged comment", Initials="AA",
             DateTimeValue=dt(2024, 7, 8, 9, 10), Name="ann1")
    w.text.insertTextContent(w.cur, a, True)
    w.cur.gotoEnd(False)
    a2 = w.mk("com.sun.star.text.TextField.Annotation", Author="Bob", Content="point comment", Initials="B",
              DateTimeValue=dt(2024, 7, 9, 9, 10))
    w.put(a2)
    w.s(" after")
    w.para("second para ranged")
    w.cur.goLeft(6, True)
    a3 = w.mk("com.sun.star.text.TextField.Annotation", Author="Cy", Content="third", Name="ann3")
    w.text.insertTextContent(w.cur, a3, True)
    w.cur.gotoEnd(False)
    save(doc, "annotations.odt")
    doc.close(True)


def doc_hyperlinks():
    doc = new()
    w = W(doc)
    w.s("link one ")
    w.cur.goLeft(9, True)
    w.cur.HyperLinkURL = "http://example.org/one"
    w.cur.HyperLinkName = "one name"
    w.cur.HyperLinkTarget = "_blank"
    w.cur.UnvisitedCharStyleName = "Internet link"
    w.cur.VisitedCharStyleName = "Visited Internet Link"
    w.cur.gotoEnd(False)
    w.cur.setPropertyToDefault("HyperLinkURL")
    w.s("link two ")
    w.cur.goLeft(9, True)
    w.cur.HyperLinkURL = "http://example.org/two"
    w.cur.HyperLinkTarget = "_self"
    w.cur.gotoEnd(False)
    w.cur.setPropertyToDefault("HyperLinkURL")
    w.s("link three ")
    w.cur.goLeft(11, True)
    w.cur.HyperLinkURL = "#bookmark"
    w.cur.HyperLinkTarget = "_top"
    w.cur.gotoEnd(False)
    w.cur.setPropertyToDefault("HyperLinkURL")
    w.s("link four ")
    w.cur.goLeft(10, True)
    w.cur.HyperLinkURL = "http://example.org/four"
    w.cur.HyperLinkTarget = "_parent"
    w.cur.gotoEnd(False)
    w.cur.setPropertyToDefault("HyperLinkURL")
    w.s("link five ")
    w.cur.goLeft(10, True)
    w.cur.HyperLinkURL = "http://example.org/five"
    w.cur.HyperLinkTarget = "named_frame"
    w.cur.gotoEnd(False)
    w.cur.setPropertyToDefault("HyperLinkURL")
    w.para("ruby ")
    w.s("base")
    w.cur.goLeft(4, True)
    w.cur.RubyText = "ruby"
    w.cur.RubyAdjust = 1
    w.cur.RubyCharStyleName = "Emphasis"
    w.cur.RubyIsAbove = False
    w.cur.gotoEnd(False)
    w.s(" more")
    w.cur.goLeft(4, True)
    w.cur.RubyText = "r2"
    w.cur.RubyAdjust = 3
    w.cur.RubyIsAbove = True
    w.cur.gotoEnd(False)
    save(doc, "hyperlinks-ruby.odt")
    doc.close(True)



def png_file():
    import struct, zlib, tempfile
    raw = b"".join(b"\x00" + b"\xff\x00\x00\xff" * 4 for _ in range(4))
    def chunk(t, d):
        c = struct.pack(">I", len(d)) + t + d
        return c + struct.pack(">I", zlib.crc32(t + d) & 0xffffffff)
    data = b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", 4, 4, 8, 6, 0, 0, 0)) + chunk(b"IDAT", zlib.compress(raw)) + chunk(b"IEND", b"")
    p = os.path.join(tempfile.gettempdir(), "t12_writer_bullet.png")
    open(p, "wb").write(data)
    return uno.systemPathToFileUrl(p)


def setlevel(rules, i, **props):
    uno.invoke(rules, "replaceByIndex", (i, uno.Any("[]com.sun.star.beans.PropertyValue", tuple(pv(k, v) for k, v in props.items()))))


def doc_lists():
    doc = new()
    w = W(doc)
    styles = doc.StyleFamilies.getByName("NumberingStyles")
    st = doc.createInstance("com.sun.star.style.NumberingStyle")
    styles.insertByName("T12 List", st)
    rules = st.NumberingRules
    fd = uno.createUnoStruct("com.sun.star.awt.FontDescriptor")
    fd.Name = "OpenSymbol"
    img = png_file()
    cfgs = [
        dict(NumberingType=4, Prefix="(", Suffix=")", StartWith=3, ParentNumbering=1, Adjust=1, LeftMargin=500, FirstLineOffset=-300, SymbolTextDistance=100),
        dict(NumberingType=0, Prefix="", Suffix=".", StartWith=1, ParentNumbering=2, Adjust=2, CharStyleName="Emphasis"),
        dict(NumberingType=9, Suffix=":"),
        dict(NumberingType=6, BulletChar="*", BulletFont=fd, BulletRelSize=60, Prefix="<", Suffix=">", BulletColor=0x336699),
        dict(NumberingType=8, GraphicURL=img, GraphicSize=uno.createUnoStruct("com.sun.star.awt.Size", 400, 300), VertOrient=1),
        dict(NumberingType=8, GraphicURL=img, GraphicSize=uno.createUnoStruct("com.sun.star.awt.Size", 400, 300), VertOrient=2),
        dict(NumberingType=8, GraphicURL=img, GraphicSize=uno.createUnoStruct("com.sun.star.awt.Size", 400, 300), VertOrient=3),
        dict(NumberingType=2, PositionAndSpaceMode=1, LabelFollowedBy=1, ListtabStopPosition=2000, FirstLineIndent=-500, IndentAt=1500),
        dict(NumberingType=3, PositionAndSpaceMode=1, LabelFollowedBy=2, FirstLineIndent=-500, IndentAt=1500),
        dict(NumberingType=10, PositionAndSpaceMode=1, LabelFollowedBy=0, ListtabStopPosition=1000, FirstLineIndent=-300, IndentAt=900),
    ]
    for i, c in enumerate(cfgs):
        attempt("list level %d" % i, setlevel, rules, i, **c)
    st.NumberingRules = rules
    st2 = doc.createInstance("com.sun.star.style.NumberingStyle")
    styles.insertByName("T12 List 2", st2)
    r2 = st2.NumberingRules
    for i in range(3):
        attempt("list2 level %d" % i, setlevel, r2, i, NumberingType=6, BulletChar="-", BulletFont=fd)
    st2.NumberingRules = r2

    for lvl in range(10):
        w.cur.NumberingStyleName = "T12 List"
        w.cur.NumberingLevel = lvl
        w.s("item level %d" % lvl)
        w.para()
    w.cur.NumberingStyleName = ""
    w.cur.setPropertyToDefault("NumberingStyleName")
    w.para("between lists")

    def item(txt, style="T12 List", lvl=0, **props):
        w.cur.NumberingStyleName = style
        w.cur.NumberingLevel = lvl
        for k, v in props.items():
            setattr(w.cur, k, v)
        w.s(txt)
        w.para()

    item("restart item", ParaIsNumberingRestart=True, NumberingStartValue=7)
    item("normal item")
    item("not numbered item", NumberingIsNumber=False)
    item("sub restart", lvl=1, ParaIsNumberingRestart=True, NumberingStartValue=4)
    item("other style item", style="T12 List 2", lvl=1)
    item("back to first", ParaIsNumberingRestart=False)
    w.cur.setPropertyToDefault("NumberingStyleName")
    w.cur.setPropertyToDefault("ParaIsNumberingRestart")
    w.cur.setPropertyToDefault("NumberingIsNumber")
    w.para("break")

    h = doc.ChapterNumberingRules
    ocfg = [dict(NumberingType=4, Prefix="Ch ", Suffix=".", StartWith=2, ParentNumbering=1),
            dict(NumberingType=0, Prefix="", Suffix=")", StartWith=3),
            dict(NumberingType=9, Prefix="[", Suffix="]", StartWith=1),
            dict(NumberingType=1, StartWith=4),
            dict(NumberingType=10), dict(NumberingType=2), dict(NumberingType=3), dict(NumberingType=4), dict(NumberingType=5)]
    for i, c in enumerate(ocfg):
        attempt("outline level %d" % i, setlevel, h, i, **c)
    for lvl in (1, 2, 3, 2, 1):
        w.cur.ParaStyleName = "Heading %d" % lvl
        w.s("Heading level %d" % lvl)
        w.para()
    w.cur.ParaStyleName = "Heading 1"
    w.cur.ParaIsNumberingRestart = True
    w.cur.NumberingStartValue = 5
    w.s("Heading restart")
    w.para()
    w.cur.ParaIsNumberingRestart = False
    w.cur.NumberingIsNumber = False
    w.s("Heading not numbered")
    w.para()
    w.cur.NumberingIsNumber = True
    w.cur.ParaStyleName = "Standard"
    w.s("end")
    save(doc, "lists-outline.odt")
    doc.close(True)


def doc_linenumbers():
    for pos in (0, 1, 2):
        doc = new()
        w = W(doc)
        lp = doc.LineNumberingProperties
        lp.IsOn = True
        lp.NumberPosition = pos
        lp.Interval = 2
        lp.SeparatorText = ""
        w.s("numbered line")
        save(doc, "linenumbers-%d.odt" % pos)
        doc.close(True)


def doc_structure():
    doc = new()
    w = W(doc)
    lp = doc.LineNumberingProperties
    lp.IsOn = False
    lp.CountEmptyLines = False
    lp.CountLinesInFrames = True
    lp.RestartAtEachPage = True
    lp.Distance = 700
    lp.NumberingType = 9
    lp.NumberPosition = 3
    lp.Interval = 3
    lp.SeparatorText = "|"
    lp.SeparatorInterval = 4
    lp.CharStyleName = "Emphasis"
    for i in range(6):
        w.para("line number test paragraph %d" % i)
    w.para()
    for n, props in enumerate((
            dict(Condition="1==1", IsVisible=False, IsProtected=True, ProtectionKey=b"\x01\x02\x03\x04\x05"),
            dict(Condition="1==2", IsVisible=True),
            dict(Condition="", IsVisible=False, IsProtected=False),
            dict(FileLink=uno.createUnoStruct("com.sun.star.text.SectionFileLink", "file:///tmp/t12_missing.odt", "writer8"),
                 LinkRegion="region1"),
            dict(FileLink=uno.createUnoStruct("com.sun.star.text.SectionFileLink", "file:///tmp/t12_missing2.odt", ""), LinkRegion=""),
            dict(DDECommandType="soffice", DDECommandFile="file.ods", DDECommandElement="Sheet1.A1", IsAutomaticUpdate=True),
            dict(DDECommandType="soffice", DDECommandFile="file2.ods", DDECommandElement="Sheet1.B1", IsAutomaticUpdate=False))):
        w.para("before section %d" % n)
        w.s("")
        sec = w.mk("com.sun.star.text.TextSection", Name="sec%d" % n)
        try:
            for k, v in props.items():
                if k == "ProtectionKey":
                    v = uno.ByteSequence(v)
                setattr(sec, k, v)
        except Exception as e:
            print("FAIL section prop", n, k, repr(e)[:100])
            failed.append("section %d %s" % (n, k))
        try:
            w.cur.gotoEnd(False)
            w.text.insertTextContent(w.cur, sec, False)
            w.cur.gotoEnd(False)
            w.s("section body %d" % n)
        except Exception as e:
            print("FAIL section", n, repr(e)[:100])
            failed.append("section %d" % n)
        w.cur.gotoEnd(False)
        w.para()
    sec = w.mk("com.sun.star.text.TextSection", Name="cols")
    cols = doc.createInstance("com.sun.star.text.TextColumns")
    cols.setColumnCount(2)
    sec.TextColumns = cols
    w.text.insertTextContent(w.cur, sec, False)
    w.cur.gotoEnd(False)
    w.s("two column section")
    w.para()
    w.s("tail")
    save(doc, "structure.odt")
    doc.close(True)



def enum(t, v):
    return uno.Enum("com.sun.star.form." + t, v)


class Forms:
    def __init__(self, doc):
        self.doc = doc
        self.page = doc.DrawPage
        self.forms = self.page.Forms
        self.n = 0
        self.y = 0

    def form(self, name, **props):
        f = self.doc.createInstance("com.sun.star.form.component.Form")
        self.forms.insertByName(name, f)
        for k, v in props.items():
            try:
                setattr(f, k, v)
            except Exception as e:
                print("FAIL form prop", name, k, repr(e)[:80])
                failed.append("form prop " + k)
        return f

    def control(self, form, service, name, **props):
        try:
            m = self.doc.createInstance("com.sun.star.form.component." + service)
        except Exception as e:
            print("FAIL control", service, repr(e)[:100])
            failed.append("control " + service)
            return None
        m.Name = name
        info = m.getPropertySetInfo()
        if info.hasPropertyByName("Tag"):
            m.Tag = "tag " + name
        for k, v in props.items():
            try:
                if k == "LabelControl":
                    continue
                if not info.hasPropertyByName(k):
                    print("no property", service, k)
                    continue
                setattr(m, k, v)
            except Exception as e:
                print("FAIL prop", service, k, repr(e)[:80])
                failed.append("%s.%s" % (service, k))
        form.insertByName(name, m)
        if "LabelControl" in props:
            try:
                m.LabelControl = props["LabelControl"]
            except Exception as e:
                print("FAIL LabelControl", repr(e)[:80])
        shape = self.doc.createInstance("com.sun.star.drawing.ControlShape")
        shape.setSize(uno.createUnoStruct("com.sun.star.awt.Size", 5000, 800))
        shape.setPosition(uno.createUnoStruct("com.sun.star.awt.Point", 500, 500 + self.y))
        self.y += 1000
        if service == "HiddenControl":
            return m
        try:
            shape.Control = m
            self.page.add(shape)
        except Exception as e:
            print("FAIL shape", service, repr(e)[:100])
            failed.append("shape " + service)
        return m


def doc_forms():
    doc = new()
    w = W(doc)
    w.s("form controls")
    doc.AutomaticControlFocus = True
    doc.ApplyFormDesignMode = False
    F = Forms(doc)
    f1 = F.form("Form1", DataSourceName="Bibliography", Command="biblio", CommandType=0, AllowDeletes=False, AllowInserts=False,
                AllowUpdates=False, ApplyFilter=True, EscapeProcessing=False, IgnoreResult=True, Filter="Year>1990", Order="Title",
                SubmitEncoding=enum("FormSubmitEncoding", "MULTIPART"), SubmitMethod=enum("FormSubmitMethod", "POST"),
                NavigationBarMode=enum("NavigationBarMode", "PARENT"), Cycle=enum("TabulatorCycle", "PAGE"),
                TargetFrame="_blank", TargetURL="http://example.org/submit")
    f2 = F.form("Form2", DataSourceName="file:///tmp/t12_missing.odb", Command="SELECT * FROM x", CommandType=2,
                SubmitEncoding=enum("FormSubmitEncoding", "TEXT"), SubmitMethod=enum("FormSubmitMethod", "GET"),
                NavigationBarMode=enum("NavigationBarMode", "NONE"), Cycle=enum("TabulatorCycle", "CURRENT"),
                MasterFields=("a", "b"), DetailFields=("c", "d"))
    f3 = F.form("Form3", Command="q1", CommandType=1, DataSourceName="OtherSource", NavigationBarMode=enum("NavigationBarMode", "CURRENT"),
                Cycle=enum("TabulatorCycle", "RECORDS"), AllowDeletes=True, ApplyFilter=False)
    f4 = F.form("Form4")
    C = F.control
    for i, (bt, extra) in enumerate((("PUSH", {}), ("SUBMIT", {}), ("RESET", {}), ("URL", dict(TargetURL="http://example.org/b", TargetFrame="_top")))):
        C(f1, "CommandButton", "btn%d" % i, Label="Button %d" % i, ButtonType=enum("FormButtonType", bt), DefaultButton=(i == 0),
          Toggle=(i == 1), FocusOnClick=(i != 2), ImagePosition=i, ImageAlign=i % 4, Tabstop=(i % 2 == 0), TabIndex=i + 1,
          Enabled=(i != 3), Printable=(i != 1), HelpText="tip %d" % i, RepeatDelay=100 * i, Repeat=(i == 1), State=i % 2, **extra)
    for i in range(13):
        C(f1, "CheckBox", "chkpos%d" % i, Label="Pos %d" % i, ImagePosition=i, ImageURL=png_file())
        C(f1, "RadioButton", "radpos%d" % i, Label="Pos %d" % i, ImagePosition=i, ImageURL=png_file())
        C(f1, "CommandButton", "btnpos%d" % i, Label="Pos %d" % i, ImagePosition=i, ImageAlign=i % 3, ImageURL=png_file())
    for i in range(4):
        C(f1, "ImageButton", "imgbtn%d" % i, ButtonType=enum("FormButtonType", ("PUSH", "SUBMIT", "RESET", "URL")[i]),
          ImageURL=png_file(), Tabstop=(i % 2 == 0), Enabled=(i != 1), Printable=(i != 2), TabIndex=10 + i, HelpText="img tip",
          TargetURL="http://example.org/i" if i == 3 else "", TargetFrame="_blank" if i == 3 else "", ScaleImage=(i % 2 == 0))
    for i, st in enumerate((0, 1, 2)):
        C(f1, "CheckBox", "chk%d" % i, Label="Check %d" % i, State=st, DefaultState=st, TriState=(i == 2), VisualEffect=i % 3,
          ImagePosition=i + 1 if i < 2 else 3, ImageAlign=i, RefValue="ref%d" % i, Tabstop=(i == 0), Enabled=(i != 1), Printable=(i != 2),
          TabIndex=20 + i, HelpText="chk tip", DataField="Author", InputRequired=False)
    for i in range(4):
        C(f1, "RadioButton", "rad%d" % i, Label="Radio %d" % i, State=(i % 2), DefaultState=(i % 2 == 0), RefValue="r%d" % i,
          VisualEffect=i % 3, ImagePosition=i + 1 if i < 3 else 0, ImageAlign=i % 3, Tabstop=(i == 1), Enabled=(i != 2),
          Printable=(i != 3), TabIndex=30 + i, HelpText="rad tip")
    C(f1, "GroupBox", "grp", Label="Group", Enabled=False, Printable=False, HelpText="grp tip")
    fx = C(f1, "FixedText", "fixed", Label="Fixed label", MultiLine=True, Enabled=False, Printable=False, HelpText="fx tip")
    C(f1, "FixedText", "fixed2", Label="Fixed label 2", MultiLine=False)
    for i in range(3):
        C(f1, "TextField", "text%d" % i, Text="abc%d" % i, DefaultText="def%d" % i, MaxTextLen=10 * (i + 1), ReadOnly=(i == 1),
          Enabled=(i != 2), Printable=(i != 0), Tabstop=(i % 2 == 0), TabIndex=40 + i, HelpText="txt tip", DataField="Title",
          ConvertEmptyToNull=(i == 0), InputRequired=(i != 1), **({"LabelControl": fx} if i == 0 else {}))
    C(f1, "TextField", "textarea", Text="line1\nline2", MultiLine=True, ReadOnly=True, MaxTextLen=50, DataField="Note",
      Enabled=False, Printable=False, Tabstop=True, ConvertEmptyToNull=True)
    C(f1, "TextField", "password", Text="secret", EchoChar=ord("*"), MaxTextLen=8, Tabstop=True)
    C(f1, "TextField", "richtext", RichText=True, MultiLine=True)
    for i in range(2):
        C(f1, "FormattedField", "fmt%d" % i, EffectiveValue=12.5, EffectiveDefault=1.5, EffectiveMin=0.0, EffectiveMax=100.0,
          TreatAsNumber=(i == 0), Spin=(i == 0), RepeatDelay=50, ReadOnly=(i == 1), MaxTextLen=12, Enabled=(i == 0), Printable=(i == 0),
          Tabstop=True, TabIndex=50 + i, HelpText="fmt tip", DataField="Number", ConvertEmptyToNull=(i == 1), InputRequired=(i == 0),
          StrictFormat=(i == 0))
    for i in range(2):
        C(f1, "NumericField", "num%d" % i, Value=3.0, DefaultValue=2.0, ValueMin=0.0, ValueMax=9.0, ValueStep=0.5, DecimalAccuracy=2,
          ShowThousandsSeparator=(i == 0), Spin=(i == 0), StrictFormat=(i == 1), ReadOnly=(i == 1), RepeatDelay=70, Enabled=(i == 0),
          Printable=(i == 0), Tabstop=True, TabIndex=55 + i, HelpText="num tip", DataField="N", ConvertEmptyToNull=(i == 1),
          InputRequired=(i == 0))
    C(f1, "CurrencyField", "cur", Value=3.0, DefaultValue=2.0, ValueMin=0.0, ValueMax=9.0, ValueStep=0.5, DecimalAccuracy=2,
      CurrencySymbol="EUR", PrependCurrencySymbol=True, Spin=True, ReadOnly=False)
    C(f1, "PatternField", "pat", Text="12-34", DefaultText="00-00", EditMask="NN-NN", LiteralMask="__-__", StrictFormat=True,
      ReadOnly=True, MaxTextLen=5, ConvertEmptyToNull=True, DataField="Pat", InputRequired=False)
    for i in range(2):
        C(f1, "DateField", "date%d" % i, Date=20240315, DefaultDate=20240101, DateMin=20000101, DateMax=20301231,
          DateFormat=i * 3, Dropdown=(i == 0), Spin=(i == 1), StrictFormat=(i == 0), ReadOnly=(i == 1), RepeatDelay=20, Enabled=(i == 0),
          Printable=(i == 0), Tabstop=(i == 1), TabIndex=60 + i, HelpText="date tip", DataField="D", ConvertEmptyToNull=(i == 1),
          InputRequired=(i == 0))
    for i in range(2):
        C(f1, "TimeField", "time%d" % i, Time=12300500, DefaultTime=8000000, TimeMin=0, TimeMax=23595900,
          TimeFormat=i * 2, Spin=(i == 0), StrictFormat=(i == 1), ReadOnly=(i == 1), RepeatDelay=30, Enabled=(i == 0), Printable=(i == 0),
          Tabstop=True, TabIndex=65 + i, HelpText="time tip", DataField="T", ConvertEmptyToNull=(i == 0), InputRequired=(i == 1))
    for i in range(2):
        C(f1, "ListBox", "list%d" % i, StringItemList=("one", "two", "three"), 
          SelectedItems=uno.Any("[]short", (1,)) if i == 0 else uno.Any("[]short", (0, 2)),
          DefaultSelection=uno.Any("[]short", (2,)), Dropdown=(i == 0), MultiSelection=(i == 1), LineCount=4 + i,
          ListSourceType=enum("ListSourceType", "VALUELIST"), BoundColumn=i, Enabled=(i == 0), Printable=(i == 0), Tabstop=(i == 1),
          TabIndex=70 + i, HelpText="list tip", DataField="L", InputRequired=(i == 0))
    for i, lst in enumerate(("TABLE", "QUERY", "SQL", "SQLPASSTHROUGH", "TABLEFIELDS")):
        C(f1, "ListBox", "listsrc%d" % i, ListSourceType=enum("ListSourceType", lst), ListSource=("biblio",), BoundColumn=1, DataField="X",
          Dropdown=(i % 2 == 0))
    for i, lst in enumerate(("VALUELIST", "TABLE", "QUERY", "SQL", "SQLPASSTHROUGH", "TABLEFIELDS")):
        C(f1, "ComboBox", "combo%d" % i, StringItemList=("alpha", "beta"), Text="alp", DefaultText="al", Dropdown=(i % 2 == 0),
          Autocomplete=(i % 2 == 1), LineCount=3 + i, MaxTextLen=20, ReadOnly=(i == 1), ListSourceType=enum("ListSourceType", lst),
          ListSource="biblio" if lst != "VALUELIST" else "", ConvertEmptyToNull=(i == 0), DataField="C", InputRequired=(i == 0),
          Enabled=(i != 2), Printable=(i != 3), Tabstop=(i == 4), TabIndex=80 + i, HelpText="combo tip")
    C(f1, "FileControl", "file", Text="/tmp/x.txt", DefaultText="/tmp/y.txt", Enabled=False, Printable=False, Tabstop=True, TabIndex=90,
      HelpText="file tip")
    C(f1, "DatabaseImageControl", "imgctl", ImageURL=png_file(), ReadOnly=True, Enabled=False, Printable=False, HelpText="imgctl tip", DataField="Pic",
      InputRequired=False, ScaleImage=True)
    C(f1, "HiddenControl", "hidden", HiddenValue="hv")
    C(f1, "ScrollBar", "scroll", ScrollValue=5, ScrollValueMin=0, ScrollValueMax=50, LineIncrement=2, BlockIncrement=10, Orientation=1,
      RepeatDelay=40, Enabled=False, Printable=False, HelpText="scr tip")
    C(f1, "ScrollBar", "scroll2", ScrollValue=1, ScrollValueMin=0, ScrollValueMax=10, Orientation=0)
    C(f1, "SpinButton", "spin", SpinValue=3, SpinValueMin=0, SpinValueMax=20, SpinIncrement=2, Orientation=1, RepeatDelay=60,
      Enabled=False, Printable=False, HelpText="spin tip")
    C(f1, "NavigationToolBar", "navbar")
    g = C(f1, "GridControl", "grid", Enabled=False, Printable=False, Tabstop=True, TabIndex=99, HelpText="grid tip")
    for i, ct in enumerate(("TextField", "CheckBox", "ComboBox", "ListBox", "DateField", "TimeField", "NumericField", "CurrencyField",
                            "PatternField", "FormattedField")):
        try:
            col = g.createColumn(ct)
            col.Label = "Col %d" % i
            col.Width = 1000 + i
            col.Align = i % 3
            col.Hidden = (i == 3)
            col.HelpText = "col tip"
            col.DataField = "F%d" % i
            g.insertByName("col%d" % i, col)
        except Exception as e:
            print("FAIL column", ct, repr(e)[:100])
            failed.append("column " + ct)
    m = C(f4, "TextField", "dyn")
    T = lambda: uno.createUnoStruct("com.sun.star.util.Time", 0, 3, 2, 1)
    dyn = [("dyn_string", "text"), ("dyn_bool", True), ("dyn_double", 2.5), ("dyn_long", 42), ("dyn_date", date(2024, 5, 6)),
           ("dyn_time", T()), ("dyn_datetime", dt()),
           ("dyn_seqs", uno.Any("[]string", ("a", "b"))), ("dyn_seqb", uno.Any("[]boolean", (True, False))),
           ("dyn_seqd", uno.Any("[]double", (1.5, 2.5))), ("dyn_seql", uno.Any("[]long", (1, 2))),
           ("dyn_seqt", uno.Any("[]com.sun.star.util.Time", (T(),))), ("dyn_seqdate", uno.Any("[]com.sun.star.util.Date", (date(2024, 5, 6),))),
           ("dyn_seqdt", uno.Any("[]com.sun.star.util.DateTime", (dt(),))),
           ]
    for nm, val in dyn:
        try:
            uno.invoke(m, "addProperty", (nm, 128, val))
        except Exception as e:
            print("FAIL dyn prop", nm, repr(e)[:100])
            failed.append("dyn " + nm)
    save(doc, "forms.odt")
    doc.close(True)



def doc_database():
    doc = new()
    w = W(doc)
    for n, (src, url, ctype) in enumerate((("Bibliography", "", 0), ("Bibliography", "", 1), ("Bibliography", "", 2), ("", "file:///tmp/t12_missing.odb", 0),
                                          ("", "file:///tmp/t12_missing.odb", 2))):
        m = doc.createInstance("com.sun.star.text.FieldMaster.Database")
        try:
            m.DataBaseName = src
            if url:
                m.DataBaseURL = url
            m.DataTableName = "biblio%d" % n
            m.DataColumnName = "Identifier"
            m.DataCommandType = ctype
        except Exception as e:
            print("FAIL master prop", n, repr(e)[:100])
        for fmtkind in (None, "number", "date"):
            for visible in (True, False):
                f = doc.createInstance("com.sun.star.text.TextField.Database")
                try:
                    f.attachTextFieldMaster(m)
                    f.Content = "<Identifier>"
                    f.IsVisible = visible
                    if fmtkind:
                        f.DataBaseFormat = False
                        f.NumberFormat = stdfmt(doc, fmtkind)
                    w.put(f)
                    w.s(" ")
                except Exception as e:
                    print("FAIL dbfield", n, fmtkind, repr(e)[:100])
                    failed.append("dbfield")
        w.para()
        for kind, extra in (("DatabaseName", dict(IsVisible=True)), ("DatabaseName", dict(IsVisible=False)),
                            ("DatabaseNextSet", dict(Condition="1==1")), ("DatabaseNumberOfSet", dict(Condition="2==2", SetNumber=3)),
                            ("DatabaseSetNumber", dict(SetNumber=5, NumberingType=4, IsVisible=True)),
                            ("DatabaseSetNumber", dict(SetNumber=6, NumberingType=1, IsVisible=False))):
            props = dict(DataBaseName=src, DataTableName="tab%d" % n, DataCommandType=ctype)
            if url:
                props["DataBaseURL"] = url
            props.update(extra)
            w.field(kind, "%s %d" % (kind, n), **props)
        w.para()
    save(doc, "database-fields.odt")
    doc.close(True)


def doc_redlines():
    doc = new()
    w = W(doc)
    for t in ("First paragraph of original text.", "Second paragraph with a word to delete.", "Third paragraph to format.",
              "Fourth paragraph restyled.", "Fifth paragraph removed entirely.", "Sixth paragraph removed too.", "Seventh paragraph."):
        w.s(t)
        w.para()
    w.s("end")
    doc.RecordChanges = True
    paras = doc.Text.createEnumeration()
    ps = []
    while paras.hasMoreElements():
        ps.append(paras.nextElement())

    def cur_in(p, skip, n):
        c = doc.Text.createTextCursorByRange(p.Start)
        c.goRight(skip, False)
        if n:
            c.goRight(n, True)
        return c

    cur_in(ps[0], 6, 0).String = "inserted "
    c = cur_in(ps[1], 22, 4)
    c.String = ""
    c = cur_in(ps[2], 6, 9)
    c.CharWeight = 150
    c.CharPosture = 1
    c = cur_in(ps[3], 0, 6)
    ps[3].ParaStyleName = "Heading 1"
    c = doc.Text.createTextCursorByRange(ps[4].Start)
    c.gotoRange(ps[5].End, True)
    c.String = ""
    c = cur_in(ps[6], 0, 7)
    c.CharColor = 0xff0000
    t = doc.createInstance("com.sun.star.text.TextTable")
    t.initialize(2, 2)
    doc.Text.insertTextContent(doc.Text.End, t, False)
    doc.RecordChanges = True
    save(doc, "redlines.odt")
    doc.RecordChanges = False
    save(doc, "redlines-notrack.odt")
    doc.close(True)



def doc_table_fields():
    doc = new()
    w = W(doc)
    t = w.mk("com.sun.star.text.TextTable")
    t.initialize(4, 3)
    w.put(t)
    for r in range(3):
        for c in range(2):
            t.getCellByPosition(c, r).Value = r * 2 + c + 1
    for n, (content, fmt, show) in enumerate((("sum <A1:A3>", "number", False), ("=<A1>+<B1>", "currency", True), ("mean <A1:B3>", None, False),
                                                ("=<A2>*2", "percent", False))):
        cell = t.getCellByPosition(2, n)
        f = doc.createInstance("com.sun.star.text.TextField.TableFormula")
        f.Content = content
        f.IsShowFormula = show
        if fmt:
            f.NumberFormat = stdfmt(doc, fmt)
        cell.Text.insertTextContent(cell.Text.createTextCursor(), f, False)
    t.getCellByPosition(0, 3).Formula = "=sum <A1:A3>"
    t.getCellByPosition(1, 3).Formula = "=<A1>+<B1>"
    t.getCellByPosition(1, 3).NumberFormat = stdfmt(doc, "currency")
    save(doc, "table-fields.odt")
    doc.close(True)


def doc_master():
    doc = new("private:factory/swriter/GlobalDocument")
    w = W(doc)
    w.s("master document body")
    w.para()
    for n in range(2):
        sec = w.mk("com.sun.star.text.TextSection", Name="Linked%d" % n)
        sec.FileLink = uno.createUnoStruct("com.sun.star.text.SectionFileLink", "file:///tmp/t12_missing_sub%d.odt" % n, "writer8")
        try:
            w.text.insertTextContent(w.cur, sec, False)
        except Exception as e:
            print("FAIL master section", n, repr(e)[:100])
            failed.append("master section %d" % n)
        w.cur.gotoEnd(False)
        w.para()
    idx = w.mk("com.sun.star.text.ContentIndex", Title="Master contents")
    w.put(idx)
    save(doc, "master-doc.odm", "writerglobal8")
    save(doc, "master-doc-as-text.odt", "writerglobal8_writer")
    doc.close(True)


def doc_settings():
    doc = new()
    w = W(doc)
    w.s("settings")
    st = doc.createInstance("com.sun.star.document.Settings")
    base = [("LinkUpdateMode", 0), ("FieldAutoUpdate", False), ("ChartAutoUpdate", False), ("AddParaTableSpacing", False),
            ("AddParaTableSpacingAtStart", False), ("AlignTabStopPosition", False), ("IsKernAsianPunctuation", True),
            ("CharacterCompressionType", 2), ("ApplyUserData", False), ("SaveGlobalDocumentLinks", True),
            ("CurrentDatabaseDataSource", "Bibliography"), ("CurrentDatabaseCommand", "biblio"), ("CurrentDatabaseCommandType", 1),
            ("SaveVersionOnClose", True), ("UpdateFromTemplate", False), ("PrinterIndependentLayout", 1), ("IsLabelDocument", True),
            ("AddFrameOffsets", True), ("AddExternalLeading", False), ("UseOldNumbering", True), ("OutlineLevelYieldsNumbering", True),
            ("AllowPrintJobCancel", False), ("UseFormerLineSpacing", True), ("AddParaSpacingToTableCells", False),
            ("UseFormerObjectPositioning", True), ("UseFormerTextWrapping", True), ("ConsiderTextWrapOnObjPos", True),
            ("IgnoreFirstLineIndentInNumbering", True), ("DoNotJustifyLinesWithManualBreak", True), ("DoNotResetParaAttrsForNumFont", True),
            ("TableRowKeep", True), ("IgnoreTabsAndBlanksForLineCalculation", True), ("LoadReadonly", True),
            ("DoNotCaptureDrawObjsOnPage", True), ("ClipAsCharacterAnchoredWriterFlyFrames", True), ("UnxForceZeroExtLeading", True),
            ("UseOldPrinterMetrics", True), ("TabsRelativeToIndent", False), ("ProtectForm", True),
            ("TabAtLeftIndentForParagraphsInList", True), ("MathBaselineAlignment", False),
            ("RedlineProtectionKey", uno.Any("[]byte", (1, 2, 3)))]
    for k, v in base:
        try:
            if isinstance(v, uno.Any):
                uno.invoke(st, "setPropertyValue", (k, v))
            else:
                st.setPropertyValue(k, v)
        except Exception as e:
            print("settings prop", k, repr(e)[:80])
    save(doc, "settings.odt")
    doc.close(True)


def doc_web():
    doc = new("private:factory/swriter/web")
    w = W(doc)
    w.s("web doc with field ")
    w.field("PageCount", NumberingType=4)
    w.para()
    f = w.mk("com.sun.star.text.TextField.Annotation", Author="W", Content="web note")
    w.put(f)
    save(doc, "web-fields.odt", "writerweb8_writer")
    doc.close(True)


def run_all():
    attempt("fields-docinfo", doc_fields_info)
    attempt("fields-misc", doc_fields_misc)
    attempt("variables", doc_variables)
    attempt("indexes", doc_indexes)
    attempt("objects", doc_objects)
    attempt("bibliography", doc_bibliography)
    attempt("refs-notes", doc_refs_notes)
    attempt("notes-variants", doc_notes_variants)
    attempt("annotations", doc_annotations)
    attempt("hyperlinks-ruby", doc_hyperlinks)
    attempt("lists-outline", doc_lists)
    attempt("structure", doc_structure)
    attempt("linenumbers", doc_linenumbers)
    attempt("forms", doc_forms)
    attempt("database-fields", doc_database)
    attempt("redlines", doc_redlines)
    attempt("table-fields", doc_table_fields)
    attempt("master", doc_master)
    attempt("settings", doc_settings)
    attempt("web", doc_web)


def set_compat(flag):
    cp = desk_ctx.ServiceManager.createInstanceWithContext("com.sun.star.configuration.ConfigurationProvider", desk_ctx)
    ca = cp.createInstanceWithArguments("com.sun.star.configuration.ConfigurationUpdateAccess",
                                        (pv("nodepath", "/org.openoffice.Office.Common/Save/Document"),))
    ca.setPropertyValue("SaveBackwardCompatibleODF", flag)
    ca.commitChanges()


run_all()
SUF = "-nocompat"
# SaveBackwardCompatibleODF=true (the default) writes the ODF 1.1 element names; the second pass shows what the 1.2 names would be
try:
    set_compat(False)
    for n, fn in (("fields-misc", doc_fields_misc), ("refs-notes", doc_refs_notes), ("variables", doc_variables),
                  ("indexes", doc_indexes), ("lists-outline", doc_lists), ("structure", doc_structure)):
        attempt(n + SUF, fn)
except Exception as e:
    print("FAIL nocompat pass", repr(e)[:150])
    failed.append("nocompat")
finally:
    try:
        set_compat(True)
    except Exception as e:
        print("FAIL restoring SaveBackwardCompatibleODF", repr(e)[:100])
sys.exit(1 if failed else 0)
