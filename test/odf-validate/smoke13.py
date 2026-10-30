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

# usage: smoke13.py <tag> [--root DIR] [--port N] [--stall SECONDS] [--known FILE]
# Import smoke test for ODF 1.3: reloads every document of a foreign corpus (tools13/make-foreign-corpus.py) saved in
# 1.3 and in 1.3 extended mode next to its 1.2 twin (same input, same producer), dumps a fixed property set of each
# (dump7.py, with the gate's stall watchdog) and compares the dumps of every twin pair (diff7.py).
# Fails on a document that hangs or kills the office, on a twin pair that differs without an entry in --known
# (TSV: name<TAB>reason, like gate-volatile.tsv), and when the built-in controls misbehave: an unchanged copy must
# compare equal and a copy with one changed word must compare different.
import argparse, collections, importlib.util, json, os, re, shutil, subprocess, sys, zipfile

D = os.path.dirname(os.path.abspath(__file__))
spec = importlib.util.spec_from_file_location("gate", D + "/gate.py")
gate = importlib.util.module_from_spec(spec)
spec.loader.exec_module(gate)
PAIRS = (("v12", "v13"), ("v12ext", "v13ext"))


def mutate(src, dst):
    """Copy of an ODF text package with one word appended to the first paragraph that has text; False if none."""
    zi, zo = zipfile.ZipFile(src), zipfile.ZipFile(dst, "w")
    done = False
    for it in zi.infolist():
        d = zi.read(it.filename)
        if it.filename == "content.xml" and not done:
            t = d.decode("utf-8")
            t2, n = re.subn(r">([A-Za-z][^<>]{2,})</text:p>", r">\1 mutated</text:p>", t, count=1)
            if n:
                d, done = t2.encode("utf-8"), True
        zo.writestr(it, d, compress_type=it.compress_type)
    zo.close()
    return done


def dumps(src, dest, profile, port, stall, log):
    r = gate.dump_dir(src, dest, profile, port, stall, log)
    if "error" in r:
        sys.exit("dump of %s failed: %s" % (src, r))
    return r


def diff(old, new, out):
    subprocess.run([sys.executable, "-I", os.path.join(D, "diff7.py"), old, new, out], capture_output=True, check=True)
    return json.load(open(out))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("tag")
    ap.add_argument("--root", default=os.path.join(D, "runs", "phase3", "foreign"))
    ap.add_argument("--port", type=int, default=2794)
    ap.add_argument("--stall", type=int, default=300)
    ap.add_argument("--known")
    ap.add_argument("--resume", action="store_true", help="reuse the dumps already in the run directory of this tag")
    a = ap.parse_args()
    run = os.path.join(D, "runs", "phase3", "smoke-" + a.tag)
    if os.path.exists(run) and not a.resume:
        sys.exit("%s exists, use a new tag or --resume" % run)
    os.makedirs(run, exist_ok=True)
    log = os.path.join(run, "dump.log")
    known = {}
    if a.known:
        known = {l.split("\t")[0]: l.rstrip("\n").split("\t")[1] for l in open(a.known) if l.strip() and not l.startswith("#")}
    problems, report = [], {}
    stats = {}
    for mode in sorted({m for p in PAIRS for m in p}):
        src = os.path.join(a.root, mode)
        if not os.path.isdir(src):
            sys.exit("no %s" % src)
        stats[mode] = dumps(src, os.path.join(run, "dump", mode), "smoke13", a.port, a.stall, log)
    for mode, r in stats.items():
        errs = []
        for f in os.listdir(os.path.join(run, "dump", mode)):
            if f.endswith(".json"):
                d = json.load(open(os.path.join(run, "dump", mode, f)))
                if isinstance(d, dict) and (d.get("error") or d.get("body_error")):
                    errs.append((f[:-5], d.get("error") or d.get("body_error")))
        report[mode] = {"documents": r["documents"], "stalls": r["stalls"], "errors": errs}
        problems += ["%s: %s: %s" % (mode, n, e) for n, e in errs]
    # controls: pick a text document that dumped with characters in it
    base = None
    for f in sorted(x for x in os.listdir(os.path.join(run, "dump", "v12")) if x.endswith(".json")):
        d = json.load(open(os.path.join(run, "dump", "v12", f)))
        if d.get("type") == "writer" and (d.get("body") or {}).get("chars", 0) > 20 and f.endswith(".odt.json"):
            base = os.path.join(a.root, "v12", f[:-5])
            break
    ctl = {"old": os.path.join(run, "ctl", "old"), "new": os.path.join(run, "ctl", "new")}
    for p in ctl.values():
        os.makedirs(p, exist_ok=True)
    if base is None:
        problems.append("controls: no text document with characters in the corpus")
    else:
        shutil.copy2(base, ctl["old"] + "/ctl-same.odt")
        shutil.copy2(base, ctl["new"] + "/ctl-same.odt")
        shutil.copy2(base, ctl["old"] + "/ctl-mut.odt")
        if not mutate(base, ctl["new"] + "/ctl-mut.odt"):
            problems.append("controls: could not change a word in " + base)
        for k in ctl:
            dumps(ctl[k], os.path.join(run, "dump", "ctl-" + k), "smoke13", a.port, a.stall, log)
        c = diff(os.path.join(run, "dump", "ctl-old"), os.path.join(run, "dump", "ctl-new"), os.path.join(run, "diff-ctl.json"))
        if c["ctl-same.odt"]["ndiff"] != 0:
            problems.append("controls: an unchanged copy compares different (%d)" % c["ctl-same.odt"]["ndiff"])
        if c["ctl-mut.odt"]["ndiff"] == 0:
            problems.append("controls: a copy with a changed word compares equal (the comparison cannot fail)")
        report["controls"] = {"base": os.path.basename(base), "same_ndiff": c["ctl-same.odt"]["ndiff"], "mutated_ndiff": c["ctl-mut.odt"]["ndiff"]}
    for old, new in PAIRS:
        dd = diff(os.path.join(run, "dump", old), os.path.join(run, "dump", new), os.path.join(run, "diff-%s-%s.json" % (old, new)))
        ident = sum(1 for v in dd.values() if not v["ndiff"])
        differing = sorted(k for k, v in dd.items() if v["ndiff"])
        unexplained = [k for k in differing if k not in known]
        report["%s vs %s" % (old, new)] = {"compared": len(dd), "identical": ident, "different": len(differing), "unexplained": unexplained}
        problems += ["%s vs %s: %s differs (%d paths, first: %s)" % (old, new, k, dd[k]["ndiff"], dd[k]["diffs"][0][:3]) for k in unexplained]
    json.dump({"report": report, "problems": problems}, open(os.path.join(run, "result.json"), "w"), indent=1, default=str)
    for k, v in report.items():
        print(k, {x: (y if not isinstance(y, list) else len(y)) for x, y in v.items()})
    for p in problems:
        print("PROBLEM", p)
    print("smoke13:", "FAIL" if problems else "PASS")
    sys.exit(1 if problems else 0)


main()
