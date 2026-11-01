# Known limitations of the import these documents exercise:
#  - a fill is ignored when its gradient, hatch or bitmap definition comes after the style that uses it;
#  - multi-stop gradients, unnamed direct gradients and hatches, and the opacity of a direct bitmap are not imported;
#  - draw:angle with a unit suffix such as "30deg" is read as 0 (existing behaviour of the gradient import, unrelated to the fill element).
# usage: python3 -I make.py <other-suite-soffice> <scratch-dir>
# Writes foreign-v8.odt, foreign-v3.odt, foreign-v11.odt with the other suite saving at ODF version setting 8, 3, 11
# (/org.openoffice.Office.Common/Save/ODF DefaultVersion), using gen.bas as macro Standard.Module1.Gen in a throw-away profile,
# then derives the edge-*.odt variants by editing the XML text of foreign-v8.odt.
import os, re, subprocess, sys, time, zipfile
from xml.sax.saxutils import escape

soffice, scratch = sys.argv[1], sys.argv[2]
here = os.path.dirname(os.path.abspath(__file__))
KINDS = "solid,solid_transp,gradient,gradient_transp,hatch,hatch_nobg,bitmap_tile,bitmap_stretch,bitmap_once,bitmap_transp"
XCU = """<?xml version="1.0" encoding="UTF-8"?>
<oor:items xmlns:oor="http://openoffice.org/2001/registry" xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
<item oor:path="/org.openoffice.Office.Common/Save/ODF"><prop oor:name="DefaultVersion" oor:op="fuse"><value>%d</value></prop></item>
<item oor:path="/org.openoffice.Office.Common/Security/Scripting"><prop oor:name="MacroSecurityLevel" oor:op="fuse"><value>0</value></prop></item>
</oor:items>
"""
MODULE = ('<?xml version="1.0" encoding="UTF-8"?>\n<!DOCTYPE script:module PUBLIC "-//OpenOffice.org//DTD OfficeDocument 1.0//EN" "module.dtd">\n'
          '<script:module xmlns:script="http://openoffice.org/2000/script" script:name="Module1" script:language="StarBasic">%s</script:module>\n')
OPTS = ["--headless", "--nofirststartwizard", "--norestore", "--nologo"]

def wait_gone(prof):
    while subprocess.run(["pgrep", "-f", prof], capture_output=True).returncode == 0:
        time.sleep(1)

for mode in (8, 3, 11):
    prof = os.path.join(scratch, "profile_%d" % mode)
    env = dict(os.environ, PFF_KINDS=KINDS, PFF_OUT=os.path.join(here, "foreign-v%d.odt" % mode), PFF_PNG="file://" + os.path.join(here, "tile.png"))
    ui = "-env:UserInstallation=file://" + prof
    os.makedirs(os.path.join(prof, "user"))
    open(os.path.join(prof, "user", "registrymodifications.xcu"), "w").write(XCU % mode)
    subprocess.run([soffice, *OPTS, ui, "macro:///Standard.Module1.Main"], env=env, timeout=300)  # creates the profile and its Standard library
    wait_gone(prof)
    open(os.path.join(prof, "user", "basic", "Standard", "Module1.xba"), "w").write(MODULE % escape(open(os.path.join(here, "gen.bas")).read()))
    subprocess.run([soffice, *OPTS, ui, "macro:///Standard.Module1.Gen"], env=env, timeout=300)
    wait_gone(prof)
    assert os.path.getsize(env["PFF_OUT"]) > 0, "no output for mode %d" % mode

def scrub(name):
    """replaces the generator string in meta.xml and the colour theme name in styles.xml by neutral ones"""
    path = os.path.join(here, name)
    zi = zipfile.ZipFile(path); items = [(it, zi.read(it.filename)) for it in zi.infolist()]; zi.close()
    zo = zipfile.ZipFile(path, "w")
    for it, data in items:
        if it.filename == "meta.xml":
            data = re.sub(rb"<meta:generator>[^<]*</meta:generator>", b"<meta:generator>other suite</meta:generator>", data)
        if it.filename == "styles.xml":
            data = re.sub(rb'(<loext:theme-colors loext:name=")[^"]*"', rb'\1Other"', data)
        zo.writestr(it, data, compress_type=zipfile.ZIP_STORED if it.filename == "mimetype" else zipfile.ZIP_DEFLATED)
    zo.close()

for mode in (8, 3, 11):
    scrub("foreign-v%d.odt" % mode)

