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
regmerge  ✅ already done
        │
codemaker  ✅ already done
        │
cppumaker  ✅ already done
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
#### idl pipeline
Compile IDL pipeline tools with /MT (static CRT)
Add /MT to the copts of idlc, regmerge, and cppumaker. This links the CRT statically into the exe — no msvcr90.dll dependency, no staging needed, they run standalone anywhere.

This is not a source code change — it's a build flag. It's also standard practice for build tools that need to run on arbitrary machines.

After that, the Starlark rule simplifies to:

Minimal staging: just copy ucpp.exe next to idlc.exe (idlc hardcodes finding ucpp by path substitution — unavoidable)
ctx.actions.run() invokes idlc, regmerge, cppumaker directly — no PATH games, no DLL workarounds
The _collect_runtime_dlls function, all four DLL attributes, and the entire DLL staging logic in idl_pipeline.bzl disappear.
###### solution
That's everything wired up. Summary of what was done:

main/external/msvcp90/BUILD.bazel — filegroup exposing the 4 checked-in CRT files
build/rules/idl_pipeline.bzl — cleaned up: _crt_dlls attr points to //main/external/msvcp90:crt_dlls, dead rename_map removed
build/rules/BUILD.bazel — empty package marker so the .bzl load path resolves
main/udkapi/BUILD — replaced both genrules with a single idl_library(name = "udkapi_idl", ...)
#### error:
idl compilation fails with windows error of wrong c call.
The IDL pipeline is working end-to-end:

.idl → .urd (idlc) ✅
.urd → .rdb (regmerge, batched) ✅
.rdb → .hdl/.hpp (cppumaker) ✅
That's a significant milestone — the full udkapi type system compiled under Bazel with no Cygwin, no shell, pure native Windows execution.

## Key conventions
- BUILD.bazel files live at main/<package>/BUILD.bazel (NOT prj/)
  - prj/ convention only worked for nmake() wrappers; cc_library needs
    glob() access to sources which requires the BUILD at module root
- build.lst drives dep graph: parse it to determine deps = []
- .tab files in textenc are #include'd data tables → use textual_hdrs
- All sal targets need deps = [":sal_headers", ":sal_pch"]

## Current frontier


## Out of scope
- Modifying source code
on shell there is INFO: From Compiling main/idlc/source/idlc.cxx [for tool]:
cl : Command line warning D9002 : ignoring unknown option '/Gw'
idlc.cxx
and:
C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\INCLUDE\xtree(637) : warning C4800: 'sal_Bool' : forcing value to bool 'true' or 'false' (performance warning)
        C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\INCLUDE\xtree(630) : while compiling class template member function 'std::pair<_Ty1,_Ty2> std::_Tree<_Traits>::insert(const rtl::OString &)'
        with
        [
            _Ty1=std::_Tree<std::_Tset_traits<rtl::OString,LessString,std::allocator<rtl::OString>,false>>::iterator,
            _Ty2=bool,
            ...
            
