# usage: runpy.sh dump7.py <port> <outdir> <file>...
# Loads each ODF package read-only over UNO and writes a JSON dump of a fixed property set (Task 7, Review Focus 2).
import sys, os, json, hashlib, uno
from unohelp import connect, desktop, load

port, outdir, files = int(sys.argv[1]), sys.argv[2], sys.argv[3:]
LIM = 400

def v(x, depth=0):
    if isinstance(x, uno.Enum): return x.value
    if x is None or isinstance(x, (bool, int, float, str)): return x
    if isinstance(x, (tuple, list)): return [v(i, depth + 1) for i in x][:200]
    if hasattr(x, "typeName"):
        if depth > 3: return "<deep>"
        try: return {k: v(getattr(x, k), depth + 1) for k in dir(x) if not k.startswith("_") and k not in ("typeName", "value")}
        except Exception: return "<struct>"
    return "<obj>"
def g(o, n):
    try: return v(o.getPropertyValue(n))
    except Exception: return "n/a"
def gs(o, names): return [g(o, n) for n in names]
def h(s): return hashlib.sha256(s.encode("utf-8", "replace")).hexdigest()[:16]
def tryf(f, d="n/a"):
    try: return f()
    except Exception as e: return d if d != "ERR" else repr(e)[:80]
def has(doc, s): return tryf(lambda: doc.supportsService(s), False)

SHAPE_PROPS = ["FillStyle", "FillColor", "FillTransparence", "FillGradientName", "FillHatchName", "FillBitmapName", "LineStyle", "LineColor", "LineWidth", "CharHeight", "ParaAdjust"]
def shape(s, depth=0):
    d = {"t": tryf(lambda: s.ShapeType), "name": tryf(lambda: s.Name), "pos": v(tryf(lambda: s.Position)), "size": v(tryf(lambda: s.Size)),
         "text": tryf(lambda: s.String[:80]), "p": gs(s, SHAPE_PROPS)}
    t = d["t"] if isinstance(d["t"], str) else ""
    if "Connector" in t:
        d["conn"] = {"kind": g(s, "EdgeKind"), "start": g(s, "StartPosition"), "end": g(s, "EndPosition"), "bez": g(s, "PolyPolygonBezier")}
    if "OLE2" in t:
        d["ole"] = {"cls": g(s, "CLSID"), "internal": g(s, "IsInternal"), "model": embedded(tryf(lambda: s.Model, None))}
    if "GraphicObject" in t: d["gr"] = gs(s, ["GraphicURL", "Transparency"])[1:]
    if tryf(lambda: s.supportsService("com.sun.star.drawing.GroupShape"), False) and depth < 4:
        d["kids"] = [shape(s.getByIndex(i), depth + 1) for i in range(min(s.Count, 100))]
    return d
def forms_of(page):
    out = []
    try: forms = page.Forms
    except Exception: return out
    def walk(c):
        for i in range(min(c.Count, 300)):
            e = c.getByIndex(i)
            if tryf(lambda: e.supportsService("com.sun.star.form.FormComponents"), False) or tryf(lambda: e.supportsService("com.sun.star.form.component.Form"), False):
                out.append(["FORM", tryf(lambda: e.Name)]); walk(e)
            else:
                names = tryf(lambda: [p.Name for p in e.getPropertySetInfo().getProperties()], [])
                out.append([tryf(lambda: e.Name), tryf(lambda: e.ImplementationName), g(e, "ObjIDinMSO") if "ObjIDinMSO" in names else "-",
                            tryf(lambda: e.getValueBinding() is not None, "-"), tryf(lambda: (e.getListEntrySource() is not None), "-"), len(names)])
    walk(forms)
    return out
def chart_info(m):
    if m is None: return None
    try:
        r = {"title": tryf(lambda: m.Title.String if m.HasMainTitle else ""), "diagram": tryf(lambda: m.Diagram.ServiceName if hasattr(m.Diagram, "ServiceName") else m.getDiagram().getDiagramType()),
             "addshapes": tryf(lambda: [m.DrawPage.Count, sorted(m.DrawPage.getByIndex(i).ShapeType.split(".")[-1] for i in range(m.DrawPage.Count))]), "intdata": tryf(lambda: m.hasInternalDataProvider())}
        return r
    except Exception: return "n/a"