def rewrite(src, dst, fn):
    zi = zipfile.ZipFile(os.path.join(here, src)); zo = zipfile.ZipFile(os.path.join(here, dst), "w")
    for it in zi.infolist():
        data = zi.read(it.filename)
        if it.filename in ("content.xml", "styles.xml"):
            data = fn(it.filename, data.decode("utf-8")).encode("utf-8")
        zo.writestr(it, data, compress_type=zipfile.ZIP_STORED if it.filename == "mimetype" else zipfile.ZIP_DEFLATED)
    zo.close()

def sub1(t, old, new):
    assert t.count(old) == 1, (old, t.count(old))
    return t.replace(old, new)

def style_xml(t, name):
    m = re.search(r'<style:style style:name="%s".*?</style:style>' % re.escape(name), t, re.S)
    assert m, name
    return m

# named gradient reference that does not exist
rewrite("foreign-v8.odt", "edge-unknown-name.odt", lambda n, t: t.replace('draw:fill-gradient-name="Gradient_20_1"', 'draw:fill-gradient-name="No_20_Such"') if n == "styles.xml" else t)
# empty element in a paragraph style that has fo:background-color
def empty(n, t):
    if n != "styles.xml": return t
    m = style_xml(t, "S_5f_gradient")
    return t.replace(m.group(0), re.sub(r'<loext:graphic-properties[^>]*/>', '<loext:graphic-properties/>', m.group(0)))
rewrite("foreign-v8.odt", "edge-empty.odt", empty)
# same element in non-paragraph styles (text style, table cell style) plus a paragraph that uses them
EXTRA_TEXT = '<style:style style:name="T9" style:family="text"><loext:graphic-properties draw:fill="solid" draw:fill-color="#ff0000"/></style:style>'
EXTRA_CELL = '<style:style style:name="C9" style:family="table-cell"><loext:graphic-properties draw:fill="solid" draw:fill-color="#ff0000"/></style:style>'
EXTRA_FRAME = '<style:style style:name="G9" style:family="graphic"><loext:graphic-properties draw:fill="solid" draw:fill-color="#ff0000"/></style:style>'
def nonpara(n, t):
    if n == "styles.xml":
        return t.replace("</office:styles>", EXTRA_FRAME + "</office:styles>", 1)
    return t.replace("</office:automatic-styles>", EXTRA_TEXT + EXTRA_CELL + "</office:automatic-styles>", 1)
rewrite("foreign-v8.odt", "edge-nonparagraph.odt", nonpara)
# duplicate element (second one must lose) and nested element
def dup(n, t):
    if n != "styles.xml": return t
    m = style_xml(t, "S_5f_solid")
    s = m.group(0).replace('<style:paragraph-properties', '<loext:graphic-properties draw:fill="solid" draw:fill-color="#ff0000"/><style:paragraph-properties', 1)
    m2 = style_xml(t, "S_5f_hatch")
    h = m2.group(0).replace('draw:fill-hatch-solid="true"/>', 'draw:fill-hatch-solid="true"><loext:graphic-properties draw:fill="solid" draw:fill-color="#ff0000"/></loext:graphic-properties>', 1)
    return t.replace(m.group(0), s).replace(m2.group(0), h)
rewrite("foreign-v8.odt", "edge-duplicate-nested.odt", dup)
# the native form and the extension element in one style, both orders
def both(n, t):
    if n != "styles.xml": return t
    m = style_xml(t, "S_5f_solid")
    native = '<style:graphic-properties draw:fill="solid" draw:fill-color="#ff0000"/>'
    s1 = m.group(0).replace('<loext:graphic-properties', native + '<loext:graphic-properties', 1)
    m2 = style_xml(t, "S_5f_solid_5f_fill") if 'S_5f_solid_5f_fill' in t else style_xml(t, "S_5f_solid_5f_transp")
    s2 = m2.group(0).replace('<style:paragraph-properties', native + '<style:paragraph-properties', 1)
    return t.replace(m.group(0), s1).replace(m2.group(0), s2)
rewrite("foreign-v8.odt", "edge-both-forms.odt", both)
# an attribute of the extension element that no fill property maps to
def unknown_attr(n, t):
    if n != "styles.xml": return t
    m = style_xml(t, "S_5f_solid")
    return t.replace(m.group(0), m.group(0).replace('<loext:graphic-properties ', '<loext:graphic-properties loext:future-attribute="x" ', 1))
rewrite("foreign-v8.odt", "edge-unknown-attr.odt", unknown_attr)
print("done")
