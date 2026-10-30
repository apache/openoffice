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

# usage: roundtrip.py <port> <file> <checks.json> [--capture <out.json>]
# checks.json: [{"name": ..., "expr": "<python expression over doc>", "expect": <value>}]
# With --capture, evaluates each expr and writes {"name": value} instead of comparing.
import sys, json, uno
from unohelp import connect, desktop, load


def plain(v):
    if isinstance(v, (bool, int, float, str)) or v is None:
        return v
    if isinstance(v, (tuple, list)):
        return [plain(x) for x in v]
    if isinstance(v, uno.Enum):
        return v.value
    if hasattr(v, "typeName"):
        return {k: plain(getattr(v, k)) for k in dir(v) if not k.startswith("_") and k not in ("typeName", "value")}
    return repr(v)


port, path, checks = int(sys.argv[1]), sys.argv[2], json.load(open(sys.argv[3]))
doc = load(desktop(connect(port)), path)
got = {}
for c in checks:
    try:
        got[c["name"]] = plain(eval(c["expr"], {"doc": doc, "uno": uno}))
    except Exception as e:
        got[c["name"]] = "ERROR " + repr(e)
doc.close(True)
if "--capture" in sys.argv:
    json.dump(got, open(sys.argv[sys.argv.index("--capture") + 1], "w"), indent=1)
    print(json.dumps(got, indent=1)); sys.exit(0)
fails = [(c["name"], c["expect"], got[c["name"]]) for c in checks if got[c["name"]] != c["expect"]]
for n, e, g in fails:
    print(f"MISMATCH {n}: expected {e!r} got {g!r}")
print(f"roundtrip: {len(checks) - len(fails)}/{len(checks)} match")
sys.exit(1 if fails else 0)
