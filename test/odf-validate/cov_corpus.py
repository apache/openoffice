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

# usage: cov_corpus.py <outdir> <cov-dir>
# What a corpus of saved packages exercises: every element, every (element, attribute) pair and the
# distinct attribute values (capped), per namespace URI, counted over the XML streams of all packages.
import sys, os, zipfile, collections
from xml.parsers import expat

D = os.path.dirname(os.path.abspath(__file__))
STREAMS = {"content.xml", "styles.xml", "meta.xml", "settings.xml", "manifest.xml"}
VALUE_CAP = 200


def esc(v):
    return v.replace("\\", "\\\\").replace("\t", "\\t").replace("\n", "\\n").replace("\r", "\\r")


def split(name):
    ns, _, local = name.rpartition(" ")
    return ns, local


def scan(data, pkg, elems, pairs, values):
    stack = []
    p = expat.ParserCreate(namespace_separator=" ")

    def start(name, attrs):
        e = split(name)
        elems[e].add(pkg)
        for a, v in attrs.items():
            k = (e, split(a))
            pairs[k].add(pkg)
            vs = values[k]
            if len(vs) < VALUE_CAP or v in vs:
                vs[v] += 1
    p.StartElementHandler = start
    p.Parse(data, True)


def main():
    outdir, covdir = sys.argv[1], sys.argv[2]
    if not os.path.abspath(covdir).startswith(os.path.join(D, "runs") + os.sep):
        sys.exit("cov_corpus.py: <cov-dir> must live under " + os.path.join(D, "runs"))
    os.makedirs(covdir, exist_ok=True)
    elems, pairs = collections.defaultdict(set), collections.defaultdict(set)
    values = collections.defaultdict(collections.Counter)
    pkgs = sorted(p for p in os.listdir(outdir) if not p.startswith("."))
    bad, nstreams = [], 0
    for pkg in pkgs:
        try:
            z = zipfile.ZipFile(os.path.join(outdir, pkg))
        except Exception:
            bad.append(pkg); continue
        for name in z.namelist():
            if name.rsplit("/", 1)[-1] not in STREAMS:
                continue
            nstreams += 1
            try:
                scan(z.read(name), pkg, elems, pairs, values)
            except expat.ExpatError as e:
                bad.append(pkg + "/" + name)
    with open(os.path.join(covdir, "elements.tsv"), "w") as f:
        for (ns, loc), s in sorted(elems.items()):
            f.write(f"{ns}\t{loc}\t{len(s)}\n")
    with open(os.path.join(covdir, "attributes.tsv"), "w") as f:
        for ((ens, eloc), (ans, aloc)), s in sorted(pairs.items()):
            f.write(f"{ens}\t{eloc}\t{ans}\t{aloc}\t{len(s)}\n")
    with open(os.path.join(covdir, "values.tsv"), "w") as f:
        for ((ens, eloc), (ans, aloc)), c in sorted(values.items()):
            for v, n in sorted(c.items()):
                f.write(f"{ens}\t{eloc}\t{ans}\t{aloc}\t{esc(v)}\t{n}\n")
    print(f"{len(pkgs)} packages, {nstreams} streams, {len(elems)} elements, {len(pairs)} element/attribute pairs, "
          f"unreadable: {bad}")


main()
