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
# usage: runpy.sh fixtures/styles-text/make.py <port> <outdir> [section ...]
# Character, paragraph, list, section, ruby and number-format properties set to each enumerated value, in styles and as direct formatting. Saves with the office's configured ODF version: run it against the strict and extended profiles.
import sys, os, traceback, uno
from unohelp import connect, desktop, pv

port, out = int(sys.argv[1]), os.path.abspath(sys.argv[2])
only = sys.argv[3:]
os.makedirs(out, exist_ok=True)
desk = desktop(connect(port))
failed = []
SECTIONS = []


def section(fn):
    SECTIONS.append(fn)
    return fn


def new(factory):
    return desk.loadComponentFromURL(factory, "_blank", 0, (pv("Hidden", True),))


def save(doc, name, filt):
    try:
        doc.storeToURL(uno.systemPathToFileUrl(os.path.join(out, name)), (pv("FilterName", filt),))
        print("saved", name, filt)
    except Exception as e:
        failed.append(name)
        print("FAIL", name, filt, type(e).__name__, repr(e)[:200], getattr(e, "Message", ""))


def setp(obj, name, value, tag=""):
    try:
        if isinstance(value, uno.Any):
            uno.invoke(obj, "setPropertyValue", (name, value))
        else:
            obj.setPropertyValue(name, value)
        return True
    except Exception as e:
        print("WARN", tag, name, repr(value)[:60], type(e).__name__, str(e)[:80].replace("\n", " "))
        return False


def setall(obj, props, tag=""):
    for k, v in props.items():
        setp(obj, k, v, tag)


def const(name):
    return uno.getConstantByName("com.sun.star.%s" % name)


def enum(t, v):
    return uno.Enum("com.sun.star.%s" % t, v)


def consts(prefix, names):
    return [const("%s.%s" % (prefix, n)) for n in names]


def run(name, fn):
    if only and name not in only:
        return
    try:
        fn()
    except Exception as e:
        failed.append(name)
        print("FAIL", name, type(e).__name__, repr(e)[:300], traceback.format_exc().splitlines()[-4:-1])


def norm(v):
    if isinstance(v, uno.Enum):
        return (v.typeName, v.value)
    if isinstance(v, float):
        return round(v, 3)
    if isinstance(v, (tuple, list)):
        return tuple(norm(x) for x in v)
    if type(v).__name__ == "pyuno":
        return repr(v)
    return v


def reload(name):
    return desk.loadComponentFromURL(uno.systemPathToFileUrl(os.path.join(out, name)), "_blank", 0, (pv("Hidden", True), pv("ReadOnly", True), pv("MacroExecutionMode", 0)))


def readback(name, family, specs):
    try:
        doc = reload(name)
        fam = doc.StyleFamilies.getByName(family)
        bad = tot = 0
        for sname, props in specs:
            try:
                st = fam.getByName(sname)
            except Exception:
                print("RT-MISSING", name, sname)
                continue
            for k, v in props.items():
                if k.startswith("__"):
                    continue
                tot += 1
                try:
                    got = st.getPropertyValue(k)
                except Exception as e:
                    print("RT-NOPROP", name, sname, k)
                    bad += 1
                    continue
                if norm(got) != norm(v):
                    bad += 1
                    print("RT-DIFF", name, sname, k, "set", norm(v), "got", norm(got))
        print("RT-SUMMARY", name, family, "values", tot, "differing", bad)
        doc.close(True)
    except Exception as e:
        print("RT-FAIL", name, type(e).__name__, str(e)[:100])


class Writer:
    def __init__(self):
        self.doc = new("private:factory/swriter")
        self.items = []
        self.n = 0
        self.specs = {"CharacterStyles": [], "ParagraphStyles": []}

    def style(self, family, props, parent=None, tag=""):
        self.n += 1
        name = "%s%d" % (tag or family[:1], self.n)
        s = self.doc.createInstance("com.sun.star.style.%s" % ("CharacterStyle" if family == "CharacterStyles" else "ParagraphStyle"))
        self.doc.StyleFamilies.getByName(family).insertByName(name, s)
        if parent:
            s.ParentStyle = parent
        setall(s, props, name)
        self.specs[family].append((name, props))
        return name

    def para(self, text, pstyle=None, cstyle=None, props=None):
        self.items.append((text, pstyle, cstyle, props))

    def char_style(self, label, props):
        self.para(label, cstyle=self.style("CharacterStyles", props, tag="C"))

    def par_style(self, label, props, parent=None):
        self.para(label, pstyle=self.style("ParagraphStyles", props, parent, tag="P"))

    def flush(self):
        t = self.doc.Text
        c = t.createTextCursor()
        for k, i in enumerate(self.items):
            if k:
                t.insertControlCharacter(c, const("text.ControlCharacter.PARAGRAPH_BREAK"), False)
            t.insertString(c, i[0], False)
        en = t.createEnumeration()
        for (label, ps, cs, props) in self.items:
            p = en.nextElement()
            if ps:
                p.ParaStyleName = ps
            if cs:
                p.CharStyleName = cs
            if props:
                setall(p, props, label)
        return self.doc


def W():
    return Writer()


WEIGHTS = [0, 10, 50, 60, 75, 90, 100, 110, 150, 175, 200, 250]
SLANTS = ["NONE", "OBLIQUE", "ITALIC", "DONTKNOW", "REVERSE_OBLIQUE", "REVERSE_ITALIC"]
FAMILIES = ["DONTKNOW", "DECORATIVE", "MODERN", "ROMAN", "SCRIPT", "SWISS", "SYSTEM"]
PITCHES = ["DONTKNOW", "FIXED", "VARIABLE"]
CHARSETS = ["DONTKNOW", "ANSI", "MAC", "IBMPC_437", "IBMPC_850", "IBMPC_860", "IBMPC_861", "IBMPC_863", "IBMPC_865", "SYSTEM", "SYMBOL"]
UNDERLINES = list(range(19))
STRIKES = list(range(7))


