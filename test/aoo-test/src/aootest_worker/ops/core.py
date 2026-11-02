################################################################
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
################################################################

"""Connecting to the office and the basic document operations. Imports uno, so only the worker's Python can load it."""
import functools
import os
import time

import uno
from com.sun.star.beans import PropertyValue

from aootest_worker.proto import Fail

FILTER_IMPORT, FILTER_EXPORT = 0x1, 0x2
FACTORIES = {"writer": "private:factory/swriter", "calc": "private:factory/scalc",
             "impress": "private:factory/simpress", "draw": "private:factory/sdraw"}

_state = {"ctx": None, "desktop": None}


def pv(name, value):
    p = PropertyValue()
    p.Name, p.Value = name, value
    return p


def _ctx():
    if _state["ctx"] is None:
        raise Fail("bad_args", "not connected: call connect first")
    return _state["ctx"]


def _create(service):
    ctx = _ctx()
    return ctx.ServiceManager.createInstanceWithContext(service, ctx)


def _desktop():
    if _state["desktop"] is None:
        _state["desktop"] = _create("com.sun.star.frame.Desktop")
    return _state["desktop"]


def connect(url, wait=2.0):
    """Tries to resolve `url` for up to `wait` seconds. Returns {"connected": bool}; the host retries while the office is alive."""
    local = uno.getComponentContext()
    resolver = local.ServiceManager.createInstanceWithContext("com.sun.star.bridge.UnoUrlResolver", local)
    deadline = time.time() + wait
    while True:
        try:
            _state["ctx"], _state["desktop"] = resolver.resolve(url), None
            return {"connected": True}
        except Exception as e:
            if "NoConnect" not in type(e).__name__ and "NoConnect" not in str(e):
                raise
            if time.time() >= deadline:
                return {"connected": False}
            time.sleep(0.25)


def config_value(path, name):
    """One value from the office configuration, e.g. /org.openoffice.Office.Common/Save/ODF, DefaultVersion."""
    cp = _create("com.sun.star.configuration.ConfigurationProvider")
    access = cp.createInstanceWithArguments("com.sun.star.configuration.ConfigurationAccess", (pv("nodepath", path),))
    return access.getByName(name)


def product_info():
    base = "/org.openoffice.Setup/Product"
    info = {}
    for key in ("ooName", "ooSetupVersion", "ooSetupVersionAboutBox", "ooSetupExtension", "ooVendor", "ooSetupBuildId"):
        try:
            info[key] = config_value(base, key)
        except Exception:
            pass
    return info


def filters(names):
    """For each filter name: its flags (IMPORT/EXPORT as booleans), document service and type, or None if unknown."""
    factory = _create("com.sun.star.document.FilterFactory")
    out = {}
    for name in names:
        if not factory.hasByName(name):
            out[name] = None
            continue
        props = {p.Name: p.Value for p in factory.getByName(name)}
        flags = props.get("Flags", 0)
        out[name] = {"import": bool(flags & FILTER_IMPORT), "export": bool(flags & FILTER_EXPORT), "flags": flags,
                     "service": props.get("DocumentService"), "type": props.get("Type")}
    return out


def _close(doc):
    try:
        doc.close(True)
    except Exception:
        doc.dispose()


def new_and_store(app, dst, filter, text="Hello"):
    """New document of `app` (writer, calc, impress, draw) with a little content, stored to `dst` with `filter`, then closed."""
    if app not in FACTORIES:
        raise Fail("bad_args", "unknown app %r" % app)
    doc = _desktop().loadComponentFromURL(FACTORIES[app], "_blank", 0, (pv("Hidden", True),))
    if doc is None:
        raise Fail("uno", "could not create a new %s document" % app)
    try:
        if app == "writer":
            doc.Text.setString(text)
        elif app == "calc":
            doc.Sheets.getByIndex(0).getCellByPosition(0, 0).setString(text)
        doc.storeToURL(uno.systemPathToFileUrl(os.path.abspath(dst)), (pv("FilterName", filter), pv("Overwrite", True)))
    finally:
        _close(doc)
    return {"dst": os.path.abspath(dst)}


def convert(src, dst, filter, load_props=None, store_props=None):
    """Loads `src` read-only, hidden, without running macros, stores it as `dst` with `filter`, and always closes it."""
    props = [pv("Hidden", True), pv("ReadOnly", True), pv("MacroExecutionMode", 0), pv("UpdateDocMode", 0)]
    props += [pv(k, v) for k, v in (load_props or {}).items()]
    doc = _desktop().loadComponentFromURL(uno.systemPathToFileUrl(os.path.abspath(src)), "_blank", 0, tuple(props))
    if doc is None:
        raise Fail("uno", "load returned None for %s" % src)
    try:
        sprops = [pv("FilterName", filter), pv("Overwrite", True)] + [pv(k, v) for k, v in (store_props or {}).items()]
        doc.storeToURL(uno.systemPathToFileUrl(os.path.abspath(dst)), tuple(sprops))
    finally:
        _close(doc)
    return {"dst": os.path.abspath(dst)}


def load_info(path):
    """Loads `path` and reports the document service it opened as, then closes it. Errors come back as a message, not a raise."""
    try:
        doc = _desktop().loadComponentFromURL(uno.systemPathToFileUrl(os.path.abspath(path)), "_blank", 0,
                                              (pv("Hidden", True), pv("ReadOnly", True), pv("MacroExecutionMode", 0), pv("UpdateDocMode", 0)))
    except Exception as e:
        return {"loaded": False, "error": "%s: %s" % (type(e).__name__, e)}
    if doc is None:
        return {"loaded": False, "error": "load returned None"}
    try:
        services = [s for s in ("com.sun.star.text.TextDocument", "com.sun.star.sheet.SpreadsheetDocument",
                                "com.sun.star.presentation.PresentationDocument", "com.sun.star.drawing.DrawingDocument")
                    if doc.supportsService(s)]
        return {"loaded": True, "services": services}
    finally:
        _close(doc)


def open_documents():
    """URLs of the documents the office has open (an empty string for an unsaved one)."""
    found = []
    components = _desktop().Components.createEnumeration()
    while components.hasMoreElements():
        c = components.nextElement()
        found.append(getattr(c, "URL", "") or "")
    return {"documents": found}


def sleep(seconds):
    """For the hang test: never returns before the host's timeout."""
    time.sleep(seconds)
    return {}


def die():
    """For the crash test: the worker exits abruptly."""
    os._exit(139)


_UNO_EXCEPTION = uno.getClass("com.sun.star.uno.Exception")


def _office_gone():
    try:
        _state["ctx"].ServiceManager
        return False
    except Exception:
        return True


def guarded(fn):
    """A failure while the bridge is dead is reported as office_dead, whatever exception pyuno raised for it."""
    @functools.wraps(fn)
    def run(*args, **kw):
        try:
            return fn(*args, **kw)
        except Fail:
            raise
        except Exception as e:
            if _state["ctx"] is not None and _office_gone():
                raise Fail("office_dead", "%s: %s" % (type(e).__name__, e))
            if isinstance(e, _UNO_EXCEPTION):
                raise Fail("uno", "%s: %s" % (type(e).__name__, e))
            raise
    return run


_OPS = {"connect": connect, "config_value": config_value, "product_info": product_info, "filters": filters,
       "new_and_store": new_and_store, "convert": convert, "load_info": load_info, "open_documents": open_documents, "sleep": sleep, "die": die}
OPS = {name: fn if name in ("connect", "sleep", "die") else guarded(fn) for name, fn in _OPS.items()}
