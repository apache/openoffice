This is the build documentation based for the bazel approach.
# Repository Overview

apache-openoffice/
├── WORKSPACE                        # Bazel workspace root
├── BUILD.bazel                      # Top-level aliases and meta targets
├── .bazelversion                    # Pinned Bazel version
│
├── build/                           # Global build infrastructure
│   ├── rules/                       # Custom Bazel rules
│   │   ├── make_rules.bzl           # Wrap existing make targets
│   │   ├── dmake_rules.bzl          # Wrap OOo's dmake tool
│   │   ├── idl_rules.bzl            # UNO IDL compilation rules
│   │   └── ant_rules.bzl            # Wrap ant builds (Java modules)
│   ├── platforms/                   # Platform definitions (Linux/Win/Mac)
│   │   └── BUILD.bazel
│   ├── toolchains/                  # Compiler and tool configurations
│   │   └── BUILD.bazel
│   ├── tools/                       # Migration and helper scripts
│   │   └── generate_build_files.py
│   │
│   └── third_party/                 # Build-only external tools
│       ├── gtest/
│       │   └── BUILD.bazel          # test framework - never ships
│       └── ratscan/
│           └── BUILD.bazel          # license compliance - never ships
│
├── main/                            # OOo source modules (source ONLY)
│   ├── sal/                         # Simple C runtime abstraction
│   │   ├── prj/
│   │   │   ├── build.lst            # existing - dependency ordering
│   │   │   ├── d.lst                # existing - file delivery list
│   │   │   └── BUILD.bazel          # new - Bazel build instructions
│   │   ├── inc/
│   │   └── source/
│   ├── vcl/                         # Visual class library
│   │   └── prj/
│   │       ├── build.lst
│   │       ├── d.lst
│   │       └── BUILD.bazel
│   └── <module>/                    # All other modules follow same pattern
│       └── prj/
│           ├── build.lst
│           ├── d.lst
│           └── BUILD.bazel
│
├── ext_sources/                     # Third-party ORIGINAL archives (unchanged)
│   ├── icu-4.2.1.tar.gz
│   ├── python-2.6.tar.gz
│   ├── curl-7.19.7.tar.gz
│   └── ...
│
└── ext_libraries/                   # Third-party built libraries
    └── <library>/
        └── prj/
            ├── build.lst
            └── BUILD.bazel

Separation of Concerns
Directory 	Purpose 	Owned By 	Bazel Role
build/ 	Global build infrastructure 	Build team 	Rules, toolchains, platforms
main/*/prj/ 	Module build instructions 	Module owners 	BUILD.bazel per module
main/*/source/ 	Source code 	Developers 	Inputs only - never touched by Bazel migration
main/*/inc/ 	Headers 	Developers 	Inputs only - never touched by Bazel migration
ext_sources/ 	Third-party source archives 	Build team 	Wrapped via BUILD.bazel in prj/
ext_libraries/ 	Third-party built libs 	Build team 	Wrapped via BUILD.bazel in prj/
prj/ Folder Evolution

The existing prj/ folder is already OOo's convention for build metadata.
Bazel extends this naturally without disrupting existing tools.

prj/
├── build.lst    # EXISTING - module dependency ordering (kept untouched)
├── d.lst        # EXISTING - file delivery to solver/ (kept untouched)
└── BUILD.bazel  # NEW      - Bazel build instructions for this module

Migration Phases per Module

Phase 1 │ BUILD.bazel wraps existing makefile.mk via dmake_rules / make_rules
        │ build.lst parsed to generate deps[] automatically
        │
Phase 2 │ BUILD.bazel replaces makefile.mk with native Bazel rules
        │ d.lst delivery mapped to Bazel outputs
        │
Phase 3 │ makefile.mk and dmake files deleted
        │ prj/ contains only BUILD.bazel (build.lst/d.lst retired)

Target Naming Convention

# Module builds
bazel build //main/sal/prj:sal
bazel build //main/vcl/prj:vcl
bazel build //main/sw/prj:sw

# Third-party
bazel build //ext_sources/boost/prj:boost
bazel build //ext_libraries/icu/prj:icu

# Everything
bazel build //main/...
bazel build //ext_libraries/...

# Query dependency graph
bazel query "deps(//main/sw/prj:sw)"

# Build infrastructure
bazel build //build/toolchains:all

# dependency handling:

Proposed Mapping to Existing Structure

build/
├── platforms/
│   ├── linux-generic.bazel
│   ├── linux-debian.bazel      # ← distro upstreams here
│   ├── linux-fedora.bazel
│   ├── freebsd.bazel
│   ├── windows.bazel
│   └── macos.bazel
│
├── third_party/                # ← becomes the dep definitions home
│   ├── README.md               # "how to add a dependency"
│   ├── mandatory/
│   │   ├── ucpp.bzl
│   │   └── mythes.bzl
│   ├── optional/
│   │   ├── graphite.bzl
│   │   ├── nss.bzl
│   │   └── cairo.bzl
│   └── build_only/
│       └── gtest.bzl

ext_libraries/
├── icu/                        # modified dependency
│   ├── patches/                # ← "I know I was modified"
│   │   └── aoo-icu-fix.patch
│   ├── MODULE                  # existing format
│   └── BUILD.bazel             # ← how to build the modified version
│
├── boost/
│   ├── patches/
│   ├── MODULE
│   └── BUILD.bazel

ext_sources/                    # phase out gradually
│                               # replaced by Bazel cache
└── (existing tarballs during migration)

main/
└── sc/
    └── prj/
        └── deps                # ← "I need icu, graphite(optional)"

The Clean Ownership Model

Question                              Owned By
──────────────────────────────────    ─────────────────────
What source/version of this dep?      build/third_party/<optional|mandatory|build_only>/
Was this dep patched for AOO?         ext_libraries/<dep>/patches/
How to build this dep?                ext_libraries/<dep>/BUILD.bazel
What does module X need?              main/<module>/prj/deps
On this OS/distro, system or bundle?  build/platforms/<distro>.bazel

How Modified Dependency Knows It Was Modified

ext_libraries/
└── icu/
    ├── MODULE                   # existing - keep format
    ├── patches/
    │   ├── 001-aoo-specific.patch
    │   └── 002-windows-fix.patch
    ├── BUILD.bazel              # build rules for patched version
    └── source.bzl               # ← points back to third_party
        # "I am a modified version of @icu from build/third_party"
## example icu
# ext_libraries/icu/source.bzl
ICU_MODIFICATION = {
    "base": "//build/third_party/mandatory:icu",  # original source
    "patches": [
        "//ext_libraries/icu/patches:001-aoo-specific.patch",
        "//ext_libraries/icu/patches:002-windows-fix.patch",
    ],
    "reason": "AOO requires non-standard collation behavior, see MODULE",
}

# workflow with lock file
## After changing MODULE.bazel, update the lock:
bazel mod deps --lockfile_mode=update

## Commit both together:
git add MODULE.bazel MODULE.bazel.lock
git commit -m "build: add rules_foreign_cc dependency"