@section
def writer_char():
    def build():
        w = W()
        for suf in ("", "Asian", "Complex"):
            for v in WEIGHTS:
                w.char_style("weight%s %s" % (suf, v), {"CharWeight" + suf: float(v)})
            for s in SLANTS:
                w.char_style("posture%s %s" % (suf, s), {"CharPosture" + suf: enum("awt.FontSlant", s)})
            for f in FAMILIES:
                w.char_style("family%s %s" % (suf, f), {"CharFontFamily" + suf: const("awt.FontFamily." + f)})
            for p in PITCHES:
                w.char_style("pitch%s %s" % (suf, p), {"CharFontPitch" + suf: const("awt.FontPitch." + p)})
            for c in CHARSETS:
                w.char_style("charset%s %s" % (suf, c), {"CharFontCharSet" + suf: const("awt.CharSet." + c)})
            for f in FAMILIES:
                for p in PITCHES[1:]:
                    w.char_style("named%s %s %s" % (suf, f, p), {"CharFontName" + suf: "T12Font%s%s%s" % (suf, f, p), "CharFontFamily" + suf: const("awt.FontFamily." + f), "CharFontPitch" + suf: const("awt.FontPitch." + p), "CharFontStyleName" + suf: "Regular"})
            w.char_style("diffheight%s" % suf, {"CharDiffHeight" + suf: 2.0})
            w.char_style("propheight%s" % suf, {"CharPropHeight" + suf: 150})
            w.char_style("height%s" % suf, {"CharHeight" + suf: 17.5})
        for v in UNDERLINES:
            w.char_style("underline %d" % v, {"CharUnderline": v})
            w.char_style("underline-col %d" % v, {"CharUnderline": v, "CharUnderlineHasColor": True, "CharUnderlineColor": 0xFF0000})
            w.char_style("underline-word %d" % v, {"CharUnderline": v, "CharWordMode": True})
            w.char_style("overline %d" % v, {"CharOverline": v})
            w.char_style("overline-col %d" % v, {"CharOverline": v, "CharOverlineHasColor": True, "CharOverlineColor": 0x00FF00})
            w.char_style("overline-word %d" % v, {"CharOverline": v, "CharWordMode": True})
        for v in STRIKES:
            w.char_style("strike %d" % v, {"CharStrikeout": v})
            w.char_style("strike-word %d" % v, {"CharStrikeout": v, "CharWordMode": True})
        for v in range(5):
            w.char_style("casemap %d" % v, {"CharCaseMap": v})
        for v in range(3):
            w.char_style("relief %d" % v, {"CharRelief": v})
        for v in (0, 1, 2, 3, 4, 11, 12, 13, 14):
            w.char_style("emphasis %d" % v, {"CharEmphasis": v})
        for v in (-30, 0, 40):
            w.char_style("kerning %d" % v, {"CharKerning": v})
        for v in (True, False):
            w.char_style("autokern %s" % v, {"CharAutoKerning": v})
            w.char_style("flash %s" % v, {"CharFlash": v})
            w.char_style("hidden %s" % v, {"CharHidden": v})
            w.char_style("shadowed %s" % v, {"CharShadowed": v})
            w.char_style("contoured %s" % v, {"CharContoured": v})
            w.char_style("fitline %s" % v, {"CharRotation": 900, "CharRotationIsFitToLine": v})
            w.char_style("combine %s" % v, {"CharCombineIsOn": v, "CharCombinePrefix": "<", "CharCombineSuffix": ">"})
            w.char_style("transparent-back %s" % v, {"CharBackColor": 0xFFFF00, "CharBackTransparent": v})
        for v in (50, 100, 150):
            w.char_style("scalewidth %d" % v, {"CharScaleWidth": v})
        for e, h in ((33, 58), (-33, 58), (101, 100), (0, 100)):
            w.char_style("escapement %d" % e, {"CharEscapement": e, "CharEscapementHeight": h})
        for r in (0, 900, 2700):
            w.char_style("rotation %d" % r, {"CharRotation": r})
        w.char_style("color auto", {"CharColor": -1})
        w.char_style("color red", {"CharColor": 0xFF0000})
        for lang, c in (("en", "US"), ("de", "DE"), ("fr", "CA"), ("ja", "JP"), ("zh", "CN"), ("ko", "KR"), ("ar", "SA"), ("th", "TH"), ("he", "IL"), ("hi", "IN")):
            loc = uno.createUnoStruct("com.sun.star.lang.Locale", lang, c, "")
            w.char_style("locale %s-%s" % (lang, c), {"CharLocale": loc, "CharLocaleAsian": loc, "CharLocaleComplex": loc})
        doc = w.flush()
        save(doc, "writer-char-styles.odt", "writer8")
        doc.close(True)
        readback("writer-char-styles.odt", "CharacterStyles", w.specs["CharacterStyles"])
    build()


