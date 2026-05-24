"""Starlark rules for the scp2 installer package pipeline.

All steps run via ctx.actions.run() — no bash, no genrule shell.

Pipeline:
  macro.pl    -> langmacros.inc        (langmacros_target macro, via Perl)
  cl.exe /P   -> *.pre   per .scp      (_scp_module_rule, direct cl.exe)
  pre2par.pl  -> osl/*.par per .pre    (_scp_module_rule, via Perl)
  par2script  -> setup/setup_osl.inf   (setup_osl_target macro, via Perl)

cl.exe is invoked directly (not through a Perl wrapper) using /P /EP /Fi
so that preprocessed output goes to a file without needing stdout capture.
Perl and cl.exe paths come from @vs_config//:paths.bzl.
"""

load("@vs_config//:paths.bzl", "PERL", "SDK", "VC", "VS")

# Absolute paths (forward slashes, constructed at load time from @vs_config).
_PERL = PERL.replace("\\", "/")
_CL   = VC.replace("\\", "/") + "/bin/cl.exe"

# INCLUDE semicolon-list for cl.exe (set as env var INCLUDE for the action).
_MSVC_INCLUDE = (
    VC.replace("\\", "/") + "/INCLUDE;" +
    VC.replace("\\", "/") + "/ATLMFC/INCLUDE;" +
    SDK.replace("\\", "/") + "/include"
)

# PATH for cl.exe actions — mirrors the toolchain's msvc_env_path so cl.exe
# finds mspdb90.dll (in Common7/IDE) and other VS companions at runtime.
_MSVC_ENV_PATH = (
    SDK.replace("\\", "/") + "/bin;" +
    VS.replace("\\", "/")  + "/Common7/IDE;" +
    VC.replace("\\", "/")  + "/bin;" +
    VS.replace("\\", "/")  + "/Common7/Tools"
)

# Preprocessor defines (without -D prefix; the rule adds -D per entry).
_SCPDEFS = [
    "_MSC",
    "WNT",
    "COMID=MSC",
    "COMNAME=MSC",
    "DLLPOSTFIX=",
    "OPENSSL",
    "ENABLE_XMLSEC",
    "BUNDLE_NSS_LIBS",
    "ICU_MAJOR=4",
    "ICU_MINOR=2",
    "ICU_MICRO=1",
    "ISOLANG_MAJOR=1",
    "LIBXSLT_MAJOR=1",
    "RAPTOR_MAJOR=0",
    "RASQAL_MAJOR=3",
    "REDLAND_MAJOR=0",
]

# ── langmacros ─────────────────────────────────────────────────────────────────

def _langmacros_impl(ctx):
    out = ctx.actions.declare_file("langmacros.inc")
    ctx.actions.run(
        executable = ctx.attr.perl_exe,
        arguments = [ctx.file.macro_pl.path, "-o", out.path],
        inputs = [ctx.file.macro_pl],
        outputs = [out],
        use_default_shell_env = True,
        execution_requirements = {"local": "1"},
        env = {"COMPLETELANGISO_VAR": "en-US"},
        mnemonic = "LangMacros",
        progress_message = "Generating langmacros.inc",
    )
    return DefaultInfo(files = depset([out]))

_langmacros_rule = rule(
    implementation = _langmacros_impl,
    attrs = {
        "macro_pl":  attr.label(allow_single_file = True, mandatory = True),
        "perl_exe":  attr.string(mandatory = True),
    },
)

def langmacros_target(name, macro_pl, visibility = None):
    _langmacros_rule(
        name = name,
        macro_pl = macro_pl,
        perl_exe = _PERL,
        visibility = visibility,
    )

# ── alllangmodules ─────────────────────────────────────────────────────────────
# Generates alllangmodules_*.inc files from module_langpack*.sct templates.
# Each .sct is expanded once per language in completelangiso to produce one
# Module block per language (used by scp files that #include these files).

def _alllangmodules_impl(ctx):
    outs = []
    for sct in ctx.files.sct_srcs:
        stem = sct.basename[:-4]  # strip .sct
        if stem == "module_langpack":
            out_name = "alllangmodules.inc"
        else:
            suffix = stem[len("module_langpack_"):]
            out_name = "alllangmodules_" + suffix + ".inc"

        out = ctx.actions.declare_file(out_name)
        ctx.actions.run(
            executable = ctx.attr.perl_exe,
            arguments = [
                ctx.file.modules_pl.path,
                "-i", sct.path,
                "-o", out.path,
            ],
            inputs = [ctx.file.modules_pl, sct],
            outputs = [out],
            use_default_shell_env = True,
            execution_requirements = {"local": "1"},
            env = {"COMPLETELANGISO_VAR": ctx.attr.completelangiso},
            mnemonic = "AllLangModules",
            progress_message = "Generating %s" % out_name,
        )
        outs.append(out)
    return DefaultInfo(files = depset(outs))

