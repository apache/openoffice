"""
localedata codegen rule for the i18npool locale-data libraries.

Pipeline (mirrors i18npool/source/localedata/data/makefile.mk):

    <lang>_<CC>.xml --(saxparser + saxparser.rdb + types.rdb)--> localedata_<lang>_<CC>.cxx

The generated .cxx files are then compiled and linked into the four
localedata_{en,es,euro,others}.dll data libraries (see main/i18npool/BUILD.bazel).
At runtime LocaleDataImpl::getLocaleItem() osl_loadModule()s these DLLs by bare
name (localedata_en.dll, …) and looks up getXxx_<lang>_<CC> functions; without
them every locale lookup throws and Writer/Calc text init fails (the Start Center
never touches locale data, which is why it stays usable).

saxparser is a UNO-bootstrapping tool: it loads the SAX parser component
(expwrap.dll) via saxparser.rdb.  Everything it touches at codegen time —
saxparser.exe itself and the DLLs it/expwrap depend on (sal3, cppu3,
cppuhelper3MSC, salhelper3MSC, reg, store, expwrap) — is staged into one flat
tools dir (ctx.actions.symlink, no shell) and put on PATH, exactly like the IDL
pipeline.  All of these are pulled in cfg="exec" so the tool set is a single
self-consistent release build (the final localedata DLLs are built in the target
config); only sal-allocator UNO types cross the DLL boundary, so there is no CRT
mismatch with the target dbg DLLs.

The actual invocation is delegated to build/tools/run_saxparser.pl (strawberry
perl, no Cygwin) because saxparser needs ABSOLUTE file:/// URLs for the two rdbs
and for OOO_INBUILD_SHAREDLIB_DIR, which only the perl Cwd helper can build
hermetically inside the action.
"""

# DLLs staged next to saxparser.exe.  (attr name, runtime filename).
# Only sal differs from its Bazel output name (LIBRARY sal3 → sal3.dll); the
# rest are identity renames kept explicit for clarity.
_DLL_RENAME = [
    ("_sal_dll",        "sal3.dll"),
    ("_cppu_dll",       "cppu3.dll"),
    ("_cppuhelper_dll", "cppuhelper3MSC.dll"),
    ("_salhelper_dll",  "salhelper3MSC.dll"),
    ("_store_dll",      "store.dll"),
    ("_reg_dll",        "reg.dll"),
    ("_expwrap_dll",    "expwrap.dll"),
    # createRegistryServiceFactory's bootstrap loads the SharedLibrary loader /
    # service manager / simple registry from bootstrap.uno.dll (resolved next to
    # saxparser.exe), so it must be staged in the tools dir too.
    ("_bootstrap_uno",  "bootstrap.uno.dll"),
    # bootstrap.uno's simpleregistry/textualservices parses the XML components
    # rdb via xmlreader.dll.
    ("_xmlreader_dll",  "xmlreader.dll"),
]

# Binary UNO bridge (C++ <=> UNO) — loaded by name (msci_uno.dll on x86,
# mscx_uno.dll on x64) to map interfaces across the saxparser <-> expwrap
# component boundary.  Staged under its real basename so the arch-selected DLL
# keeps its own name (the name saxparser's CPPU_ENV asks the loader for).
_BRIDGE_ATTR = "_cpp_uno_bridge"

