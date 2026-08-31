#!/usr/bin/env python3
"""uno_graph.py -- join the UNO type graph with Bazel registration/provenance facts.

Reads three inputs and writes one SQLite store:

  1. a regview text dump of a built .rdb   -> services, interfaces, methods
  2. JSON fact fragments from the aspect   -> component -> uri -> target -> staged
  3. the .component XML files those name   -> implementation -> service

See build/tools/unograph_schema.md for the contract.  This script reads only
those three things; it must never import Bazel internals or assume a bazel-out
layout.

Usage:
    uno_graph.py --workspace . --facts bazel-bin --out uno.sqlite
                 --rdb-dump offapi.txt [--rdb-dump udkapi.txt]

    # or let it run regview itself
    uno_graph.py --workspace . --facts bazel-bin --out uno.sqlite
                 --regview bazel-bin/main/registry/regview.exe
                 --rdb bazel-bin/main/offapi/offapi_idl.rdb
"""

import argparse
import json
import re
import sqlite3
import subprocess
import sys
import xml.etree.ElementTree as ET
from pathlib import Path

SCHEMA_VERSION = 1

_NS = {"c": "http://openoffice.org/2010/uno-components"}

# regview line forms.  Parsed on stripped lines: the full-tree dump nests keys
# by indentation, so the Data block's column varies with depth.
_RE_CLASS = re.compile(r'^type class: (.+)$')
_RE_TYPENAME = re.compile(r'^type name: "([^"]*)"$')
_RE_SUPER = re.compile(r'^super type name \d+: "([^"]*)"$')
_RE_SORT = re.compile(r'^sort: (\w+)$')


def parse_regview_dump(path):
    """Yield type records from a regview text dump.

    Sound only because regview never emits a multi-line value -- long
    documentation is escaped onto a single line.  If that ever changes this
    parser must fail loudly rather than silently truncate, which is why the
    caller asserts every recovered type has a name.
    """
    types, cur, pending_sort = [], None, None
    with open(path, encoding="utf-8-sig", errors="replace") as fh:
        for raw in fh:
            s = raw.strip()

            m = _RE_CLASS.match(s)
            if m:
                cur = {"class": m.group(1), "name": None, "supers": [], "refs": []}
                types.append(cur)
                pending_sort = None
                continue
            if cur is None:
                continue

            m = _RE_TYPENAME.match(s)
            if m:
                if cur["name"] is None:
                    cur["name"] = m.group(1)          # first one is the type itself
                elif pending_sort is not None:        # inside a reference block
                    cur["refs"].append((pending_sort, m.group(1)))
                    pending_sort = None
                continue

            m = _RE_SUPER.match(s)
            if m:
                cur["supers"].append(m.group(1))
                continue

            m = _RE_SORT.match(s)
            if m:
                pending_sort = m.group(1)
    return types


def run_regview(regview, rdb, out_path):
    with open(out_path, "wb") as fh:
        subprocess.run([str(regview), str(rdb)], stdout=fh, check=True)
    return out_path


def load_facts(facts_root):
    """Collect *.unofacts.json fragments written by uno_provenance_aspect."""
    provenance, registrations = [], []
    for p in Path(facts_root).rglob("*.unofacts.json"):
        try:
            obj = json.loads(p.read_text(encoding="utf-8"))
        except (json.JSONDecodeError, OSError) as e:
            print("warning: skipping {}: {}".format(p, e), file=sys.stderr)
            continue
        if obj.get("fact") == "provenance":
            provenance.append(obj)
        elif obj.get("fact") == "registration":
            registrations.extend(obj.get("components", []))
    return provenance, registrations


def classify_uri(uri):
    """(kind, artifact) for a component URI.  See unograph_schema.md."""
    if uri.startswith("vnd.openoffice.pymodule:"):
        return "pymodule", uri.split(":", 1)[1]
    if "/program/classes/" in uri:
        return "java", uri.rsplit("/", 1)[1]
    if "$URE_INTERNAL_JAVA_DIR/" in uri:
        return "ure_java", uri.rsplit("/", 1)[1]
    if "/program/" in uri:
        return "native", uri.rsplit("/", 1)[1]
    return "unknown", uri.rsplit("/", 1)[-1]


