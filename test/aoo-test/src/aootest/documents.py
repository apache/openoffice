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

"""The documents every save mode is run over. None of them is stored in this repository: they come from the office under
test (the templates and documents its install ships under share/), from documents the office creates itself, and from a
directory the user names with --documents."""
import json
import os
from collections import namedtuple
from pathlib import Path

from aootest import install
from aootest.paths import DATA

# Source: id is the stable name used in test ids; path is None for a document the office creates (`app` names a new empty one,
# `builder` is a (builder name, file name) pair for one the worker's generate op makes); kind is the ODF type it is saved as
# (odt, ods, odp, odg); checks are the data/checks files with the values a reload must read back.
Source = namedtuple("Source", "id path kind app builder checks", defaults=(None, None, ()))
SHIPPED_KIND = {"odt": "odt", "ott": "odt", "ods": "ods", "ots": "ods", "odp": "odp", "otp": "odp", "odg": "odg", "otg": "odg"}
GENERATED = (("writer", "odt"), ("calc", "ods"), ("impress", "odp"), ("draw", "odg"))
# builder, file it makes, kind, checks
BUILT = (("page-fill", "page-fill.odt", "odt", ("page-fill/checks.json", "page-fill/bitmap.json")),
         ("fill-others", "others-writer.odt", "odt", ("fill-others/writer.json",)),
         ("fill-others", "others-calc.ods", "ods", ("fill-others/calc.json",)),
         ("fill-others", "others-draw.odg", "odg", ("fill-others/draw.json",)),
         ("fill-others", "others-impress.odp", "odp", ("fill-others/impress.json",)),
         ("t13", "dropdown.odt", "odt", ("t13/checks-dropdown.json",)),
         ("t13", "annot.odt", "odt", ("t13/checks-annot.json",)),
         ("t13", "tabcolor.ods", "ods", ("t13/checks-tabcolor.json",)),
         ("t13", "scaleto.ods", "ods", ("t13/checks-scaleto.json",)),
         ("t13", "chart.ods", "ods", ("t13/checks-chart.json",)),
         ("page-fill-1_3", "aoo-src.odt", "odt", ("page-fill-1_3/checks.json",)),
         ("page-fill-1_3", "calc-src.ods", "ods", ("page-fill-1_3/checks-calc.json",)),
         ("page-fill-1_3", "calc-drawing-page.ods", "ods", ("page-fill-1_3/checks-calc.json",)))
DEFAULT_LIMIT = 40
SHIPPED_DIRS = ("template", "samples")


def share_dir(layout):
    """share/ sits next to program/ in every layout (macOS: Contents/share)."""
    return layout.program.parent / "share"


def _walk(base):
    for root, dirs, files in os.walk(base):
        dirs.sort()
        for name in sorted(files):
            ext = name.rsplit(".", 1)[-1].lower() if "." in name else ""
            if ext in SHIPPED_KIND:
                yield Path(root) / name, ext


def _interleave(groups):
    """Takes one item from each group in turn, so a limit still covers every type and many directories."""
    iters = [iter(g) for g in groups if g]
    while iters:
        for it in list(iters):
            try:
                yield next(it)
            except StopIteration:
                iters.remove(it)


def _spread(items):
    """Orders (id, path, ext) items so that consecutive ones come from different directories: one from each in turn."""
    by_dir = {}
    for item in items:
        by_dir.setdefault(item[1].parent, []).append(item)
    return list(_interleave([by_dir[d] for d in sorted(by_dir)]))


def shipped(layout, limit=DEFAULT_LIMIT):
    """Documents and templates under share/template and share/samples of the install, at most `limit`: types and directories
    are interleaved, so the same install always gives the same list."""
    share = share_dir(layout)
    groups = {}
    for sub in SHIPPED_DIRS:
        if (share / sub).is_dir():
            for path, ext in _walk(share / sub):
                groups.setdefault(ext, []).append(("shipped/" + path.relative_to(share).as_posix(), path, ext))
    ordered = _interleave([_spread(groups[e]) for e in sorted(groups)])
    return [Source(i, p, SHIPPED_KIND[e]) for i, p, e in list(ordered)[:limit]]


def generated():
    """One small document per application, and the documents the worker builds, all created by the office under test."""
    return ([Source("generated/" + app, None, kind, app) for app, kind in GENERATED] +
            [Source("built/%s/%s" % (b, f), None, kind, None, (b, f), checks) for b, f, kind, checks in BUILT])


def load_checks(name):
    """The list of {"name", "expr", "expect"} in data/checks/<name>."""
    return json.load(open(DATA / "checks" / name, encoding="utf-8"))


def extra(directory):
    """Every ODF document under a directory named by the user."""
    if not directory:
        return []
    base = Path(directory)
    return [Source("documents/" + p.relative_to(base).as_posix(), p, SHIPPED_KIND[e]) for p, e in _walk(base)]


def collect(office=None, limit=DEFAULT_LIMIT, documents=None):
    """The sources for a run: [] when no usable office is named (the tests then skip with a reason)."""
    if not office:
        return []
    try:
        layout = install.discover(office)
    except install.LayoutError:
        return []
    return generated() + shipped(layout, limit) + extra(documents)


def out_name(source):
    """Name of the saved copy: the id with separators and dots flattened (so x.ott and x.odt stay apart), in the extension of
    the ODF type it is saved as."""
    return source.id.replace("/", "__").replace("\\", "__").replace(".", "_") + "." + source.kind


def from_config(config, environ=None):
    """The sources for the pytest run `config` describes (--office or AOO_OFFICE, --shipped-limit, --documents)."""
    environ = os.environ if environ is None else environ
    return collect(config.getoption("--office") or environ.get("AOO_OFFICE"), config.getoption("--shipped-limit"), config.getoption("--documents"))


class Prepared:
    """The file of each source, made once: a shipped or user document is used where it is, anything else is created by `session`
    in `directory`."""

    def __init__(self, session, directory):
        self.session, self.dir, self._made, self._built = session, Path(directory), {}, set()

    def file(self, source):
        if source.path is not None:
            return str(source.path)
        if source.id not in self._made:
            if source.builder:
                name, filename = source.builder
                if name not in self._built:
                    self.session.call("generate", {"builder": name, "outdir": str(self.dir / name)}, timeout=300)
                    self._built.add(name)
                self._made[source.id] = str(self.dir / name / filename)
            else:
                path = self.dir / ("%s.%s" % (source.app, source.kind))
                self.dir.mkdir(parents=True, exist_ok=True)
                self.session.call("new_and_store", {"app": source.app, "dst": str(path), "filter": FILTERS[source.kind],
                                                    "text": "Hello %s" % source.app}, timeout=180)
                self._made[source.id] = str(path)
        return self._made[source.id]


FILTERS = {"odt": "writer8", "ods": "calc8", "odp": "impress8", "odg": "draw8"}
