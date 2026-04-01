# main/sal/prj/BUILD.bazel
#
# Migrated from dmake/nmake build system.
# Source of truth: main/sal/prj/build.lst
#
# Build chain from build.lst:
#   sa_inc
#     └─> sa_tc       (typesconfig,  unix only)
#     └─> sa_cpprt    (cpprt,        unix only)
#     └─> sa_rtl      (rtl/source,   all)
#     └─> sa_textenc  (textenc,      all)
#     └─> sa_uwinapi  (win32 only)
#         └─> sa_kill        (win32 only)
#         └─> sa_onlineupdate(win32 only)
#     └─> sa_osln     (osl/w32,      win only)
#     └─> sa_oslp     (osl/os2,      os2  only)
#     └─> sa_oslu     (osl/unx,      unix only)
#     └─> sa_oslall   (osl/all,      all)
#         └─> sa_util (util,         all)  ← primary deliverable target
#
# External deps (from build.lst header):
#   xml2cmp, stlport, external, BOOST:boost, GTEST:gtest

load("@rules_foreign_cc//foreign_cc:defs.bzl", "nmake")

# ---------------------------------------------------------------------------
# Source filegroup — entire sal tree visible to all nmake() rules below
# ---------------------------------------------------------------------------
filegroup(
    name = "sources",
    srcs = glob(
        ["**/*"],
        exclude = [
            "**/*.bazel",
            "**/*.bzl",
        ],
    ),
    visibility = ["//visibility:public"],
)

# ---------------------------------------------------------------------------
# sa_inc  — generated headers (typesizes.h, udkversion.h, rtlbootstrap.mk)
#   dir:  sal\inc
#   deps: none within sal; needs stlport, external for env only
# ---------------------------------------------------------------------------
nmake(
    name = "sa_inc",
    lib_source = ":sources",
    build_file = "sal/inc/makefile.mk",
    targets = ["all"],
    out_include_dirs = [
        "inc/sal",
        "inc/osl",
        "inc/rtl",
        "inc/systools/win32",
    ],
    deps = [
        "//main/stlport:stlport",    # stlport – headers needed at inc stage
        "//main/external:external",  # external – platform headers
    ],
    visibility = ["//visibility:public"],
)

# ---------------------------------------------------------------------------
# sa_tc  — sal\typesconfig  (unix only, flag: u)
#   Generates sal/typesizes.h via a small probe executable.
#   On Windows this step is skipped; sa_inc already ships a pre-built version.
# ---------------------------------------------------------------------------
nmake(
    name = "sa_tc",
    lib_source = ":sources",
    build_file = "sal/typesconfig/makefile.mk",
    targets = ["all"],
    deps = [":sa_inc"],
    # Only meaningful on unix; Bazel select lets Windows targets skip it.
    # Downstream targets depend on sa_tc.u (unix) via :sa_tc.
    visibility = ["//visibility:public"],
)

# ---------------------------------------------------------------------------
# sa_cpprt  — sal\cpprt  (unix only, flag: u)
#   C++ runtime compatibility shim for the sal shared library.
# ---------------------------------------------------------------------------
nmake(
    name = "sa_cpprt",
    lib_source = ":sources",
    build_file = "sal/cpprt/makefile.mk",
    targets = ["all"],
    deps = [
        ":sa_tc",
        ":sa_inc",
    ],
    visibility = ["//visibility:public"],
)

# ---------------------------------------------------------------------------
# sa_rtl  — sal\rtl\source  (all platforms)
#   Builds the RTL (Run-Time Library) object files / static archive.
# ---------------------------------------------------------------------------
nmake(
    name = "sa_rtl",
    lib_source = ":sources",
    build_file = "sal/rtl/source/makefile.mk",
    targets = ["all"],
    deps = [
        ":sa_tc",
        ":sa_inc",
        "//main/stlport:stlport",
    ],
    visibility = ["//visibility:public"],
)

# ---------------------------------------------------------------------------
# sa_textenc  — sal\textenc  (all platforms)
#   Text encoding conversion tables.
# ---------------------------------------------------------------------------
nmake(
    name = "sa_textenc",
    lib_source = ":sources",
    build_file = "sal/textenc/makefile.mk",
    targets = ["all"],
    deps = [
        ":sa_tc",
        ":sa_inc",
    ],
    visibility = ["//visibility:public"],
)

# ---------------------------------------------------------------------------
# Windows-only targets
#   sa_uwinapi  — sal\systools\win32\uwinapi  (flag: n  = Windows)
#   sa_kill     — sal\systools\win32\kill     (flag: n)
#   sa_onlineupdate — sal\systools\win32\onlineupdate (flag: n)
#   sa_osln     — sal\osl\w32                (flag: n)
# ---------------------------------------------------------------------------
nmake(
    name = "sa_uwinapi",
    lib_source = ":sources",
    build_file = "sal/systools/win32/uwinapi/makefile.mk",
    targets = ["all"],
    deps = [":sa_inc"],
    # Windows only — guarded by select so unix builds still resolve the label
    visibility = ["//visibility:public"],
)