_alllangmodules_rule = rule(
    implementation = _alllangmodules_impl,
    attrs = {
        "sct_srcs":        attr.label_list(allow_files = [".sct"], mandatory = True),
        "modules_pl":      attr.label(allow_single_file = True, mandatory = True),
        "completelangiso": attr.string(default = "en-US"),
        "perl_exe":        attr.string(mandatory = True),
    },
)

def alllangmodules_target(name, sct_srcs, modules_pl, completelangiso = "en-US", visibility = None):
    _alllangmodules_rule(
        name = name,
        sct_srcs = sct_srcs,
        modules_pl = modules_pl,
        completelangiso = completelangiso,
        perl_exe = _PERL,
        visibility = visibility,
    )

# ── scp_module ─────────────────────────────────────────────────────────────────

def _scp_module_impl(ctx):
    langmacros_dir = ctx.file.langmacros_inc.dirname
    alllangmodules_files = ctx.attr.alllangmodules_incs[DefaultInfo].files.to_list()

    pre2par_pl = None
    for f in ctx.files.pre2par_srcs:
        if f.basename == "pre2par.pl":
            pre2par_pl = f
            break
    if pre2par_pl == None:
        fail("pre2par.pl not found in pre2par_srcs")

    defines = ctx.attr.defines + ctx.attr.extra_defines
    all_pars = []

    for scp in ctx.files.srcs:
        stem = scp.basename[:-4]  # strip .scp

        # ── Step 1: scp_preprocess.pl → cl.exe /EP /Tc → .pre ────────────────
        # The Perl wrapper invokes cl.exe by name (not full path) via cmd.exe,
        # sidestepping MSYS2 fork/exec failures for space-containing PE paths.
        # PATH = _MSVC_ENV_PATH ensures cmd.exe finds cl.exe in VC\bin and
        # that cl.exe's DLL companions (c1.dll, mspdb90.dll, etc.) are on PATH.
        pre = ctx.actions.declare_file(stem + ".pre")
        ctx.actions.run(
            executable = ctx.attr.perl_exe,
            arguments = (
                [
                    ctx.file.scp_preprocess.path,
                    "--cl",      ctx.attr.cl_exe,
                    "--include", ctx.attr.msvc_include,
                    "-I", "main/scp2/inc",
                    "-I", langmacros_dir,
                    "--out", pre.path,
                ] +
                [item for d in defines for item in ("-D", d)] +
                [scp.path]
            ),
            inputs = [
                scp,
                ctx.file.scp_preprocess,
                ctx.file.macros_inc,
                ctx.file.langmacros_inc,
            ] + alllangmodules_files,
            outputs = [pre],
            use_default_shell_env = True,
            execution_requirements = {"local": "1"},
            env = {"PATH": _MSVC_ENV_PATH},
            mnemonic = "ScpPreprocess",
            progress_message = "Preprocessing %s" % scp.basename,
        )

        # ── Step 2: pre2par.pl ─────────────────────────────────────────────
        par = ctx.actions.declare_file(ctx.attr.product_type + "/" + stem + ".par")
        ctx.actions.run(
            executable = ctx.attr.perl_exe,
            arguments = [
                pre2par_pl.path,
                "-s", pre.path,
                "-o", par.path,
            ],
            inputs = [pre] + ctx.files.pre2par_srcs,
            outputs = [par],
            use_default_shell_env = True,
            execution_requirements = {"local": "1"},
            env = {"SOLARENV": "main/solenv"},
            mnemonic = "ScpToPar",
            progress_message = "Converting %s to .par" % scp.basename,
        )

        all_pars.append(par)

    return DefaultInfo(files = depset(all_pars))

