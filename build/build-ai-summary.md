Migration Summary for New Chat Context
Project

Apache OpenOffice (AOO) 4.5.0, migrating from autoconf/dmake/nmake build system to Bazel. Windows, Cygwin environment.
What is already done

✅ MODULE.bazel         (root, with rules_cc, rules_foreign_cc, platforms)
✅ user.bazelrc         (points Bazel to MSVC, builds work already)
✅ bootstrap replaced   (external modules via Bazel registry)
✅ autoconf/configure   (replaced by Bazel toolchain via user.bazelrc)

MVP Goal

For each package (sal, salhelper, etc.):
  1. Write BUILD.bazel wrapping existing nmake
  2. Wire deps = [] from build.lst
  3. bazel build //main/... replaces build.pl --all

Key files per package to understand before writing BUILD.bazel

main/<package>/prj/build.lst    → package name, deps, build command
main/<package>/prj/d.lst        → what files get delivered (output)
main/<package>/makefile.mk      → top level makefile

build.lst format

shp salhelper          : sal NULL          ← depends on sal
shp salhelper\prj      nmake - all shp_prj NULL  ← build target name

Pattern for each package BUILD.bazel

# main/salhelper/prj/BUILD.bazel
load("@rules_foreign_cc//foreign_cc:defs.bzl", "make")

filegroup(
    name = "sources",
    srcs = glob(["**/*"]),
    visibility = ["//visibility:public"],
)

make(
    name = "shp_prj",          # from build.lst
    lib_source = "//main/salhelper:sources",
    target = "all",
    deps  = ["//main/sal/prj:sal_prj"],   # from build.lst
)


To start a new package migration, provide

    build.lst content
    makefile.mk content
    d.lst content if available
    Any known special dependencies