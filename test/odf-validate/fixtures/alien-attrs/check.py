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

# usage: check.py <outdir> <strict|ext>
# outdir holds the alien-writer.odt, alien-calc.ods and alien-draw.odg that make.py saved in that mode. Preserved
# attributes in ODF namespaces are dropped in both modes; the foreign ex:alien-foreign survives only in ext.
import sys, os, re, zipfile

outdir, mode = sys.argv[1], sys.argv[2]
foreign = {"strict": {"alien-writer.odt": 0, "alien-calc.ods": 0, "alien-draw.odg": 0},
           "ext": {"alien-writer.odt": 5, "alien-calc.ods": 1, "alien-draw.odg": 1}}[mode]
fail = 0
for name, want in foreign.items():
    z = zipfile.ZipFile(os.path.join(outdir, name))
    text = "".join(z.read(s).decode("utf8") for s in ("content.xml", "styles.xml"))
    got = len(re.findall(r'\bex:alien-foreign="v-ex"', text))
    odf = len(re.findall(r'\b(?:text|style|draw|table|office|svg|fo):(?:alien-\w+|enable-numbering)="v-', text))
    ok = got == want and odf == 0
    fail += not ok
    print("%s %s: foreign %d (want %d), ODF-namespace %d (want 0)" % ("PASS" if ok else "FAIL", name, got, want, odf))
sys.exit(1 if fail else 0)
