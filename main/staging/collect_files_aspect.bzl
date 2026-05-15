"""Aspect and rules for collecting and staging build outputs."""

_TRAVERSAL_ATTRS = ["deps", "srcs", "data", "exports", "additional_linker_inputs"]

def _collect_files_aspect_impl(target, ctx):
    transitive = []

    for attr_name in _TRAVERSAL_ATTRS:
        if hasattr(ctx.rule.attr, attr_name):
            for dep in getattr(ctx.rule.attr, attr_name):
                if OutputGroupInfo in dep:
                    transitive.append(dep[OutputGroupInfo].collected_files)

    files = depset(
        transitive = [target[DefaultInfo].files] + transitive,
    )
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

# Extensions included in the flat install directory.
_INSTALL_EXTS = {"exe": True, "dll": True, "rdb": True, "zip": True, "py": True, "ini": True}

def _flat_install_impl(ctx):
    all_files = depset(transitive = [
        dep[DefaultInfo].files for dep in ctx.attr.srcs
    ])

    seen = {}
    filtered = []
    for f in all_files.to_list():
        if f.extension.lower() not in _INSTALL_EXTS:
            continue
        if f.basename in seen:
            continue
        seen[f.basename] = True
        filtered.append(f)

    if not filtered:
        return [DefaultInfo(files = depset())]

    outputs = [ctx.actions.declare_file(f.basename) for f in filtered]

    lines = ["@echo off"]
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
    },
)

# ── tree_install ──────────────────────────────────────────────────────────────
# Stages a set of source files into a subdirectory tree, preserving their
# relative paths after stripping a known prefix.
#
# strip_prefix: path component within the repo to strip.
#   For "@python//:python_lib" (glob(["Lib/**"])), set strip_prefix = "Lib"
#   so "external/python+/Lib/abc.py" → "abc.py" → installed at dst/abc.py.
#   Set "" to strip nothing (files land at dst/<basename>).
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
            in_repo = f.path[len(ws_root) + 1:]  # e.g. "Lib/abc.py"
        else:
            in_repo = f.short_path

        # Strip the known prefix (e.g. "Lib/") to get the bare relative path.
        if strip:
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
        "dst":          attr.string(mandatory = True, doc = "Destination subdir in output (e.g. 'python-core-2.7.18/lib')"),
    },
)
