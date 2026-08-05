"""Aspect and rules for collecting and staging build outputs."""

_TRAVERSAL_ATTRS = ["deps", "srcs", "data", "exports", "additional_linker_inputs"]

def _collect_files_aspect_impl(target, ctx):
    transitive = []

    for attr_name in _TRAVERSAL_ATTRS:
        if hasattr(ctx.rule.attr, attr_name):
            for dep in getattr(ctx.rule.attr, attr_name):
                if OutputGroupInfo in dep:
                    transitive.append(dep[OutputGroupInfo].collected_files)

    # Also collect PDB files from cc targets when the generate_pdb_file feature
    # is active (--compilation_mode=dbg or --features=generate_pdb_file).
    # The pdb_file output group is empty in fastbuild/opt, so this is a no-op
    # unless the caller also sets --//build:debug_info=True and builds with dbg.
    own_files = [target[DefaultInfo].files]
    if OutputGroupInfo in target and hasattr(target[OutputGroupInfo], "pdb_file"):
        own_files.append(target[OutputGroupInfo].pdb_file)

    files = depset(transitive = own_files + transitive)
    return [OutputGroupInfo(collected_files = files)]

collect_files_aspect = aspect(
    implementation = _collect_files_aspect_impl,
    attr_aspects = _TRAVERSAL_ATTRS,
)

def _collect_rule_impl(ctx):
    files = depset(transitive = [
        dep[OutputGroupInfo].collected_files
        for dep in ctx.attr.deps
    ])
    return [DefaultInfo(files = files)]

collect_outputs = rule(
    implementation = _collect_rule_impl,
    attrs = {
        "deps": attr.label_list(aspects = [collect_files_aspect]),
    },
)

# ── res_stage ─────────────────────────────────────────────────────────────────
# Stages OOo binary .res resource files into program/resource/ with renamed
# output names.  resmgr.cxx scans $OOO_BASE_DIR/program/resource/ at startup;
# filenames must match the <prefix><lang>-<country>.res pattern the runtime
# expects (e.g. vclen-US.res for prefix "vcl").
#
# Usage:
#   res_stage(
#       name = "_install_resources",
#       res_files = {
#           "//main/vcl:vcl_res": "vclen-US",
#           "//main/sfx2:sfx2_res": "sfxen-US",
#       },
#   )

def _res_stage_impl(ctx):
    outputs = []
    pairs   = []

    for src_tgt, out_name in ctx.attr.res_files.items():
        src_list = src_tgt.files.to_list()
        if len(src_list) != 1:
            fail("res_files key must resolve to exactly one file, got {}".format(len(src_list)))
        src_file = src_list[0]
        out = ctx.actions.declare_file("program/resource/" + out_name + ".res")
        pairs.append((src_file, out))
        outputs.append(out)

    if not outputs:
        return [DefaultInfo(files = depset())]

    # One mkdir + copy per file, in a single .bat for efficiency.
    out_dir = outputs[0].dirname.replace("/", "\\")
    lines = [
        "@echo off",
        'if not exist "{d}" mkdir "{d}"'.format(d = out_dir),
    ]
    for src_file, out in pairs:
        lines.append('copy /Y "{}" "{}" >nul || exit /b 1'.format(
            src_file.path.replace("/", "\\"),
            out.path.replace("/", "\\"),
        ))

    bat = ctx.actions.declare_file("_stage_resources.bat")
    ctx.actions.write(output = bat, content = "\r\n".join(lines) + "\r\n", is_executable = True)
    ctx.actions.run(
        inputs           = depset([f for f, _ in pairs] + [bat]),
        outputs          = outputs,
        executable       = bat,
        use_default_shell_env = True,
        mnemonic         = "StageResFiles",
        progress_message = "Staging {} .res files to program/resource/".format(len(outputs)),
    )

    return [DefaultInfo(files = depset(outputs))]

res_stage = rule(
    implementation = _res_stage_impl,
    attrs = {
        "res_files": attr.label_keyed_string_dict(
            allow_files = True,
            doc = "Mapping {rsc_res_target: output_name_without_extension}. Output is placed at program/resource/<output_name>.res",
        ),
    },
    doc = "Stage OOo .res resource files into program/resource/ with locale-named filenames.",
)

# Extensions included in the flat install directory.
# "pyd": Python extension modules.  Python 2.7's importer accepts ONLY this
# suffix on Windows (Python/dynload_win.c), so pyuno ships as pyuno.pyd; without
# it here the file is collected but silently dropped on the way to program/.
_INSTALL_EXTS = {"exe": True, "dll": True, "pyd": True, "rdb": True, "zip": True, "py": True, "ini": True, "manifest": True}

def _flat_install_impl(ctx):
    all_files = depset(transitive = [
        dep[DefaultInfo].files for dep in ctx.attr.srcs
    ])

    exts = dict(_INSTALL_EXTS)
    if ctx.attr.install_pdb:
        exts["pdb"] = True

    seen = {}
    filtered = []
    for f in all_files.to_list():
        if f.extension.lower() not in exts:
            continue
        if f.basename in seen:
            continue
        seen[f.basename] = True
        filtered.append(f)

    if not filtered:
        return [DefaultInfo(files = depset())]

    outputs = [ctx.actions.declare_file("program/" + f.basename) for f in filtered]

    program_dir = outputs[0].dirname.replace("/", "\\")
    lines = ["@echo off", 'if not exist "{d}" mkdir "{d}"'.format(d = program_dir)]
    for f, out in zip(filtered, outputs):
        lines.append('copy /Y "{}" "{}" >nul || exit /b 1'.format(
            f.path.replace("/", "\\"),
            out.path.replace("/", "\\"),
        ))

    bat = ctx.actions.declare_file("_stage.bat")
    ctx.actions.write(output = bat, content = "\r\n".join(lines) + "\r\n", is_executable = True)

    ctx.actions.run(
        inputs = depset(filtered + [bat]),
        outputs = outputs,
        executable = bat,
        use_default_shell_env = True,
    )

    return [DefaultInfo(files = depset(outputs))]

