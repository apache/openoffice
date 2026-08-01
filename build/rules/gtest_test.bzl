###############################################################################
# gtest_test — runnable GoogleTest target for the /MD MSVC toolchain.
#
# Problem: the custom VS2008 toolchain links /MD and embeds NO manifest, so a
# bare cc_test .exe cannot launch under `bazel test`:
#   * its runtime DLLs (sal3.dll, …) and the VC90 CRT (msvcr90.dll) are not
#     beside the exe — `data` deps land in runfiles SUBDIRS, which the Windows
#     loader does not search;
#   * loose msvcr90.dll with no activation context raises R6034.
#
# Solution (the test analog of //main/idl:svidl_bundle): stage the exe + all
# runtime DLLs + the VC90 CRT assembly + an external "<exe>.manifest" into ONE
# flat directory via ctx.actions.symlink, and make THAT staged exe the test
# executable.  Co-located DLLs are found; the external manifest supplies the
# CRT activation context (no R6034).
#
# Usage:
#   load("//build/rules:gtest_test.bzl", "gtest_test")
#   gtest_test(
#       name = "o3tl_test",
#       srcs = glob(["qa/*.cxx", "qa/*.hxx"]),
#       copts = [...], defines = [...],
#       deps = [":o3tl", "//main/sal:sal_headers", "//main/stlport:stlport"],
#       additional_linker_inputs = ["//main/sal:sal_implib"],
#       linkopts = ["$(execpath //main/sal:sal_implib)"],
#       runtime_dlls = ["//main/sal:sal3"],   # DLLs the exe loads at run time
#   )
###############################################################################

"""Runnable GoogleTest targets for the /MD MSVC toolchain (see header above)."""

load("@rules_cc//cc:defs.bzl", "cc_binary")

_CRT = "//main/external/msvcp90:crt_dlls"
_APP_MANIFEST = "//main/external/msvcp90:vc90_app_manifest"

# VC90-CRT manifest compiled to a .res, linked into every test exe so it sits in
# the image at RT_MANIFEST id 1.  The staged external <exe>.manifest below only
# covers DLLs bound at process start; a test that osl_loadModule()s a UNO DLL
# later (any cppu Mapping pulls in the C++/UNO bridge on demand) needs the CRT
# activation context to still apply at that point, or the late load resolves
# msvcr90 loose → R6034 → DllMain fails → exit 0xC0000142.
_APP_MANIFEST_RES = "//main/external/msvcp90:vc90_app_manifest_res"

def _staged_gtest_test_impl(ctx):
    d = ctx.label.name + ".run"
    staged = []

    # The gtest exe, renamed to the test target's name.
    staged_exe = ctx.actions.declare_file(d + "/" + ctx.label.name + ".exe")
    ctx.actions.symlink(output = staged_exe, target_file = ctx.executable.binary)
    staged.append(staged_exe)

    # Runtime DLLs + VC90 CRT (msvcr90/msvcp90/msvcm90 + Microsoft.VC90.CRT.manifest).
    for f in ctx.files.runtime:
        o = ctx.actions.declare_file(d + "/" + f.basename)
        ctx.actions.symlink(output = o, target_file = f)
        staged.append(o)

    # Companion helper exes (e.g. a child process the test spawns by name).
    # Staged under their own basename + a matching <name>.exe.manifest so they
    # too satisfy the VC90 CRT activation context when launched.
    for c in ctx.attr.companions:
        cexe = c[DefaultInfo].files_to_run.executable
        co = ctx.actions.declare_file(d + "/" + cexe.basename)
        ctx.actions.symlink(output = co, target_file = cexe)
        staged.append(co)
        cman = ctx.actions.declare_file(d + "/" + cexe.basename + ".manifest")
        ctx.actions.symlink(output = cman, target_file = ctx.file.app_manifest)
        staged.append(cman)

    # External application manifest, named <exe>.manifest, declaring the VC90
    # CRT dependency so the loader builds an activation context (no R6034).
    man = ctx.actions.declare_file(d + "/" + ctx.label.name + ".exe.manifest")
    ctx.actions.symlink(output = man, target_file = ctx.file.app_manifest)
    staged.append(man)

    # Co-locating a data file with the exe is not enough for a test that opens it
    # by bare relative name: `bazel test` runs the executable with the working
    # directory set to the execroot, not to the exe's directory (the loader finds
    # the staged DLLs via the exe's own path, which is why those work regardless).
    # When run_in_staged_dir is set, hand Bazel a .bat that cd's into the staged
    # dir first and forwards the exit code, so relative paths resolve there.
    executable = staged_exe
    if ctx.attr.run_in_staged_dir:
        launcher = ctx.actions.declare_file(d + "/" + ctx.label.name + "_run.bat")
        ctx.actions.write(
            output = launcher,
            content = "\r\n".join([
                "@echo off",
                'cd /d "%~dp0" || exit /b 1',
                '"%~dp0' + staged_exe.basename + '" %*',
                "exit /b %ERRORLEVEL%",
                "",
            ]),
            is_executable = True,
        )
        staged.append(launcher)
        executable = launcher

    return [DefaultInfo(
        executable = executable,
        runfiles = ctx.runfiles(files = staged),
        files = depset([executable]),
    )]

