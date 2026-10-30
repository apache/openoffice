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
#**************************************************************
# usage: runpy.sh fixtures/draw/make.py <port> <outdir>
# Draw and Impress features the corpus lacks: animations, transitions, presentation settings, click actions, image maps,
# 3D scenes, custom shapes, fontwork, connectors, glue points, measure, captions, plugin/applet/frame, page anchors.
# Saves with the office's configured ODF version: run it against the strict and extended profiles.
import sys, os, math, wave, struct, tempfile, threading, uno
from unohelp import connect, desktop, pv, odf_version

port, out = int(sys.argv[1]), os.path.abspath(sys.argv[2])
os.makedirs(out, exist_ok=True)
ctx = connect(port)
smgr = ctx.ServiceManager
desk = desktop(ctx)
failed = []
STEP_SECONDS = 90
crashes = []
ONLY = set(sys.argv[3:]) | set(os.environ.get("DRAW_ONLY", "").split())
tmp = tempfile.mkdtemp(prefix="t12_draw_")
SOUND = os.path.join(tmp, "beep.wav")
w = wave.open(SOUND, "wb")
w.setnchannels(1); w.setsampwidth(2); w.setframerate(8000)
w.writeframes(b"".join(struct.pack("<h", int(8000 * math.sin(i / 5.0))) for i in range(4000)))
w.close()
SOUND_URL = uno.systemPathToFileUrl(SOUND)

# online spelling runs on a timer and was seen crashing the office on closed documents (ImpEditEngine::DoOnlineSpelling)
def quiet_spelling():
    try:
        smgr.createInstanceWithContext("com.sun.star.linguistic2.LinguProperties", ctx).IsSpellAuto = False
    except Exception as e:
        print("  cannot disable online spelling", repr(e)[:100])


quiet_spelling()
PROFILE = "t12_draw_" + ("strict" if odf_version(ctx) == 4 else "ext")

CLIP = {"odp": "impress8", "odg": "draw8", "otp": "impress8_template", "otg": "draw8_template",
        "odt": "writer8", "ods": "calc8"}
FACTORY = {"odp": "simpress", "odg": "sdraw", "odt": "swriter", "ods": "scalc"}


def new(kind):
    return desk.loadComponentFromURL("private:factory/" + FACTORY[kind], "_blank", 0, (pv("Hidden", True),))


KEEP = []


def done(doc):
    # closing documents or releasing annotation/table proxies after a close crashed the office (use after free); leak them and _exit
    KEEP.append(doc)


def save(doc, name):
    try:
        doc.storeToURL(uno.systemPathToFileUrl(os.path.join(out, name)), (pv("FilterName", CLIP[name.rsplit(".", 1)[1]]),))
        print("saved", name)
    except Exception as e:
        failed.append(name)
        print("FAIL save", name, repr(e)[:300])


def step(label, fn, *a, **kw):
    print('  step', label, flush=True)
    try:
        return fn(*a, **kw)
    except Exception as e:
        failed.append(label)
        import traceback
        print("FAIL", label, type(e).__name__, repr(e)[:300], traceback.format_exc().splitlines()[-3:-1])


def office_alive():
    try:
        desk.getImplementationName()
        return True
    except Exception:
        return False


def kill_office():
    import subprocess
    print("WATCHDOG: step exceeded %ds, killing the office" % STEP_SECONDS, flush=True)
    subprocess.run(["pkill", "-f", "[p]rofile_" + PROFILE])


def restart_office():
    global ctx, smgr, desk
    import subprocess
    harness = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(sys.argv[0]))))
    prof = PROFILE
    subprocess.run(["pkill", "-f", "[p]rofile_" + prof])
    subprocess.run([os.path.join(harness, "start.sh"), prof, str(port)], check=True)
    ctx = connect(port)
    smgr = ctx.ServiceManager
    desk = desktop(ctx)
    quiet_spelling()


def top(label, fn, *a):
    if ONLY and label not in ONLY:
        return
    print('step', label, flush=True)
    for attempt in (1, 2):
        nfailed = len(failed)
        dog = threading.Timer(STEP_SECONDS, kill_office)
        dog.start()
        try:
            fn(*a)
            return
        except Exception as e:
            if office_alive():
                failed.append(label)
                print("FAIL", label, type(e).__name__, repr(e)[:300])
                return
            del failed[nfailed:]
            crashes.append(label)
            print("CRASH", label, "attempt", attempt, repr(e)[:150])
            restart_office()
        finally:
            dog.cancel()
    failed.append(label)


def struct_(name, *a):
    return uno.createUnoStruct(name, *a)


def pt(x, y):
    return struct_("com.sun.star.awt.Point", x, y)


def sz(w, h):
    return struct_("com.sun.star.awt.Size", w, h)


def mk(doc, page, service, x=1000, y=1000, w=3000, h=2000, text=None, **props):
    s = doc.createInstance("com.sun.star.drawing." + service)
    page.add(s)
    if w is not None:
        s.Position = pt(x, y)
        s.Size = sz(w, h)
    if text is not None:
        s.String = text
    for k, v in props.items():
        try:
            setattr(s, k, v)
        except Exception as e:
            print("  prop", service, k, repr(e)[:120])
    return s


def setp(obj, **props):
    for k, v in props.items():
        try:
            setattr(obj, k, v)
        except Exception as e:
            print("  prop", k, repr(e)[:120])


def spec(name):
    return smgr.createInstanceWithContext("com.sun.star.animations." + name, ctx)


def add_page(doc):
    doc.DrawPages.insertNewByIndex(doc.DrawPages.Count)
    return doc.DrawPages.getByIndex(doc.DrawPages.Count - 1)


def presentation_settings():
    doc = new("odp")
    pages = [doc.DrawPages.getByIndex(0)] + [add_page(doc) for _ in range(3)]
    for i, p in enumerate(pages):
        p.Name = "Slide%d" % (i + 1)
        mk(doc, p, "RectangleShape", text="slide %d" % (i + 1))
    shows = doc.CustomPresentations
    for nm, idx in (("ShowA", (0, 2)), ("ShowB", (1, 3, 0))):
        show = shows.createInstance()
        for i in idx:
            show.insertByIndex(show.Count, pages[i])
        shows.insertByName(nm, show)
    pres = doc.Presentation
    pres.IsShowAll = False
    pres.CustomShow = "ShowB"
    pres.IsEndless = True
    pres.Pause = 7
    pres.AllowAnimations = False
    pres.IsAlwaysOnTop = True
    pres.IsAutomatic = True
    pres.IsFullScreen = False
    pres.IsMouseVisible = False
    pres.StartWithNavigator = True
    pres.UsePen = True
    pres.IsTransitionOnClick = False
    pres.IsShowLogo = True
    save(doc, "pres-settings.odp")
    pres.CustomShow = ""
    pres.FirstPage = "Slide3"
    save(doc, "pres-settings-start.odp")
    done(doc)


def header_footer():
    doc = new("odp")
    p1 = doc.DrawPages.getByIndex(0)
    p2 = add_page(doc)
    for i, p in enumerate((p1, p2)):
        mk(doc, p, "RectangleShape", text="hf %d" % i)
        setp(p, IsFooterVisible=True, FooterText="footer %d" % i, IsPageNumberVisible=True, IsDateTimeVisible=True)
    setp(p1, IsDateTimeFixed=True, DateTimeText="fixed date")
    setp(p2, IsDateTimeFixed=False, DateTimeFormat=0x2002)
    for p in (p1, p2):
        setp(p.NotesPage, IsHeaderVisible=True, HeaderText="notes header", IsFooterVisible=True, FooterText="notes footer",
             IsDateTimeVisible=True, IsDateTimeFixed=True, DateTimeText="notes date", IsPageNumberVisible=True)
    setp(doc.MasterPages.getByIndex(0), IsFooterVisible=True, FooterText="master footer", IsPageNumberVisible=True)
    try:
        hm = doc.getHandoutMasterPage()
    except Exception as e:
        hm = None
        print("  handout master", repr(e)[:100])
    hm is not None and setp(hm, IsHeaderVisible=True, HeaderText="handout header", IsFooterVisible=True,
         FooterText="handout footer", IsDateTimeVisible=True, IsDateTimeFixed=True, DateTimeText="handout date")
    save(doc, "header-footer.odp")
    done(doc)


def layouts():
    doc = new("odp")
    first = True
    for n in range(0, 40):
        try:
            p = doc.DrawPages.getByIndex(0) if first else add_page(doc)
            first = False
            p.Layout = n
        except Exception as e:
            print("  layout", n, repr(e)[:100])
    save(doc, "layouts.odp")
    done(doc)


def const(prefix, name):
    mod = "presentation" if prefix in ("EffectNodeType", "EffectPresetClass", "ShapeAnimationSubType", "EffectCommands") else "animations"
    return uno.getConstantByName("com.sun.star.%s.%s.%s" % (mod, prefix, name))


