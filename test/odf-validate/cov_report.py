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

# usage: cov_report.py <task-dir> <corpus-name>... [-- <observed-name>...]
# Joins the schema universe (cov_schema.py), the names the exporters can emit (cov_source.py) and what a corpus exercises
# (cov_corpus.py) under <task-dir>/{schema,source,<corpus-name>} and prints the gaps, element by element.
# Emittable is by name only: the source scan cannot tell which element an attribute is written on, and it misses
# table-driven sites, so names seen in the observed corpora count as emittable too.
import sys, os, collections

T = sys.argv[1]
ARGS = sys.argv[2:]
CORPORA, OBSERVED = (ARGS[:ARGS.index("--")], ARGS[ARGS.index("--") + 1:]) if "--" in ARGS else (ARGS, [])


def rows(path, n):
    with open(path) as f:
        return [tuple(l.rstrip("\n").split("\t")[:n]) for l in f if l.strip() and not l.startswith("#")]


def load(sub, name, n):
    return rows(os.path.join(T, sub, name), n)


S_EL, S_AT, S_VAL = set(), {}, collections.defaultdict(set)
for sub in ("schema", "schema/manifest"):
    S_EL |= set(load(sub, "elements.tsv", 2))
    S_AT.update({r[:4]: r[4] for r in load(sub, "attributes.tsv", 5)})
    for r in load(sub, "values.tsv", 5):
        S_VAL[r[:4]].add(r[4])
STATIC = {r[:2] for r in load("source", "emit.tsv", 2)}
C_EL, C_AT, C_VAL = set(), set(), collections.defaultdict(set)
for c in CORPORA:
    C_EL |= {r[:2] for r in load(c, "elements.tsv", 2)}
    C_AT |= {(r[0], r[1], r[2], r[3]) for r in load(c, "attributes.tsv", 4)}
    for r in load(c, "values.tsv", 5):
        C_VAL[r[:4]].add(r[4])


OBS = set()
for c in CORPORA + OBSERVED:
    OBS |= {r[:2] for r in load(c, "elements.tsv", 2)} | {r[2:4] for r in load(c, "attributes.tsv", 4)}
EMIT = STATIC | OBS


def short(ns, loc):
    return ns.rsplit("xmlns:", 1)[-1].replace(":1.0", "") + ":" + loc if "xmlns:" in ns else ns + " " + loc


def block(title, items):
    print("\n== %s: %d" % (title, len(items)))
    for i in sorted(items):
        print("  " + i)


print("schema: %d elements, %d element/attribute pairs; emit set: %d names; corpora: %s" %
      (len(S_EL), len(S_AT), len(EMIT), ", ".join(CORPORA)))
print("static emit set %d names; observed %d names, of which %d not found by the static scan" %
      (len(STATIC), len(OBS), len(OBS - STATIC)))
print("corpus covers %d of %d schema elements and %d of %d schema pairs" %
      (len(S_EL & C_EL), len(S_EL), len(set(S_AT) & C_AT), len(S_AT)))

miss_el = S_EL - C_EL
block("elements not exercised, AOO can emit (GAP: generate)", [short(*e) for e in miss_el if e in EMIT])
block("elements not exercised, no emit site found and never observed (UNKNOWN: names built at run time are missed, e.g. animation, text fields)", [short(*e) for e in miss_el if e not in EMIT])

miss_at = {k for k in S_AT if k not in C_AT and k[:2] in C_EL}
block("attributes not exercised on an exercised element, name emitted by AOO (GAP)",
      ["%s @%s" % (short(*k[:2]), short(*k[2:])) for k in miss_at if k[2:] in EMIT])
block("attributes not exercised on an exercised element, name never emitted",
      ["%s @%s" % (short(*k[:2]), short(*k[2:])) for k in miss_at if k[2:] not in EMIT])

n_val = sum(len(v) for v in S_VAL.values())
hit_val = sum(len(vs & C_VAL[k]) for k, vs in S_VAL.items())
print("corpus covers %d of %d schema enumerated values" % (hit_val, n_val))
miss_val = []
for k, vs in S_VAL.items():
    if k in C_AT:
        for v in sorted(vs - C_VAL[k]):
            miss_val.append("%s @%s = %s" % (short(*k[:2]), short(*k[2:]), v))
block("enumerated values not exercised on an exercised attribute", miss_val)

emit_unknown = [short(*e) for e in EMIT if e[0].startswith("urn:oasis:names:tc:opendocument:xmlns:") and e not in S_EL
                and not any(k[2:] == e for k in S_AT)]
block("emitted names in ODF namespaces that the schema defines neither as element nor attribute", emit_unknown)
