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

    return [DefaultInfo(
        executable = staged_exe,
        runfiles = ctx.runfiles(files = staged),
        files = depset([staged_exe]),
    )]

_staged_gtest_test = rule(
    implementation = _staged_gtest_test_impl,
    test = True,
    attrs = {
        "binary": attr.label(executable = True, cfg = "target", mandatory = True),
        "runtime": attr.label_list(allow_files = True),
        "companions": attr.label_list(cfg = "target"),
        "app_manifest": attr.label(allow_single_file = True, default = _APP_MANIFEST),
    },
)

def gtest_test(
        name,
        srcs,
        deps = [],
        copts = [],
        defines = [],
        runtime_dlls = [],
        companions = [],
        additional_linker_inputs = [],
        linkopts = [],
        size = "small",
        **kwargs):
    """A GoogleTest suite that actually runs under `bazel test` on Windows/MD."""
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
        additional_linker_inputs = additional_linker_inputs,
        linkopts = linkopts + ["/MANIFEST:NO"],
        testonly = True,
        **kwargs
    )
    _staged_gtest_test(
        name = name,
        binary = ":" + name + "_bin",
        runtime = runtime_dlls + [_CRT],
        companions = companions,
        size = size,
    )