def embedded(m, depth=0):
    """Dump an embedded model: drawing and spreadsheet documents fully, charts by title/type/additional shapes."""
    if m is None: return None
    if depth > 1: return "<deep>"
    try:
        if has(m, "com.sun.star.drawing.DrawingDocument") or has(m, "com.sun.star.presentation.PresentationDocument"): return {"draw": draw(m)}
        if has(m, "com.sun.star.sheet.SpreadsheetDocument"): return {"calc": calc(m)}
        if has(m, "com.sun.star.chart2.ChartDocument") or has(m, "com.sun.star.chart.ChartDocument"): return chart_info(m)
        return {"other": tryf(lambda: list(m.SupportedServiceNames)[:3])}
    except Exception as e: return "ERR " + repr(e)[:80]
def docprops(doc):
    p = doc.DocumentProperties
    ud = tryf(lambda: [[n, v(p.UserDefinedProperties.getPropertyValue(n))] for n in sorted(i.Name for i in p.UserDefinedProperties.getPropertySetInfo().getProperties())], [])
    return {"title": p.Title, "subject": p.Subject, "author": p.Author, "keywords": list(p.Keywords), "desc": p.Description, "gen": p.Generator[:20] if p.Generator else "",
            "print_date": v(p.PrintDate), "printed_by": p.PrintedBy, "creation_date": v(p.CreationDate), "template": p.TemplateName, "edit_cycles": p.EditingCycles,
            "autoload": [p.AutoloadURL, p.AutoloadSecs], "defaulttarget": p.DefaultTarget, "userdef": ud}
def basic(doc):
    out = {}
    for k in ("BasicLibraries", "DialogLibraries"):
        out[k] = tryf(lambda: sorted(getattr(doc, k).ElementNames), "n/a")
    return out

