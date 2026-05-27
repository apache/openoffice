"""Starlark rule for packing OOo image sets into zip archives.

images_zip — collects PNG/BMP source files and packs them into a zip using
             make_images_zip.pl (Archive::Zip).  The zip entry name for each
             file is its path with strip_prefix/ removed from the front.
"""

_PERL = "@strawberry-perl//:perl_exe"

def _images_zip_impl(ctx):
    zip_file  = ctx.actions.declare_file(ctx.label.name + ".zip")
    list_file = ctx.actions.declare_file("_" + ctx.label.name + "_filelist.txt")

    perl   = ctx.file._perl
    script = ctx.file._make_images_zip

    ctx.actions.write(
        output  = list_file,
        content = "\n".join([f.path for f in ctx.files.srcs]) + "\n",
    )

    ctx.actions.run(
        executable = perl,
        arguments  = [script.path, zip_file.path, ctx.attr.strip_prefix, list_file.path],
        inputs     = [perl, script, list_file] + ctx.files.srcs,
        outputs    = [zip_file],
        mnemonic         = "ImagesZip",
        progress_message = "Creating {}.zip ({} images)".format(
            ctx.label.name, len(ctx.files.srcs)),
    )

    return [DefaultInfo(files = depset([zip_file]))]

images_zip = rule(
    implementation = _images_zip_impl,
    attrs = {
        "srcs": attr.label_list(
            allow_files = True,
            doc = "Image source files (PNG, BMP) to pack into the zip",
        ),
        "strip_prefix": attr.string(
            mandatory = True,
            doc = "Repo-relative path prefix to strip, e.g. 'main/default_images'",
        ),
        "_perl": attr.label(
            default           = _PERL,
            allow_single_file = True,
            cfg               = "exec",
        ),
        "_make_images_zip": attr.label(
            default           = "//build/tools:make_images_zip.pl",
            allow_single_file = True,
        ),
    },
)