def parse_component(path):
    """[(impl_name, [service_name, ...]), ...] from a .component XML."""
    try:
        root = ET.parse(path).getroot()
    except (ET.ParseError, OSError) as e:
        print("warning: cannot parse {}: {}".format(path, e), file=sys.stderr)
        return []
    out = []
    for impl in root.findall(".//c:implementation", _NS):
        name = impl.get("name")
        if not name:
            continue
        svcs = [s.get("name") for s in impl.findall("c:service", _NS) if s.get("name")]
        out.append((name, svcs))
    return out


_DDL = """
CREATE TABLE meta (key TEXT PRIMARY KEY, value TEXT);
CREATE TABLE types (name TEXT PRIMARY KEY, class TEXT);
CREATE TABLE supertypes (type TEXT, super TEXT);
CREATE TABLE service_refs (service TEXT, sort TEXT, target TEXT);
CREATE TABLE components (path TEXT PRIMARY KEY, label TEXT, uri TEXT,
                         kind TEXT, artifact TEXT);
CREATE TABLE impl_services (component TEXT, impl TEXT, service TEXT);
CREATE TABLE provenance (label TEXT, kind TEXT, staged INT, output TEXT);
CREATE INDEX ix_refs ON service_refs(service);
CREATE INDEX ix_impl ON impl_services(service);
CREATE INDEX ix_prov ON provenance(output);
"""


def build(conn, types, provenance, registrations, workspace):
    c = conn.cursor()
    c.executescript(_DDL)
    c.execute("INSERT INTO meta VALUES ('schema_version', ?)", (str(SCHEMA_VERSION),))

    for t in types:
        if not t["name"]:
            continue
        dotted = t["name"].replace("/", ".")
        c.execute("INSERT OR REPLACE INTO types VALUES (?,?)", (dotted, t["class"]))
        for s in t["supers"]:
            c.execute("INSERT INTO supertypes VALUES (?,?)",
                      (dotted, s.replace("/", ".")))
        for sort, tgt in t["refs"]:
            c.execute("INSERT INTO service_refs VALUES (?,?,?)",
                      (dotted, sort, tgt.replace("/", ".")))

    for r in registrations:
        kind, artifact = classify_uri(r["uri"])
        c.execute("INSERT OR REPLACE INTO components VALUES (?,?,?,?,?)",
                  (r["component"], r.get("label", ""), r["uri"], kind, artifact))
        comp_path = Path(workspace) / r["component"]
        if comp_path.exists():
            for impl, svcs in parse_component(comp_path):
                for svc in svcs:
                    c.execute("INSERT INTO impl_services VALUES (?,?,?)",
                              (r["component"], impl, svc))
        else:
            print("warning: component file not found: {}".format(comp_path),
                  file=sys.stderr)

    for p in provenance:
        for out in p["outputs"]:
            c.execute("INSERT INTO provenance VALUES (?,?,?,?)",
                      (p["label"], p.get("kind", ""), 1 if p.get("staged") else 0, out))

    conn.commit()


def main(argv=None):
    ap = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--workspace", default=".",
                    help="repo root, to resolve .component paths")
    ap.add_argument("--facts", required=True,
                    help="dir to scan for *.unofacts.json")
    ap.add_argument("--rdb-dump", action="append", default=[],
                    help="regview text dump")
    ap.add_argument("--rdb", action="append", default=[],
                    help=".rdb to dump via --regview")
    ap.add_argument("--regview", help="path to regview.exe")
    ap.add_argument("--out", required=True, help="output SQLite file")
    args = ap.parse_args(argv)

    dumps = list(args.rdb_dump)
    if args.rdb:
        if not args.regview:
            ap.error("--rdb requires --regview")
        for i, rdb in enumerate(args.rdb):
            tmp = Path(args.out).with_suffix(".dump{}.txt".format(i))
            dumps.append(run_regview(args.regview, rdb, tmp))
    if not dumps:
        ap.error("need at least one --rdb-dump or --rdb")

    types = []
    for d in dumps:
        got = parse_regview_dump(d)
        print("  {}: {} types".format(d, len(got)))
        types.extend(got)

    unnamed = sum(1 for t in types if not t["name"])
    if unnamed:
        sys.exit("error: {} types parsed without a name -- regview format changed?"
                 .format(unnamed))

    provenance, registrations = load_facts(args.facts)
    print("  facts: {} provenance, {} registrations"
          .format(len(provenance), len(registrations)))

    out = Path(args.out)
    if out.exists():
        out.unlink()
    conn = sqlite3.connect(out)
    build(conn, types, provenance, registrations, args.workspace)
    conn.close()
    print("wrote {}".format(out))
    return 0


if __name__ == "__main__":
    sys.exit(main())
