#!/usr/bin/env python3
"""uno_lsp.py -- a language server for the UNO layer, over the fact graph.

Answers the questions clangd structurally cannot, because they leave C++: the
binding between a service-name string literal and the code that implements it
lives in registry XML and the build graph, not in any translation unit.

    hover      on "com.sun.star.frame.Desktop"  -> interfaces, impl, library,
                                                   Bazel target, staged path
    definition on a service string              -> the .component that
                                                   registers it, at the
                                                   <implementation> line
    definition on a UNO type name               -> its .idl
    workspace/symbol                            -> search services/interfaces
    diagnostics                                 -> a service string that
                                                   nothing implements

Designed to run ALONGSIDE clangd, not instead of it: it claims no C++ knowledge
and only recognises UNO names, so the two do not overlap.

    uno_lsp.py --db uno.sqlite --workspace C:/workspace/openoffice

Speaks LSP over stdin/stdout.  Build the database first with uno_graph.py.
"""

import argparse
import json
import re
import sqlite3
import sys
import urllib.parse
import urllib.request
from pathlib import Path

# A UNO name as it appears in source: dotted, and rooted somewhere plausible.
# Deliberately conservative -- a false positive here means hijacking a hover
# that belongs to clangd.
_UNO_NAME = re.compile(r"\b(?:com|org|vnd)(?:\.[A-Za-z_][A-Za-z0-9_]*)+\b")
_STRING_LIT = re.compile(r'"([^"\\]*)"')

# Where IDL lives.  regview reports an empty "file name" for every type, so the
# .idl path is reconstructed by convention and confirmed on disk.
_IDL_ROOTS = ("main/offapi", "main/udkapi")


def uri_to_path(uri):
    p = urllib.parse.urlparse(uri)
    path = urllib.request.url2pathname(p.path)
    return path.lstrip("\\/") if re.match(r"^[\\/][A-Za-z]:", path) else path


def path_to_uri(path):
    return Path(path).absolute().as_uri()


class Graph:
    """Queries the fact graph WITHOUT holding the database open.

    A long-lived connection keeps a Windows file handle on uno.sqlite, and the
    editor outlives every refresh -- so refresh_index.py could not replace the
    file while Lapce was running.  Connecting per query costs microseconds on a
    database this size and has the better property besides: a refreshed graph
    is picked up on the next hover instead of needing the server restarted.
    """

    def __init__(self, db, workspace):
        self.db = db
        self.workspace = Path(workspace)

    def _q(self, sql, params=()):
        with sqlite3.connect(self.db) as conn:
            return conn.execute(sql, params).fetchall()

    def type_of(self, name):
        r = self._q("SELECT class FROM types WHERE name = ?", (name,))
        return r[0][0] if r else None

    def interfaces(self, service):
        return self._q(
            "SELECT sort, target FROM service_refs WHERE service = ? "
            "ORDER BY sort, target", (service,))

    def implementations(self, service):
        return self._q("""
            SELECT i.impl, i.component, c.artifact, c.kind
            FROM impl_services i JOIN components c ON c.path = i.component
            WHERE i.service = ?
        """, (service,))

    def producer(self, artifact):
        rows = self._q("SELECT label, kind, output, staged FROM provenance")
        built, staged = None, None
        for label, kind, output, is_staged in rows:
            if Path(output).name != artifact:
                continue
            if is_staged:
                staged = staged or output
            elif kind not in ("filegroup", "collect_outputs", "flat_install",
                              "tree_install", "res_stage", "alias"):
                built = built or (label, kind)
        return built, staged

    def idl_path(self, name):
        rel = name.replace(".", "/") + ".idl"
        for root in _IDL_ROOTS:
            cand = self.workspace / root / rel
            if cand.exists():
                return cand
        return None

    def services_of_impl(self, impl):
        """Rows for an IMPLEMENTATION name.

        Implementation names appear as literals in the tree just as often as
        service names do -- getImplementationName() returns one, and the
        factory tables are built from them -- and they are easy to mistake for
        services because they differ only by a `.comp.` segment.
        """
        return self._q("""
            SELECT i.service, i.component, c.artifact, c.kind
            FROM impl_services i JOIN components c ON c.path = i.component
            WHERE i.impl = ? ORDER BY i.service
        """, (impl,))

    def search(self, query):
        like = "%{}%".format(query)
        return self._q(
            "SELECT name, class FROM types WHERE name LIKE ? ORDER BY name LIMIT 200",
            (like,))


