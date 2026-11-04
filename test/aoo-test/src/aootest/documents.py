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
from aootest.client import WorkerFailure, WorkerHang
from aootest.paths import DATA

# Source: id is the stable name used in test ids; path is None for a document the office creates (`app` names a new empty one,
# `builder` is a (builder name, file name) pair for one the worker's generate op makes); kind is the ODF type it is saved as
# (odt, ods, odp, odg); checks are the data/checks files with the values a reload must read back.
Source = namedtuple("Source", "id path kind app builder checks", defaults=(None, None, ()))
SHIPPED_KIND = {"odt": "odt", "ott": "odt", "ods": "ods", "ots": "ods", "odp": "odp", "otp": "odp", "odg": "odg", "otg": "odg"}
GENERATED = (("writer", "odt"), ("calc", "ods"), ("impress", "odp"), ("draw", "odg"))
# builder, file it makes, kind, checks. The para-fill-foreign builder is not here: it crashes the trunk dev app
# (tests/odf/test_known_crashes.py), and a crash slows the office starts that follow.
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
         ("page-fill-1_3", "calc-drawing-page.ods", "ods", ("page-fill-1_3/checks-calc.json",)),
         ("row-split", "row-split.odt", "odt", ("row-split/checks.json",)),
         ("grid-snap", "grid-snap.odt", "odt", ("grid-snap/checks.json",)),
         ("ole-text", "ole-text.odg", "odg", ("ole-text/checks.json",)),
         ("ole-text-edge", "ole-text-edge.odg", "odg", ("ole-text-edge/checks.json",)),
         ("chart-shapes", "chart-shapes.ods", "ods", ("chart-shapes/checks.json",)),
         ("normal-values5b", "normal5b.ods", "ods", ("normal-values5b/checks-image.json",)),
         ("connector-viewbox", "conn.odg", "odg", ("connector-viewbox/checks.json",)),
         ("connector-viewbox", "conn.odp", "odp", ("connector-viewbox/checks.json",)),
         ("para-fill", "para-fill.odt", "odt", ("para-fill/checks.json", "para-fill/bitmap.json")),
         ("alien-attrs", "alien-writer.odt", "odt", ("alien-attrs/writer.json", "alien-attrs/writer-cell.json", "alien-attrs/writer-foreign.json")),
         ("alien-attrs", "alien-draw.odg", "odg", ("alien-attrs/draw.json",)),
         ("alien-attrs", "alien-calc.ods", "ods", ("alien-attrs/calc.json",)),
         ("boolean-writer", "boolean-writer.odt", "odt", ("boolean-writer/checks.json",)),
         ("normal-values", "normal-values.odt", "odt", ("normal-values/checks.json",)),
         ("note-ref", "note-ref.odt", "odt", ("note-ref/checks-seq.json",)))
# The files of the script builders (the ODF harness's large make.py scripts), generated from a run: builder<TAB>file. They carry no
# reload checks, so they only add the save-mode items. Types the four above do not cover get their own filter in FILTERS.
BUILT_KIND = dict(SHIPPED_KIND, odm="odm", odf="odf", odb="odb")
BUILD_TIMEOUT, BUILD_TIMEOUTS = 300, {"base": 180, "calc": 900, "chart": 900, "draw": 900, "styles-gfx": 900, "styles-text": 900, "writer": 900}
DEFAULT_LIMIT = 40
SHIPPED_DIRS = ("template", "samples")


def _tsv(name):
    return [line.split("\t") for line in (DATA / name).read_text(encoding="utf-8").splitlines() if line and not line.startswith("#")]


def script_excluded():
    """(builder, file, reason) rows of data/built-excluded.tsv: files of the script builders that are not in the default run."""
    return [tuple(r) for r in _tsv("built-excluded.tsv")]


def script_built():
    """(builder, file, kind, checks) rows for the files in data/built-files.tsv, without the excluded ones."""
    left_out = {(b, f) for b, f, _ in script_excluded()}
    return [(b, f, BUILT_KIND[f.rsplit(".", 1)[1]], ()) for b, f in _tsv("built-files.tsv") if (b, f) not in left_out]


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
            [Source("built/%s/%s" % (b, f), None, kind, None, (b, f), checks) for b, f, kind, checks in BUILT + tuple(script_built())])


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
    in `directory`. A script builder runs in the office that `own_office(builder)` returns, and that office is closed afterwards:
    those scripts fail or stall depending on what the office did before."""

    def __init__(self, session, directory, own_office=None):
        self.session, self.dir, self._made, self._built, self._failed = session, Path(directory), {}, set(), {}
        self._outdir = {}
        self.own_office = own_office
        self.script_builders = {b for b, _ in _tsv("built-files.tsv")}

    def file(self, source):
        """The file of `source`. A source that could not be made raises the same error every time, without trying again."""
        if source.path is not None:
            return str(source.path)
        if source.id in self._failed:
            raise self._failed[source.id]
        if source.id not in self._made:
            try:
                self._make(source)
            except WorkerFailure as e:
                self._failed[source.id] = e
                raise
        return self._made[source.id]

    def _build(self, name):
        """Runs builder `name`. A script builder that stalls is run once more in a fresh office and a directory of its own: the base
        script stalled in its first variant in two of about six runs, and left a lock file behind."""
        scripted = self.own_office and name in self.script_builders
        for attempt in (1, 2):
            outdir = self.dir / name / ("out" if attempt == 1 else "out-retry")
            own = self.own_office(name) if scripted else None
            try:
                (own or self.session).call("generate", {"builder": name, "outdir": str(outdir)},
                                           timeout=BUILD_TIMEOUTS.get(name, BUILD_TIMEOUT))
                break
            except WorkerHang as e:
                if not scripted or attempt == 2:
                    self._failed[name] = e
                    raise
            except WorkerFailure as e:
                self._failed[name] = e
                raise
            finally:
                if own:
                    own.close()
        self._outdir[name] = outdir
        self._built.add(name)

    def _make(self, source):
        if source.builder:
            name, filename = source.builder
            if name in self._failed:
                raise self._failed[name]
            if name not in self._built:
                self._build(name)
            self._made[source.id] = str(self._outdir[name] / filename)
        else:
            path = self.dir / ("%s.%s" % (source.app, source.kind))
            self.dir.mkdir(parents=True, exist_ok=True)
            self.session.call("new_and_store", {"app": source.app, "dst": str(path), "filter": FILTERS[source.kind],
                                                "text": "Hello %s" % source.app}, timeout=180)
            self._made[source.id] = str(path)


FILTERS = {"odt": "writer8", "ods": "calc8", "odp": "impress8", "odg": "draw8", "odm": "writerglobal8", "odf": "math8",
           "odb": "StarOffice XML (Base)"}
