---
description: "Apache OpenOffice Bazel migration workspace. Provides build commands, key conventions, architecture overview, and links to detailed migration documentation."
---

# Apache OpenOffice Bazel Migration

This workspace contains the Apache OpenOffice codebase undergoing migration from the legacy Perl/dmake/gmake build system to Bazel, with the goal of eliminating Cygwin dependency.

## Project Overview

- **Goal**: Replace orchestration layer with Bazel, keep source code unchanged.
- **Environment**: Windows 11, MSVC (VS2008), Bazel with custom toolchain.
- **Status**: Phase 1 complete (sal, salhelper, cppu migrated). Phase 2: idl pipeline (store, registry, idlc, codemaker).

## Key Conventions

- BUILD.bazel files in main/<module>/ (not prj/)
- Dependencies from build.lst parsing
- Windows DEF files for DLL exports (not GNU ld maps)
- Two-tier include paths: strip_include_prefix + includes for PCH
- VS2008 quirks: /Dsnprintf=_snprintf, /Zm500, /EHsc

## Build Commands

- `bazel build //main/sal:sal` - Build SAL DLL
- `bazel build //main/...` - All migrated modules
- `bazel clean` - Clear cache

## Architecture

Modules layered: stlport → sal → salhelper → cppu → (store → registry → idlc → codemaker)

## Pitfalls

- Import libraries require Windows DEF files
- DllMain in srcs, not sub-libs
- Hardcoded paths in toolchain

For detailed migration status, blockers, and completed phases: see [CLAUDE.md](CLAUDE.md)

For build patterns and directory conventions: see [build/build.md](build/build.md)

For quick reference: see [build/build-ai-summary.md](build/build-ai-summary.md)