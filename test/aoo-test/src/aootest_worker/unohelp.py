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

"""The helper module the ported fixture scripts import as `unohelp`. It keeps their imports unchanged and hands them the
worker's own office connection; the port argument is ignored."""
import uno

from aootest_worker.ops import core
from aootest_worker.ops.core import pv  # noqa: F401  (re-exported for the scripts)

FILTERS = {"odt": "writer8", "ods": "calc8", "odp": "impress8", "odg": "draw8"}


def connect(port=None, tries=None):
    return core._ctx()


def desktop(ctx):
    return core._desktop()


def odf_version(ctx):
    return core.config_value("/org.openoffice.Office.Common/Save/ODF", "DefaultVersion")


def load(desk, path, readonly=True):
    doc = desk.loadComponentFromURL(uno.systemPathToFileUrl(path), "_blank", 0,
                                    (pv("Hidden", True), pv("MacroExecutionMode", 0), pv("ReadOnly", readonly), pv("UpdateDocMode", 0)))
    if doc is None:
        raise RuntimeError("load returned None")
    return doc