def writer(doc):
    o = {}
    txt = doc.Text.String
    o["text_hash"] = h(txt); o["chars"] = len(txt)
    paras = []; ncell = 0
    e = doc.Text.createEnumeration(); tbl = 0; port_hash = []
    while e.hasMoreElements() and len(paras) < LIM:
        p = e.nextElement()
        if p.supportsService("com.sun.star.text.Paragraph"):
            paras.append([len(p.String), g(p, "ParaStyleName"), g(p, "ParaBackColor"), g(p, "ParaBackTransparent"), g(p, "FillStyle"), g(p, "FillColor"), g(p, "FillTransparence"),
                          g(p, "CharHeight"), g(p, "ParaAdjust"), g(p, "ParaHyphenationMaxLeadingChars"), g(p, "ParaHyphenationMaxTrailingChars"), g(p, "ParaHyphenationMaxHyphens")])
            try:
                pe = p.createEnumeration(); sig = []
                while pe.hasMoreElements():
                    t = pe.nextElement(); ty = t.TextPortionType; sig.append(ty)
                    if ty == "TextField": sig.append(tryf(lambda: t.TextField.ServiceName if hasattr(t.TextField, "ServiceName") else ",".join(t.TextField.SupportedServiceNames)))
                    u = g(t, "HyperLinkURL")
                    if u: sig.append("URL:" + str(u))
                port_hash.append(h("|".join(map(str, sig))))
            except Exception: port_hash.append("n/a")
        else: tbl += 1
    o["paras"] = paras; o["nparas"] = len(paras); o["port_hash"] = h("".join(port_hash)); o["top_tables"] = tbl
    ps = doc.StyleFamilies.getByName("PageStyles")
    pk = ["Width", "Height", "BackColor", "BackTransparent", "FillStyle", "FillColor", "FillTransparence", "HeaderIsOn", "HeaderBackColor", "HeaderBackTransparent", "HeaderFillStyle",
          "HeaderFillColor", "HeaderFillTransparence", "FooterIsOn", "FooterBackColor", "FooterBackTransparent", "FooterFillStyle", "FooterFillColor", "FooterFillTransparence",
          "GridMode", "GridBaseWidth", "GridBaseHeight", "GridLines", "GridSnapToChars", "StandardPageMode", "GridRubyHeight", "GridDisplay", "GridPrint", "IsLandscape", "PageStyleLayout"]
    o["pagestyles"] = {n: gs(ps.getByName(n), pk) + [bool(g(ps.getByName(n), "BackGraphicURL")), bool(g(ps.getByName(n), "HeaderBackGraphicURL")), bool(g(ps.getByName(n), "FooterBackGraphicURL"))] for n in ps.ElementNames}
    pa = doc.StyleFamilies.getByName("ParagraphStyles")
    ak = ["ParaBackColor", "ParaBackTransparent", "FillStyle", "FillColor", "FillTransparence", "CharHeight", "ParaHyphenationMaxLeadingChars", "ParaHyphenationMaxTrailingChars", "ParaKeepTogether", "ParaAdjust"]
    o["parastyles"] = {n: gs(pa.getByName(n), ak) for n in pa.ElementNames}
    cs = doc.StyleFamilies.getByName("CharacterStyles")
    o["charstyles"] = {n: gs(cs.getByName(n), ["CharHeight", "CharWeight", "CharColor"]) for n in cs.ElementNames}
    fs = doc.StyleFamilies.getByName("FrameStyles")
    o["framestyles"] = {n: gs(fs.getByName(n), ["BackColor", "FillStyle", "FillTransparence", "FillColor", "Width", "Height"]) for n in fs.ElementNames}
    tabs = []
    for i in range(min(doc.TextTables.Count, 100)):
        t = doc.TextTables.getByIndex(i)
        cn = list(t.CellNames)
        tabs.append({"name": t.Name, "rows": t.Rows.Count, "cols": t.Columns.Count, "cells": len(cn), "cellhash": h("|".join(cn) + "".join(tryf(lambda: t.getCellByName(c).String, "") for c in cn[:300])),
                     "split": [tryf(lambda: t.Rows.getByIndex(r).IsSplitAllowed) for r in range(min(t.Rows.Count, 50))], "bg": g(t, "BackColor"), "kt": g(t, "Split")})
    o["tables"] = tabs
    fld = []
    en = doc.TextFields.createEnumeration()
    while en.hasMoreElements() and len(fld) < LIM:
        f = en.nextElement()
        sn = tryf(lambda: ",".join(f.SupportedServiceNames))
        r = [sn.replace("com.sun.star.text.", "")[:120]]
        if "GetReference" in sn: r += gs(f, ["ReferenceFieldPart", "ReferenceFieldSource", "SourceName", "SequenceNumber"])
        if "URL" in sn: r += gs(f, ["URL", "Representation"])
        fld.append(r)
    o["fields"] = fld
    o["footnotes"] = [doc.Footnotes.Count, [tryf(lambda: doc.Footnotes.getByIndex(i).String[:30], "") for i in range(min(doc.Footnotes.Count, 30))]]
    o["endnotes"] = [doc.Endnotes.Count, [tryf(lambda: doc.Endnotes.getByIndex(i).String[:30], "") for i in range(min(doc.Endnotes.Count, 30))]]
    fr = []
    for coll, name in ((doc.TextFrames, "frame"), (doc.GraphicObjects, "graphic"), (doc.EmbeddedObjects, "embedded")):
        for n in coll.ElementNames[:100]:
            f = coll.getByName(n)
            fr.append([name, n, g(f, "AnchorType"), g(f, "AnchorPageNo"), g(f, "Width"), g(f, "Height"), g(f, "BackColor"), g(f, "FillTransparence"), g(f, "Transparency"), g(f, "FillStyle"), g(f, "HoriOrientPosition"), g(f, "VertOrientPosition")])
            if name == "embedded": fr[-1].append(embedded(tryf(lambda: f.Model, None)))
    o["frames"] = fr
    sec = []
    for n in doc.TextSections.ElementNames[:100]:
        s = doc.TextSections.getByName(n); sec.append([n, g(s, "FileLink"), g(s, "LinkRegion"), g(s, "IsProtected"), g(s, "IsVisible")])
    o["sections"] = sec
    rl = []
    try:
        en = doc.Redlines.createEnumeration()
        while en.hasMoreElements() and len(rl) < 100:
            r = en.nextElement(); rl.append([g(r, "RedlineType"), g(r, "RedlineAuthor"), g(r, "RedlineDateTime"), g(r, "RedlineComment"), g(r, "RedlineSuccessorData") != "n/a" and bool(g(r, "RedlineSuccessorData"))])
    except Exception: rl = "n/a"
    o["redlines"] = rl
    o["bookmarks"] = tryf(lambda: sorted(doc.Bookmarks.ElementNames))
    o["refmarks"] = tryf(lambda: sorted(doc.ReferenceMarks.ElementNames))
    o["indexes"] = tryf(lambda: doc.DocumentIndexes.Count)
    o["shapes"] = [shape(doc.DrawPage.getByIndex(i)) for i in range(min(doc.DrawPage.Count, 150))]
    o["forms"] = forms_of(doc.DrawPage)
    o["nstyles"] = {k: len(doc.StyleFamilies.getByName(k).ElementNames) for k in doc.StyleFamilies.ElementNames}
    return o