def transitions():
    doc = new("odp")
    first = True
    types = [uno.getConstantByName("com.sun.star.animations.TransitionType.BARWIPE") + i for i in range(43)]
    subs = list(range(0, 120))
    combos = [(t, 0) for t in types] + [(1, st) for st in subs if st]
    for i, (t, st) in enumerate(combos):
        p = doc.DrawPages.getByIndex(0) if first else add_page(doc)
        first = False
        setp(p, TransitionType=t, TransitionSubtype=st, TransitionDuration=1.5 + (i % 3), TransitionDirection=bool(i % 2),
             TransitionFadeColor=0x336699 + i)
    p = add_page(doc)
    setp(p, TransitionType=37, TransitionSubtype=const("TransitionSubType", "FADETOCOLOR"), TransitionDirection=False,
         TransitionFadeColor=0xFF8800)
    p = add_page(doc)
    setp(p, TransitionType=37, TransitionSubtype=const("TransitionSubType", "FADEFROMCOLOR"), TransitionFadeColor=0x0088FF)
    p = add_page(doc)
    setp(p, TransitionType=0, Sound=SOUND_URL, LoopSound=True)
    p = add_page(doc)
    setp(p, TransitionType=4, TransitionSubtype=3, Sound=SOUND_URL)
    p = add_page(doc)
    setp(p, TransitionType=0, Sound=True)
    p = add_page(doc)
    setp(p, TransitionType=36, Sound=False, Change=1, Duration=5)
    save(doc, "transitions.odp")
    done(doc)


def nv(name, value):
    return struct_("com.sun.star.beans.NamedValue", name, value)


def node(name, **props):
    n = spec(name)
    for k, v in props.items():
        setattr(n, k, v)
    return n


def userdata(**kw):
    return tuple(nv(k.replace("_", "-"), v) for k, v in kw.items())


def event(trigger, source=None, offset=None):
    e = struct_("com.sun.star.animations.Event")
    e.Trigger = trigger
    if source is not None:
        e.Source = source
    if offset is not None:
        e.Offset = offset
    return e


def animations_one(doc, p, k):
    sh = mk(doc, p, "RectangleShape", text="target one\ntwo\nthree")
    sh2 = mk(doc, p, "EllipseShape", x=6000, y=1000, text="second")
    sh3 = mk(doc, p, "RectangleShape", x=1000, y=5000, text="third")
    root = p.AnimationNode
    seq = node("SequenceTimeContainer", UserData=userdata(node_type=const("EffectNodeType", "MAIN_SEQUENCE")))
    root.appendChild(seq)
    EN = lambda n: const("EffectNodeType", n)

    def effect(nodetype, preset, cls, *children, **kw):
        par = node("ParallelTimeContainer", Begin=kw.pop("Begin", 0.0),
                   UserData=userdata(node_type=EN(nodetype), preset_id=preset, preset_class=const("EffectPresetClass", cls),
                                     preset_sub_type="", group_id=1))
        for c in children:
            par.appendChild(c)
        for a, b in kw.items():
            setattr(par, a, b)
        return par

    def click(*effects, trig=True):
        outer = node("ParallelTimeContainer", Begin=event(const("EventTrigger", "ON_NEXT")) if trig else 0.0)
        inner = node("ParallelTimeContainer", Begin=0.0)
        for e in effects:
            inner.appendChild(e)
        outer.appendChild(inner)
        seq.appendChild(outer)

    st = node("AnimateSet", Target=sh, AttributeName="Visibility", To=True, Begin=0.0, Duration=0.001, Fill=const("AnimationFill", "HOLD"))
    click(effect("ON_CLICK", "ooo-entrance-appear", "ENTRANCE", st))
    an = node("Animate", Target=sh2, AttributeName="X", Values=(0.0, 0.5), KeyTimes=(0.0, 1.0), Duration=2.0,
              Formula="$X+#x", CalcMode=const("AnimationCalcMode", "DISCRETE"), Accumulate=True,
              Additive=const("AnimationAdditiveMode", "SUM"), TimeFilter=(struct_("com.sun.star.animations.TimeFilterPair", 0.0, 0.0), struct_("com.sun.star.animations.TimeFilterPair", 1.0, 1.0)))
    an2 = node("Animate", Target=sh2, AttributeName="Opacity", From=0.0, By=0.1, To=1.0, Duration=1.0,
               CalcMode=const("AnimationCalcMode", "PACED"), Additive=const("AnimationAdditiveMode", "MULTIPLY"))
    an3 = node("Animate", Target=sh2, AttributeName="Y", From=0.0, To=0.5, Duration=1.0, Additive=const("AnimationAdditiveMode", "BASE"))
    an4 = node("Animate", Target=sh2, AttributeName="Height", From=0.0, To=0.5, Duration=1.0, Additive=const("AnimationAdditiveMode", "NONE"))
    click(effect("WITH_PREVIOUS", "ooo-entrance-fly-in", "ENTRANCE", an, an2, an3, an4))
    ac = node("AnimateColor", Target=sh, AttributeName="FillColor", From=0xFF0000, To=0x00FF00, Duration=1.0,
              ColorInterpolation=const("AnimationColorSpace", "HSL"), Direction=False)
    ac2 = node("AnimateColor", Target=sh3, AttributeName="LineColor", By=0x101010, Duration=1.0,
               ColorInterpolation=const("AnimationColorSpace", "RGB"), Direction=True)
    click(effect("AFTER_PREVIOUS", "ooo-emphasis-change-fill-color", "EMPHASIS", ac, ac2))
    am = node("AnimateMotion", Target=sh3, Path="M 0 0 L 0.25 0.25 C 0.3 0.3 0.4 0.3 0.5 0.5", Duration=2.0,
              CalcMode=const("AnimationCalcMode", "PACED"))
    click(effect("ON_CLICK", "ooo-motionpath-user", "MOTIONPATH", am))
    ats = [node("AnimateTransform", Target=sh2, TransformType=tt, Values=("1", "2"), Duration=1.0, SubItem=sub)
           for tt, sub in ((0, 0), (1, 1), (2, 2), (3, 0), (4, 1))]
    click(effect("ON_CLICK", "ooo-emphasis-spin", "EMPHASIS", *ats))
    attrs = [("Width", "0", "width"), ("Height", "0", "height"), ("Y", "0", "y"), ("Rotate", 0.0, 90.0), ("SkewX", 0.0, 30.0),
             ("CharColor", 0, 0xFF0000), ("CharRotation", 0, 90), ("CharWeight", 100.0, 150.0), ("CharUnderline", 0, 1),
             ("CharFontName", "Arial", "Times"), ("CharHeight", 1.0, 2.0), ("CharPosture", 0, 2),
             ("LineStyle", 0, 1), ("FillStyle", 0, 1), ("DimColor", 0, 0xAAAAAA), ("Visibility", False, True)]
    kids = []
    for an_name, fr, to in attrs:
        try:
            kids.append(node("Animate", Target=sh3, AttributeName=an_name, From=fr, To=to, Duration=0.5,
                             Restart=const("AnimationRestart", "ALWAYS"), RestartDefault=const("AnimationRestart", "NEVER"),
                             FillDefault=const("AnimationFill", "FREEZE"), Acceleration=0.2, Decelerate=0.3,
                             AutoReverse=True, RepeatCount=3.0, RepeatDuration=9.0, EndSync=const("AnimationEndSync", "FIRST"),
                             End=5.0))
        except Exception as e:
            print("  animate", an_name, repr(e)[:100])
    click(effect("ON_CLICK", "ooo-emphasis-custom", "CUSTOM", *kids))
    tf = node("TransitionFilter", Target=sh, Transition=1, Subtype=2, Mode=False, Direction=False, FadeColor=0x123456, Duration=1.0)
    tf2 = node("TransitionFilter", Target=sh2, Transition=37, Subtype=const("TransitionSubType", "FADETOCOLOR"), Mode=True,
               Direction=True, FadeColor=0x654321, Duration=1.0)
    click(effect("ON_CLICK", "ooo-entrance-wipe", "ENTRANCE", tf, tf2))
    au = node("Audio", Source=SOUND_URL, Volume=0.5, Begin=0.0)
    au2 = node("Audio", Source=SOUND_URL, Volume=1.0)
    cm = [node("Command", Target=sh, Command=c, Parameter=None) for c in range(6)]
    click(effect("ON_CLICK", "media-call", "MEDIACALL", au, au2, *cm))
    it = node("IterateContainer", Target=sh, SubItem=const("ShapeAnimationSubType", "ONLY_TEXT"),
              IterateType=1, IterateInterval=0.25,
              UserData=userdata(node_type=EN("ON_CLICK"), preset_id="ooo-entrance-fade-in", preset_class=const("EffectPresetClass", "ENTRANCE")))
    it.appendChild(node("AnimateSet", Target=sh, AttributeName="Visibility", To=True, Begin=0.0, Duration=0.001))
    click(it)
    for sub, itype in ((1, 0), (0, 2)):
        it2 = node("IterateContainer", Target=ParagraphTarget(sh, 1), SubItem=sub, IterateType=itype, IterateInterval=1.5)
        it2.appendChild(node("AnimateSet", Target=ParagraphTarget(sh, 1), AttributeName="Visibility", To=True))
        click(it2)
    seq2 = node("SequenceTimeContainer", UserData=userdata(node_type=EN("INTERACTIVE_SEQUENCE")),
                Begin=event(const("EventTrigger", "ON_CLICK"), sh3), End=(event(const("EventTrigger", "ON_END"), sh), 5.0))
    seq2.appendChild(node("AnimateSet", Target=sh2, AttributeName="Visibility", To=False))
    root.appendChild(seq2)
    for kind in range(12):
        trig = kind + 1
        e = event(trig, sh, 0.5 if kind % 2 else None)
        seq3 = node("SequenceTimeContainer", UserData=userdata(node_type=EN("INTERACTIVE_SEQUENCE")), Begin=e)
        seq3.appendChild(node("AnimateSet", Target=sh2, AttributeName="Visibility", To=True))
        root.appendChild(seq3)
    for fill, rest in ((1, 1), (2, 2), (3, 3), (4, 0), (5, 1)):
        par = node("ParallelTimeContainer", Fill=fill, Restart=rest, Begin=0.0)
        par.appendChild(node("AnimateSet", Target=sh, AttributeName="Visibility", To=True))
        root.appendChild(par)


