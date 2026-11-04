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

# usage: run-in-mode.sh <mode> <port> <outdir> fixtures/t14/hf_fill.py [<file.odt>]
# no file: writes <outdir>/hf-fill-<mode>.odt with page styles whose header and footer carry a solid fill with
#          transparency, a gradient, a hatch and a tiled bitmap, in the office's current save mode.
# file:    loads that file and prints the header and footer fill properties of those page styles.
import os, sys
import uno
from unohelp import FILTERS, connect, desktop, pv

port, out, tag = int(sys.argv[1]), os.path.abspath(sys.argv[2]), sys.argv[3]
os.makedirs(out, exist_ok=True)
desk = desktop(connect(port))
dst = os.path.abspath(sys.argv[4]) if len(sys.argv) > 4 else os.path.join(out, "hf-fill-%s.odt" % tag)
phase = "reload" if len(sys.argv) > 4 else "make"
url = uno.systemPathToFileUrl
png = url(os.path.join(os.path.dirname(os.path.abspath(sys.argv[0])), "..", "page-fill-1_3", "tile.png"))


def fe(name):
    return uno.Enum("com.sun.star.drawing.FillStyle", name)


def grad(c1, c2):
    g = uno.createUnoStruct("com.sun.star.awt.Gradient")
    g.Style = uno.Enum("com.sun.star.awt.GradientStyle", "LINEAR")
    g.StartColor, g.EndColor, g.Angle = c1, c2, 300
    g.Border, g.XOffset, g.YOffset, g.StartIntensity, g.EndIntensity, g.StepCount = 5, 50, 50, 100, 100, 0
    return g


def hatch():
    h = uno.createUnoStruct("com.sun.star.drawing.Hatch")
    h.Style = uno.Enum("com.sun.star.drawing.HatchStyle", "SINGLE")
    h.Color, h.Distance, h.Angle = 0x00aa00, 200, 450
    return h


PAGES = ("HF_solid", "HF_gradient", "HF_hatch", "HF_bitmap")


def setup(st, kind, pre):
    st.setPropertyValue(pre + "IsOn", True)
    sp = lambda n, v: st.setPropertyValue(pre + n, v)
    if kind == "HF_solid":
        sp("FillStyle", fe("SOLID")); sp("FillColor", 0x3355aa); sp("FillTransparence", 40)
    elif kind == "HF_gradient":
        sp("FillStyle", fe("GRADIENT")); sp("FillGradient", grad(0xff0000, 0x0000ff))
    elif kind == "HF_hatch":
        sp("FillStyle", fe("HATCH")); sp("FillHatch", hatch()); sp("FillColor", 0xffffcc); sp("FillBackground", True)
    else:
        sp("FillStyle", fe("BITMAP")); sp("FillBitmapURL", png)
        sp("FillBitmapMode", uno.Enum("com.sun.star.drawing.BitmapMode", "REPEAT"))


def show(label, st, pre):
    sp = lambda n: st.getPropertyValue(pre + n)
    line = [label, sp("FillStyle").value]
    kind = line[1]
    if kind == "SOLID":
        line += ["0x%06x" % sp("FillColor"), sp("FillTransparence")]
    elif kind == "GRADIENT":
        g = sp("FillGradient")
        line += [g.Style.value, hex(g.StartColor), hex(g.EndColor), g.Angle, g.Border]
    elif kind == "HATCH":
        h = sp("FillHatch")
        line += [h.Style.value, hex(h.Color), h.Distance, h.Angle, "0x%06x" % sp("FillColor"), sp("FillBackground")]
    elif kind == "BITMAP":
        line += [sp("FillBitmapMode").value, bool(sp("FillBitmapURL"))]
    print(*line)


if phase == "make":
    doc = desk.loadComponentFromURL("private:factory/swriter", "_blank", 0, (pv("Hidden", True),))
    ps = doc.StyleFamilies.getByName("PageStyles")
    text = doc.Text
    pb = uno.getConstantByName("com.sun.star.text.ControlCharacter.PARAGRAPH_BREAK")
    for i, k in enumerate(PAGES):
        ps.insertByName(k, doc.createInstance("com.sun.star.style.PageStyle"))
        st = ps.getByName(k)
        setup(st, k, "Header")
        setup(st, PAGES[(i + 1) % len(PAGES)], "Footer")
        text.insertString(text.End, "page " + k, False)
        if i < len(PAGES) - 1:
            text.insertControlCharacter(text.End, pb, False)
    enum = text.createEnumeration()
    for k in PAGES:
        enum.nextElement().PageDescName = k
    for k in PAGES:
        show(k + " header", ps.getByName(k), "Header")
        show(k + " footer", ps.getByName(k), "Footer")
    if os.path.exists(dst):
        os.remove(dst)
    doc.storeToURL(url(dst), (pv("FilterName", FILTERS["odt"]), pv("Overwrite", True)))
    doc.close(True)
    print("stored", dst)
else:
    doc = desk.loadComponentFromURL(url(dst), "_blank", 0, (pv("Hidden", True),))
    ps = doc.StyleFamilies.getByName("PageStyles")
    for k in PAGES:
        show(k + " header", ps.getByName(k), "Header")
        show(k + " footer", ps.getByName(k), "Footer")
    doc.close(True)
