"""
lngconvex tool bundle + .ulf → .rc conversion rule.

lngconvex_bundle stages lngconvex.exe + all runtime DLLs into a single flat
directory (main/shell/lngconvex_bundle/) so the tool can find its DLL
dependencies when run as a Bazel action.  Mirrors the svidl_bundle approach
in sdi_target.bzl.

lngconvex_rc invokes lngconvex to convert a .ulf localisation file + RC
template fragments into a Windows .rc resource file.

Usage:

    lngconvex_bundle(
        name = "lngconvex_bundle",
        lngconvex = ":lngconvex",
        visibility = ["//visibility:public"],
    )

    lngconvex_rc(
        name = "soreport_rc",
        out  = "soreport.rc",
        ulf  = "source/all/crashrep.ulf",
        rct  = "source/win32/rctemplate.txt",
        rch  = "source/win32/rcheader.txt",
        rcf  = "source/win32/rcfooter.txt",
    )
"""

# ── LngconvexBundleInfo / lngconvex_bundle ────────────────────────────────────

LngconvexBundleInfo = provider(
    doc = "Single flat directory with lngconvex.exe + all runtime DLLs.",
    fields = {
        "exe":       "The staged lngconvex.exe File",
        "tools_dir": "execroot-relative directory to set as PATH when running lngconvex",
        "files":     "All staged File objects (exe + DLLs + manifest)",
    },
)

def _lngconvex_bundle_impl(ctx):
    exe        = ctx.executable.lngconvex
    data_files = ctx.attr.lngconvex[DefaultInfo].data_runfiles.files.to_list()

    staged = {}
    for f in [exe] + data_files:
        if f.basename not in staged:
            out = ctx.actions.declare_file("lngconvex_bundle/" + f.basename)
            ctx.actions.symlink(output = out, target_file = f)
            staged[f.basename] = out

    all_staged = list(staged.values())
    exe_out    = staged[exe.basename]

    return [
        DefaultInfo(files = depset(all_staged)),
        LngconvexBundleInfo(
            exe       = exe_out,
            tools_dir = exe_out.dirname,
            files     = all_staged,
        ),
    ]

lngconvex_bundle = rule(
    implementation = _lngconvex_bundle_impl,
    attrs = {
        "lngconvex": attr.label(
            executable  = True,
            cfg         = "exec",
            allow_files = True,
            doc         = "The lngconvex cc_binary",
        ),
    },
    doc = "Stages lngconvex.exe + all runtime DLLs into one flat directory. " +
          "All lngconvex_rc instances share this bundle.",
)

# ── lngconvex_rc ──────────────────────────────────────────────────────────────

def _lngconvex_rc_impl(ctx):
    bundle     = ctx.attr._lngconvex_bundle[LngconvexBundleInfo]
    exe        = bundle.exe
    tools_dir  = bundle.tools_dir
    all_staged = bundle.files

    rc_out = ctx.actions.declare_file(ctx.attr.out)

    ctx.actions.run(
        executable = exe.path,
        arguments  = [
            "-ulf", ctx.file.ulf.path,
            "-rc",  rc_out.path,
            "-rct", ctx.file.rct.path,
            "-rch", ctx.file.rch.path,
            "-rcf", ctx.file.rcf.path,
        ],
        inputs  = [ctx.file.ulf, ctx.file.rct, ctx.file.rch, ctx.file.rcf] + all_staged,
        outputs = [rc_out],
        env     = {"PATH": tools_dir},
        mnemonic         = "LngconvexRc",
        progress_message = "Generating %s from ULF" % rc_out.short_path,
        use_default_shell_env = False,
    )

    return [DefaultInfo(files = depset([rc_out]))]

lngconvex_rc = rule(
    implementation = _lngconvex_rc_impl,
    attrs = {
        "ulf": attr.label(
            allow_single_file = True,
            doc = ".ulf localisation input file",
        ),
        "rct": attr.label(
            allow_single_file = True,
            doc = "RC template file passed to -rct",
        ),
        "rch": attr.label(
            allow_single_file = True,
            doc = "RC header file passed to -rch",
        ),
        "rcf": attr.label(
            allow_single_file = True,
            doc = "RC footer file passed to -rcf",
        ),
        "out": attr.string(
            doc = "Output .rc filename (e.g. 'soreport.rc')",
        ),
        "_lngconvex_bundle": attr.label(
            default     = "//main/shell:lngconvex_bundle",
            providers   = [LngconvexBundleInfo],
            cfg         = "exec",
            allow_files = True,
            doc         = "Shared tool bundle — lngconvex.exe + runtime DLLs staged in main/shell/lngconvex_bundle/",
        ),
    },
)
