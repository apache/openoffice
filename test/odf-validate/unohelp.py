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

import time, uno
from com.sun.star.beans import PropertyValue

FILTERS = {"odt": "writer8", "ods": "calc8", "odp": "impress8", "odg": "draw8"}


def pv(n, v):
    p = PropertyValue(); p.Name = n; p.Value = v; return p


def connect(port, tries=60):
    local = uno.getComponentContext()
    res = local.ServiceManager.createInstanceWithContext("com.sun.star.bridge.UnoUrlResolver", local)
    for _ in range(tries):
        try:
            return res.resolve("uno:socket,host=localhost,port=%d;urp;StarOffice.ComponentContext" % port)
        except Exception:
            time.sleep(1)
    raise SystemExit("no connection on port %d" % port)


def odf_version(ctx):
    cp = ctx.ServiceManager.createInstanceWithContext("com.sun.star.configuration.ConfigurationProvider", ctx)
    ca = cp.createInstanceWithArguments("com.sun.star.configuration.ConfigurationAccess",
                                        (pv("nodepath", "/org.openoffice.Office.Common/Save/ODF"),))
    return ca.getByName("DefaultVersion")


def desktop(ctx):
    return ctx.ServiceManager.createInstanceWithContext("com.sun.star.frame.Desktop", ctx)


def load(desk, path, readonly=True):
    doc = desk.loadComponentFromURL(uno.systemPathToFileUrl(path), "_blank", 0,
            (pv("Hidden", True), pv("MacroExecutionMode", 0), pv("ReadOnly", readonly), pv("UpdateDocMode", 0)))
    if doc is None:
        raise RuntimeError("load returned None")
    return doc
