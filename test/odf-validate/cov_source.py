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

"""Statically extract the (namespace, local name) pairs that the ODF export
code can emit.  Usage: python3 -I cov_source.py <repo-main-dir> <out-dir>

Writes emit.tsv, emit-unresolved.tsv and emit-skipped.tsv into <out-dir>.
"""

import collections
import os
import re
import sys

SRC_EXT = (".cxx", ".hxx", ".h")
SKIP_DIRS = {"test", "qa", "workben", "unotest", "transform", "unoxml", "solver"}
SKIP_PATHS = ("filter/source/svg/",)
KEEP_NAMES = {"impastp4.cxx"}
IMPORT_NAME = re.compile(r"import|imp", re.I)

NS_RE = re.compile(r"XML_NAMESPACE_([A-Z][A-Z0-9_]*)")
TOK_RE = re.compile(r"\bXML_[A-Z_][A-Z0-9_]*\b")
STR_RE = re.compile(r'"((?:[^"\\\n]|\\.)*)"')
NO_EXPORT = re.compile(r"\bMID_FLAG_NO_PROPERTY(_EXPORT)?\b|\bMID_SW_FLAG_NO_ITEM(_EXPORT)?\b")
ELEMENT_ROW = re.compile(r"\bMID_(SW_)?FLAG_ELEMENT_ITEM(_EXPORT)?\b")
PROP_ROW = re.compile(r"\bMID_\w+|\bXML_(?:\w+_)?TYPE_|\bsizeof\s*\(")
EVENT_ROW = re.compile(r'^\{\s*"[^"]*"\s*,\s*XML_NAMESPACE_\w+\s*,\s*"')
EMIT_ATTR = ("AddAttribute", "AddAttributeASCII")
EMIT_FUNCS = set(EMIT_ATTR) | {"AddAttributeIdLegacy", "SvXMLElementExport", "GetQNameByKey"}
DECL_ARG = re.compile(r"^(?:const\s+|enum\s+|sal_uInt16\b|SvXMLExport\b|SvXMLAttributeList\b)")


def read(path):
    with open(path, errors="replace") as f:
        return f.read()


def mask(text):
    """Blank comments and string contents (same length, newlines kept)."""
    out = list(text)
    i, n = 0, len(text)
    while i < n:
        c = text[i]
        if c == "/" and text[i:i + 2] == "//":
            j = text.find("\n", i)
            j = n if j < 0 else j
            for k in range(i, j):
                out[k] = " "
            i = j
        elif c == "/" and text[i:i + 2] == "/*":
            j = text.find("*/", i + 2)
            j = n if j < 0 else j + 2
            for k in range(i, j):
                if text[k] != "\n":
                    out[k] = " "
            i = j
        elif c in "\"'":
            j = i + 1
            while j < n and text[j] != c and text[j] != "\n":
                j += 2 if text[j] == "\\" else 1
            for k in range(i + 1, min(j, n)):
                out[k] = " "
            i = j + 1
        else:
            i += 1
    return "".join(out)


def directive_spans(text):
    """Character ranges of preprocessor lines, with backslash continuations."""
    spans = []
    for m in re.finditer(r"^[ \t]*#(?:[^\n]*\\\n)*[^\n]*", text, re.M):
        spans.append((m.start(), m.end()))
    return spans


def blank(masked, spans):
    out = list(masked)
    for a, b in spans:
        for k in range(a, b):
            if out[k] != "\n":
                out[k] = " "
    return "".join(out)


def match_close(m, i):
    """Index of the bracket closing the one at m[i]."""
    depth = 0
    for k in range(i, len(m)):
        c = m[k]
        if c in "({[":
            depth += 1
        elif c in ")}]":
            depth -= 1
            if depth == 0:
                return k
    return len(m) - 1


def split_args(m, open_i, close_i):
    """(start, end) of each top-level argument between two brackets."""
    args, depth, start = [], 0, open_i + 1
    for k in range(open_i + 1, close_i):
        c = m[k]
        if c in "({[":
            depth += 1
        elif c in ")}]":
            depth -= 1
        elif c == "," and depth == 0:
            args.append((start, k))
            start = k + 1
    args.append((start, close_i))
    return args


