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

# usage: formulacheck.py <outdir> <formulas.json>
# ODF 1.2 Part 1 2.2.1 D.3 and 2.2.4 D/E: the style:condition, table:condition, table:expression,
# table:formula and text:formula values are parsed with the OpenFormula grammar (Part 2 chapter 5)
# and the condition syntaxes of Part 1 19.468 and 19.596. Syntax only: no semantics, no function arity.
# Findings are "shall: [rule] ..." or "should: [rule] ...".
import sys, os, re, json, zipfile, collections, unicodedata
from xml.parsers import expat

D = os.path.dirname(os.path.abspath(__file__))
OF = "urn:oasis:names:tc:opendocument:xmlns:of:1.2"
NS = "urn:oasis:names:tc:opendocument:xmlns:%s:1.0"
ATTRS = {NS % "style" + " condition": "style:condition", NS % "table" + " condition": "table:condition",
         NS % "table" + " expression": "table:expression", NS % "table" + " formula": "table:formula",
         NS % "text" + " formula": "text:formula"}
SPREADSHEET = NS % "office" + " spreadsheet"
STANDARD = {l.strip().upper() for l in open(os.path.join(D, "openformula-functions.txt")) if l.strip() and not l.startswith("#")}


class Bad(Exception):
    def __init__(self, pos, msg):
        Exception.__init__(self, msg)
        self.pos, self.msg = pos, msg


def is_letter(c):
    return unicodedata.category(c)[0] == "L" or unicodedata.category(c) == "Nl"


def is_namechar(c):
    return is_letter(c) or unicodedata.category(c) in ("Nd", "Mn", "Mc", "Me") or c == "_"


# Part 2 5.3, 5.4, 5.8, 5.12
NUMBER = re.compile(r"(?:[0-9]+(?:\.[0-9]+)?|\.[0-9]+)(?:[eE][-+]?[0-9]+)?")
STRING = re.compile(r'"(?:[^"\x00]|"")*"')
SINGLE = re.compile(r"'(?:[^']|'')+'")
ERROR = re.compile(r"#[A-Z0-9]+(?:[!?]|/(?:[A-Z]|[0-9][!?]))")
_Q = r"\$?'(?:[^']|'')+'"
_SN = r"(?:%s|\$?[^\]\. #$']+)" % _Q
_COL, _ROW = r"\$?[A-Z]+", r"\$?[1-9][0-9]*"
_CELL = _COL + _ROW
_SL = r"%s(?:\.(?:%s|%s))*" % (_SN, _CELL, _Q)
RANGE = re.compile("|".join([
    r"(?:%s)?\.%s(?::\.%s)?" % (_SL, _CELL, _CELL), r"(?:%s)?\.%s:\.%s" % (_SL, _COL, _COL), r"(?:%s)?\.%s:\.%s" % (_SL, _ROW, _ROW),
    r"%s\.%s:%s\.%s" % (_SL, _CELL, _SL, _CELL), r"%s\.%s:%s\.%s" % (_SL, _COL, _SL, _COL), r"%s\.%s:%s\.%s" % (_SL, _ROW, _SL, _ROW)]))
QSHEET = re.compile(_Q)
CELLREF = re.compile(r"[A-Za-z]+[0-9]+")
INFIX = ("<>", "<=", ">=", "+", "-", "*", "/", "^", "=", "<", ">", "&", ":", "!", "~")
# Part 1 19.468, 19.596
DECIMAL = re.compile(r"[-+]?(?:[0-9]+(?:\.[0-9]*)?|\.[0-9]+)")
RELOP = re.compile(r"<=|>=|!=|<|>|=")
CNAME = re.compile(r"[a-z][a-z-]*")


def check_iri(iri):
    """Approximates the RFC 3987 IRI-reference rule: no controls, spaces or the characters excluded from IRIs."""
    if re.search(r'[\x00-\x20<>"{}|\\^`\x7f]', iri) or re.search(r"%(?![0-9A-Fa-f]{2})", iri):
        raise ValueError("not an IRI reference")


