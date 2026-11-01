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
#
# usage: runpy.sh fixtures/styles-gfx/make.py <port> <outdir> [<document name>...]
# Documents with every enumerated value of the graphic, drawing page, page layout, header/footer and table style
# properties. Saves with the office's configured ODF version: run it against the strict and extended profiles.
import sys, os, shutil, struct, tempfile, zlib, gc, uno
from unohelp import connect, desktop, pv

port, out = int(sys.argv[1]), os.path.abspath(sys.argv[2])
only = set(sys.argv[3:])
os.makedirs(out, exist_ok=True)
ctx = connect(port)
desk = desktop(ctx)
failed = []
NOPROP = set()
work = tempfile.mkdtemp(prefix="styles-gfx-")
SZ, PT = "com.sun.star.awt.Size", "com.sun.star.awt.Point"


def E(typ, val):
    return uno.Enum(typ, val)


def st(name):
    return uno.createUnoStruct(name)


def png(path, w=16, h=16):
    raw = b"".join(b"\x00" + bytes(v for x in range(w) for v in (x * 255 // w, y * 255 // h, 128)) for y in range(h))

    def chunk(t, d):
        c = struct.pack(">I", len(d)) + t + d
        return c + struct.pack(">I", zlib.crc32(t + d) & 0xffffffff)
    with open(path, "wb") as f:
        f.write(b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", w, h, 8, 2, 0, 0, 0)) +
                chunk(b"IDAT", zlib.compress(raw)) + chunk(b"IEND", b""))


IMG = os.path.join(work, "img.png")
png(IMG)
IMGURL = uno.systemPathToFileUrl(IMG)


def new(factory):
    return desk.loadComponentFromURL(factory, "_blank", 0, (pv("Hidden", True),))


def save(doc, name, filt):
    try:
        doc.storeToURL(uno.systemPathToFileUrl(os.path.join(out, name)), (pv("FilterName", filt),))
        print("saved", name, filt)
    except Exception as e:
        failed.append(name)
        print("FAIL save", name, filt, type(e).__name__, str(e)[:200])


def setp(o, tag, quiet=False, **kw):
    for k, v in kw.items():
        try:
            o.setPropertyValue(k, v)
        except Exception as e:
            if type(e).__name__.endswith("UnknownPropertyException"):
                if not quiet and k not in NOPROP:
                    NOPROP.add(k)
                    print("NOPROP", tag, k)
            elif not quiet:
                failed.append(tag + "." + k)
                print("FAIL set", tag, k, repr(v)[:60], type(e).__name__, str(e)[:100])


def guard(tag, fn, *a, **kw):
    try:
        return fn(*a, **kw)
    except Exception as e:
        failed.append(tag)
        print("FAIL", tag, type(e).__name__, str(e)[:200])


def wanted(name):
    return not only or name in only


class Grid:
    def __init__(self, cols=6, w=3000, h=2000, gap=500):
        self.i, self.cols, self.w, self.h, self.gap = 0, cols, w, h, gap

    def next(self):
        c, r = self.i % self.cols, self.i // self.cols
        self.i += 1
        return (500 + c * (self.w + self.gap), 500 + r * (self.h + self.gap))


def draw_tables(doc):
    t = doc.createInstance("com.sun.star.drawing.DashTable")
    d = st("com.sun.star.drawing.LineDash")
    d.Style = E("com.sun.star.drawing.DashStyle", "ROUND")
    d.Dots, d.DotLen, d.Dashes, d.DashLen, d.Distance = 2, 100, 1, 300, 150
    t.insertByName("dash-a", d)
    d.Style = E("com.sun.star.drawing.DashStyle", "RECT")
    t.insertByName("dash-b", d)
    t = doc.createInstance("com.sun.star.drawing.GradientTable")
    for n, s in (("grad-lin", "LINEAR"), ("grad-ax", "AXIAL"), ("grad-rad", "RADIAL"), ("grad-ell", "ELLIPTICAL"),
                 ("grad-sq", "SQUARE"), ("grad-rect", "RECT")):
        g = st("com.sun.star.awt.Gradient")
        g.Style = E("com.sun.star.awt.GradientStyle", s)
        g.StartColor, g.EndColor, g.Angle, g.Border = 0xff0000, 0x0000ff, 300, 10
        g.XOffset, g.YOffset, g.StartIntensity, g.EndIntensity, g.StepCount = 40, 60, 90, 80, 0
        t.insertByName(n, g)
    t = doc.createInstance("com.sun.star.drawing.TransparencyGradientTable")
    g = st("com.sun.star.awt.Gradient")
    g.Style = E("com.sun.star.awt.GradientStyle", "LINEAR")
    g.StartColor, g.EndColor, g.Angle, g.Border = 0x000000, 0xcccccc, 0, 0
    g.XOffset, g.YOffset, g.StartIntensity, g.EndIntensity = 50, 50, 100, 100
    t.insertByName("trans-a", g)
    t = doc.createInstance("com.sun.star.drawing.HatchTable")
    for n, s in (("hatch-s", "SINGLE"), ("hatch-d", "DOUBLE"), ("hatch-t", "TRIPLE")):
        h = st("com.sun.star.drawing.Hatch")
        h.Style = E("com.sun.star.drawing.HatchStyle", s)
        h.Color, h.Distance, h.Angle = 0x008000, 150, 450
        t.insertByName(n, h)
    t = doc.createInstance("com.sun.star.drawing.BitmapTable")
    t.insertByName("bitmap-a", IMGURL)


def mkstyle(doc, family, name, parent=None, service=None, **props):
    fam = doc.StyleFamilies.getByName(family)
    s = doc.createInstance(service) if service else fam.createInstance()
    fam.insertByName(name, s)
    s = fam.getByName(name)
    if parent:
        s.ParentStyle = parent
    setp(s, "style " + name, quiet=True, **props)
    return s


STYLE_N = [0]


class DrawBuilder:
    def __init__(self, doc, page, grid=None):
        self.doc, self.page, self.grid = doc, page, grid or Grid()

    def shape(self, kind, tag, props, text=None, style=True, size=None, ns="drawing"):
        STYLE_N[0] += 1
        s = self.doc.createInstance("com.sun.star.%s.%s" % (ns, kind))
        self.page.add(s)
        x, y = self.grid.next()
        s.Position = uno.createUnoStruct(PT, x, y)
        s.Size = uno.createUnoStruct(SZ, *(size or (self.grid.w, self.grid.h)))
        if text:
            s.String = text
        setp(s, tag, **props)
        if style:
            fam = "graphics"
            guard("style " + tag, mkstyle, self.doc, fam, "gs-%s-%d" % (tag, STYLE_N[0]), None, **props)
        return s


def fillprops():
    FS = "com.sun.star.drawing.FillStyle"
    cases = []
    cases.append(("fill-solid", dict(FillStyle=E(FS, "SOLID"), FillColor=0xffcc00, FillColor2=0x00ccff, FillTransparence=40)))
    cases.append(("fill-none", dict(FillStyle=E(FS, "NONE"))))
    for g in ("grad-lin", "grad-ax", "grad-rad", "grad-ell", "grad-sq", "grad-rect"):
        cases.append((g, dict(FillStyle=E(FS, "GRADIENT"), FillGradientName=g, FillGradientStepCount=7)))
    cases.append(("grad-trans", dict(FillStyle=E(FS, "GRADIENT"), FillGradientName="grad-lin",
                                      FillTransparenceGradientName="trans-a")))
    for h in ("hatch-s", "hatch-d", "hatch-t"):
        for bg in (True, False):
            cases.append((h + str(bg), dict(FillStyle=E(FS, "HATCH"), FillHatchName=h, FillBackground=bg, FillColor=0xffff99)))
    BM = "com.sun.star.drawing.BitmapMode"
    for m in ("REPEAT", "STRETCH", "NO_REPEAT"):
        cases.append(("bmp-" + m, dict(FillStyle=E(FS, "BITMAP"), FillBitmapName="bitmap-a", FillBitmapMode=E(BM, m))))
    for p in ("LEFT_TOP", "MIDDLE_TOP", "RIGHT_TOP", "LEFT_MIDDLE", "MIDDLE_MIDDLE", "RIGHT_MIDDLE", "LEFT_BOTTOM",
              "MIDDLE_BOTTOM", "RIGHT_BOTTOM"):
        cases.append(("bmp-ref-" + p, dict(FillStyle=E(FS, "BITMAP"), FillBitmapName="bitmap-a",
                                            FillBitmapMode=E(BM, "REPEAT"), FillBitmapRectanglePoint=E("com.sun.star.drawing.RectanglePoint", p),
                                            FillBitmapPositionOffsetX=25, FillBitmapPositionOffsetY=75,
                                            FillBitmapSizeX=500, FillBitmapSizeY=700, FillBitmapLogicalSize=False)))
    cases.append(("bmp-offx", dict(FillStyle=E(FS, "BITMAP"), FillBitmapName="bitmap-a", FillBitmapOffsetX=30,
                                    FillBitmapLogicalSize=True, FillBitmapSizeX=40, FillBitmapSizeY=40)))
    cases.append(("bmp-offy", dict(FillStyle=E(FS, "BITMAP"), FillBitmapName="bitmap-a", FillBitmapOffsetY=20)))
    return cases


def linecases():
    LS, LJ, LC = "com.sun.star.drawing.LineStyle", "com.sun.star.drawing.LineJoint", "com.sun.star.drawing.LineCap"
    cases = []
    for j in ("NONE", "MITER", "ROUND", "BEVEL", "MIDDLE"):
        cases.append(("join-" + j, dict(LineStyle=E(LS, "SOLID"), LineWidth=300, LineColor=0x336699, LineJoint=E(LJ, j))))
    for c in ("BUTT", "ROUND", "SQUARE"):
        cases.append(("cap-" + c, dict(LineStyle=E(LS, "SOLID"), LineWidth=300, LineCap=E(LC, c))))
    cases.append(("line-none", dict(LineStyle=E(LS, "NONE"))))
    cases.append(("line-dash-a", dict(LineStyle=E(LS, "DASH"), LineDashName="dash-a", LineTransparence=30)))
    cases.append(("line-dash-b", dict(LineStyle=E(LS, "DASH"), LineDashName="dash-b")))
    return cases


def draw_gfx(kind, tag, doc, page, cases, text=None, **extra):
    b = DrawBuilder(doc, page)
    for n, p in cases:
        guard("shape " + n, b.shape, kind, n, p, text, **extra)
    return b


def doc_draw_fill():
    doc = new("private:factory/sdraw")
    guard("tables", draw_tables, doc)
    page = doc.DrawPages.getByIndex(0)
    draw_gfx("RectangleShape", "fill", doc, page, fillprops())
    draw_gfx("EllipseShape", "line", doc, page, linecases())
    p2 = doc.DrawPages.insertNewByIndex(1)
    b = DrawBuilder(doc, p2)
    LS = "com.sun.star.drawing.LineStyle"
    arrows = []
    for sc, ec in ((True, False), (False, True), (True, True), (False, False)):
        arrows.append(("arrow-%d%d" % (sc, ec), dict(LineStyle=E(LS, "SOLID"), LineWidth=200, LineStartName="Arrow",
                                                       LineStartWidth=600, LineStartCenter=sc, LineEndName="Arrow", LineEndWidth=800,
                                                       LineEndCenter=ec)))
    for n, p in arrows:
        guard("line " + n, b.shape, "LineShape", n, p)
    shp = []
    for tr in (0, 25, 75):
        shp.append(("shadow%d" % tr, dict(Shadow=True, ShadowXDistance=200, ShadowYDistance=250, ShadowColor=0x808080,
                                          ShadowTransparence=tr, FillColor=0xaaddff)))
    shp.append(("noshadow", dict(Shadow=False)))
    for n, p in shp:
        guard("shadow " + n, b.shape, "RectangleShape", n, p)
    save(doc, "draw-fill-line.odg", "draw8")
    doc.close(True)


def doc_draw_text():
    doc = new("private:factory/sdraw")
    page = doc.DrawPages.getByIndex(0)
    D = "com.sun.star.drawing."
    b = DrawBuilder(doc, page)
    for a in ("LEFT", "CENTER", "RIGHT", "BLOCK"):
        for v in ("TOP", "CENTER", "BOTTOM", "BLOCK"):
            b.shape("RectangleShape", "tadj-%s-%s" % (a, v), dict(TextHorizontalAdjust=E(D + "TextHorizontalAdjust", a),
                    TextVerticalAdjust=E(D + "TextVerticalAdjust", v)), text="adjust")
    for ft in ("NONE", "PROPORTIONAL", "ALLLINES", "RESIZEATTR"):
        b.shape("RectangleShape", "fit-" + ft, dict(TextFitToSize=E(D + "TextFitToSizeType", ft)), text="fit to size text")
    for i, (gh, gw, cf, ww) in enumerate(((True, True, True, True), (False, False, False, False), (True, False, True, False))):
        b.shape("RectangleShape", "grow%d" % i, dict(TextAutoGrowHeight=gh, TextAutoGrowWidth=gw, TextContourFrame=cf,
                TextWordWrap=ww, TextMinimumFrameHeight=500, TextMinimumFrameWidth=600, TextMaximumFrameHeight=5000,
                TextMaximumFrameWidth=6000, TextLeftDistance=100, TextRightDistance=120, TextUpperDistance=140,
                TextLowerDistance=160), text="grow")
    b.shape("TextShape", "plain", dict(TextAutoGrowHeight=True), text="text shape")
    p2 = doc.DrawPages.insertNewByIndex(1)
    b = DrawBuilder(doc, p2)
    AK, AD = D + "TextAnimationKind", D + "TextAnimationDirection"
    for k in ("NONE", "BLINK", "SCROLL", "ALTERNATE", "SLIDE"):
        for d in ("LEFT", "RIGHT", "UP", "DOWN"):
            b.shape("TextShape", "anim-%s-%s" % (k, d), dict(TextAnimationKind=E(AK, k), TextAnimationDirection=E(AD, d),
                    TextAnimationStartInside=(d in ("LEFT", "UP")), TextAnimationStopInside=(d in ("RIGHT", "UP")),
                    TextAnimationCount=3, TextAnimationDelay=250, TextAnimationAmount=5), text="animated")
    b.shape("TextShape", "anim-defaults", dict(TextAnimationKind=E(AK, "SCROLL"), TextAnimationStartInside=False,
            TextAnimationStopInside=False, TextAnimationAmount=0, TextAnimationCount=0), text="scroll")
    save(doc, "draw-text.odg", "draw8")
    doc.close(True)


def doc_draw_objects():
    doc = new("private:factory/sdraw")
    page = doc.DrawPages.getByIndex(0)
    D = "com.sun.star.drawing."
    b = DrawBuilder(doc, page)
    GM = D + "ColorMode"
    for m in ("STANDARD", "GREYS", "MONO", "WATERMARK"):
        b.shape("GraphicObjectShape", "gfx-" + m, dict(GraphicURL=IMGURL, GraphicColorMode=E(GM, m), AdjustLuminance=10,
                AdjustContrast=-10, Gamma=1.5, AdjustRed=5, AdjustGreen=-5, AdjustBlue=15, Transparency=20))
    b.shape("GraphicObjectShape", "gfx-crop", dict(GraphicURL=IMGURL, GraphicCrop=uno.createUnoStruct("com.sun.star.text.GraphicCrop", 100, 100, 100, 100)))
    mp = doc.DrawPages.insertNewByIndex(1)
    b = DrawBuilder(doc, mp)
    HP, VP = D + "MeasureTextHorzPos", D + "MeasureTextVertPos"
    i = 0
    for h in ("AUTO", "LEFTOUTSIDE", "INSIDE", "RIGHTOUTSIDE"):
        for v in ("AUTO", "EAST", "WEST", "CENTERED"):
            u = (0, 1, 2, 3, 4, 6, 7, 8, 9, 10)[i % 10]
            i += 1
            sh = b.shape("MeasureShape", "meas-%s-%s" % (h, v), dict(MeasureTextHorizontalPosition=E(HP, h),
                         MeasureTextVerticalPosition=E(VP, v), MeasureUnit=u, MeasureShowUnit=bool(i % 2),
                         MeasureBelowReferenceEdge=bool(i % 3), MeasureTextRotate90=bool(i % 2), MeasureDecimalPlaces=i % 4,
                         MeasureLineDistance=300, MeasureHelpLineOverhang=100, MeasureHelpLineDistance=120,
                         MeasureHelpLine1Length=200, MeasureHelpLine2Length=250), size=(3000, 400))
            if sh is not None:
                setp(sh, "meas pos", StartPosition=uno.createUnoStruct(PT, 1000, 8000 + 500 * i),
                     EndPosition=uno.createUnoStruct(PT, 6000, 8000 + 500 * i))
    cp = doc.DrawPages.insertNewByIndex(2)
    b = DrawBuilder(doc, cp)
    for t in (0, 1, 2, 3):
        for ed in (0, 1, 2):
            sh = b.shape("CaptionShape", "cap-%d-%d" % (t, ed), dict(CaptionType=t, CaptionEscapeDirection=ed,
                         CaptionIsFixedAngle=bool(ed % 2), CaptionAngle=45 * (t + 1), CaptionGap=100 + 50 * ed,
                         CaptionLineLength=400, CaptionIsFitLineLength=bool(t % 2), CaptionIsEscapeRelative=bool(t % 2),
                         CaptionEscapeRelative=500 * (t + 1), CaptionEscapeAbsolute=300), text="caption")
            if sh is not None:
                setp(sh, "cap pt", CaptionPoint=uno.createUnoStruct(PT, -1000, -1000))
    ep = doc.DrawPages.insertNewByIndex(3)
    b = DrawBuilder(doc, ep)
    for k in ("STANDARD", "CURVE", "LINE", "LINES"):
        s1 = b.shape("RectangleShape", "c1-" + k, {}, style=False, size=(1500, 1000))
        s2 = b.shape("EllipseShape", "c2-" + k, {}, style=False, size=(1500, 1000))
        c = b.shape("ConnectorShape", "conn-" + k, dict(EdgeKind=E(D + "ConnectorType", k), EdgeNode1HorzDist=100,
                    EdgeNode1VertDist=110, EdgeNode2HorzDist=120, EdgeNode2VertDist=130, EdgeLine1Delta=50, EdgeLine2Delta=60,
                    LineEndName="Arrow", LineEndWidth=400), size=(100, 100))
        if c is not None and s1 is not None and s2 is not None:
            setp(c, "conn", StartShape=s1, EndShape=s2)
    save(doc, "draw-objects.odg", "draw8")
    doc.close(True)


def doc_draw_3d():
    doc = new("private:factory/sdraw")
    page = doc.DrawPages.getByIndex(0)
    D = "com.sun.star.drawing."
    scene = doc.createInstance(D + "Shape3DSceneObject")
    page.add(scene)
    scene.Position = uno.createUnoStruct(PT, 500, 500)
    scene.Size = uno.createUnoStruct(SZ, 12000, 9000)
    NK, TP, TK, TM = D + "NormalsKind", D + "TextureProjectionMode", D + "TextureKind", D + "TextureMode"
    n = 0
    for kind in ("Shape3DCubeObject", "Shape3DSphereObject"):
        for nk in ("SPECIFIC", "FLAT", "SPHERE"):
            for tp in ("OBJECTSPECIFIC", "PARALLEL", "SPHERE"):
                o = guard("3d " + kind, doc.createInstance, D + kind)
                if o is None:
                    continue
                guard("3d add", scene.add, o)
                n += 1
                setp(o, "3d %s %s %s" % (kind, nk, tp), D3DNormalsKind=E(NK, nk), D3DNormalsInvert=bool(n % 2),
                     D3DTextureProjectionX=E(TP, tp), D3DTextureProjectionY=E(TP, tp), D3DTextureKind=E(TK, ("LUMINANCE", "COLOR")[n % 2]),
                     D3DTextureMode=E(TM, ("REPLACE", "MODULATE", "BLEND")[n % 3]), D3DTextureFilter=bool(n % 2),
                     D3DDoubleSided=bool(n % 2), D3DShadow3D=bool(n % 2), D3DMaterialColor=0xcc3300 + n,
                     D3DMaterialEmission=0x101010, D3DMaterialSpecular=0xffffff, D3DMaterialSpecularIntensity=50,
                     D3DHorizontalSegments=8, D3DVerticalSegments=6, D3DPercentDiagonal=10, D3DBackscale=80,
                     D3DEndAngle=3000, D3DDepth=500, D3DCloseFront=bool(n % 2), D3DCloseBack=bool(n % 3))
    setp(scene, "scene", D3DSceneAmbientColor=0x404040, D3DSceneTwoSidedLighting=True, D3DSceneShadowSlant=10,
         D3DSceneShadeMode=E(D + "ShadeMode", "PHONG"))
    for k, nm in (("Shape3DExtrudeObject", "ext"), ("Shape3DLatheObject", "lathe")):
        o = guard("3d " + k, doc.createInstance, D + k)
        if o is None:
            continue
        guard("3d add", scene.add, o)
        pts = uno.createUnoStruct("com.sun.star.drawing.PolyPolygonShape3D")
        pts.SequenceX = ((1000, 3000, 3000, 1000),)
        pts.SequenceY = ((1000, 1000, 3000, 3000),)
        pts.SequenceZ = ((0, 0, 0, 0),)
        setp(o, "3d " + nm, D3DPolyPolygon3D=pts, D3DCloseFront=False, D3DCloseBack=False, D3DDepth=700,
             D3DNormalsKind=E(NK, "FLAT"), D3DEndAngle=2000, D3DBackscale=70)
    save(doc, "draw-3d.odg", "draw8")
    doc.close(True)


def doc_draw_frames():
    doc = new("private:factory/sdraw")
    page = doc.DrawPages.getByIndex(0)
    D = "com.sun.star.drawing."
    b = DrawBuilder(doc, page)
    for i, (sc, br) in enumerate(((True, True), (False, False))):
        b.shape("FrameShape", "frame%d" % i, dict(FrameURL="file:///nonexistent.html", FrameName="fr%d" % i,
                FrameIsAutoScroll=sc, FrameIsBorder=br, FrameMarginWidth=12 + i, FrameMarginHeight=14 + i))
    for i, kind in enumerate(("com.sun.star.drawing.OLE2Shape",)):
        o = guard("ole", doc.createInstance, kind)
        if o is not None:
            page.add(o)
            setp(o, "ole", CLSID="078B7ABA-54FC-457F-8551-6147e776a997")
            o.Position = uno.createUnoStruct(PT, 500, 6000)
            o.Size = uno.createUnoStruct(SZ, 4000, 2000)
    forms = page.Forms
    form = doc.createInstance("com.sun.star.form.component.Form")
    forms.insertByName("Form1", form)
    form = forms.getByName("Form1")
    for i, kind in enumerate(("ScrollBar", "SpinButton", "CommandButton", "ComboBox")):
        m = guard("ctl model", doc.createInstance, "com.sun.star.form.component." + kind)
        cs = guard("control", doc.createInstance, D + "ControlShape")
        if m is None or cs is None:
            continue
        form.insertByName("c%d" % i, m)
        m = form.getByName("c%d" % i)
        page.add(cs)
        cs.Position = uno.createUnoStruct(PT, 500 + 4500 * i, 9000)
        cs.Size = uno.createUnoStruct(SZ, 4000, 1500)
        guard("ctl set", cs.setControl, m)
        setp(cs, "ctl " + kind, ControlSymbolColor=0x0000ff + i, ControlBackground=0xeeeeee, ControlBorder=i % 3,
             ControlBorderColor=0x00ff00)
    save(doc, "draw-frames.odg", "draw8")
    doc.close(True)


def enum_names(name):
    tdm = ctx.getValueByName("/singletons/com.sun.star.reflection.theTypeDescriptionManager")
    return list(tdm.getByHierarchicalName(name).getEnumNames())


def const(name):
    return uno.getConstantByName(name)


def background(doc, page, **props):
    bg = doc.createInstance("com.sun.star.drawing.Background")
    setp(bg, "background", **props)
    guard("set background", setattr, page, "Background", bg)


def bgcases():
    return [c for c in fillprops() if not c[0].startswith("bmp-ref-") or c[0].endswith(("TOP", "BOTTOM"))]


def doc_impress_slides():
    doc = new("private:factory/simpress")
    guard("tables", draw_tables, doc)
    pages = doc.DrawPages
    effects = enum_names("com.sun.star.presentation.FadeEffect")
    spd = ("SLOW", "MEDIUM", "FAST")
    for i, ef in enumerate(effects):
        pg = pages.getByIndex(0) if i == 0 else pages.insertNewByIndex(i)
        setp(pg, "slide " + ef, Effect=E("com.sun.star.presentation.FadeEffect", ef),
             Speed=E("com.sun.star.presentation.AnimationSpeed", spd[i % 3]), Change=i % 3, Duration=1 + i % 5,
             Visible=bool(i % 4), IsBackgroundVisible=bool(i % 3), IsBackgroundObjectsVisible=bool(i % 5),
             IsHeaderVisible=bool(i % 2), IsFooterVisible=bool(i % 3), IsPageNumberVisible=bool(i % 4),
             IsDateTimeVisible=bool(i % 5), HeaderText="hdr%d" % (i % 3), FooterText="ftr%d" % (i % 2),
             DateTimeText="dt", IsDateTimeFixed=bool(i % 2), DateTimeFormat=i % 4)
        setp(pg.NotesPage, "notes " + ef, IsHeaderVisible=bool(i % 2), IsFooterVisible=bool(i % 3),
             IsPageNumberVisible=bool(i % 4), IsDateTimeVisible=bool(i % 5), HeaderText="nhdr%d" % (i % 3),
             FooterText="nftr%d" % (i % 2), DateTimeText="ndt", IsDateTimeFixed=bool(i % 2), DateTimeFormat=i % 4)
    save(doc, "impress-effects.odp", "impress8")
    doc.close(True)


def doc_impress_transitions():
    doc = new("private:factory/simpress")
    pages = doc.DrawPages
    names = [n for n in dir(uno.getClass("com.sun.star.animations.TransitionType")) if n.isupper()] if False else None
    types = {"BARWIPE": 1, "BOXWIPE": 2, "FOURBOXWIPE": 3, "BARNDOORWIPE": 4, "DIAGONALWIPE": 5, "BOWTIEWIPE": 6,
             "VEEWIPE": 8, "ZIGZAGWIPE": 10, "TRIANGLEWIPE": 13, "ARROWHEADWIPE": 14, "PENTAGONWIPE": 15, "HEXAGONWIPE": 16,
             "ELLIPSEWIPE": 17, "EYEWIPE": 18, "ROUNDRECTWIPE": 19, "STARWIPE": 20, "MISCSHAPEWIPE": 21, "CLOCKWIPE": 22,
             "PINWHEELWIPE": 23, "SINGLESWEEPWIPE": 24, "FANWIPE": 25, "DOUBLEFANWIPE": 26, "DOUBLESWEEPWIPE": 27,
             "SALOONDOORWIPE": 28, "WINDSHIELDWIPE": 29, "SNAKEWIPE": 30, "SPIRALWIPE": 31, "PARALLELSNAKESWIPE": 32,
             "BOXSNAKESWIPE": 33, "WATERFALLWIPE": 34, "PUSHWIPE": 35, "SLIDEWIPE": 36, "FADE": 37, "RANDOMBARWIPE": 38,
             "CHECKERBOARDWIPE": 39, "DISSOLVE": 40, "IRISWIPE": 41, "BLINDSWIPE": 42, "FOURBOXWIPE2": 43}
    tc = {}
    for n in types:
        try:
            tc[n] = const("com.sun.star.animations.TransitionType." + n)
        except Exception:
            pass
    subs = []
    for n in ("DEFAULT", "LEFTTORIGHT", "TOPTOBOTTOM", "TOPLEFT", "TOPRIGHT", "BOTTOMRIGHT", "BOTTOMLEFT", "TOPCENTER",
              "RIGHTCENTER", "BOTTOMCENTER", "LEFTCENTER", "CORNERSIN", "CORNERSOUT", "VERTICAL", "HORIZONTAL", "FROMLEFT",
              "FROMTOP", "FROMRIGHT", "FROMBOTTOM", "CROSSFADE", "FADETOCOLOR", "FADEFROMCOLOR", "FADEOVERCOLOR",
              "THREEBLADE", "EIGHTBLADE", "ONEBLADE", "ACROSS", "TOPTOBOTTOMLEFT", "FOURBLADE", "TWOBOXTOP", "FOURBOXVERTICAL",
              "CLOCKWISETOP", "COUNTERCLOCKWISETOP", "DIAGONALBOTTOMLEFT", "DIAGONALTOPLEFT", "RECTANGLE", "CIRCLE",
              "DIAMOND", "HEART", "KEYHOLE", "HORIZONTALLEFT", "VERTICALTOP", "FANOUTVERTICAL", "FANINHORIZONTAL"):
        try:
            subs.append(const("com.sun.star.animations.TransitionSubType." + n))
        except Exception:
            pass
    print("transition types", len(tc), "subtypes", len(subs))
    i = 0
    for n, t in tc.items():
        pg = pages.getByIndex(0) if i == 0 else pages.insertNewByIndex(i)
        setp(pg, "trans " + n, TransitionType=t, TransitionSubtype=subs[i % len(subs)], TransitionDirection=bool(i % 2),
             TransitionFadeColor=0x102030 * (i % 8), Duration=2, Change=1 + i % 2, Speed=E("com.sun.star.presentation.AnimationSpeed", "FAST"))
        i += 1
    for sub in subs:
        pg = pages.insertNewByIndex(i)
        i += 1
        setp(pg, "sub %s" % sub, TransitionType=tc.get("PUSHWIPE", 35), TransitionSubtype=sub, TransitionDirection=False)
    save(doc, "impress-transitions.odp", "impress8")
    doc.close(True)


def doc_impress_backgrounds():
    # after a Draw document in the same session getHandoutMasterPage is missing on a new Impress document: Impress first
    doc = new("private:factory/simpress")
    guard("tables", draw_tables, doc)
    pages = doc.DrawPages
    cases = bgcases()
    for i, (n, p) in enumerate(cases):
        pg = pages.getByIndex(0) if i == 0 else pages.insertNewByIndex(i)
        background(doc, pg, **p)
        setp(pg, "bgpage " + n, IsBackgroundVisible=bool(i % 7), IsBackgroundObjectsVisible=bool(i % 5))
    masters = doc.MasterPages
    mp = masters.getByIndex(0)
    background(doc, mp, **dict(cases[1][1]))
    m2 = masters.insertNewByIndex(1)
    background(doc, m2, **dict(cases[8][1]))
    for nm in ("BackgroundFullSize",):
        setp(m2, "master " + nm, **{nm: True})
        setp(mp, "master " + nm, **{nm: False})
    hm = doc.getHandoutMasterPage()
    setp(hm, "handout", HeaderText="handout header", FooterText="handout footer", DateTimeText="handout date",
         IsDateTimeFixed=True, IsHeaderVisible=True, IsFooterVisible=True, IsDateTimeVisible=True, IsPageNumberVisible=True)
    for pg in (mp, m2):
        background(doc, pg, **dict(cases[4][1]))
    save(doc, "impress-backgrounds.odp", "impress8")
    doc.close(True)


def doc_draw_backgrounds():
    doc = new("private:factory/sdraw")
    guard("tables", draw_tables, doc)
    pages = doc.DrawPages
    for i, (n, p) in enumerate(bgcases()):
        pg = pages.getByIndex(0) if i == 0 else pages.insertNewByIndex(i)
        background(doc, pg, **p)
    save(doc, "draw-backgrounds.odg", "draw8")
    doc.close(True)


WT = "com.sun.star.text."


def border(inner, outer, dist, color=0x3366cc):
    b = st("com.sun.star.table.BorderLine")
    b.Color, b.InnerLineWidth, b.OuterLineWidth, b.LineDistance = color, inner, outer, dist
    return b


def shadow(loc, width=150, color=0x808080, transparent=False):
    sf = st("com.sun.star.table.ShadowFormat")
    sf.Location = E("com.sun.star.table.ShadowLocation", loc)
    sf.ShadowWidth, sf.Color, sf.IsTransparent = width, color, transparent
    return sf


def writer_doc(nparas=20):
    doc = new("private:factory/swriter")
    cur = doc.Text.createTextCursor()
    for i in range(nparas):
        doc.Text.insertString(cur, "Paragraph %d of the frame anchor test text, long enough to wrap." % i, False)
        doc.Text.insertControlCharacter(cur, 0, False)
    return doc


def para_cursor(doc, i):
    en = doc.Text.createEnumeration()
    n = 0
    while en.hasMoreElements():
        p = en.nextElement()
        if n == i:
            return doc.Text.createTextCursorByRange(p.Start)
        n += 1
    return doc.Text.createTextCursor()


class FrameBuilder:
    def __init__(self, doc):
        self.doc, self.n = doc, 0
        self.styles = doc.StyleFamilies.getByName("FrameStyles")

    def make(self, tag, kind, anchor, pre=None, post=None, para=0, text=None, size=(2500, 1200), style=True,
             anchor_frame=None):
        self.n += 1
        STYLE_N[0] += 1
        f = self.doc.createInstance(WT + kind)
        f.AnchorType = E(WT + "TextContentAnchorType", "AT_PARAGRAPH" if anchor == "AT_FRAME" else anchor)
        if kind == "TextFrame":
            f.Width, f.Height = size
        setp(f, tag + " pre", **(pre or {}))
        cur = para_cursor(self.doc, para)
        cur.goRight(5, False)
        guard("insert " + tag, self.doc.Text.insertTextContent, cur, f, False)
        if anchor_frame is not None:
            setp(f, tag + " anchor", AnchorFrame=anchor_frame)
        setp(f, tag, **(post or {}))
        if text is not None:
            guard("text " + tag, setattr, f.Text if kind == "TextFrame" else f, "String", text)
        if style and kind == "TextFrame":
            guard("fstyle " + tag, self.fstyle, "fs-%s-%d" % (tag, STYLE_N[0]),
                  dict(pre or {}, AnchorType=E(WT + "TextContentAnchorType", anchor), **(post or {})))
        return f

    def fstyle(self, name, props):
        s = self.doc.createInstance("com.sun.star.style.FrameStyle")
        self.styles.insertByName(name, s)
        s = self.styles.getByName(name)
        setp(s, "style " + name, quiet=True, **props)


def doc_writer_frames():
    doc = writer_doc()
    fb = FrameBuilder(doc)
    RO = lambda n: n
    for anchor in ("AT_PARAGRAPH", "AT_CHARACTER", "AT_PAGE", "AT_FRAME"):
        base = None
        if anchor == "AT_FRAME":
            base = fb.make("base", "TextFrame", "AT_PARAGRAPH", para=19, text="base frame", size=(8000, 4000))
        for i in range(30):
            post = dict(HoriOrient=i % 6 if i % 6 != 6 else 0, HoriOrientRelation=(i * 7) % 10, PageToggle=bool((i // 6) % 2),
                        VertOrient=i % 10, VertOrientRelation=(i * 3) % 10)
            if i % 6 == 0 or anchor == "AT_PAGE":
                post.update(HoriOrientPosition=500 + 20 * i, VertOrientPosition=300 + 20 * i)
            if anchor == "AT_PAGE":
                post["AnchorPageNo"] = 1 + i % 3
            fb.make("%s-%d" % (anchor, i), "TextFrame", anchor, post=post, para=1 + i % 15, anchor_frame=base,
                    text="f", style=(i < 12))
    for i, anchor in enumerate(("AS_CHARACTER",) * 10):
        fb.make("aschar-%d" % i, "TextFrame", anchor, post=dict(VertOrient=i, VertOrientRelation=0), para=3 + i % 5, text="c")
    save(doc, "writer-frames-pos.odt", "writer8")
    doc.close(True)


def doc_writer_frames2():
    doc = writer_doc()
    fb = FrameBuilder(doc)
    WM = "com.sun.star.text.WrapTextMode"
    i = 0
    for wrap in ("NONE", "THROUGHT", "PARALLEL", "DYNAMIC", "LEFT", "RIGHT"):
        for contour, outside in ((False, False), (True, False), (True, True)):
            i += 1
            fb.make("wrap-%s-%d" % (wrap, i), "TextFrame", "AT_PARAGRAPH", para=i % 15,
                    post=dict(TextWrap=E(WM, wrap), SurroundContour=contour, ContourOutside=outside,
                              SurroundAnchorOnly=bool(i % 2), Opaque=bool(i % 3)), text="w")
    for k in range(1, 4):
        fb.make("infl-%d" % k, "TextFrame", "AT_CHARACTER", para=k, post=dict(WrapInfluenceOnPosition=k), text="i")
    for k in range(5):
        fb.make("wmode-%d" % k, "TextFrame", "AT_PARAGRAPH", para=4 + k, post=dict(WritingMode=k), text="m")
    fb.make("flags", "TextFrame", "AT_PARAGRAPH", para=8, post=dict(Print=False, EditInReadonly=True, ContentProtected=True,
            SizeProtected=True, PositionProtected=True, IsFollowingTextFlow=True), text="flags")
    fb.make("flags2", "TextFrame", "AT_PARAGRAPH", para=9, post=dict(Print=True, EditInReadonly=False), text="flags2")
    for k, (rw, rh, wt) in enumerate(((50, 20, 1), (100, 30, 1), (75, 0, 2), (40, 10, 0))):
        fb.make("rel-%d" % k, "TextFrame", "AT_PARAGRAPH", para=10 + k,
                post=dict(RelativeWidth=rw, RelativeHeight=rh, IsSyncWidthToHeight=bool(k % 2),
                          IsSyncHeightToWidth=bool(k % 2), WidthType=wt, SizeType=wt), text="r")
    for k, loc in enumerate(("TOP_LEFT", "TOP_RIGHT", "BOTTOM_LEFT", "BOTTOM_RIGHT", "NONE")):
        fb.make("shadow-%s" % loc, "TextFrame", "AT_PARAGRAPH", para=k,
                post=dict(ShadowFormat=shadow(loc, 100 + 20 * k, transparent=bool(k % 2)), LeftMargin=100 * k, RightMargin=100 * k,
                          TopMargin=100 * k, BottomMargin=100 * k), text="s")
    brd = (border(20, 30, 40), border(25, 45, 30), border(10, 10, 10), border(35, 15, 20))
    fb.make("borders-all", "TextFrame", "AT_PARAGRAPH", para=5, text="b",
            post=dict(LeftBorder=brd[0], RightBorder=brd[0], TopBorder=brd[0], BottomBorder=brd[0], LeftBorderDistance=100,
                      RightBorderDistance=100, TopBorderDistance=100, BottomBorderDistance=100, LeftMargin=200, RightMargin=200,
                      TopMargin=200, BottomMargin=200))
    fb.make("borders-each", "TextFrame", "AT_PARAGRAPH", para=6, text="b",
            post=dict(LeftBorder=brd[0], RightBorder=brd[1], TopBorder=brd[2], BottomBorder=brd[3], LeftBorderDistance=100,
                      RightBorderDistance=110, TopBorderDistance=120, BottomBorderDistance=130, LeftMargin=100, RightMargin=200,
                      TopMargin=300, BottomMargin=400))
    FS = "com.sun.star.drawing.FillStyle"
    fb.make("frame-fill", "TextFrame", "AT_PARAGRAPH", para=7, text="f",
            post=dict(FillStyle=E(FS, "SOLID"), FillColor=0xccffcc, FillColor2=0xffcccc, FillTransparence=30))
    fb.make("frame-back", "TextFrame", "AT_PARAGRAPH", para=7, text="f",
            post=dict(BackColor=0xaabbcc, BackTransparent=False, BackColorTransparency=20))
    save(doc, "writer-frames-misc.odt", "writer8")
    doc.close(True)


def doc_writer_objects():
    doc = writer_doc()
    fb = FrameBuilder(doc)
    CM = "com.sun.star.drawing.ColorMode"
    for i, m in enumerate(("STANDARD", "GREYS", "MONO", "WATERMARK")):
        g = fb.make("gfx-" + m, "TextGraphicObject", "AT_PARAGRAPH", para=i, style=False, pre=dict(GraphicURL=IMGURL),
                    post=dict(GraphicColorMode=E(CM, m), GraphicIsInverted=bool(i % 2), AdjustLuminance=10 * i,
                              AdjustContrast=-5 * i, AdjustRed=i, AdjustGreen=-i, AdjustBlue=2 * i, Gamma=1.0 + i / 10.0,
                              Transparency=10 * i, HoriMirroredOnEvenPages=bool(i % 2), HoriMirroredOnOddPages=bool(i % 3),
                              VertMirrored=bool(i % 2), Width=2000, Height=1500))
    for i in range(3):
        fb.make("gfx-pos-%d" % i, "TextGraphicObject", ("AT_CHARACTER", "AS_CHARACTER", "AT_PAGE")[i], para=5 + i, style=False,
                pre=dict(GraphicURL=IMGURL), post=dict(Width=1500, Height=1000, HoriOrient=i, VertOrient=i + 1))
    S = "com.sun.star.drawing."
    for i, anchor in enumerate(("AT_PARAGRAPH", "AT_CHARACTER", "AS_CHARACTER", "AT_PAGE")):
        for j in range(8):
            shp = doc.createInstance(S + "RectangleShape")
            shp.AnchorType = E(WT + "TextContentAnchorType", anchor)
            doc.DrawPage.add(shp)
            shp.Size = uno.createUnoStruct(SZ, 1500, 1000)
            shp.Position = uno.createUnoStruct(PT, 500 + 200 * j, 500 + 300 * i)
            STYLE_N[0] += 1
            post = dict(HoriOrient=j % 6, HoriOrientRelation=(j * 3) % 10, VertOrient=j % 10, VertOrientRelation=(j * 7) % 10,
                        PageToggle=bool(j % 2), TextWrap=E("com.sun.star.text.WrapTextMode", ("NONE", "THROUGHT", "PARALLEL", "DYNAMIC", "LEFT", "RIGHT")[j % 6]),
                        SurroundContour=bool(j % 2), ContourOutside=bool(j % 3), SurroundAnchorOnly=bool(j % 2), Opaque=bool(j % 2),
                        WrapInfluenceOnPosition=1 + j % 3, IsFollowingTextFlow=bool(j % 2), LeftMargin=100, RightMargin=100,
                        TopMargin=100, BottomMargin=100)
            if anchor == "AS_CHARACTER":
                post.pop("HoriOrient"); post.pop("HoriOrientRelation")
            setp(shp, "wshape %s %d" % (anchor, j), **post)
    emb = guard("ole", doc.createInstance, WT + "TextEmbeddedObject")
    for clsid in ("078B7ABA-54FC-457F-8551-6147e776a997", "47BBB4CB-CE4C-4E80-A591-42D9AE74950F"):
        emb = guard("ole", doc.createInstance, WT + "TextEmbeddedObject")
        if emb is None:
            continue
        setp(emb, "ole", CLSID=clsid, AnchorType=E(WT + "TextContentAnchorType", "AT_PARAGRAPH"))
        guard("ole insert", doc.Text.insertTextContent, para_cursor(doc, 12), emb, False)
        setp(emb, "ole post", Width=3000, Height=2000)
    save(doc, "writer-objects.odt", "writer8")
    doc.close(True)


def pagestyle(doc, name, props, service="com.sun.star.style.PageStyle"):
    fam = doc.StyleFamilies.getByName("PageStyles")
    s = doc.createInstance(service)
    fam.insertByName(name, s)
    s = fam.getByName(name)
    setp(s, "pagestyle " + name, **props)
    return s


def hf_props(pre, i):
    b = (border(20, 30, 40), border(10, 25, 15), border(35, 15, 20), border(5, 20, 10))
    d = dict()
    d[pre + "IsOn"] = True
    d[pre + "IsShared"] = bool(i % 2)
    d[pre + "IsDynamicHeight"] = bool(i % 2)
    d[pre + "Height"] = 800 + 50 * i
    d[pre + "BodyDistance"] = 100 + 10 * i
    d[pre + "LeftMargin"] = 100 + 10 * i
    d[pre + "RightMargin"] = 100 + 10 * i
    d[pre + "DynamicSpacing"] = bool(i % 2)
    if i % 3 == 0:
        d[pre + "LeftBorder"] = d[pre + "RightBorder"] = d[pre + "TopBorder"] = d[pre + "BottomBorder"] = b[0]
        d[pre + "LeftBorderDistance"] = d[pre + "RightBorderDistance"] = d[pre + "TopBorderDistance"] = d[pre + "BottomBorderDistance"] = 120
    else:
        d[pre + "LeftBorder"], d[pre + "RightBorder"], d[pre + "TopBorder"], d[pre + "BottomBorder"] = b
        d[pre + "LeftBorderDistance"], d[pre + "RightBorderDistance"], d[pre + "TopBorderDistance"], d[pre + "BottomBorderDistance"] = 100, 110, 120, 130
    d[pre + "ShadowFormat"] = shadow(("TOP_LEFT", "TOP_RIGHT", "BOTTOM_LEFT", "BOTTOM_RIGHT", "NONE")[i % 5])
    d[pre + "BackColor"] = 0xeeeeee
    d[pre + "BackTransparent"] = bool(i % 2)
    return d


def page_common(i):
    b = (border(20, 30, 40), border(10, 25, 15), border(35, 15, 20), border(5, 20, 10))
    d = dict(ShadowFormat=shadow(("TOP_LEFT", "TOP_RIGHT", "BOTTOM_LEFT", "BOTTOM_RIGHT", "NONE")[i % 5]))
    if i % 2:
        d.update(LeftBorder=b[0], RightBorder=b[1], TopBorder=b[2], BottomBorder=b[3], LeftBorderDistance=100,
                 RightBorderDistance=110, TopBorderDistance=120, BottomBorderDistance=130)
    else:
        d.update(LeftBorder=b[0], RightBorder=b[0], TopBorder=b[0], BottomBorder=b[0], LeftBorderDistance=100,
                 RightBorderDistance=100, TopBorderDistance=100, BottomBorderDistance=100)
    d.update(LeftMargin=1500, RightMargin=1500, TopMargin=1500, BottomMargin=1500)
    return d


def doc_writer_pages():
    doc = new("private:factory/swriter")
    doc.Text.insertString(doc.Text.createTextCursor(), "page style test", False)
    guard("tables", draw_tables, doc)
    n = 0
    for wm in range(5):
        n += 1
        pagestyle(doc, "ps-wm-%d" % wm, dict(WritingMode=wm))
    for lay in ("ALL", "LEFT", "RIGHT", "MIRRORED"):
        pagestyle(doc, "ps-layout-" + lay, dict(PageStyleLayout=E("com.sun.star.style.PageStyleLayout", lay)))
    for nt in (0, 1, 2, 3, 4, 5, 6, 7, 9, 10, 11, 20, 30, 40):
        pagestyle(doc, "ps-num-%d" % nt, dict(NumberingType=nt))
    pagestyle(doc, "ps-tray", dict(RegisterModeActive=True, RegisterParagraphStyle="Text body",
              FirstPageNumber=0, IsLandscape=True))
    for gm in (0, 1, 2):
        for std in (True, False):
            pagestyle(doc, "ps-grid-%s-%d" % (gm, std), dict(GridMode=gm, GridDisplay=bool(std), GridPrint=not std,
                      GridBaseHeight=500, GridBaseWidth=400, GridRubyHeight=200, GridLines=20, GridColor=0x00ff00,
                      RubyBelow=not std, StandardPageMode=std, GridSnapToChars=bool(std)))
    for i in range(6):
        d = page_common(i)
        d.update(hf_props("Header", i))
        d.update(hf_props("Footer", i + 1))
        pagestyle(doc, "ps-hf-%d" % i, d)
    for i, adj in enumerate((0, 1, 2)):
        pagestyle(doc, "ps-ftn-%d" % i, dict(FootnoteLineAdjust=adj, FootnoteLineWeight=20 + 10 * i, FootnoteLineColor=0xff0000,
                  FootnoteLineRelativeWidth=50 + i, FootnoteLineDistance=100, FootnoteLineTextDistance=50, FootnoteHeight=2000))
    nfam = doc.StyleFamilies.getByName("NumberingStyles")
    for lf in (0, 1, 2):
        ns = doc.createInstance("com.sun.star.style.NumberingStyle")
        nfam.insertByName("ns-lf-%d" % lf, ns)
        ns = nfam.getByName("ns-lf-%d" % lf)
        rule = ns.NumberingRules
        lvl = [pv(p.Name, lf if p.Name == "LabelFollowedBy" else p.Value) for p in rule.getByIndex(0)]
        guard("numbering", uno.invoke, rule, "replaceByIndex", (0, uno.Any("[]com.sun.star.beans.PropertyValue", tuple(lvl))))
        guard("numbering set", setattr, ns, "NumberingRules", rule)
        doc.Text.insertString(doc.Text.createTextCursor(), "list %d" % lf, False)
    pagestyle(doc, "ps-hdr-off", dict(HeaderIsOn=False, FooterIsOn=False))
    pagestyle(doc, "ps-hdr-on", dict(HeaderIsOn=True, FooterIsOn=True, HeaderIsShared=True, FooterIsShared=True))
    pagestyle(doc, "ps-hdr-left", dict(HeaderIsOn=True, FooterIsOn=True, HeaderIsShared=False, FooterIsShared=False))
    for k, (hi, fi) in enumerate(((True, False), (False, True))):
        pagestyle(doc, "ps-hdr-ft-%d" % k, dict(HeaderIsOn=hi, FooterIsOn=fi, HeaderIsShared=bool(k), FooterIsShared=not k))
    FS = "com.sun.star.drawing.FillStyle"
    for i, (nm, p) in enumerate(fillprops()[:14]):
        pagestyle(doc, "ps-fill-%d" % i, dict(p, **{("Header" + k[0:]): v for k, v in p.items()}, HeaderIsOn=True))
    save(doc, "writer-pagestyles.odt", "writer8")
    doc.close(True)


def doc_writer_tables():
    doc = new("private:factory/swriter")
    guard("bits", draw_tables, doc)
    cur = doc.Text.createTextCursor()
    cur.gotoEnd(False)
    BT = "com.sun.star.style.BreakType"
    cases = []
    for bt in ("NONE", "COLUMN_BEFORE", "COLUMN_AFTER", "COLUMN_BOTH", "PAGE_BEFORE", "PAGE_AFTER", "PAGE_BOTH"):
        cases.append(("brk-" + bt, dict(BreakType=E(BT, bt))))
    for k in (True, False):
        cases.append(("keep%d" % k, dict(KeepTogether=k, Split=k)))
    for ho in range(0, 6):
        cases.append(("hori%d" % ho, dict(HoriOrient=ho, LeftMargin=100, RightMargin=200)))
    cases.append(("rel", dict(RelativeWidth=60)))
    cases.append(("abs", dict(Width=8000, IsWidthRelative=False)))
    cases.append(("margins", dict(LeftMargin=300, RightMargin=300, TopMargin=300, BottomMargin=300, BackColor=0xffffcc, BackTransparent=False)))
    for loc in ("TOP_LEFT", "TOP_RIGHT", "BOTTOM_LEFT", "BOTTOM_RIGHT", "NONE"):
        cases.append(("shadow-" + loc, dict(ShadowFormat=shadow(loc))))
    for wm in range(5):
        cases.append(("wm%d" % wm, dict(WritingMode=wm)))
    cases.append(("collapse", dict(CollapsingBorders=True)))
    cases.append(("collapse0", dict(CollapsingBorders=False)))
    cases.append(("pagedesc", dict(PageDescName="Standard", PageNumberOffset=3)))
    for n, props in cases:
        doc.Text.insertString(cur, "table " + n, False)
        doc.Text.insertControlCharacter(cur, 0, False)
        t = doc.createInstance(WT + "TextTable")
        t.initialize(3, 3)
        guard("ins " + n, doc.Text.insertTextContent, cur, t, False)
        setp(t, "table " + n, **props)
        for r in range(3):
            row = t.Rows.getByIndex(r)
            setp(row, "row %s %d" % (n, r), BackColor=0xddeeff if r == 1 else 0xffffff, BackTransparent=(r != 1),
                 Height=700 + 100 * r, IsAutoHeight=bool(r % 2), IsSplitAllowed=bool(r % 2))
        for ci, cn in enumerate(("A1", "B1", "C2", "B3")):
            c = t.getCellByName(cn)
            c.String = cn
            setp(c, "cell " + n + cn, BackColor=0xffeeee, VertOrient=ci, LeftBorder=border(20, 30, 40), RightBorder=border(10, 25, 15),
                 TopBorder=border(35, 15, 20), BottomBorder=border(5, 20, 10), LeftBorderDistance=100, RightBorderDistance=110,
                 TopBorderDistance=120, BottomBorderDistance=130, WritingMode=ci, ShadowFormat=shadow("BOTTOM_RIGHT"))
        doc.Text.insertControlCharacter(cur, 0, False)
    cols = t.TableColumnSeparators
    save(doc, "writer-tables.odt", "writer8")
    doc.close(True)


def calc_style(doc, family, name, props):
    fam = doc.StyleFamilies.getByName(family)
    s = doc.createInstance("com.sun.star.style.%s" % ("CellStyle" if family == "CellStyles" else "PageStyle"))
    fam.insertByName(name, s)
    s = fam.getByName(name)
    setp(s, family + " " + name, **props)
    return s


def doc_calc_cells():
    doc = new("private:factory/scalc")
    sh = doc.Sheets.getByIndex(0)
    CJ = "com.sun.star.table.CellHoriJustify"
    CV = "com.sun.star.table.CellVertJustify"
    cases = []
    for hj in ("STANDARD", "LEFT", "CENTER", "RIGHT", "BLOCK", "REPEAT"):
        cases.append(("hj-" + hj, dict(HoriJustify=E(CJ, hj), IsTextWrapped=bool(hj == "BLOCK"))))
    for rr in ("STANDARD", "TOP", "CENTER", "BOTTOM"):
        cases.append(("rot-" + rr, dict(RotateReference=E(CV, rr), RotateAngle=4500, VertJustify=1 if rr == "TOP" else 2)))
    for v in ("STANDARD", "TOP", "CENTER", "BOTTOM"):
        cases.append(("vj-" + v, dict(VertJustify=("STANDARD", "TOP", "CENTER", "BOTTOM").index(v))))
    cases.append(("shrink", dict(ShrinkToFit=True)))
    cases.append(("noshrink", dict(ShrinkToFit=False)))
    cases.append(("vertical", dict(AsianVerticalMode=True)))
    cases.append(("vertical0", dict(AsianVerticalMode=False)))
    for loc in ("TOP_LEFT", "TOP_RIGHT", "BOTTOM_LEFT", "BOTTOM_RIGHT", "NONE"):
        cases.append(("shadow-" + loc, dict(ShadowFormat=shadow(loc))))
    cp = st("com.sun.star.util.CellProtection")
    cp.IsLocked, cp.IsFormulaHidden, cp.IsHidden, cp.IsPrintHidden = False, True, True, True
    cases.append(("prot-print", dict(CellProtection=cp)))
    cp2 = st("com.sun.star.util.CellProtection")
    cp2.IsLocked, cp2.IsFormulaHidden, cp2.IsHidden, cp2.IsPrintHidden = True, False, False, False
    cases.append(("prot-lock", dict(CellProtection=cp2)))
    cases.append(("diag", dict(DiagonalTLBR=border(20, 30, 40), DiagonalBLTR=border(10, 25, 15))))
    cases.append(("diag2", dict(DiagonalTLBR=border(0, 20, 0), DiagonalBLTR=border(35, 15, 20))))
    cases.append(("borders", dict(LeftBorder=border(20, 30, 40), RightBorder=border(10, 25, 15), TopBorder=border(35, 15, 20),
                                  BottomBorder=border(5, 20, 10), ParaLeftMargin=100, ParaRightMargin=110, ParaTopMargin=120,
                                  ParaBottomMargin=130)))
    cases.append(("borders-all", dict(LeftBorder=border(20, 30, 40), RightBorder=border(20, 30, 40), TopBorder=border(20, 30, 40),
                                      BottomBorder=border(20, 30, 40), ParaLeftMargin=100, ParaRightMargin=100, ParaTopMargin=100,
                                      ParaBottomMargin=100)))
    for wm in range(5):
        cases.append(("wm%d" % wm, dict(WritingMode=wm)))
    cases.append(("bg", dict(CellBackColor=0xffeecc, IsCellBackgroundTransparent=False, ParaIndent=200, NumberFormat=2,
                             StandardDecimals=3)))
    for i, (n, props) in enumerate(cases):
        guard("cellstyle " + n, calc_style, doc, "CellStyles", "cs-" + n, props)
        c = sh.getCellByPosition(i % 8, i // 8)
        c.String = n
        setp(c, "cell " + n, **props)
        guard("apply " + n, setattr, c, "CellStyle", "cs-" + n)
    save(doc, "calc-cells.ods", "calc8")
    doc.close(True)


def doc_calc_sheets():
    doc = new("private:factory/scalc")
    sheets = doc.Sheets
    for i in range(1, 9):
        sheets.insertNewByName("s%d" % i, i)
    for i in range(sheets.Count):
        sh = sheets.getByIndex(i)
        sh.getCellByPosition(0, 0).String = "sheet %d" % i
        setp(sh, "sheet %d" % i, TableLayout=i % 6, IsVisible=bool(i % 3), TabColor=0xff0000 + i)
        cols, rows = sh.Columns, sh.Rows
        for j in range(4):
            cs = cols.getByIndex(j)
            setp(cs, "col %d %d" % (i, j), Width=2000 + 300 * j, OptimalWidth=bool((i + j) % 2), IsVisible=bool((i + j) % 4),
                 IsManualPageBreak=bool((i + j) % 2), IsStartOfNewPage=bool(j % 2))
            rw = rows.getByIndex(j)
            setp(rw, "row %d %d" % (i, j), Height=500 + 100 * j, OptimalHeight=bool((i + j) % 2), IsVisible=bool((i + j) % 4),
                 IsManualPageBreak=bool((i + j) % 2), IsStartOfNewPage=bool(j % 2), CellBackColor=0xccffcc, IsCellBackgroundTransparent=bool(j % 2))
    save(doc, "calc-sheets.ods", "calc8")
    doc.close(True)


def doc_calc_pages():
    doc = new("private:factory/scalc")
    sh = doc.Sheets.getByIndex(0)
    sh.getCellByPosition(0, 0).String = "pages"
    guard("tables", draw_tables, doc)
    n = 0
    for pdf in (True, False):
        for ch, cv in ((True, False), (False, True), (True, True), (False, False)):
            n += 1
            calc_style(doc, "PageStyles", "pg-%d" % n, dict(PrintDownFirst=pdf, CenterHorizontally=ch, CenterVertically=cv,
                       ScaleToPagesX=n, ScaleToPagesY=n + 1, PrintAnnotations=bool(n % 2), PrintCharts=bool(n % 3), PrintDrawing=bool(n % 2),
                       PrintFormulas=bool(n % 2), PrintGrid=bool(n % 3), PrintHeaders=bool(n % 2), PrintObjects=bool(n % 3),
                       PrintZeroValues=bool(n % 2), IsLandscape=bool(n % 2)))
    for k in (1, 2, 5):
        calc_style(doc, "PageStyles", "pg-scale-pages-%d" % k, dict(ScaleToPages=k))
        calc_style(doc, "PageStyles", "pg-scale-%d" % k, dict(PageScale=50 + 10 * k))
    for lay in ("ALL", "LEFT", "RIGHT", "MIRRORED"):
        calc_style(doc, "PageStyles", "pg-layout-" + lay, dict(PageStyleLayout=E("com.sun.star.style.PageStyleLayout", lay)))
    for nt in (0, 1, 2, 3, 4, 6, 7):
        calc_style(doc, "PageStyles", "pg-num-%d" % nt, dict(NumberingType=nt))
    for wm in range(5):
        calc_style(doc, "PageStyles", "pg-wm-%d" % wm, dict(WritingMode=wm))
    for i in range(6):
        d = page_common(i)
        d.update(hf_props("Header", i))
        d.update(hf_props("Footer", i + 1))
        calc_style(doc, "PageStyles", "pg-hf-%d" % i, d)
    for i in range(3):
        calc_style(doc, "PageStyles", "pg-back-%d" % i, dict(BackColor=0xffeecc, BackTransparent=bool(i % 2), HeaderIsOn=True,
                   HeaderBackColor=0xccddee, HeaderBackTransparent=bool(i % 2)))
    calc_style(doc, "PageStyles", "pg-hdr-off", dict(HeaderIsOn=False, FooterIsOn=False))
    for k, (hi, fi, hs, fs_) in enumerate(((True, True, True, True), (True, True, False, False), (False, True, True, False))):
        calc_style(doc, "PageStyles", "pg-hf-onoff-%d" % k, dict(HeaderIsOn=hi, FooterIsOn=fi, HeaderIsShared=hs, FooterIsShared=fs_))
    save(doc, "calc-pagestyles.ods", "calc8")
    doc.close(True)


def fill_table(doc, page):
    t = doc.createInstance("com.sun.star.drawing.TableShape")
    page.add(t)
    t.Position = uno.createUnoStruct(PT, 1000, 1000)
    t.Size = uno.createUnoStruct(SZ, 12000, 6000)
    m = t.Model
    m.Columns.insertByIndex(1, 3)
    m.Rows.insertByIndex(1, 2)
    for i in range(m.Columns.Count):
        setp(m.Columns.getByIndex(i), "col %d" % i, Width=2000 + 500 * i, OptimalWidth=bool(i % 2), IsVisible=bool(i % 3),
             IsStartOfNewPage=bool(i % 2))
    for i in range(m.Rows.Count):
        setp(m.Rows.getByIndex(i), "row %d" % i, Height=800 + 100 * i, OptimalHeight=bool(i % 2), IsVisible=bool(i % 3),
             IsStartOfNewPage=bool(i % 2))
    FS = "com.sun.star.drawing.FillStyle"
    for r in range(m.Rows.Count):
        for c in range(m.Columns.Count):
            cell = m.getCellByPosition(c, r)
            cell.String = "c%d%d" % (c, r)
            setp(cell, "cell", FillStyle=E(FS, "SOLID"), FillColor=0xffcc00 + 17 * (r + c),
                 TextVerticalAdjust=E("com.sun.star.drawing.TextVerticalAdjust", ("TOP", "CENTER", "BOTTOM", "BLOCK")[(r + c) % 4]),
                 TextHorizontalAdjust=E("com.sun.star.drawing.TextHorizontalAdjust", ("LEFT", "CENTER", "RIGHT", "BLOCK")[(r + c) % 4]),
                 LeftBorder=border(20, 30, 40), RightBorder=border(10, 25, 15), TopBorder=border(35, 15, 20),
                 BottomBorder=border(5, 20, 10), TextLeftDistance=100, TextRightDistance=120, TextUpperDistance=140,
                 TextLowerDistance=160, ParaAdjust=(r + c) % 4)


def doc_impress_tables():
    doc = new("private:factory/simpress")
    guard("tables", draw_tables, doc)
    # releasing a table cell proxy after the document is closed crashes the office
    fill_table(doc, doc.DrawPages.getByIndex(0))
    gc.collect()
    save(doc, "impress-table.odp", "impress8")
    doc.close(True)


DOCS = [("impress-backgrounds", doc_impress_backgrounds), ("impress-effects", doc_impress_slides),
        ("impress-table", doc_impress_tables),
        ("impress-transitions", doc_impress_transitions), ("draw-fill", doc_draw_fill), ("draw-text", doc_draw_text),
        ("draw-objects", doc_draw_objects), ("draw-3d", doc_draw_3d), ("draw-frames", doc_draw_frames),
        ("draw-backgrounds", doc_draw_backgrounds), ("writer-frames-pos", doc_writer_frames),
        ("writer-frames-misc", doc_writer_frames2), ("writer-objects", doc_writer_objects),
        ("writer-pagestyles", doc_writer_pages), ("writer-tables", doc_writer_tables), ("calc-cells", doc_calc_cells),
        ("calc-sheets", doc_calc_sheets), ("calc-pagestyles", doc_calc_pages)]
for name, fn in DOCS:
    if wanted(name):
        guard(name, fn)
shutil.rmtree(work, ignore_errors=True)
sys.exit(1 if failed else 0)
