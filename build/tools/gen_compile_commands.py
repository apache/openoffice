#!/usr/bin/env python3
"""gen_compile_commands.py -- emit compile_commands.json from Bazel, for clangd.

Without this, clangd cannot index AOO at all: every translation unit depends on
defines injected globally by the toolchain (MSC, _HAS_ITERATOR_DEBUGGING=0,
CPPU_ENV, the arch set) plus a long generated-header include path.  Guessing
them indexes the wrong #if branch -- the same failure mode as the dirent.cxx
bug, where three modules silently compiled the non-Windows path.

How the flags are recovered:

    bazel aquery 'mnemonic("CppCompile", deps(<target>))' --output=jsonproto

Compile actions normally invoke `cl.exe @some.params`, and that params file is
written only when the action actually runs -- so aquery alone would yield no
flags, and --nobuild never materializes them.  Passing
`--features=-compiler_param_file` puts every argument back on the command line,
so the full flag set is recoverable from analysis alone, with nothing compiled.

The emitted entries are rewritten to invoke clang-cl rather than cl.exe, which
is what puts clangd in MSVC-compatible driver mode so it understands /D, /I,
/Zc: and friends.

    gen_compile_commands.py --target //main/staging:install
    gen_compile_commands.py --target //main/sw:sw --config win10-x64

NOTE ON TOOLCHAIN CHOICE: prefer --config win10-x64.  Indexing resolves the
system headers of whichever toolchain the config selects, and a modern clang
parses the VS2019/UCRT headers far more cleanly than the VS2008 ones.  The
compile flags that matter for correctness (the global defines, the include
paths) are the same either way, so indexing against the modern toolchain costs
nothing and avoids a pile of spurious errors inside MSVC's own headers.
"""

import argparse
import json
import shutil
import subprocess
import sys
from pathlib import Path

# Arguments that describe the build's outputs rather than how to parse the
# source.  clangd does not need them and /Fo actively confuses some tooling.
_DROP_PREFIXES = ("/Fo", "-Fo", "/showIncludes", "/Fd", "-Fd")


def run_bazel(args, config, capture=True):
    cmd = ["bazel"] + args
    if config:
        cmd.append("--config=" + config)
    proc = subprocess.run(cmd, capture_output=capture, text=True)
    if proc.returncode != 0:
        sys.exit("bazel failed: {}\n{}".format(" ".join(cmd), proc.stderr[-4000:]))
    return proc.stdout


def execution_root(config):
    return run_bazel(["info", "execution_root"], config).strip()


def workspace_root(config):
    return run_bazel(["info", "workspace"], config).strip()


def absolute_source(src, root, workspace):
    """Resolve a source path to somewhere clangd will actually match.

    Bazel reports sources relative to the execroot, where the first-party tree
    is reached through junctions back into the real workspace.  An editor opens
    C:/workspace/openoffice/main/..., so an entry keyed on the execroot path can
    fail to match the open buffer.  Generated and external sources have no
    workspace copy, so those stay under the execroot.
    """
    src = src.replace("\\", "/")
    if src.startswith(("bazel-out/", "external/", "/", "C:", "c:")):
        return src if src[1:2] == ":" else "{}/{}".format(root, src)
    return "{}/{}".format(workspace, src)


def aquery(target, config):
    out = run_bazel([
        "aquery",
        'mnemonic("CppCompile", deps({}))'.format(target),
        "--output=jsonproto",
        "--include_artifacts=false",
        # See module docstring: this is what makes the flags visible at all.
        # Both are needed -- --features reaches only the target configuration,
        # so without the host one every exec-config action (the codegen tools
        # and their deps, ~10% here) still uses a params file and is silently
        # dropped for having no visible source argument.
        "--features=-compiler_param_file",
        "--host_features=-compiler_param_file",
    ], config)
    return json.loads(out)