class Parser:
    def __init__(self, text):
        self.s, self.i, self.funcs, self.ragged = text, 0, [], None

    def fail(self, msg, at=None):
        raise Bad(self.i if at is None else at, msg)

    def at(self, chars):
        return self.i < len(self.s) and self.s[self.i] in chars

    def ws(self):
        while self.at(" \t\n\r"):
            self.i += 1

    def expect(self, ch):
        if not self.at(ch):
            self.fail("%r expected" % ch)
        self.i += 1

    def end(self):
        if self.i < len(self.s):
            self.fail("unexpected %r" % self.s[self.i])

    # 5.2: Formula ::= Intro? Expression
    def formula(self):
        if self.at("="):
            self.i += 1
            if self.at("="):
                self.i += 1
        self.expression()
        self.end()

    def expression(self):
        self.ws()
        self.operand()
        while True:
            self.ws()
            op = next((o for o in INFIX if self.s.startswith(o, self.i)), None)
            if op is None:
                return
            self.i += len(op)
            self.ws()
            self.operand()

    def operand(self):
        while self.at("+-"):
            self.i += 1
            self.ws()
        self.primary()
        while True:
            j = self.i
            self.ws()
            if self.at("%"):
                self.i += 1
            else:
                self.i = j
                return

    def primary(self):
        s, i = self.s, self.i
        if i >= len(s):
            self.fail("expression expected")
        c = s[i]
        m = NUMBER.match(s, i) if (c in "0123456789.") else None
        if m:
            self.i = m.end()
        elif c == '"':
            m = STRING.match(s, i)
            if not m:
                self.fail("unterminated string")
            self.i = m.end()
        elif c == "{":
            self.array()
        elif c == "(":
            self.i += 1
            self.expression()
            self.expect(")")
        elif c == "[":
            self.reference()
        elif c == "#":
            m = ERROR.match(s, i)
            if not m:
                self.fail("malformed error constant")
            self.i = m.end()
        elif c in "'$":
            self.quoted_or_named()
        elif is_letter(c):
            self.name()
        else:
            self.fail("unexpected %r" % c)

    def array(self):
        self.i += 1
        widths = []
        while True:
            n = 1
            self.expression()
            while self.at(";"):
                self.i += 1
                self.expression()
                n += 1
            widths.append(n)
            if self.at("|"):
                self.i += 1
                continue
            break
        self.expect("}")
        if len(set(widths)) > 1:
            self.ragged = widths

    def name(self):
        s, j = self.s, self.i
        k = j
        while k < len(s) and (is_namechar(s[k]) or s[k] == "."):
            k += 1
        if k < len(s) and s[k] == "(":
            self.funcs.append(s[j:k])
            self.i = k + 1
            self.params()
            return
        ident = s[j:k].split(".", 1)[0]
        self.i = j + len(ident)
        if self.i == k and re.match(r"[ \t\n\r]+\(", s[k:]):
            self.fail("whitespace between function name and '('")
        self.check_identifier(ident)

    def check_identifier(self, ident):
        if CELLREF.fullmatch(ident):
            self.fail("%r looks like a cell reference; references are written in square brackets" % ident, self.i - len(ident))
        if ident.lower() in ("true", "false"):
            self.fail("%r is not an identifier; use %s()" % (ident, ident.upper()), self.i - len(ident))

    def params(self):
        while True:
            self.ws()
            if self.at(");"):
                pass
            else:
                self.expression()
                self.ws()
            if self.at(";"):
                self.i += 1
                continue
            self.expect(")")
            return

    # 5.8, 5.10, 5.11
    def source(self):
        """Source ::= "'" IRI "'" "#" at the current position, if there is one."""
        m = SINGLE.match(self.s, self.i)
        if m and self.s.startswith("#", m.end()):
            try:
                check_iri(m.group(0)[1:-1].replace("''", "'"))
            except ValueError as e:
                self.fail("source: %s" % e)
            self.i = m.end() + 1
            return True
        return False

    def simple_named(self):
        """SimpleNamedExpression ::= Identifier | '$$' (Identifier | SingleQuoted)"""
        if self.s.startswith("$$", self.i):
            self.i += 2
            m = SINGLE.match(self.s, self.i)
            if m:
                self.i = m.end()
                return
        self.identifier()

    def quoted_or_named(self):
        s = self.s
        if self.source():
            m = QSHEET.match(s, self.i)
            if m and s.startswith(".", m.end()):
                self.i = m.end() + 1
            self.simple_named()
            return
        if s.startswith("$$", self.i):
            self.simple_named()
            return
        m = QSHEET.match(s, self.i)
        if not m:
            self.fail("'$' must be '$$' or precede a quoted sheet name")
        j = m.end()
        k = j
        while k < len(s) and s[k] in " \t\n\r":
            k += 1
        if s.startswith(".", j):
            self.i = j + 1
            self.simple_named()
        elif s[self.i] != "$" and s.startswith("!!", k):
            self.i = k + 2
            self.ws()
            m = SINGLE.match(s, self.i)
            if not m:
                self.fail("quoted label expected after '!!'")
            self.i = m.end()
        elif s[self.i] == "$":
            self.fail("a quoted sheet name must be followed by '.'")
        else:
            self.i = j

    def identifier(self):
        s, j = self.s, self.i
        if j >= len(s) or not is_letter(s[j]):
            self.fail("identifier expected")
        k = j
        while k < len(s) and is_namechar(s[k]):
            k += 1
        self.i = k
        self.check_identifier(s[j:k])

    def reference(self):
        s = self.s
        j = self.i + 1
        k = j
        while k < len(s) and s[k] != "]":
            if s[k] == "'":
                m = SINGLE.match(s, k)
                if not m:
                    self.fail("unterminated quote in reference")
                k = m.end()
            else:
                k += 1
        if k >= len(s):
            self.fail("reference without ']'", self.i)
        inner = s[j:k]
        sub = Parser(inner)
        sub.i = 0
        try:
            if inner == "#REF!":
                pass
            else:
                sub.source()
                if not RANGE.fullmatch(inner, sub.i):
                    raise Bad(j + sub.i, "malformed reference [%s]" % inner)
        except Bad as e:
            raise Bad(j + e.pos if e.pos < len(inner) else j, e.msg)
        self.i = k + 1

    # Part 1 19.468 / 19.596 value, condition
    def value(self):
        s, save = self.s, self.i
        for rx in (DECIMAL, STRING):
            m = rx.match(s, save)
            if m:
                j = m.end()
                while j < len(s) and s[j] in " \t\n\r":
                    j += 1
                if j >= len(s) or s[j] in ",)":
                    self.i = j
                    return
        self.i = save
        self.expression()

    def cname(self):
        self.ws()
        m = CNAME.match(self.s, self.i)
        if not m:
            self.fail("condition expected")
        self.i = m.end()
        return m.group(0)

    def call(self, nargs):
        """'(' value {',' value} ')' or '()'"""
        self.expect("(")
        self.ws()
        if nargs == 0:
            self.expect(")")
            return
        for n in range(nargs):
            if n:
                self.ws()
                self.expect(",")
            self.ws()
            self.value()
        self.ws()
        self.expect(")")

    def op_value(self):
        self.ws()
        if not RELOP.match(self.s, self.i):
            self.fail("relational operator expected")
        self.i = RELOP.match(self.s, self.i).end()
        self.ws()
        self.value()

    def style_condition(self):
        name = self.cname()
        if name in ("footnote", "endnote", "header", "footer", "section", "table", "table-header"):
            self.call(0)
        elif name in ("list-level", "outline-level"):
            self.call(0)
            self.ws()
            self.expect("=")
            self.ws()
            m = re.compile(r"\+?0*[1-9][0-9]*").match(self.s, self.i)
            if not m:
                self.fail("positive integer expected")
            self.i = m.end()
        elif name == "value":
            self.call(0)
            self.ws()
            m = RELOP.match(self.s, self.i)
            if not m:
                self.fail("relational operator expected")
            self.i = m.end()
            self.ws()
            m = re.compile(r"true|false").match(self.s, self.i) or DECIMAL.match(self.s, self.i)
            if not m:
                self.fail("number, true or false expected")
            self.i = m.end()
        elif name == "cell-content":
            self.call(0)
            self.op_value()
        elif name in ("cell-content-is-between", "cell-content-is-not-between"):
            self.call(2)
        elif name == "is-true-formula":
            self.call_expression()
        else:
            self.fail("unknown condition %r" % name, self.i - len(name))
        self.ws()
        self.end()

    def call_expression(self):
        self.expect("(")
        self.expression()
        self.expect(")")

    def value_condition(self):
        name = self.cname()
        if name == "cell-content":
            self.call(0)
            self.op_value()
        elif name in ("cell-content-is-between", "cell-content-is-not-between"):
            self.call(2)
        elif name == "is-true-formula":
            self.call_expression()
        else:
            self.fail("value condition expected, found %r" % name, self.i - len(name))

    def string_list(self):
        """list of string entries separated by ';'; leaves the position before ')'"""
        s, j = self.s, self.i
        while True:
            m = STRING.match(s, j)
            if not m:
                return False
            k = m.end()
            while k < len(s) and s[k] in " \t\n\r":
                k += 1
            if s.startswith(";", k):
                j = k + 1
                while j < len(s) and s[j] in " \t\n\r":
                    j += 1
            elif s.startswith(")", k):
                self.i = k
                return True
            else:
                return False

    def validation_condition(self):
        name = self.cname()
        if name == "cell-content-text-length":
            self.call(0)
            self.op_value()
        elif name in ("cell-content-text-length-is-between", "cell-content-text-length-is-not-between"):
            self.call(2)
        elif name == "cell-content-is-in-list":
            self.expect("(")
            self.ws()
            if not self.string_list():
                self.expression()
            self.expect(")")
        elif name in ("cell-content-is-decimal-number", "cell-content-is-whole-number", "cell-content-is-date", "cell-content-is-time"):
            self.call(0)
            m = re.compile(r"\s+and\s+").match(self.s, self.i)
            if not m:
                self.fail("'and' expected")
            self.i = m.end()
            self.value_condition()
        elif name == "is-true-formula":
            self.call_expression()
        else:
            self.fail("unknown condition %r" % name, self.i - len(name))
        self.ws()
        self.end()


