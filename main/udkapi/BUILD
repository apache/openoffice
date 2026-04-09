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

# Expose the cppumaker-generated headers as a cc_library.
# The _inc directory is a declare_directory output — Bazel can't parse it
# for virtual includes, so we pass the path explicitly via copts.
# Consumers get both the header files (as inputs) and the -I flag.
cc_library(
    name = "udkapi_idl_headers",
    hdrs = [":udkapi_idl"],
    # includes paths are relative to execroot and propagated transitively.
    # "." here means execroot itself — but we need the generated output dir.
    # Use includes with the bin-relative path: Bazel prepends both
    # the source root and the bin root automatically for each entry in includes.
    includes = ["udkapi_idl_inc"],
    visibility = ["//visibility:public"],
)