def enclosing_call(m, pos):
    """(function name, argument index, argument count) of the innermost call around pos."""
    depth = 0
    for k in range(pos - 1, -1, -1):
        c = m[k]
        if c in ")}]":
            depth += 1
        elif c in "({[":
            if depth == 0:
                if c != "(":
                    return None
                name = re.search(r"(\w+)\s*$", m[max(0, k - 60):k])
                a = split_args(m, k, match_close(m, k))
                idx = next((n for n, (x, y) in enumerate(a) if x <= pos < y), None)
                return (name.group(1) if name else None), idx, len(a)
            depth -= 1
    return None


def parse_tokens(path):
    toks = {}
    pat = re.compile(r'TOKEN\(\s*"((?:[^"\\]|\\.)*)"\s*,\s*(XML_\w+)\s*(?:/\*.*?\*/)?\s*\)')
    for line in read(path).splitlines():
        if line.lstrip().startswith("//"):
            continue
        m = pat.search(line)
        if m:
            toks[m.group(2)] = m.group(1).replace('\\"', '"')
    return toks


ADD_RE = re.compile(r"(?:\b_GetNamespaceMap\s*\(\s*\)\s*\.|\bmpNamespaceMap\s*->)\s*Add(?:AtIndex)?\s*\(")


def ns_adds(masked, clean, toks):
    """(key, URI) pairs from export-side namespace map Add/AddAtIndex calls."""
    defs = dict(re.findall(r'^[ \t]*#[ \t]*define[ \t]+(\w+)[ \t]+"([^"]*)"', clean, re.M))
    out = []
    for m in ADD_RE.finditer(masked):
        o = m.end() - 1
        a = split_args(masked, o, match_close(masked, o))
        if len(a) not in (3, 4):
            continue
        key = NS_RE.fullmatch(masked[a[-1][0]:a[-1][1]].strip())
        arg = clean[a[-2][0]:a[-2][1]]
        tok = re.search(r"\b(XML_N_\w+)", arg)
        lit = next((defs[w] for w in re.findall(r"\w+", arg) if w in defs), None)
        uri = toks.get(tok.group(1)) if tok else lit
        if key and uri is not None:
            out.append((key.group(1), uri))
    return out


class Macro:
    def __init__(self, ns_i, ns_fixed, tok_i, tok_prefix):
        self.ns_i, self.ns_fixed, self.tok_i, self.tok_prefix = ns_i, ns_fixed, tok_i, tok_prefix


def parse_macros(text):
    """Function-like macros that carry a namespace key and a token parameter."""
    macros = {}
    for m in re.finditer(r"^[ \t]*#[ \t]*define[ \t]+(\w+)\(([^)]*)\)((?:[^\n]|\\\n)*)", text, re.M):
        name, params, body = m.group(1), [p.strip() for p in m.group(2).split(",")], m.group(3)
        ns = re.search(r"XML_NAMESPACE_##(\w+)", body)
        fixed = re.search(r"XML_NAMESPACE_([A-Z][A-Z0-9_]*)\s*,\s*(\w+)", body)
        if ns and ns.group(1) in params:
            ns_i = params.index(ns.group(1))
            tok = re.search(r"(?:xmloff::token::)?XML_##(\w+)", body)
            if tok and tok.group(1) in params:
                macros[name] = Macro(ns_i, None, params.index(tok.group(1)), "XML_")
                continue
            tok = re.search(r"xmloff::token::(\w+)", body)
            if tok and tok.group(1) in params:
                macros[name] = Macro(ns_i, None, params.index(tok.group(1)), "")
        elif fixed and fixed.group(2) in params:
            macros[name] = Macro(None, fixed.group(1), params.index(fixed.group(2)), "")
    return macros


def included(text):
    return re.findall(r'^[ \t]*#[ \t]*include[ \t]*["<]([^">]+)[">]', text, re.M)


KEYWORDS = {"if", "for", "while", "switch", "catch", "return", "sizeof", "else"}
DEF_TAIL = re.compile(r"\s*(?:const\s*)?(?:throw\s*\([^)]*\)\s*)?(?::[^{;]*)?\{")