def ParagraphTarget(shape, n):
    return struct_("com.sun.star.presentation.ParagraphTarget", shape, n)


def animations():
    doc = new("odp")
    p = doc.DrawPages.getByIndex(0)
    step("anim-tree", animations_one, doc, p, 0)
    save(doc, "animations.odp")
    done(doc)


def enum(t, v):
    return uno.Enum("com.sun.star.presentation." + t, v)


ANIM_EFFECTS = """FADE_FROM_LEFT FADE_FROM_TOP FADE_FROM_RIGHT FADE_FROM_BOTTOM FADE_TO_CENTER FADE_FROM_CENTER MOVE_FROM_LEFT MOVE_FROM_TOP
MOVE_FROM_RIGHT MOVE_FROM_BOTTOM VERTICAL_STRIPES HORIZONTAL_STRIPES CLOCKWISE COUNTERCLOCKWISE FADE_FROM_UPPERLEFT FADE_FROM_UPPERRIGHT
FADE_FROM_LOWERLEFT FADE_FROM_LOWERRIGHT CLOSE_VERTICAL CLOSE_HORIZONTAL OPEN_VERTICAL OPEN_HORIZONTAL PATH MOVE_TO_LEFT MOVE_TO_TOP
MOVE_TO_RIGHT MOVE_TO_BOTTOM SPIRALIN_LEFT SPIRALIN_RIGHT SPIRALOUT_LEFT SPIRALOUT_RIGHT DISSOLVE WAVYLINE_FROM_LEFT WAVYLINE_FROM_TOP
WAVYLINE_FROM_RIGHT WAVYLINE_FROM_BOTTOM RANDOM VERTICAL_LINES HORIZONTAL_LINES LASER_FROM_LEFT LASER_FROM_TOP LASER_FROM_RIGHT
LASER_FROM_BOTTOM LASER_FROM_UPPERLEFT LASER_FROM_UPPERRIGHT LASER_FROM_LOWERLEFT LASER_FROM_LOWERRIGHT APPEAR HIDE MOVE_FROM_UPPERLEFT
MOVE_FROM_UPPERRIGHT MOVE_FROM_LOWERRIGHT MOVE_FROM_LOWERLEFT MOVE_TO_UPPERLEFT MOVE_TO_UPPERRIGHT MOVE_TO_LOWERRIGHT MOVE_TO_LOWERLEFT
MOVE_SHORT_FROM_LEFT MOVE_SHORT_FROM_UPPERLEFT MOVE_SHORT_FROM_TOP MOVE_SHORT_FROM_UPPERRIGHT MOVE_SHORT_FROM_RIGHT
MOVE_SHORT_FROM_LOWERRIGHT MOVE_SHORT_FROM_BOTTOM MOVE_SHORT_FROM_LOWERLEFT MOVE_SHORT_TO_LEFT MOVE_SHORT_TO_UPPERLEFT MOVE_SHORT_TO_TOP
MOVE_SHORT_TO_UPPERRIGHT MOVE_SHORT_TO_RIGHT MOVE_SHORT_TO_LOWERRIGHT MOVE_SHORT_TO_BOTTOM MOVE_SHORT_TO_LOWERLEFT VERTICAL_CHECKERBOARD
HORIZONTAL_CHECKERBOARD HORIZONTAL_ROTATE VERTICAL_ROTATE HORIZONTAL_STRETCH VERTICAL_STRETCH STRETCH_FROM_LEFT STRETCH_FROM_UPPERLEFT
STRETCH_FROM_TOP STRETCH_FROM_UPPERRIGHT STRETCH_FROM_RIGHT STRETCH_FROM_LOWERRIGHT STRETCH_FROM_BOTTOM STRETCH_FROM_LOWERLEFT ZOOM_IN
ZOOM_IN_SMALL ZOOM_IN_SPIRAL ZOOM_OUT ZOOM_OUT_SMALL ZOOM_OUT_SPIRAL ZOOM_IN_FROM_LEFT ZOOM_IN_FROM_UPPERLEFT ZOOM_IN_FROM_TOP
ZOOM_IN_FROM_UPPERRIGHT ZOOM_IN_FROM_RIGHT ZOOM_IN_FROM_LOWERRIGHT ZOOM_IN_FROM_BOTTOM ZOOM_IN_FROM_LOWERLEFT ZOOM_IN_FROM_CENTER
ZOOM_OUT_FROM_LEFT ZOOM_OUT_FROM_UPPERLEFT ZOOM_OUT_FROM_TOP ZOOM_OUT_FROM_UPPERRIGHT ZOOM_OUT_FROM_RIGHT ZOOM_OUT_FROM_LOWERRIGHT
ZOOM_OUT_FROM_BOTTOM ZOOM_OUT_FROM_LOWERLEFT ZOOM_OUT_FROM_CENTER""".split()
CLICK_ACTIONS = "NONE PREVPAGE NEXTPAGE FIRSTPAGE LASTPAGE BOOKMARK DOCUMENT INVISIBLE SOUND VERB VANISH PROGRAM MACRO STOPPRESENTATION".split()


def png(path, w=8, h=8):
    import zlib
    raw = b"".join(b"\x00" + b"".join(bytes((x * 30 % 256, y * 30 % 256, 128)) for x in range(w)) for y in range(h))
    def chunk(t, d):
        c = struct.pack(">I", len(d)) + t + d
        return c + struct.pack(">I", zlib.crc32(t + d) & 0xffffffff)
    data = b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", w, h, 8, 2, 0, 0, 0)) + chunk(b"IDAT", zlib.compress(raw)) + chunk(b"IEND", b"")
    open(path, "wb").write(data)


PNG = os.path.join(tmp, "pix.png")
png(PNG)
PNG_URL = uno.systemPathToFileUrl(PNG)


