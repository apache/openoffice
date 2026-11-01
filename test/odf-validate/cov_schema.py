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

import os, sys
import xml.etree.ElementTree as ET

HERE = os.path.dirname(os.path.abspath(__file__))
SCHEMAS = [("", "OpenDocument-v1.2-os-schema.rng"),
           ("manifest", "OpenDocument-v1.2-os-manifest-schema.rng")]
RNG = "{http://relaxng.org/ns/structure/1.0}"
XML_NS = "http://www.w3.org/XML/1998/namespace"
WILD = ("*", "*")


def tag(e):
    return e.tag[len(RNG):]


class Schema:
    def __init__(self, path):
        self.prefixes = {"xml": XML_NS}
        for ev, v in ET.iterparse(path, events=("start-ns", "end")):
            if ev == "start-ns":
                p, uri = v
                if self.prefixes.setdefault(p, uri) != uri:
                    sys.exit("prefix %r bound twice in %s" % (p, path))
        self.root = ET.parse(path).getroot()
        self.defs = {}
        for d in self.root.iter(RNG + "define"):
            if d.get("name") in self.defs:
                sys.exit("duplicate define %s" % d.get("name"))
            self.defs[d.get("name")] = d
        self.cache = {}
        self.stack = set()

    def qname(self, text, is_attr):
        text = text.strip()
        if ":" in text:
            p, local = text.split(":", 1)
            return self.prefixes[p], local
        if not is_attr:
            sys.exit("unprefixed element name %r" % text)
        return "", text

    def names(self, nc, is_attr):
        t = tag(nc)
        if t == "name":
            if nc.get("ns") is not None:
                sys.exit("name with ns attribute")
            return [self.qname(nc.text, is_attr)]
        if t == "choice":
            parts = [self.names(c, is_attr) for c in nc]
            return None if any(p is None for p in parts) else [n for p in parts for n in p]
        return None

    def merge_group(self, parts):
        out = {}
        for part in parts:
            for k, (req, vals) in part.items():
                if k in out:
                    r0, v0 = out[k]
                    vals = None if v0 is None or vals is None else v0 | vals
                    req = r0 or req
                out[k] = (req, vals)
        return out

    def merge_choice(self, parts):
        out = {}
        for part in parts:
            for k, (req, vals) in part.items():
                if k in out:
                    r0, v0 = out[k]
                    vals = None if v0 is None or vals is None else v0 | vals
                    req = r0 and req
                else:
                    req = req and all(k in p for p in parts)
                out[k] = (req, vals)
        return out

    def values(self, children):
        parts = [self.value_of(c) for c in children]
        return None if len(parts) != 1 else parts[0]

    def value_of(self, n):
        t = tag(n)
        if t == "value":
            if n.get("type", "token") != "token" or n.get("datatypeLibrary"):
                return None
            return frozenset([n.text or ""])
        if t == "ref":
            return self.values(list(self.defs[n.get("name")]))
        if t == "choice":
            parts = [self.value_of(c) for c in n]
            return None if any(p is None for p in parts) else frozenset().union(*parts)
        return None

    def attrs(self, n):
        t = tag(n)
        if t == "attribute":
            kids = list(n)
            if "name" in n.attrib:
                names, body = [self.qname(n.get("name"), True)], kids
            else:
                names, body = self.names(kids[0], True), kids[1:]
            if names is None:
                return {WILD: (False, None)}
            vals = self.values(body) if body else None
            req = len(names) == 1
            return {k: (req, vals) for k in names}
        if t == "ref":
            return self.attrs_define(n.get("name"))
        if t in ("element", "value", "data", "text", "empty", "list", "notAllowed"):
            return {}
        parts = [self.attrs(c) for c in n]
        if t == "choice":
            return self.merge_choice(parts)
        out = self.merge_group(parts)
        if t in ("optional", "zeroOrMore"):
            out = {k: (False, v) for k, (r, v) in out.items()}
        return out

    def attrs_define(self, name):
        if name in self.cache:
            return self.cache[name]
        if name in self.stack:
            sys.exit("pattern recursion without element: %s" % name)
        self.stack.add(name)
        res = self.merge_group([self.attrs(c) for c in self.defs[name]])
        self.stack.discard(name)
        self.cache[name] = res
        return res

    def element_defs(self):
        for e in self.root.iter(RNG + "element"):
            kids = list(e)
            if "name" in e.attrib:
                names, body = [self.qname(e.get("name"), False)], kids
            else:
                names, body = self.names(kids[0], False), kids[1:]
            yield names, self.merge_group([self.attrs(c) for c in body])


def extract(path):
    s = Schema(path)
    elements, per_elem = set(), {}
    stats = {"element_nodes": 0, "wild_elements": 0, "wild_attr_elements": 0}
    for names, attrs in s.element_defs():
        stats["element_nodes"] += 1
        if names is None:
            stats["wild_elements"] += 1
            continue
        if WILD in attrs:
            stats["wild_attr_elements"] += len(names)
            del attrs[WILD]
        for nm in names:
            elements.add(nm)
            per_elem.setdefault(nm, []).append(attrs)
    pairs = {}
    for nm, defs in per_elem.items():
        keys = set().union(*defs)
        for k in keys:
            reqs = [d[k][0] if k in d else False for d in defs]
            vals = [d[k][1] if k in d else frozenset() for d in defs]
            vals = None if any(v is None for v in vals) else frozenset().union(*vals)
            pairs[(nm, k)] = (all(reqs), vals)
    return elements, pairs, stats


def write(outdir, elements, pairs):
    os.makedirs(outdir, exist_ok=True)
    with open(os.path.join(outdir, "elements.tsv"), "w") as f:
        for ns, local in sorted(elements):
            f.write("%s\t%s\n" % (ns, local))
    with open(os.path.join(outdir, "attributes.tsv"), "w") as f, \
         open(os.path.join(outdir, "values.tsv"), "w") as g:
        for (en, an), (req, vals) in sorted(pairs.items()):
            f.write("%s\t%s\t%s\t%s\t%s\n" % (en + an + ("required" if req else "optional",)))
            for v in sorted(vals or ()):
                g.write("%s\t%s\t%s\t%s\t%s\n" % (en + an + (v,)))


def main():
    if len(sys.argv) != 2:
        sys.exit("usage: python3 -I cov_schema.py <out-dir>")
    out = sys.argv[1]
    for sub, name in SCHEMAS:
        elements, pairs, stats = extract(os.path.join(HERE, "schema", name))
        write(os.path.join(out, sub), elements, pairs)
        enum = [p for p, (r, v) in pairs.items() if v]
        sys.stderr.write(
            "%s: elements=%d (element nodes=%d) attributes=%d pairs=%d required=%d "
            "enumerated pairs=%d values=%d\n"
            "  skipped wildcards: %d anyName/nsName element nodes, "
            "%d elements with a wildcard attribute\n"
            % (name, len(elements), stats["element_nodes"],
               len({k for (_, k) in pairs}), len(pairs),
               sum(1 for r, v in pairs.values() if r), len(enum),
               sum(len(pairs[p][1]) for p in enum),
               stats["wild_elements"], stats["wild_attr_elements"]))


main()