def describe(graph, name):
    """Markdown hover body for a UNO name, or None if it is not one we know."""
    cls = graph.type_of(name)
    impls = graph.implementations(name)
    if cls is None and not impls:
        return describe_impl(graph, name)

    out = ["**{}**".format(name)]
    if cls:
        out.append("`{}`".format(cls))

    refs = graph.interfaces(name)
    if refs:
        out.append("")
        for sort, target in refs:
            out.append("- _{}_ `{}`".format(sort, target))

    for impl, component, artifact, kind in impls:
        built, staged = graph.producer(artifact)
        out.append("")
        out.append("**implementation** `{}`".format(impl))
        out.append("- library `{}` ({})".format(artifact, kind))
        if built:
            out.append("- built by `{}` [{}]".format(built[0], built[1]))
        if staged:
            out.append("- staged `{}`".format(staged))
        else:
            out.append("- **NOT STAGED** -- will fail to load")
        out.append("- registered in `{}`".format(component))

    if cls and not impls and "service" in cls:
        out.append("")
        out.append("_No registered implementation._")
    return "\n".join(out)


def describe_impl(graph, name):
    """Hover body for an implementation name, or None if it is not one."""
    rows = graph.services_of_impl(name)
    if not rows:
        return None
    _svc, component, artifact, kind = rows[0]
    built, staged = graph.producer(artifact)
    out = ["**{}**".format(name), "`implementation`", ""]
    out.append("- library `{}` ({})".format(artifact, kind))
    if built:
        out.append("- built by `{}` [{}]".format(built[0], built[1]))
    out.append("- staged `{}`".format(staged) if staged
               else "- **NOT STAGED** -- will fail to load")
    out.append("- registered in `{}`".format(component))
    out.append("")
    out.append("**implements**")
    for svc, _c, _a, _k in rows:
        out.append("- `{}`".format(svc))
    return "\n".join(out)


def token_at(line, character):
    """The UNO name under the cursor, preferring a string literal.

    Checking string literals first is what makes createInstance("...") work:
    the interesting name is almost always inside quotes, and the identifier
    scan would otherwise stop at the quote.
    """
    for m in _STRING_LIT.finditer(line):
        if m.start(1) <= character <= m.end(1):
            inner = m.group(1)
            return inner if _UNO_NAME.fullmatch(inner) else None
    for m in _UNO_NAME.finditer(line):
        if m.start() <= character <= m.end():
            return m.group(0)
    return None