_scp_module_rule = rule(
    implementation = _scp_module_impl,
    attrs = {
        "srcs":                attr.label_list(allow_files = [".scp"], mandatory = True),
        "macros_inc":          attr.label(allow_single_file = True, mandatory = True),
        "langmacros_inc":      attr.label(allow_single_file = True, mandatory = True),
        "alllangmodules_incs": attr.label(mandatory = True),
        "pre2par_srcs":        attr.label(allow_files = True, mandatory = True),
        "scp_preprocess":      attr.label(allow_single_file = True, mandatory = True),
        "product_type":   attr.string(default = "osl"),
        "perl_exe":       attr.string(mandatory = True),
        "cl_exe":         attr.string(mandatory = True),
        "msvc_include":   attr.string(mandatory = True),
        "defines":        attr.string_list(default = []),
        "extra_defines":  attr.string_list(default = []),
    },
)

def scp_module(
        name,
        srcdir,
        extra_scpdefs = "",
        product_type = "osl",
        visibility = None):
    """Preprocess all .scp files in srcdir and convert them to .par format.

    No shell is used: cl.exe /P /EP is invoked directly by ctx.actions.run(),
    and pre2par.pl is run via ctx.actions.run(executable=perl_exe).

    Args:
        name:          Bazel target name.
        srcdir:        Package-relative directory containing .scp files.
        extra_scpdefs: Additional defines (space-separated, no -D prefix).
        product_type:  Output path prefix (default "osl").
        visibility:    Bazel visibility list.
    """
    scp_srcs = native.glob([srcdir + "/*.scp"])
    if not scp_srcs:
        return

    _scp_module_rule(
        name = name,
        srcs = scp_srcs,
        macros_inc = "//main/scp2:macros_inc",
        langmacros_inc = "//main/scp2:langmacros_inc",
        alllangmodules_incs = "//main/scp2:alllangmodules_incs",
        pre2par_srcs = "//main/solenv/bin:pre2par_srcs",
        scp_preprocess = "//main/solenv/bin:scp_preprocess_pl",
        product_type = product_type,
        perl_exe = _PERL,
        cl_exe = _CL,
        msvc_include = _MSVC_INCLUDE,
        defines = _SCPDEFS,
        extra_defines = extra_scpdefs.split() if extra_scpdefs else [],
        visibility = visibility,
    )

# ── setup_osl ──────────────────────────────────────────────────────────────────

def _setup_osl_impl(ctx):
    par2script_pl = None
    for f in ctx.files.par2script_srcs:
        if f.basename == "par2script.pl":
            par2script_pl = f
            break
    if par2script_pl == None:
        fail("par2script.pl not found in par2script_srcs")

    # Collect every .par file produced by the scp_module deps.
    all_pars = []
    for dep in ctx.attr.scp_modules:
        all_pars.extend(dep[DefaultInfo].files.to_list())
    if not all_pars:
        fail("No .par files found in scp_modules")

    # All .par files land in the same osl/ output directory.
    par_dir = all_pars[0].dirname

    # Write only the requested basenames to the listfile.
    # Split "@@" across two literals to avoid the Bazel linter treating it
    # as a canonical repository reference.
    par_list = ctx.actions.declare_file("parlist.tmp")
    ctx.actions.write(par_list, ",".join(ctx.attr.par_names))

    inf_out = ctx.actions.declare_file("setup/setup_osl.inf")

    ctx.actions.run(
        executable = ctx.attr.perl_exe,
        arguments = [
            par2script_pl.path,
            "-i", par_dir,
            "@" + "@" + par_list.path,
            "-o", inf_out.path,
        ],
        inputs = all_pars + ctx.files.par2script_srcs + [par_list],
        outputs = [inf_out],
        use_default_shell_env = True,
        execution_requirements = {"local": "1"},
        env = {"SOLARENV": "main/solenv"},
        mnemonic = "Par2Script",
        progress_message = "Generating setup_osl.inf",
    )

    return DefaultInfo(files = depset([inf_out]))

_setup_osl_rule = rule(
    implementation = _setup_osl_impl,
    attrs = {
        "scp_modules":     attr.label_list(mandatory = True),
        "par_names":       attr.string_list(mandatory = True),
        "par2script_srcs": attr.label(allow_files = True, mandatory = True),
        "perl_exe":        attr.string(mandatory = True),
    },
)

def setup_osl_target(name, scp_modules, par_names, par2script_srcs, visibility = None):
    _setup_osl_rule(
        name = name,
        scp_modules = scp_modules,
        par_names = par_names,
        par2script_srcs = par2script_srcs,
        perl_exe = _PERL,
        visibility = visibility,
    )
