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

# usage: run-in-mode.sh <mode> <port> <outdir> fixtures/t14/para_fill.py [<file.odt>]
# no file: writes <outdir>/para-fill-<mode>.odt with a solid fill in a paragraph style, a solid fill set directly on a paragraph
#         and a gradient fill set directly on a paragraph, in the office's current save mode.
# file:    loads that file and prints the fill properties of the style and of the three paragraphs.
import os, sys
import uno
from unohelp import FILTERS, connect, desktop, pv

port, out, tag = int(sys.argv[1]), os.path.abspath(sys.argv[2]), sys.argv[3]
os.makedirs(out, exist_ok=True)
desk = desktop(connect(port))
dst = os.path.abspath(sys.argv[4]) if len(sys.argv) > 4 else os.path.join(out, "para-fill-%s.odt" % tag)
phase = "reload" if len(sys.argv) > 4 else "make"
url = uno.systemPathToFileUrl


def fillenum(name):
    return uno.Enum("com.sun.star.drawing.FillStyle", name)


def show(label, obj):
    print(label, obj.FillStyle.value, "0x%06x" % obj.FillColor, obj.FillTransparence)


if phase == "make":
    doc = desk.loadComponentFromURL("private:factory/swriter", "_blank", 0, (pv("Hidden", True),))
    styles = doc.StyleFamilies.getByName("ParagraphStyles")
    st = doc.createInstance("com.sun.star.style.ParagraphStyle")
    styles.insertByName("FillSolid", st)
    st.FillStyle = fillenum("SOLID")
    st.FillColor = 0x3366cc
    text = doc.Text
    paras = []
    for name in ("styled", "direct solid", "direct gradient"):
        text.insertString(text.End, name, False)
        text.insertControlCharacter(text.End, uno.getConstantByName("com.sun.star.text.ControlCharacter.PARAGRAPH_BREAK"), False)
    enum = text.createEnumeration()
    while enum.hasMoreElements():
        paras.append(enum.nextElement())
    paras[0].ParaStyleName = "FillSolid"
    paras[1].FillStyle = fillenum("SOLID")
    paras[1].FillColor = 0xcc6633
    paras[1].FillTransparence = 30
    g = uno.createUnoStruct("com.sun.star.awt.Gradient")
    g.Style = uno.Enum("com.sun.star.awt.GradientStyle", "LINEAR")
    g.StartColor, g.EndColor, g.Angle = 0xff0000, 0x0000ff, 450
    g.Border, g.XOffset, g.YOffset, g.StartIntensity, g.EndIntensity, g.StepCount = 0, 50, 50, 100, 100, 0
    paras[2].FillStyle = fillenum("GRADIENT")
    paras[2].FillGradient = g
    show("style", st)
    for i, p in enumerate(paras[:3]):
        show("para%d" % i, p)
    if os.path.exists(dst):
        os.remove(dst)
    doc.storeToURL(url(dst), (pv("FilterName", FILTERS["odt"]), pv("Overwrite", True)))
    doc.close(True)
    print("stored", dst)
else:
    doc = desk.loadComponentFromURL(url(dst), "_blank", 0, (pv("Hidden", True),))
    show("style", doc.StyleFamilies.getByName("ParagraphStyles").getByName("FillSolid"))
    enum = doc.Text.createEnumeration()
    i = 0
    while enum.hasMoreElements():
        p = enum.nextElement()
        if i < 3:
            show("para%d" % i, p)
            if p.FillStyle.value == "GRADIENT":
                gg = p.FillGradient
                print("  gradient", gg.Style.value, hex(gg.StartColor), hex(gg.EndColor), gg.Angle)
        i += 1
    doc.close(True)
