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

# Removes foreign elements and attributes (ODF 1.2 Part 1 3.17) so that the rest can be validated as an
# extended document (2.2.2 B.1). Loaded by validate.py and selftest.py.
#   foreign attribute: namespace outside tables 1-4 of 1.5 (xml: included), removed.
#   foreign element: namespace outside tables 1-4, outside any element of tables 1-4 (their wildcard
#     content is the schema's), removed with its content, except that its tags alone are removed when
#     it sits in a text:p or text:h and every ancestor up to it permits character data in the schema.
# office:process-content (deprecated in 1.2) is not consulted. Table 4 names that the schema does not
# define are not removed, so they stay visible as errors.
import re
from xml.etree import ElementTree as ET
from xml.parsers import expat
from xml.sax.saxutils import escape, quoteattr

ODF = "urn:oasis:names:tc:opendocument:xmlns:%s:1.0"
T123 = {ODF % x for x in "animation chart config database dr3d drawing form manifest meta datastyle office presentation script table text style "
        "xsl-fo-compatible svg-compatible smil-compatible".split()} | {"http://docs.oasis-open.org/ns/office/1.2/meta/odf#"}
T4 = {"http://purl.org/dc/elements/1.1/", "http://www.w3.org/1998/Math/MathML", "http://www.w3.org/2002/xforms", "http://www.w3.org/1999/xlink",
      "http://www.w3.org/1999/xhtml", "http://www.w3.org/2003/g/data-view#", "http://www.w3.org/XML/1998/namespace"}
RNG = "{http://relaxng.org/ns/structure/1.0}"
TEXT_P, TEXT_H = "{%s}p" % (ODF % "text"), "{%s}h" % (ODF % "text")


def mixed_elements(rng_path):
    """Expanded names of the schema's elements whose content model permits character data."""
    prefixes = dict(re.findall(r'xmlns:(\w+)="([^"]+)"', open(rng_path, encoding="utf-8").read(4000)))
    defines = {}
    for d in ET.parse(rng_path).getroot().iter(RNG + "define"):
        defines.setdefault(d.get("name"), []).append(d)
    memo = {}

    def has_text(node, seen):
        tag = node.tag
        if tag in (RNG + "text", RNG + "mixed"):
            return True
        if tag in (RNG + "element", RNG + "attribute", RNG + "data", RNG + "value"):
            return False
        if tag == RNG + "ref":
            n = node.get("name")
            if n in seen:
                return False
            return any(has_text(c, seen | {n}) for d in defines.get(n, []) for c in d)
        return any(has_text(c, seen) for c in node)
    out = set()
    for el in ET.parse(rng_path).getroot().iter(RNG + "element"):
        name = el.get("name")
        if name and ":" in name:
            pre, local = name.split(":", 1)
            if any(has_text(c, frozenset()) for c in el):
                out.add("{%s}%s" % (prefixes[pre], local))
    return out


def strip_foreign(text, mixed):
    out, scopes, frames, state = [], [{"xml": "http://www.w3.org/XML/1998/namespace"}], [], {"drop": 0, "pending": {}}
    p = expat.ParserCreate()
    p.ordered_attributes = True

    def expand(name, scope, is_attr):
        """Expanded name, or None when the prefix is unbound (kept, so that the validator reports it)."""
        if ":" in name:
            pre, local = name.split(":", 1)
            return "{%s}%s" % (scope[pre], local) if pre in scope else None
        uri = "" if is_attr else scope.get("", "")
        return "{%s}%s" % (uri, name) if uri else name

    def ns_of(expanded):
        return expanded[1:].split("}", 1)[0] if expanded and expanded.startswith("{") else ""

    def start(name, attrs):
        if state["drop"]:
            state["drop"] += 1
            return
        decl = {}
        for i in range(0, len(attrs), 2):
            if attrs[i] == "xmlns" or attrs[i].startswith("xmlns:"):
                decl[attrs[i][6:]] = attrs[i + 1]
        scope = dict(scopes[-1])
        scope.update(decl)
        exp = expand(name, scope, False)
        in_t4 = any(ns_of(f[0]) in T4 for f in frames if f[1])
        if exp is not None and ns_of(exp) not in T123 | T4 and not in_t4:
            if unwrap(frames):
                scopes.append(scope)
                frames.append((exp, False))
                state["pending"].update(decl)
            else:
                state["drop"] = 1
            return
        kept = []
        for i in range(0, len(attrs), 2):
            k, v = attrs[i], attrs[i + 1]
            ke = expand(k, scope, True)
            if k == "xmlns" or k.startswith("xmlns:") or ":" not in k or ke is None or ns_of(ke) in T123 | T4:
                kept.append((k, v))
        have = {k for k, _ in kept}
        for pre, uri in state["pending"].items():
            k = "xmlns:" + pre if pre else "xmlns"
            if k not in have:
                kept.insert(0, (k, uri))
        state["pending"] = {}
        out.append("<" + name + "".join(" %s=%s" % (k, quoteattr(v, {"\n": "&#10;", "\t": "&#9;", "\r": "&#13;"})) for k, v in kept) + ">")
        scopes.append(scope)
        frames.append((exp, True))

    def end(name):
        if state["drop"]:
            state["drop"] -= 1
            return
        scopes.pop()
        if frames.pop()[1]:
            out.append("</" + name + ">")

    def data(s):
        if not state["drop"]:
            out.append(escape(s))

    def unwrap(fr):
        """Foreign element in a paragraph, every ancestor up to it permitting character data?"""
        for exp, kept in reversed(fr):
            if not kept:
                continue
            if exp in (TEXT_P, TEXT_H):
                return True
            if exp not in mixed:
                return False
        return False
    p.StartElementHandler, p.EndElementHandler, p.CharacterDataHandler = start, end, data
    p.Parse(text.encode("utf-8") if isinstance(text, str) else text, True)
    return '<?xml version="1.0" encoding="UTF-8"?>\n' + "".join(out) + "\n"
