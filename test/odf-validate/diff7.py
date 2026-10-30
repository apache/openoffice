# usage: diff7.py <old_dump_dir> <new_dump_dir> <out.json>
# Flattens the per-document reload dumps and lists every differing path with old/new value.
import sys, os, json, collections
old, new, out = sys.argv[1:4]
IGN = ("props.creation_date", "props.gen", "props.edit_cycles")   # none expected to differ; creation_date is kept from the input
def flat(x, p=""):
    if isinstance(x, dict):
        for k in sorted(x): yield from flat(x[k], p + "." + k if p else k)
    elif isinstance(x, list):
        if len(x) > 40 and all(not isinstance(i, (dict, list)) for i in x): yield p, x
        else:
            for i, e in enumerate(x): yield from flat(e, f"{p}[{i}]")
    else: yield p, x
res = {}
names = sorted(f for f in os.listdir(old) if f.endswith(".json"))
for f in names:
    a = json.load(open(os.path.join(old, f)))
    b = json.load(open(os.path.join(new, f))) if os.path.exists(os.path.join(new, f)) else {"error": "MISSING"}
    fa, fb = dict(flat(a)), dict(flat(b))
    d = [(k, fa.get(k, "<absent>"), fb.get(k, "<absent>")) for k in sorted(set(fa) | set(fb)) if fa.get(k, "<absent>") != fb.get(k, "<absent>")]
    res[f[:-5]] = {"type": a.get("type") or b.get("type") or ("error" if "error" in a else "?"), "errors": [a.get("error"), b.get("error"), a.get("body_error"), b.get("body_error")], "ndiff": len(d), "diffs": d, "nprops": len(fa)}
json.dump(res, open(out, "w"), indent=0, default=str)
c = collections.Counter(); ident = collections.Counter(); diff = collections.Counter()
for k, v in res.items():
    c[v["type"]] += 1
    (diff if v["ndiff"] else ident)[v["type"]] += 1
for t in sorted(c): print(f"{t:8s} docs={c[t]:3d} identical={ident[t]:3d} different={diff[t]:3d}")
print("total", len(res), "identical", sum(ident.values()), "different", sum(diff.values()))
