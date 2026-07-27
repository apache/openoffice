"""copy_file — re-expose a single file under a different name.

Pure Bazel (ctx.actions.symlink), no shell: cmd.exe `copy` chokes on the
forward-slash paths Bazel hands to genrules, and the staging rules already
symlink their inputs, so an extra link level costs nothing.

Used to give a cc_binary output a name Bazel cannot produce itself: a
linkshared cc_binary is always named "<target>.dll" on Windows, but Python
2.7's extension-module loader only accepts ".pyd" (Python/dynload_win.c).
"""

def _copy_file_impl(ctx):
    out = ctx.actions.declare_file(ctx.attr.out)
    ctx.actions.symlink(output = out, target_file = ctx.file.src)
    return [DefaultInfo(files = depset([out]))]

copy_file = rule(
    implementation = _copy_file_impl,
    doc = "Copies (symlinks) src to a file named `out` in this package.",
    attrs = {
        "src": attr.label(
            allow_single_file = True,
            mandatory = True,
            doc = "Single file to re-expose.",
        ),
        "out": attr.string(
            mandatory = True,
            doc = "Basename of the produced file.",
        ),
    },
)
