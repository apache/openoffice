"""odkcommon.zip assembly: individual files + the gendocu reference TREES.

oxt_package zips a fixed set of files (archive path known at analysis time).
The API reference targets (idl/cpp/java) are TREE artifacts whose file set is
only known after autodoc/javadoc run, so they cannot be enumerated into
OxtEntriesInfo.  This rule zips both in one zipper action: the known entries as
literal "<archive path>=<file>" operands, and each tree expanded at EXECUTION
time via Args.add_all(expand_directories = True).

The archive path for a tree file is its short_path with the leading "main/odk/"
removed -- which is exactly the tree's declared out_dir (e.g. "docs/common/ref")
followed by the file's path within it.  This rule is therefore specific to
trees declared in //main/odk.
"""

load("//build/rules:extension.bzl", "OxtEntriesInfo")

_PKG_PREFIX = "main/odk/"

def _tree_entry(f):
    # Map one expanded tree file to a zipper "<archive path>=<file>" operand,
    # or None to drop it.  autodoc writes a diagnostics file that upstream does
    # not ship, so skip it.
    if f.basename == "Autodoc_DiagnosticMessages.txt":
        return None
    if not f.short_path.startswith(_PKG_PREFIX):
        fail("odk_sdk_zip: tree file %s is not under %s" % (f.short_path, _PKG_PREFIX))
    return f.short_path[len(_PKG_PREFIX):] + "=" + f.path

def _odk_sdk_zip_impl(ctx):
    entries = {}
    for t in ctx.attr.contents:
        for path, f in t[OxtEntriesInfo].entries.items():
            if path in entries:
                fail("%s is contributed twice (again by %s)" % (path, t.label))
            entries[path] = f

    out = ctx.actions.declare_file(ctx.attr.out)

    flags = ctx.actions.args()
    flags.add("cC")  # create, deflate
    flags.add(out)

    entry_args = ctx.actions.args()
    for path in sorted(entries.keys()):
        entry_args.add("%s=%s" % (path, entries[path].path))
    for tree in ctx.files.trees:
        entry_args.add_all(
            [tree],
            map_each = _tree_entry,
            expand_directories = True,
        )
    entry_args.use_param_file("@%s", use_always = True)
    entry_args.set_param_file_format("multiline")

    ctx.actions.run(
        executable = ctx.executable._zipper,
        arguments = [flags, entry_args],
        inputs = depset(entries.values() + ctx.files.trees),
        outputs = [out],
        mnemonic = "OdkSdkZip",
        progress_message = "Packaging %s" % ctx.attr.out,
    )
    return [DefaultInfo(files = depset([out]))]

odk_sdk_zip = rule(
    implementation = _odk_sdk_zip_impl,
    attrs = {
        "out": attr.string(mandatory = True, doc = "Archive filename, e.g. \"odkcommon.zip\"."),
        "contents": attr.label_list(
            providers = [OxtEntriesInfo],
            doc = "ext_files groups: the individual (non-tree) SDK files.",
        ),
        "trees": attr.label_list(
            doc = "Tree-artifact reference targets (idl/cpp/java), zipped at " +
                  "their declared out_dir path inside the archive.",
        ),
        "_zipper": attr.label(
            default = "@bazel_tools//tools/zip:zipper",
            executable = True,
            cfg = "exec",
        ),
    },
)
