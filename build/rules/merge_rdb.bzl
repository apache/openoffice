"""Starlark rule for merging UNO RDB files.

merge_rdb: takes one or more idl_library (or other RDB-producing) targets,
           filters their DefaultInfo files to .rdb, and merges them into a
           single output RDB via regmerge at the root key "/".

Usage:
    merge_rdb(
        name = "types_rdb",
        rdbs = ["//main/udkapi:udkapi_idl", "//main/offapi:offapi_idl"],
        out  = "types.rdb",          # optional; defaults to <name>.rdb
    )
"""

# DLL files that need to be staged alongside regmerge.exe.
_DLL_RENAME = [
    ("_sal_dll",       "sal3.dll"),
    ("_salhelper_dll", "salhelper3MSC.dll"),
    ("_store_dll",     "store.dll"),
    ("_reg_dll",       "reg.dll"),
]

def _merge_rdb_impl(ctx):
    out_name = ctx.attr.out if ctx.attr.out else ctx.label.name + ".rdb"
    rdb = ctx.actions.declare_file(out_name)

    # ── Stage regmerge + required DLLs ───────────────────────────────────────
    rename = {}
    for attr, runtime in _DLL_RENAME:
        rename[getattr(ctx.file, attr)] = runtime

    all_to_stage = [ctx.executable._regmerge] + [
        getattr(ctx.file, attr) for attr, _ in _DLL_RENAME
    ] + ctx.files._crt_dlls

    staged = {}
    for f in all_to_stage:
        dst_name = rename.get(f, f.basename)
        out = ctx.actions.declare_file(ctx.label.name + "_tools/" + dst_name)
        ctx.actions.symlink(output = out, target_file = f)
        staged[dst_name] = out

    # Stage external .manifest for regmerge.exe so Windows finds the VC90 CRT.
    manifest_out = ctx.actions.declare_file(
        ctx.label.name + "_tools/regmerge.exe.manifest"
    )
    ctx.actions.symlink(output = manifest_out, target_file = ctx.file._app_manifest)
    staged["regmerge.exe.manifest"] = manifest_out

    tools_dir  = staged["regmerge.exe"].dirname
    all_staged = list(staged.values())

    # ── Collect .rdb inputs from rdbs (filter out _inc directories) ──────────
    src_rdbs = [f for f in ctx.files.rdbs if f.extension == "rdb"]

    ctx.actions.run(
        executable = staged["regmerge.exe"].path,
        # Merge all source RDBs at the root key "/" so /UCR/... lands correctly.
        arguments  = [rdb.path, "/"] + [f.path for f in src_rdbs],
        inputs     = src_rdbs + all_staged,
        outputs    = [rdb],
        env        = {"PATH": tools_dir},
        mnemonic         = "MergeRdb",
        progress_message = "Merging {} RDB(s) → {}".format(len(src_rdbs), rdb.basename),
        use_default_shell_env = False,
    )

    return [DefaultInfo(files = depset([rdb]))]

merge_rdb = rule(
    implementation = _merge_rdb_impl,
    attrs = {
        "rdbs": attr.label_list(
            allow_files = True,
            doc         = "idl_library or other targets whose DefaultInfo contains .rdb files",
        ),
        "out": attr.string(
            default = "",
            doc     = "Output filename (default: <name>.rdb)",
        ),
        "_regmerge": attr.label(
            default     = "//main/registry:regmerge",
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
            default     = "//main/external/msvcp90:msvcp90",
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
