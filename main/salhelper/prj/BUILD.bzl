
load("@rules_foreign_cc//foreign_cc:defs.bzl", "make")

make(
    name = "shp_prj",
    lib_source = "//main/salhelper:sources",
    target = "all",
    deps  = ["//main/sal/prj:sal_prj"],
)