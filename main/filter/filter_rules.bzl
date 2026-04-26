"""Shared build rules and constants for the filter module."""

load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

# Common compiler options for all filter DLLs
FILTER_COPTS = [
    "/Imain/filter/inc",
    "/Imain/filter/inc/pch",
    "/Zc:wchar_t-",
]

# Common header deps for all filter DLLs
FILTER_HDR_DEPS = [
    "//main/sal:sal_headers",
    "//main/cppu:cppu_headers",
    "//main/cppuhelper:cppuhelper_headers",
    "//main/udkapi:udkapi_idl_headers",
    "//main/offapi:offapi_idl_headers",
    "//main/vcl:vcl_headers",
    "//main/tools:tools_headers",
    "//main/svl:svl_headers",
    "//main/svtools:svtools_headers",
    "//main/unotools:unotools_headers",
    "//main/comphelper:comphelper_headers",
    "//main/basegfx:basegfx_headers",
    "//main/toolkit:toolkit_headers",
    "//main/sfx2:sfx2_headers",
    "//main/xmloff:xmloff_headers",
    "//main/editeng:editeng_headers",
    "//main/svx:svx_headers",
    "//main/framework:framework_headers",
    "//main/ucbhelper:ucbhelper_headers",
    "//main/sot:sot_headers",
    "//main/vos:vos_headers",
    "//main/stlport:stlport",
    "@boost.legacy//:boost.legacy",
]

def graphic_filter(
        name,
        subdir,
        srcs,
        win_def,
        local_hrc = [],
        extra_copts = [],
        extra_deps = [],
        extra_implins = [],
        extra_linkopts = []):
    """Builds a graphic import/export filter DLL.

    Each graphic filter is a small linkshared DLL exporting a single entry
    point (GraphicImport, GraphicExport, or ImportCGM).

    Args:
        name:           Bazel target name — also the DLL stem (e.g. "egi").
        subdir:         Subdirectory under source/graphicfilter/ (e.g. "egif").
        srcs:           List of .cxx/.hxx filenames relative to that subdir.
        win_def:        DEF file label relative to //main/filter.
        local_hrc:      .hrc files in the same subdir included by the .cxx
                        sources.  Cannot go in srcs; wrapped in textual_hdrs.
        extra_copts:    Additional /I flags (e.g. icgm needs its own dir).
        extra_deps:     Additional header deps beyond FILTER_HDR_DEPS.
        extra_implins:  Additional import libraries to link.
        extra_linkopts: Additional linker flags ($(execpath ...) entries).
    """
    hrc_deps = extra_deps
    if local_hrc:
        hrc_lib = name + "_hrc"
        cc_library(
            name = hrc_lib,
            textual_hdrs = [
                "source/graphicfilter/" + subdir + "/" + h
                for h in local_hrc
            ],
        )
        hrc_deps = extra_deps + [":" + hrc_lib]

    cc_binary(
        name = name,
        srcs = ["source/graphicfilter/" + subdir + "/" + s for s in srcs],
        copts = FILTER_COPTS + extra_copts,
        defines = [
            "WNT", "GUI", "WIN32",
            "WINVER=0x0500", "_WIN32_WINNT=0x0500", "_WIN32_IE=0x0500",
            "CPPU_ENV=msci", "INTEL", "_X86_=1",
            "FULL_DESK", "SHAREDLIB", "_DLL_",
            "snprintf=_snprintf",
            "snwprintf=_snwprintf",
            "SUPD=680",
        ],
        win_def_file = win_def,
        linkshared = True,
        deps = FILTER_HDR_DEPS + hrc_deps,
        additional_linker_inputs = [
            "//main/sal:sal_implib",
            "//main/cppu:cppu3_implib",
            "//main/vcl:vcl_implib",
            "//main/tools:tl_implib",
            "//main/svtools:svt_implib",
            "//main/basegfx:basegfx_implib",
        ] + extra_implins,
        linkopts = [
            "$(execpath //main/sal:sal_implib)",
            "$(execpath //main/cppu:cppu3_implib)",
            "$(execpath //main/vcl:vcl_implib)",
            "$(execpath //main/tools:tl_implib)",
            "$(execpath //main/svtools:svt_implib)",
            "$(execpath //main/basegfx:basegfx_implib)",
            "gdi32.lib", "user32.lib",
            "/MANIFEST:NO",
        ] + extra_linkopts,
        visibility = ["//visibility:public"],
    )
    native.filegroup(
        name = name + "_implib",
        srcs = [":" + name],
        output_group = "interface_library",
        visibility = ["//visibility:public"],
    )
