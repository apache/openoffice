# sal/BUILD
load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "cppu",
    hdrs = glob(["inc/**/*.h", "inc/**/*.hxx"], allow_empty = True),
    includes = ["inc"],
    visibility = ["//visibility:public"],
)