def calc(doc):
    o = {"sheets": []}
    nfs = doc.NumberFormats; ftype = {}
    def ft(k):
        if k not in ftype: ftype[k] = tryf(lambda: nfs.getByKey(k).Type)
        return ftype[k]
    for i in range(doc.Sheets.Count):
        sh = doc.Sheets.getByIndex(i)
        cur = sh.createCursor(); cur.gotoEndOfUsedArea(False)
        rng = sh.getCellRangeByPosition(0, 0, cur.RangeAddress.EndColumn, cur.RangeAddress.EndRow)
        if cur.RangeAddress.EndColumn > 300 or cur.RangeAddress.EndRow > 3000:
            rng = sh.getCellRangeByPosition(0, 0, min(cur.RangeAddress.EndColumn, 300), min(cur.RangeAddress.EndRow, 3000))
        da = rng.DataArray; fa = rng.FormulaArray
        so = {"name": sh.Name, "end": [cur.RangeAddress.EndColumn, cur.RangeAddress.EndRow], "data_hash": h(repr(da)), "formula_hash": h(repr(fa)),
              "nformulas": sum(1 for r in fa for c in r if isinstance(c, str) and c.startswith("="))}
        cells = []
        nrow = min(len(da), 400)
        for r in range(nrow):
            for c in range(min(len(da[r]), 60)):
                if da[r][c] != "" and da[r][c] is not None:
                    cell = rng.getCellByPosition(c, r)
                    cells.append([r, c, ft(cell.NumberFormat), cell.getError(), cell.Type.value])
        so["cells_hash"] = h(repr(cells)); so["ncells"] = len(cells)
        so["errors"] = sum(1 for x in cells if x[3])
        so["annotations"] = [sh.Annotations.Count, h("".join(tryf(lambda: sh.Annotations.getByIndex(k).String, "") for k in range(min(sh.Annotations.Count, 100))))]
        so["charts"] = {n: chart_info(tryf(lambda: sh.Charts.getByName(n).EmbeddedObject, None)) for n in sh.Charts.ElementNames[:30]}
        urls = []
        for r in range(nrow):
            for c in range(min(len(da[r]), 40)):
                if isinstance(fa[r][c], str) and fa[r][c] and not fa[r][c].startswith("="):
                    cell = rng.getCellByPosition(c, r)
                    try:
                        en = cell.Text.createEnumeration()
                        while en.hasMoreElements():
                            pe = en.nextElement().createEnumeration()
                            while pe.hasMoreElements():
                                pt = pe.nextElement()
                                if pt.TextPortionType == "TextField": urls.append([r, c, tryf(lambda: pt.TextField.URL), tryf(lambda: pt.TextField.Representation)])
                    except Exception: pass
                    if len(urls) > 100: break
        so["urls"] = urls
        so["shapes"] = [shape(sh.DrawPage.getByIndex(k)) for k in range(min(sh.DrawPage.Count, 100))]
        so["forms"] = forms_of(sh.DrawPage)
        so["pagestyle"] = sh.PageStyle
        so["flags"] = gs(sh, ["IsVisible", "IsProtected"])
        o["sheets"].append(so)
    o["dde"] = tryf(lambda: [[n, doc.DDELinks.getByName(n).Application, doc.DDELinks.getByName(n).Topic, doc.DDELinks.getByName(n).Item] for n in doc.DDELinks.ElementNames])
    o["names"] = tryf(lambda: sorted(doc.NamedRanges.ElementNames))
    ps = doc.StyleFamilies.getByName("PageStyles")
    o["pagestyles"] = {n: gs(ps.getByName(n), ["BackColor", "BackTransparent", "FillStyle", "HeaderIsOn", "FooterIsOn", "IsLandscape", "Width", "Height", "ScaleToPagesX"]) for n in ps.ElementNames}
    cs = doc.StyleFamilies.getByName("CellStyles")
    o["cellstyles"] = {n: gs(cs.getByName(n), ["CellBackColor", "IsCellBackgroundTransparent", "CharHeight", "NumberFormat"]) for n in cs.ElementNames}
    return o

