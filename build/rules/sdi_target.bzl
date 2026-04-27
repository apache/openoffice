"""
SDI (Slot Definition Interface) pipeline for Apache OpenOffice Bazel build.

svidl takes .sdi files and generates a C++ header (.hxx) containing
slot dispatch maps, item info tables, and other binding data used by
the SFX framework dispatcher.

svidl_bundle stages svidl.exe + all its runtime DLLs into a single flat
directory (main/idl/svidl_bundle/).  Every sdi_target instance shares this
one directory rather than re-staging the tool per module.

sdi_package stages .sdi source files under a named prefix so that
downstream sdi_target rules can include them with a module-qualified
path (e.g. include "sfx2/sfxitems.sdi").  The original build achieved
this via Package_sdi.mk delivering files to <outdir>/inc/<prefix>/.

Usage:
    sdi_target(
        name = "sfxslots_sdi",
        main_sdi = "sdi/sfxslots.sdi",
        exports  = "sdi/sfx.sdi",
        srcs     = glob(["sdi/*.sdi", "inc/sfx2/*.hrc"]),
        slot_name    = "sfxslots",
        include_dirs = ["main/sfx2/sdi", "main/sfx2/inc/sfx2", "main/sfx2/inc"],
    )

The rule places its output header at <name>_inc/<slot_name>.hxx.
Consuming cc_library should set includes = ["<name>_inc"].

    sdi_package(
        name = "sfx2_sdi_pkg",
        srcs = ["sdi/sfx.sdi", "sdi/sfxitems.sdi"],
        prefix = "sfx2",
        visibility = ["//visibility:public"],
    )
"""

# ── SdiPkgInfo / sdi_package ──────────────────────────────────────────────────

SdiPkgInfo = provider(
    doc = "Staged SDI files accessible as <prefix>/<basename> via the include_root -I path.",
    fields = {
        "files": "list of staged File objects",
        "include_root": "execroot-relative directory to pass as -I to svidl",
    },
)

def _sdi_package_impl(ctx):
    staged = []
    for src in ctx.files.srcs:
        out = ctx.actions.declare_file(ctx.label.name + "/" + ctx.attr.prefix + "/" + src.basename)
        ctx.actions.run_shell(
            inputs = [src],
            outputs = [out],
            command = "cp -f \"%s\" \"%s\"" % (src.path, out.path),
            mnemonic = "SdiPkgCopy",
            progress_message = "Staging %s" % src.basename,
        )
        staged.append(out)

    include_root = None
    if staged:
        # staged[0].dirname ends with /<prefix>; go one level up to get the -I root
        include_root = staged[0].dirname[:staged[0].dirname.rfind("/")]

    return [
        DefaultInfo(files = depset(staged)),
        SdiPkgInfo(files = staged, include_root = include_root),
    ]

sdi_package = rule(
    implementation = _sdi_package_impl,
    attrs = {
        "srcs": attr.label_list(allow_files = True, doc = ".sdi files to stage"),
        "prefix": attr.string(doc = "Directory prefix used in include statements (e.g. 'sfx2')"),
    },
)

# ── SvidlBundleInfo / svidl_bundle ────────────────────────────────────────────

SvidlBundleInfo = provider(
    doc = "Single flat directory with svidl.exe + all runtime DLLs, shared by all sdi_target instances.",
    fields = {
        "exe":       "The staged svidl.exe File",
        "tools_dir": "execroot-relative directory to set as PATH when running svidl",
        "files":     "All staged File objects (exe + DLLs)",
    },
)

def _svidl_bundle_impl(ctx):
    exe        = ctx.executable.svidl
    data_files = ctx.attr.svidl[DefaultInfo].data_runfiles.files.to_list()

    staged = {}
    for f in [exe] + data_files:
        if f.basename not in staged:
            out = ctx.actions.declare_file("svidl_bundle/" + f.basename)
            ctx.actions.symlink(output = out, target_file = f)
            staged[f.basename] = out

    all_staged = list(staged.values())
    exe_out    = staged[exe.basename]

    return [
        DefaultInfo(files = depset(all_staged)),
        SvidlBundleInfo(
            exe       = exe_out,
            tools_dir = exe_out.dirname,
            files     = all_staged,
        ),
    ]

