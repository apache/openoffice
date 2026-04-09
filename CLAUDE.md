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
- cppuhelper ✅ — cppuhelper3MSC.dll
    - private IDL compiled via idl_library with include_dirs + extra_rdbs
    - idl_pipeline.bzl gained include_dirs (string_list) and extra_rdbs attrs

## Key conventions
- BUILD.bazel files live at main/<package>/BUILD.bazel (NOT prj/)
  - prj/ convention only worked for nmake() wrappers; cc_library needs
    glob() access to sources which requires the BUILD at module root
- build.lst drives dep graph: parse it to determine deps = []
- .tab files in textenc are #include'd data tables → use textual_hdrs
- All sal targets need deps = [":sal_headers", ":sal_pch"]

## Current frontier

```
sal         ✅
salhelper   ✅
store       ✅
registry    ✅
ucpp        ✅
idlc        ✅  (+ regmerge, cppumaker)
udkapi      ✅  (.idl → .urd → .rdb → .hdl/.hpp via idl_pipeline.bzl)
cppu        ✅  (cppu3, purpenvhelper3MSC, affine/unsafe/log bridge DLLs)
cppuhelper  ✅  (cppuhelper3MSC.dll)
            │
            ▼
          bridges    ← next target
                     deps: cppuhelper, salhelper
```

### Notes for cppuhelper (done)
- libxslt listed in build.lst but not actually used by any source — skipped
- offapi listed in build.lst but all required headers come from udkapi — skipped
- Private IDL: `unotypes/cppuhelper/detail/XExceptionThrower.idl`
    - uses `include_dirs` + `extra_rdbs` attrs added to idl_pipeline.bzl
    - `extra_rdbs = ["//main/udkapi:udkapi_idl"]` for cppumaker type resolution
    - `.rdb` filter needed: `idl_library` DefaultInfo returns both .rdb and _inc dir
- Export macro: `CPPUHELPER_DLLIMPLEMENTATION` (not `CPPUHELPER_DLL_IMPLEMENTATION`)

### Next: bridges
- build.lst: `bridges : cppuhelper jurt jvmaccess salhelper NULL`
- jurt/jvmaccess are Java bridge deps — likely not needed for the C++ bridge
- The C++ bridge (msci bridge) is in `source/cpp_uno/msvc_win32_intel/`
- Will need its own DEF file

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
            
