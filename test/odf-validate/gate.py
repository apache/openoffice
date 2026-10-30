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

# usage: gate.py <tag> [--stages s,...] [--modes ext,strict] [--fixture-modes v00,v10,v11,ext,strict] [--prev DIR] [--jobs FILE] [--limit N] [--port P]
#                [--build mod,...] [--libs lib.dylib,...] [--stall SECONDS] [--force] [--crosscheck]
# The corpus regression gate as one resumable run in runs/gate-<tag>/. Stages, in order:
#   build     rebuild the modules (--build) and deliver them to the solver
#   refresh   copy the rebuilt libraries (--libs) from the solver into the app copy and verify them
#   corpus    save the job list in ODF 1.2 Extended (ext) and strict (strict) mode with the app copy
#   validate  validate.py and the checkers on the saved packages; kinds compared with baseline/ and --prev;
#             a checker that did not run, a shall finding of idcheck/pkgcheck/doctypecheck, or more formula findings than --prev fails
#   dump      reload every saved package and dump a fixed property set (dump7.py), with a stall watchdog
#   diff      dumps (diff7.py) and packages (cmp7.py) against --prev; differences not in gate-volatile.tsv are unexplained
#   fixtures  gate-fixtures.tsv saved in five version modes, reload checks, validation; results against --prev
# --jobs freezes the inputs (source<TAB>output lines; outputs are rewritten); default: --prev's list, else make-jobs.sh.
# Exit status 0 only if every stage that ran passed.
import argparse, collections, json, os, re, shutil, socket, subprocess, sys, threading, time

D = os.path.dirname(os.path.abspath(__file__))
PY = [sys.executable, "-I"]
STAGES = ["build", "refresh", "corpus", "validate", "dump", "diff", "fixtures"]
FIX_MODES = ["v00", "v10", "v11", "ext", "strict"]
DEFAULT_LIBS = "libxo.dylib,libsw.dylib,libsc.dylib,libsd.dylib,libsfx.dylib"


def listening(port):
    with socket.socket() as s:
        s.settimeout(0.5)
        return s.connect_ex(("localhost", port)) == 0


def kill_office(profile):
    subprocess.run(["pkill", "-f", "profile_" + profile], capture_output=True)
    time.sleep(2)


def run(cmd, log=None, timeout=None, cwd=None, env=None):
    """Runs cmd, appending its output to log; returns the exit code (124 on timeout)."""
    out = open(log, "a") if log else subprocess.DEVNULL
    try:
        return subprocess.run(cmd, stdout=out, stderr=subprocess.STDOUT, timeout=timeout, cwd=cwd, env=env).returncode
    except subprocess.TimeoutExpired:
        return 124
    finally:
        if log:
            out.close()


def read_jobs(path):
    return [tuple(l.rstrip("\n").split("\t")) for l in open(path) if l.strip()]


def rewrite_jobs(jobs, outdir):
    """Same sources, outputs moved into outdir."""
    return [(src, os.path.join(outdir, os.path.basename(out))) for src, out in jobs]


def read_volatile(path=os.path.join(D, "gate-volatile.tsv")):
    return {l.split("\t")[0]: l.rstrip("\n").split("\t")[1] for l in open(path) if l.strip() and not l.startswith("#")}


def classify(diff, volatile):
    """diff: diff7.py's result; returns (identical, volatile differing, unexplained differing)."""
    differing = sorted(k for k, v in diff.items() if v["ndiff"])
    vol = [k for k in differing if k in volatile]
    return len(diff) - len(differing), vol, [k for k in differing if k not in volatile]


def run_with_stall(cmd, progress, stall, poll=2.0, log=None):
    """Runs cmd until it exits or progress() has not changed for `stall` seconds (then kills it).
    Returns ('exit', returncode) or ('stalled', None)."""
    out = open(log, "a") if log else subprocess.DEVNULL
    p = subprocess.Popen(cmd, stdout=out, stderr=subprocess.STDOUT)
    last, since = progress(), time.time()
    try:
        while p.poll() is None:
            time.sleep(poll)
            now = progress()
            if now != last:
                last, since = now, time.time()
            elif time.time() - since > stall:
                p.kill(); p.wait()
                return "stalled", None
        return "exit", p.returncode
    finally:
        if log:
            out.close()


