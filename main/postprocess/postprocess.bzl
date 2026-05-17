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

fcfg_merge    : merges filter-config fragment .xcu files (types/, filters/,
                frameloaders/, contenthandlers/) into a single combined XCU
                via fcfg_merge.pl.  Replaces the Java/Python FCFGMerge tool.
                Fragment file list written by ctx.actions.write; xmlpackage
                selects the oor:name (Types | Filter | Misc | GraphicFilter).

pack_registry : assembles XCS + XCU + spool XCU files into a .xcd registry
                package via packregistry.xslt.  Uses make_services_input.pl
                to build the file:/// URI list; xcs must precede xcu in the
                list (configmgr requirement).

All rules use @strawberry-perl//:perl_exe.  No shell required.
"""

# ── helpers ───────────────────────────────────────────────────────────────────

_PERL = "@strawberry-perl//:perl_exe"

# ── services_rdb ─────────────────────────────────────────────────────────────

def _services_rdb_impl(ctx):
    out_name   = ctx.attr.out if ctx.attr.out else ctx.label.name + ".rdb"
    rdb_file   = ctx.actions.declare_file(out_name)
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
        "out": attr.string(
            default = "",
            doc     = "Output filename (default: <name>.rdb)",
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

# ── fcfg_merge ────────────────────────────────────────────────────────────────

def _fcfg_merge_impl(ctx):
    xcu      = ctx.actions.declare_file(ctx.label.name + ".xcu")
    manifest = ctx.actions.declare_file(ctx.label.name + "_manifest.txt")

    perl   = ctx.file._perl
    script = ctx.file._fcfg_merge

    # Build manifest content: section keywords followed by absolute paths.
    lines = []
    for section, frags in [
        ("types",    ctx.files.types),
        ("filters",  ctx.files.filters),
        ("loaders",  ctx.files.loaders),
        ("handlers", ctx.files.handlers),
    ]:
        if frags:
            lines.append(section)
            lines.extend([f.path for f in frags])

    ctx.actions.write(
        output  = manifest,
        content = "\n".join(lines) + "\n",
    )

    all_frags = (ctx.files.types + ctx.files.filters +
                 ctx.files.loaders + ctx.files.handlers)

    ctx.actions.run(
        executable = perl,
        arguments  = [script.path, xcu.path, ctx.attr.xmlpackage, manifest.path],
        inputs     = [perl, script, manifest] + all_frags,
        outputs    = [xcu],
        mnemonic         = "FcfgMerge",
        progress_message = "Merging {} fragments → {}".format(
            len(all_frags), xcu.basename),
    )

    return [DefaultInfo(files = depset([xcu]))]

fcfg_merge = rule(
    implementation = _fcfg_merge_impl,
    attrs = {
        "xmlpackage": attr.string(
            mandatory = True,
            doc       = "oor:name value: Types | Filter | Misc | GraphicFilter",
        ),
        "types": attr.label_list(
            allow_files = True,
            doc = "type fragment .xcu files (types/ subdir)",
        ),
        "filters": attr.label_list(
            allow_files = True,
            doc = "filter fragment .xcu files (filters/ or internalgraphicfilters/)",
        ),
        "loaders": attr.label_list(
            allow_files = True,
            doc = "frame-loader fragment .xcu files (frameloaders/ subdir)",
        ),
        "handlers": attr.label_list(
            allow_files = True,
            doc = "content-handler fragment .xcu files (contenthandlers/ subdir)",
        ),
        "_perl": attr.label(
            default         = _PERL,
            allow_single_file = True,
            cfg             = "exec",
        ),
        "_fcfg_merge": attr.label(
            default         = "//build/tools:fcfg_merge.pl",
            allow_single_file = True,
        ),
    },
)

# ── pack_registry ─────────────────────────────────────────────────────────────

def _pack_registry_impl(ctx):
    out_name   = ctx.attr.out if ctx.attr.out else ctx.label.name + ".xcd"
    xcd_file   = ctx.actions.declare_file(out_name)
    input_file = ctx.actions.declare_file(ctx.label.name + ".input")

    perl   = ctx.file._perl
    script = ctx.file._make_services_input

    # XCS files must precede XCU files (configmgr requirement).
    all_files = ctx.files.xcs + ctx.files.xcu

    # Action 1: write input list with absolute file:/// URIs.
    ctx.actions.run(
        executable = perl,
        arguments  = (
            [script.path, input_file.path] +
            [f.path for f in all_files]
        ),
        inputs  = [perl, script] + all_files,
        outputs = [input_file],
        mnemonic         = "RegistryInput",
        progress_message = "Generating {}".format(input_file.basename),
    )

    # Action 2: xsltproc packs all files → XCD.
    ctx.actions.run(
        executable = ctx.executable._xsltproc,
        arguments  = [
            "--nonet",
            "--stringparam", "prefix", "",
            "-o", xcd_file.path,
            ctx.file.xslt.path,
            input_file.path,
        ],
        inputs  = [input_file, ctx.file.xslt] + all_files,
        outputs = [xcd_file],
        mnemonic         = "PackRegistry",
        progress_message = "Packing {} files → {}".format(
            len(all_files), xcd_file.basename),
    )

    return [DefaultInfo(files = depset([xcd_file]))]

pack_registry = rule(
    implementation = _pack_registry_impl,
    attrs = {
        "out": attr.string(
            default = "",
            doc     = "Output filename (default: <name>.xcd)",
        ),
        "xcs": attr.label_list(
            allow_files = True,
            doc = ".xcs schema files (must precede xcu in the registry list)",
        ),
        "xcu": attr.label_list(
            allow_files = True,
            doc = ".xcu data + spool files",
        ),
        "xslt": attr.label(
            allow_single_file = True,
            mandatory         = True,
            doc               = "packregistry.xslt",
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

# ── Label helpers for pack_registry targets ───────────────────────────────────
# These live here (not in BUILD.bazel) because BUILD files may not define functions.

def oc_xcs(path):
    """Return a label for an officecfg XCS schema file."""
    return "//main/officecfg:registry/schema/org/openoffice/" + path

def oc_xcu(path):
    """Return a label for an officecfg XCU data file."""
    return "//main/officecfg:registry/data/org/openoffice/" + path

def oc_mod(path, module):
    """Return a label for an alllang_module spool-file output."""
    return "//main/officecfg:spool_module/{}/org/openoffice/{}".format(module, path)

def fcfg(name):
    """Return a label for an fcfg_merge target in the filter fragments package."""
    return "//main/filter/source/config/fragments:" + name
