# usage: python3 -I fixtures/xforms-list/make.py <xforms-ubl.odt> <out.odt>
# Derives a document with a combobox and a listbox that both have form:xforms-list-source.
import re, sys, zipfile

src, out = sys.argv[1], sys.argv[2]
zin = zipfile.ZipFile(src)
zout = zipfile.ZipFile(out, "w", zipfile.ZIP_DEFLATED)
for info in zin.infolist():
    data = zin.read(info.filename)
    if info.filename == "content.xml":
        s = data.decode("utf-8")
        m = re.search(r'<form:combobox form:name="ComboBox1"[^>]*>.*?</form:combobox>', s, re.S)
        assert m, "ComboBox1 not found"
        lb = '<form:listbox form:name="ListBox1" form:control-implementation="ooo:com.sun.star.form.component.ListBox" form:id="control50" form:dropdown="true" form:tab-index="29" xforms:bind="Bindung 45" form:xforms-list-source="currencies"/>'
        s = s[:m.start()] + lb + s[m.end():]
        data = s.encode("utf-8")
    zout.writestr(info, data, zipfile.ZIP_STORED if info.filename == "mimetype" else zipfile.ZIP_DEFLATED)
zout.close()
