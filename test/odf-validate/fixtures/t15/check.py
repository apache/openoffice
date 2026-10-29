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
# usage: runpy.sh fixtures/t15/check.py <port> <outdir> <mode>   (mode ext | strict | v11: the office profile's save version)
# Task 15 fixes: protection keys (SHA-256 over UTF-8 with the digest attribute), foreign office:meta content in strict mode,
# drawooo:display in Writer and Calc, a missing number:transliteration-style. Prints PASS/FAIL lines.
import sys, os, re, zipfile, hashlib, base64, traceback, uno
import xml.etree.ElementTree as ET
from unohelp import connect, desktop, pv
from com.sun.star.awt import Size, Point

port, out, mode = int(sys.argv[1]), os.path.abspath(sys.argv[2]), sys.argv[3]
os.makedirs(out, exist_ok=True)
desk = desktop(connect(port))
P, WRONG = "Pässwörd中1", "Passwort"
DSIG256 = "http://www.w3.org/2000/09/xmldsig#sha256"
ENC256 = "http://www.w3.org/2001/04/xmlenc#sha256"
DSIG1 = "http://www.w3.org/2000/09/xmldsig#sha1"
DRAWOOO = "http://openoffice.org/2010/draw"
new_form = mode in ("ext", "strict")


def sha1(enc):
    return hashlib.sha1(P.encode(enc)).digest()


H_LE, H_BE, H_U8 = sha1("utf-16-le"), sha1("utf-16-be"), sha1("utf-8")
H_256 = hashlib.sha256(P.encode("utf-8")).digest()
b64 = lambda b: base64.b64encode(b).decode()


def check(name, ok, detail=""):
    print(("PASS " + name) if ok else ("FAIL %s %s" % (name, detail)), flush=True)


def new(kind):
    return desk.loadComponentFromURL("private:factory/" + kind, "_blank", 0, (pv("Hidden", True),))


def load(path):
    return desk.loadComponentFromURL(uno.systemPathToFileUrl(path), "_blank", 0, (pv("Hidden", True),))


def store(doc, name, flt):
    p = os.path.join(out, name)
    doc.storeToURL(uno.systemPathToFileUrl(p), (pv("FilterName", flt),))
    return p


def member(path, name):
    with zipfile.ZipFile(path) as z:
        return z.read(name).decode("utf-8")


def rewrite(src, dst, edits):
    """copy a package, applying {member: fn(text) -> text}; mimetype stays first and stored"""
    with zipfile.ZipFile(src) as zi, zipfile.ZipFile(dst, "w") as zo:
        for i in zi.infolist():
            data = zi.read(i.filename)
            if i.filename in edits:
                data = edits[i.filename](data.decode("utf-8")).encode("utf-8")
            zo.writestr(i, data, compress_type=zipfile.ZIP_STORED if i.filename == "mimetype" else zipfile.ZIP_DEFLATED)
    return dst


def attrs(xml, tag, attr):
    """values of attribute attr (qualified name) on every <tag ...> start tag"""
    return [m.group(1) for t in re.findall(r"<%s\b[^>]*>" % re.escape(tag), xml)
            for m in [re.search(r'\b%s="([^"]*)"' % re.escape(attr), t)] if m]


def setkey(key, digest):
    def f(xml):
        xml = re.sub(r' table:protection-key-digest-algorithm="[^"]*"', "", xml)
        xml = re.sub(r'(table:protection-key=")[^"]*"', lambda m: m.group(1) + b64(key) + '"' +
                     (' table:protection-key-digest-algorithm="%s"' % digest if digest else ""), xml)
        return xml
    return f


def guarded(fn):
    try:
        fn()
    except Exception:
        check(fn.__name__, False, traceback.format_exc().strip().splitlines()[-1])


