"""
SDI (Slot Definition Interface) pipeline for Apache OpenOffice Bazel build.

svidl takes .sdi files and generates a C++ header (.hxx) containing
slot dispatch maps, item info tables, and other binding data used by
the SFX framework dispatcher.

svidl.exe dynamically links against sal3.dll and tl.dll.
tl.dll in turn pulls in cppu3, comphelp, cppuhelper3MSC, salhelper3MSC,
ucbhelper, vos3, basegfx, and i18nisolang1.  All of these are staged in
the same directory as svidl.exe so the Windows PE loader finds them via
the EXE-directory search rule without requiring PATH changes.

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
"""

# DLLs that need runtime-name renaming.
# sal's import lib records LIBRARY sal3, so Windows looks for sal3.dll.
_DLL_RENAME = [
    ("_sal_dll",            "sal3.dll"),
    # All others: Bazel target name already matches the expected DLL name.
    ("_tl_dll",             "tl.dll"),
    ("_cppu3_dll",          "cppu3.dll"),
    ("_comphelp_dll",       "comphelp.dll"),
    ("_cppuhelper_dll",     "cppuhelper3MSC.dll"),
    ("_salhelper_dll",      "salhelper3MSC.dll"),
    ("_ucbhelper_dll",      "ucbhelper.dll"),
    ("_vos3_dll",           "vos3.dll"),
    ("_basegfx_dll",        "basegfx.dll"),
    ("_i18nisolang1_dll",   "i18nisolang1.dll"),
]

def _sdi_target_impl(ctx):
    svidl = ctx.executable._svidl

    # ── Stage svidl.exe + all runtime DLLs into one flat directory ─────────
    dll_files = [getattr(ctx.file, attr) for attr, _ in _DLL_RENAME]
    crt_files = ctx.files._crt_dlls

    staged = {}

    # Stage svidl.exe itself
    svidl_out = ctx.actions.declare_file(ctx.label.name + "_tools/svidl.exe")
    ctx.actions.symlink(output = svidl_out, target_file = svidl)
    staged["svidl.exe"] = svidl_out

    # Stage svidl.exe.manifest
    manifest_out = ctx.actions.declare_file(ctx.label.name + "_tools/svidl.exe.manifest")
    ctx.actions.symlink(output = manifest_out, target_file = ctx.file._app_manifest)
    staged["svidl.exe.manifest"] = manifest_out

    # Stage DLLs with their runtime names
    for (attr, runtime_name), src_file in zip(_DLL_RENAME, dll_files):
        out = ctx.actions.declare_file(ctx.label.name + "_tools/" + runtime_name)
        ctx.actions.symlink(output = out, target_file = src_file)
        staged[runtime_name] = out

    # Stage CRT DLLs (keep original names)
    for f in crt_files:
        out = ctx.actions.declare_file(ctx.label.name + "_tools/" + f.basename)
        ctx.actions.symlink(output = out, target_file = f)
        staged[f.basename] = out

    all_staged = list(staged.values())
    tools_dir  = staged["svidl.exe"].dirname

    # ── Declare the .hxx output ─────────────────────────────────────────────
    hxx = ctx.actions.declare_file(
        ctx.label.name + "_inc/" + ctx.attr.slot_name + ".hxx",
    )

    # ── Build include args ───────────────────────────────────────────────────
    include_args = ["-I" + d for d in ctx.attr.include_dirs]

    # ── Run svidl ───────────────────────────────────────────────────────────
    ctx.actions.run(
        executable = staged["svidl.exe"].path,
        arguments  = ["-quiet"] + include_args + [
            "-fs" + hxx.path,
            "-fx" + ctx.file.exports.path,
            ctx.file.main_sdi.path,
        ],
        inputs  = (ctx.files.srcs +
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
        "include_dirs": attr.string_list(
            default = [],
            doc = "Extra -I paths (relative to execroot) for svidl include resolution",
        ),
        "_svidl": attr.label(
            default     = "//main/idl:svidl",
            executable  = True,
            cfg         = "exec",
            allow_files = True,
        ),
        # ── Runtime DLLs ────────────────────────────────────────────────────
        # svidl → sal3 + tl; tl → cppu3, comphelp, cppuhelper3MSC,
        #          salhelper3MSC, ucbhelper, vos3, basegfx, i18nisolang1
        "_sal_dll": attr.label(
            default = "//main/sal:sal", allow_single_file = True, cfg = "exec",
        ),
        "_tl_dll": attr.label(
            default = "//main/tools:tl", allow_single_file = True, cfg = "exec",
        ),
        "_cppu3_dll": attr.label(
            default = "//main/cppu:cppu3", allow_single_file = True, cfg = "exec",
        ),
        "_comphelp_dll": attr.label(
            default = "//main/comphelper:comphelp", allow_single_file = True, cfg = "exec",
        ),
        "_cppuhelper_dll": attr.label(
            default = "//main/cppuhelper:cppuhelper3MSC", allow_single_file = True, cfg = "exec",
        ),
        "_salhelper_dll": attr.label(
            default = "//main/salhelper:salhelper3MSC", allow_single_file = True, cfg = "exec",
        ),
        "_ucbhelper_dll": attr.label(
            default = "//main/ucbhelper:ucbhelper", allow_single_file = True, cfg = "exec",
        ),
        "_vos3_dll": attr.label(
            default = "//main/vos:vos3", allow_single_file = True, cfg = "exec",
        ),
        "_basegfx_dll": attr.label(
            default = "//main/basegfx:basegfx", allow_single_file = True, cfg = "exec",
        ),
        "_i18nisolang1_dll": attr.label(
            default = "//main/i18npool:i18nisolang1", allow_single_file = True, cfg = "exec",
        ),
        "_crt_dlls": attr.label(
            default = "//main/external/msvcp90:msvcp90", allow_files = True, cfg = "exec",
        ),
        "_app_manifest": attr.label(
            default = "//main/external/msvcp90:vc90_app_manifest",
            allow_single_file = True,
            cfg = "exec",
        ),
    },
)