def func_defs(m):
    """(name, parameter identifiers, body start, body end) of function definitions."""
    out = []
    for mm in re.finditer(r"\b([A-Za-z_]\w*)\s*\(", m):
        if mm.group(1) in KEYWORDS:
            continue
        o = mm.end() - 1
        c = match_close(m, o)
        tail = DEF_TAIL.match(m, c + 1)
        if not tail:
            continue
        params = []
        for a, b in split_args(m, o, c):
            w = re.findall(r"\w+", re.sub(r"=.*", "", m[a:b], flags=re.S))
            params.append(w[-1] if len(w) > 1 else None)
        b0 = tail.end() - 1
        out.append((mm.group(1), params, b0, match_close(m, b0)))
    return out


class Wrapper:
    def __init__(self, tok_i, nparams, keys, ns_param, role):
        self.tok_i, self.nparams, self.keys, self.ns_param, self.role = tok_i, nparams, keys, ns_param, role


class Scanner:
    def __init__(self, main):
        self.main = main
        self.toks = parse_tokens(os.path.join(main, "xmloff/source/core/xmltoken.cxx"))
        self.pairs = collections.defaultdict(lambda: {"roles": set(), "mods": set(), "ex": []})
        self.unres, self.skipped, self.files = [], [], []
        self.header_macros = {}
        self.nskey_uri_global = {}
        self.ns_local = {}
        self.wrappers = collections.defaultdict(list)
        self.cache = {}
        self.funcs = {}

    def walk(self):
        out = []
        for d, ds, fs in os.walk(self.main):
            rel = os.path.relpath(d, self.main).replace(os.sep, "/") + "/"
            ds[:] = [x for x in ds if x not in SKIP_DIRS]
            if any(p in rel for p in SKIP_PATHS):
                continue
            for f in fs:
                if f.endswith(SRC_EXT):
                    out.append(os.path.join(d, f))
        return sorted(out)

    @staticmethod
    def eligible(path):
        base = os.path.basename(path)
        return base.lower() in KEEP_NAMES or not IMPORT_NAME.search(base)

    def uri(self, key, path):
        key = re.sub(r"_IDX$", "", key)
        uri = self.ns_local.get(path, {}).get(key) or self.nskey_uri_global.get(key)
        if uri:
            return uri
        if key == "NONE":
            return ""
        if key == "XML":
            return self.toks["XML_N_XML"]
        return None

    def prepare(self):
        texts = {}
        for p in self.walk():
            t = read(p)
            if "XML_NAMESPACE_" not in t:
                continue
            texts[p] = t
        for p, t in texts.items():
            if p.endswith((".hxx", ".h")):
                mc = parse_macros(t)
                if mc:
                    self.header_macros[os.path.basename(p)] = mc
        first = {}
        for p, t in sorted(texts.items(), key=lambda kv: kv[0]):
            if not self.eligible(p):
                continue
            self.files.append(p)
            m = mask(t)
            for key, tok in ns_adds(m, t, self.toks):
                self.ns_local.setdefault(p, {})[key] = tok
                if p.endswith("xmloff/source/core/xmlexp.cxx"):
                    self.nskey_uri_global[key] = tok
                else:
                    first.setdefault(key, tok)
        for key, tok in first.items():
            self.nskey_uri_global.setdefault(key, tok)
        self.texts = texts

    def names(self, arg_masked, arg_clean):
        """Names in a name argument, and whether anything was left unresolved."""
        found, rest = [], arg_masked
        for m in STR_RE.finditer(arg_clean):
            if m.group(1):
                found.append(m.group(1))
        for m in TOK_RE.finditer(arg_masked):
            if m.group(0) in self.toks and not m.group(0).startswith(("XML_NP_", "XML_N_")):
                found.append(self.toks[m.group(0)])
        if not found and re.search(r"\bXML_NP?_[A-Z]", arg_masked):
            return [], False
        rest = TOK_RE.sub(" ", STR_RE.sub(" ", arg_clean))
        rest = re.sub(r"(?:::)?(?:xmloff::token::|token::|GetXMLToken|RTL_CONSTASCII_USTRINGPARAM|"
                      r"OUString|createFromAscii|rtl::|sal_Char|static_cast<[^>]*>)", " ", rest)
        rest = re.sub(r"[\s()?:]|\bsal_True\b|\bsal_False\b", "", rest)
        return found, bool(rest) or not found

    def record(self, path, line, key, names, role, row):
        uri = self.uri(key, path)
        if uri is None:
            self.unres.append((path, line, "unmapped namespace key " + key, row))
            return
        mod = os.path.relpath(path, self.main).split(os.sep)[0]
        for n in names:
            e = self.pairs[(uri, n)]
            e["roles"].add(role)
            e["mods"].add(mod)
            e["ex"].append((os.path.relpath(path, self.main), line, role))

    def scan_file(self, path):
        text = self.texts[path]
        spans = directive_spans(text)
        m = blank(mask(text), spans)
        clean = blank(text, spans)
        macros = {}
        for inc in included(text):
            macros.update(self.header_macros.get(os.path.basename(inc), {}))
        macros.update(parse_macros(text))
        self.cache[path] = (m, clean, text)
        line_of = lambda i: text.count("\n", 0, i) + 1
        snip = lambda a, b: " ".join(clean[a:b].split())[:160]
        handled = set()
        opener = self.openers(m)

        funcs = self.funcs[path] = func_defs(m)
        tokmaps = [(t.end(), match_close(m, t.end() - 1))
                   for t in re.finditer(r"SvXMLTokenMapEntry\s+\w+\s*\[[^\]]*\]\s*=\s*\{", m)]
        in_tokmap = lambda i: any(a <= i < b for a, b in tokmaps)

        def wrap(ns_a, ns_b, nm_a, nm_b, role, i):
            """Register the enclosing function as a wrapper when it forwards its parameters."""
            inner = [f for f in funcs if f[2] < i < f[3]]
            if not inner:
                return False
            name, params, b0, b1 = min(inner, key=lambda f: f[3] - f[2])
            nm, ns = m[nm_a:nm_b].strip(), m[ns_a:ns_b].strip()
            if nm not in params or name in EMIT_FUNCS or role == "either":
                return False
            if NS_RE.fullmatch(ns):
                keys, ns_param = [NS_RE.fullmatch(ns).group(1)], None
            elif ns in params:
                keys, ns_param = None, ns
            else:
                return False
            module = os.path.relpath(path, self.main).split(os.sep)[0]
            self.wrappers[(module, name)].append(
                Wrapper(params.index(nm), len(params), keys, params.index(ns) if ns_param else None, role))
            return True

        def pair_from(ns_a, ns_b, nm_a, nm_b, role, ctx_a, ctx_b, i, fixed=None, wrapok=True):
            if in_tokmap(i):
                return
            keys = NS_RE.findall(m[ns_a:ns_b])
            names, bad = (fixed, False) if fixed else self.names(m[nm_a:nm_b], clean[nm_a:nm_b])
            row = snip(ctx_a, ctx_b)
            for k in NS_RE.finditer(m[ns_a:ns_b]):
                handled.add(ns_a + k.start())
            if (not keys or bad) and wrapok and not fixed and wrap(ns_a, ns_b, nm_a, nm_b, role, i):
                return
            if not keys:
                self.unres.append((path, line_of(i), "namespace not a literal key", row))
                return
            if bad:
                self.unres.append((path, line_of(i), "name not a literal token or string", row))
            if not names:
                return
            if NO_EXPORT.search(clean[ctx_a:ctx_b]):
                self.skipped.append((path, line_of(i), "not exported (flags)", row))
                return
            for k in keys:
                self.record(path, line_of(i), k, names, role, row)

        for name, params, b0, b1 in funcs:
            ns_fn = [f for f in funcs if f[0] == re.sub(r"Name$", "Namespace", name) and name.endswith("AttributeName")]
            if not ns_fn:
                continue
            ns_body = clean[ns_fn[0][2]:ns_fn[0][3]]
            default = re.findall(r"return\s+XML_NAMESPACE_([A-Z][A-Z0-9_]*)\s*;", ns_body)[-1:]
            special = {(y if x in ns_fn[0][1] else x): k for x, y, k in re.findall(
                r"if\s*\(\s*(\w+)\s*==\s*(\w+)\s*\)\s*return\s+XML_NAMESPACE_(\w+)", ns_body)}
            for cm in re.finditer(r'case\s+(\w+)\s*:\s*return\s+"([^"]+)"', clean[b0:b1]):
                key = special.get(cm.group(1)) or default and default[0]
                if key:
                    self.record(path, line_of(b0 + cm.start()), key, [cm.group(2)], "attribute", cm.group(0))

        for mm in re.finditer(r"\b(%s)\s*\(" % "|".join(EMIT_ATTR), m):
            o = mm.end() - 1
            c = match_close(m, o)
            a = split_args(m, o, c)
            if len(a) < 2 or (len(a) == 2 and "XML_NAMESPACE_" not in m[a[0][0]:a[0][1]]):
                continue
            if DECL_ARG.match(m[a[0][0]:a[0][1]].strip()):
                continue
            pair_from(a[0][0], a[0][1], a[1][0], a[1][1], "attribute", mm.start(), c + 1, mm.start())

        for mm in re.finditer(r"\bSvXMLElementExport\b\s*(?:\w+\s*)?\(", m):
            o = mm.end() - 1
            c = match_close(m, o)
            a = split_args(m, o, c)
            k = 2 if len(a) >= 6 else 1
            if len(a) < 3 or len(a) <= k + 1 or DECL_ARG.match(m[a[k][0]:a[k][1]].strip()):
                continue
            pair_from(a[k][0], a[k][1], a[k + 1][0], a[k + 1][1], "element", mm.start(), c + 1, mm.start())

        for mm in re.finditer(r"\bAddAttributeIdLegacy\s*\(", m):
            o = mm.end() - 1
            c = match_close(m, o)
            a = split_args(m, o, c)
            if len(a) == 2 and not DECL_ARG.match(m[a[0][0]:a[0][1]].strip()):
                pair_from(a[0][0], a[0][1], 0, 0, "attribute", mm.start(), c + 1, mm.start(), ["id"])

        for mm in re.finditer(r"\bGetQNameByKey\s*\(", m):
            o = mm.end() - 1
            c = match_close(m, o)
            a = split_args(m, o, c)
            up = enclosing_call(m, mm.start())
            if up and up[0] in EMIT_ATTR and up[1] >= (1 if up[2] == 2 else 2):
                self.skipped.append((path, line_of(mm.start()), "QName used as an attribute value", snip(mm.start(), c + 1)))
                for k in NS_RE.finditer(m[a[0][0]:a[0][1]]):
                    handled.add(a[0][0] + k.start())
            elif len(a) == 2 and STR_RE.search(clean[a[1][0]:a[1][1]]) and not TOK_RE.search(m[a[1][0]:a[1][1]]):
                self.skipped.append((path, line_of(mm.start()), "QName with literal local name (a value)", snip(mm.start(), c + 1)))
            elif len(a) == 2:
                pair_from(a[0][0], a[0][1], a[1][0], a[1][1], "either", mm.start(), c + 1, mm.start(), wrapok=False)

        for name, mac in macros.items():
            for mm in re.finditer(r"\b%s\s*\(" % re.escape(name), m):
                o = mm.end() - 1
                c = match_close(m, o)
                a = split_args(m, o, c)
                if len(a) <= max(mac.ns_i or 0, mac.tok_i):
                    continue
                if in_tokmap(mm.start()):
                    continue
                row = snip(mm.start(), c + 1)
                ns = mac.ns_fixed or m[a[mac.ns_i][0]:a[mac.ns_i][1]].strip()
                tk = mac.tok_prefix + clean[a[mac.tok_i][0]:a[mac.tok_i][1]].strip()
                names, bad = self.names(tk, tk)
                if not re.fullmatch(r"[A-Z][A-Z0-9_]*", ns) or bad:
                    self.unres.append((path, line_of(mm.start()), "macro argument not a known key/token", row))
                    continue
                body = clean[mm.start():c]
                if NO_EXPORT.search(body):
                    self.skipped.append((path, line_of(mm.start()), "not exported (flags)", row))
                    continue
                role = "element" if ELEMENT_ROW.search(body) else "attribute" if PROP_ROW.search(body) else "either"
                self.record(path, line_of(mm.start()), ns, names, role, row)

        for mm in NS_RE.finditer(m):
            if mm.start() in handled:
                continue
            j = mm.end()
            while j < len(m) and m[j] in " \t\r\n":
                j += 1
            if j >= len(m) or m[j] != ",":
                continue
            br = opener.get(mm.start())
            if br is None:
                continue
            bi, bc = br
            before = re.search(r"(\w+)\s*(\w*)\s*$", m[max(0, bi - 80):bi])
            if m[bi] == "(" and before and (before.group(1) in EMIT_FUNCS or before.group(1) == "SvXMLElementExport"
                                            or before.group(2) and before.group(1) == "SvXMLElementExport"):
                continue
            a = split_args(m, bi, bc)
            idx = next(n for n, (s, e) in enumerate(a) if s <= mm.start() < e)
            if idx + 1 >= len(a) or m[a[idx][0]:a[idx][1]].strip() != mm.group(0):
                continue
            row_clean = clean[bi:bc + 1]
            if m[bi] == "{" and re.search(r"\bXML_TOK_\w+", row_clean):
                continue
            if EVENT_ROW.match(row_clean):
                self.skipped.append((path, line_of(mm.start()), "event-name translation (a value, not a name)", snip(bi, bc + 1)))
                continue
            if m[bi] == "{" or PROP_ROW.search(row_clean):
                role = "element" if ELEMENT_ROW.search(row_clean) else "attribute"
                if not PROP_ROW.search(row_clean):
                    role = "either"
            else:
                role = "either"
            pair_from(a[idx][0], a[idx][1], a[idx + 1][0], a[idx + 1][1], role, bi, bc + 1, mm.start(), wrapok=False)

    def ns_default(self, module, fname, ns_param_i):
        """Default namespace key(s) of the ns parameter of fname, from its declarations."""
        out = set()
        for p, (m, clean, text) in self.cache.items():
            if os.path.relpath(p, self.main).split(os.sep)[0] != module:
                continue
            for mm in re.finditer(r"\b%s\s*\(" % re.escape(fname), m):
                o = mm.end() - 1
                a = split_args(m, o, match_close(m, o))
                if ns_param_i < len(a):
                    d = re.search(r"=\s*XML_NAMESPACE_([A-Z][A-Z0-9_]*)", m[a[ns_param_i][0]:a[ns_param_i][1]])
                    if d:
                        out.add(d.group(1))
        return out

    def scan_wrapper_calls(self, path, discover):
        """Resolve calls of wrapper functions; with discover, only register forwarders."""
        m, clean, text = self.cache[path]
        module = os.path.relpath(path, self.main).split(os.sep)[0]
        line_of = lambda i: text.count("\n", 0, i) + 1
        found = 0
        for (mod, fname), ws in list(self.wrappers.items()):
            if mod != module:
                continue
            for mm in re.finditer(r"\b%s\s*\(" % re.escape(fname), m):
                o = mm.end() - 1
                c = match_close(m, o)
                a = split_args(m, o, c)
                a0 = m[a[0][0]:a[0][1]].strip()
                if DEF_TAIL.match(m, c + 1) or re.fullmatch(r"[\w:]+(?:\s+[\w:*&]+)+\s*(=.*)?", a0, re.S):
                    continue
                row = " ".join(clean[mm.start():c + 1].split())[:160]
                res, fwd = set(), []
                for w in ws:
                    if not (w.tok_i < len(a) <= w.nparams):
                        continue
                    ta, tb = a[w.tok_i]
                    names, bad = self.names(m[ta:tb], clean[ta:tb])
                    if w.keys:
                        keys = tuple(w.keys)
                    elif w.ns_param < len(a):
                        k = NS_RE.fullmatch(m[a[w.ns_param][0]:a[w.ns_param][1]].strip())
                        keys = (k.group(1),) if k else None
                    else:
                        d = self.ns_default(module, fname, w.ns_param)
                        keys = tuple(d) if len(d) == 1 else None
                    if bad and keys:
                        fwd.append((m[ta:tb].strip(), keys, w.role))
                    res.add(None if bad or not keys else (tuple(names), keys, w.role))
                if discover:
                    inner = [f for f in self.funcs[path] if f[2] < mm.start() < f[3]]
                    if inner and len(fwd) == 1 and len(res) == 1:
                        name, params, b0, b1 = min(inner, key=lambda f: f[3] - f[2])
                        if fwd[0][0] in params and name not in EMIT_FUNCS:
                            nw = Wrapper(params.index(fwd[0][0]), len(params), list(fwd[0][1]), None, fwd[0][2])
                            old = self.wrappers[(module, name)]
                            if not any(x.tok_i == nw.tok_i and x.nparams == nw.nparams and x.keys == nw.keys for x in old):
                                old.append(nw)
                                found += 1
                    continue
                if len(res) != 1 or None in res:
                    if res and not (len(fwd) == 1 and any(f[2] < mm.start() < f[3] and fwd[0][0] in f[1] for f in self.funcs[path])):
                        self.unres.append((path, line_of(mm.start()), "wrapper %s() call not resolvable" % fname, row))
                    continue
                names, keys, role = res.pop()
                for k in keys:
                    self.record(path, line_of(mm.start()), k, list(names), role, row)
        return found

    @staticmethod
    def openers(m):
        """Map every XML_NAMESPACE_ match start to its innermost enclosing bracket."""
        starts = {x.start() for x in NS_RE.finditer(m)}
        out, stack = {}, []
        pairs = {}
        for i, c in enumerate(m):
            if c in "({":
                stack.append(i)
            elif c in ")}":
                if stack:
                    pairs[stack.pop()] = i
            elif i in starts:
                out[i] = stack[-1] if stack else None
        res = {}
        for s, o in out.items():
            if o is not None and o in pairs:
                res[s] = (o, pairs[o])
        return res

    def write(self, outdir):
        os.makedirs(outdir, exist_ok=True)
        rows = []
        for (uri, name), e in self.pairs.items():
            known = e["roles"] - {"either"}
            role = known.pop() if len(known) == 1 else "either"
            ex = sorted(e["ex"], key=lambda x: (x[2] == "either", x[0], x[1]))[0]
            rows.append((uri, name, role, ",".join(sorted(e["mods"])), "%s:%d" % ex[:2]))
        rows.sort()
        with open(os.path.join(outdir, "emit.tsv"), "w") as f:
            f.write("# ODF export names found statically in %d export-side C++ files under main/.\n" % len(self.files))
            f.write("# Rule: .cxx/.hxx/.h under main/ whose name has no 'imp'/'import' (impastp4.cxx kept), outside test/qa/workben/unotest/unoxml/solver,"
                    " xmloff/source/transform and filter/source/svg; shared tables (property maps, handlers) included;"
                    " import token-map rows (XML_TOK_) and rows flagged no-export dropped.\n")
            f.write("# role: element/attribute from the emitting call or property-map flags; either = unknown or seen as both."
                    " Empty ns_uri = XML_NAMESPACE_NONE.\n")
            f.write("# ns_uri\tlocal\trole\tmodules\texample_file:line\n")
            for r in rows:
                f.write("\t".join(r) + "\n")
        for name, lst in (("emit-unresolved.tsv", self.unres), ("emit-skipped.tsv", self.skipped)):
            with open(os.path.join(outdir, name), "w") as f:
                f.write("# file:line\treason\tsnippet\n")
                for p, ln, why, row in sorted(set(lst)):
                    f.write("%s:%d\t%s\t%s\n" % (os.path.relpath(p, self.main), ln, why, row))
        return rows


def main(argv):
    if len(argv) != 3:
        sys.exit(__doc__)
    sc = Scanner(argv[1])
    sc.prepare()
    for p in sc.files:
        sc.scan_file(p)
    for _ in range(5):
        if not sum(sc.scan_wrapper_calls(p, True) for p in sc.files):
            break
    for p in sc.files:
        sc.scan_wrapper_calls(p, False)
    rows = sc.write(argv[2])
    roles = collections.Counter(r[2] for r in rows)
    print("tokens in xmltoken.cxx: %d; files scanned: %d" % (len(sc.toks), len(sc.files)))
    print("distinct pairs: %d %s" % (len(rows), dict(roles)))
    print("unresolved sites: %d; skipped rows: %d" % (len(set(sc.unres)), len(set(sc.skipped))))


if __name__ == "__main__":
    main(sys.argv)
