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

## Key conventions
- BUILD.bazel files live at main/<package>/BUILD.bazel (NOT prj/)
  - prj/ convention only worked for nmake() wrappers; cc_library needs
    glob() access to sources which requires the BUILD at module root
- build.lst drives dep graph: parse it to determine deps = []
- .tab files in textenc are #include'd data tables → use textual_hdrs
- All sal targets need deps = [":sal_headers", ":sal_pch"]

## Current frontier
### migragion of the idl pipeline
win_flex + win_bison  ← new ext dep (generate parser.cxx, scanner.cxx)
        │
ucpp  ✅ already done
sal   ✅ already done
        │
salhelper  ✅ already done
store  ✅ already done
        │
registry  ✅ already done
        │
idlc  ✅ already done
regmerge   1 .cxx, deps: registry              ← trivial
        │
codemaker  27 .cxx, deps: registry   ← medium
        │
cppumaker   8 .cxx, deps: codemaker  ← easy
        │
genrule: .idl → .urd → .rdb → .hdl/.hpp

### Current frontier: salhelper link step
What works:

All 3 .cxx files compile cleanly
sal_implib filegroup correctly exposes sal.if.lib to salhelper's linker (it now appears in the params file)
Blocker discovered:
sal.if.lib is only 1484 bytes — a Bazel-generated empty stub, not a real import library. The cause: sal.map is a GNU ld version script format, not a Windows DEF file. MSVC link.exe ignores it and uses __declspec(dllexport) for the actual exports — but that means MSVC only builds a real import library when a proper /DEF: file is supplied in Windows format.

What needs to happen:
main/sal/ needs a proper Windows DEF file (listing all exported C symbols like osl_acquireMutex, rtl_uString_acquire, etc.) so that link.exe generates a usable sal.if.lib. The sal.map already has the full symbol list — it just needs to be converted from GNU map format to MSVC DEF format.

Pattern already established in cppu: util/cppu.def is a proper Windows DEF file. The same approach is needed for sal.

Then salhelper (and all future DLLs linking against sal) will just work.
#### update:
Created main/sal/util/sal.def — a proper Windows DEF file with 545 exports converted from the GNU ld version script sal.map (minus GetVersionInfo which requires a generated _version.h not yet available in the Bazel build).

Switched to win_def_file = "util/sal.def" in main/sal/BUILD.bazel — using the win_def_file attribute instead of linkopts so Bazel uses our DEF file instead of generating an empty one that was overriding the linkopts-specified DEF.

Moved /Dsnprintf=_snprintf to _COPTS — it was only on rtl before, but osl_w32 and osl_all also need it since VS2008 lacks snprintf.

sal.if.lib went from 1484 bytes (empty stub) to 131 KB (real import library with all 545 exports), unblocking all downstream DLLs.

Confirms what we already found: GetVersionInfo is a build-system artifact injected by tg_def.mk into every DLL's DEF file. The filter scripts (fix_def_file.cmd, fix_dxp_file.cmd, fix_exp_file.cmd) also strip _GetVersionInfo when processing export lists. Removing it from sal.def was the right call.
#### registry changes
Fixed salhelper: converted GNU ld msci.map → proper Windows salhelper.def, switched to win_def_file, added salhelper_implib filegroup
Created sal/udkversion.h — static file from the udkversion.mk constants (3.2.7)
Created store_headers/store_implib were already working from the previous step
Created main/registry/util/reg.def — 73 exports from both UDK_3_0_0 and UDK_3.1 sections
Created main/registry/BUILD.bazel with stlport dep (needed for <hash_map>)
## Out of scope
- Modifying source code