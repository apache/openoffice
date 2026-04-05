# Apache OpenOffice — Bazel Migration

## Goal
Replace the Perl/dmake/gmake orchestration layer with Bazel.
Primary constraint: eliminate the Cygwin dependency from the build.
Source code is NOT being changed — only the build system.

## Environment
- OS: Windows 11
- Shell during migration work: bash (via Cygwin, being phased out)
- Compiler: MSVC (configured via user.bazelrc)
- Bazel configured via MODULE.bazel and user.bazelrc

## Build strategy
- Third-party deps: wrap with rules_foreign_cc (cmake/make as appropriate)
- First-party modules: migrate to native Bazel cc_library/cc_binary
- Goal: eliminate gmake and dmake from first-party builds entirely
- rules_foreign_cc is a bridge for external code, not a destination

## What is done
- MODULE.bazel (root) with rules_cc, rules_foreign_cc, platforms
- user.bazelrc pointing Bazel to MSVC
- Bootstrap/external deps via Bazel registry
    - modules seperated, only partial migration
- autoconf/configure replaced by Bazel toolchain

## Key conventions
- BUILD.bazel files live at main/<package>/prj/BUILD.bazel
- build.lst drives dep graph: parse it to determine deps = []
- Wrapping nmake with rules_foreign_cc make() for now (MVP)

## Current frontier
- building sal module

## Out of scope
- Modifying source code