def check_value(attr, body, has_prefix):
    """Returns (None | (pos, message), [function names], ragged column counts | None)."""
    p = Parser(body)
    try:
        if attr in ("table:formula", "text:formula"):
            p.formula()
        elif attr == "table:expression":
            if has_prefix and not body.startswith("="):
                raise Bad(0, "an expression with a namespace prefix must start with '=' (19.635)")
            p.formula()
        elif attr == "style:condition":
            p.style_condition()
        else:
            p.validation_condition()
    except Bad as e:
        return (e.pos, e.msg), p.funcs, p.ragged
    return None, p.funcs, p.ragged


NCPREFIX = re.compile(r"([^\W\d][\w.\-]*):")


def check_stream(data, stats):
    """Returns [(line, col, level, rule, message)]."""
    out = []
    scope = collections.defaultdict(list)
    state = {"depth": 0, "ss": None}
    p = expat.ParserCreate(namespace_separator=" ")
    p.StartNamespaceDeclHandler = lambda pre, uri: scope[pre].append(uri)
    p.EndNamespaceDeclHandler = lambda pre: scope[pre].pop()

    def start(name, attrs):
        state["depth"] += 1
        if name == SPREADSHEET:
            state["ss"] = state["depth"]
        for key, value in attrs.items():
            attr = ATTRS.get(key)
            if attr is None:
                continue
            line, col = p.CurrentLineNumber, p.CurrentColumnNumber

            def add(level, rule, msg):
                out.append((line, col, level, rule, '%s="%s": %s' % (attr, value if len(value) < 80 else value[:77] + "...", msg)))
            m = NCPREFIX.match(value)
            body, has_prefix = value, False
            if m:
                pre = m.group(1)
                uri = scope[pre][-1] if scope.get(pre) else None
                stats[(attr, pre if uri is None else uri)] += 1
                if uri is None:
                    add("shall", "formula-prefix-unbound", "namespace prefix \"%s\" is not bound" % pre)
                    continue
                if uri != OF:
                    if attr == "table:formula" and state["ss"] is not None:
                        add("shall", "formula-namespace", "bound to <%s>, not to the of:1.2 namespace (2.2.4 D)" % uri)
                    else:
                        add("should", "formula-foreign-namespace", "bound to <%s>; the syntax is not defined by Part 2" % uri)
                    continue
                body, has_prefix = value[m.end():], True
            else:
                stats[(attr, "(none)")] += 1
            err, funcs, ragged = check_value(attr, body, has_prefix)
            if err:
                pos, msg = err
                add("shall", "formula-syntax", "%s at character %d" % (msg, pos + (len(value) - len(body)) + 1))
            if ragged:
                add("should", "formula-array-ragged", "inline array rows have %s columns (5.13)" % "/".join(map(str, ragged)))
            for f in funcs:
                if f.upper() not in STANDARD and "." not in f:
                    add("should", "formula-function-unprefixed", "%s is not a function of Part 2 and has no origin prefix (5.7)" % f)
                if f != f.upper():
                    add("should", "formula-function-case", "%s: function names should be written in upper case (6.1)" % f)
    p.StartElementHandler = start

    def end(name):
        if state["ss"] == state["depth"]:
            state["ss"] = None
        state["depth"] -= 1
    p.EndElementHandler = end
    try:
        p.Parse(data, True)
    except expat.ExpatError as e:
        out.append((e.lineno, e.offset, "shall", "formula-stream", "not namespace-well-formed: " + expat.ErrorString(e.code)))
    return out