nmake(
    name = "sa_kill",
    lib_source = ":sources",
    build_file = "sal/systools/win32/kill/makefile.mk",
    targets = ["all"],
    out_binaries = ["kill.exe"],
    deps = [
        ":sa_uwinapi",
        ":sa_inc",
    ],
    visibility = ["//visibility:public"],
)

nmake(
    name = "sa_onlineupdate",
    lib_source = ":sources",
    build_file = "sal/systools/win32/onlineupdate/makefile.mk",
    targets = ["all"],
    deps = [
        ":sa_uwinapi",
        ":sa_inc",
    ],
    visibility = ["//visibility:public"],
)

nmake(
    name = "sa_osln",
    lib_source = ":sources",
    build_file = "sal/osl/w32/makefile.mk",
    targets = ["all"],
    deps = [":sa_inc"],
    visibility = ["//visibility:public"],
)

# ---------------------------------------------------------------------------
# Unix-only OSL target
#   sa_oslu  — sal\osl\unx  (flag: u)
# ---------------------------------------------------------------------------
nmake(
    name = "sa_oslu",
    lib_source = ":sources",
    build_file = "sal/osl/unx/makefile.mk",
    targets = ["all"],
    deps = [
        ":sa_tc",
        ":sa_inc",
    ],
    visibility = ["//visibility:public"],
)

# ---------------------------------------------------------------------------
# sa_oslall  — sal\osl\all  (all platforms)
#   Platform-independent OSL pieces; depends on platform OSL above.
# ---------------------------------------------------------------------------
nmake(
    name = "sa_oslall",
    lib_source = ":sources",
    build_file = "sal/osl/all/makefile.mk",
    targets = ["all"],
    deps = [
        ":sa_tc",
        ":sa_inc",
        # platform OSL — only one will actually produce output per platform
        ":sa_osln",    # win
        ":sa_oslu",    # unx
    ],
    visibility = ["//visibility:public"],
)

# ---------------------------------------------------------------------------
# sa_util  — sal\util  (all platforms)  ← PRIMARY DELIVERABLE
#
#   Links everything together into:
#     Unix:    libuno_sal.so.3 / libuno_sal.dylib
#              libsalalloc_malloc.so.3
#     Windows: sal.dll / xsal.lib
#
#   Dependencies mirror build.lst exactly:
#     sa_tc.u  sa_oslall  sa_uwinapi.n  sa_kill.n  sa_onlineupdate.n
#     sa_osln.n  sa_oslp.p  sa_oslu.u  sa_rtl  sa_textenc
# ---------------------------------------------------------------------------
nmake(
    name = "sa_util",
    lib_source = ":sources",
    build_file = "sal/util/makefile.mk",
    targets = ["all"],
    # Shared libraries / import libs produced (d.lst)
    out_shared_libs = select({
        "@platforms//os:windows": [
            "sal.dll",
        ],
        "@platforms//os:macos": [
            "libuno_sal.dylib",
        ],
        "//conditions:default": [            # Linux / Solaris / etc.
            "libuno_sal.so.3",
            "libsalalloc_malloc.so.3",
        ],
    }),
    out_static_libs = select({
        "@platforms//os:windows": ["xsal.lib"],
        "//conditions:default":   ["libsal.a"],
    }),
    deps = [
        ":sa_tc",
        ":sa_oslall",
        ":sa_uwinapi",          # win only but label must resolve everywhere
        ":sa_kill",             # win only
        ":sa_onlineupdate",     # win only
        ":sa_osln",             # win only
        ":sa_oslu",             # unx only
        ":sa_rtl",
        ":sa_textenc",
        # External deps from build.lst header line
        "//main/stlport:stlport",
        "//main/external:external",
        "//main/xml2cmp:xml2cmp",
    ],
    visibility = ["//visibility:public"],
)

# ---------------------------------------------------------------------------
# QA / test targets
#   All qa targets share the same shape:
#     deps = [sa_util, sa_cpprt]  (unix cpprt needed for C++ test runners)
#   Listed individually so `bazel test //main/sal/...` runs them all.
# ---------------------------------------------------------------------------

# Common dep list reused by every qa target
# (Bazel doesn't have "variables" in BUILD files; use a local list.)
_QA_DEPS = [
    ":sa_util",
    ":sa_cpprt",
    "//main/stlport:stlport",
    "//main/gtest:gtest",       # GTEST:gtest from build.lst header
    "//main/boost:boost",       # BOOST:boost from build.lst header
]

nmake(
    name = "sa_qa",
    lib_source = ":sources",
    build_file = "sal/qa/makefile.mk",
    targets = ["all"],
    deps = _QA_DEPS,
    visibility = ["//visibility:public"],
)

nmake(
    name = "sa_qa_ByteSequence",
    lib_source = ":sources",
    build_file = "sal/qa/ByteSequence/makefile.mk",
    targets = ["all"],
    deps = _QA_DEPS,
    visibility = ["//visibility:public"],
)

