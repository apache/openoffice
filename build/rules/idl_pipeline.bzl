"""
IDL pipeline rules for Apache OpenOffice Bazel build.

Pipeline:  .idl --(idlc)--> .urd --(regmerge)--> .rdb --(cppumaker)--> .hpp/.hdl

All tools are MSVC-compiled Windows PEs.  ctx.actions.run() invokes them
directly — no bash/cmd wrapper required.

All executables and their runtime DLLs are staged into a single flat directory
using ctx.actions.symlink (pure Bazel, no shell).  PATH is set to that
directory so the Windows DLL loader finds everything in one place.

sal.if.lib records "LIBRARY sal3", so at runtime Windows looks for sal3.dll.
We create a symlink named sal3.dll alongside sal.dll in the staging directory.

idlc finds ucpp.exe by replacing "idlc" in its own executable path with
"ucpp.exe", so ucpp must be staged alongside idlc.

The VS2008 CRT DLLs (msvcr90.dll, msvcp90.dll, msvcm90.dll) and their
manifest are checked in under //main/external/msvcp90 and staged alongside
the executables so the CRT assembly is found as a local deployment.

IDL compilation strategy — one action per file, mirrored subdirs:
  idlc is called once per .idl file (matching the original dmake/gbuild
  build which used one make rule per .urd target).  Each invocation gets a
  fresh idlc instance with XInterface predefined, which is why files like
  XLinkAuthorizer.idl can reference ::com::sun::star::uno::XInterface
  without an explicit #include — it is always predefined by idlc.

  To avoid basename collisions across directories (chart/ChartDocument.idl
  vs chart2/ChartDocument.idl both producing ChartDocument.urd), we mirror
  the source sub-directory path under the output directory name:
      offapi_idl_urd/com/sun/star/chart/ChartDocument.urd
      offapi_idl_urd/com/sun/star/chart2/ChartDocument.urd

  Files that reference sibling types without #include AND without the
  predefined XInterface workaround (e.g. service Scaling in PowerScaling.idl)
  must be excluded from srcs to match the original UnoApi_offapi.mk list.
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

    dll_files = [getattr(ctx.file, attr) for attr, _ in _DLL_RENAME]
    crt_files = ctx.files._crt_dlls
    app_manifest = ctx.file._app_manifest

    # Build rename map: source File -> runtime filename
    rename = {}
    for attr, runtime in _DLL_RENAME:
        rename[getattr(ctx.file, attr)] = runtime

    # ── Stage all tools + DLLs into one flat directory ───────────────────
    # ctx.actions.symlink is a pure Bazel operation (hardlink on Windows) —
    # no shell, bash, or cmd.exe involved.
    all_to_stage = [idlc, ucpp, regmerge, cppumaker] + dll_files + crt_files
    staged = {}
    for f in all_to_stage:
        dst_name = rename.get(f, f.basename)
        out = ctx.actions.declare_file(ctx.label.name + "_tools/" + dst_name)
        ctx.actions.symlink(output = out, target_file = f)
        staged[dst_name] = out

    # Stage an external application manifest for each EXE so Windows finds
    # the VC90 CRT assembly without needing mt.exe to embed it.
    for exe in ["idlc.exe", "ucpp.exe", "regmerge.exe", "cppumaker.exe"]:
        out = ctx.actions.declare_file(ctx.label.name + "_tools/" + exe + ".manifest")
        ctx.actions.symlink(output = out, target_file = app_manifest)
        staged[exe + ".manifest"] = out

    tools_dir    = staged["idlc.exe"].dirname
    all_staged   = list(staged.values())

    # ── Compile each .idl → .urd (one action per file) ───────────────────
    # idlc writes <basename>.urd into the -O directory (strips the path,
    # keeping only the basename per idlcmain.cxx).
    #
    # We mirror the source sub-directory path under the output directory so
    # basenames from different directories never collide:
    #   chart/ChartDocument.idl  → urd/com/sun/star/chart/ChartDocument.urd
    #   chart2/ChartDocument.idl → urd/com/sun/star/chart2/ChartDocument.urd
    #
    # Each file gets a fresh idlc invocation, so idlc's predefined types
    # (including ::com::sun::star::uno::XInterface) are always in scope,
    # matching the original build's per-file compilation model.
    urd_dir_name = ctx.label.name + "_urd"
    urd_files = []

    package_prefix = ctx.label.package + "/"
    idl_include_flags = ["-I" + ctx.label.package] + ["-I" + d for d in ctx.attr.include_dirs]

    for idl in ctx.files.srcs:
        # Compute path relative to package root
        # e.g. "com/sun/star/chart/ChartDocument.idl"
        if idl.short_path.startswith(package_prefix):
            rel_path = idl.short_path[len(package_prefix):]
        else:
            rel_path = idl.basename

        slash_idx = rel_path.rfind("/")
        if slash_idx >= 0:
            rel_dir = rel_path[:slash_idx]   # e.g. "com/sun/star/chart"
            urd = ctx.actions.declare_file(urd_dir_name + "/" + rel_dir + "/" + idl.basename[:-4] + ".urd")
        else:
            urd = ctx.actions.declare_file(urd_dir_name + "/" + idl.basename[:-4] + ".urd")
        urd_files.append(urd)

        ctx.actions.run(
            executable = staged["idlc.exe"].path,
            arguments  = idl_include_flags + [
                "-O" + urd.dirname,   # output dir; idlc writes basename.urd here
                "-C",                 # keep comments
                idl.path,
            ],
            inputs           = [idl] + all_staged,
            outputs          = [urd],
            env              = {"PATH": tools_dir},
            mnemonic         = "Idlc",
            progress_message = "Compiling %s" % idl.short_path,
            use_default_shell_env = False,
        )

    # ── Merge all .urd → single .rdb ─────────────────────────────────────
    # Windows CreateProcessW limit is 32767 chars; 400+ .urd paths overflow it.
    # Split into batches of 50 URDs → intermediate .rdb files, then merge those.
    _BATCH = 50
    batches = []
    for i in range(0, len(urd_files), _BATCH):
        chunk = urd_files[i:i + _BATCH]
        batch_rdb = ctx.actions.declare_file(
            "%s_urd_batch_%d.rdb" % (ctx.label.name, i // _BATCH),
        )
        ctx.actions.run(
            executable = staged["regmerge.exe"].path,
            arguments  = [batch_rdb.path, "UCR"] + [u.path for u in chunk],
            inputs     = chunk + all_staged,
            outputs    = [batch_rdb],
            env        = {"PATH": tools_dir},
            mnemonic         = "RegMergeBatch",
            progress_message = "Merging URD batch %d for %s" % (i // _BATCH, ctx.label.name),
            use_default_shell_env = False,
        )
        batches.append(batch_rdb)

    rdb = ctx.actions.declare_file(ctx.label.name + ".rdb")
    ctx.actions.run(
        executable = staged["regmerge.exe"].path,
        # Batch RDBs already have content under /UCR/...; merge at root "/"
        # so the tree lands at /UCR/... in the final RDB, not /UCR/UCR/...
        arguments  = [rdb.path, "/"] + [b.path for b in batches],
        inputs     = batches + all_staged,
        outputs    = [rdb],
        env        = {"PATH": tools_dir},
        mnemonic         = "RegMerge",
        progress_message = "Merging %s.rdb" % ctx.label.name,
        use_default_shell_env = False,
    )

    # ── Generate C++ headers from .rdb ───────────────────────────────────
    # Use declare_directory so Bazel doesn't enforce a fixed file list —
    # cppumaker only emits headers for interfaces/structs/exceptions/enums,
    # not for services or modules, so we can't pre-declare every output.
    #
    # The "./" prefix on -O is critical: osl's convertToFileUrl checks
    # fileName.indexOf('.') == 0 to decide whether to use getAbsoluteFileURL
    # (relative-to-workdir, works) vs getFileURLFromSystemPath (fails for
    # relative paths on Windows).  Without "./" cppumaker can't create files.
    hdr_dir = ctx.actions.declare_directory(ctx.label.name + "_inc")
    # extra_rdbs labels may resolve to both .rdb and _inc dir — filter to .rdb only
    extra_rdbs = [f for f in ctx.files.extra_rdbs if f.extension == "rdb"]

    ctx.actions.run(
        executable = staged["cppumaker.exe"].path,
        arguments  = [
            "-Gc",             # generate include guards
            "-L",              # local (relative) includes
            "-BUCR",           # browse UCR section
            "-O./" + hdr_dir.path,
            rdb.path,
        ] + [r.path for r in extra_rdbs],
        inputs           = [rdb] + extra_rdbs + all_staged,
        outputs          = [hdr_dir],
        env              = {"PATH": tools_dir},
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
        "include_dirs": attr.string_list(
            default = [],
            doc = "Extra -I paths passed to idlc (relative to execroot)",
        ),
        "extra_rdbs": attr.label_list(
            allow_files = [".rdb"],
            default = [],
            doc = "Extra .rdb files passed to cppumaker for type resolution",
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
        "_app_manifest": attr.label(
            default           = "//main/external/msvcp90:vc90_app_manifest",
            allow_single_file = True,
            cfg               = "exec",
        ),
    },
)