def _localedata_gen_impl(ctx):
    saxparser    = ctx.executable._saxparser
    # services_rdb emits both the .rdb and its .input list; keep the .rdb only.
    rdb_candidates = [f for f in ctx.files.rdb if f.extension == "rdb"]
    if len(rdb_candidates) != 1:
        fail("rdb attribute must resolve to exactly one .rdb file, got {}".format(
            len(rdb_candidates)))
    rdb          = rdb_candidates[0]
    types_rdb    = ctx.file.types_rdb
    perl         = ctx.file._perl
    wrapper      = ctx.file._run_saxparser
    crt_files    = ctx.files._crt_dlls
    app_manifest = ctx.file._app_manifest

    rename = {}
    for attr, runtime in _DLL_RENAME:
        rename[getattr(ctx.file, attr)] = runtime
    dll_files = [getattr(ctx.file, attr) for attr, _ in _DLL_RENAME]
    # C++/UNO bridge: stage under its real basename (msci_uno.dll / mscx_uno.dll).
    bridge_dll = getattr(ctx.file, _BRIDGE_ATTR)
    dll_files.append(bridge_dll)

    # ── Stage saxparser.exe + DLLs + CRT into one flat tools dir ──────────────
    all_to_stage = [saxparser] + dll_files + crt_files
    staged = {}
    for f in all_to_stage:
        dst = rename.get(f, f.basename)
        out = ctx.actions.declare_file(ctx.label.name + "_tools/" + dst)
        ctx.actions.symlink(output = out, target_file = f)
        staged[dst] = out

    # External application manifest so Windows resolves the VC90 CRT assembly
    # for saxparser.exe without mt.exe embedding (same trick as idl_pipeline).
    sax_manifest = ctx.actions.declare_file(
        ctx.label.name + "_tools/saxparser.exe.manifest",
    )
    ctx.actions.symlink(output = sax_manifest, target_file = app_manifest)
    staged["saxparser.exe.manifest"] = sax_manifest

    sax_exe    = staged["saxparser.exe"]
    tools_dir  = sax_exe.dirname
    all_staged = staged.values()

    # ── One saxparser invocation per locale XML → generated .cxx ──────────────
    outs = []
    for xml in ctx.files.srcs:
        locale = xml.basename[:-4]   # "en_US.xml" → "en_US"
        out = ctx.actions.declare_file(
            ctx.label.name + "/localedata_" + locale + ".cxx",
        )
        outs.append(out)
        ctx.actions.run(
            executable = perl,
            arguments  = [
                wrapper.path,
                sax_exe.path,
                tools_dir,
                locale,
                xml.path,
                out.path,
                rdb.path,
                types_rdb.path,
            ],
            inputs  = [wrapper, xml, rdb, types_rdb] + all_staged,
            outputs = [out],
            mnemonic         = "LocaleData",
            progress_message = "Generating localedata_%s.cxx" % locale,
            use_default_shell_env = False,
        )

    return [DefaultInfo(files = depset(outs))]

localedata_gen = rule(
    implementation = _localedata_gen_impl,
    attrs = {
        "srcs": attr.label_list(
            allow_files = [".xml"],
            mandatory   = True,
            doc = "Locale <lang>_<CC>.xml files for this DLL group.",
        ),
        "rdb": attr.label(
            allow_files = True,
            mandatory = True,
            doc = "services_rdb target producing saxparser.rdb (the .rdb is selected).",
        ),
        "_saxparser": attr.label(
            default    = "//main/i18npool:saxparser",
            executable = True,
            cfg        = "exec",
            allow_files = True,
        ),
        "types_rdb": attr.label(
            allow_single_file = True,
            default = "//main/offapi:types_rdb",
            doc = "UNO types.rdb (offapi).",
        ),
        "_sal_dll":        attr.label(default = "//main/sal:sal",                  allow_single_file = True, cfg = "exec"),
        "_cppu_dll":       attr.label(default = "//main/cppu:cppu3",               allow_single_file = True, cfg = "exec"),
        "_cppuhelper_dll": attr.label(default = "//main/cppuhelper:cppuhelper3MSC", allow_single_file = True, cfg = "exec"),
        "_salhelper_dll":  attr.label(default = "//main/salhelper:salhelper3MSC",  allow_single_file = True, cfg = "exec"),
        "_store_dll":      attr.label(default = "//main/store:store",              allow_single_file = True, cfg = "exec"),
        "_reg_dll":        attr.label(default = "//main/registry:reg",             allow_single_file = True, cfg = "exec"),
        "_expwrap_dll":    attr.label(default = "//main/sax:expwrap",              allow_single_file = True, cfg = "exec"),
        "_bootstrap_uno":  attr.label(default = "//main/stoc:bootstrap.uno",       allow_single_file = True, cfg = "exec"),
        "_xmlreader_dll":  attr.label(default = "//main/xmlreader:xmlreader",       allow_single_file = True, cfg = "exec"),
        # C++/UNO bridge, cfg="exec" (built for the x64 host).  The alias selects
        # msci_uno.dll (x86) / mscx_uno.dll (x64) in the exec config — so on this
        # x64 host it resolves mscx_uno and never pulls the x86-only msci_uno into
        # the exec config.  Staged under its real basename (see _DLL_RENAME).
        "_cpp_uno_bridge": attr.label(default = "//main/bridges:cpp_uno_bridge",     allow_single_file = True, cfg = "exec"),
        "_crt_dlls": attr.label(
            default = "//main/external/msvcp90:msvcp90",
            allow_files = True,
            cfg = "exec",
        ),
        "_app_manifest": attr.label(
            default = "//main/external/msvcp90:vc90_app_manifest",
            allow_single_file = True,
            cfg = "exec",
        ),
        "_perl": attr.label(
            default = "@strawberry-perl//:perl_exe",
            allow_single_file = True,
            cfg = "exec",
        ),
        "_run_saxparser": attr.label(
            default = "//build/tools:run_saxparser.pl",
            allow_single_file = True,
        ),
    },
)
