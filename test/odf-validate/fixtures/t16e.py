#!/usr/bin/env python3
# usage: runpy.sh fixtures/t16e.py <port> <outdir>
# Task 16a: runtime checks of the XML processing requirements of ODF 1.2 Part 1 section 2.4 E on hand-built Writer packages.
# Prints "RESULT <name> <observation>" lines; the expectation of a conforming consumer is in the name or the comment.
import sys, os, re, zipfile, uno
from unohelp import connect, desktop, pv
port, out = int(sys.argv[1]), os.path.abspath(sys.argv[2])
os.makedirs(out, exist_ok=True)
desk = desktop(connect(port))
OFFICE = "urn:oasis:names:tc:opendocument:xmlns:office:1.0"
TEXT = "urn:oasis:names:tc:opendocument:xmlns:text:1.0"
STYLE = "urn:oasis:names:tc:opendocument:xmlns:style:1.0"
XL = "http://www.w3.org/1999/xlink"
MAN = ('<?xml version="1.0" encoding="UTF-8"?><manifest:manifest xmlns:manifest="urn:oasis:names:tc:opendocument:xmlns:manifest:1.0" manifest:version="1.2">'
       '<manifest:file-entry manifest:full-path="/" manifest:media-type="application/vnd.oasis.opendocument.text"/>'
       '<manifest:file-entry manifest:full-path="content.xml" manifest:media-type="text/xml"/>'
       '<manifest:file-entry manifest:full-path="styles.xml" manifest:media-type="text/xml"/></manifest:manifest>')
STYLES = ('<?xml version="1.0" encoding="UTF-8"?><office:document-styles xmlns:office="%s" xmlns:style="%s" xmlns:text="%s" office:version="1.2">'
          '<office:styles><style:style style:name="P1" style:family="paragraph"><style:text-properties style:text-position="33%% 100%%"/></style:style></office:styles>'
          '</office:document-styles>' % (OFFICE, STYLE, TEXT))
def pkg(name, content):
    p = os.path.join(out, name)
    z = zipfile.ZipFile(p, "w")
    z.writestr("mimetype", "application/vnd.oasis.opendocument.text", compress_type=zipfile.ZIP_STORED)
    z.writestr("META-INF/manifest.xml", MAN); z.writestr("styles.xml", STYLES); z.writestr("content.xml", content)
    z.close(); return p
def doc(body, root_ns=None, pre="", extra_attrs=""):
    ns = root_ns if root_ns is not None else 'xmlns:office="%s" xmlns:text="%s" xmlns:style="%s" xmlns:xlink="%s"' % (OFFICE, TEXT, STYLE, XL)
    return '<?xml version="1.0" encoding="UTF-8"?>%s<office:document-content %s office:version="1.2"><office:body><office:text>%s</office:text></office:body></office:document-content>' % (pre, ns, body)
def observe(path):
    try:
        d = desk.loadComponentFromURL(uno.systemPathToFileUrl(path), "_blank", 0, (pv("Hidden", True),))
    except Exception as e:
        return {"loaded": False, "error": repr(e)[:120]}
    if d is None:
        return {"loaded": False, "error": "None"}
    r = {"loaded": True}
    try:
        r["text"] = d.Text.String.replace("\n", "|")
        en = d.Text.createEnumeration(); refs = []; styles = []
        while en.hasMoreElements():
            e = en.nextElement()
            if e.supportsService("com.sun.star.text.Paragraph"):
                refs.append(e.MetadataReference.Second); styles.append(e.ParaStyleName)
        r["xmlid"] = refs; r["parastyle"] = styles
        r["bookmarks"] = d.Bookmarks.Count
    except Exception as e:
        r["obs_error"] = repr(e)[:100]
    d.close(True); return r
def run(name, content, note=""):
    r = observe(pkg(name + ".odt", content)); print("RESULT %-34s %s %s" % (name, r, ("# " + note) if note else ""), flush=True)