def png(name):
    import zlib, struct
    d = os.path.join(os.path.dirname(out), "work")
    os.makedirs(d, exist_ok=True)
    path = os.path.join(d, name)
    raw = b"".join(b"\x00" + b"".join(bytes([(x * 30) % 256, (y * 30) % 256, 128]) for x in range(8)) for y in range(8))

    def chunk(t, b):
        c = struct.pack(">I", len(b)) + t + b
        return c + struct.pack(">I", zlib.crc32(t + b) & 0xFFFFFFFF)
    with open(path, "wb") as f:
        f.write(b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", 8, 8, 8, 2, 0, 0, 0)) + chunk(b"IDAT", zlib.compress(raw)) + chunk(b"IEND", b""))
    return uno.systemPathToFileUrl(path)


def border(outer, inner=0, dist=0, color=0):
    return uno.createUnoStruct("com.sun.star.table.BorderLine", color, inner, outer, dist)


def tab(pos, align, fill=" ", dec="."):
    t = uno.createUnoStruct("com.sun.star.style.TabStop")
    t.Position, t.Alignment, t.FillChar, t.DecimalChar = pos, enum("style.TabAlign", align), uno.Char(fill), uno.Char(dec)
    return t


def tabs(*ts):
    if len(ts) == 1 and isinstance(ts[0], (tuple, list)) or len(ts) == 1 and hasattr(ts[0], "__next__"):
        ts = tuple(ts[0])
    return uno.Any("[]com.sun.star.style.TabStop", tuple(ts))


def breaks():
    return [enum("style.BreakType", n) for n in ("NONE", "COLUMN_BEFORE", "COLUMN_AFTER", "COLUMN_BOTH", "PAGE_BEFORE", "PAGE_AFTER", "PAGE_BOTH")]


@section
def writer_para():
    w = W()
    for v in range(5):
        w.par_style("adjust %d" % v, {"ParaAdjust": v})
        w.par_style("lastline %d" % v, {"ParaAdjust": 3, "ParaLastLineAdjust": v})
        w.par_style("vertalign %d" % v, {"ParaVertAlignment": v})
        w.par_style("writingmode %d" % v, {"WritingMode": v})
        w.para("direct adjust %d" % v, props={"ParaAdjust": v})
        w.para("direct writingmode %d" % v, props={"WritingMode": v})
        w.para("direct vertalign %d" % v, props={"ParaVertAlignment": v})
    for v in (True, False):
        for p in ("ParaExpandSingleWord", "ParaIsAutoFirstLineIndent", "SnapToGrid", "ParaRegisterModeActive", "ParaIsHyphenation", "ParaIsHangingPunctuation", "ParaIsCharacterDistance", "ParaIsForbiddenRules", "ParaSplit", "ParaKeepTogether", "ParaLineNumberCount", "FontIndependentLineSpacing", "ParaIsConnectBorder"):
            w.par_style("%s %s" % (p, v), {p: v})
            w.para("direct %s %s" % (p, v), props={p: v})
    for i, b in enumerate(breaks()):
        w.par_style("break %d" % i, {"BreakType": b})
        w.para("direct break %d" % i, props={"BreakType": b})
    w.par_style("pagedesc", {"PageDescName": "Standard", "PageNumberOffset": 3})
    w.par_style("pagedesc-break", {"BreakType": enum("style.BreakType", "PAGE_BEFORE"), "PageDescName": "Standard", "PageNumberOffset": 5})
    w.par_style("linenumber", {"ParaLineNumberCount": True, "ParaLineNumberStartValue": 7})
    w.par_style("hyphen", {"ParaIsHyphenation": True, "ParaHyphenationMaxLeadingChars": 3, "ParaHyphenationMaxTrailingChars": 4, "ParaHyphenationMaxHyphens": 2})
    w.par_style("orphans", {"ParaOrphans": 3, "ParaWidows": 4})
    w.par_style("tabs-all", {"ParaTabStops": tabs(tab(1000 * (i + 1), a, f, d) for i, (a, f, d) in enumerate((("LEFT", " ", "."), ("CENTER", ".", "."), ("RIGHT", "-", "."), ("DECIMAL", "_", ","), ("DECIMAL", " ", "."), ("DECIMAL", "*", "x"), ("DEFAULT", " ", "."))))})
    w.para("direct tabs", props={"ParaTabStops": tabs(tab(1500, "DECIMAL", ".", ","), tab(3000, "RIGHT", "-"))})
    w.par_style("tabstopdistance", {"TabStopDistance": 1500})
    for mode, h in ((0, 150), (1, 600), (2, 200), (3, 500)):
        ls = uno.createUnoStruct("com.sun.star.style.LineSpacing", mode, h)
        w.par_style("linespacing %d" % mode, {"ParaLineSpacing": ls})
        w.para("direct linespacing %d" % mode, props={"ParaLineSpacing": ls})
    w.par_style("margins", {"ParaLeftMargin": 1000, "ParaRightMargin": 800, "ParaTopMargin": 300, "ParaBottomMargin": 200, "ParaFirstLineIndent": 500})
    w.par_style("margins-rel", {"ParaLeftMarginRelative": 10, "ParaRightMarginRelative": 10, "ParaTopMarginRelative": 10, "ParaBottomMarginRelative": 10, "ParaFirstLineIndentRelative": 10})
    w.par_style("margins-same", {"ParaLeftMargin": 1000, "ParaRightMargin": 1000, "ParaTopMargin": 1000, "ParaBottomMargin": 1000})
    w.par_style("back", {"ParaBackColor": 0x99CCFF})
    w.par_style("back-transparent", {"ParaBackTransparent": True})
    url = png("para.png")
    for loc in ("NONE", "LEFT_TOP", "MIDDLE_TOP", "RIGHT_TOP", "LEFT_MIDDLE", "MIDDLE_MIDDLE", "RIGHT_MIDDLE", "LEFT_BOTTOM", "MIDDLE_BOTTOM", "RIGHT_BOTTOM", "AREA", "TILED"):
        w.par_style("backgraphic %s" % loc, {"ParaBackGraphicURL": url, "ParaBackGraphicLocation": enum("style.GraphicLocation", loc), "ParaBackGraphicFilter": "png"})
        w.par_style("backgraphic-nofilter %s" % loc, {"ParaBackGraphicURL": url, "ParaBackGraphicLocation": enum("style.GraphicLocation", loc)})
    for i, (o, n, d) in enumerate(((20, 0, 0), (20, 20, 20), (50, 30, 40), (100, 0, 0), (35, 35, 35))):
        b = border(o, n, d, 0x336699)
        w.par_style("border-all %d" % i, {"LeftBorder": b, "RightBorder": b, "TopBorder": b, "BottomBorder": b, "LeftBorderDistance": 100, "RightBorderDistance": 100, "TopBorderDistance": 100, "BottomBorderDistance": 100})
        w.par_style("border-left-only %d" % i, {"LeftBorder": b, "LeftBorderDistance": 120})
        w.par_style("border-lr %d" % i, {"LeftBorder": b, "RightBorder": border(o + 10, n, d)})
        w.par_style("border-tb %d" % i, {"TopBorder": b, "BottomBorder": border(o + 10, n, d), "ParaIsConnectBorder": False})
        w.par_style("border-distance %d" % i, {"LeftBorder": b, "RightBorder": b, "TopBorder": b, "BottomBorder": b, "LeftBorderDistance": 50 * i, "RightBorderDistance": 60, "TopBorderDistance": 70, "BottomBorderDistance": 80})
    w.par_style("border-inner-only", {"LeftBorder": border(0, 20, 0, 0x336699), "TopBorder": border(0, 20, 30, 0x336699)})
    for loc in ("NONE", "TOP_LEFT", "TOP_RIGHT", "BOTTOM_LEFT", "BOTTOM_RIGHT"):
        sh = uno.createUnoStruct("com.sun.star.table.ShadowFormat", enum("table.ShadowLocation", loc), 100, False, 0x808080)
        w.par_style("shadow %s" % loc, {"ParaShadowFormat": sh})
    dc = uno.createUnoStruct("com.sun.star.style.DropCapFormat", 3, 2, 200)
    cs = w.style("CharacterStyles", {"CharWeight": 150.0}, tag="C")
    w.par_style("dropcap", {"DropCapFormat": dc, "DropCapWholeWord": False, "DropCapCharStyleName": cs})
    w.par_style("dropcap-word", {"DropCapFormat": dc, "DropCapWholeWord": True})
    w.par_style("outline", {"OutlineLevel": 2})
    w.par_style("defaultoutline", {"OutlineLevel": 0})
    w.para("drop cap text word", pstyle="P%d" % w.n)
    doc = w.flush()
    save(doc, "writer-para-styles.odt", "writer8")
    doc.close(True)
    readback("writer-para-styles.odt", "ParagraphStyles", w.specs["ParagraphStyles"])


def level(rules, i, **kw):
    cur = {p.Name: p.Value for p in rules.getByIndex(i)}
    cur.update(kw)
    uno.invoke(rules, "replaceByIndex", (i, uno.Any("[]com.sun.star.beans.PropertyValue", tuple(pv(k, v) for k, v in cur.items()))))


def font(name, family=0, pitch=0, charset=0, style=""):
    f = uno.createUnoStruct("com.sun.star.awt.FontDescriptor")
    f.Name, f.StyleName, f.Family, f.Pitch, f.CharSet = name, style, family, pitch, charset
    return f


@section
def writer_list():
    w = W()
    url = png("bullet.png")
    n = 0
    sz = uno.createUnoStruct("com.sun.star.awt.Size", 500, 400)
    for adj in (3, 1, 2):
        for typ in (0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90) if adj == 3 else (4, 6, 8):
            try:
                st = w.doc.createInstance("com.sun.star.style.NumberingStyle")
                n += 1
                w.doc.StyleFamilies.getByName("NumberingStyles").insertByName("N%d" % n, st)
                r = st.NumberingRules
                for lv in range(3):
                    kw = {"NumberingType": typ, "Adjust": adj, "StartWith": 3 + lv, "ParentNumbering": lv + 1, "Prefix": "(", "Suffix": ")", "PositionAndSpaceMode": lv % 2, "LabelFollowedBy": lv, "IndentAt": 1000 * (lv + 1), "FirstLineIndent": -500, "ListtabStopPosition": 1500 * (lv + 1)}
                    if typ == 6:
                        kw.update({"BulletChar": chr(0x2022 + lv), "BulletFont": font("OpenSymbol", lv, lv % 3, 0, "Regular"), "BulletRelSize": 80, "BulletColor": 0xFF0000})
                    if typ == 8:
                        kw.update({"GraphicURL": url, "GraphicSize": sz, "VertOrient": (adj * 3 + lv * 4) % 10})
                    level(r, lv, **kw)
                st.NumberingRules = r
                w.para("list style %d type %d adj %d" % (n, typ, adj), props={"NumberingStyleName": "N%d" % n})
            except Exception as e:
                print("WARN list", typ, adj, type(e).__name__, str(e)[:80])
    for vo in range(10):
        st = w.doc.createInstance("com.sun.star.style.NumberingStyle")
        n += 1
        w.doc.StyleFamilies.getByName("NumberingStyles").insertByName("N%d" % n, st)
        r = st.NumberingRules
        for lv in range(2):
            level(r, lv, NumberingType=8, GraphicURL=url, GraphicSize=sz, VertOrient=(vo + lv) % 10, PositionAndSpaceMode=lv)
        st.NumberingRules = r
        w.para("image bullet vertorient %d" % vo, props={"NumberingStyleName": "N%d" % n})
    for vo in range(10):
        r = w.doc.ChapterNumberingRules
        for lv in range(10):
            level(r, lv, NumberingType=(vo + lv) % 8, Adjust=(1, 2, 3)[lv % 3], PositionAndSpaceMode=vo % 2, StartWith=2)
    doc = w.flush()
    save(doc, "writer-list-styles.odt", "writer8")
    doc.close(True)


@section
def writer_ruby():
    w = W()
    for adj in range(5):
        for above in (True, False):
            w.para("ruby base %d %s" % (adj, above))
    doc = w.flush()
    en = doc.Text.createEnumeration()
    k = 0
    while en.hasMoreElements():
        p = en.nextElement()
        c = doc.Text.createTextCursorByRange(p)
        adj, above = divmod(k, 2)
        setp(c, "RubyText", "ru", "ruby")
        setp(c, "RubyAdjust", adj, "ruby")
        setp(c, "RubyIsAbove", above == 0, "ruby")
        k += 1
    save(doc, "writer-ruby.odt", "writer8")
    doc.close(True)


@section
def writer_section():
    doc = new("private:factory/swriter")
    url = png("section.png")
    cnt = 0
    for wm in (0, 1, 2, 3, 4):
        for sep in (True, False):
            for balance in (True, False):
                cnt += 1
                sec = doc.createInstance("com.sun.star.text.TextSection")
                c = doc.Text.createTextCursor()
                c.gotoEnd(False)
                doc.Text.insertString(c, "section %d" % cnt, False)
                c.goLeft(len("section %d" % cnt), True)
                doc.Text.insertTextContent(c, sec, True)
                setall(sec, {"WritingMode": wm, "DontBalanceTextColumns": balance, "SectionLeftMargin": 100 * cnt, "SectionRightMargin": 50 * cnt, "EditInReadonly": sep, "BackColor": 0xEEEEEE if sep else -1, "IsProtected": balance and sep}, "section")
                cols = sec.TextColumns
                cols.setColumnCount(2 + (cnt % 2))
                if sep:
                    setall(cols, {"SeparatorLineIsOn": True, "SeparatorLineWidth": 10 * cnt, "SeparatorLineColor": 0x333333, "SeparatorLineRelativeHeight": 50 + cnt, "SeparatorLineVerticalAlignment": enum("style.VerticalAlignment", ("TOP", "MIDDLE", "BOTTOM")[cnt % 3])}, "cols")
                sec.TextColumns = cols
                c.gotoEnd(False)
                doc.Text.insertControlCharacter(c, const("text.ControlCharacter.PARAGRAPH_BREAK"), False)
    for loc in ("LEFT_TOP", "AREA", "TILED", "MIDDLE_MIDDLE"):
        sec = doc.createInstance("com.sun.star.text.TextSection")
        c = doc.Text.createTextCursor()
        c.gotoEnd(False)
        doc.Text.insertString(c, "bg %s" % loc, False)
        c.goLeft(len("bg %s" % loc), True)
        doc.Text.insertTextContent(c, sec, True)
        setall(sec, {"BackGraphicURL": url, "BackGraphicLocation": enum("style.GraphicLocation", loc), "BackGraphicFilter": "png"}, "section")
        c.gotoEnd(False)
        doc.Text.insertControlCharacter(c, const("text.ControlCharacter.PARAGRAPH_BREAK"), False)
    save(doc, "writer-sections.odt", "writer8")
    doc.close(True)


LOCALES = [("en", "US"), ("de", "DE"), ("ja", "JP"), ("ar", "SA"), ("th", "TH"), ("he", "IL"), ("zh", "TW"), ("ko", "KR"), ("fr", "FR")]

FORMATS = [
    ("en", "US", ["YYYY-MM-DD", "D.M.YY", "MMM D, YYYY", "MMMM D", "NN, DD MMMM YYYY", "NNN D", "NNNN", "DD/MM/YYYY HH:MM:SS", "MM/DD/YY HH:MM AM/PM",
                  "HH:MM:SS", "[HH]:MM:SS", "HH:MM:SS.00", "HH:MM AM/PM", "[MM]:SS", "[SS]", "[HH]:MM", "MM:SS.0", "YY", "YYYY", "MM", "MMM", "MMMM", "MMMMM", "D", "DD", "NN", "NNN", "NNNN",
                  "Q", "QQ", "\"Q\"Q YYYY", "WW", "\"Week \"WW", "YYYY \"Q\"Q WW", "G", "GG", "GGG", "E", "EE", "R", "RR", "GGGE\"/\"M\"/\"D", "GE.M.D", "RR/MM/DD",
                  "[~buddhist]YYYY-MM-DD", "[~buddhist]D MMMM YYYY NNNN", "[~buddhist]WW", "[~buddhist]Q", "[~buddhist]GGGEE", "[~buddhist]MMM", "[~buddhist]NN",
                  "[~hijri]D/M/YYYY", "[~hijri]D MMMM YYYY NNN", "[~hijri]QQ", "[~hijri]WW", "[~hijri]GGG E", "[~hijri]NNNN",
                  "[~gengou]GGGE.M.D", "[~gengou]GE\"年\"M\"月\"D\"日\"", "[~gengou]EE", "[~gengou]D MMMM", "[~gengou]WW", "[~gengou]Q",
                  "[~jewish]D MMMM YYYY", "[~jewish]WW", "[~jewish]QQ", "[~jewish]NNN D",
                  "[~ROC]RR/MM/DD", "[~ROC]GGG R", "[~ROC]EE", "[~ROC]NNN", "[~ROC]Q", "[~ROC]WW",
                  "[~gregorian]YYYY-MM-DD", "[~julian]YYYY-MM-DD", "[~persian]YYYY/MM/DD", "[~dangi]YYYY-MM-DD", "[~indian]YYYY-MM-DD", "[~coptic]YYYY-MM-DD", "[~ethiopic]YYYY-MM-DD",
                  "[NatNum1]YYYY-MM-DD", "[NatNum2][$-411]YYYY/MM/DD", "[NatNum3]HH:MM", "[DBNum1][$-804]YYYY\"年\"M\"月\"", "[DBNum2][$-804]YYYY", "[DBNum3][$-411]D", "[NatNum1]NNNN",
                  "0", "0.00", "#,##0", "#,##0.00", "0.0000", "00000", "#", "#.##", "0.00E+00", "#,##0.0E+000", "##0.0E+0", "0E+00", "0.0E-00", "0%", "0.00%", "#,##0%", "0.0E+00%",
                  "# ?/?", "# ??/??", "# ???/???", "#,### ?/?", "?/?", "# ?/16", "# ?/8", "# ??/100", "#,##0 ?/4", "0,", "0,,", "#,##0,", "#,##0,,", "0.0,,\"M\"", "0.0,\"k\"", "#,##0.00,",
                  "0\" abc\"", "\"abc \"0", "000-0000", "###\\-####", "0\" x \"0", "00\":\"00", "(000) 000-0000", "0.0\" x \"0", "#\" \"##", "0 ??/??\"x\"", "0.00\" kg\"", "\"#\"0",
                  "000\"-\"##\"-\"0000", "#,##0.00\" EUR \"##", "#\" m\"0.0", "0\"a\"0\"b\"0",
                  "[RED]0;[BLUE]-0", "[>100]0;[<0]-0;0", "[<1]0.00;[>=1]0", "[RED][<0]0;[GREEN][>=0]0.00", "0;-0;\"zero\"", "0;[RED]-0;0;\"text:\"@", "[MAGENTA]#,##0.00;[CYAN]-#,##0.00", "[=1]\"one\";[=2]\"two\";\"many\"",
                  "[$€-407] #,##0.00", "#,##0.00 [$USD]", "[$¥-411] #,##0", "[$$-409]#,##0.00", "[$-409]#,##0.00", "[$-407]0.00", "[$-411]0", "[$-C09]0", "[$-1009]0", "#,##0.00 [$zł-415]", "[$CHF-807] #,##0.00",
                  "[$-F400]HH:MM:SS AM/PM", "[$-F800]DDDD, MMMM DD, YYYY", "[$-411]YYYY\"年\"M\"月\"D\"日\"", "[$-404]YYYY/MM/DD", "[$-41E]D MMMM YYYY", "[$-1000000]YYYY", "[$-1010409]0", "[$-2000409]0",
                  "[NatNum1]0", "[NatNum2]0", "[NatNum3]#,##0", "[NatNum4]0", "[NatNum5]0", "[NatNum6]0", "[NatNum7]0", "[NatNum8]0", "[NatNum9]0", "[NatNum10]0", "[NatNum11]0", "[NatNum12]0",
                  "[NatNum1][$-411]0", "[NatNum2][$-804]0", "[NatNum3][$-404]0", "[NatNum1][$-401]0", "[NatNum1][$-41E]0", "[NatNum1][$-439]0", "[NatNum1][$-412]0", "[DBNum1]0", "[DBNum2]0", "[DBNum3]0", "[DBNum1][$-411]0", "[DBNum2][$-804]0", "[DBNum3][$-404]0",
                  "[NatNum1]0;[NatNum2]-0", "[NatNum1][RED]0", "[NatNum1]0.0E+00", "[NatNum1]0%", "[NatNum1]0 ?/?",
                  "@", "\"Text: \"@", "[RED]@", "@\" suffix\"", "\"<\"@\">\"", "[NatNum1]@", "BOOLEAN", "[RED]BOOLEAN", "General", "General\" x\"", "[BLUE]General", "0.00\" \"[$EUR]",
                  "[<0.5]0%;[>=0.5]0.00%", "[>1]0.0%;[<-1]-0.0%;0.00%", "0%;[RED]-0%;0.0%", "[NatNum1][$€-407] #,##0.00", "[NatNum2][$¥-411]#,##0", "[NatNum1]BOOLEAN", "[NatNum1]0.00%", "[NatNum1]0%;[NatNum2]-0%",
                  "[DBNum1][$-804]0%", "[>5]0.00;[NatNum1]0"] + ["[NatNum%d]YYYY-MM-DD" % n for n in range(1, 13)] + ["[NatNum%d][$-411]YYYY\"/\"MM\"/\"DD" % n for n in range(1, 13)]
                 + ["[DBNum%d][$-804]YYYY-MM-DD" % n for n in range(1, 4)] + ["[NatNum%d]HH:MM:SS" % n for n in (1, 2, 3, 4)] + ["[NatNum%d][$-411]HH:MM" % n for n in (1, 2, 3)]),
]


@section
def calc_numbers():
    doc = new("private:factory/scalc")
    sh = doc.Sheets.getByIndex(0)
    nf = doc.NumberFormats
    row = 0
    seen = {}
    locs = [(l, c) for l, c in LOCALES]
    ok = bad = 0
    expect = []
    for (l0, c0, codes) in FORMATS:
        for (l, c) in ([(l0, c0)] + locs):
            loc = uno.createUnoStruct("com.sun.star.lang.Locale", l, c, "")
            for code in codes:
                if (l, c) != (l0, c0) and not any(t in code for t in ("[~", "GG", "NN", "[NatNum", "[DBNum", "Q", "WW", "[$")):
                    continue
                try:
                    key = nf.queryKey(code, loc, False)
                    if key == -1:
                        key = nf.addNew(code, loc)
                except Exception as e:
                    bad += 1
                    print("WARN numfmt", l, c, code, type(e).__name__)
                    continue
                if key in seen:
                    continue
                seen[key] = 1
                ok += 1
                cell = sh.getCellByPosition(0, row)
                cell.Value = 44197.4375 + row if ("Q" in code or "W" in code or "Y" in code or "D" in code or "G" in code or "H" in code or "E" in code or "R" in code or "N" in code or "M" in code) and "0" not in code[:1] else 1234.5678 - row
                if "@" in code:
                    cell.String = "text %d" % row
                if "BOOLEAN" in code:
                    cell.Value = 1
                cell.NumberFormat = key
                sh.getCellByPosition(1, row).String = "%s %s" % (l + "-" + c, code)
                expect.append((row, code, l, c))
                row += 1
    print("numfmt ok", ok, "bad", bad)
    for t in (1, 2, 4, 8, 16, 32, 64, 128, 256, 1024):
        for (l, c) in locs:
            try:
                loc = uno.createUnoStruct("com.sun.star.lang.Locale", l, c, "")
                key = nf.getStandardFormat(t, loc)
                cell = sh.getCellByPosition(2, row)
                cell.Value = 44197.4375 + row
                cell.NumberFormat = key
                row += 1
            except Exception as e:
                print("WARN stdfmt", t, l, type(e).__name__)
    for fmt in range(0, 60):
        try:
            loc = uno.createUnoStruct("com.sun.star.lang.Locale", "en", "US", "")
            cell = sh.getCellByPosition(3, fmt)
            cell.Value = 44197.4375 + fmt
            cell.NumberFormat = fmt
        except Exception:
            pass
    save(doc, "calc-numberformats.ods", "calc8")
    doc.close(True)
    try:
        d2 = reload("calc-numberformats.ods")
        sh2 = d2.Sheets.getByIndex(0)
        nf2 = d2.NumberFormats
        diff = 0
        for (r, code, l, c) in expect:
            try:
                ps = nf2.getByKey(sh2.getCellByPosition(0, r).NumberFormat)
                got, gl = ps.getPropertyValue("FormatString"), ps.getPropertyValue("Locale")
            except Exception as e:
                diff += 1
                print("RT-NUMFMT", l, c, code, "-> exception", type(e).__name__)
                continue
            if got != code or (gl.Language, gl.Country) != (l, c):
                diff += 1
                print("RT-NUMFMT", l, c, code, "->", got, gl.Language, gl.Country)
        print("RT-SUMMARY calc-numberformats.ods formats", len(expect), "differing", diff)
        d2.close(True)
    except Exception as e:
        print("RT-FAIL calc-numberformats", type(e).__name__, str(e)[:100])


def char_cases():
    c = []
    for suf in ("", "Asian", "Complex"):
        c += [("weight%s %s" % (suf, v), {"CharWeight" + suf: float(v)}) for v in WEIGHTS]
        c += [("posture%s %s" % (suf, v), {"CharPosture" + suf: enum("awt.FontSlant", v)}) for v in SLANTS]
        c += [("family%s %s" % (suf, v), {"CharFontFamily" + suf: const("awt.FontFamily." + v)}) for v in FAMILIES]
        c += [("pitch%s %s" % (suf, v), {"CharFontPitch" + suf: const("awt.FontPitch." + v)}) for v in PITCHES]
        c += [("charset%s %s" % (suf, v), {"CharFontCharSet" + suf: const("awt.CharSet." + v)}) for v in CHARSETS]
        c += [("named%s %s" % (suf, f), {"CharFontName" + suf: "T12Face%s%s" % (suf, f), "CharFontFamily" + suf: const("awt.FontFamily." + f), "CharFontPitch" + suf: const("awt.FontPitch.FIXED"), "CharFontStyleName" + suf: "Bold"}) for f in FAMILIES]
        c += [("height%s" % suf, {"CharHeight" + suf: 17.5}), ("propheight%s" % suf, {"CharPropHeight" + suf: 150}), ("diffheight%s" % suf, {"CharDiffHeight" + suf: 2.0})]
    for v in UNDERLINES:
        c += [("underline %d" % v, {"CharUnderline": v}), ("underline-col %d" % v, {"CharUnderline": v, "CharUnderlineHasColor": True, "CharUnderlineColor": 0xFF0000}), ("underline-word %d" % v, {"CharUnderline": v, "CharWordMode": True}),
              ("overline %d" % v, {"CharOverline": v}), ("overline-col %d" % v, {"CharOverline": v, "CharOverlineHasColor": True, "CharOverlineColor": 0x00FF00}), ("overline-word %d" % v, {"CharOverline": v, "CharWordMode": True})]
    for v in STRIKES:
        c += [("strike %d" % v, {"CharStrikeout": v}), ("strike-word %d" % v, {"CharStrikeout": v, "CharWordMode": True}), ("crossed %d" % v, {"CharCrossedOut": bool(v)})]
    c += [("casemap %d" % v, {"CharCaseMap": v}) for v in range(5)]
    c += [("relief %d" % v, {"CharRelief": v}) for v in range(3)]
    c += [("emphasis %d" % v, {"CharEmphasis": v}) for v in (0, 1, 2, 3, 4, 11, 12, 13, 14)]
    c += [("kerning %d" % v, {"CharKerning": v}) for v in (-30, 0, 40)]
    for v in (True, False):
        c += [("autokern %s" % v, {"CharAutoKerning": v}), ("flash %s" % v, {"CharFlash": v}), ("shadowed %s" % v, {"CharShadowed": v}), ("contoured %s" % v, {"CharContoured": v}),
              ("fitline %s" % v, {"CharRotation": 900, "CharRotationIsFitToLine": v}), ("combine %s" % v, {"CharCombineIsOn": v, "CharCombinePrefix": "<", "CharCombineSuffix": ">"}),
              ("hidden %s" % v, {"CharHidden": v})]
    c += [("scalewidth %d" % v, {"CharScaleWidth": v}) for v in (50, 100, 150)]
    c += [("escapement %d" % e, {"CharEscapement": e, "CharEscapementHeight": h}) for e, h in ((33, 58), (-33, 58), (101, 100), (0, 100))]
    c += [("rotation %d" % r, {"CharRotation": r}) for r in (0, 900, 2700)]
    c += [("color auto", {"CharColor": -1}), ("color red", {"CharColor": 0xFF0000}), ("back", {"CharBackColor": 0xFFFF00})]
    for lang, cc in (("ja", "JP"), ("ar", "SA"), ("th", "TH"), ("zh", "CN"), ("he", "IL"), ("ko", "KR")):
        loc = uno.createUnoStruct("com.sun.star.lang.Locale", lang, cc, "")
        c.append(("locale %s" % lang, {"CharLocale": loc, "CharLocaleAsian": loc, "CharLocaleComplex": loc}))
    return c


def para_cases():
    c = []
    for v in range(5):
        c += [("adjust %d" % v, {"ParaAdjust": v}), ("lastline %d" % v, {"ParaAdjust": 3, "ParaLastLineAdjust": v}), ("vertalign %d" % v, {"ParaVertAlignment": v}), ("writingmode %d" % v, {"WritingMode": v})]
    for v in (True, False):
        for p in ("ParaExpandSingleWord", "ParaIsAutoFirstLineIndent", "SnapToGrid", "ParaRegisterModeActive", "ParaIsHyphenation", "ParaIsHangingPunctuation", "ParaIsCharacterDistance", "ParaIsForbiddenRules", "ParaSplit", "ParaKeepTogether", "FontIndependentLineSpacing", "ParaIsConnectBorder"):
            c.append(("%s %s" % (p, v), {p: v}))
    c.append(("tabs", {"ParaTabStops": tabs(tab(1500, "DECIMAL", ".", ","), tab(3000, "RIGHT", "-"), tab(4000, "CENTER", "."), tab(5000, "LEFT", "_"))}))
    for mode, h in ((0, 150), (1, 600), (2, 200), (3, 500)):
        c.append(("linespacing %d" % mode, {"ParaLineSpacing": uno.createUnoStruct("com.sun.star.style.LineSpacing", mode, h)}))
    c += [("margins", {"ParaLeftMargin": 1000, "ParaRightMargin": 800, "ParaTopMargin": 300, "ParaBottomMargin": 200, "ParaFirstLineIndent": 500}),
          ("margins-rel", {"ParaLeftMarginRelative": 10, "ParaTopMarginRelative": 10, "ParaBottomMarginRelative": 10, "ParaFirstLineIndentRelative": 10}),
          ("hyphen", {"ParaIsHyphenation": True, "ParaHyphenationMaxLeadingChars": 3, "ParaHyphenationMaxTrailingChars": 4, "ParaHyphenationMaxHyphens": 2}),
          ("orphans", {"ParaOrphans": 3, "ParaWidows": 4}), ("tabdist", {"TabStopDistance": 1500}), ("outline", {"NumberingLevel": 2}),
          ("back", {"ParaBackColor": 0x99CCFF}), ("regmode", {"ParaRegisterModeActive": True})]
    return c


def apply_text(doc, shape, label, props):
    shape.String = label
    chars = {k: v for k, v in props.items() if k.startswith("Char")}
    others = {k: v for k, v in props.items() if k not in chars}
    setall(shape, others, label)
    if chars:
        cur = shape.createTextCursor()
        cur.gotoStart(False)
        cur.goRight(len(label) - 1, True)
        setall(cur, chars, label)


def shapes_page(doc, page, cases, cols=6):
    for k, (label, props) in enumerate(cases):
        sh = doc.createInstance("com.sun.star.drawing.TextShape")
        page.add(sh)
        sh.setSize(uno.createUnoStruct("com.sun.star.awt.Size", 3000, 800))
        sh.setPosition(uno.createUnoStruct("com.sun.star.awt.Point", 500 + (k % cols) * 3300, 500 + (k // cols) * 900))
        apply_text(doc, sh, label, props)


def style_cases(doc, family, svc, cases, tag):
    fam = doc.StyleFamilies.getByName(family)
    names = []
    for k, (label, props) in enumerate(cases):
        st = fam.createInstance() if svc is None else doc.createInstance(svc)
        name = "%s%d" % (tag, k)
        fam.insertByName(name, st)
        setall(st, props, name)
        names.append(name)
    return names


@section
def draw_text():
    doc = new("private:factory/sdraw")
    page = doc.DrawPages.getByIndex(0)
    cases = char_cases() + para_cases()
    shapes_page(doc, page, cases)
    names = style_cases(doc, "graphics", None, cases, "G")
    page2 = doc.DrawPages.insertNewByIndex(1)
    for k, n in enumerate(names):
        sh = doc.createInstance("com.sun.star.drawing.TextShape")
        page2.add(sh)
        sh.setSize(uno.createUnoStruct("com.sun.star.awt.Size", 3000, 800))
        sh.setPosition(uno.createUnoStruct("com.sun.star.awt.Point", 500 + (k % 6) * 3300, 500 + (k // 6) * 900))
        sh.String = cases[k][0]
        sh.Style = doc.StyleFamilies.getByName("graphics").getByName(n)
    save(doc, "draw-text.odg", "draw8")
    doc.close(True)


@section
def impress_text():
    doc = new("private:factory/simpress")
    page = doc.DrawPages.getByIndex(0)
    cases = char_cases() + para_cases()
    shapes_page(doc, page, cases)
    names = style_cases(doc, "graphics", None, cases[:120], "G")
    page2 = doc.DrawPages.insertNewByIndex(1)
    for k, n in enumerate(names):
        sh = doc.createInstance("com.sun.star.drawing.TextShape")
        page2.add(sh)
        sh.setSize(uno.createUnoStruct("com.sun.star.awt.Size", 3000, 800))
        sh.setPosition(uno.createUnoStruct("com.sun.star.awt.Point", 500 + (k % 6) * 3300, 500 + (k // 6) * 900))
        sh.String = cases[k][0]
        sh.Style = doc.StyleFamilies.getByName("graphics").getByName(n)
    save(doc, "impress-text.odp", "impress8")
    doc.close(True)


@section
def calc_text():
    doc = new("private:factory/scalc")
    sh = doc.Sheets.getByIndex(0)
    cases = char_cases() + para_cases()
    names = style_cases(doc, "CellStyles", "com.sun.star.style.CellStyle", cases, "S")
    for k, (label, props) in enumerate(cases):
        a = sh.getCellByPosition(0, k)
        a.String = label
        setp(a, "CellStyle", names[k], label)
        b = sh.getCellByPosition(1, k)
        b.String = label
        setall(b, {kk: v for kk, v in props.items() if kk.startswith("Char") or kk.startswith("Para")}, label)
        cur = sh.getCellByPosition(2, k)
        cur.String = label
        cc = cur.createTextCursor()
        cc.gotoStart(False)
        cc.goRight(len(label) - 1, True)
        setall(cc, {kk: v for kk, v in props.items() if kk.startswith("Char")}, label)
    save(doc, "calc-text.ods", "calc8")
    doc.close(True)


@section
def writer_page():
    doc = new("private:factory/swriter")
    url = png("page.png")
    pages = doc.StyleFamilies.getByName("PageStyles")
    cases = [{"BackGraphicURL": url, "BackGraphicLocation": enum("style.GraphicLocation", loc), "BackGraphicFilter": "png", "BackGraphicTransparency": 30} for loc in ("LEFT_TOP", "AREA", "TILED", "RIGHT_BOTTOM", "NONE")]
    cases += [{"BackColor": 0xCCCCCC, "BackTransparent": False}]
    cases += [{"WritingMode": w} for w in range(5)]
    for sepw, al in ((10, "TOP"), (30, "MIDDLE"), (50, "BOTTOM")):
        cases.append({"__cols": (2, {"SeparatorLineIsOn": True, "SeparatorLineWidth": sepw, "SeparatorLineColor": 0x884400, "SeparatorLineRelativeHeight": 70, "SeparatorLineVerticalAlignment": enum("style.VerticalAlignment", al)})})
    for k, props in enumerate(cases):
        st = doc.createInstance("com.sun.star.style.PageStyle")
        pages.insertByName("T12Page%d" % k, st)
        props = dict(props)
        cols = props.pop("__cols", None)
        setall(st, props, "page%d" % k)
        if cols:
            tc = st.TextColumns
            tc.setColumnCount(cols[0])
            setall(tc, cols[1], "pagecols")
            st.TextColumns = tc
        c = doc.Text.createTextCursor()
        c.gotoEnd(False)
        doc.Text.insertString(c, "page style %d" % k, False)
        setp(c, "PageDescName", "T12Page%d" % k, "page")
        doc.Text.insertControlCharacter(c, const("text.ControlCharacter.PARAGRAPH_BREAK"), False)
    save(doc, "writer-page-styles.odt", "writer8")
    doc.close(True)


# --- end of sections
for _fn in SECTIONS:
    run(_fn.__name__, _fn)
sys.exit(1 if failed else 0)
