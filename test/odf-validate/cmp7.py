# usage: cmp7.py <old_run> <new_run> <mode:strict|ext> <out.json> [-v]
# Masked structural comparison of two corpus output sets (<run>/out/*).
# Layer 1: environment/noise masks only (RAND values, dates, ids, printer config, relative-href depth, auto-style numbering).
# Layer 2: additionally applies the INTENDED edits of the fixes under test to both sides, so the remaining diff = unexplained.
# Counts, per package, how often each fix's marker is present on the old side and on the new side (exercise map input).
import zipfile, sys, re, os, hashlib, difflib, json, collections
import xml.etree.ElementTree as ET
old, new, mode, outjson = sys.argv[1:5]
verbose = "-v" in sys.argv
NS = dict(style="urn:oasis:names:tc:opendocument:xmlns:style:1.0", office="urn:oasis:names:tc:opendocument:xmlns:office:1.0",
          text="urn:oasis:names:tc:opendocument:xmlns:text:1.0", draw="urn:oasis:names:tc:opendocument:xmlns:drawing:1.0",
          table="urn:oasis:names:tc:opendocument:xmlns:table:1.0", form="urn:oasis:names:tc:opendocument:xmlns:form:1.0",
          chart="urn:oasis:names:tc:opendocument:xmlns:chart:1.0", meta="urn:oasis:names:tc:opendocument:xmlns:meta:1.0",
          fo="urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0", svg="urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0",
          xlink="http://www.w3.org/1999/xlink", config="urn:oasis:names:tc:opendocument:xmlns:config:1.0",
          dc="http://purl.org/dc/elements/1.1/")
def q(p, l): return "{%s}%s" % (NS[p], l)
STYLE_NAME = q("style", "name")

def mask_text(d):
    d = re.sub(rb'Pictures/[0-9A-F]{24,40}', b'Pictures/H', d)
    d = re.sub(rb'text:change-id="ct[0-9]+"', b'text:change-id="CT"', d)
    d = re.sub(rb'<text:(time|date)\b([^>]*)>[^<]*</text:\1>', rb'<text:\1\2>T</text:\1>', d)
    d = re.sub(rb'office:value="[-0-9.eE+]+"', b'office:value="#"', d)
    d = re.sub(rb'<text:p>[0-9.]+</text:p>', b'<text:p>#</text:p>', d)
    d = re.sub(rb'(text:time-value|text:date-value|text:time|text:date)="[^"]*"', rb'\1="T"', d)
    d = re.sub(rb'<meta:(creation-date|editing-duration)>[^<]*</meta:\1>', rb'<meta:\1>T</meta:\1>', d)
    d = re.sub(rb'<dc:date>[^<]*</dc:date>', b'<dc:date>T</dc:date>', d)
    d = re.sub(rb'(xml:id|text:id|form:id|text:continue-list)="[^"]*"', rb'\1="ID"', d)
    # environment: default printer of the host, relative links depend on the output directory depth
    d = re.sub(rb'(<config:config-item config:name="(?:PrinterName|PrinterSetup)"[^>]*>)[^<]*', rb'\1PRN', d)
    d = re.sub(rb'(xlink:href|form:image-data)="(?:\.\./)+', rb'\1="../*/', d)
    # host calendar date moved from 2026-10-22 (baseline run) to 2026-10-23 (TODAY()-style cells)
    d = re.sub(rb'2026-10-2[23]', b'DATE', d)
    d = re.sub(rb'10/2[23]/26', b'DATE', d)
    # fields.odt: weekday name length (Donnerstag vs Freitag) changes the document character count
    d = re.sub(rb'(character-count[^>]*?>|meta:character-count=")\d+', rb'\1N', d)
    return d
def isref(k):
    return k.endswith("style-name") or k.endswith("page-layout-name") or k.endswith("master-page-name") or k.endswith("class-names") or k.endswith("style-names")
def ser(e, m, defn=False, out=None):
    out = [] if out is None else out
    attrs = sorted((k, m.get(v, v) if isref(k) and not (defn and k == STYLE_NAME) else v) for k, v in e.attrib.items() if not (defn and k == STYLE_NAME))
    out.append("<" + e.tag + " " + " ".join(f'{k}="{v}"' for k, v in attrs) + ">" + (e.text or "").strip())
    for c in e: ser(c, m, False, out); out.append((c.tail or "").strip())
    out.append("</>")
    return "".join(out)