def bridge_dead(path):
    """True for a dump that only records that the office died under an earlier document."""
    try:
        d = json.load(open(path))
    except Exception:
        return False
    return isinstance(d, dict) and (str(d.get("error", "")).startswith("Binary URP bridge") or str(d.get("body_error", "")).startswith("Binary URP bridge"))


def dump_dir(src, dest, profile, port, stall, log, tool=None, poll=2.0):
    """Reloads every package in src into dest/*.json. A document that stalls the office or kills it gets an error
    marker only after `stall` seconds without progress, never because the whole batch took long.
    tool(port, dest, files) -> argv replaces dump7.py under the office (for tests); then no office is started."""
    os.makedirs(dest, exist_ok=True)
    files = sorted(os.path.join(src, f) for f in os.listdir(src) if not f.startswith("."))
    count = lambda: len([f for f in os.listdir(dest) if f.endswith(".json")])
    stalls = 0
    retried = 0
    for attempt in range(4):
        while count() < len(files):
            if tool is None and not listening(port):
                if run([os.path.join(D, "start.sh"), profile, str(port)], log) != 0:
                    return {"error": "office did not start", "stalls": stalls}
            before = count()
            marker = os.path.join(dest, ".current")
            argv = tool(port, dest, files) if tool else [os.path.join(D, "runpy.sh"), os.path.join(D, "dump7.py"), str(port), dest] + files
            how, rc = run_with_stall(argv, lambda: (count(), os.path.getmtime(marker) if os.path.exists(marker) else 0), stall, poll, log=log)
            if how == "stalled" or rc != 0:
                cur = open(marker).read().strip() if os.path.exists(marker) else ""
                if cur and not os.path.exists(os.path.join(dest, os.path.basename(cur) + ".json")):
                    reason = ("HANG: no progress for %ds" % stall) if how == "stalled" else "CRASH: rc=%s" % rc
                    json.dump({"error": reason}, open(os.path.join(dest, os.path.basename(cur) + ".json"), "w"))
                    stalls += 1
                if tool is None:
                    kill_office(profile)
            elif count() == before:
                return {"error": "dump7.py made no progress", "stalls": stalls}
        # dump7.py records "bridge disposed" for every document after an office crash; those are not dumps, so redo them
        gone = [f for f in os.listdir(dest) if f.endswith(".json") and bridge_dead(os.path.join(dest, f))]
        if not gone or attempt == 3:
            break
        if tool is None:
            kill_office(profile)
        for f in gone:
            os.remove(os.path.join(dest, f))
        retried += len(gone)
    if tool is None:
        kill_office(profile)
    res = {"documents": count(), "stalls": stalls}
    if retried:
        res["bridge_retried"] = retried
    return res


def count_done(done):
    ok = fail = 0
    if os.path.exists(done):
        for l in open(done):
            ok += l.rstrip("\n").endswith("\tOK")
            fail += "\tFAIL" in l
    return ok, fail


