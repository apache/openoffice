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

# usage: compare.py <base.kinds.json> <new.kinds.json>
# Exit 1 if any kind is new or any kind's error or package count increased.
import sys, json
a, b = json.load(open(sys.argv[1])), json.load(open(sys.argv[2]))
bad = False
for k in sorted(set(a) | set(b)):
    ea, eb = a.get(k, {}).get("errors", 0), b.get(k, {}).get("errors", 0)
    pa, pb = len(a.get(k, {}).get("packages", [])), len(b.get(k, {}).get("packages", []))
    if (ea, pa) == (eb, pb):
        continue
    worse = k not in a or eb > ea or pb > pa
    bad |= worse
    print(f"{'WORSE' if worse else 'better'}  errors {ea}->{eb}  packages {pa}->{pb}  {k[:110]}")
print("compare:", "FAIL" if bad else "OK")
sys.exit(1 if bad else 0)