def canon(root, dedupe):
    autos = [c for ac in root.iter(q("office", "automatic-styles")) for c in ac if STYLE_NAME in c.attrib]
    m = {}
    for _ in range(4):
        nm = {}
        for c in autos: nm[c.get(STYLE_NAME)] = "S" + hashlib.md5(ser(c, m, True).encode()).hexdigest()[:10]
        if nm == m: break
        m = nm
    lines = []
    def walk(e, depth):
        if e.tag == q("office", "automatic-styles"):
            lines.append("<automatic-styles>")
            ss = sorted(ser(c, m, True) for c in e)
            if dedupe: ss = sorted(set(ss))
            for s in ss: lines.append("  " + s)
            lines.append("</automatic-styles>")
            return
        attrs = sorted((k, m.get(v, v) if isref(k) else v) for k, v in e.attrib.items())
        lines.append(" " * depth + "<" + e.tag + " " + " ".join(f'{k}="{v}"' for k, v in attrs) + ">" + ((e.text or "").strip()))
        for c in e: walk(c, depth + 1)
    walk(root, 0)
    return lines

def pct(v):
    m = re.fullmatch(r'(-?[0-9.]+)%', v); return float(m.group(1)) if m else None
def length_pos(v):
    m = re.fullmatch(r'(-?[0-9.]+)([a-z%]*)', v); return float(m.group(1)) > 0 if m else True

def normalise(root, cnt, strict, is_old):
    """Apply the intended edits of the fixes (both sides). cnt[key] counts how often an edit was needed on this side."""
    parent = {c: p for p in root.iter() for c in p}
    def bump(k): cnt[k] += 1
    def drop_attr(e, name): del e.attrib[name]
    for e in list(root.iter()):
        t = e.tag; a = e.attrib
        if t == q("draw", "connector") and q("svg", "viewBox") in a: drop_attr(e, q("svg", "viewBox")); bump("cc5a09edf8 viewBox")
        if t in (q("text", "a"), q("text", "section-source"), q("meta", "auto-reload")):
            k = {q("text", "a"): "4b1c1a1375 text:a xlink:type", q("text", "section-source"): "dd43dd10dc section-source xlink:type", q("meta", "auto-reload"): "60942d1a9c auto-reload xlink:type"}[t]
            if q("xlink", "type") in a: drop_attr(e, q("xlink", "type")); bump(k)
            elif q("xlink", "href") in a or t != q("text", "section-source"): bump(k + " MISSING")
        if t in (q("office", "document-content"), q("office", "document-styles")) and q("office", "version") in a:
            drop_attr(e, q("office", "version")); bump("c7615ea10d office:version present")
        elif t in (q("office", "document-content"), q("office", "document-styles")): bump("c7615ea10d office:version MISSING")
        if t == q("style", "page-layout-properties") and q("style", "layout-grid-standard-mode") in a:
            drop_attr(e, q("style", "layout-grid-standard-mode")); bump("f18a0e638f layout-grid-standard-mode in page layouts")
        if t == q("meta", "print-date"):
            if not re.match(r'\d{4,}-\d\d-\d\dT', (e.text or "")): parent[e].remove(e); bump("150358e458 print-date bogus")
        if t == q("form", "property") and q("office", "value") in a:
            if a[q("office", "value")] == "": bump("e64781e76d form office:value empty")
            a[q("office", "value")] = "#"
        for atn in (q("draw", "opacity"), q("draw", "image-opacity")):
            if atn in a:
                p = pct(a[atn])
                if p is not None and (p > 100 or p < 0): bump("d47ac291e9 opacity out of range"); a[atn] = "%g%%" % max(0, min(100, p))
        if q("fo", "font-size") in a and not length_pos(a[q("fo", "font-size")]): drop_attr(e, q("fo", "font-size")); bump("efbe32fc4d font-size<=0")
        for hn in ("hyphenation-push-char-count", "hyphenation-remain-char-count"):
            if q("fo", hn) in a and a[q("fo", hn)] == "0": drop_attr(e, q("fo", hn)); bump("fe667bb0b4 hyphenation 0")
        if q("office", "boolean-value") in a and a[q("office", "boolean-value")] not in ("true", "false"):
            del a[q("office", "boolean-value")]; a[q("office", "value-type")] = "float"; a[q("office", "value")] = "#"; bump("4250607123 boolean non-0/1")
        if q("table", "number-columns-spanned") in a and a[q("table", "number-columns-spanned")] == "0": drop_attr(e, q("table", "number-columns-spanned")); bump("895d935e38 colspan 0")
        if q("text", "anchor-page-number") in a and a[q("text", "anchor-page-number")] == "0": drop_attr(e, q("text", "anchor-page-number")); bump("b070ffe226 anchor-page 0")
        if t == q("text", "note-ref") and a.get(q("text", "reference-format")) in ("category-and-value", "caption", "value"):
            a[q("text", "reference-format")] = "text"; bump("9eca93128d note-ref format")
        if t == q("table", "dde-link"):
            tabs = [c for c in e if c.tag == q("table", "table")]
            if not tabs: bump("f8ad6c41e5 dde-link without table")
            else:
                tb = tabs[0]
                kids = [c.tag for c in tb]
                if kids == [q("table", "table-columns"), q("table", "table-rows")] and len(list(tb.iter(q("table", "table-cell")))) == 1 and not tb.attrib.get(q("table", "name")) is None and False: pass
                if kids == [q("table", "table-columns"), q("table", "table-rows")] and len(list(tb.iter(q("table", "table-cell")))) == 1 and not "".join(tb.itertext()).strip() and len(list(tb.iter())) == 6 and not any(x.attrib for x in tb.iter(q("table", "table-cell"))):
                    e.remove(tb); bump("f8ad6c41e5 dde-link wrapper table"); 
        if t == q("chart", "symbol-image") and q("xlink", "href") not in a: parent[e].remove(e); bump("d6bd426c78 empty symbol-image")
        if t in (q("table", "table-columns"), q("table", "table-rows")) and len(e) == 0 and parent.get(e) is not None and parent[e].tag == q("table", "table") and root.tag.endswith("document-content"):
            # chart internal data (empty wrappers)
            parent[e].remove(e); bump("26a3e0ceb0 empty chart table wrapper")
    if strict:
        norm_strict(root, cnt, parent)

