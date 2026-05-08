"""Starlark rules for the postprocess pipeline.

services_rdb  : merges .component XML files → services.rdb via xsltproc.
                make_services_input.pl writes services.input with absolute
                file:/// URIs (using Perl's cwd()); xsltproc then processes it.
                Absolute URIs are required because packcomponents.xslt's
                document() resolves relative URIs against the stylesheet's base
                URI, not the input document's base URI.

uiconfig_zip  : collects uiconfig/**/*.xml from all modules and creates
                uiconfig.zip via make_uiconfig_zip.pl (Archive::Zip).
                File list is written by ctx.actions.write to avoid Windows
                command-line length limits with 400+ files.

Both rules use @strawberry-perl//:perl_exe, already present in MODULE.bazel.
No shell (bash / cmd.exe / PowerShell) is required.
"""

# ── helpers ───────────────────────────────────────────────────────────────────

_PERL = "@strawberry-perl//:perl_exe"

# ── services_rdb ─────────────────────────────────────────────────────────────

def _services_rdb_impl(ctx):
    rdb_file   = ctx.actions.declare_file(ctx.label.name + ".rdb")
    input_file = ctx.actions.declare_file(ctx.label.name + ".input")

    perl   = ctx.file._perl
    script = ctx.file._make_services_input

    # Action 1: write services.input with absolute file:/// URIs.
    ctx.actions.run(
        executable = perl,
        arguments  = (
            [script.path, input_file.path] +
            [f.path for f in ctx.files.components]
        ),
        inputs  = [perl, script] + ctx.files.components,
        outputs = [input_file],
        mnemonic         = "ServicesInput",
        progress_message = "Generating {}".format(input_file.basename),
    )

    # Action 2: xsltproc merges services.input → services.rdb.
    ctx.actions.run(
        executable = ctx.executable._xsltproc,
        arguments  = [
            "--nonet",
            "--stringparam", "prefix", "",
            "-o", rdb_file.path,
            ctx.file.xslt.path,
            input_file.path,
        ],
        inputs  = [input_file, ctx.file.xslt] + ctx.files.components,
        outputs = [rdb_file],
        mnemonic         = "ServicesRdb",
        progress_message = "Merging {} components → {}".format(
            len(ctx.files.components), rdb_file.basename),
    )

    return [DefaultInfo(files = depset([rdb_file, input_file]))]

services_rdb = rule(
    implementation = _services_rdb_impl,
    attrs = {
        "components": attr.label_list(
            allow_files = True,
            doc = ".component XML files to merge into the output .rdb",
        ),
        "xslt": attr.label(
            allow_single_file = True,
            mandatory = True,
            doc = "packcomponents.xslt",
        ),
        "_perl": attr.label(
            default         = _PERL,
            allow_single_file = True,
            cfg             = "exec",
        ),
        "_make_services_input": attr.label(
            default         = "//build/tools:make_services_input.pl",
            allow_single_file = True,
        ),
        "_xsltproc": attr.label(
            default    = "@libxslt//:xsltproc",
            executable = True,
            cfg        = "exec",
        ),
    },
)

# ── uiconfig_zip ──────────────────────────────────────────────────────────────

def _uiconfig_zip_impl(ctx):
    zip_file  = ctx.actions.declare_file("uiconfig.zip")
    list_file = ctx.actions.declare_file("_uiconfig_filelist.txt")

    perl   = ctx.file._perl
    script = ctx.file._make_uiconfig_zip

    # Write the file list via ctx.actions.write to avoid Windows command-line
    # length limits (437 files × ~70 chars ≈ 30 KB > 32 KB CreateProcess cap).
    ctx.actions.write(
        output  = list_file,
        content = "\n".join([f.path for f in ctx.files.srcs]) + "\n",
    )

    ctx.actions.run(
        executable = perl,
        arguments  = [script.path, zip_file.path, list_file.path],
        inputs     = [perl, script, list_file] + ctx.files.srcs,
        outputs    = [zip_file],
        mnemonic         = "UiConfigZip",
        progress_message = "Creating uiconfig.zip ({} XML files)".format(
            len(ctx.files.srcs)),
    )

    return [DefaultInfo(files = depset([zip_file]))]

uiconfig_zip = rule(
    implementation = _uiconfig_zip_impl,
    attrs = {
        "srcs": attr.label_list(
            allow_files = True,
            doc = "All uiconfig/**/*.xml source files to zip",
        ),
        "_perl": attr.label(
            default         = _PERL,
            allow_single_file = True,
            cfg             = "exec",
        ),
        "_make_uiconfig_zip": attr.label(
            default         = "//build/tools:make_uiconfig_zip.pl",
            allow_single_file = True,
        ),
    },
)