def actions():
    doc = new("odp")
    page = doc.DrawPages.getByIndex(0)
    pg2 = add_page(doc)
    pg2.Name = "Target"
    x = 500
    y = 500
    for i, a in enumerate(CLICK_ACTIONS):
        sh = mk(doc, page, "RectangleShape", x=x, y=y, w=2000, h=1000, text=a)
        x += 2200
        if x > 22000:
            x = 500; y += 1200
        props = dict(OnClick=enum("ClickAction", a))
        if a in ("BOOKMARK",):
            props["Bookmark"] = "Target"
        if a in ("DOCUMENT",):
            props["Bookmark"] = "file:///tmp/other.odp#Slide1"
        if a == "PROGRAM":
            props["Bookmark"] = "file:///usr/bin/true"
        if a == "SOUND":
            props.update(Bookmark=SOUND_URL, PlayFull=True)
        if a == "VERB":
            props["Verb"] = 1
        if a == "MACRO":
            props["Bookmark"] = "macro:///Standard.Module1.Main"
        setp(sh, **props)
    for i, eff in enumerate(ANIM_EFFECTS):
        sh = mk(doc, page, "RectangleShape", x=500 + (i % 10) * 2200, y=6000 + (i // 10) * 1100, w=2000, h=900, text=eff[:14])
        setp(sh, OnClick=enum("ClickAction", "VANISH"), Effect=enum("AnimationEffect", eff),
             Speed=enum("AnimationSpeed", ("SLOW", "MEDIUM", "FAST")[i % 3]), SoundOn=bool(i % 2), Sound=SOUND_URL, PlayFull=bool(i % 3 == 0),
             DimPrevious=False, DimHide=False)
    save(doc, "click-actions.odp")
    done(doc)


def draw_links():
    doc = new("odg")
    pages = [doc.DrawPages.getByIndex(0), add_page(doc)]
    pages[1].Name = "Second"
    for i, a in enumerate(("BOOKMARK", "DOCUMENT")):
        sh = mk(doc, pages[0], "RectangleShape", x=500 + i * 3500, text=a)
        setp(sh, OnClick=enum("ClickAction", a), Bookmark="Second" if i == 0 else "file:///tmp/other.odg#Page1")
    save(doc, "draw-links.odg")
    done(doc)


def image_maps():
    doc = new("odg")
    page = doc.DrawPages.getByIndex(0)
    g = mk(doc, page, "GraphicObjectShape", x=1000, y=1000, w=8000, h=6000)
    g.GraphicURL = PNG_URL
    imap = g.ImageMap
    r = doc.createInstance("com.sun.star.image.ImageMapRectangleObject")
    r.Boundary = struct_("com.sun.star.awt.Rectangle", 0, 0, 3000, 2000)
    r.URL = "http://example.org/rect"
    r.Target = "_blank"
    r.Description = "rect area"
    r.IsActive = True
    r.Name = "r1"
    imap.insertByIndex(0, r)
    c = doc.createInstance("com.sun.star.image.ImageMapCircleObject")
    c.Center = pt(5000, 3000)
    c.Radius = 1000
    c.URL = "http://example.org/circle"
    c.Title = "circle title"
    c.IsActive = False
    imap.insertByIndex(1, c)
    po = doc.createInstance("com.sun.star.image.ImageMapPolygonObject")
    po.Polygon = (pt(0, 0), pt(100, 0), pt(100, 100), pt(0, 100))
    po.URL = "#target"
    po.Description = "poly area"
    imap.insertByIndex(2, po)
    g.ImageMap = imap
    gr2 = mk(doc, page, "GraphicObjectShape", x=10000, y=1000, w=3000, h=3000)
    gr2.GraphicURL = PNG_URL
    setp(gr2, Title="a picture", Description="a described picture")
    save(doc, "image-map.odg")
    done(doc)


def pts(*xy):
    return tuple(pt(x, y) for x, y in xy)


def poly(doc, page, service, points, **props):
    s_ = doc.createInstance("com.sun.star.drawing." + service)
    page.add(s_)
    s_.PolyPolygon = (pts(*points),)
    setp(s_, **props)
    return s_


def bezier(doc, page, service, **props):
    s_ = doc.createInstance("com.sun.star.drawing." + service)
    page.add(s_)
    C = uno.Enum("com.sun.star.drawing.PolygonFlags", "NORMAL")
    K = uno.Enum("com.sun.star.drawing.PolygonFlags", "CONTROL")
    coords = struct_("com.sun.star.drawing.PolyPolygonBezierCoords")
    coords.Coordinates = (pts((1000, 1000), (2000, 0), (3000, 2000), (4000, 1000), (5000, 3000), (1000, 3000)),)
    coords.Flags = ((C, K, K, C, C, C),)
    s_.PolyPolygonBezier = coords
    setp(s_, **props)
    return s_


def shapes_core(doc, page):
    mk(doc, page, "RectangleShape", x=500, y=500, text="plain", Name="rect1")
    mk(doc, page, "RectangleShape", x=4000, y=500, text="round", CornerRadius=400, RotateAngle=3000, ShearAngle=500, Name="rect2")
    mk(doc, page, "RectangleShape", x=8000, y=500, text="rot", RotateAngle=9000)
    for i, kind in enumerate(("FULL", "CUT", "SECTION", "ARC")):
        ck = uno.Enum("com.sun.star.drawing.CircleKind", kind)
        mk(doc, page, "EllipseShape", x=500 + i * 2600, y=3500, w=2400, h=2400, CircleKind=ck,
           CircleStartAngle=3000, CircleEndAngle=24000, Name="circle-" + kind)
        mk(doc, page, "EllipseShape", x=500 + i * 3200, y=6500, w=3000, h=2000, CircleKind=ck,
           CircleStartAngle=3000, CircleEndAngle=24000, Name="ellipse-" + kind)
        mk(doc, page, "EllipseShape", x=500 + i * 3200, y=9000, w=2400, h=2400, CircleKind=ck, RotateAngle=4500,
           CircleStartAngle=3000, CircleEndAngle=24000)
    mk(doc, page, "EllipseShape", x=22000, y=9000, w=3000, h=2000, RotateAngle=2500, Name="ellipse-rot")
    poly(doc, page, "LineShape", ((500, 12000), (4000, 13000)), Name="line1")
    poly(doc, page, "LineShape", ((4500, 12000), (8000, 13000)), RotateAngle=2000)
    poly(doc, page, "PolyLineShape", ((9000, 12000), (10000, 14000), (11000, 12000), (12000, 14000)), Name="polyline1")
    poly(doc, page, "PolyLineShape", ((13000, 12000), (14000, 14000), (15000, 12000)), RotateAngle=3000)
    poly(doc, page, "PolyPolygonShape", ((16000, 12000), (18000, 12000), (17000, 14000)), Name="polygon1")
    poly(doc, page, "PolyPolygonShape", ((19000, 12000), (21000, 12000), (20000, 14000)), RotateAngle=1500)
    bezier(doc, page, "ClosedBezierShape", Name="path-closed")
    bezier(doc, page, "OpenBezierShape", RotateAngle=1000)
    bezier(doc, page, "ClosedFreeHandShape")
    bezier(doc, page, "OpenFreeHandShape")
    poly(doc, page, "PolyLinePathShape", ((1000, 16000), (2000, 17000), (3000, 16000)))
    poly(doc, page, "PolyPolygonPathShape", ((4000, 16000), (6000, 16000), (5000, 17500)))
    mk(doc, page, "TextShape", x=7000, y=16000, w=4000, h=1000, text="text box", TextAutoGrowHeight=True,
       TextMinimumFrameWidth=2000, TextMaximumFrameWidth=6000, TextMinimumFrameHeight=500, TextMaximumFrameHeight=3000, Name="text1")
    t = mk(doc, page, "TextShape", x=12000, y=16000, w=4000, h=1000, text="titled")
    setp(t, Title="text title", Description="text description")
    g1 = mk(doc, page, "RectangleShape", x=500, y=18000, text="g1")
    g2 = mk(doc, page, "EllipseShape", x=4000, y=18000, text="g2")
    grp = doc.createInstance("com.sun.star.drawing.GroupShape")
    page.add(grp)
    grp.add(g1)
    grp.add(g2)
    setp(grp, Name="group1", Title="group title", Description="group desc")
    inner = doc.createInstance("com.sun.star.drawing.GroupShape")
    page.add(inner)
    inner.add(mk(doc, page, "RectangleShape", x=8000, y=18000, text="n1"))
    inner.add(mk(doc, page, "RectangleShape", x=11000, y=18000, text="n2"))
    outer = doc.createInstance("com.sun.star.drawing.GroupShape")
    page.add(outer)
    outer.add(inner)
    outer.add(mk(doc, page, "RectangleShape", x=14000, y=18000, text="n3"))
    setp(outer, Name="group2")


def connectors(doc, page):
    r = [mk(doc, page, "RectangleShape", x=1000 + (i % 3) * 6000, y=1000 + (i // 3) * 5000, w=2500, h=1500, text="n%d" % i, Name="node%d" % i)
         for i in range(9)]
    for i in range(4):
        gp = struct_("com.sun.star.drawing.GluePoint2")
        gp.Position = pt(500 * i, 200 * i)
        gp.IsRelative = bool(i % 2)
        gp.PositionAlignment = uno.Enum("com.sun.star.drawing.Alignment", ("TOP_LEFT", "CENTER", "BOTTOM_RIGHT", "LEFT")[i])
        gp.Escape = uno.Enum("com.sun.star.drawing.EscapeDirection", ("SMART", "LEFT", "UP", "DOWN")[i])
        gp.IsUserDefined = True
        r[0].GluePoints.insert(gp)
    escs = ("SMART", "LEFT", "RIGHT", "UP", "DOWN", "HORIZONTAL", "VERTICAL")
    for i, esc in enumerate(escs):
        gp = struct_("com.sun.star.drawing.GluePoint2")
        gp.Position = pt(100 * i, 100 * i)
        gp.IsRelative = False
        gp.PositionAlignment = uno.Enum("com.sun.star.drawing.Alignment", ("TOP_LEFT", "TOP", "TOP_RIGHT", "LEFT", "CENTER", "RIGHT", "BOTTOM_LEFT")[i])
        gp.Escape = uno.Enum("com.sun.star.drawing.EscapeDirection", esc)
        gp.IsUserDefined = True
        r[1].GluePoints.insert(gp)
    kinds = ("STANDARD", "CURVE", "LINE", "LINES")
    n = 0
    for k in kinds:
        c = doc.createInstance("com.sun.star.drawing.ConnectorShape")
        page.add(c)
        c.EdgeKind = uno.Enum("com.sun.star.drawing.ConnectorType", k)
        c.StartShape = r[n % 9]
        c.EndShape = r[(n + 4) % 9]
        c.StartGluePointIndex = 0 if n % 2 else 4
        c.EndGluePointIndex = 5 + n
        setp(c, Title="conn " + k, Description="connector " + k, EdgeLine1Delta=300, EdgeLine2Delta=-300, EdgeNode1HorzDist=100, EdgeNode2VertDist=200)
        n += 1
        c2 = doc.createInstance("com.sun.star.drawing.ConnectorShape")
        page.add(c2)
        c2.EdgeKind = uno.Enum("com.sun.star.drawing.ConnectorType", k)
        c2.StartPosition = pt(1000, 12000 + n * 700)
        c2.EndPosition = pt(9000, 13000 + n * 900)
        c2.String = "free " + k


def measure_caption(doc, page):
    for i, (a, b) in enumerate((((1000, 1000), (9000, 1000)), ((1000, 3000), (9000, 6000)), ((11000, 1000), (11000, 8000)))):
        m = doc.createInstance("com.sun.star.drawing.MeasureShape")
        page.add(m)
        m.StartPosition = pt(*a)
        m.EndPosition = pt(*b)
        setp(m, Name="measure%d" % i, MeasureTextHorizontalPosition=i % 4, MeasureTextVerticalPosition=i % 4,
             MeasureLineDistance=500, MeasureHelpLine1Length=200, MeasureHelpLine2Length=200, MeasureOverhang=100,
             MeasureUnit=i, MeasureShowUnit=True, MeasureBelowReferenceEdge=bool(i % 2), MeasureTextRotate90=bool(i % 2),
             MeasureTextIsFixedAngle=True, MeasureTextFixedAngle=1500 * i, MeasureDecimalPlaces=2,
             MeasureTextAutoAngle=False, MeasureTextAutoAngleView=i, Title="m", Description="measure")
        m.String = "measure %d" % i
    for i in range(3):
        c = doc.createInstance("com.sun.star.drawing.CaptionShape")
        page.add(c)
        c.Position = pt(1000 + i * 7000, 10000)
        c.Size = sz(4000, 2000)
        c.String = "caption %d" % i
        setp(c, CaptionPoint=pt(500 + i * 7000, 14000), CaptionType=i, CaptionIsFitLineLength=bool(i % 2), CaptionIsEscapeRelative=bool(i % 2),
             CaptionEscapeDirection=i, CaptionEscapeRelative=500, CaptionEscapeAbsolute=300, CaptionLineLength=700,
             CaptionGap=100, CaptionAngle=300 * i, CornerRadius=300 * i, Name="caption%d" % i)


def layers_doc():
    doc = new("odg")
    page = doc.DrawPages.getByIndex(0)
    lm = doc.LayerManager
    for i, nm in enumerate(("layerA", "layerB")):
        ly = lm.insertNewByIndex(lm.Count)
        ly.Name = nm
        setp(ly, IsVisible=bool(i % 2 == 0), IsPrintable=bool(i == 0), IsLocked=bool(i == 1), Title="t" + nm, Description="d" + nm)
        sh = mk(doc, page, "RectangleShape", x=500 + 3000 * i, text=nm)
        sh.LayerName = nm
    for k, v in enumerate(("a", "b")):
        sh = mk(doc, page, "EllipseShape", x=500 + 3000 * k, y=4000, text="nav%d" % k)
    try:
        order = smgr.createInstance("com.sun.star.drawing.ShapeCollection")
        for i in reversed(range(page.Count)):
            order.add(page.getByIndex(i))
        page.NavigationOrder = order
    except Exception as e:
        print("  navorder", repr(e)[:150])
    save(doc, "layers.odg")
    done(doc)


def shapes_docs():
    for kind in ("odg", "odp"):
        for label, fn in (("core", shapes_core), ("connectors", connectors), ("measure", measure_caption)):
            doc = new(kind)
            page = doc.DrawPages.getByIndex(0)
            step("%s-%s" % (label, kind), fn, doc, page)
            save(doc, "%s.%s" % (label, kind))
            done(doc)


def p3(x, y, z):
    return struct_("com.sun.star.drawing.Position3D", x, y, z)


def d3(x, y, z):
    return struct_("com.sun.star.drawing.Direction3D", x, y, z)


def scene_3d(doc, page, x, y, nm, mode="SMOOTH", proj="PERSPECTIVE", extra=None):
    sc = doc.createInstance("com.sun.star.drawing.Shape3DSceneObject")
    page.add(sc)
    sc.Position = pt(x, y)
    sc.Size = sz(5000, 5000)
    cube = doc.createInstance("com.sun.star.drawing.Shape3DCubeObject")
    sc.add(cube)
    setp(cube, D3DPosition=p3(-1000, -1000, -1000), D3DSize=d3(2000, 3000, 2500))
    sph = doc.createInstance("com.sun.star.drawing.Shape3DSphereObject")
    sc.add(sph)
    setp(sph, D3DPosition=p3(500, 500, 0), D3DSize=d3(1500, 1500, 1500), D3DHorizontalSegments=12, D3DVerticalSegments=10)
    ex = doc.createInstance("com.sun.star.drawing.Shape3DExtrudeObject")
    sc.add(ex)
    poly3 = struct_("com.sun.star.drawing.PolyPolygonShape3D")
    poly3.SequenceX = ((0.0, 1000.0, 1000.0, 0.0),)
    poly3.SequenceY = ((0.0, 0.0, 1000.0, 1000.0),)
    poly3.SequenceZ = ((0.0, 0.0, 0.0, 0.0),)
    setp(ex, D3DPolyPolygon3D=poly3, D3DDepth=700, D3DPercentDiagonal=5, D3DDoubleSided=True,
         D3DCloseFront=True, D3DCloseBack=False, D3DNormalsKind=uno.Enum("com.sun.star.drawing.NormalsKind", "SPECIFIC"),
         D3DNormalsInvert=True, D3DTextureProjectionX=uno.Enum("com.sun.star.drawing.TextureProjectionMode", "SPHERE"),
         D3DTextureProjectionY=uno.Enum("com.sun.star.drawing.TextureProjectionMode", "OBJECTSPECIFIC"),
         D3DTextureKind=uno.Enum("com.sun.star.drawing.TextureKind", "COLOR"),
         D3DTextureMode=uno.Enum("com.sun.star.drawing.TextureMode", "BLEND"), D3DTextureFilter=True,
         D3DMaterialColor=0x336699, D3DMaterialEmission=0x101010, D3DMaterialSpecular=0xAAAAAA, D3DMaterialSpecularIntensity=40,
         D3DShadow3D=True)
    la = doc.createInstance("com.sun.star.drawing.Shape3DLatheObject")
    sc.add(la)
    poly4 = struct_("com.sun.star.drawing.PolyPolygonShape3D")
    poly4.SequenceX = ((500.0, 1500.0, 1000.0),)
    poly4.SequenceY = ((0.0, 0.0, 1500.0),)
    poly4.SequenceZ = ((0.0, 0.0, 0.0),)
    setp(la, D3DPolyPolygon3D=poly4, D3DHorizontalSegments=16, D3DEndAngle=2700, D3DPercentDiagonal=10)
    cg = struct_("com.sun.star.drawing.CameraGeometry")
    cg.vrp = p3(0.0, 0.0, 6000.0)
    cg.vpn = d3(1.0, 1.0, 5.0)
    cg.vup = d3(0.0, 1.0, 0.2)
    setp(sc, Name=nm, D3DCameraGeometry=cg, D3DScenePerspective=uno.Enum("com.sun.star.drawing.ProjectionMode", proj),
         D3DSceneShadeMode=uno.Enum("com.sun.star.drawing.ShadeMode", mode), D3DSceneDistance=3000, D3DSceneFocalLength=40,
         D3DSceneShadowSlant=15, D3DSceneAmbientColor=0x202040, D3DSceneTwoSidedLighting=True,
         D3DSceneLightOn1=True, D3DSceneLightOn2=True, D3DSceneLightOn3=False, D3DSceneLightColor1=0xFFFFFF,
         D3DSceneLightColor2=0xFF0000, D3DSceneLightColor5=0x00FF00, D3DSceneLightOn5=True,
         D3DSceneLightDirection1=d3(1.0, 1.0, 1.0), D3DSceneLightDirection2=d3(-1.0, 0.0, 1.0), RotateAngle=1000 if extra else 0,
         Title="3d scene", Description="a scene")
    if extra:
        setp(sc, **extra)
    return sc


def threed(kind):
    doc = new(kind)
    page = doc.DrawPages.getByIndex(0)
    n = 0
    for mode in ("FLAT", "PHONG", "SMOOTH", "DRAFT"):
        for proj in ("PERSPECTIVE", "PARALLEL"):
            step("scene-%s-%s" % (mode, proj), scene_3d, doc, page, 500 + (n % 4) * 5500, 500 + (n // 4) * 5500, "scene%d" % n, mode, proj)
            n += 1
    save(doc, "scenes.%s" % kind)
    done(doc)


def pv_(name, value):
    return pv(name, value)


def param(value, kind=0):
    p_ = struct_("com.sun.star.drawing.EnhancedCustomShapeParameter")
    p_.Value = value
    p_.Type = kind
    return p_


def pair(a, b, ka=0, kb=0):
    pp = struct_("com.sun.star.drawing.EnhancedCustomShapeParameterPair")
    pp.First = param(a, ka)
    pp.Second = param(b, kb)
    return pp


def custom(doc, page, x, y, w, h, geometry, text=None, **props):
    c = doc.createInstance("com.sun.star.drawing.CustomShape")
    page.add(c)
    c.Position = pt(x, y)
    c.Size = sz(w, h)
    if text:
        c.String = text
    c.CustomShapeGeometry = tuple(pv(k, v) for k, v in geometry)
    setp(c, **props)
    return c


PRESETS = """rectangle round-rectangle ellipse diamond isosceles-triangle right-triangle parallelogram trapezoid octagon hexagon pentagon cross
ring heart sun moon smiley-face lightning flower-1 cloud-callout star4 star5 star6 star8 star12 star24 right-arrow left-arrow up-arrow
down-arrow left-right-arrow quad-arrow bent-arrow u-turn-arrow circular-arrow notched-right-arrow striped-right-arrow pentagon-right
chevron flowchart-process flowchart-decision flowchart-terminator flowchart-document flowchart-data flowchart-connector
flowchart-card flowchart-delay flowchart-sort mso-spt100 mso-spt21 round-callout rectangular-callout line-callout-1 bracket-pair
brace-pair left-bracket right-brace wave double-wave horizontal-scroll vertical-scroll cube can bevel frame folded-corner
block-arc forbidden down-ribbon up-ribbon quad-bevel octagon-bevel diamond-bevel left-right-up-arrow
ellipse-ribbon ellipse-ribbon-2 irregular-seal1 irregular-seal2 doorplate
fontwork-plain-text fontwork-slant-up fontwork-wave fontwork-fade-up-and-right fontwork-arch-up-curve fontwork-circle-curve
fontwork-open-circle-curve fontwork-arch-up-pour fontwork-circle-pour fontwork-triangle-up fontwork-stop fontwork-inflate""".split()


def custom_shapes():
    doc = new("odg")
    page = doc.DrawPages.getByIndex(0)
    for i, name in enumerate(PRESETS):
        step("preset " + name, custom, doc, page, 500 + (i % 8) * 2700, 500 + (i // 8) * 2300, 2400, 2000, [("Type", name)], text=name[:8])
    save(doc, "custom-presets.odg")
    done(doc)


def custom_full():
    doc = new("odg")
    page = doc.DrawPages.getByIndex(0)
    PV = "[]com.sun.star.beans.PropertyValue"
    ECS = "com.sun.star.drawing.EnhancedCustomShape"
    pairs = lambda seq: uno.Any("[]" + ECS + "ParameterPair", tuple(seq))
    shade = lambda n: uno.Enum("com.sun.star.drawing.ShadeMode", n)
    proj = lambda n: uno.Enum("com.sun.star.drawing.ProjectionMode", n)
    for i, (sm, pm) in enumerate((("FLAT", "PARALLEL"), ("PHONG", "PERSPECTIVE"), ("SMOOTH", "PARALLEL"), ("DRAFT", "PERSPECTIVE"))):
        ext = uno.Any(PV, (pv("Extrusion", True), pv("Brightness", 33.0), pv("Depth", pair(900.0, 0.5)), pv("Diffusion", 20.0),
               pv("NumberOfLineSegments", 12), pv("LightFace", bool(i % 2)), pv("FirstLightHarsh", True), pv("SecondLightHarsh", False),
               pv("FirstLightLevel", 60.0), pv("SecondLightLevel", 40.0), pv("FirstLightDirection", d3(10.0, 20.0, 30.0)),
               pv("SecondLightDirection", d3(-1.0, 2.0, 3.0)), pv("Metal", bool(i % 2)), pv("ShadeMode", shade(sm)),
               pv("RotateAngle", pair(10.0, 20.0)), pv("RotationCenter", d3(0.1, 0.2, 0.3)), pv("Shininess", 50.0),
               pv("Skew", pair(50.0, 135.0)), pv("Specularity", 70.0), pv("ProjectionMode", proj(pm)),
               pv("ViewPoint", p3(3500.0, -3500.0, 25000.0)), pv("Origin", pair(0.5, -0.5)), pv("Color", True)))
        tp = uno.Any(PV, (pv("TextPath", True), pv("TextPathMode", uno.Enum("com.sun.star.drawing.EnhancedCustomShapeTextPathMode", ("NORMAL", "PATH", "SHAPE", "NORMAL")[i])),
              pv("ScaleX", bool(i % 2)), pv("SameLetterHeights", bool(i // 2))))
        path = uno.Any(PV, (pv("ExtrusionAllowed", True), pv("ConcentricGradientFillAllowed", True), pv("TextPathAllowed", True),
                pv("GluePointType", i % 3), pv("StretchX", 100 * i), pv("StretchY", 50 * i),
                pv("GluePoints", pairs((pair(0, 0), pair(10800, 0), pair(21600, 21600)))),
                pv("Coordinates", pairs((pair(0, 0), pair(21600, 0), pair(21600, 21600), pair(0, 21600)))),
                pv("Segments", uno.Any("[]" + ECS + "Segment", (struct_(ECS + "Segment", 1, 1), struct_(ECS + "Segment", 2, 3), struct_(ECS + "Segment", 4, 1)))),
                pv("TextFrames", uno.Any("[]" + ECS + "TextFrame", (struct_(ECS + "TextFrame", pair(0, 0), pair(21600, 21600)),)))))
        h = uno.Any(PV, (pv("Position", pair(10800, 0)), pv("MirroredX", bool(i % 2)), pv("MirroredY", bool(i // 2)), pv("Switched", bool(i % 2)),
             pv("RangeXMinimum", param(0)), pv("RangeXMaximum", param(21600)), pv("RangeYMinimum", param(0)), pv("RangeYMaximum", param(21600))))
        hp = uno.Any(PV, (pv("Position", pair(10800, 10800)), pv("Polar", pair(10800, 10800)), pv("RadiusRangeMinimum", param(0)),
              pv("RadiusRangeMaximum", param(10800))))
        adj = struct_(ECS + "AdjustmentValue")
        adj.Value = 5400
        adj.State = uno.Enum("com.sun.star.beans.PropertyState", "DIRECT_VALUE")
        geo = [("Type", "non-primitive"), ("MirroredX", bool(i % 2)), ("MirroredY", bool(i // 2)),
               ("ViewBox", struct_("com.sun.star.awt.Rectangle", 0, 0, 21600, 21600)), ("TextRotateAngle", 15.0 * i),
               ("Extrusion", ext), ("TextPath", tp), ("Path", path), ("Equations", uno.Any("[]string", ("x*2", "?0 -3", "$0 +1"))),
               ("Handles", uno.Any("[][]com.sun.star.beans.PropertyValue", (h.value, hp.value))),
               ("AdjustmentValues", uno.Any("[]" + ECS + "AdjustmentValue", (adj,)))]
        step("custom-full-%d" % i, custom, doc, page, 500 + (i % 2) * 8000, 500 + (i // 2) * 7000, 7000, 6000, geo, text="fw%d" % i,
             CustomShapeEngine="com.sun.star.drawing.EnhancedCustomShapeEngine", CustomShapeData="data%d" % i)
    save(doc, "custom-full.odg")
    done(doc)


def fontwork():
    doc = new("odg")
    page = doc.DrawPages.getByIndex(0)
    for i, name in enumerate(("fontwork-plain-text", "fontwork-wave", "fontwork-slant-up", "fontwork-arch-up-curve", "fontwork-circle-pour")):
        geo = [("Type", name), ("TextPath", uno.Any("[]com.sun.star.beans.PropertyValue", (pv("TextPath", True), pv("TextPathMode", uno.Enum("com.sun.star.drawing.EnhancedCustomShapeTextPathMode", "SHAPE")),
                                              pv("ScaleX", False), pv("SameLetterHeights", bool(i % 2))))),
               ("Path", uno.Any("[]com.sun.star.beans.PropertyValue", (pv("TextPathAllowed", True), pv("ExtrusionAllowed", True)))),
               ("Extrusion", uno.Any("[]com.sun.star.beans.PropertyValue", (pv("Extrusion", bool(i % 2)),)))]
        c = step("fw-" + name, custom, doc, page, 500, 500 + i * 3200, 12000, 3000, geo, text="Fontwork " + name[9:])
        if c is not None:
            setp(c, TextAutoGrowHeight=False)
    save(doc, "fontwork.odg")
    done(doc)


def cmds(**kw):
    return uno.Any("[]com.sun.star.beans.PropertyValue", tuple(pv(k, v) for k, v in kw.items()))


def special_shapes(doc, page, impress):
    step("plugin", lambda: mk(doc, page, "PluginShape", x=500, y=500, w=4000, h=3000, PluginURL="file:///tmp/clip.mov",
                              PluginMimeType="video/quicktime", PluginCommands=cmds(autoplay="true", loop="false")))
    step("applet", lambda: mk(doc, page, "AppletShape", x=5000, y=500, w=4000, h=3000, AppletCodeBase="file:///tmp/",
                              AppletName="myapplet", AppletCode="Demo.class", AppletIsScript=True,
                              AppletCommands=cmds(speed="5", color="red")))
    step("frame", lambda: mk(doc, page, "FrameShape", x=10000, y=500, w=4000, h=3000, FrameURL="http://example.org/",
                             FrameName="myframe", FrameIsAutoScroll=True, FrameIsBorder=True, FrameMarginWidth=100, FrameMarginHeight=200))
    step("frame2", lambda: mk(doc, page, "FrameShape", x=15000, y=500, w=4000, h=3000, FrameURL="file:///tmp/x.html", FrameIsAutoScroll=False,
                              FrameIsBorder=False))

    def media():
        m = mk(doc, page, "MediaShape", x=500, y=4000, w=4000, h=3000)
        setp(m, MediaURL=SOUND_URL, Loop=True, Mute=True, VolumeDB=-6,
             Zoom=uno.Enum("com.sun.star.media.ZoomLevel", "FIT_TO_WINDOW"))
        for i, z in enumerate(("ZOOM_1_TO_4", "ZOOM_1_TO_2", "ORIGINAL", "ZOOM_2_TO_1", "ZOOM_4_TO_1", "FIT_TO_WINDOW_FIXED_ASPECT", "FULLSCREEN")):
            m2 = mk(doc, page, "MediaShape", x=5000 + i * 2000, y=4000, w=1800, h=1500)
            setp(m2, MediaURL=SOUND_URL, Zoom=uno.Enum("com.sun.star.media.ZoomLevel", z))
    step("media", media)
    step("page-thumb", lambda: mk(doc, page, "PageShape", x=500, y=8000, w=4000, h=3000, PageNumber=1, Name="thumb"))
    step("page-thumb-rot", lambda: mk(doc, page, "PageShape", x=5000, y=8000, w=4000, h=3000, PageNumber=1, RotateAngle=2000,
                                      Title="thumb title", Description="thumb desc"))
    for i, (clsid, nm) in enumerate((("078B7ABA-54FC-457F-8551-6147E776A997", "math"), ("12DCAE26-281F-416F-A234-C3086127382E", "chart"),
                                     ("47BBB4CB-CE4C-4E80-A591-42D9AE74950F", "calc"))):
        def ole(clsid=clsid, nm=nm, i=i):
            o = doc.createInstance("com.sun.star.drawing.OLE2Shape")
            page.add(o)
            o.CLSID = clsid
            o.Position = pt(500 + i * 5000, 12000)
            o.Size = sz(4000, 3000)
            o.Name = "ole-" + nm
            o.Title = "ole"
            o.Description = nm
        step("ole-" + nm, ole)

    def control():
        form = doc.createInstance("com.sun.star.form.component.Form")
        page.Forms.insertByIndex(0, form)
        for i, svc in enumerate(("CommandButton", "TextField", "CheckBox")):
            model = doc.createInstance("com.sun.star.form.component." + svc)
            model.Name = "ctl%d" % i
            form.insertByIndex(i, model)
            cs = doc.createInstance("com.sun.star.drawing.ControlShape")
            page.add(cs)
            cs.Position = pt(500 + i * 4000, 16000)
            cs.Size = sz(3000, 800)
            cs.Control = model
            setp(cs, Title="control", Description="ctl", Name="shape-ctl%d" % i, RotateAngle=0)
    step("control", control)


def special_docs():
    for kind in ("odg", "odp"):
        doc = new(kind)
        page = doc.DrawPages.getByIndex(0)
        step("special-" + kind, special_shapes, doc, page, kind == "odp")
        save(doc, "special.%s" % kind)
        done(doc)


def masters_and_notes():
    doc = new("odp")
    mp = doc.MasterPages.getByIndex(0)
    mp.Name = "MasterOne"
    mk(doc, mp, "RectangleShape", x=500, y=500, text="on master")
    mp2 = doc.MasterPages.insertNewByIndex(doc.MasterPages.Count)
    mp2.Name = "MasterTwo"
    mk(doc, mp2, "EllipseShape", x=500, y=500, text="master two")
    pages = [doc.DrawPages.getByIndex(0), add_page(doc)]
    pages[0].MasterPage = mp
    pages[1].MasterPage = mp2
    for i, p in enumerate(pages):
        mk(doc, p, "RectangleShape", text="slide %d" % i)
        np_ = p.NotesPage
        mk(doc, np_, "RectangleShape", x=500, y=9000, text="note shape")
        mk(doc, np_, "PageShape", x=500, y=500, w=4000, h=3000, PageNumber=i + 1)
    try:
        mk(doc, doc.getHandoutMasterPage(), "RectangleShape", x=500, y=500, text="handout")
    except Exception as e:
        print("  handout master", repr(e)[:100])
    save(doc, "masters-notes.odp")
    doc.storeToURL(uno.systemPathToFileUrl(os.path.join(out, "masters-notes.otp")), (pv("FilterName", "impress8_template"),))
    done(doc)
    doc = new("odg")
    page = doc.DrawPages.getByIndex(0)
    mp = doc.MasterPages.getByIndex(0)
    mk(doc, mp, "RectangleShape", x=500, y=500, text="draw master")
    page.MasterPage = mp
    shapes_core(doc, page)
    save(doc, "draw-master.odg")
    doc.storeToURL(uno.systemPathToFileUrl(os.path.join(out, "draw-master.otg")), (pv("FilterName", "draw8_template"),))
    done(doc)


ANCHORS = ("AS_CHARACTER", "AT_CHARACTER", "AT_PARAGRAPH", "AT_PAGE", "AT_FRAME")


def anchor_enum(n):
    return uno.Enum("com.sun.star.text.TextContentAnchorType", n)


def writer_shapes():
    doc = new("odt")
    text = doc.Text
    cur = text.createTextCursor()
    text.insertString(cur, "Anchoring test paragraph with some text to anchor to.", False)
    text.insertControlCharacter(cur, uno.getConstantByName("com.sun.star.text.ControlCharacter.PARAGRAPH_BREAK"), False)
    frame = doc.createInstance("com.sun.star.text.TextFrame")
    frame.Size = sz(8000, 4000)
    frame.Name = "Host"
    frame.AnchorType = anchor_enum("AT_PARAGRAPH")
    text.insertTextContent(cur, frame, False)
    frame.Text.insertString(frame.Text.createTextCursor(), "frame text", False)
    services = ("RectangleShape", "EllipseShape", "LineShape", "PolyPolygonShape", "ClosedBezierShape", "CustomShape", "TextShape")
    for sv in services:
        for a in ANCHORS:
            def one(sv=sv, a=a):
                if sv in ("LineShape", "PolyPolygonShape"):
                    sh = doc.createInstance("com.sun.star.drawing." + sv)
                    sh.PolyPolygon = (pts((0, 0), (1500, 800)) if sv == "LineShape" else pts((0, 0), (1500, 0), (800, 1000)),)
                elif sv == "ClosedBezierShape":
                    sh = doc.createInstance("com.sun.star.drawing.ClosedBezierShape")
                    coords = struct_("com.sun.star.drawing.PolyPolygonBezierCoords")
                    coords.Coordinates = (pts((0, 0), (1000, 0), (1000, 1000)),)
                    N = uno.Enum("com.sun.star.drawing.PolygonFlags", "NORMAL")
                    coords.Flags = ((N, N, N),)
                    sh.PolyPolygonBezier = coords
                else:
                    sh = doc.createInstance("com.sun.star.drawing." + sv)
                    sh.Size = sz(1500, 1000)
                    if sv == "CustomShape":
                        sh.CustomShapeGeometry = (pv("Type", "diamond"),)
                sh.AnchorType = anchor_enum(a)
                if a == "AT_PAGE":
                    sh.AnchorPageNo = 1
                cursor = text.createTextCursor()
                cursor.gotoStart(False)
                if a == "AT_FRAME":
                    sh.AnchorFrame = frame
                    frame.Text.insertTextContent(frame.Text.createTextCursor(), sh, False)
                else:
                    text.insertTextContent(cursor, sh, False)
                setp(sh, Title="t " + sv, Description="d " + a)
            step("%s-%s" % (sv, a), one)
    def group():
        for a in ANCHORS[:4]:
            g = doc.createInstance("com.sun.star.drawing.GroupShape")
            doc.DrawPage.add(g)
            for k in range(2):
                r = doc.createInstance("com.sun.star.drawing.RectangleShape")
                r.Size = sz(800, 800)
                r.Position = pt(k * 900, 0)
                g.add(r)
            g.AnchorType = anchor_enum(a)
            if a == "AT_PAGE":
                g.AnchorPageNo = 1
    step("groups", group)

    def controls():
        form = doc.createInstance("com.sun.star.form.component.Form")
        doc.DrawPage.Forms.insertByIndex(0, form)
        for i, a in enumerate(ANCHORS):
            model = doc.createInstance("com.sun.star.form.component.CommandButton")
            model.Name = "btn%d" % i
            form.insertByIndex(i, model)
            cs = doc.createInstance("com.sun.star.drawing.ControlShape")
            cs.Size = sz(2000, 700)
            cs.Control = model
            cs.AnchorType = anchor_enum(a)
            if a == "AT_PAGE":
                cs.AnchorPageNo = 1
            if a == "AT_FRAME":
                cs.AnchorFrame = frame
            cursor = text.createTextCursor()
            cursor.gotoStart(False)
            if a == "AT_FRAME":
                frame.Text.insertTextContent(frame.Text.createTextCursor(), cs, False)
            else:
                text.insertTextContent(cursor, cs, False)
    step("controls", controls)
    save(doc, "anchors.odt")
    done(doc)


def writer_group_frame():
    doc = new("odt")
    text = doc.Text
    cur = text.createTextCursor()
    text.insertString(cur, "Group anchored to a frame.", False)
    frame = doc.createInstance("com.sun.star.text.TextFrame")
    frame.Size = sz(8000, 4000)
    frame.AnchorType = anchor_enum("AT_PARAGRAPH")
    text.insertTextContent(cur, frame, False)
    g = doc.createInstance("com.sun.star.drawing.GroupShape")
    r1 = doc.createInstance("com.sun.star.drawing.RectangleShape")
    r1.Size = sz(800, 800)
    g.AnchorType = anchor_enum("AT_FRAME")
    g.AnchorFrame = frame
    frame.Text.insertTextContent(frame.Text.createTextCursor(), g, False)
    g.add(r1)
    save(doc, "anchors-group-frame.odt")
    done(doc)


def writer_graphics():
    doc = new("odt")
    text = doc.Text
    cur = text.createTextCursor()
    text.insertString(cur, "Graphics with contours, image maps and chained frames.", False)
    text.insertControlCharacter(cur, uno.getConstantByName("com.sun.star.text.ControlCharacter.PARAGRAPH_BREAK"), False)
    for i, (name, contour) in enumerate((("single", (pts((0, 0), (100, 0), (100, 100), (0, 100)),)),
                                         ("multi", (pts((0, 0), (50, 0), (50, 50)), pts((60, 60), (100, 60), (100, 100)))),
                                         ("none", None))):
        def one(i=i, name=name, contour=contour):
            g = doc.createInstance("com.sun.star.text.TextGraphicObject")
            g.GraphicURL = PNG_URL
            g.AnchorType = anchor_enum(("AS_CHARACTER", "AT_PARAGRAPH", "AT_PAGE")[i])
            g.Width = 3000
            g.Height = 2000
            g.Name = "grf-" + name
            if contour is not None:
                setp(g, ContourPolyPolygon=contour, SurroundContour=True, ContourOutside=bool(i % 2), IsPixelContour=bool(i // 1 % 2))
            setp(g, GraphicFilter="png", Title="graphic " + name, Description="described", GraphicRotation=900 * i)
            cursor = text.createTextCursor()
            cursor.gotoEnd(False)
            text.insertTextContent(cursor, g, False)
        step("grf-" + name, one)
    def chain():
        f1 = doc.createInstance("com.sun.star.text.TextFrame")
        f2 = doc.createInstance("com.sun.star.text.TextFrame")
        for f, nm in ((f1, "Chain1"), (f2, "Chain2")):
            f.Name = nm
            f.Size = sz(4000, 1500)
            f.AnchorType = anchor_enum("AT_PARAGRAPH")
            cursor = text.createTextCursor()
            cursor.gotoEnd(False)
            text.insertTextContent(cursor, f, False)
        f1.ChainNextName = "Chain2"
        f1.Text.insertString(f1.Text.createTextCursor(), "first frame of a chain " * 20, False)
        f3 = doc.createInstance("com.sun.star.text.TextFrame")
        f3.Name = "MinMax"
        f3.Width = 3000
        setp(f3, WidthType=uno.getConstantByName("com.sun.star.text.SizeType.MIN"), HeightType=uno.getConstantByName("com.sun.star.text.SizeType.MIN"),
             Height=800)
        f3.AnchorType = anchor_enum("AT_CHARACTER")
        cursor = text.createTextCursor()
        cursor.gotoEnd(False)
        text.insertTextContent(cursor, f3, False)
    step("chain", chain)
    save(doc, "graphics.odt")
    done(doc)


def calc_shapes():
    doc = new("ods")
    sh = doc.Sheets.getByIndex(0)
    dp = sh.DrawPage
    for r in range(6):
        sh.getCellByPosition(0, r).String = "row %d" % r
    kinds = ("RectangleShape", "EllipseShape", "LineShape", "PolyPolygonShape", "PolyLineShape", "ClosedBezierShape", "CustomShape",
             "GraphicObjectShape", "TextShape", "ConnectorShape", "CaptionShape", "MeasureShape", "PageShape")
    for i, sv in enumerate(kinds):
        def one(i=i, sv=sv):
            o = doc.createInstance("com.sun.star.drawing." + sv)
            dp.add(o)
            if sv in ("LineShape", "PolyPolygonShape", "PolyLineShape"):
                o.PolyPolygon = (pts((0, 0), (1500, 0), (800, 1000)),)
            elif sv == "ClosedBezierShape":
                coords = struct_("com.sun.star.drawing.PolyPolygonBezierCoords")
                coords.Coordinates = (pts((0, 0), (1000, 0), (1000, 1000)),)
                N = uno.Enum("com.sun.star.drawing.PolygonFlags", "NORMAL")
                coords.Flags = ((N, N, N),)
                o.PolyPolygonBezier = coords
            else:
                o.Size = sz(2500, 1500)
            if sv == "CustomShape":
                o.CustomShapeGeometry = (pv("Type", "diamond"),)
            if sv == "GraphicObjectShape":
                o.GraphicURL = PNG_URL
            if sv not in ("LineShape", "PolyPolygonShape", "PolyLineShape", "ClosedBezierShape", "ConnectorShape", "MeasureShape"):
                o.Position = pt(500 + (i % 4) * 4000, 500 + (i // 4) * 3000)
            elif sv == "ConnectorShape":
                o.StartPosition = pt(500, 12000)
                o.EndPosition = pt(5000, 14000)
            elif sv == "MeasureShape":
                o.StartPosition = pt(6000, 12000)
                o.EndPosition = pt(10000, 12000)
            if i % 2 == 0:
                setp(o, Anchor=sh.getCellByPosition(1 + i % 3, i))
            if i % 3 == 1:
                setp(o, LayerName="background")
            setp(o, Title="calc " + sv, Description="d")
        step("calc-" + sv, one)
    g = doc.createInstance("com.sun.star.drawing.GroupShape")
    dp.add(g)
    for k in range(2):
        r = doc.createInstance("com.sun.star.drawing.RectangleShape")
        r.Size = sz(800, 800)
        r.Position = pt(k * 900 + 15000, 1000)
        g.add(r)
    sh2 = doc.Sheets.getByIndex(0)
    save(doc, "shapes.ods")
    done(doc)


def annotations_docs():
    for kind in ("odp", "odg"):
        doc = new(kind)
        for n in range(2):
            page = doc.DrawPages.getByIndex(0) if n == 0 else add_page(doc)
            mk(doc, page, "RectangleShape", text="annotated %d" % n)
            for k in range(2):
                ann = page.createAndInsertAnnotation()
                KEEP.append(ann)
                ann.Author = "Tester %d" % k
                ann.Position = struct_("com.sun.star.geometry.RealPoint2D", 3.0 + k, 4.0)
                ann.Size = struct_("com.sun.star.geometry.RealSize2D", 5.0, 6.0)
                ann.TextRange.String = "annotation %d.%d" % (n, k)
                ann.DateTime = struct_("com.sun.star.util.DateTime", 0, 12, 30, 10, 5, 6, 2025)
        save(doc, "annotations.%s" % kind)
        done(doc)


def fills():
    doc = new("odg")
    page = doc.DrawPages.getByIndex(0)
    GS = lambda n: uno.Enum("com.sun.star.awt.GradientStyle", n)
    for i, gs in enumerate(("LINEAR", "AXIAL", "RADIAL", "ELLIPTICAL", "SQUARE", "RECT")):
        g = struct_("com.sun.star.awt.Gradient", GS(gs), 0xFF0000, 0x0000FF, 450 * i, 10 * i, 40 + i, 60 - i, 100, 80, 5)
        t = struct_("com.sun.star.awt.Gradient", GS(gs), 0x202020, 0xE0E0E0, 300 * i, 5 * i, 50, 50, 100, 100, 0)
        mk(doc, page, "RectangleShape", x=500 + i * 3000, y=500, w=2800, h=2000, FillStyle=uno.Enum("com.sun.star.drawing.FillStyle", "GRADIENT"),
           FillGradient=g)
        tt = doc.createInstance("com.sun.star.drawing.TransparencyGradientTable")
        tt.insertByName("trans-" + gs, t)
        mk(doc, page, "RectangleShape", x=500 + i * 3000, y=3000, w=2800, h=2000, FillStyle=uno.Enum("com.sun.star.drawing.FillStyle", "SOLID"),
           FillColor=0x3366CC, FillTransparenceGradientName="trans-" + gs)
    for i, hs in enumerate(("SINGLE", "DOUBLE", "TRIPLE")):
        h = struct_("com.sun.star.drawing.Hatch", uno.Enum("com.sun.star.drawing.HatchStyle", hs), 0x00AA00, 120 + 20 * i, 450 * i)
        mk(doc, page, "RectangleShape", x=500 + i * 3000, y=5500, w=2800, h=2000, FillStyle=uno.Enum("com.sun.star.drawing.FillStyle", "HATCH"),
           FillHatch=h, FillBackground=bool(i % 2), FillColor=0xFFEECC)
    for i, ds in enumerate(("RECT", "ROUND", "RECTRELATIVE", "ROUNDRELATIVE")):
        d = struct_("com.sun.star.drawing.LineDash", uno.Enum("com.sun.star.drawing.DashStyle", ds), 2, 100, 3, 300, 150)
        mk(doc, page, "LineShape" if False else "RectangleShape", x=500 + i * 3000, y=8000, w=2800, h=1500,
           LineStyle=uno.Enum("com.sun.star.drawing.LineStyle", "DASH"), LineDash=d, LineWidth=100)
    save(doc, "fills.odg")
    done(doc)


top("pres-settings", presentation_settings)
top("header-footer", header_footer)
top("layouts", layouts)
top("transitions", transitions)
top("animations", animations)
top("actions", actions)
top("draw-links", draw_links)
top("image-maps", image_maps)
top("shapes", shapes_docs)
top("layers", layers_doc)
top("special", special_docs)
top("annotations", annotations_docs)
top("fills", fills)
top("masters-notes", masters_and_notes)
top("writer-shapes", writer_shapes)
top("writer-group-frame", writer_group_frame)
top("writer-graphics", writer_graphics)
top("calc-shapes", calc_shapes)
top("custom-presets", custom_shapes)
top("custom-full", custom_full)
top("fontwork", fontwork)
top("3d-odg", threed, "odg")
top("3d-odp", threed, "odp")

print("crashed steps (retried):", crashes)
print("failed:", failed)
sys.stdout.flush()
os._exit(1 if failed else 0)
