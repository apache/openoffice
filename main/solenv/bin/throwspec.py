#!/usr/bin/env python3
# throwspec.py — find and remove C++ dynamic exception specifications.
#
# Usage:  throwspec.py report            classify every site, change nothing
#         throwspec.py apply             delete the specifications with a type list
#         throwspec.py verify            check a finished rewrite against HEAD
#
#         --root <dir>   repository root (default: two levels above this script)
#
# This is a ONE-SHOT SOURCE MIGRATION TOOL, not a build step.  No Bazel rule
# refers to it and it is deliberately absent from BUILD.bazel: it edits the
# source tree in place and is run by hand.  It lives here so the next sweep does
# not have to re-derive the classification rules below, which took several
# wrong attempts to get right.
#
# WHAT IT SEPARATES.  A regular expression cannot tell these apart -- both are
# the token `throw', whitespace, `(', a name, `)':
#
#     void foo( int n ) throw (RuntimeException);    // specification -> delete
#     if ( bad ) throw (UINT) ERROR_ALREADY_RUNNING; // statement     -> keep
#
# A site counts as a specification only when ALL FOUR of these hold:
#
#   1. the argument parses as a comma-separated list of type names, allowing
#      `typename', template arguments, and a macro line-continuation INSIDE the
#      list;
#   2. the previous significant token is `)', `const' or `volatile', where a
#      `\'+newline is skipped as whitespace -- without that, every specification
#      written inside a #define body looks unanchored, because the declarator's
#      closing paren is on the previous physical line;
#   3. the argument is non-empty (an EMPTY specification is a separate task --
#      MSVC implements throw() as __declspec(nothrow), so it means something);
#   4. the FOLLOWING token is one of `{ ; = : , ) #'.  This is the rule that
#      catches a throw statement whose operand carries a C-style cast; exactly
#      one site in the tree needs it (desktop/win32/source/setup/setup_main.cxx).
#      An identifier may follow only across a line break, which is where a macro
#      whose body ends in a specification meets the next line.
#
# Everything the classifier rejects is reported, never silently skipped.  There
# were 30 such sites when this removed 72,490 specifications; read all of them.
#
# WHY `verify' EXISTS.  The rewrite only ever deletes, which gives a free
# structural invariant: per file the counts of `{', `}' and `;' must be
# unchanged, and `(' and `)' must drop by the same amount.  That check covers all
# of a five-thousand-file diff in seconds and is what makes a sweep this size
# reviewable at all.  Run it before asking anyone to build.
#
# It compares the working tree against HEAD, so any file you ALSO edited by hand
# in the same change will appear in its output -- that is the point.  Reconcile
# the list against the files you touched deliberately; it should not be empty,
# it should be exactly those.
#
# FOR THE NEXT SWEEP (throw() -> noexcept): the classifier already reports empty
# specifications as kind EMPTY.  Select those instead of SPEC, and replace
# `rewrite()' with a substituting variant -- the span bookkeeping is the same,
# only the replacement text differs.

import collections
import os
import re
import subprocess
import sys
import threading

CODE, COMMENT, LITERAL = 1, 2, 3

SOURCE_EXTS = ('.hxx', '.cxx', '.hpp', '.hdl', '.inl', '.h', '.c', '.cc',
               '.cpp', '.mm')


# ── lexical pass ─────────────────────────────────────────────────────────