# ---- Calc protection keys
def calc_keys():
    doc = new("scalc")
    doc.Sheets.getByIndex(0).protect(P)
    doc.protect(P)
    base = store(doc, "calc-new.ods", "calc8")
    doc.close(True)
    x = member(base, "content.xml")
    sheet_key = attrs(x, "table:table", "table:protection-key")
    body_key = attrs(x, "office:spreadsheet", "table:protection-key")
    sheet_dig = attrs(x, "table:table", "table:protection-key-digest-algorithm")
    body_dig = attrs(x, "office:spreadsheet", "table:protection-key-digest-algorithm")
    want = H_256 if new_form else H_LE
    check("calc-new key", sheet_key == [b64(want)] and body_key == [b64(want)], (sheet_key, body_key))
    check("calc-new digest attribute", (sheet_dig == [DSIG256] and body_dig == [DSIG256]) if new_form else (not sheet_dig and not body_dig), (sheet_dig, body_dig))

    def verifies(path, label, ok_expected):
        d = load(path)
        sh = d.Sheets.getByIndex(0)
        was = sh.isProtected()
        try:
            sh.unprotect(WRONG)
        except Exception:
            pass
        wrong_kept = sh.isProtected()
        try:
            sh.unprotect(P)
        except Exception:
            pass
        right_open = not sh.isProtected()
        d.close(True)
        check("calc %s" % label, was and wrong_kept and (right_open == ok_expected), (was, wrong_kept, right_open))

    verifies(base, "new file verifies", True)
    variants = [("legacy sha1 utf16le", H_LE, None, True), ("sha1 utf16be", H_BE, None, True),
                ("sha1 utf8 no attribute", H_U8, None, True), ("sha1 utf8 xmldsig#sha1", H_U8, DSIG1, True),
                ("sha256 xmldsig", H_256, DSIG256, True), ("sha256 xmlenc", H_256, ENC256, True),
                ("sha256 no attribute", H_256, None, True), ("unknown algorithm", os.urandom(20), "urn:x-test:alg", False)]
    for i, (label, key, dig, ok) in enumerate(variants):
        p = rewrite(base, os.path.join(out, "calc-var%d.ods" % i), {"content.xml": setkey(key, dig)})
        verifies(p, label, ok)

    # ext and ODF 1.1 copy an unchanged sheet's stream verbatim (strict re-exports it), so the input attribute survives
    for label, key, dig, edited in (
            ("legacy", H_LE, None, False), ("sha256 without attribute", H_256, None, False), ("sha256 xmlenc", H_256, ENC256, False)):
        src = rewrite(base, os.path.join(out, "calc-resave-src.ods"), {"content.xml": setkey(key, dig)})
        d = load(src)
        p = store(d, "calc-resave.ods", "calc8")
        d.close(True)
        if key == H_LE:
            want_dig = None
        else:
            want_dig = DSIG256 if mode == "strict" else dig
        x = member(p, "content.xml")
        got = attrs(x, "table:table", "table:protection-key")
        gdig = attrs(x, "table:table", "table:protection-key-digest-algorithm")
        check("calc re-save %s" % label, got == [b64(key)] and gdig == ([want_dig] if want_dig else []), (got, gdig))


    # a password changed after loading must reach the file
    P2 = "other\u00e9"
    src = rewrite(base, os.path.join(out, "calc-change-src.ods"), {"content.xml": setkey(H_LE, None)})
    d = load(src)
    sh = d.Sheets.getByIndex(0)
    sh.unprotect(P)
    sh.protect(P2)
    p = store(d, "calc-change.ods", "calc8")
    d.close(True)
    x = member(p, "content.xml")
    got = attrs(x, "table:table", "table:protection-key")
    want = hashlib.sha256(P2.encode("utf-8")).digest() if new_form else hashlib.sha1(P2.encode("utf-16-le")).digest()
    check("calc changed password is written", got == [b64(want)], got)


# ---- Writer section protection keys
def writer_keys():
    def make(key, name):
        doc = new("swriter")
        sec = doc.createInstance("com.sun.star.text.TextSection")
        cur = doc.Text.createTextCursor()
        doc.Text.insertTextContent(cur, sec, False)
        sec.setPropertyValue("ProtectionKey", uno.ByteSequence(key))
        sec.setPropertyValue("IsProtected", True)
        p = store(doc, name, "writer8")
        doc.close(True)
        return p

    for label, key, want_dig in (("sha256", H_256, DSIG256 if new_form else None), ("legacy sha1", H_LE, None)):
        p = make(key, "writer-%s.odt" % label.split()[-1])
        x = member(p, "content.xml")
        got = attrs(x, "text:section", "text:protection-key")
        gdig = attrs(x, "text:section", "text:protection-key-digest-algorithm")
        check("writer %s key" % label, got == [b64(key)], got)
        check("writer %s digest attribute" % label, gdig == ([want_dig] if want_dig else []), gdig)
    base = make(H_256, "writer-base.odt")
    src = rewrite(base, os.path.join(out, "writer-xmlenc.odt"), {"content.xml": lambda x: re.sub(
        r'(text:protection-key-digest-algorithm=")[^"]*"', lambda m: m.group(1) + ENC256 + '"', x)})
    d = load(src)
    sec = d.TextSections.getByIndex(0)
    got = bytes(sec.getPropertyValue("ProtectionKey").value) if hasattr(sec.getPropertyValue("ProtectionKey"), "value") else bytes(sec.getPropertyValue("ProtectionKey"))
    p = store(d, "writer-xmlenc-resave.odt", "writer8")
    d.close(True)
    check("writer xmlenc key loaded", got == H_256, got.hex())
    gdig = attrs(member(p, "content.xml"), "text:section", "text:protection-key-digest-algorithm")
    check("writer xmlenc re-saved as xmldsig", gdig == ([DSIG256] if new_form else []), gdig)


