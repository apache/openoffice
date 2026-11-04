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

"""Import twins: the same source saved as ODF 1.2 and as ODF 1.3 by another producer (plain and extended) must import alike.
The corpus is a directory with v12, v13, v12ext and v13ext subdirectories holding the same file names. Each document is
loaded once, dumped (aootest_worker.ops.dump) and the dumps of every pair are compared."""
import json
import re
import zipfile
from pathlib import Path

from aootest.checkers import diff7
from aootest.client import WorkerError, WorkerFailure, WorkerHang

PAIRS = (("v12", "v13"), ("v12ext", "v13ext"))
MODES = sorted({m for p in PAIRS for m in p})
SUFFIXES = (".odt", ".ods", ".odp", ".odg")


def corpus_problem(root):
    """Why `root` is not a usable twin corpus, or None."""
    for m in MODES:
        if not (Path(root) / m).is_dir():
            return "no %s directory in %s" % (m, root)
    names = [set(d.name for d in (Path(root) / m).iterdir() if d.suffix in SUFFIXES) for m in MODES]
    if not names[0]:
        return "no documents in %s" % (Path(root) / MODES[0])
    if any(n != names[0] for n in names):
        return "the four directories do not hold the same file names"
    return None


def document_names(root, mode="v12"):
    return sorted(p.name for p in (Path(root) / mode).iterdir() if p.suffix in SUFFIXES)


def mutate(src, dst):
    """Copy of an ODF text package with one word appended to the first paragraph that has text; False if there is none."""
    done = False
    with zipfile.ZipFile(src) as zi, zipfile.ZipFile(dst, "w") as zo:
        for it in zi.infolist():
            data = zi.read(it.filename)
            if it.filename == "content.xml" and not done:
                text = data.decode("utf-8")
                text2, n = re.subn(r">([A-Za-z][^<>]{2,})</text:p>", r">\1 mutated</text:p>", text, count=1)
                if n:
                    data, done = text2.encode("utf-8"), True
            zo.writestr(it, data, compress_type=it.compress_type)
    return done


def dump_one(session, path, timeout=300):
    """The dump of one document, or {"error": text} when it did not load, hung or killed the office."""
    try:
        return session.call("dump_document", {"path": str(path)}, timeout=timeout)
    except WorkerError as e:
        return {"error": "%s: %s" % (e.kind, e.message)}
    except WorkerHang as e:
        return {"error": "HANG: %s" % e}
    except WorkerFailure as e:
        return {"error": "CRASH: %s" % e}


class TwinRun:
    def __init__(self, session, root, workdir):
        self.session, self.root, self.dir = session, Path(root), Path(workdir)
        self._dumps, self._diffs, self._controls = {}, {}, None

    def dumps(self, mode, source_dir=None, tag=None):
        """Directory of <document>.json dumps for the documents of `mode` (or of `source_dir`), written on first use."""
        tag = tag or mode
        if tag not in self._dumps:
            out = self.dir / "dump" / tag
            out.mkdir(parents=True, exist_ok=True)
            src = Path(source_dir) if source_dir else self.root / mode
            for p in sorted(src.iterdir()):
                if p.suffix in SUFFIXES:
                    (out / (p.name + ".json")).write_text(json.dumps(dump_one(self.session, p), sort_keys=True, indent=0, default=str))
            self._dumps[tag] = out
        return self._dumps[tag]

    def load(self, mode, name):
        return json.loads((self.dumps(mode) / (name + ".json")).read_text())

    def diff(self, pair):
        """{document: {"ndiff", "diffs", ...}} for the dumps of a pair of modes."""
        if pair not in self._diffs:
            self._diffs[pair] = diff7.diff_dirs(str(self.dumps(pair[0])), str(self.dumps(pair[1])))
        return self._diffs[pair]

    def controls(self):
        """Compares an unchanged copy and a copy with one changed word of a text document: ({"same", "mutated"} ndiff, base name),
        or (None, reason). The comparison can only be trusted if the first is 0 and the second is not."""
        if self._controls is None:
            base = None
            for name in document_names(self.root):
                d = self.load("v12", name)
                if name.endswith(".odt") and d.get("type") == "writer" and (d.get("body") or {}).get("chars", 0) > 20:
                    base = self.root / "v12" / name
                    break
            if base is None:
                self._controls = (None, "no text document with characters in the corpus")
            else:
                old, new = self.dir / "ctl" / "old", self.dir / "ctl" / "new"
                old.mkdir(parents=True), new.mkdir(parents=True)
                for d in (old, new):
                    (d / "ctl-same.odt").write_bytes(base.read_bytes())
                (old / "ctl-mut.odt").write_bytes(base.read_bytes())
                if not mutate(base, new / "ctl-mut.odt"):
                    self._controls = (None, "could not change a word in %s" % base.name)
                else:
                    res = diff7.diff_dirs(str(self.dumps(None, old, "ctl-old")), str(self.dumps(None, new, "ctl-new")))
                    self._controls = ({"same": res["ctl-same.odt"]["ndiff"], "mutated": res["ctl-mut.odt"]["ndiff"]}, base.name)
        return self._controls
