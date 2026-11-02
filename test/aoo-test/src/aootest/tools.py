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

"""Locates the external tools the checkers need: an environment variable first, then the verified fetch cache (aootest.fetch)."""
import os
import shutil

from aootest import fetch


def _file(var):
    p = os.environ.get(var)
    return p if p and os.path.isfile(p) else None


def _cached(name):
    p = fetch.cached_path(name)
    return str(p) if p else None


def java():
    return shutil.which(os.environ.get("AOO_JAVA", "java"))


def jing():
    """Path of a jing jar (AOO_JING, else the fetch cache), or None."""
    return _file("AOO_JING") or _cached("jing")


def odfvalidator():
    """Path of the odfvalidator jar with dependencies (AOO_ODFVALIDATOR, else the fetch cache), or None."""
    return _file("AOO_ODFVALIDATOR") or _cached("odfvalidator")


def schema_dir():
    """Directory with the OASIS ODF 1.2, 1.3 and 1.4 RELAX NG schemas (AOO_SCHEMA_DIR, else the fetch cache), or None when files are missing."""
    d = os.environ.get("AOO_SCHEMA_DIR")
    need = ("OpenDocument-v1.2-os-schema.rng", "OpenDocument-v1.2-os-manifest-schema.rng", "OpenDocument-v1.2-os-dsig-schema.rng",
            "OpenDocument-v1.3-schema.rng", "OpenDocument-v1.3-manifest-schema.rng", "OpenDocument-v1.4-schema.rng", "OpenDocument-v1.4-manifest-schema.rng")
    if d and all(os.path.isfile(os.path.join(d, f)) for f in need):
        return d
    cached = fetch.group_dir("schemas")
    return str(cached) if cached else None


def jing_command():
    """[java, -jar, jar, -i] or None when java or jing is missing."""
    j, jar = java(), jing()
    return [j, "-jar", jar, "-i"] if j and jar else None