# ---- foreign content in office:meta
def meta_foreign():
    doc = new("swriter")
    doc.Text.insertString(doc.Text.End, "meta", False)
    base = store(doc, "meta-base.odt", "writer8")
    doc.close(True)

    def inject(m):
        m = m.replace("<office:meta>", '<office:meta xmlns:foo="urn:example:foreign" foo:metaattr="m">', 1)
        # a foreign sibling that rebinds dc: must not change how the following dc: element resolves
        return m.replace("</office:meta>", '<foo:bar foo:attr="1">x</foo:bar><x:ext xmlns:x="urn:example:x" xmlns:dc="urn:example:other"/><dc:subject>trapsubject</dc:subject></office:meta>', 1)
    src = rewrite(base, os.path.join(out, "meta-in.odt"), {"meta.xml": inject})
    check("meta input carries the foreign content", "foo:bar" in member(src, "meta.xml"))
    d = load(src)
    p = store(d, "meta-out.odt", "writer8")
    d.close(True)
    m = member(p, "meta.xml")
    ET.fromstring(m)
    foreign = "urn:example:foreign" in m or "foo:" in m
    check("meta foreign content %s" % ("dropped in strict" if mode == "strict" else "kept"), foreign == (mode != "strict"), m[:400])
    check("meta ODF element after a rebinding foreign sibling kept", "trapsubject" in m, m[:600])
    check("meta ODF content kept", "<meta:generator>" in m or "<meta:creation-date>" in m or "<dc:" in m or "<meta:" in m, m[:200])


# ---- drawooo:display in Writer and Calc
def display_ns():
    def shapes(doc, page):
        for i, (vis, prn) in enumerate(((True, False), (False, True), (False, False))):
            s = doc.createInstance("com.sun.star.drawing.RectangleShape")
            page.add(s)
            s.Position = Point(1000, 1000 + i * 2500)
            s.Size = Size(4000, 2000)
            s.setPropertyValue("Visible", vis)
            s.setPropertyValue("Printable", prn)
    for kind, flt, ext in (("swriter", "writer8", "odt"), ("scalc", "calc8", "ods")):
        doc = new(kind)
        page = doc.DrawPage if kind == "swriter" else doc.Sheets.getByIndex(0).DrawPage
        shapes(doc, page)
        p = store(doc, "display-%s.%s" % (kind, ext), flt)
        doc.close(True)
        root = ET.fromstring(member(p, "content.xml"))
        pref = bare = 0
        for e in root.iter():
            for a in e.attrib:
                pref += a == "{%s}display" % DRAWOOO
                bare += a == "display"
        want = 3 if mode == "ext" else 0
        check("%s drawooo:display count (%s)" % (kind, mode), pref == want, pref)
        check("%s no unprefixed display" % kind, bare == 0, bare)


# ---- missing number:transliteration-style
def translit():
    doc = new("scalc")
    sh = doc.Sheets.getByIndex(0)
    nf = doc.NumberFormats
    loc = uno.createUnoStruct("com.sun.star.lang.Locale")
    loc.Language, loc.Country = "ar", "SA"
    key = nf.queryKey("[NatNum1]0", loc, False)
    if key == -1:
        key = nf.addNew("[NatNum1]0", loc)
    c = sh.getCellByPosition(0, 0)
    c.Value = 123
    c.NumberFormat = key
    base = store(doc, "translit-base.ods", "calc8")
    doc.close(True)
    digits = "١٢٣"
    for label, fn in (("with style", lambda x: x), ("without style", lambda x: re.sub(r' number:transliteration-style="[^"]*"', "", x))):
        src = rewrite(base, os.path.join(out, "translit-%s.ods" % label.replace(" ", "-")), {"content.xml": fn, "styles.xml": fn})
        d = load(src)
        s = d.Sheets.getByIndex(0).getCellByPosition(0, 0).String
        d.close(True)
        check("transliteration %s shows Arabic-Indic digits" % label, s == digits, s)


for fn in (calc_keys, writer_keys, meta_foreign, display_ns, translit):
    guarded(fn)
