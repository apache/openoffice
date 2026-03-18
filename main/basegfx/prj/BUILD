# basegfx/BUILD
load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "basegfx",
    srcs = glob(["source/**/*.cxx"]),
    hdrs = glob([
        "inc/basegfx/**/*.hxx",
        "inc/pch/*.hxx",
    ]),
    includes = [
        "inc",
        "source/inc",
        "inc/pch",
    ],
    defines = [
        "BASEGFX_DLLIMPLEMENTATION",
        "WIN32",
        "WNT",
        "_WINDOWS",
    ],
    deps = [
        "//sal:sal",
        "//cppu:cppu",
        "//cppuhelper:cppuhelper",
        "//udkapi:udkapi",
        "//offapi:offapi",
    ],
    visibility = ["//visibility:public"],
)