svidl_bundle = rule(
    implementation = _svidl_bundle_impl,
    attrs = {
        "svidl": attr.label(
            executable  = True,
            cfg         = "exec",
            allow_files = True,
            doc         = "The svidl cc_binary",
        ),
    },
    doc = "Stages svidl.exe + all runtime DLLs into one flat directory in main/idl/. " +
          "All sdi_target instances share this bundle; no per-module tool staging.",
)

# ── sdi_target ────────────────────────────────────────────────────────────────

def _sdi_target_impl(ctx):
    bundle     = ctx.attr._svidl_bundle[SvidlBundleInfo]
    svidl_exe  = bundle.exe
    tools_dir  = bundle.tools_dir
    all_staged = bundle.files

    # ── Declare the .hxx output ─────────────────────────────────────────────
    hxx = ctx.actions.declare_file(
        ctx.label.name + "_inc/" + ctx.attr.slot_name + ".hxx",
    )

    # ── Collect pkg_deps (staged sdi packages) ──────────────────────────────
    pkg_files = []
    pkg_include_dirs = []
    for dep in ctx.attr.pkg_deps:
        info = dep[SdiPkgInfo]
        pkg_files.extend(info.files)
        if info.include_root and info.include_root not in pkg_include_dirs:
            pkg_include_dirs.append(info.include_root)

    # ── Build include args ───────────────────────────────────────────────────
    include_args = (
        ["-I" + d for d in ctx.attr.include_dirs] +
        ["-I" + d for d in pkg_include_dirs]
    )

    # ── Run svidl ───────────────────────────────────────────────────────────
    ctx.actions.run(
        executable = svidl_exe.path,
        arguments  = ["-quiet"] + include_args + [
            "-fs" + hxx.path,
            "-fx" + ctx.file.exports.path,
            ctx.file.main_sdi.path,
        ],
        inputs  = (ctx.files.srcs +
                   pkg_files +
                   [ctx.file.exports, ctx.file.main_sdi] +
                   all_staged),
        outputs = [hxx],
        env     = {"PATH": tools_dir},
        mnemonic         = "SdiTarget",
        progress_message = "Generating %s from SDI" % hxx.short_path,
        use_default_shell_env = False,
    )

    return [DefaultInfo(files = depset([hxx]))]

sdi_target = rule(
    implementation = _sdi_target_impl,
    attrs = {
        "main_sdi": attr.label(
            allow_single_file = True,
            doc = "Primary .sdi input file (e.g. sdi/sfxslots.sdi)",
        ),
        "exports": attr.label(
            allow_single_file = True,
            doc = "Export .sdi file passed to -fx (e.g. sdi/sfx.sdi)",
        ),
        "srcs": attr.label_list(
            allow_files = True,
            doc = "Supporting .sdi and .hrc files referenced during parsing",
        ),
        "slot_name": attr.string(
            doc = "Base name for generated output (e.g. 'sfxslots' → sfxslots.hxx)",
        ),
        "pkg_deps": attr.label_list(
            default = [],
            providers = [SdiPkgInfo],
            doc = "sdi_package targets whose staged files are added as inputs with their include roots",
        ),
        "include_dirs": attr.string_list(
            default = [],
            doc = "Extra -I paths (relative to execroot) for svidl include resolution",
        ),
        "_svidl_bundle": attr.label(
            default     = "//main/idl:svidl_bundle",
            providers   = [SvidlBundleInfo],
            cfg         = "exec",
            allow_files = True,
            doc         = "Shared tool bundle — svidl.exe + runtime DLLs staged once in main/idl/svidl_bundle/",
        ),
    },
)
