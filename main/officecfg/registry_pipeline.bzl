# registry_pipeline.bzl — helpers for officecfg's language-pack pipeline.
#
# for-loops with mutable accumulation are only valid in .bzl files, not
# in BUILD files — hence this module.

def lang_zip_srcs(langs, lang_id_fn):
    """Return a select()-chain that includes registry_zip_<locale> for each
    enabled locale.  Consumers add this to filegroup srcs with +.

    Each locale is gated by its //build:build_<id> config_setting, which is
    True only when --//build:lang_<id>=True is set (default: en-US only).
    """
    result = []
    for lang in langs:
        lid = lang_id_fn(lang)
        result = result + select({
            "//build:build_" + lid: [":registry_zip_" + lid],
            "//conditions:default": [],
        })
    return result

def _registry_zip_impl(ctx):
    """Create a registry_<locale>.zip using Bazel's built-in zipper tool.

    Input files come from two sources:
      - spool files: ctx.files.spool_files  (paths contain spool/<lang>/)
      - nonlocalized files: ctx.files.nonlocalized (paths contain registry/data/)

    The archive entry name is derived by stripping the source-tree prefix so
    that all entries are relative to the registry/data/ layout.
    """
    out = ctx.outputs.out
    zipper = ctx.executable._zipper

    args = ctx.actions.args()
    args.add("c")
    args.add(out.path)

    inputs = []

    spool_marker = "spool/" + ctx.attr.lang + "/"
    for f in ctx.files.spool_files:
        p = f.path.replace("\\", "/")
        idx = p.find(spool_marker)
        if idx == -1:
            fail("spool file {} does not contain expected marker {}".format(p, spool_marker))
        arc_name = p[idx + len(spool_marker):]
        args.add("{}={}".format(arc_name, f.path))
        inputs.append(f)

    data_marker = "registry/data/"
    for f in ctx.files.nonlocalized:
        p = f.path.replace("\\", "/")
        idx = p.find(data_marker)
        if idx == -1:
            fail("nonlocalized file {} does not contain expected marker {}".format(p, data_marker))
        arc_name = p[idx + len(data_marker):]
        args.add("{}={}".format(arc_name, f.path))
        inputs.append(f)

    ctx.actions.run(
        executable = zipper,
        arguments = [args],
        inputs = inputs,
        outputs = [out],
        mnemonic = "RegistryZip",
        progress_message = "Packaging registry_{}.zip".format(ctx.attr.lang),
    )

registry_zip = rule(
    implementation = _registry_zip_impl,
    attrs = {
        "lang": attr.string(mandatory = True),
        "spool_files": attr.label_list(allow_files = True),
        "nonlocalized": attr.label_list(allow_files = True),
        "_zipper": attr.label(
            default = "@bazel_tools//tools/zip:zipper",
            executable = True,
            cfg = "exec",
        ),
    },
    outputs = {"out": "registry_%{lang}.zip"},
)
