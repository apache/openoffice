"""
IDL pipeline rules for Apache OpenOffice Bazel build.

Pipeline:  .idl --(idlc)--> .urd --(regmerge)--> .rdb --(cppumaker)--> .hpp/.hdl

All tools are MSVC-compiled Windows PEs.  ctx.actions.run() invokes them
directly (no bash/cmd wrapper), which avoids the WinSxS / MSYS2 exec issues
that affect genrule cmd/cmd_bat on this toolchain.

The VS2008 CRT DLLs (msvcr90.dll, msvcp90.dll, msvcm90.dll) are checked in
under //main/external/msvcp90.  We stage all executables + DLLs into a single
directory and pass that directory in PATH so the Windows DLL loader finds them.

sal.if.lib records "LIBRARY sal3", so at runtime Windows looks for sal3.dll.
We create a copy named sal3.dll alongside sal.dll in the staging directory.

idlc finds ucpp.exe by replacing "idlc" in its own executable path with
"ucpp.exe", so ucpp must be staged alongside idlc.
"""

# DLL files that need to be staged alongside the executables, with the name
# Windows expects at runtime (some differ from the Bazel output name).
_DLL_RENAME = [
    # (attr name,          runtime name)
    ("_sal_dll",           "sal3.dll"),
    ("_salhelper_dll",     "salhelper3MSC.dll"),
    ("_store_dll",         "store.dll"),
    ("_reg_dll",           "reg.dll"),
]

def _idl_library_impl(ctx):
    idlc      = ctx.executable._idlc
    ucpp      = ctx.executable._ucpp
    regmerge  = ctx.executable._regmerge
    cppumaker = ctx.executable._cppumaker

    # Collect DLL files that need to be co-located with the tools
    dll_files = [getattr(ctx.file, attr) for attr, _ in _DLL_RENAME]
    crt_files = ctx.files._crt_dlls

    # All files that go into the staging directory
    all_staged = [idlc, ucpp, regmerge, cppumaker] + dll_files + crt_files

    # ── Stage all tools + DLLs into one directory ────────────────────────
    # run_shell (bash) is used only for file copying — bash cannot exec MSVC
    # PEs but it can copy files.  The actual tool invocations use run() below.
    tools_dir = ctx.actions.declare_directory(ctx.label.name + "_tools")

    # Build rename map: File object -> runtime name
    rename = {}
    for attr, runtime in _DLL_RENAME:
        f = getattr(ctx.file, attr)
        rename[f] = runtime

    copy_cmds = []
    for f in all_staged:
        dst_name = rename.get(f, f.basename)
        copy_cmds.append('cp -f "{src}" "{dst}/{name}"'.format(
            src  = f.path,
            dst  = tools_dir.path,
            name = dst_name,
        ))

    ctx.actions.run_shell(
        inputs   = all_staged,
        outputs  = [tools_dir],
        command  = " && ".join(copy_cmds),
        mnemonic = "StageIdlTools",
        progress_message = "Staging IDL pipeline tools",
    )

    # ── Compile each .idl → .urd ─────────────────────────────────────────
    # idlc writes <basename>.urd into the -O directory (it strips the path,
    # keeping only the basename per idlcmain.cxx).  We declare each .urd
    # in a flat output directory.
    urd_dir_name = ctx.label.name + "_urd"
    urd_files = []

    for idl in ctx.files.srcs:
        urd = ctx.actions.declare_file(urd_dir_name + "/" + idl.basename[:-4] + ".urd")
        urd_files.append(urd)

        ctx.actions.run(
            executable = tools_dir.path + "/idlc.exe",
            arguments  = [
                "-I" + ctx.label.package,  # include root (e.g. main/udkapi)
                "-O" + urd.dirname,        # output dir
                "-C",                      # keep comments
                idl.path,
            ],
            inputs           = [idl, tools_dir],
            outputs          = [urd],
            env              = {"PATH": tools_dir.path},
            mnemonic         = "Idlc",
            progress_message = "Compiling %s" % idl.short_path,
            use_default_shell_env = False,
        )

    # ── Merge all .urd → single .rdb ─────────────────────────────────────
    rdb = ctx.actions.declare_file(ctx.label.name + ".rdb")
    ctx.actions.run(
        executable = tools_dir.path + "/regmerge.exe",
        arguments  = [rdb.path, "UCR"] + [u.path for u in urd_files],
        inputs     = urd_files + [tools_dir],
        outputs    = [rdb],
        env        = {"PATH": tools_dir.path},
        mnemonic         = "RegMerge",
        progress_message = "Merging %s.rdb" % ctx.label.name,
        use_default_shell_env = False,
    )

    # ── Generate C++ headers from .rdb ───────────────────────────────────
    hdr_dir = ctx.actions.declare_directory(ctx.label.name + "_inc")
    ctx.actions.run(
        executable = tools_dir.path + "/cppumaker.exe",
        arguments  = [
            "-Gc",             # generate include guards
            "-L",              # local (relative) includes
            "-BUCR",           # browse UCR section
            "-O" + hdr_dir.path,
            rdb.path,
        ],
        inputs           = [rdb, tools_dir],
        outputs          = [hdr_dir],
        env              = {"PATH": tools_dir.path},
        mnemonic         = "CppuMaker",
        progress_message = "Generating headers from %s.rdb" % ctx.label.name,
        use_default_shell_env = False,
    )

    return [
        DefaultInfo(files = depset([rdb, hdr_dir])),
    ]

# ---------------------------------------------------------------------------
# idl_library rule definition
# ---------------------------------------------------------------------------

idl_library = rule(
    implementation = _idl_library_impl,
    attrs = {
        "srcs": attr.label_list(
            allow_files = [".idl"],
            doc = "IDL source files to compile",
        ),
        "_idlc": attr.label(
            default     = "//main/idlc:idlc",
            executable  = True,
            cfg         = "exec",
            allow_files = True,
        ),
        "_ucpp": attr.label(
            default     = "@ucpp//:ucpp",
            executable  = True,
            cfg         = "exec",
            allow_files = True,
        ),
        "_regmerge": attr.label(
            default     = "//main/registry:regmerge",
            executable  = True,
            cfg         = "exec",
            allow_files = True,
        ),
        "_cppumaker": attr.label(
            default     = "//main/codemaker:cppumaker",
            executable  = True,
            cfg         = "exec",
            allow_files = True,
        ),
        "_sal_dll": attr.label(
            default           = "//main/sal:sal",
            allow_single_file = True,
            cfg               = "exec",
        ),
        "_salhelper_dll": attr.label(
            default           = "//main/salhelper:salhelper3MSC",
            allow_single_file = True,
            cfg               = "exec",
        ),
        "_store_dll": attr.label(
            default           = "//main/store:store",
            allow_single_file = True,
            cfg               = "exec",
        ),
        "_reg_dll": attr.label(
            default           = "//main/registry:reg",
            allow_single_file = True,
            cfg               = "exec",
        ),
        "_crt_dlls": attr.label(
            default     = "//main/external/msvcp90:crt_dlls",
            allow_files = True,
            cfg         = "exec",
        ),
    },
)
