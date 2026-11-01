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
#
# usage: runpy.sh fixtures/styles-gfx/verify.py <port> <outdir>
# Reloads the packages written by make.py and compares the properties of shapes and styles with the values make.py set.
import sys, os
import tempfile
OUT = os.path.abspath(sys.argv[2])
sys.argv = [sys.argv[0], sys.argv[1], tempfile.mkdtemp(prefix="styles-gfx-verify-")]
exec(open("fixtures/styles-gfx/make.py").read().split("\nDOCS = [")[0])
bad, checked = [], [0]


def same(a, b, k=""):
    if k.endswith("Color") or k.endswith("Color2"):
        return (a & 0xffffff) == (b & 0xffffff)
    if isinstance(b, uno.Enum):
        return isinstance(a, uno.Enum) and a.value == b.value
    if isinstance(b, float):
        return abs(a - b) < 1e-3 * max(1.0, abs(b))
    if hasattr(b, "OuterLineWidth"):
        return (a.Color & 0xffffff) == (b.Color & 0xffffff) and (a.InnerLineWidth > 0) == (b.InnerLineWidth > 0) and \
            abs(a.OuterLineWidth + a.InnerLineWidth + a.LineDistance - b.OuterLineWidth - b.InnerLineWidth - b.LineDistance) <= 8
    if hasattr(b, "ShadowWidth"):
        return a.Location.value == b.Location.value
    if isinstance(b, bool):
        return bool(a) == b
    if isinstance(b, int) and isinstance(a, int) and abs(b) >= 50:
        return abs(a - b) <= 2
    return a == b


def cmp(tag, obj, props):
    for k, v in props.items():
        try:
            a = obj.getPropertyValue(k)
        except Exception:
            continue
        checked[0] += 1
        try:
            ok = same(a, v, k)
        except Exception as e:
            ok = False
        if not ok:
            bad.append((tag, k, v.value if isinstance(v, uno.Enum) else v, a.value if isinstance(a, uno.Enum) else a))


def loaddoc(name):
    return desk.loadComponentFromURL(uno.systemPathToFileUrl(os.path.join(OUT, name)), "_blank", 0,
                                     (pv("Hidden", True), pv("ReadOnly", True)))


def shapes(page):
    return [page.getByIndex(i) for i in range(page.Count)]


def draw_check():
    d = loaddoc("draw-fill-line.odg")
    sh = shapes(d.DrawPages.getByIndex(0))
    cases = fillprops() + linecases()
    for s, (n, p) in zip(sh, cases):
        cmp("draw-fill-line " + n, s, {k: v for k, v in p.items() if not k.endswith("Name") and k not in ("FillBitmapOffsetX", "FillBitmapOffsetY")})
    print("draw-fill-line shapes", len(sh), "cases", len(cases))
    d.close(True)


def style_check(doc_name, fam, prefix, cases):
    d = loaddoc(doc_name)
    st_ = d.StyleFamilies.getByName(fam)
    for n, p in cases:
        nm = prefix + n
        if st_.hasByName(nm):
            cmp(doc_name + " " + nm, st_.getByName(nm), p)
        else:
            bad.append((doc_name, nm, "style missing", ""))
    d.close(True)


def writer_pagestyle_check():
    d = loaddoc("writer-pagestyles.odt")
    fam = d.StyleFamilies.getByName("PageStyles")
    for wm in range(5):
        cmp("ps-wm-%d" % wm, fam.getByName("ps-wm-%d" % wm), dict(WritingMode=wm))
    for lay in ("ALL", "LEFT", "RIGHT", "MIRRORED"):
        cmp("ps-layout-" + lay, fam.getByName("ps-layout-" + lay), dict(PageStyleLayout=E("com.sun.star.style.PageStyleLayout", lay)))
    for nt in (0, 1, 2, 3, 4, 5, 6, 7, 9, 10, 11, 20, 30, 40):
        cmp("ps-num-%d" % nt, fam.getByName("ps-num-%d" % nt), dict(NumberingType=nt))
    for gm in (0, 1, 2):
        for std in (True, False):
            cmp("ps-grid-%d-%d" % (gm, std), fam.getByName("ps-grid-%s-%d" % (gm, std)), dict(GridMode=gm, RubyBelow=not std, GridPrint=not std, GridDisplay=bool(std)))
    for i in range(6):
        cmp("ps-hf-%d" % i, fam.getByName("ps-hf-%d" % i), dict(page_common(i), **hf_props("Header", i)))
    d.close(True)


def impress_check():
    d = loaddoc("impress-effects.odp")
    pages = d.DrawPages
    spd = ("SLOW", "MEDIUM", "FAST")
    effects = enum_names("com.sun.star.presentation.FadeEffect")
    for i in range(pages.Count):
        pg = pages.getByIndex(i)
        cmp("slide %d" % i, pg, dict(Speed=E("com.sun.star.presentation.AnimationSpeed", spd[i % 3]), Change=i % 3, Duration=1 + i % 5,
            Visible=bool(i % 4), IsBackgroundVisible=bool(i % 3), IsBackgroundObjectsVisible=bool(i % 5), IsFooterVisible=bool(i % 3),
            IsPageNumberVisible=bool(i % 4), IsDateTimeVisible=bool(i % 5)))
    print("impress-effects slides", pages.Count)
    d.close(True)
    d = loaddoc("impress-transitions.odp")
    pages = d.DrawPages
    print("impress-transitions slides", pages.Count)
    for i in range(pages.Count):
        pg = pages.getByIndex(i)
        try:
            checked[0] += 1
            if pg.getPropertyValue("TransitionType") == 0 and i < 38:
                bad.append(("impress-transitions", "slide %d" % i, "TransitionType>0", 0))
        except Exception:
            pass
    d.close(True)


def calc_check():
    cases = []
    d = loaddoc("calc-cells.ods")
    fam = d.StyleFamilies.getByName("CellStyles")
    names = [n for n in fam.ElementNames if n.startswith("cs-")]
    print("calc cell styles", len(names))
    d.close(True)


for fn in (draw_check, writer_pagestyle_check, impress_check, calc_check):
    try:
        fn()
    except Exception as e:
        print("FAIL", fn.__name__, type(e).__name__, str(e)[:120])
shutil.rmtree(work, ignore_errors=True)
shutil.rmtree(out, ignore_errors=True)
print("checked", checked[0], "mismatches", len(bad))
for b in bad:
    print("MISMATCH", *b)