def system_includes(action):
    """INCLUDE from the action env, as clang-cl /imsvc flags.

    The toolchain passes system headers through the INCLUDE environment
    variable, not as /I flags, so an entry that only copied the command line
    would fail to find <stdio.h>.  clangd runs the compile without the build's
    environment, so these have to be baked into the entry.  /imsvc rather than
    /I marks them as system headers, which suppresses warnings inside them.
    """
    for kv in action.get("environmentVariables", []):
        if kv.get("key") == "INCLUDE":
            dirs = [d.strip() for d in kv.get("value", "").split(";") if d.strip()]
            flags = []
            for d in dirs:
                flags += ["/imsvc", d.replace("\\", "/")]
            return flags
    return []


def split_action(arguments):
    """(source_file, flags) for one CppCompile action, or (None, None)."""
    src = None
    flags = []
    skip_next = False
    for i, a in enumerate(arguments[1:], start=1):
        if skip_next:
            skip_next = False
            continue
        if a == "/c" or a == "-c":
            # The compiled TU is the argument after /c.
            if i + 1 < len(arguments):
                src = arguments[i + 1]
                skip_next = True
            continue
        if a.startswith(_DROP_PREFIXES):
            continue
        flags.append(a)
    return src, flags


def main(argv=None):
    ap = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--target", default="//main/staging:install",
                    help="Bazel target whose deps get indexed")
    ap.add_argument("--config", default=None,
                    help="bazelrc config, e.g. win10-x64 (recommended)")
    ap.add_argument("--driver", default=None,
                    help="path to clang-cl.exe (default: found on PATH)")
    ap.add_argument("--out", default="compile_commands.json")
    args = ap.parse_args(argv)

    driver = args.driver or shutil.which("clang-cl")
    if not driver:
        sys.exit("clang-cl not found; pass --driver "
                 r'"C:\Program Files\LLVM\bin\clang-cl.exe"')

    root = execution_root(args.config)
    workspace = workspace_root(args.config)
    print("execution_root: {}".format(root))
    print("workspace:      {}".format(workspace))
    print("querying {} ...".format(args.target))
    data = aquery(args.target, args.config)

    # Sources shared between the product and a codegen tool are compiled twice,
    # in two different configurations -- and here those use two different MSVC
    # toolchains.  Indexing a file with the exec toolchain's flags reintroduces
    # exactly the VS2008 header problems the --config choice exists to avoid, so
    # the target-configuration entry has to win the de-duplication below.
    tool_configs = {
        c.get("id") for c in data.get("configuration", []) if c.get("isTool")
    }

    entries = []
    skipped = 0
    for action in data.get("actions", []):
        arguments = action.get("arguments") or []
        if not arguments:
            skipped += 1
            continue
        src, flags = split_action(arguments)
        if not src:
            skipped += 1
            continue
        abs_src = absolute_source(src, root, workspace)
        entries.append({
            "_is_tool": action.get("configurationId") in tool_configs,
            # directory stays the execroot: the /I flags Bazel emits are
            # relative to it, and bazel-out/ and external/ only exist there.
            "directory": root,
            "file": abs_src,
            "arguments": [driver] + flags + system_includes(action) + ["/c", abs_src],
        })

    # clangd uses the first entry it finds for a file, so collapse to one --
    # target configuration first, tool configuration only as a fallback for
    # sources that are built nowhere else.
    best = {}
    for e in entries:
        cur = best.get(e["file"])
        if cur is None or (cur["_is_tool"] and not e["_is_tool"]):
            best[e["file"]] = e
    unique = []
    for e in best.values():
        e.pop("_is_tool")
        unique.append(e)

    Path(args.out).write_text(json.dumps(unique, indent=1), encoding="utf-8")
    print("{} actions -> {} entries ({} unique files, {} skipped)"
          .format(len(data.get("actions", [])), len(entries), len(unique), skipped))
    print("wrote {}".format(args.out))
    return 0


if __name__ == "__main__":
    sys.exit(main())
