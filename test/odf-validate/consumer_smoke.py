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

# usage: consumer_smoke.py <tag> [--port N] [--stall SECONDS] [--dir DIR]
# Import smoke test for the ODF 1.4 additions: reloads every package of fixtures/t14/consumer (made by make-consumer.py)
# with the office of the app copy, dumps every property of the objects it touches (dump14.py, or dump7.py with --dumper; the gate's stall watchdog) and compares
# every package with its '-base' twin (the same document without the construct, diff7.py). Reports per package: loaded or
# not (an error marker in the dump), and the number of differing dump keys against the twin. A package without a twin
# is only checked for loading. Results in runs/consumer-<tag>/ (summary in summary.json).
import argparse, importlib.util, json, os, shutil, subprocess, sys

D = os.path.dirname(os.path.abspath(__file__))
spec = importlib.util.spec_from_file_location("gate", D + "/gate.py")
gate = importlib.util.module_from_spec(spec)
spec.loader.exec_module(gate)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("tag")
    ap.add_argument("--port", type=int, default=2950)
    ap.add_argument("--stall", type=int, default=120)
    ap.add_argument("--dir", default=os.path.join(D, "fixtures", "t14", "consumer"))
    ap.add_argument("--dumper", default="dump14.py", help="dump14.py (every property of the touched objects) or dump7.py (the gate's fixed set)")
    a = ap.parse_args()
    root = os.path.join(D, "runs", "consumer-" + a.tag)
    shutil.rmtree(root, ignore_errors=True)
    os.makedirs(root)
    dest = os.path.join(root, "dump")
    log = os.path.join(root, "dump.log")
    r = gate.dump_dir(a.dir, dest, "consumer_" + a.tag, a.port, a.stall, log, dumper=a.dumper)
    gate.kill_office("consumer_" + a.tag)
    names = sorted(f[:-5] for f in os.listdir(dest) if f.endswith(".json"))
    summary = {}
    for n in names:
        d = json.load(open(os.path.join(dest, n + ".json")))
        summary[n] = {"loaded": "error" not in d or d.get("error") is None, "error": d.get("error"), "body_error": d.get("body_error")}
    base = {n[:-5]: n for n in names if os.path.splitext(n)[0].endswith("-base")}
    for n in names:
        stem, ext = os.path.splitext(n)
        twin = stem + "-base" + ext
        if twin not in names:
            continue
        pa, pb = os.path.join(root, "pairA"), os.path.join(root, "pairB")
        for p in (pa, pb):
            os.makedirs(p, exist_ok=True)
        shutil.copy(os.path.join(dest, n + ".json"), os.path.join(pa, n + ".json"))
        shutil.copy(os.path.join(dest, twin + ".json"), os.path.join(pb, n + ".json"))
    if os.path.isdir(os.path.join(root, "pairA")):
        out = os.path.join(root, "pairs.json")
        subprocess.run([sys.executable, "-I", os.path.join(D, "diff7.py"), os.path.join(root, "pairA"), os.path.join(root, "pairB"), out],
                       capture_output=True, check=True)
        for n, v in json.load(open(out)).items():
            summary[n]["ndiff_vs_base"] = v["ndiff"]
            summary[n]["diff_keys"] = [x[0] for x in v["diffs"][:8]]
    json.dump(summary, open(os.path.join(root, "summary.json"), "w"), indent=1)
    print("dump:", r)
    for n in sorted(summary):
        s = summary[n]
        print("%-36s %-6s %s" % (n, "loaded" if s["loaded"] else "FAILED", ("diff vs base: %d %s" % (s["ndiff_vs_base"], s["diff_keys"][:3])) if "ndiff_vs_base" in s else ""))
    bad = [n for n, s in summary.items() if not s["loaded"]]
    print("not loaded:", bad)
    sys.exit(1 if bad else 0)


main()