class Gate:
    def __init__(self, a):
        self.a = a
        self.run = os.path.join(D, "runs", "gate-" + a.tag)
        os.makedirs(self.run, exist_ok=True)
        self.res_path = os.path.join(self.run, "results.json")
        self.res = json.load(open(self.res_path)) if os.path.exists(self.res_path) else {}
        self.modes = a.modes.split(",")
        self.fix_modes = a.fixture_modes.split(",")
        self.prev = os.path.abspath(a.prev) if a.prev else None

    def save(self):
        json.dump(self.res, open(self.res_path, "w"), indent=1)

    def log(self, name):
        return os.path.join(self.run, name + ".log")

    def prev_path(self, *candidates):
        for c in candidates:
            p = os.path.join(self.prev, c) if self.prev else None
            if p and os.path.exists(p):
                return p
        return None

    # ---- stages
    def build(self):
        mods = [m for m in self.a.build.split(",") if m]
        if not mods:
            return {"skipped": "no --build modules"}
        env = os.environ.get("ODFV_BUILD_ENV") or sorted(__import__("glob").glob(os.path.join(D, "..", "..", "main", "*Env.Set.sh")))[-1]
        res = {}
        for m in mods:
            cmd = "cd %s/../../main && source %s && cd %s && perl $SOLARENV/bin/build.pl && perl $SOLARENV/bin/deliver.pl" % (D, env, m)
            res[m] = run(["/bin/zsh", "-c", cmd], self.log("build_" + m))
        return {"ok": all(v == 0 for v in res.values()), "rc": res}

    def refresh(self):
        libs = [l for l in self.a.libs.split(",") if l]
        out = subprocess.run([os.path.join(D, "refresh-app.sh")] + libs, capture_output=True, text=True)
        return {"ok": out.returncode == 0, "output": out.stdout.strip().splitlines()}

    def jobs(self):
        if self.a.jobs:
            return read_jobs(self.a.jobs)
        pj = self.prev_path("jobs.tsv", "ext/jobs.tsv")
        if pj:
            return read_jobs(pj)
        out = subprocess.run([os.path.join(D, "make-jobs.sh"), "OUT"], capture_output=True, text=True, check=True).stdout
        return [tuple(l.split("\t")) for l in out.splitlines()]

    def corpus(self):
        jobs = self.jobs()
        if self.a.limit:
            jobs = jobs[:self.a.limit]
        with open(os.path.join(self.run, "jobs.tsv"), "w") as f:
            f.writelines("%s\t%s\n" % j for j in jobs)
        procs, res = {}, {}
        for i, mode in enumerate(self.modes):
            md = os.path.join(self.run, mode)
            os.makedirs(os.path.join(md, "out"), exist_ok=True)
            with open(os.path.join(md, "jobs.tsv"), "w") as f:
                f.writelines("%s\t%s\n" % j for j in rewrite_jobs(jobs, os.path.join(md, "out")))
            open(os.path.join(md, "done.tsv"), "a").close()
            procs[mode] = subprocess.Popen([os.path.join(D, "drive.sh"), mode, str(self.a.port + i), os.path.join(md, "jobs.tsv"), os.path.join(md, "done.tsv")])
        for mode, p in procs.items():
            p.wait()
            md = os.path.join(self.run, mode)
            ok, fail = count_done(os.path.join(md, "done.tsv"))
            res[mode] = {"rc": p.returncode, "ok": ok, "fail": fail, "jobs": len(jobs), "packages": len(os.listdir(os.path.join(md, "out")))}
            pd = self.prev_path(mode + "/done.tsv")
            if pd and not self.a.limit:
                res[mode]["prev_ok_fail"] = count_done(pd)
        res["ok"] = all(r["rc"] == 0 and r["ok"] + r["fail"] == r["jobs"] and r["ok"] > 0 for r in res.values() if isinstance(r, dict))
        if not self.a.limit:
            res["ok"] = res["ok"] and all(tuple(r["prev_ok_fail"]) == (r["ok"], r["fail"]) for r in res.values() if isinstance(r, dict) and "prev_ok_fail" in r)
        return res

    def prev_validate(self, *keys):
        """The validate-result dict of --prev at keys, or None when it has none."""
        pf = self.prev_path("results.json")
        d = json.load(open(pf)) if pf else {}
        for k in keys:
            d = d.get(k) if isinstance(d, dict) else None
        return d if isinstance(d, dict) else None

    def shall_problems(self, r, prev):
        """Checker results that must fail the gate: a checker that did not run, a Part 1/3 finding, a formula finding that grew."""
        bad = []
        for n in ("ids", "pkg", "doctype", "formulas"):
            if r.get("shall_" + n) is None:
                bad.append("%s checker did not run" % n)
        for n in ("ids", "pkg", "doctype"):
            if r.get("shall_" + n):
                bad.append("%d shall findings in %s" % (r["shall_" + n], n))
        pf = (prev or {}).get("shall_formulas")
        if not self.a.limit and r.get("shall_formulas") is not None and pf is not None and r["shall_formulas"] > pf:
            bad.append("shall_formulas %d > %d in --prev" % (r["shall_formulas"], pf))
        return bad

    def validate_dir(self, md, out, kinds_base, label):
        """validate.py + report.py + the checkers on out/; returns a result dict."""
        r = {}
        log = os.path.join(md, "validate.log")
        run(PY + [os.path.join(D, "validate.py"), out, os.path.join(md, "val.json")], log)
        val = json.load(open(os.path.join(md, "val.json")))
        r["errors"] = {v: len(e) for v, e in val["errors"].items()}
        r["packages"] = len(val["packages"])
        for v in ("12", "14", "12x", "13", "13x"):
            kp = os.path.join(md, "kinds%s.json" % ("" if v == "12" else v))
            run(PY + [os.path.join(D, "report.py"), os.path.join(md, "val.json"), kp, v], os.path.join(md, "report%s.log" % v))
            r["kinds" + v] = len(json.load(open(kp)))
        for name, script, pat in (("ids", "idcheck.py", r"(\d+) errors"), ("pkg", "pkgcheck.py", r"(\d+) shall"),
                                  ("doctype", "doctypecheck.py", r"(\d+) shall"), ("formulas", "formulacheck.py", r"(\d+) shall")):
            o = subprocess.run(PY + [os.path.join(D, script), out, os.path.join(md, name + ".json")], capture_output=True, text=True)
            m = re.search(pat, o.stdout)
            r["shall_" + name] = int(m.group(1)) if m and o.returncode == 0 else None
        base = kinds_base
        if base and os.path.exists(base):
            r["vs_baseline"] = run(PY + [os.path.join(D, "compare.py"), base, os.path.join(md, "kinds.json")], os.path.join(md, "compare_base.log"))
        return r

    def validate(self):
        res = {}
        for mode in self.modes:
            md = os.path.join(self.run, mode)
            r = self.validate_dir(md, os.path.join(md, "out"), os.path.join(D, "baseline", mode + ".kinds.json"), mode)
            pk = self.prev_path(mode + "/kinds.json")
            r["problems"] = self.shall_problems(r, self.prev_validate("validate", mode))
            if not self.a.limit and not pk and "vs_baseline" not in r:
                r["problems"].append("no baseline/%s.kinds.json and no --prev kinds to compare with" % mode)
            if pk and not self.a.limit:
                r["vs_prev"] = run(PY + [os.path.join(D, "compare.py"), pk, os.path.join(md, "kinds.json")], os.path.join(md, "compare_prev.log"))
                r["vs_prev_reverse"] = run(PY + [os.path.join(D, "compare.py"), os.path.join(md, "kinds.json"), pk], os.path.join(md, "compare_prev_rev.log"))
            res[mode] = r
        res["ok"] = all(r.get("vs_baseline", 0) == 0 and r.get("vs_prev", 0) == 0 and r.get("vs_prev_reverse", 0) == 0 and not r["problems"] for r in res.values())
        return res

    def dump(self):
        res, threads = {}, []
        for i, mode in enumerate(self.modes):
            def one(mode=mode, i=i):
                md = os.path.join(self.run, mode)
                res[mode] = dump_dir(os.path.join(md, "out"), os.path.join(md, "dump"), "gate_dump_%s_%s" % (self.a.tag, mode),
                                     self.a.port + 10 + i, self.a.stall, self.log("dump_" + mode))
            t = threading.Thread(target=one); t.start(); threads.append(t)
        for t in threads:
            t.join()
        res["ok"] = all("error" not in r for r in res.values())
        for mode in self.modes:
            md = os.path.join(self.run, mode, "dump")
            res[mode]["bridge_errors"] = sorted(f for f in os.listdir(md) if f.endswith(".json") and bridge_dead(os.path.join(md, f)))
            res["ok"] = res["ok"] and not res[mode]["bridge_errors"]
            res[mode]["hang_markers"] = sorted(f for f in os.listdir(md) if f.endswith(".json") and "error" in json.load(open(os.path.join(md, f))) and
                                               str(json.load(open(os.path.join(md, f)))["error"]).startswith(("HANG", "CRASH")))
        return res

    def diff(self):
        if not self.prev:
            return {"skipped": "no --prev"}
        volatile, res = read_volatile(), {}
        for mode in self.modes:
            md = os.path.join(self.run, mode)
            old = self.prev_path(mode + "/dump", "dump_" + mode)
            oldrun = self.prev_path(mode + "/out")
            r = {}
            if old:
                # compare only the documents this run saved (a --limit run saves fewer)
                sub = os.path.join(md, "prev-dump")
                shutil.rmtree(sub, ignore_errors=True); os.makedirs(sub)
                for f in os.listdir(os.path.join(md, "dump")):
                    if f.endswith(".json") and os.path.exists(os.path.join(old, f)):
                        os.symlink(os.path.join(old, f), os.path.join(sub, f))
                dj = os.path.join(md, "diff_dump.json")
                run(PY + [os.path.join(D, "diff7.py"), sub, os.path.join(md, "dump"), dj], os.path.join(md, "diff_dump.log"))
                same, vol, unexplained = classify(json.load(open(dj)), volatile)
                r["dump"] = {"compared": len(os.listdir(sub)), "identical": same, "volatile": vol, "unexplained": unexplained}
            if oldrun:
                stage = os.path.join(self.run, "cmp-" + mode)
                shutil.rmtree(stage, ignore_errors=True); os.makedirs(os.path.join(stage, "out"))
                for f in os.listdir(os.path.join(md, "out")):
                    if os.path.exists(os.path.join(oldrun, f)):
                        os.symlink(os.path.join(oldrun, f), os.path.join(stage, "out", f))
                # cmp7 reads <run>/out for both sides: the old side is the subset linked above, the new side this run
                cj = os.path.join(md, "cmp7.json")
                cp = subprocess.run(PY + [os.path.join(D, "cmp7.py"), stage, md, mode, cj], capture_output=True, text=True)
                open(os.path.join(md, "cmp7.log"), "w").write(cp.stdout + cp.stderr)
                m = re.search(r"differing after env masks: (\d+); differing after intended-edit normalisation: (\d+)", cp.stdout)
                if m:
                    cmp = json.load(open(cj))
                    differing = sorted(k for k, v in cmp.items() if v.get("norm_diff_streams") or v.get("names_diff") or v.get("missing_in_new"))
                    r["packages"] = {"after_masks": int(m.group(1)), "volatile": [k for k in differing if k in volatile],
                                     "unexplained": [k for k in differing if k not in volatile]}
                else:
                    r["packages"] = {"error": (cp.stderr or cp.stdout)[-200:], "unexplained": ["cmp7.py failed"]}
            res[mode] = r
        res["ok"] = all(("dump" not in r or not r["dump"]["unexplained"]) and ("packages" not in r or not r["packages"]["unexplained"]) for r in res.values())
        return res

    def fixtures(self):
        entries = [l.rstrip("\n").split("\t") for l in open(os.path.join(D, "gate-fixtures.tsv")) if l.strip() and not l.startswith("#")]
        def src_path(s):
            return os.path.join(D, "..", "..", "main", s[4:]) if s.startswith("EXT:") else os.path.join(D, "fixtures", s)
        def out_name(s):
            b = os.path.splitext(os.path.basename(s))[0]
            ext = {"doc": "odt", "docx": "odt", "odt": "odt", "sxw": "odt", "ods": "ods", "sxc": "ods", "odg": "odg", "sxd": "odg", "odp": "odp", "sxi": "odp"}[s.rsplit(".", 1)[1]]
            return ("pf13-" if s.startswith("page-fill-1_3/") else "") + b + "." + ext
        sources = sorted({e[0] for e in entries})
        res, threads = {}, []
        for i, mode in enumerate(self.fix_modes):
            def one(mode=mode, i=i):
                md = os.path.join(self.run, "fixtures", mode)
                out = os.path.join(md, "out")
                os.makedirs(out, exist_ok=True)
                missing = [s for s in sources if not os.path.isfile(src_path(s))]
                with open(os.path.join(md, "jobs.tsv"), "w") as f:
                    f.writelines("%s\t%s\n" % (os.path.abspath(src_path(s)), os.path.join(out, out_name(s))) for s in sources if s not in missing)
                open(os.path.join(md, "done.tsv"), "a").close()
                rc = run([os.path.join(D, "drive.sh"), mode, str(self.a.port + 20 + i), os.path.join(md, "jobs.tsv"), os.path.join(md, "done.tsv")])
                ok, fail = count_done(os.path.join(md, "done.tsv"))
                r = {"drive_rc": rc, "saved": ok, "failed": fail, "missing": missing, "checks": {}}
                prof, port = "gate_rt_%s_%s" % (self.a.tag, mode), self.a.port + 30 + i
                run([os.path.join(D, "start.sh"), prof, str(port)], os.path.join(md, "rt.log"))
                for s, ck in (e for e in entries if e[1] != "-"):
                    doc = os.path.join(out, out_name(s))
                    key = "%s|%s" % (os.path.basename(s), ck)
                    if not os.path.isfile(doc):
                        r["checks"][key] = {"error": "no saved copy"}; continue
                    p = subprocess.run([os.path.join(D, "runpy.sh"), os.path.join(D, "roundtrip.py"), str(port), doc, os.path.join(D, "fixtures", ck)], capture_output=True, text=True, timeout=600)
                    m = re.search(r"roundtrip: (\d+)/(\d+) match", p.stdout)
                    r["checks"][key] = {"mismatches": sorted(re.findall(r"^MISMATCH (\S+?):", p.stdout, re.M)), "match": int(m.group(1)) if m else None, "total": int(m.group(2)) if m else None}
                kill_office(prof)
                r["validate"] = self.validate_dir(md, out, None, mode)
                r["problems"] = self.shall_problems(r["validate"], self.prev_validate("fixtures", mode, "validate"))
                res[mode] = r
            t = threading.Thread(target=one); t.start(); threads.append(t)
        for t in threads:
            t.join()
        pf = self.prev_path("results.json")
        changed = []
        if pf:
            prev = json.load(open(pf)).get("fixtures", {})
            for mode in self.fix_modes:
                for k, c in res.get(mode, {}).get("checks", {}).items():
                    pc = prev.get(mode, {}).get("checks", {}).get(k)
                    if pc is not None and pc != c:
                        changed.append("%s %s" % (mode, k))
        res["changed_vs_prev"] = changed if pf else "no previous fixtures results"
        res["ok"] = all(res[m]["drive_rc"] == 0 and res[m]["failed"] == 0 and not res[m]["missing"] and
                        all("error" not in c and c["total"] for c in res[m]["checks"].values()) and not res[m]["problems"] for m in self.fix_modes) and not changed
        return res


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("tag")
    ap.add_argument("--stages", default="refresh,corpus,validate,dump,diff,fixtures")
    ap.add_argument("--modes", default="ext,strict")
    ap.add_argument("--fixture-modes", default=",".join(FIX_MODES))
    ap.add_argument("--prev")
    ap.add_argument("--jobs")
    ap.add_argument("--limit", type=int, default=0)
    ap.add_argument("--port", type=int, default=2013)
    ap.add_argument("--build", default="")
    ap.add_argument("--libs", default=DEFAULT_LIBS)
    ap.add_argument("--stall", type=int, default=300)
    ap.add_argument("--force", action="store_true")
    a = ap.parse_args()
    g = Gate(a)
    stages = [s for s in STAGES if s in a.stages.split(",") or (s == "build" and a.build)]
    failed = []
    for s in stages:
        if s in g.res and not a.force:
            print("%-9s already done (--force to rerun)" % s)
        else:
            t0 = time.time()
            g.res[s] = getattr(g, s)()
            g.res[s]["seconds"] = int(time.time() - t0)
            g.save()
        ok = g.res[s].get("ok", True)
        print("%-9s %s  %s" % (s, "PASS" if ok else "FAIL", json.dumps({k: v for k, v in g.res[s].items() if k != "ok"})[:300]))
        if not ok:
            failed.append(s)
    print("gate %s: %s" % (a.tag, "PASS" if not failed else "FAIL in " + ", ".join(failed)))
    sys.exit(1 if failed else 0)


if __name__ == "__main__":
    main()
