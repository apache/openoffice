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

# usage: runpy.sh fixtures/signed/make.py <port> <outdir>
# Certificate: fixtures/signed/setup-cert.sh builds runs/task12/gen-signed/nssdb (certutil, one self-signed RSA key).
# Needs an office started by fixtures/signed/start-ui.sh <strict|ext> <port> (profile runs/profile_t12_sign_<mode>), not
# start.sh: -headless cancels every modal dialog and the Digital Signatures dialog is the only signing entry point.
# Saves with the office's configured ODF version, then signs each saved file in place by driving the dialog through
# its accessibility tree (Sign Document, pick the only certificate, OK, Close). The dialog runs on the office main
# thread (AsyncCallback), otherwise Execute() has nothing to run on.
import os, sys, time, threading, uno, unohelper
from com.sun.star.awt import XCallback
from com.sun.star.accessibility.AccessibleStateType import ENABLED
from unohelp import FILTERS, connect, desktop, pv

port, outdir = int(sys.argv[1]), sys.argv[2]
os.makedirs(outdir, exist_ok=True)
ctx = connect(port)
sm = ctx.ServiceManager
d = desktop(ctx)
tk = sm.createInstanceWithContext("com.sun.star.awt.Toolkit", ctx)
disp = sm.createInstanceWithContext("com.sun.star.frame.DispatchHelper", ctx)
PUSH_BUTTON = 44


def build_writer(doc):
    t = doc.Text
    t.insertString(t.createTextCursor(), "Signed text document.", False)


def build_calc(doc):
    sh = doc.Sheets.getByIndex(0)
    sh.getCellByPosition(0, 0).String = "label"
    sh.getCellByPosition(1, 0).Value = 42
    sh.getCellByPosition(1, 1).Formula = "=B1*2"


def build_impress(doc):
    page = doc.DrawPages.getByIndex(0)
    sh = doc.createInstance("com.sun.star.drawing.TextShape")
    page.add(sh)
    sh.String = "Signed presentation"


def build_draw(doc):
    page = doc.DrawPages.getByIndex(0)
    sh = doc.createInstance("com.sun.star.drawing.RectangleShape")
    page.add(sh)
    sh.Size = uno.createUnoStruct("com.sun.star.awt.Size", 4000, 2000)
    sh.String = "Signed drawing"


KINDS = (("text", "odt", "swriter", build_writer), ("calc", "ods", "scalc", build_calc),
         ("impress", "odp", "simpress", build_impress), ("draw", "odg", "sdraw", build_draw))


def find(acc, role, name):
    try:
        c = acc.getAccessibleContext()
    except Exception:
        return None
    if c.getAccessibleRole() == role and c.getAccessibleName() == name:
        return acc
    for i in range(c.getAccessibleChildCount()):
        r = find(c.getAccessibleChild(i), role, name)
        if r:
            return r


def top(title):
    for k in range(tk.getTopWindowCount()):
        w = tk.getTopWindow(k)
        if w is not None and w.getAccessibleContext().getAccessibleName() == title:
            return w


def wait_top(title, secs=30):
    end = time.time() + secs
    while time.time() < end:
        w = top(title)
        if w is not None:
            return w
        time.sleep(0.2)
    raise SystemExit("timeout waiting for window " + title)


def press(win, name):
    b = find(win, PUSH_BUTTON, name)
    if b is None:
        raise SystemExit("no button " + name)
    t = threading.Thread(target=lambda: b.getAccessibleContext().doAccessibleAction(0), daemon=True)
    t.start()
    return t


class Sign(unohelper.Base, XCallback):
    def __init__(self, frame):
        self.frame, self.done, self.err = frame, False, None

    def notify(self, data):
        try:
            disp.executeDispatch(self.frame, ".uno:Signature", "", 0, ())
        except Exception as e:
            self.err = e
        self.done = True


def sign(path):
    doc = d.loadComponentFromURL(uno.systemPathToFileUrl(path), "_blank", 0, (pv("Hidden", True),))
    cb = Sign(doc.CurrentController.Frame)
    sm.createInstanceWithContext("com.sun.star.awt.AsyncCallback", ctx).addCallback(cb, None)
    dlg = wait_top("Digital Signatures")
    press(dlg, "Sign Document")
    chooser = wait_top("Select Certificate")
    pick_certificate(chooser)
    press(chooser, "OK")
    end = time.time() + 30
    while top("Select Certificate") is not None and time.time() < end:
        time.sleep(0.2)
    dlg = wait_top("Digital Signatures")
    time.sleep(1)
    press(dlg, "Close")
    end = time.time() + 30
    while not cb.done and time.time() < end:
        time.sleep(0.2)
    if not cb.done or cb.err:
        raise SystemExit("signing did not finish: %r" % cb.err)
    doc.close(True)


def pick_certificate(chooser):
    ok = find(chooser, PUSH_BUTTON, "OK").getAccessibleContext()
    # The chooser preselects the only certificate of the throw-away database.
    if not ok.getAccessibleStateSet().contains(ENABLED):
        raise SystemExit("no certificate preselected in the chooser")


for kind, ext, factory, build in KINDS:
    out = os.path.join(outdir, "signed-%s.%s" % (kind, ext))
    doc = d.loadComponentFromURL("private:factory/" + factory, "_blank", 0, (pv("Hidden", True),))
    build(doc)
    doc.storeToURL(uno.systemPathToFileUrl(out), (pv("FilterName", FILTERS[ext]),))
    doc.close(True)
    sign(out)
    print(out)
