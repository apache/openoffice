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

# usage: report.py <val.json> <kinds.json> [12|14|12x]   -- groups errors by normalized message
import sys, re, json, collections


def norm(m):
    m = re.sub(r'; expected .*', '', m)
    m = re.sub(r' \(with xmlns.*', '', m)
    return re.sub(r'value of attribute "([^"]+)" is invalid.*', r'value of attribute "\1" is invalid', m)


val, out = sys.argv[1], sys.argv[2]
ver = sys.argv[3] if len(sys.argv) > 3 else "12"
d = json.load(open(val))
kinds = collections.defaultdict(lambda: {"errors": 0, "packages": set()})
for f, line, col, msg in d["errors"][ver]:
    k = kinds[norm(msg) if f != "?" else "UNPARSED: " + msg]
    k["errors"] += 1; k["packages"].add(f.split("/")[0])
res = {k: {"errors": v["errors"], "packages": sorted(v["packages"])} for k, v in kinds.items()}
json.dump(res, open(out, "w"), indent=1, sort_keys=True)
tot = sum(v["errors"] for v in res.values())
print(f"ODF {ver[0]}.{ver[1]}{' extended, foreign content removed' if ver.endswith('x') else ''}: {len(res)} kinds, {tot} errors, {len({p for v in res.values() for p in v['packages']})} packages with errors")
for k, v in sorted(res.items(), key=lambda kv: -kv[1]["errors"]):
    print(f"  {v['errors']:5d} {len(v['packages']):4d}  {k[:120]}")