PARA_FAM = {"paragraph", "text"}
def norm_strict(root, cnt, parent):
    chart_shapes_dropped = False
    for e in list(root.iter()):
        t = e.tag; a = e.attrib
        if t == q("style", "style") and a.get(q("style", "family")) == "paragraph":
            for c in list(e):
                if c.tag == q("style", "graphic-properties") and any(k.startswith("{%s}fill" % NS["draw"]) or k == q("draw", "opacity") or k.startswith("{%s}fill-" % NS["draw"]) for k in c.attrib):
                    e.remove(c); cnt["ef0a8aeea9 para draw:fill"] += 1
        if t in (q("style", "page-layout-properties"), q("style", "header-footer-properties")):
            for k in list(a):
                if k.startswith("{%s}fill" % NS["draw"]) or k == q("draw", "opacity") or k.startswith("{%s}opacity" % NS["draw"]):
                    del a[k]; cnt["e86e06dbff page/hf draw:fill"] += 1
        if t == q("style", "header-footer-properties"):
            for c in list(e):
                if c.tag == q("style", "background-image"):
                    if not c.attrib and len(c) == 0: e.remove(c); cnt["83b30299d0 hf background-image empty"] += 1
                    else: e.remove(c); cnt["83b30299d0 hf background-image with content"] += 1
        if q("style", "layout-grid-snap-to-characters") in a:
            a[q("style", "layout-grid-snap-to")] = a.pop(q("style", "layout-grid-snap-to-characters")); cnt["be94c9a190 grid-snap rename"] += 1
        if t == q("style", "table-row-properties") and q("style", "keep-together") in a: del a[q("style", "keep-together")]; cnt["58c72c10be row keep-together"] += 1
        if t in (q("draw", "object"), q("draw", "object-ole")):
            for c in list(e):
                if c.tag == q("text", "p"):
                    cnt["57b28c93a4 draw:object text:p " + ("with text" if "".join(c.itertext()).strip() else "empty")] += 1; e.remove(c)
        if t == q("form", "combobox") and q("form", "xforms-list-source") in a: del a[q("form", "xforms-list-source")]; cnt["1028eb24a1 combobox xforms-list-source"] += 1
        if t == q("chart", "chart"):
            for c in list(e):
                if c.tag.startswith("{%s}" % NS["draw"]): e.remove(c); cnt["edc67281bb chart additional shape " + c.tag.split("}")[1]] += 1; chart_shapes_dropped = True
        if t.startswith("{%s}" % NS["style"]) and t.endswith("-properties") and q("text", "enable-numbering") in a:
            del a[q("text", "enable-numbering")]; cnt["e9b28885e5 alien text:enable-numbering"] += 1
        if t == q("text", "changed-region"):
            for c in list(e):
                if c.tag in (q("text", "insertion"),) and e.find(q("text", "insertion")) is not c: pass
    # P3: successor change = an text:insertion/deletion nested in a changed-region that already carries a change; nested `office:change-info` with attribute form
    for e in list(root.iter(q("office", "change-info"))):
        if q("office", "chg-author") in e.attrib or q("office", "chg-date-time") in e.attrib:
            p = parent.get(e)
            if p is not None and p.tag in (q("text", "insertion"), q("text", "deletion"), q("text", "format-change")) and parent.get(p) is not None and len(p) == 1:
                parent[p].remove(p); cnt["c8b13970b2 redline successor change-info"] += 1
            elif p is not None: p.remove(e); cnt["c8b13970b2 redline successor change-info (other)"] += 1
    # empty property elements left behind by dropped alien attributes
    for e in list(root.iter()):
        for c in list(e):
            if c.tag.startswith("{%s}" % NS["style"]) and c.tag.endswith("-properties") and not c.attrib and len(c) == 0 and not (c.text or "").strip():
                e.remove(c); cnt["empty style:*-properties dropped"] += 1
    if chart_shapes_dropped:
        for _ in range(3):
            used = {v for e in root.iter() for k, v in e.attrib.items() if isref(k)}
            for ac in root.iter(q("office", "automatic-styles")):
                for st in list(ac):
                    if st.get(STYLE_NAME) is not None and st.get(STYLE_NAME) not in used and st.tag == q("style", "style") and st.get(q("style", "family")) in ("graphic", "paragraph"):
                        ac.remove(st); cnt["orphan auto style after chart shapes dropped"] += 1
    # drop empty paragraph auto styles and the references to them (merge / omission effect of row 1)
    empty = set()
    for ac in root.iter(q("office", "automatic-styles")):
        for s in list(ac):
            if s.tag == q("style", "style") and len(s) == 0 and set(s.attrib) <= {STYLE_NAME, q("style", "family"), q("style", "parent-style-name")} and q("style", "parent-style-name") not in s.attrib and s.get(q("style", "family")) in ("paragraph", "text", "graphic", "presentation"):
                empty.add(s.get(STYLE_NAME)); ac.remove(s)
    if empty:
        for e in root.iter():
            for k in list(e.attrib):
                if isref(k) and e.attrib[k] in empty and not (e.tag == q("style", "style")): del e.attrib[k]