def classify(text):
    """Per-character map: CODE / COMMENT / LITERAL (string or char literal)."""
    n = len(text)
    m = bytearray(n)
    i = 0
    while i < n:
        c = text[i]
        if c == '/' and i + 1 < n and text[i + 1] == '/':
            start = i
            i += 2
            while i < n:
                if text[i] == '\\':          # continuation keeps the comment open
                    j = i + 1
                    if j < n and text[j] == '\r':
                        j += 1
                    if j < n and text[j] == '\n':
                        i = j + 1
                        continue
                    i += 1
                    continue
                if text[i] == '\n':
                    break
                i += 1
            for k in range(start, i):
                m[k] = COMMENT
            continue
        if c == '/' and i + 1 < n and text[i + 1] == '*':
            start = i
            i += 2
            while i + 1 < n and not (text[i] == '*' and text[i + 1] == '/'):
                i += 1
            i = min(i + 2, n)
            for k in range(start, i):
                m[k] = COMMENT
            continue
        if c == '"' or c == "'":
            start = i
            q = c
            i += 1
            ok = False
            while i < n:
                if text[i] == '\\':
                    i += 2
                    continue
                if text[i] == '\n':
                    break
                if text[i] == q:
                    i += 1
                    ok = True
                    break
                i += 1
            if not ok:      # unterminated on its line: an apostrophe, not a literal
                i = start + 1
                m[start] = CODE
                continue
            for k in range(start, i):
                m[k] = LITERAL
            continue
        m[i] = CODE
        i += 1
    return m


def is_continuation(text, i):
    """True if text[i] is a macro line-continuation backslash."""
    if text[i] != '\\':
        return False
    j = i + 1
    while j < len(text) and text[j] in ' \t\r':
        j += 1
    return j < len(text) and text[j] == '\n'


def _skippable(text, m, i):
    return (m[i] == COMMENT
            or (m[i] == CODE and (text[i].isspace() or is_continuation(text, i))))


def skip_fwd(text, m, i):
    """First significant position at or after i (whitespace, comments and
    macro line-continuations are not significant)."""
    n = len(text)
    while i < n and _skippable(text, m, i):
        i += 1
    return i


def skip_back(text, m, i):
    """Last significant position at or before i."""
    while i >= 0 and _skippable(text, m, i):
        i -= 1
    return i


def match_paren(text, m, open_pos):
    """Index of the ')' matching the '(' at open_pos, or -1."""
    depth = 0
    n = len(text)
    i = open_pos
    while i < n:
        if m[i] == CODE:
            if text[i] == '(':
                depth += 1
            elif text[i] == ')':
                depth -= 1
                if depth == 0:
                    return i
        i += 1
    return -1


def strip_comments(text, m, a, b):
    return ''.join(text[k] for k in range(a, b) if m[k] != COMMENT)


# ── classification ───────────────────────────────────────────────────────

SEG = r'[A-Za-z_]\w*(?:\s*<[^<>()]*>)?'
NAME = r'(?:\s*typename\b)?(?:\s*::)?\s*%s(?:\s*::\s*%s)*\s*' % (SEG, SEG)
TYPELIST = re.compile(r'^%s(?:,%s)*$' % (NAME, NAME))
CONTINUATION = re.compile(r'\\[ \t]*\r?\n')
KEYWORD = re.compile(r'\b(?:SAL_THROW_EXTERN_C|SAL_THROW_DTOR|SAL_THROW|throw)\b')
IDENT_BACK = re.compile(r'[A-Za-z_]\w*$')


