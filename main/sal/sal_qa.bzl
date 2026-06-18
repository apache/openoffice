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

load("//build/rules:gtest_test.bzl", "gtest_test")

_DEFINES = [
    "WNT",
    "WIN32",
    "INTEL",
    "_X86_=1",
    "snprintf=_snprintf",
]

def sal_qa_test(name, srcs, subdir, copts = [], deps = [], linkopts = [], companions = [], **kwargs):
    gtest_test(
        name = name,
        srcs = srcs,
        companions = companions,
        copts = [
            "/Imain/sal/inc/pch",
            "/Imain/sal/qa/inc",
            "/Imain/sal/qa/" + subdir,
            "/Imain/soltools/winunistd",
            "/Zc:wchar_t-",
        ] + copts,
        defines = _DEFINES,
        deps = [
            "//main/sal:sal_headers",
            "//main/stlport:stlport",
            "@boost.legacy//:boost.legacy",
        ] + deps,
        additional_linker_inputs = ["//main/sal:sal_implib"],
        linkopts = ["$(execpath //main/sal:sal_implib)"] + linkopts,
        runtime_dlls = ["//main/sal:sal3"],
        **kwargs
    )