def load(path, name, strict, dedupe, cnt, is_old, raw):
    z = zipfile.ZipFile(path); data = z.read(name)
    d = mask_text(data)
    try: root = ET.fromstring(d)
    except ET.ParseError: return [d.decode("utf8", "replace")], [d.decode("utf8", "replace")]
    raw_lines = canon(ET.fromstring(d), False)
    if raw: return raw_lines, None
    normalise(root, cnt, strict, is_old)
    return raw_lines, canon(root, dedupe)

strict = mode == "strict"
res = {}
tot = collections.Counter()
for f in sorted(os.listdir(old + "/out")):
    r = {"raw_diff_streams": [], "norm_diff_streams": [], "old": collections.Counter(), "new": collections.Counter()}
    if not os.path.exists(new + "/out/" + f): res[f] = {"missing_in_new": True}; continue
    pa, pb = old + "/out/" + f, new + "/out/" + f
    za, zb = zipfile.ZipFile(pa), zipfile.ZipFile(pb)
    nn = lambda z: sorted(re.sub(r'Pictures/[0-9A-F]{24,40}', 'Pictures/H', n) for n in z.namelist())
    if nn(za) != nn(zb): r["names_diff"] = sorted(set(nn(za)) ^ set(nn(zb)))
    for n in sorted(za.namelist()):
        if n.endswith("/") or not n.endswith(".xml") or n not in zb.namelist(): continue
        ca, cb = collections.Counter(), collections.Counter()
        ra, xa = load(pa, n, strict, strict, ca, True, False)
        rb, xb = load(pb, n, strict, strict, cb, False, False)
        r["old"].update(ca); r["new"].update(cb)
        if ra != rb: r["raw_diff_streams"].append(n)
        if xa != xb:
            r["norm_diff_streams"].append(n)
            diffl = [l[:300] for l in difflib.unified_diff(xa, xb, lineterm="", n=0) if not l.startswith(("---", "+++", "@@"))]
            r.setdefault("diff_lines", {})[n] = diffl[:40]
            if verbose:
                print("UNEXPLAINED", f, n)
                for l in diffl[:20]: print("    ", l)
    r["old"] = dict(r["old"]); r["new"] = dict(r["new"])
    res[f] = r
json.dump(res, open(outjson, "w"), indent=1)
nr = sum(1 for v in res.values() if v.get("raw_diff_streams")); nn_ = sum(1 for v in res.values() if v.get("norm_diff_streams") or v.get("names_diff") or v.get("missing_in_new"))
print(f"packages {len(res)}; differing after env masks: {nr}; differing after intended-edit normalisation: {nn_}")
