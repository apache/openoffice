#!/usr/bin/env python3
"""uno_query.py -- ask the UNO fact graph the questions an IDE cannot answer.

The chain a C++ index cannot follow, because it crosses a string literal, an XML
registry, and the build graph:

    createInstance("com.sun.star.frame.Desktop")
      -> service       supports XDesktop, XComponentLoader, ...
      -> implementation com.sun.star.comp.framework.Desktop
      -> library        fwk.dll
      -> Bazel target   //main/framework:fwk
      -> staged path    program/fwk.dll

Usage:
    uno_query.py --db uno.sqlite service com.sun.star.frame.Desktop
    uno_query.py --db uno.sqlite impl    com.sun.star.comp.framework.Desktop
    uno_query.py --db uno.sqlite artifact fwk.dll
    uno_query.py --db uno.sqlite type    com.sun.star.frame.XComponentLoader
"""

import argparse
import sqlite3
import sys
from pathlib import Path


def _staged_for(conn, artifact):
    rows = conn.execute(
        "SELECT label, output FROM provenance WHERE staged = 1").fetchall()
    return [(lbl, out) for lbl, out in rows if Path(out).name == artifact]


# Rules that only re-export files someone else produced.  They match on
# basename just as well as the real producer, so they are ranked below it.
_AGGREGATOR_KINDS = {"filegroup", "collect_outputs", "flat_install",
                     "tree_install", "res_stage", "alias"}


def _producer_for(conn, artifact):
    rows = conn.execute(
        "SELECT label, kind, output FROM provenance WHERE staged = 0").fetchall()
    hits = [(lbl, kind, out) for lbl, kind, out in rows if Path(out).name == artifact]
    real = [h for h in hits if h[1] not in _AGGREGATOR_KINDS]
    return real or hits


def cmd_service(conn, name):
    row = conn.execute("SELECT class FROM types WHERE name = ?", (name,)).fetchone()
    print(name)
    print("  type class : {}".format(row[0] if row else "(not in type graph)"))

    refs = conn.execute(
        "SELECT sort, target FROM service_refs WHERE service = ? ORDER BY sort, target",
        (name,)).fetchall()
    if refs:
        print("  interfaces :")
        for sort, target in refs:
            print("      {:<9} {}".format(sort, target))

    impls = conn.execute("""
        SELECT i.impl, i.component, c.uri, c.kind, c.artifact, c.label
        FROM impl_services i JOIN components c ON c.path = i.component
        WHERE i.service = ?
    """, (name,)).fetchall()

    if not impls:
        print("  implementation: NONE REGISTERED")
        return 1

    for impl, comp, uri, kind, artifact, label in impls:
        print("  implementation: {}".format(impl))
        print("      component  : {}".format(comp))
        print("      uri        : {}".format(uri))
        print("      artifact   : {} ({})".format(artifact, kind))
        for lbl, k, out in _producer_for(conn, artifact):
            print("      built by   : {}  [{}]".format(lbl, k))
        staged = _staged_for(conn, artifact)
        if staged:
            for lbl, out in staged:
                print("      staged     : {}".format(out))
        else:
            print("      staged     : NOT STAGED  <-- will fail to load")
    return 0


def cmd_impl(conn, name):
    rows = conn.execute("""
        SELECT i.service, i.component, c.artifact, c.kind
        FROM impl_services i JOIN components c ON c.path = i.component
        WHERE i.impl = ? ORDER BY i.service
    """, (name,)).fetchall()
    if not rows:
        print("no such implementation: {}".format(name))
        return 1
    print(name)
    print("  component : {}".format(rows[0][1]))
    print("  artifact  : {} ({})".format(rows[0][2], rows[0][3]))
    print("  services  :")
    for svc, _, _, _ in rows:
        print("      {}".format(svc))
    return 0


def cmd_artifact(conn, name):
    rows = conn.execute(
        "SELECT path, label, uri, kind FROM components WHERE artifact = ?",
        (name,)).fetchall()
    if not rows:
        print("no component registers {}".format(name))
        return 1
    print(name)
    for path, label, uri, kind in rows:
        print("  component : {}".format(path))
        print("  kind      : {}".format(kind))
        impls = conn.execute(
            "SELECT DISTINCT impl FROM impl_services WHERE component = ? ORDER BY impl",
            (path,)).fetchall()
        print("  implementations ({}):".format(len(impls)))
        for (impl,) in impls:
            print("      {}".format(impl))
    staged = _staged_for(conn, name)
    print("  staged    : {}".format(staged[0][1] if staged else "NOT STAGED"))
    return 0


def cmd_type(conn, name):
    row = conn.execute("SELECT class FROM types WHERE name = ?", (name,)).fetchone()
    if not row:
        print("no such type: {}".format(name))
        return 1
    print("{}\n  class   : {}".format(name, row[0]))
    supers = conn.execute(
        "SELECT super FROM supertypes WHERE type = ?", (name,)).fetchall()
    for (s,) in supers:
        print("  extends : {}".format(s))
    users = conn.execute(
        "SELECT service, sort FROM service_refs WHERE target = ? ORDER BY service",
        (name,)).fetchall()
    if users:
        print("  used by ({}):".format(len(users)))
        for svc, sort in users:
            print("      {:<9} {}".format(sort, svc))
    return 0


def main(argv=None):
    ap = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--db", required=True, help="SQLite store from uno_graph.py")
    ap.add_argument("command", choices=["service", "impl", "artifact", "type"])
    ap.add_argument("name")
    args = ap.parse_args(argv)

    conn = sqlite3.connect(args.db)
    return {
        "service": cmd_service,
        "impl": cmd_impl,
        "artifact": cmd_artifact,
        "type": cmd_type,
    }[args.command](conn, args.name)


if __name__ == "__main__":
    sys.exit(main())
