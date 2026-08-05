"""sal_qa_test — thin wrapper over gtest_test for the sal/qa unit tests.

Every sal/qa GoogleTest app is a standalone exe (own main()) that links sal
only.  They differ just in their source(s) and a per-dir include for a local
*_Const.h.  This macro bakes in the common bits:
  * /Imain/sal/inc/pch   — precompiled_sal.hxx (empty PCH, force-included)
  * /Imain/sal/qa/inc     — stringhelper.hxx / valueequal.hxx (header-only)
  * /Imain/sal/qa/<subdir> — the test's local <name>_Const.h
  * /Imain/soltools/winunistd — <unistd.h> shim (harmless where unused)
  * sal_headers + stlport + boost; sal_implib link; sal3.dll at run time.
"""

load("@rules_cc//cc:defs.bzl", "cc_binary")
load("//build/rules:gtest_test.bzl", "gtest_test")

_DEFINES = [
    "WNT",
    "WIN32",
    "INTEL",
    "_X86_=1",
    "snprintf=_snprintf",
]

_COMMON_COPTS = [
    "/Imain/sal/inc/pch",
    "/Imain/sal/qa/inc",
    "/Imain/soltools/winunistd",
    "/Zc:wchar_t-",
]

_COMMON_DEPS = [
    "//main/sal:sal_headers",
    "//main/stlport:stlport",
    "@boost.legacy//:boost.legacy",
]

def sal_qa_test(name, srcs, subdir, copts = [], deps = [], linkopts = [], companions = [], **kwargs):
    gtest_test(
        name = name,
        srcs = srcs,
        companions = companions,
        copts = _COMMON_COPTS + ["/Imain/sal/qa/" + subdir] + copts,
        defines = _DEFINES,
        deps = _COMMON_DEPS + deps,
        additional_linker_inputs = ["//main/sal:sal_implib"],
        linkopts = ["$(execpath //main/sal:sal_implib)"] + linkopts,
        runtime_dlls = ["//main/sal:sal3"],
        **kwargs
    )

def sal_qa_helper_exe(name, srcs, subdir, copts = [], deps = [], linkopts = []):
    """A child process a sal/qa suite spawns by name (osl_process_child, …).

    Same compile environment as sal_qa_test, minus gtest: these are not test
    harnesses but plain exes that the parent launches and then inspects the exit
    code of, the arguments echoed by, or a dumped environment file written by.
    Pass them to sal_qa_test's `companions` so they are staged beside the exe.
    """
    cc_binary(
        name = name,
        srcs = srcs,
        copts = _COMMON_COPTS + ["/Imain/sal/qa/" + subdir] + copts,
        defines = _DEFINES,
        deps = _COMMON_DEPS + deps,
        additional_linker_inputs = ["//main/sal:sal_implib"],
        # /MANIFEST:NO — the staging rule supplies an external <exe>.manifest
        # for the VC90 CRT, exactly as it does for the test exe itself.
        linkopts = ["$(execpath //main/sal:sal_implib)", "/MANIFEST:NO"] + linkopts,
        testonly = True,
    )
