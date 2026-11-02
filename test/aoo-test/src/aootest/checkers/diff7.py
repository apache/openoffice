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

# usage: diff7.py <old_dump_dir> <new_dump_dir> <out.json>
# Flattens the per-document reload dumps and lists every differing path with old/new value.
import sys, os, json, collections
IGN = ("props.creation_date", "props.gen", "props.edit_cycles")   # none expected to differ; creation_date is kept from the input
def flat(x, p=""):
    if isinstance(x, dict):
        for k in sorted(x): yield from flat(x[k], p + "." + k if p else k)
    elif isinstance(x, list):
        if len(x) > 40 and all(not isinstance(i, (dict, list)) for i in x): yield p, x
        else:
            for i, e in enumerate(x): yield from flat(e, f"{p}[{i}]")
    else: yield p, x


def diff_dirs(old, new):
    """Returns {document: {type, errors, ndiff, diffs, nprops}} for two dump directories."""
    res = {}
    names = sorted(f for f in os.listdir(old) if f.endswith(".json"))
    for f in names:
        a = json.load(open(os.path.join(old, f)))
        b = json.load(open(os.path.join(new, f))) if os.path.exists(os.path.join(new, f)) else {"error": "MISSING"}
        fa, fb = dict(flat(a)), dict(flat(b))
        d = [(k, fa.get(k, "<absent>"), fb.get(k, "<absent>")) for k in sorted(set(fa) | set(fb)) if fa.get(k, "<absent>") != fb.get(k, "<absent>")]
        res[f[:-5]] = {"type": a.get("type") or b.get("type") or ("error" if "error" in a else "?"), "errors": [a.get("error"), b.get("error"), a.get("body_error"), b.get("body_error")], "ndiff": len(d), "diffs": d, "nprops": len(fa)}
    return res


def main(argv=None):
    old, new, out = sys.argv[1:4] if argv is None else argv
    res = diff_dirs(old, new)
    json.dump(res, open(out, "w"), indent=0, default=str)
    c = collections.Counter(); ident = collections.Counter(); diff = collections.Counter()
    for k, v in res.items():
        c[v["type"]] += 1
        (diff if v["ndiff"] else ident)[v["type"]] += 1
    for t in sorted(c): print(f"{t:8s} docs={c[t]:3d} identical={ident[t]:3d} different={diff[t]:3d}")
    print("total", len(res), "identical", sum(ident.values()), "different", sum(diff.values()))


if __name__ == "__main__":
    main()