def sites(path, text):
    """Yield a dict per specification-shaped site.

    kind is one of:
      SPEC                     a specification with a type list -- rewrite this
      EMPTY                    throw() / SAL_THROW( () ) / SAL_THROW_EXTERN_C()
      STATEMENT                a throw statement; the argument is an expression
      REVIEW_UNANCHORED        type list, but not attached to a declarator
      REVIEW_FOLLOWED_BY_EXPR  type list, but an expression follows it
      ODD_SAL_THROW            SAL_THROW without the inner parentheses
      UNBALANCED               unmatched parenthesis
    """
    m = classify(text)
    for mo in KEYWORD.finditer(text):
        s = mo.start()
        if m[s] != CODE:
            continue
        kw = mo.group(0)
        op = skip_fwd(text, m, mo.end())
        if op >= len(text) or text[op] != '(':
            continue
        cl = match_paren(text, m, op)
        line = text.count('\n', 0, s) + 1
        if cl < 0:
            yield dict(kind='UNBALANCED', kw=kw, start=s, end=mo.end(),
                       path=path, line=line, content='')
            continue

        if kw in ('throw', 'SAL_THROW_EXTERN_C'):
            inner_a, inner_b = op + 1, cl
        else:
            # SAL_THROW( (...) ): unwrap the inner parentheses
            a = skip_fwd(text, m, op + 1)
            if a >= cl or text[a] != '(':
                yield dict(kind='ODD_SAL_THROW', kw=kw, start=s, end=cl + 1,
                           path=path, line=line, content='')
                continue
            b = match_paren(text, m, a)
            if b < 0 or skip_fwd(text, m, b + 1) != cl:
                yield dict(kind='ODD_SAL_THROW', kw=kw, start=s, end=cl + 1,
                           path=path, line=line, content='')
                continue
            inner_a, inner_b = a + 1, b

        content = strip_comments(text, m, inner_a, inner_b)
        if not content.strip():
            yield dict(kind='EMPTY', kw=kw, start=s, end=cl + 1, path=path,
                       line=line, content='')
            continue
        content = CONTINUATION.sub(' ', content)   # a spec may span macro lines

        prev = skip_back(text, m, s - 1)
        prev_ch = text[prev] if prev >= 0 else ''
        ident = IDENT_BACK.search(text[max(0, prev - 32):prev + 1])
        prev_word = ident.group(0) if ident else ''
        anchored = prev_ch == ')' or prev_word in ('const', 'volatile')
        listish = bool(TYPELIST.match(content))

        after = skip_fwd(text, m, cl + 1)
        followed_ok = (after >= len(text) or text[after] in '{;=:,)#'
                       or '\n' in text[cl + 1:after])

        if kw != 'throw':
            kind = 'SPEC' if listish else 'ODD_SAL_THROW'
        elif not listish:
            kind = 'STATEMENT'
        elif not anchored:
            kind = 'REVIEW_UNANCHORED'
        elif not followed_ok:
            kind = 'REVIEW_FOLLOWED_BY_EXPR'
        else:
            kind = 'SPEC'

        yield dict(kind=kind, kw=kw, start=s, end=cl + 1, path=path, line=line,
                   content=content.strip(), prev_ch=prev_ch, prev_word=prev_word)


# ── rewriting ────────────────────────────────────────────────────────────

BLANK_LINE = re.compile(r'[ \t]*(\\?)[ \t]*\r?$')


def _drop_emptied_line(out, pos):
    """Drop the line at pos if deleting the specification left it blank.

    A macro's continuation backslash must not change hands: remove the line only
    when its own trailing backslash (if any) matches the previous line's, so a
    macro body can never absorb the line that followed it.
    """
    ls = out.rfind('\n', 0, pos) + 1
    le = out.find('\n', pos)
    if le < 0:
        le = len(out)
    mo = BLANK_LINE.fullmatch(out[ls:le])
    if not mo:
        return out
    ps = out.rfind('\n', 0, ls - 1) + 1 if ls else 0
    prev_continues = out[ps:max(ls - 1, ps)].rstrip('\r').endswith('\\')
    if bool(mo.group(1)) != prev_continues:
        return out
    return out[:ls] + out[le + 1:] if le < len(out) else out[:ls]


def rewrite(text, spans):
    """Delete the given (start, end) spans, closing up the line behind them."""
    m = classify(text)
    out = text
    joined = 0
    for start, end in sorted(spans, reverse=True):
        d = start
        while d > 0 and out[d - 1] in ' \t\r\n':
            d -= 1
        # Never swallow whitespace that hangs off a comment (joining the rest of
        # the line onto a // comment would comment it out) nor off a macro's
        # line-continuation backslash (which would leave a stray one mid-line).
        if d > 0 and (m[d - 1] != CODE or out[d - 1] == '\\'):
            d = start
        elif '\n' in out[d:start]:
            joined += 1
        out = _drop_emptied_line(out[:d] + out[end:], d)
    return out, joined


# ── drivers ──────────────────────────────────────────────────────────────

def tracked_sources(root):
    out = subprocess.run(['git', 'ls-files', 'main/'], cwd=root,
                         capture_output=True, text=True, check=True).stdout
    for p in out.splitlines():
        if p.lower().endswith(SOURCE_EXTS):
            yield p