flat_install = rule(
    implementation = _flat_install_impl,
    attrs = {
        "srcs": attr.label_list(providers = [DefaultInfo]),
        "install_pdb": attr.bool(default = False, doc = "Also stage .pdb debug-info files alongside DLLs/EXEs"),
    },
)

# ── tree_install ──────────────────────────────────────────────────────────────
# Stages a set of source files into a subdirectory tree, preserving their
# relative paths after stripping a known prefix.
#
# strip_prefix: path component within the repo to strip.
#   For "@python//:python_lib" (glob(["Lib/**"])), set strip_prefix = "Lib"
#   so "external/python+/Lib/abc.py" → "abc.py" → installed at dst/abc.py.
#   Set "" to strip nothing (files keep their full in-repo path under dst).
#
# flatten: discard the tree entirely — every file lands at dst/<basename>.
#   Use when the sources come from several packages, so no single strip_prefix
#   applies, and the tree carries no meaning at the destination.
#
# dst: destination subdirectory inside the rule's output dir.
#   e.g. "python-core-2.7.18/lib"
#
# Usage:
#   tree_install(
#       name = "install_python_lib",
#       src  = "@python//:python_lib",
#       strip_prefix = "Lib",
#       dst  = "python-core-2.7.18/lib",
#   )

def _tree_install_impl(ctx):
    strip = ctx.attr.strip_prefix  # e.g. "Lib"
    dst   = ctx.attr.dst           # e.g. "python-core-2.7.18/lib"

    all_files = ctx.files.src
    outputs   = []

    lines = ["@echo off"]
    for f in all_files:
        # Compute in-repo relative path by stripping the workspace root.
        ws_root = f.owner.workspace_root  # e.g. "external/python+"
        if ws_root:
            if f.is_source:
                # Source files: f.path = "external/python+/Lib/abc.py"
                in_repo = f.path[len(ws_root) + 1:]
            else:
                # Generated files (cc_binary etc.): f.path starts with
                # "bazel-out/<config>/bin/..." so we cannot strip ws_root
                # from the front.  short_path is "../python+/python.exe".
                repo_basename = ws_root.split("/")[-1]  # "python+"
                short_prefix = "../" + repo_basename + "/"
                short = f.short_path
                in_repo = short[len(short_prefix):] if short.startswith(short_prefix) else short
        else:
            in_repo = f.short_path

        # Strip the known prefix (e.g. "Lib/") to get the bare relative path.
        if ctx.attr.flatten:
            # No tree to preserve: every file lands directly in dst under its own
            # basename.  For inputs that come from SEVERAL packages there is no
            # single strip_prefix that works (each would need its own), and the
            # destination name is already fixed by the producing rule — as for the
            # Java UNO runtime jars, whose basenames are exact-string lookups in
            # javavm.cxx / UnoClassLoader (see build/rules/java_pipeline.bzl).
            rel = f.basename
        elif strip:
            marker = strip + "/"
            idx = in_repo.find(marker)
            if idx >= 0:
                rel = in_repo[idx + len(marker):]  # e.g. "abc.py"
            else:
                rel = in_repo
        else:
            rel = in_repo

        dst_rel = dst + "/" + rel  # e.g. "python-core-2.7.18/lib/abc.py"

        out = ctx.actions.declare_file(dst_rel)
        outputs.append((f, out))

    if not outputs:
        return [DefaultInfo(files = depset())]

    out_files = [o for _, o in outputs]

    # Ensure destination directories exist before copying.
    # One `if not exist … mkdir` per unique directory.
    dirs_seen = {}
    for _, out in outputs:
        d = out.dirname.replace("/", "\\")
        if d not in dirs_seen:
            dirs_seen[d] = True
            lines.append('if not exist "{d}" mkdir "{d}"'.format(d = d))

    for src_f, out in outputs:
        lines.append('copy /Y "{}" "{}" >nul || exit /b 1'.format(
            src_f.path.replace("/", "\\"),
            out.path.replace("/", "\\"),
        ))

    bat = ctx.actions.declare_file("_tree_stage_{}.bat".format(ctx.label.name))
    ctx.actions.write(output = bat, content = "\r\n".join(lines) + "\r\n", is_executable = True)

    ctx.actions.run(
        inputs  = depset([f for f, _ in outputs] + [bat]),
        outputs = out_files,
        executable = bat,
        use_default_shell_env = True,
    )

    return [DefaultInfo(files = depset(out_files))]

tree_install = rule(
    implementation = _tree_install_impl,
    attrs = {
        "src":          attr.label(allow_files = True),
        "strip_prefix": attr.string(default = "", doc = "In-repo path prefix to strip (e.g. 'Lib')"),
        "flatten":      attr.bool(default = False, doc = "Ignore strip_prefix and install every file flat into dst under its basename"),
        "dst":          attr.string(mandatory = True, doc = "Destination subdir in output (e.g. 'python-core-2.7.18/lib')"),
    },
)
