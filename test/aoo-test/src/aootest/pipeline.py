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

"""One save mode over the source documents: convert every document once, then run the checkers once over the output
directory (one JVM per mode). Everything is computed on first use and kept."""
from pathlib import Path

from aootest import documents, tools
from aootest.checkers import crosscheck, doctypecheck, formulacheck, idcheck, pkgcheck, validate
from aootest.client import WorkerError, WorkerFailure, WorkerHang

FILTERS = documents.FILTERS


def by_package(errors):
    """{package: [entries]} for a checker's error list (the first item of an entry is '<package>/<part>')."""
    out = {}
    for e in errors:
        out.setdefault(e[0].split("/", 1)[0], []).append(e)
    return out


def save_one(session, src, dst, timeout=180):
    """Converts `src` to `dst` with the filter for its extension. Returns {"status": ok|FAIL|HANG|CRASH, "detail": str}."""
    try:
        session.call("convert", {"src": src, "dst": dst, "filter": FILTERS[dst.rsplit(".", 1)[1]]}, timeout=timeout)
        return {"status": "ok", "detail": ""}
    except WorkerError as e:
        return {"status": "FAIL", "detail": "%s: %s" % (e.kind, e.message)}
    except WorkerHang as e:
        return {"status": "HANG", "detail": str(e)}
    except WorkerFailure as e:
        return {"status": "CRASH", "detail": str(e)}


class ModeRun:
    def __init__(self, name, session, workdir, sources, prepared):
        self.name, self.session, self.sources, self.prepared = name, session, sources, prepared
        self.dir = Path(workdir) / name
        self._saved = self._again = self._validation = self._cross = self._checks = None

    @property
    def outdir(self):
        return self.dir / "out"

    def saved(self):
        """{source id: {"status", "detail", "out"}}; status is ok, FAIL, HANG or CRASH, and "out" is the saved file when ok."""
        if self._saved is None:
            self.outdir.mkdir(parents=True, exist_ok=True)
            res = {}
            for source in self.sources:
                dst = str(self.outdir / documents.out_name(source))
                try:
                    src = self.prepared.file(source)
                except WorkerFailure as e:
                    status = "FAIL" if isinstance(e, WorkerError) else "HANG" if isinstance(e, WorkerHang) else "CRASH"
                    res[source.id] = {"status": status, "detail": str(e), "out": None}
                    continue
                r = save_one(self.session, src, dst)
                r["out"] = dst if r["status"] == "ok" else None
                res[source.id] = r
            self._saved = res
        return self._saved

    def saved_again(self):
        """The same conversions as saved(), a second time, into out2/: {source id: {"status", "detail", "out"}}."""
        if self._again is None:
            self.saved()
            out2 = self.dir / "out2"
            out2.mkdir(parents=True, exist_ok=True)
            res = {}
            for source in self.sources:
                if self._saved[source.id]["out"] is None:
                    res[source.id] = {"status": "skipped", "detail": "the first save failed", "out": None}
                    continue
                dst = str(out2 / documents.out_name(source))
                r = save_one(self.session, self.prepared.file(source), dst)
                r["out"] = dst if r["status"] == "ok" else None
                res[source.id] = r
            self._again = res
        return self._again

    def validation(self):
        """{"val", "pkg", "doctype", "ids", "formulas"}: each checker's result over the output directory; the Java ones are
        None when java, jing or the schemas are missing."""
        if self._validation is None:
            self.saved()
            res = {"ids": idcheck.check(str(self.outdir)), "doctype": doctypecheck.check(str(self.outdir)),
                   "formulas": formulacheck.check(str(self.outdir)), "val": None, "pkg": None}
            jing, schemas = tools.jing_command(), tools.schema_dir()
            if jing and schemas:
                res["val"] = validate.check(str(self.outdir), str(self.dir / "w-validate"), jing, schemas)
                res["pkg"] = pkgcheck.check(str(self.outdir), str(self.dir / "w-pkg"), jing, schemas)
            self._validation = res
        return self._validation

    def crosscheck(self):
        """The odfvalidator comparison with jing over the output directory (aootest.checkers.crosscheck), or None when java,
        the odfvalidator jar, jing or the schemas are missing."""
        if self._cross is None:
            val = self.validation()["val"]
            jar, java, schemas = tools.odfvalidator(), tools.java(), tools.schema_dir()
            if val is None or not (jar and java and schemas):
                return None
            self._cross = crosscheck.check(str(self.outdir), val, str(self.dir / "w-cross"), jar, str(schemas), java)
        return self._cross

    def reload_checks(self):
        """{source id: {check name: value} | {"__error__": text}} for the saved copies of the sources that have reload checks."""
        if self._checks is None:
            saved, res = self.saved(), {}
            for source in self.sources:
                if not source.checks or saved[source.id]["out"] is None:
                    continue
                checks = [{"name": c["name"], "expr": c["expr"]} for f in source.checks for c in documents.load_checks(f)]
                try:
                    res[source.id] = self.session.call("roundtrip", {"path": saved[source.id]["out"], "checks": checks}, timeout=300)
                except WorkerFailure as e:
                    res[source.id] = {"__error__": "%s: %s" % (type(e).__name__, e)}
            self._checks = res
        return self._checks