# ---- baseline and namespace handling
run("00-baseline", doc('<text:p xml:id="p1">one</text:p><text:p>two</text:p><text:bookmark text:name="b1" xml:id="bk1"/>'))
# prefixes renamed throughout (expected: identical to the baseline)
b = doc('<text:p xml:id="p1">one</text:p><text:p>two</text:p><text:bookmark text:name="b1" xml:id="bk1"/>')
b = re.sub(r'(</?|\s)text:', r'\1t:', b); b = b.replace('xmlns:text=', 'xmlns:t=')
b = re.sub(r'(</?|\s)office:', r'\1o:', b); b = b.replace('xmlns:office=', 'xmlns:o=')
run("01-prefixes-renamed", b, "conforming: same as baseline")
# default namespace for the text elements
run("02-default-namespace", doc('<p xml:id="p1">one</p><p>two</p>', root_ns='xmlns:office="%s" xmlns="%s" xmlns:style="%s"' % (OFFICE, TEXT, STYLE)), "conforming: same text")
# unprefixed attribute under a default namespace: no namespace per Namespaces in XML, so style-name must be ignored
run("03-unprefixed-attr", doc('<p style-name="P1">one</p>', root_ns='xmlns:office="%s" xmlns="%s" xmlns:style="%s"' % (OFFICE, TEXT, STYLE)), "conforming: parastyle stays Standard")
# prefix rebound to another namespace inside the element: it is then not a text:p
run("04-prefix-rebound", doc('<text:p xmlns:text="urn:example:other">hidden</text:p><text:p>shown</text:p>'), "conforming: only 'shown'")
# undeclared prefix: not namespace-well-formed
run("05-undeclared-prefix", doc('<text:p>one</text:p>', root_ns='xmlns:office="%s"' % OFFICE), "conforming: load fails")
# unknown element and attribute in the ODF namespace
run("06-unknown-content", doc('<text:foo>x</text:foo><text:p text:bogus="1">kept</text:p>'), "tolerated: 'kept'")
# ---- xml:id
run("10-xmlid-whitespace", doc('<text:p xml:id="  w1  ">one</text:p>'), "xml:id spec: normalised to w1")
run("11-xmlid-duplicate", doc('<text:p xml:id="d1">one</text:p><text:p xml:id="d1">two</text:p>'), "xml:id spec: duplicate is an error")
run("12-xmlid-not-ncname", doc('<text:p xml:id="1bad">one</text:p>'), "xml:id spec: invalid value")
# ---- DTD and entities
run("20-internal-entity", doc('<text:p>&e;</text:p>', pre='<!DOCTYPE office:document-content [<!ENTITY e "ENTITYTEXT">]>'), "expanded")
host = open("/etc/hostname").read().strip() if os.path.exists("/etc/hostname") else "zzz"
open(os.path.join(out, "secret.txt"), "w").write("SECRETFILE")
run("21-external-entity", doc('<text:p>a&x;b</text:p>', pre='<!DOCTYPE office:document-content [<!ENTITY x SYSTEM "file://%s/secret.txt">]>' % out), "must NOT contain SECRETFILE")
run("22-external-dtd", doc('<text:p>one</text:p>', pre='<!DOCTYPE office:document-content SYSTEM "file://%s/secret.txt">' % out), "must load, nothing fetched")
laugh = '<!DOCTYPE office:document-content [<!ENTITY a "lol"><!ENTITY b "&a;&a;&a;&a;&a;&a;&a;&a;&a;&a;"><!ENTITY c "&b;&b;&b;&b;&b;&b;&b;&b;&b;&b;"><!ENTITY d "&c;&c;&c;&c;&c;&c;&c;&c;&c;&c;"><!ENTITY e "&d;&d;&d;&d;&d;&d;&d;&d;&d;&d;"><!ENTITY f "&e;&e;&e;&e;&e;&e;&e;&e;&e;&e;"><!ENTITY g "&f;&f;&f;&f;&f;&f;&f;&f;&f;&f;"><!ENTITY h "&g;&g;&g;&g;&g;&g;&g;&g;&g;&g;"><!ENTITY i "&h;&h;&h;&h;&h;&h;&h;&h;&h;&h;">]>'
run("23-entity-expansion-limit", doc('<text:p>&i;</text:p>', pre=laugh), "expansion 10^8: must not hang or exhaust memory")
print("DONE")
