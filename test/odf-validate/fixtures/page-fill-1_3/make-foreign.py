# usage: python3 -I make-foreign.py <other-suite-soffice> <scratch-dir> [edges-only]
# Writes foreign-v<N>.odt with the other suite saving at ODF version setting N
# (/org.openoffice.Office.Common/Save/ODF DefaultVersion; 10/11 write 1.3, 12/3 write 1.4, 8 writes 1.2 extended),
# using gen.bas as macro Standard.Module1.Gen in a throw-away profile, then derives the edge-*.odt variants
# by editing the XML text of foreign-v10.odt and foreign-v11.odt (edges-only: only the variants).
import os, re, subprocess, sys, time, zipfile
from xml.sax.saxutils import escape

soffice, scratch = sys.argv[1], sys.argv[2]
here = os.path.dirname(os.path.abspath(__file__))
KINDS = "solid,solid_transp,gradient,gradient_transp,gradient_tgrad,hatch,hatch_nobg,bitmap_tile,bitmap_stretch,bitmap_once,none,shared"
MODES = (10, 11, 12, 3, 8)
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

def generate():
    for mode in MODES:
        prof = os.path.join(scratch, "profile_page_%d" % mode)
        out = os.path.join(here, "foreign-v%d.odt" % mode)
        env = dict(os.environ, PFF_KINDS=KINDS, PFF_OUT=out, PFF_PNG="file://" + os.path.join(here, "tile.png"))
        ui = "-env:UserInstallation=file://" + prof
        os.makedirs(os.path.join(prof, "user"))
        open(os.path.join(prof, "user", "registrymodifications.xcu"), "w").write(XCU % mode)
        subprocess.run([soffice, *OPTS, ui, "macro:///Standard.Module1.Main"], env=env, timeout=300)  # creates the profile and its Standard library
        wait_gone(prof)
        open(os.path.join(prof, "user", "basic", "Standard", "Module1.xba"), "w").write(MODULE % escape(open(os.path.join(here, "gen.bas")).read()))
        subprocess.run([soffice, *OPTS, ui, "macro:///Standard.Module1.Gen"], env=env, timeout=300)
        wait_gone(prof)
        assert os.path.getsize(out) > 0, "no output for mode %d" % mode
        scrub(out)

def scrub(path):
    """replaces the generator string in meta.xml and the colour theme name in styles.xml by neutral ones"""
    zi = zipfile.ZipFile(path); items = [(it, zi.read(it.filename)) for it in zi.infolist()]; zi.close()
    zo = zipfile.ZipFile(path, "w")
    for it, data in items:
        if it.filename == "meta.xml":
            data = re.sub(rb"<meta:generator>[^<]*</meta:generator>", b"<meta:generator>other suite</meta:generator>", data)
        if it.filename == "styles.xml":
            data = re.sub(rb'(<loext:theme-colors loext:name=")[^"]*"', rb'\1Other"', data)
        zo.writestr(it, data, compress_type=zipfile.ZIP_STORED if it.filename == "mimetype" else zipfile.ZIP_DEFLATED)
    zo.close()

def rewrite(src, dst, fn):
    zi = zipfile.ZipFile(os.path.join(here, src)); zo = zipfile.ZipFile(os.path.join(here, dst), "w")
    for it in zi.infolist():
        data = zi.read(it.filename)
        if it.filename == "styles.xml":
            data = fn(data.decode("utf-8")).encode("utf-8")
        zo.writestr(it, data, compress_type=zipfile.ZIP_STORED if it.filename == "mimetype" else zipfile.ZIP_DEFLATED)
    zo.close()

def sub1(t, old, new):
    assert t.count(old) == 1, (old, t.count(old))
    return t.replace(old, new)

def dp(t, name):
    m = re.search(r'<style:style style:name="%s" style:family="drawing-page">.*?</style:style>' % name, t)
    assert m, name
    return m.group(0)

def edges():
    # unresolved names: gradient (P_gradient, P_shared, P_shared2), hatch (P_hatch), transparency gradient only (P_gradient_tgrad)
    def unknown(t):
        t = sub1(t, dp(t, "Mdp3"), dp(t, "Mdp3").replace('"Gradient_20_1"', '"No_20_Such"'))
        t = sub1(t, dp(t, "Mdp6"), dp(t, "Mdp6").replace('"Red_20_90_20_Degrees_20_Crossed_20_1"', '"No_20_Such"'))
        return sub1(t, dp(t, "Mdp5"), dp(t, "Mdp5").replace('draw:opacity-name="TG1"', 'draw:opacity-name="No_20_Such"'))
    rewrite("foreign-v10.odt", "edge-unknown-name.odt", unknown)
    # empty properties element (P_gradient_transp), style without properties element (P_solid_transp)
    def empty(t):
        t = sub1(t, dp(t, "Mdp4"), '<style:style style:name="Mdp4" style:family="drawing-page"><style:drawing-page-properties/></style:style>')
        return sub1(t, dp(t, "Mdp2"), '<style:style style:name="Mdp2" style:family="drawing-page"/>')
    rewrite("foreign-v10.odt", "edge-empty.odt", empty)
    # master page naming a drawing page style that is not defined (P_hatch)
    rewrite("foreign-v10.odt", "edge-undefined.odt", lambda t: sub1(t, 'style:page-layout-name="Mpm7" draw:style-name="Mdp6"', 'style:page-layout-name="Mpm7" draw:style-name="No_20_Such"'))
    # draw:fill="none" next to the page layout's fo:background-color (Standard, P_solid)
    rewrite("foreign-v10.odt", "edge-none.odt", lambda t: sub1(t, dp(t, "Mdp1"), dp(t, "Mdp1").replace('draw:fill="solid"', 'draw:fill="none"')))
    # drawing page style among the common styles, before the hatch it names (P_hatch)
    def common(t):
        d = dp(t, "Mdp6")
        return sub1(sub1(t, d, ""), "<office:styles>", "<office:styles>" + d)
    rewrite("foreign-v10.odt", "edge-common.odt", common)
    # both forms disagree: page layouts keep the gradient, the drawing page says solid red (P_gradient, P_shared, P_shared2);
    # unusable drawing page next to a usable page layout fill (P_hatch_nobg, First Page)
    def both(t):
        t = sub1(t, dp(t, "Mdp3"), '<style:style style:name="Mdp3" style:family="drawing-page"><style:drawing-page-properties draw:fill="solid" draw:fill-color="#ff0000"/></style:style>')
        return sub1(t, dp(t, "Mdp7"), dp(t, "Mdp7").replace('"Red_20_90_20_Degrees_20_Crossed_20_1"', '"No_20_Such"'))
    rewrite("foreign-v11.odt", "edge-both.odt", both)
    # page layout shared by a master page with a drawing page fill and one without (P_shared2; P_bitmap_stretch moved to the
    # tile layout): the second keeps the page layout's own fallback
    def shared(t):
        t = sub1(t, 'style:display-name="P_shared2" style:page-layout-name="Mpm13" draw:style-name="Mdp3"', 'style:display-name="P_shared2" style:page-layout-name="Mpm13"')
        return sub1(t, 'style:display-name="P_bitmap_stretch" style:page-layout-name="Mpm10" draw:style-name="Mdp9"', 'style:display-name="P_bitmap_stretch" style:page-layout-name="Mpm9"')
    rewrite("foreign-v10.odt", "edge-shared-layout.odt", shared)

if __name__ == "__main__":
    if sys.argv[3:4] != ["edges-only"]:
        generate()
    edges()
