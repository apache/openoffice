"""images_pack — one images*.zip the way pack/makefile.mk builds it.

The ENTRY SET is not a directory glob: it is every image some resource bundle
names (the .ilst rsc2 writes beside each .res, read from rsc_res's "ilst"
output group) plus res/commandimagelist.  Each entry is taken from the first
theme directory that has it, else from default_images.  So every theme zip is
a COMPLETE set -- which it must be, because vcl's ImplImageTree reads
images.zip only for the "default" style and nothing else for any other.

All the real work is upstream's own solenv/bin scripts; packimages_driver.pl
only lays out their inputs.  See main/packimages/readme.md.
"""

load("//main/staging:res_bundles.bzl", "RES_BUNDLES")

_PERL = "@strawberry-perl//:perl_exe"

def _images_pack_impl(ctx):
    out = ctx.actions.declare_file(ctx.attr.out)
    work = ctx.actions.declare_directory(ctx.label.name + "_work")

    ilsts = depset(ctx.files.ilsts, transitive = [
        t[OutputGroupInfo].ilst
        for t in ctx.attr.res
    ]).to_list()
    ilst_list = ctx.actions.declare_file(ctx.label.name + "_ilst.txt")
    ctx.actions.write(ilst_list, "\n".join([f.path for f in ilsts]) + "\n")

    # The uiconfig tree's files are declared at <pkg>/soffice.cfg/modules/...;
    # the driver wants the soffice.cfg directory.
    uiconfig_root = None
    for f in ctx.files.uiconfig:
        i = f.path.find("/soffice.cfg/")
        if i >= 0:
            uiconfig_root = f.path[:i] + "/soffice.cfg"
            break
    if uiconfig_root == None:
        fail("uiconfig: no file under soffice.cfg/")

    args = ctx.actions.args()
    args.add(ctx.file._driver)
    args.add("--packimages", ctx.file._packimages)
    args.add("--imagesort", ctx.file._imagesort)
    args.add("--hicontrast-pl", ctx.file._hicontrast_pl)
    args.add("--default", ctx.attr.default_root)
    args.add("--sortlst", ctx.file.sort_list)
    args.add("--uiconfig", uiconfig_root)
    args.add("--work", work.path)
    args.add("--out", out)
    args.add("--ilst-list", ilst_list)
    for c in ctx.attr.custom_roots:
        args.add("--custom", c)
    if ctx.file.classic_tar:
        args.add("--classic-tar", ctx.file.classic_tar)
    if ctx.attr.hicontrast:
        args.add("--hicontrast")

    ctx.actions.run(
        executable = ctx.file._perl,
        arguments = [args],
        inputs = depset(
            [ctx.file._driver, ctx.file._packimages, ctx.file._imagesort,
             ctx.file._hicontrast_pl, ctx.file.sort_list, ilst_list] +
            ilsts + ctx.files.default_images + ctx.files.custom_images +
            ctx.files.uiconfig +
            ([ctx.file.classic_tar] if ctx.file.classic_tar else []),
        ),
        outputs = [out, work],
        mnemonic = "PackImages",
        progress_message = "Packing %s" % ctx.attr.out,
    )
    return [DefaultInfo(files = depset([out]))]

images_pack = rule(
    implementation = _images_pack_impl,
    attrs = {
        "out": attr.string(mandatory = True, doc = "e.g. \"images_industrial.zip\""),
        "res": attr.label_list(
            mandatory = True,
            doc = "rsc_res targets; their \"ilst\" output groups are the image lists.",
        ),
        "ilsts": attr.label_list(
            allow_files = [".ilst"],
            doc = "Image lists that come from no rsc_res (helpcontent2's helpimg.ilst).",
        ),
        "default_images": attr.label_list(allow_files = True, mandatory = True),
        "default_root": attr.string(mandatory = True, doc = "Exec-root path of default_images."),
        "custom_images": attr.label_list(allow_files = True),
        "custom_roots": attr.string_list(
            doc = "Exec-root theme directories, searched before default_images, first hit wins.",
        ),
        "classic_tar": attr.label(allow_single_file = True),
        "hicontrast": attr.bool(doc = "Derive the theme from default_images' high-contrast variants."),
        "sort_list": attr.label(allow_single_file = True, mandatory = True),
        "uiconfig": attr.label_list(allow_files = True, mandatory = True),
        "_driver": attr.label(default = "//main/packimages:packimages_driver.pl", allow_single_file = True),
        "_packimages": attr.label(default = "//main/solenv/bin:packimages.pl", allow_single_file = True),
        "_imagesort": attr.label(default = "//main/solenv/bin:image-sort.pl", allow_single_file = True),
        "_hicontrast_pl": attr.label(default = "//main/solenv/bin:hicontrast-to-theme.pl", allow_single_file = True),
        "_perl": attr.label(default = _PERL, allow_single_file = True, cfg = "exec"),
    },
)

# Resource bundles built only for their image list, never installed.
_IMAGE_ONLY_RES = [
    "//main/connectivity:hsqldb_res",
]

def images_theme(name, out, **kwargs):
    """images_pack with the inputs every theme shares (pack/makefile.mk)."""
    images_pack(
        name = name,
        out = out,
        res = RES_BUNDLES.keys() + _IMAGE_ONLY_RES,
        ilsts = ["//main/helpcontent2:helpimg_ilst"],
        default_images = ["//main/default_images:all_png"],
        default_root = "main/default_images",
        sort_list = "//main/packimages:pack/image-sort.lst",
        uiconfig = ["//main/postprocess:uiconfig_tree"],
        visibility = ["//visibility:public"],
        **kwargs
    )
