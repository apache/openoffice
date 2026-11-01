# usage: python3 -I mkexpect.py <cap-dir> <tag>
# Writes the expect values of the checks-*.json files.
#  - styled paragraphs and paragraph styles: what AOO's own form gives after a reload (<cap-dir>/<tag>_aoo-form.json,
#    captured by roundtrip.py --capture from aoo-form.odt, written by make-aoo.py with the same fills);
#  - direct paragraphs: derived from the XML text of foreign-v8.odt (an unnamed gradient or hatch cannot be restored, so
#    fo:background-color is what remains; a bitmap comes from style:background-image);
#  - edge documents: the same, except the styles an edge changes, which take <cap-dir>/<tag>_<edge>.json (captured with
#    libraries without the import) or the value stated below.
import copy, json, os, re, sys, zipfile
cap, tag = sys.argv[1], sys.argv[2]
here = os.path.dirname(os.path.abspath(__file__))
C = lambda d: json.load(open(os.path.join(cap, "%s_%s.json" % (tag, d))))
checks = json.load(open(os.path.join(here, "checks.json")))
ref = C("aoo-form")

def rgb(s): return int(s[1:], 16)
z = zipfile.ZipFile(os.path.join(here, "foreign-v8.odt")); content = z.read("content.xml").decode()
direct = {}
for m in re.finditer(r'<text:p text:style-name="(P\d+)">direct (\w+)</text:p>', content):
    sty = re.search(r'<style:style style:name="%s".*?</style:style>' % m.group(1), content, re.S).group(0)
    kind = m.group(2)
    fill = re.search(r'<loext:graphic-properties draw:fill="(\w+)"(?: draw:fill-color="(#\w+)")?(?: draw:opacity="(\d+)%")?', sty)
    bg = re.search(r'fo:background-color="(#\w+)"', sty)
    rep = re.search(r'<style:background-image [^>]*style:repeat="([\w-]+)"', sty)
    if fill.group(1) == "solid":
        direct[kind] = ["SOLID", rgb(fill.group(2)), 100 - int(fill.group(3) or 100) if fill.group(3) else 0, None, False]
    elif fill.group(1) == "bitmap" and "<style:background-image " in sty:
        direct[kind] = ["BITMAP", None, 0, {"stretch": "STRETCH", "no-repeat": "NO_REPEAT"}.get(rep.group(1) if rep else "", "REPEAT"), True]
    else:
        direct[kind] = ["SOLID", rgb(bg.group(1)), 0, None, False]
direct_rows = [["direct " + k] + v for k, v in direct.items()]

def build(styled, styles, direct_rows):
    return {"styled_paragraphs": styled, "para_styles": styles, "direct_paragraphs": direct_rows}

base = build(ref["styled_paragraphs"], ref["para_styles"], direct_rows)

def write(name, exp, only=None, angle=None):
    ck = copy.deepcopy(checks)
    out = []
    for c in ck:
        if only and c["name"] not in only: continue
        e = copy.deepcopy(exp[c["name"]])
        if angle is not None:
            for r in e:
                if len(r) > 5 and r[5] is not None and r[1] == "GRADIENT": r[5][3] = angle
        c["expect"] = e
        out.append(c)
    json.dump(out, open(os.path.join(here, name), "w"), indent=1); open(os.path.join(here, name), "a").write("\n")

def swap(exp, edge, labels):
    """replace the rows named in labels by the rows of the capture of <edge> taken without the import"""
    red = C(edge); exp = copy.deepcopy(exp)
    for k in ("styled_paragraphs", "para_styles"):
        exp[k] = [next(r for r in red[k] if r[0] == row[0]) if row[0] in labels else row for row in exp[k]]
    return exp

write("checks.json", base)
write("checks-v13-v14.json", base, angle=0)
write("checks-aoo-form.json", base, only=("styled_paragraphs", "para_styles"))
write("checks-edge-unknown-name.json", swap(base, "edge-unknown-name", ("styled gradient", "styled gradient_transp", "S_gradient", "S_gradient_transp")))
write("checks-edge-empty.json", swap(base, "edge-empty", ("styled gradient", "S_gradient")))
write("checks-edge-nonparagraph.json", base)
write("checks-edge-duplicate-nested.json", base)
both = swap(base, "edge-both-forms", ("styled solid", "S_solid"))
for k in ("styled_paragraphs", "para_styles"):
    for r in both[k]:
        if r[0] in ("styled solid_transp", "S_solid_transp"):   # native element after the extension one: its colour wins, the extension's opacity survives
            r[2] = 16711680; r[3] = 40; r[12] = 1711210496
write("checks-edge-both-forms.json", both)
write("checks-edge-unknown-attr.json", base)
ua = json.load(open(os.path.join(here, "checks-unknown-attr.json")))
ua[0]["expect"] = C("edge-unknown-attr_ua")["style_user_attributes"]   # what the styles carry without the import: the attribute must not be added
json.dump(ua, open(os.path.join(here, "checks-unknown-attr.json"), "w"), indent=1); open(os.path.join(here, "checks-unknown-attr.json"), "a").write("\n")
print("written; direct rows:", len(direct_rows))
