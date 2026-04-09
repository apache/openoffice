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
- Custom MSVC toolchain at //build/toolchain (VS2008, x86)
    - windows_cc_toolchain_config.bzl copied and patched:
      - default_cpp_std disabled (VS2008 has no /std: flag)
      - remove_unreferenced_code disabled (VS2008 has no /Zc:inline)
    - tool_bin_path = VC\bin (not msvc_env_path) sets PATH for actions
    - BAZEL_DO_NOT_DETECT_CPP_TOOLCHAIN=1 disables auto-detection
- main/sal/BUILD.bazel with native cc_library targets -> done
    - sal_headers: exported inc/ headers (strip_include_prefix = "inc")
    - sal_pch: inc/pch/ headers (strip_include_prefix = "inc/pch")
    - textenc: BUILT SUCCESSFULLY → textenc.lib
    - rtl: BUILT SUCCESSFULLY
    - osl: BUILT SUCCESSFULLY
    - uwinapi: BUILT SUCCESSFULLY

- sal ✅ (sal3.dll, sal.if.lib — 545 exports via sal.def)
- salhelper ✅ (salhelper3MSC.dll)
- store ✅
- registry ✅ (reg.dll, regmerge.exe)
- idlc ✅, codemaker/cppumaker ✅, regmerge ✅
- udkapi ✅ — IDL pipeline working end-to-end:
    - idl_pipeline.bzl: ctx.actions.symlink staging (no shell), batched regmerge,
      external .manifest for CRT, "./" prefix on cppumaker -O for osl path compat
    - //main/udkapi:udkapi_idl → udkapi_idl.rdb + udkapi_idl_inc/
    - //main/udkapi:udkapi_idl_headers → transitive cc_library with includes = ["udkapi_idl_inc"]
- cppu ✅ — all targets:
    - cppu3.dll (full UDK_3_0_0 + UDK_3.1 + UDK_3.2 + UDK_3.3 exports in cppu.def)
    - purpenvhelper3MSC.dll
    - unsafe_uno_uno.dll, affine_uno_uno.dll, log_uno_uno.dll

## Key conventions
- BUILD.bazel files live at main/<package>/BUILD.bazel (NOT prj/)
  - prj/ convention only worked for nmake() wrappers; cc_library needs
    glob() access to sources which requires the BUILD at module root
- build.lst drives dep graph: parse it to determine deps = []
- .tab files in textenc are #include'd data tables → use textual_hdrs
- All sal targets need deps = [":sal_headers", ":sal_pch"]

## Current frontier

```
sal       ✅
salhelper ✅
store     ✅
registry  ✅
ucpp      ✅
idlc      ✅  (+ regmerge, cppumaker)
udkapi    ✅  (.idl → .urd → .rdb → .hdl/.hpp via idl_pipeline.bzl)
cppu      ✅  (cppu3, purpenvhelper3MSC, affine/unsafe/log bridge DLLs)
          │
          ▼
      cppuhelper   ← next target
          │        deps: cppu, sal, salhelper, codemaker, boost, libxslt
          │        produces: cppuhelper3MSC.dll
          │
          ▼
        bridges    deps: cppuhelper, salhelper
```

### Notes for cppuhelper
- build.lst: `cppuhelper : BOOST:boost LIBXSLT:libxslt codemaker cppu offapi NULL`
- `offapi` is the OOo API IDL layer — may need its own idl_library target first
- libxslt is a third-party dep (rules_foreign_cc or pre-built)
- Sources are flat in `source/` — no subdirectory split like cppu
- Will need `cppuhelper.map` → `cppuhelper3MSC.def` conversion (same pattern as sal/registry)
- Generated headers from `udkapi_idl` already available via `//main/udkapi:udkapi_idl_headers`

## Key conventions
- BUILD.bazel files live at main/<package>/BUILD.bazel (NOT prj/)
  - prj/ convention only worked for nmake() wrappers; cc_library needs
    glob() access to sources which requires the BUILD at module root
- build.lst drives dep graph: parse it to determine deps = []
- .tab files in textenc are #include'd data tables → use textual_hdrs
- All sal targets need deps = [":sal_headers", ":sal_pch"]
- Generated UNO headers: depend on //main/udkapi:udkapi_idl_headers
  (provides -I path to cppumaker output via includes = ["udkapi_idl_inc"])
- DLL export pattern: convert <name>.map GNU ld script → <name>.def Windows DEF,
  use win_def_file = "util/<name>.def", expose implib via filegroup output_group = "interface_library"
- IDL pipeline: tools link /MD (dynamic CRT); stage msvcr90/msvcp90 DLLs +
  external .manifest files alongside EXEs (no mt.exe embedding); use
  ctx.actions.symlink (pure Bazel, no shell) for staging
- cppumaker output dir: prefix -O with "./" so osl's convertToFileUrl uses
  getAbsoluteFileURL (relative-to-workdir) instead of failing on relative paths

## Out of scope
- Modifying source code
            
