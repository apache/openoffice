#!/usr/bin/env python3
"""refresh_index.py -- rebuild everything the editor reads, from Bazel.

Bazel already knows every fact the tooling needs; this is the one command that
pushes them out to where clangd and uno_lsp can see them:

    compile_commands.json   flags for clangd            (gen_compile_commands.py)
    uno.sqlite              the UNO fact graph          (uno_graph.py)

Run it after changing BUILD files or adding sources.  Both outputs are
snapshots, and a stale uno.sqlite is worse than none -- it reports staging that
is no longer true.

    refresh_index.py              # both
    refresh_index.py --graph      # just uno.sqlite
    refresh_index.py --index      # just compile_commands.json

THE TWO HALVES USE DIFFERENT CONFIGURATIONS ON PURPOSE:

  the graph is built in the DEFAULT configuration, because staging facts are
  per-configuration and winXP-x86 is the product baseline -- the graph should
  describe what actually ships;

  the compile database is built with --config=win10-x64, because indexing
  resolves the system headers of whichever toolchain the config selects, and a
  modern clang parses the VS2019/UCRT headers far more cleanly than the VS2008
  ones.  The flags that matter for correctness are identical either way.
"""

import argparse
import os
import shutil
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parent.parent

ASPECT = "//build/rules:unograph.bzl%uno_provenance_aspect"

# regview links reg/sal/salhelper/store, and Bazel leaves each beside its own
# package rather than next to the executable, so the loader needs them on PATH.
_DLL_PACKAGES = ("registry", "sal", "salhelper", "store")


def run(cmd, env=None, quiet=False):
    if not quiet:
        print("+ " + " ".join(str(c) for c in cmd))
    proc = subprocess.run([str(c) for c in cmd], cwd=ROOT, env=env)
    if proc.returncode != 0:
        sys.exit("failed ({}): {}".format(proc.returncode, " ".join(str(c) for c in cmd)))


def bazel(args, config=None):
    cmd = ["bazel"] + args
    if config:
        cmd.append("--config=" + config)
    return cmd


def bazel_info(key, config=None):
    cmd = bazel(["info", key], config)
    out = subprocess.run([str(c) for c in cmd], cwd=ROOT,
                         capture_output=True, text=True)
    if out.returncode != 0:
        sys.exit("bazel info {} failed:\n{}".format(key, out.stderr[-2000:]))
    return out.stdout.strip()


def refresh_graph(config):
    print("\n=== UNO fact graph ===")
    run(bazel(["build",
               "//main/registry:regview",
               "//main/offapi:offapi_idl",
               "//main/udkapi:udkapi_idl"], config))

    # The aspect is a pure observer behind an output group: this compiles
    # nothing, it only writes the JSON fact fragments.
    run(bazel(["build", "//main/staging:install",
               "--aspects=" + ASPECT,
               "--output_groups=uno_facts"], config))

    bin_dir = Path(bazel_info("bazel-bin", config))
    env = dict(os.environ)
    env["PATH"] = os.pathsep.join(
        [str(bin_dir / "main" / p) for p in _DLL_PACKAGES] + [env.get("PATH", "")])

    run([sys.executable, HERE / "uno_graph.py",
         "--workspace", ROOT,
         "--facts", bin_dir,
         "--regview", bin_dir / "main" / "registry" / "regview.exe",
         "--rdb", bin_dir / "main" / "offapi" / "offapi_idl.rdb",
         "--rdb", bin_dir / "main" / "udkapi" / "udkapi_idl.rdb",
         "--out", ROOT / "uno.sqlite"], env=env)


def refresh_index(config, driver):
    print("\n=== compile_commands.json ===")
    driver = driver or shutil.which("clang-cl")
    if not driver:
        sys.exit("clang-cl not found; pass --driver")
    run([sys.executable, HERE / "gen_compile_commands.py",
         "--target", "//main/staging:install",
         "--config", config,
         "--driver", driver,
         "--out", ROOT / "compile_commands.json"])


def main(argv=None):
    ap = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--graph", action="store_true", help="only the UNO graph")
    ap.add_argument("--index", action="store_true", help="only compile_commands.json")
    ap.add_argument("--graph-config", default=None,
                    help="bazelrc config for the graph (default: the default platform)")
    ap.add_argument("--index-config", default="win10-x64",
                    help="bazelrc config for the compile database")
    ap.add_argument("--driver", default=None, help="path to clang-cl.exe")
    args = ap.parse_args(argv)

    both = not (args.graph or args.index)
    if args.graph or both:
        refresh_graph(args.graph_config)
    if args.index or both:
        refresh_index(args.index_config, args.driver)

    print("\nDone.  Restart the language servers to pick up the new snapshots.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
