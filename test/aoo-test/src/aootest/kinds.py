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
"""Error kinds of the extended save modes. Those modes write extension content on purpose, so their output is not expected to be
schema valid; what must not happen is that it gets worse. A baseline per (mode, schema) lists the documents it covers and each
kind of error (the message with the expected-alternatives tail removed) with its count and the packages that have it. A run is
worse when a kind is new, a count or a package count went up, or a baseline document is no longer covered. Only documents the
tests build or create are used: the ones an install ships differ from install to install."""
import json
import re

from aootest.checkers import compare, report
from aootest.paths import DATA

BASELINE = DATA / "baseline"
# (save mode, schema): 12, 13 and 14 are the plain schemas, 12x, 13x and 14x the same with foreign content removed first
PAIRS = (("ext", "12"), ("ext", "12x"), ("x13", "13"), ("x13", "13x"), ("x14", "14"), ("x14", "14x"))
STABLE_PREFIXES = ("built/", "generated/")


def stable(sources):
    """The sources whose files do not depend on the install."""
    return [s for s in sources if s.id.startswith(STABLE_PREFIXES)]


def kinds_of(val, version, packages):
    """The kinds of the errors that validate.check() found against `version` in the named packages. A jing failure that names
    the work directory is grouped under one key, whatever the directory was."""
    entries = [e for e in val["errors"][version] if e[0].split("/", 1)[0] in packages]
    res = {}
    for key, v in report.kinds_of({"errors": {version: entries}}, version).items():
        key = re.sub(r"/[^ ]*/w-validate", "<workdir>", key)
        old = res.setdefault(key, {"errors": 0, "packages": []})
        old["errors"] += v["errors"]
        old["packages"] = sorted(set(old["packages"]) | set(v["packages"]))
    return res


def path(mode, version):
    return BASELINE / ("kinds-%s-%s.json" % (mode, version))


def load(mode, version):
    p = path(mode, version)
    return json.loads(p.read_text(encoding="utf-8")) if p.exists() else None


def write(mode, version, documents, kinds):
    BASELINE.mkdir(exist_ok=True)
    path(mode, version).write_text(json.dumps({"documents": sorted(documents), "kinds": kinds}, indent=1, sort_keys=True) + "\n", encoding="utf-8")


def judge(baseline, documents, kinds):
    """(lines, bad): how `kinds` of the run differ from the baseline, and whether it is worse."""
    missing = sorted(set(baseline["documents"]) - set(documents))
    lines, bad = compare.compare(baseline["kinds"], kinds)
    return ["NOT COVERED  " + d for d in missing] + lines, bool(missing) or bad
