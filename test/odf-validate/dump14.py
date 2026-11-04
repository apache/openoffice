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

# usage: runpy.sh dump14.py <port> <outdir> <file>...
# Dump for consumer_smoke.py: loads each ODF package read-only and writes EVERY property value (not a fixed list, unlike dump7.py)
# of the objects the ODF 1.4 additions touch: paragraphs, user-defined styles, text frames, numbering rules, page styles, custom
# shape geometry, database range filters, chart axes. A construct the office reads then shows up as a difference against the twin
# without anyone having to guess its property name. Same output layout and .current marker as dump7.py (diff7.py compares it).
import sys, os, json, hashlib, uno
from unohelp import connect, desktop, load

port, outdir, files = int(sys.argv[1]), sys.argv[2], sys.argv[3:]
CAP = 100


def conv(x, depth=0):
    if depth > 8:
        return "<deep>"
    if isinstance(x, uno.Enum):
        return x.value
    if x is None or isinstance(x, (bool, int, float)):
        return x
    if isinstance(x, str):
        return x[:300]
    if isinstance(x, bytes):
        return "bytes:" + hashlib.sha256(x).hexdigest()[:12]
    if isinstance(x, (tuple, list)):
        return [conv(i, depth + 1) for i in x][:CAP]
    if hasattr(x, "typeName"):
        if x.typeName.endswith(".PropertyValue"):
            return [x.Name, conv(x.Value, depth + 1)]
        try:
            return {k: conv(getattr(x, k), depth + 1) for k in dir(x) if not k.startswith("_") and k not in ("typeName", "value")}
        except Exception:
            return "<struct>"
    if hasattr(x, "getCount") and hasattr(x, "getByIndex"):
        try:
            return [conv(x.getByIndex(i), depth + 1) for i in range(min(x.getCount(), 20))]
        except Exception:
            return "<index>"
    return "<obj>"


def tryf(f, d="n/a"):
    try:
        return f()
    except Exception:
        return d


NOISE = ("RuntimeUID", "ListId", "NumberingStyleName")   # differ between any two documents


def allprops(o):
    """Every readable property of o; values that are interfaces (conv gives '<obj>') are left out."""
    out = {}
    try:
        names = sorted(p.Name for p in o.getPropertySetInfo().getProperties())
    except Exception:
        return out
    for n in names:
        if n in NOISE:
            continue
        try:
            val = conv(o.getPropertyValue(n))
        except Exception:
            val = "n/a"
        if val != "<obj>":
            out[n] = val
    return out


def styles(doc, family):
    """User-defined styles of a family, plus the default page/paragraph style."""
    out = {}
    try:
        fam = doc.StyleFamilies.getByName(family)
    except Exception:
        return out
    for n in fam.ElementNames:
        s = fam.getByName(n)
        if tryf(lambda: s.isUserDefined(), False) or n in ("Standard", "Default", "Default Style"):
            out[n] = allprops(s)
    return out


def shapes(page, limit=100):
    out = []
    for k in range(min(page.Count, limit)):
        s = page.getByIndex(k)
        d = {"type": tryf(lambda: s.ShapeType), "text": tryf(lambda: s.String[:80]), "props": allprops(s)}
        if tryf(lambda: s.supportsService("com.sun.star.drawing.GroupShape"), False):
            d["kids"] = shapes(s, 30)
        out.append(d)
    return out


def writer(doc):
    o = {"paras": [], "frames": {}}
    e = doc.Text.createEnumeration()
    while e.hasMoreElements() and len(o["paras"]) < 50:
        p = e.nextElement()
        if p.supportsService("com.sun.star.text.Paragraph"):
            o["paras"].append({"text": p.String[:80], "props": allprops(p)})
    for n in doc.TextFrames.ElementNames[:30]:
        o["frames"][n] = allprops(doc.TextFrames.getByName(n))
    o["shapes"] = shapes(doc.DrawPage)
    o["parastyles"] = styles(doc, "ParagraphStyles")
    o["framestyles"] = styles(doc, "FrameStyles")
    o["pagestyles"] = styles(doc, "PageStyles")
    o["liststyles"] = styles(doc, "NumberingStyles")
    o["settings"] = allprops(doc.createInstance("com.sun.star.document.Settings"))
    o["docprops"] = allprops(doc)
    return o