def draw(doc):
    o = {"pages": []}
    pages = doc.DrawPages
    for i in range(pages.Count):
        pg = pages.getByIndex(i)
        po = {"name": tryf(lambda: pg.Name), "shapes": [shape(pg.getByIndex(k)) for k in range(min(pg.Count, 300))], "forms": forms_of(pg),
              "layout": g(pg, "Layout"), "bg": tryf(lambda: gs(pg.Background, ["FillStyle", "FillColor", "FillTransparence", "FillGradientName"])), "master": tryf(lambda: pg.MasterPage.Name)}
        if has(doc, "com.sun.star.presentation.PresentationDocument"):
            po["notes"] = tryf(lambda: [pg.NotesPage.getByIndex(k).String[:40] for k in range(min(pg.NotesPage.Count, 20)) if hasattr(pg.NotesPage.getByIndex(k), "String")], "n/a")
        o["pages"].append(po)
    masters = doc.MasterPages
    o["masters"] = [[tryf(lambda: masters.getByIndex(i).Name), len(shape_list(masters.getByIndex(i)))] for i in range(masters.Count)]
    for fam in ("graphics", "Default"):
        pass
    try:
        gf = doc.StyleFamilies.getByName("graphics")
        o["graphicstyles"] = {n: gs(gf.getByName(n), ["FillStyle", "FillColor", "FillTransparence", "CharHeight", "LineStyle", "LineColor", "TextWritingMode"]) for n in gf.ElementNames}
    except Exception: pass
    return o
def shape_list(pg): return [shape(pg.getByIndex(k)) for k in range(min(pg.Count, 300))]

DESK = desktop(connect(port))
os.makedirs(outdir, exist_ok=True)
for f in files:
    dest = os.path.join(outdir, os.path.basename(f) + ".json")
    if os.path.exists(dest): continue
    open(os.path.join(outdir, ".current"), "w").write(f)
    out = {}
    try:
        doc = load(DESK, f)
        out["type"] = ("writer" if has(doc, "com.sun.star.text.TextDocument") else "calc" if has(doc, "com.sun.star.sheet.SpreadsheetDocument")
                       else "impress" if has(doc, "com.sun.star.presentation.PresentationDocument") else "draw" if has(doc, "com.sun.star.drawing.DrawingDocument") else "other")
        out["props"] = tryf(lambda: docprops(doc), "ERR")
        out["basic"] = basic(doc)
        try:
            out["body"] = {"writer": writer, "calc": calc, "impress": draw, "draw": draw}.get(out["type"], lambda d: {})(doc)
        except Exception as e:
            out["body_error"] = repr(e)[:200]
        doc.close(True)
    except Exception as e:
        out["error"] = repr(e)[:200]
    json.dump(out, open(dest, "w"), sort_keys=True, indent=0, default=str)
print("done", len(files))