_staged_gtest_test = rule(
    implementation = _staged_gtest_test_impl,
    test = True,
    attrs = {
        "binary": attr.label(executable = True, cfg = "target", mandatory = True),
        "runtime": attr.label_list(allow_files = True),
        "companions": attr.label_list(cfg = "target"),
        "app_manifest": attr.label(allow_single_file = True, default = _APP_MANIFEST),
        "run_in_staged_dir": attr.bool(default = False),
    },
)

# The staging rule above is NOT gtest-specific: it stages an arbitrary /MD exe +
# runtime DLLs + VC90 CRT + external manifest into one flat dir and runs it as a
# test (pass/fail = process exit code).  Exposed for non-gtest runnable tests,
# e.g. bridges' inter_libs_exc (a cross-DLL C++ exception smoke test whose exe
# loads two sibling DLLs and exits 0 iff exception propagation works).
staged_run_test = _staged_gtest_test

def gtest_test(
        name,
        srcs,
        deps = [],
        copts = [],
        defines = [],
        runtime_dlls = [],
        data_files = [],
        companions = [],
        additional_linker_inputs = [],
        linkopts = [],
        size = "small",
        **kwargs):
    """A GoogleTest suite that actually runs under `bazel test` on Windows/MD.

    data_files: inputs the test opens by relative path (fixture documents, …).
    They are staged beside the exe AND the test is launched with its working
    directory set to that staged dir, which co-location alone does not give you
    (see run_in_staged_dir in the staging rule).
    """
    cc_binary(
        name = name + "_bin",
        srcs = srcs,
        deps = deps + ["@gtest//:gtest"],
        # /Zc:wchar_t- MUST match @gtest's own build (see its overlay BUILD):
        # gtest.lib is compiled with wchar_t == unsigned short, so every test TU
        # linking it must agree, or PrintTo(wchar_t)/PrintTo(unsigned short)
        # mangle differently → LNK2019.
        copts = ["/Zc:wchar_t-"] + copts,
        defines = defines,
        # /MANIFEST:NO keeps the LINKER from generating one; the .res supplies
        # the manifest as a resource instead (see _APP_MANIFEST_RES).
        additional_linker_inputs = additional_linker_inputs + [_APP_MANIFEST_RES],
        linkopts = linkopts + [
            "$(execpath %s)" % _APP_MANIFEST_RES,
            "/MANIFEST:NO",
        ],
        testonly = True,
        **kwargs
    )
    _staged_gtest_test(
        name = name,
        binary = ":" + name + "_bin",
        runtime = runtime_dlls + data_files + [_CRT],
        companions = companions,
        run_in_staged_dir = bool(data_files),
        size = size,
    )