def calc(doc):
    o = {"sheets": []}
    for i in range(doc.Sheets.Count):
        sh = doc.Sheets.getByIndex(i)
        so = {"name": sh.Name, "props": allprops(sh), "cells": {}}
        for r in range(0, 5):
            for c in range(0, 3):
                cell = sh.getCellByPosition(c, r)
                if cell.Type.value != "EMPTY":
                    so["cells"]["%d,%d" % (c, r)] = {"type": cell.Type.value, "error": cell.getError(), "string": cell.String[:80], "props": allprops(cell)}
        dbs = {}
        for n in tryf(lambda: doc.DatabaseRanges.ElementNames, []):
            dr = doc.DatabaseRanges.getByName(n)
            fd = tryf(lambda: dr.getFilterDescriptor(), None)
            dbs[n] = {"props": allprops(dr), "filter": allprops(fd) if fd else None,
                      "fields": conv(tryf(lambda: fd.getFilterFields(), []))}
        so["dbranges"] = dbs
        so["shapes"] = shapes(sh.DrawPage)
        o["sheets"].append(so)
    o["cellstyles"] = styles(doc, "CellStyles")
    o["pagestyles"] = styles(doc, "PageStyles")
    o["docprops"] = allprops(doc)
    return o


def draw(doc):
    o = {"pages": []}
    for i in range(doc.DrawPages.Count):
        pg = doc.DrawPages.getByIndex(i)
        o["pages"].append({"props": allprops(pg), "shapes": shapes(pg), "annotations": tryf(lambda: conv(pg.Annotations), "n/a") if hasattr(pg, "Annotations") else "n/a"})
    o["graphicstyles"] = styles(doc, "graphics")
    o["pagestyles"] = styles(doc, "PageStyles")
    return o


def chart(doc):
    o = {}
    try:
        cs = doc.getFirstDiagram().getCoordinateSystems()[0]
        o["coordsys"] = allprops(cs)
        for dim in range(cs.getDimension()):
            for idx in range(cs.getMaximumAxisIndexByDimension(dim) + 1):
                ax = cs.getAxisByDimension(dim, idx)
                o["axis%d_%d" % (dim, idx)] = {"props": allprops(ax), "scale": conv(ax.getScaleData())}
    except Exception as e:
        o["error"] = repr(e)[:100]
    o["docprops"] = allprops(doc)
    return o


def has(doc, s):
    return tryf(lambda: doc.supportsService(s), False)


DESK = desktop(connect(port))
os.makedirs(outdir, exist_ok=True)
for f in files:
    dest = os.path.join(outdir, os.path.basename(f) + ".json")
    if os.path.exists(dest):
        continue
    open(os.path.join(outdir, ".current"), "w").write(f)
    out = {}
    try:
        doc = load(DESK, f)
        out["type"] = ("writer" if has(doc, "com.sun.star.text.TextDocument") else "calc" if has(doc, "com.sun.star.sheet.SpreadsheetDocument")
                       else "impress" if has(doc, "com.sun.star.presentation.PresentationDocument") else "draw" if has(doc, "com.sun.star.drawing.DrawingDocument")
                       else "chart" if has(doc, "com.sun.star.chart2.ChartDocument") else "other")
        try:
            out["body"] = {"writer": writer, "calc": calc, "impress": draw, "draw": draw, "chart": chart}.get(out["type"], lambda d: {})(doc)
        except Exception as e:
            out["body_error"] = repr(e)[:200]
        doc.close(True)
    except Exception as e:
        out["error"] = repr(e)[:200]
    json.dump(out, open(dest, "w"), sort_keys=True, indent=0, default=str)
print("done", len(files))
