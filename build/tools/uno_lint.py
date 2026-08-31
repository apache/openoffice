#!/usr/bin/env python3
"""uno_lint.py -- validate the UNO registration/staging graph.

Holds one invariant that nothing else in the build enforces:

    every component registered in services.rdb must have its artifact staged

Staging is gated (ATL, optional SDKs, deferred modules); registration is not.
When the two disagree the component is listed in services.rdb but its library is
absent, and the failure surfaces only when something instantiates the service --
as "loading component library failed", far from the cause.

Exits non-zero on findings so this can become a bazel test target.

Usage:
    uno_lint.py --db uno.sqlite [--allow build/tools/unolint_allow.cfg]
"""

import argparse
import sqlite3
import sys
from pathlib import Path


def load_allowlist(path):
    """artifact -> reason.  Blank lines and #-comments ignored."""
    allowed = {}
    if not path:
        return allowed
    for line in Path(path).read_text(encoding="utf-8").splitlines():
        line = line.strip()
        if not line or line.startswith("#"):
            continue
        artifact, _, reason = line.partition("#")
        allowed[artifact.strip()] = reason.strip() or "(no reason given)"
    return allowed


def staged_basenames(conn):
    rows = conn.execute("SELECT output FROM provenance WHERE staged = 1").fetchall()
    return {Path(r[0]).name for r in rows}


def expected_names(kind, artifact):
    """Names that would satisfy a registration, by URI kind."""
    if kind == "pymodule":
        return {artifact + ".py", artifact}
    return {artifact}


def check_unstaged(conn, allowed):
    staged = staged_basenames(conn)
    findings = []
    rows = conn.execute(
        "SELECT path, label, kind, artifact FROM components ORDER BY artifact"
    ).fetchall()
    for path, label, kind, artifact in rows:
        if staged & expected_names(kind, artifact):
            continue
        if artifact in allowed:
            continue
        findings.append((artifact, kind, path, label))
    return findings, len(rows), len(staged)


def check_unknown_services(conn):
    """Services registered by a component but absent from the type graph.

    Informational: a service can legitimately come from an .rdb this graph was
    not built over (oovbaapi, an extension's own types), so this is reported
    separately and never fails the run.
    """
    return conn.execute("""
        SELECT DISTINCT s.service, s.component
        FROM impl_services s
        LEFT JOIN types t ON t.name = s.service
        WHERE t.name IS NULL
        ORDER BY s.service
    """).fetchall()


def main(argv=None):
    ap = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--db", required=True, help="SQLite store from uno_graph.py")
    ap.add_argument("--allow", help="allowlist of known-unstaged artifacts")
    ap.add_argument("--show-unknown-services", action="store_true",
                    help="also list services absent from the type graph")
    args = ap.parse_args(argv)

    conn = sqlite3.connect(args.db)
    allowed = load_allowlist(args.allow)

    findings, n_components, n_staged = check_unstaged(conn, allowed)

    print("checked {} registered components against {} staged files"
          .format(n_components, n_staged))
    if allowed:
        print("{} artifact(s) allowlisted".format(len(allowed)))

    if args.show_unknown_services:
        unknown = check_unknown_services(conn)
        if unknown:
            print("\nservices not in the type graph ({}, informational):"
                  .format(len(unknown)))
            for svc, comp in unknown[:20]:
                print("  {}  <- {}".format(svc, comp))
            if len(unknown) > 20:
                print("  ... and {} more".format(len(unknown) - 20))

    if not findings:
        print("\nOK: every registered component is staged")
        return 0

    print("\nREGISTERED BUT NOT STAGED ({}):".format(len(findings)))
    for artifact, kind, path, label in findings:
        print("  {:<28} {:<9} {}".format(artifact, kind, path))
    print("\nEach will fail at instantiation with "
          '"loading component library failed".')
    print("Fix by staging the artifact, gating its registration to match, or "
          "allowlisting it with a reason.")
    return 1


if __name__ == "__main__":
    sys.exit(main())
