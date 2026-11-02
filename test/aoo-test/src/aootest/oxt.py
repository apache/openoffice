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

"""Builds a small extension (a Python UNO component) with zipfile, so installing one needs no compiler or JRE."""
import zipfile
from pathlib import Path

IDENTIFIER = "org.apache.openoffice.aootest.testextension"
SERVICE = "com.sun.star.comp.smoketest.TestExtension"
IMPLEMENTATION = "com.sun.star.comp.smoketest.TestExtension$_TestExtension"
FILE_NAME = "TestExtension.oxt"

COMPONENT = """import unohelper
from com.sun.star.lang import XServiceInfo

IMPL = %(impl)r
SERVICE = %(service)r


class TestExtension(unohelper.Base, XServiceInfo):
    def __init__(self, ctx):
        self.ctx = ctx

    def getImplementationName(self):
        return IMPL

    def supportsService(self, name):
        return name == SERVICE

    def getSupportedServiceNames(self):
        return (SERVICE,)


g_ImplementationHelper = unohelper.ImplementationHelper()
g_ImplementationHelper.addImplementation(TestExtension, IMPL, (SERVICE,))
""" % {"impl": IMPLEMENTATION, "service": SERVICE}

MANIFEST = """<?xml version="1.0" encoding="UTF-8"?>
<manifest:manifest xmlns:manifest="http://openoffice.org/2001/manifest">
 <manifest:file-entry manifest:full-path="testextension.py" manifest:media-type="application/vnd.sun.star.uno-component;type=Python"/>
</manifest:manifest>
"""

DESCRIPTION = """<?xml version="1.0" encoding="UTF-8"?>
<description xmlns="http://openoffice.org/extensions/description/2006" xmlns:xlink="http://www.w3.org/1999/xlink">
 <identifier value="%s"/>
 <version value="1.0"/>
 <display-name><name lang="en">AOO test suite extension</name></display-name>
</description>
""" % IDENTIFIER


def build(directory):
    """Writes TestExtension.oxt into `directory` and returns its path."""
    path = Path(directory) / FILE_NAME
    with zipfile.ZipFile(path, "w", zipfile.ZIP_DEFLATED) as z:
        z.writestr("META-INF/manifest.xml", MANIFEST)
        z.writestr("description.xml", DESCRIPTION)
        z.writestr("testextension.py", COMPONENT)
    return path