def main():
    outdir, fjson = sys.argv[1], sys.argv[2]
    if not os.path.abspath(fjson).startswith(os.path.join(D, "runs") + os.sep):
        sys.exit("formulacheck.py: <formulas.json> must live under " + os.path.join(D, "runs"))
    errors, bad, stats = [], [], collections.Counter()
    pkgs = sorted(p for p in os.listdir(outdir) if not p.startswith("."))
    for pkg in pkgs:
        try:
            z = zipfile.ZipFile(os.path.join(outdir, pkg))
        except Exception:
            bad.append(pkg); continue
        for name in z.namelist():
            if name.rsplit("/", 1)[-1] not in ("content.xml", "styles.xml", "settings.xml"):
                continue
            for line, col, level, rule, msg in check_stream(z.read(name), stats):
                errors.append([pkg + "/" + name, line, col, "%s: [%s] %s" % (level, rule, msg)])
    json.dump({"packages": pkgs, "bad": bad, "checked": {"%s %s" % k: v for k, v in sorted(stats.items())}, "errors": errors}, open(fjson, "w"))
    shall = [e for e in errors if e[3].startswith("shall:")]
    print(f"formula check: {sum(stats.values())} values checked; {len(shall)} shall and {len(errors) - len(shall)} should findings in "
          f"{len({e[0].split('/')[0] for e in errors})} of {len(pkgs)} packages, unreadable: {bad}")
    for (attr, ns), v in sorted(stats.items()):
        print(f"  {v:6d} {attr} {ns}")
    rules = collections.Counter(e[3].split("]")[0] + "]" for e in errors)
    for k, v in sorted(rules.items()):
        print(f"  {v:6d} {k}")


if __name__ == "__main__":
    main()
