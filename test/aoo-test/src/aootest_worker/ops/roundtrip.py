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

"""Reload checks: evaluate expressions over a loaded document and report the values."""
import os

import uno

from aootest_worker.ops import core
from aootest_worker.ops.core import pv


def plain(v):
    """A JSON-able copy of a UNO value."""
    if isinstance(v, (bool, int, float, str)) or v is None:
        return v
    if isinstance(v, (tuple, list)):
        return [plain(x) for x in v]
    if isinstance(v, uno.Enum):
        return v.value
    if hasattr(v, "typeName"):
        return {k: plain(getattr(v, k)) for k in dir(v) if not k.startswith("_") and k not in ("typeName", "value")}
    return repr(v)


def roundtrip(path, checks):
    """Loads `path` and evaluates each {"name", "expr"} of `checks` with `doc` and `uno` in scope. The expressions come from
    the suite's own data files. Returns {name: value}; an expression that raises gives the string "ERROR <repr>"."""
    doc = core._desktop().loadComponentFromURL(uno.systemPathToFileUrl(os.path.abspath(path)), "_blank", 0,
                                               (pv("Hidden", True), pv("ReadOnly", True), pv("MacroExecutionMode", 0), pv("UpdateDocMode", 0)))
    if doc is None:
        raise core.Fail("uno", "load returned None for %s" % path)
    got = {}
    try:
        for c in checks:
            try:
                got[c["name"]] = plain(eval(c["expr"], {"doc": doc, "uno": uno}))
            except Exception as e:
                got[c["name"]] = "ERROR " + repr(e)
    finally:
        core._close(doc)
    return got


OPS = {"roundtrip": core.guarded(roundtrip)}
