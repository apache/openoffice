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

# usage: runpy.sh fixtures/t14/forms.py <port> <outdir> <tag> [file.odt]
# With a file nothing is created: the file is loaded and its InputRequired values are written.
# A Writer document with one form holding two controls of each type that can carry form:input-required: one with InputRequired
# set to false and one left at its default (true in AOO). Saved with the office's configured ODF version and reloaded.
# Writes <outdir>/forms-<tag>.json with the InputRequired value of every control after the reload (a control written with
# the wrong value, or without the attribute where the reader's default differs, changes value).
import json, os, sys
import uno
from unohelp import connect, desktop, pv

port, out, tag = int(sys.argv[1]), os.path.abspath(sys.argv[2]), sys.argv[3]
os.makedirs(out, exist_ok=True)
ctx = connect(port)
desk = desktop(ctx)
KINDS = ["TextField", "CheckBox", "ComboBox", "ListBox", "RadioButton", "DateField", "TimeField", "FormattedField", "NumericField",
         "CurrencyField", "PatternField", "DatabaseImageControl", "FileControl"]
if len(sys.argv) > 4:
    path = os.path.abspath(sys.argv[4])
else:
    doc = desk.loadComponentFromURL("private:factory/swriter", "_blank", 0, (pv("Hidden", True),))
    forms = doc.DrawPage.Forms
    form = ctx.ServiceManager.createInstanceWithContext("com.sun.star.form.component.Form", ctx)
    forms.insertByName("f", form)
    for i, k in enumerate(KINDS):
        for suffix in ("", "_default"):
            m = ctx.ServiceManager.createInstanceWithContext("com.sun.star.form.component." + k, ctx)
            if m is None:
                print("service not available:", k)
                break
            m.Name = k + suffix
            if not suffix:
                try:
                    m.InputRequired = False
                except Exception as e:
                    print("no InputRequired on", k, e)
            form.insertByIndex(form.Count, m)
    path = os.path.join(out, tag + ".odt")
    if os.path.exists(path):
        os.remove(path)
    doc.storeToURL(uno.systemPathToFileUrl(path), (pv("FilterName", "writer8"), pv("Overwrite", True)))
    doc.close(True)
d2 = desk.loadComponentFromURL(uno.systemPathToFileUrl(path), "_blank", 0, (pv("Hidden", True),))
f2 = d2.DrawPage.Forms.getByName("f")
res = {}
for i in range(f2.Count):
    c = f2.getByIndex(i)
    try:
        res[c.Name] = c.InputRequired
    except Exception as e:
        res[c.Name] = "n/a"
d2.close(True)
json.dump(res, open(os.path.join(out, "forms-" + tag + ".json"), "w"), indent=1, sort_keys=True)
print(tag, res)