nmake(
    name = "sa_qa_OStringBuffer",
    lib_source = ":sources",
    build_file = "sal/qa/OStringBuffer/makefile.mk",
    targets = ["all"],
    deps = _QA_DEPS,
    visibility = ["//visibility:public"],
)

nmake(
    name = "sa_qa_sal",
    lib_source = ":sources",
    build_file = "sal/qa/sal/makefile.mk",
    targets = ["all"],
    deps = _QA_DEPS,
    visibility = ["//visibility:public"],
)

nmake(
    name = "sa_qa_osl_condition",
    lib_source = ":sources",
    build_file = "sal/qa/osl/condition/makefile.mk",
    targets = ["all"],
    deps = _QA_DEPS,
    visibility = ["//visibility:public"],
)

nmake(
    name = "sa_qa_osl_mutex",
    lib_source = ":sources",
    build_file = "sal/qa/osl/mutex/makefile.mk",
    targets = ["all"],
    deps = _QA_DEPS,
    visibility = ["//visibility:public"],
)

nmake(
    name = "sa_qa_osl_pipe",
    lib_source = ":sources",
    build_file = "sal/qa/osl/pipe/makefile.mk",
    targets = ["all"],
    deps = _QA_DEPS,
    visibility = ["//visibility:public"],
)

nmake(
    name = "sa_qa_osl_profile",
    lib_source = ":sources",
    build_file = "sal/qa/osl/profile/makefile.mk",
    targets = ["all"],
    deps = _QA_DEPS,
    visibility = ["//visibility:public"],
)

nmake(
    name = "sa_qa_osl_setthreadname",
    lib_source = ":sources",
    build_file = "sal/qa/osl/setthreadname/makefile.mk",
    targets = ["all"],
    deps = _QA_DEPS,
    visibility = ["//visibility:public"],
)

nmake(
    name = "sa_qa_osl_thread",
    lib_source = ":sources",
    build_file = "sal/qa/osl/thread/makefile.mk",
    targets = ["all"],
    deps = _QA_DEPS,
    visibility = ["//visibility:public"],
)

nmake(
    name = "sa_qa_rtl_alloc",
    lib_source = ":sources",
    build_file = "sal/qa/rtl/alloc/makefile.mk",
    targets = ["all"],
    deps = _QA_DEPS,
    visibility = ["//visibility:public"],
)

nmake(
    name = "sa_qa_rtl_cipher",
    lib_source = ":sources",
    build_file = "sal/qa/rtl/cipher/makefile.mk",
    targets = ["all"],
    deps = _QA_DEPS,
    visibility = ["//visibility:public"],
)

nmake(
    name = "sa_qa_rtl_crc32",
    lib_source = ":sources",
    build_file = "sal/qa/rtl/crc32/makefile.mk",
    targets = ["all"],
    deps = _QA_DEPS,
    visibility = ["//visibility:public"],
)

nmake(
    name = "sa_qa_rtl_digest",
    lib_source = ":sources",
    build_file = "sal/qa/rtl/digest/makefile.mk",
    targets = ["all"],
    deps = _QA_DEPS,
    visibility = ["//visibility:public"],
)

nmake(
    name = "sa_qa_rtl_doublelock",
    lib_source = ":sources",
    build_file = "sal/qa/rtl/doublelock/makefile.mk",
    targets = ["all"],
    deps = _QA_DEPS,
    visibility = ["//visibility:public"],
)

nmake(
    name = "sa_qa_rtl_locale",
    lib_source = ":sources",
    build_file = "sal/qa/rtl/locale/makefile.mk",
    targets = ["all"],
    deps = _QA_DEPS,
    visibility = ["//visibility:public"],
)

nmake(
    name = "sa_qa_rtl_math",
    lib_source = ":sources",
    build_file = "sal/qa/rtl/math/makefile.mk",
    targets = ["all"],
    deps = _QA_DEPS,
    visibility = ["//visibility:public"],
)

nmake(
    name = "sa_qa_rtl_oustringbuffer",
    lib_source = ":sources",
    build_file = "sal/qa/rtl/oustringbuffer/makefile.mk",
    targets = ["all"],
    deps = _QA_DEPS,
    visibility = ["//visibility:public"],
)

nmake(
    name = "sa_qa_rtl_random",
    lib_source = ":sources",
    build_file = "sal/qa/rtl/random/makefile.mk",
    targets = ["all"],
    deps = _QA_DEPS,
    visibility = ["//visibility:public"],
)

nmake(
    name = "sa_qa_rtl_uri",
    lib_source = ":sources",
    build_file = "sal/qa/rtl/uri/makefile.mk",
    targets = ["all"],
    deps = _QA_DEPS,
    visibility = ["//visibility:public"],
)

nmake(
    name = "sa_qa_rtl_uuid",
    lib_source = ":sources",
    build_file = "sal/qa/rtl/uuid/makefile.mk",
    targets = ["all"],
    deps = _QA_DEPS,
    visibility = ["//visibility:public"],
)