class Server:
    def __init__(self, graph):
        self.graph = graph
        self.docs = {}

    # ── document text ────────────────────────────────────────────────────────
    def text(self, uri):
        if uri in self.docs:
            return self.docs[uri]
        try:
            return Path(uri_to_path(uri)).read_text(encoding="utf-8", errors="replace")
        except OSError:
            return ""

    def line_at(self, uri, line_no):
        lines = self.text(uri).splitlines()
        return lines[line_no] if 0 <= line_no < len(lines) else ""

    # ── requests ─────────────────────────────────────────────────────────────
    def on_initialize(self, _params):
        return {"capabilities": {
            "textDocumentSync": 1,          # full
            "hoverProvider": True,
            "definitionProvider": True,
            "workspaceSymbolProvider": True,
        }, "serverInfo": {"name": "uno-lsp", "version": "0.1"}}

    def on_hover(self, params):
        pos = params["position"]
        line = self.line_at(params["textDocument"]["uri"], pos["line"])
        name = token_at(line, pos["character"])
        if not name:
            return None
        body = describe(self.graph, name)
        if not body:
            return None
        return {"contents": {"kind": "markdown", "value": body}}

    def on_definition(self, params):
        pos = params["position"]
        line = self.line_at(params["textDocument"]["uri"], pos["line"])
        name = token_at(line, pos["character"])
        if not name:
            return None

        # A service resolves to the .component that registers it, positioned on
        # the <implementation> element -- that is the code-side answer people
        # want.  Everything else resolves to its IDL.
        for impl, component, _artifact, _kind in self.graph.implementations(name):
            path = self.graph.workspace / component
            if not path.exists():
                continue
            line_no = 0
            for i, text in enumerate(path.read_text(encoding="utf-8",
                                                    errors="replace").splitlines()):
                if impl in text:
                    line_no = i
                    break
            return self._location(path, line_no)

        idl = self.graph.idl_path(name)
        return self._location(idl, 0) if idl else None

    @staticmethod
    def _location(path, line_no):
        return {"uri": path_to_uri(path),
                "range": {"start": {"line": line_no, "character": 0},
                          "end": {"line": line_no, "character": 0}}}

    def on_symbol(self, params):
        query = params.get("query", "")
        if len(query) < 3:
            return []
        out = []
        for name, cls in self.graph.search(query):
            idl = self.graph.idl_path(name)
            if not idl:
                continue
            out.append({
                "name": name,
                "kind": 11 if "interface" in cls else 5,   # Interface / Class
                "location": self._location(idl, 0),
            })
        return out

    def diagnostics(self, uri):
        """Service strings this file tries to INSTANTIATE but nothing implements.

        The instantiation context is not optional.  756 services in offapi have
        no registered implementation, and that is normal -- many are abstract
        service descriptions that exist only to be named.  Flagging every
        mention would light up every supportsService() and
        getSupportedServiceNames() in the tree, which is idiomatic and correct.
        Only an actual create call makes a missing implementation a bug.
        """
        items = []
        for i, line in enumerate(self.text(uri).splitlines()):
            if "createInstance" not in line:
                continue
            for m in _STRING_LIT.finditer(line):
                name = m.group(1)
                if not _UNO_NAME.fullmatch(name):
                    continue
                cls = self.graph.type_of(name)
                if cls is None or "service" not in cls:
                    continue
                if self.graph.implementations(name):
                    continue
                items.append({
                    "range": {"start": {"line": i, "character": m.start(1)},
                              "end": {"line": i, "character": m.end(1)}},
                    "severity": 2,
                    "source": "uno",
                    "message": "No registered implementation for service "
                               "'{}'".format(name),
                })
        return items


def read_message(stream):
    length = None
    while True:
        line = stream.readline()
        if not line:
            return None
        line = line.decode("utf-8").strip()
        if not line:
            break
        if line.lower().startswith("content-length:"):
            length = int(line.split(":", 1)[1].strip())
    if not length:
        return None
    return json.loads(stream.read(length).decode("utf-8"))


def write_message(stream, payload):
    data = json.dumps(payload).encode("utf-8")
    stream.write("Content-Length: {}\r\n\r\n".format(len(data)).encode("ascii"))
    stream.write(data)
    stream.flush()


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--db", required=True)
    ap.add_argument("--workspace", default=".")
    args = ap.parse_args(argv)

    server = Server(Graph(args.db, args.workspace))
    stdin, stdout = sys.stdin.buffer, sys.stdout.buffer

    handlers = {
        "initialize": server.on_initialize,
        "textDocument/hover": server.on_hover,
        "textDocument/definition": server.on_definition,
        "workspace/symbol": server.on_symbol,
    }

    while True:
        msg = read_message(stdin)
        if msg is None:
            break
        method, mid = msg.get("method"), msg.get("id")

        if method == "shutdown":
            write_message(stdout, {"jsonrpc": "2.0", "id": mid, "result": None})
            continue
        if method == "exit":
            break

        if method in ("textDocument/didOpen", "textDocument/didChange"):
            doc = msg["params"]["textDocument"]
            uri = doc["uri"]
            if method == "textDocument/didOpen":
                server.docs[uri] = doc["text"]
            else:
                changes = msg["params"].get("contentChanges") or []
                if changes:
                    server.docs[uri] = changes[-1]["text"]
            write_message(stdout, {
                "jsonrpc": "2.0",
                "method": "textDocument/publishDiagnostics",
                "params": {"uri": uri, "diagnostics": server.diagnostics(uri)},
            })
            continue
        if method == "textDocument/didClose":
            server.docs.pop(msg["params"]["textDocument"]["uri"], None)
            continue

        if mid is None:
            continue        # a notification we do not handle
        handler = handlers.get(method)
        result = handler(msg.get("params") or {}) if handler else None
        write_message(stdout, {"jsonrpc": "2.0", "id": mid, "result": result})

    return 0


if __name__ == "__main__":
    sys.exit(main())
