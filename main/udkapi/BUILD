load("@rules_cc//cc:defs.bzl", "cc_library")
load("//build/rules:idl_pipeline.bzl", "idl_library")

cc_library(
    name = "udkapi",
    hdrs = glob(["inc/**/*.h", "inc/**/*.hxx"], allow_empty = True),
    includes = ["inc"],
    visibility = ["//visibility:public"],
)

idl_library(
    name = "udkapi_idl",
    srcs = glob(["com/sun/star/**/*.idl"]),
    visibility = ["//visibility:public"],
)