def read(root, rel):
    # latin-1 is a byte<->char bijection, so line endings and any non-UTF-8
    # bytes round-trip untouched.  Every token this tool matches is ASCII.
    with open(os.path.join(root, rel.replace('/', os.sep)),
              encoding='latin-1', newline='') as f:
        return f.read()


def write(root, rel, text):
    with open(os.path.join(root, rel.replace('/', os.sep)), 'w',
              encoding='latin-1', newline='') as f:
        f.write(text)


def run_report(root, apply_changes):
    counts = collections.Counter()
    review = collections.defaultdict(list)
    changed = joins = 0
    for rel in tracked_sources(root):
        text = read(root, rel)
        if 'throw' not in text and 'SAL_THROW' not in text:
            continue
        spans = []
        for s in sites(rel, text):
            counts[s['kind']] += 1
            if s['kind'] == 'SPEC':
                spans.append((s['start'], s['end']))
            elif s['kind'] != 'EMPTY':
                review[s['kind']].append(s)
        if spans and apply_changes:
            new, j = rewrite(text, spans)
            if new != text:
                write(root, rel, new)
                changed += 1
                joins += j
    for kind, n in counts.most_common():
        print('%-24s %7d' % (kind, n))
    if apply_changes:
        print('\n%d files rewritten, %d lines closed up' % (changed, joins))
    print('\nEvery site below was rejected by the classifier.  Read all of them:')
    for kind in sorted(review):
        for s in review[kind]:
            print('  %-24s %s:%d  <%s>'
                  % (kind, s['path'], s['line'],
                     ' '.join(s['content'].split())[:70]))
    return 0


def run_verify(root):
    """Confirm a finished rewrite deleted only, and broke no nesting."""
    files = subprocess.run(['git', 'diff', '--name-only'], cwd=root,
                           capture_output=True, text=True, check=True).stdout.split()
    files = [f for f in files if f.lower().endswith(SOURCE_EXTS)]
    p = subprocess.Popen(['git', 'cat-file', '--batch'], cwd=root,
                         stdin=subprocess.PIPE, stdout=subprocess.PIPE)

    # Feed from a thread: writing every request up front deadlocks once git's
    # stdout fills the pipe while it is still blocked reading stdin.
    def feed():
        for f in files:
            p.stdin.write(('HEAD:%s\n' % f).encode())
        p.stdin.close()
    threading.Thread(target=feed, daemon=True).start()

    bad = []
    for rel in files:
        size = int(p.stdout.readline().split()[2])
        old = p.stdout.read(size)
        p.stdout.read(1)
        with open(os.path.join(root, rel.replace('/', os.sep)), 'rb') as f:
            new = f.read()
        problems = ['%s %d->%d' % (c, old.count(c.encode()), new.count(c.encode()))
                    for c in '{};'
                    if old.count(c.encode()) != new.count(c.encode())]
        if (old.count(b'(') - new.count(b'(')) != (old.count(b')') - new.count(b')')):
            problems.append('parentheses removed unevenly')
        if problems:
            bad.append((rel, problems))
    print('checked %d files, %d with problems' % (len(files), len(bad)))
    for rel, problems in bad:
        print('  %s  %s' % (rel, ', '.join(problems)))
    return 1 if bad else 0


def main(argv):
    mode = argv[1] if len(argv) > 1 else ''
    root = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
    if '--root' in argv:
        root = argv[argv.index('--root') + 1]
    if mode == 'report':
        return run_report(root, False)
    if mode == 'apply':
        return run_report(root, True)
    if mode == 'verify':
        return run_verify(root)
    sys.stderr.write('usage: throwspec.py {report|apply|verify} [--root <dir>]\n'
                     '  report  classify every site, change nothing\n'
                     '  apply   delete the specifications with a type list\n'
                     '  verify  check a finished rewrite against HEAD\n')
    return 2


if __name__ == '__main__':
    sys.exit(main(sys.argv))
