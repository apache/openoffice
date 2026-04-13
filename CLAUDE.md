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
    msci_uno.dll built successfully. Here's what was done:

Sources — two layers compiled into one DLL:

source/cpp_uno/shared/ — 7 platform-neutral .cxx files (bridge, component, vtablefactory, etc.)
source/cpp_uno/msvc_win32_intel/ — 4 MSVC x86 files (cpp2uno, uno2cpp, except, dllinit)
DEF file — util/msci_uno.def with 3 extern "C" exports; the RTTI symbols in bridge_exports.map are GCC-mangled and don't apply to MSVC.

Links — cppu3.if.lib + sal.if.lib only, matching the original SHL1STDLIBS in makefile.mk.

The only wrinkle was no <hash_map> stlport needed — VS2008's native STL provided it fine, just with a harmless warning.

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
bridges     ✅  (msci_uno.dll)
rdbmaker    ✅  (rdbmaker.exe)
xmlreader   ✅  (xmlreader.dll)
stoc        ✅  (bootstrap.uno.dll, stocservices.uno.dll)
jvmfwk      ✅  (jvmfwk.dll)
io          ✅  (streams.dll, acceptor.dll, connector.dll, textinstream.dll, textoutstream.dll)
            │
            ▼
o3tl       ✅  (header-only)
vos        ✅  (vos3.dll)
libxslt    ✅  (@libxslt//:libxslt + :libexslt)
offapi     ✅  (offapi_idl.rdb + offapi_idl_inc/ — 3718 IDL files)
binaryurp     ✅  (binaryurp.dll)
remotebridges ✅  (uuresolver.dll)
cpputools     ✅  (regcomp.exe, regsingleton.exe, sp2bv.exe, uno.exe)
i18nutil      ✅  (i18nutil.dll)
ucbhelper     ✅  (ucbhelper.dll)
comphelper    ✅  (comphelp.dll)
configmgr     ✅  (configmgr.uno.dll)
sax           ✅  (sax.dll, expwrap.dll, fastsax.dll)
package       ✅  (package2.dll, xstor.dll)
i18npool      partial:
  i18nisolang1 ✅  (i18nisolang1.dll — no ICU, language/locale identification)
  i18npaper    ✅  (i18npaper.dll — no ICU, paper size info)
  i18npool.dll ⏳  (needs ICU 49.1.2 — see notes below)
  i18nsearch   ⏳  (needs ICU — same)
            │
            ▼
          ??? ← next
```
### current situation
Problem: i18npool.dll is failing to link because OpenOffice_dat.obj has 13 unresolved external symbol errors like:


LNK2001: unresolved external symbol _OpenOffice_bazel_out_x64_windows_fastbuild_bin_main_i18npool_pool_char_in_patched_brk
Root cause: A naming inconsistency between genccode and gencmn:

genccode is called with brk.path (full path like bazel-out/.../char_in_patched.brk) but internally it calls findBasename() — so it strips the directory and generates symbol OpenOffice_char_in_patched_brk (basename only)
gencmn list file is written with b.path (full paths), so it derives symbol OpenOffice_bazel_out_x64_windows_fastbuild_bin_main_i18npool_pool_char_in_patched_brk (full path)
The compiled _brk.c objects define one set of symbol names; OpenOffice_dat.c references a different set.

Fix needed in main/i18npool/breakiter_pipeline.bzl: The gencmn list file at line 74 currently uses b.path (full exec-root paths). We need it to use basenames instead. But gencmn also needs to be able to open the .brk files — if the list only has basenames like char_in_patched.brk, gencmn needs to be run from the directory containing those files.

Solution options:

Use ctx.actions.run_shell to cd into the .brk output directory before running gencmn (with a basename-only list file)
Look for a gencmn flag that strips paths from symbol names

### Notes for i18nisolang1 and i18npaper (done)
- inwnt.cxx is #included by insys.cxx via platform guard — add `cc_library(textual_hdrs = [...])` dep
- i18npaper links against i18nisolang1 (for MsLangId class)

### Notes for i18npool ICU setup (pending)
ICU 72.1 is the only version in ext_sources but requires C++17 — won't compile with VS2008.
ICU 49.1.2 is the newest VS2008-compatible ICU (official VS2008 project files included).

To unblock i18npool.dll:
1. Download `icu4c-49_1_2-src.tgz` from https://download.icu-project.org/files/icu4c/49.1.2/
2. Place at `ext_sources/download.icu-project.org/files/icu4c/49.1.2/icu4c-49_1_2-src.tgz`
3. Compute SHA256: `openssl dgst -sha256 -binary <file> | openssl base64`
4. Update `ext_libraries/modules/icu/49.1.2/source.json` with the real integrity hash
5. Uncomment `bazel_dep(name = "icu", version = "49.1.2")` in MODULE.bazel
6. Run `bazel mod deps --lockfile_mode=refresh` then build `//main/i18npool/pool:i18npool`

ICU module structure already set up:
- `ext_libraries/modules/icu/49.1.2/overlay/BUILD.bazel` — cc_library targets for icuuc/icui18n/icudata
  static builds for build tools (genbrk/gencmn/genccode), shared DLLs for i18npool/i18nsearch
- `ext_libraries/modules/icu/49.1.2/overlay/MODULE.bazel` — module declaration
- `main/i18npool/breakiter_pipeline.bzl` — Starlark rule: .txt → genbrk → .brk → genccode → _brk.c
  + gencmn → OpenOffice_dat.c (no bash genrule — uses ctx.actions.run)
- `main/i18npool/pool/BUILD.bazel` still TODO (i18npool.dll and i18nsearch.dll targets)

### Notes for io (done)
- 5 DLLs: streams, acceptor, connector, textinstream, textoutstream
- All use SAL_DLLPUBLIC_EXPORT — no DEF files needed
- LIBXSLT in build.lst is a legacy entry — none of the source files use it
- Requires `/Zc:wchar_t-` so sal_Unicode = unsigned short (not native wchar_t);
  without it Sequence<sal_Unicode> fails to resolve cppu_detail_getUnoType overload

### Notes for jvmfwk (done)
- Deps: cppu, cppuhelper, sal, udkapi_idl_headers, stlport, boost.legacy, @libxml2//:libxml2, advapi32
- Exports: 21 C-linkage `jfw_*` functions from `jfw_headers` (inc/jvmfwk/) via jvmfwk.def
- libxml2 overlay fixes needed for VS2008:
  - `config.h` overlay: win32/VC10/config.h base + HAVE_STDINT_H guarded to VS2010+, SEND_ARG2_CAST defined empty
  - `libxml2-configure.patch` disables FTP, HTTP, iconv, debug, zlib, lzma in xmlversion.h
  - nanoftp.c and nanohttp.c excluded from LIBXML2_SRCS (networking not needed)
  - Use `bazel mod deps --lockfile_mode=refresh` after any overlay/patch hash change

### Notes for bridges (done)
- jurt/jvmaccess are Java bridge deps — skipped (not needed for C++ bridge)
- DLL name: `msci_uno` (from COMNAME=msci on WNT-INTEL-msci)
- Sources: `source/cpp_uno/shared/*.cxx` + `source/cpp_uno/msvc_win32_intel/*.cxx`
- Local private headers (component.hxx, guardedarray.hxx, msci.hxx) accessed via copts `/I`
- Exports: 3 C-linkage functions (component_canUnload, uno_initEnvironment, uno_ext_getMapping)
  in a simple DEF — no RTTI symbols (those are GCC-only in bridge_exports.map)
- Links: cppu3 + sal only (matches SHL1STDLIBS in makefile.mk)

### Notes for rdbmaker (done)
- rdbmaker has its own frozen copy of the codemaker API in `rdbmaker/inc/codemaker/`
  (older interface using TypeReader, not typereg::Reader — do NOT use //main/codemaker:codemaker_headers)
- No pch dir; private source headers accessed via copts `/Imain/rdbmaker/source/rdbmaker`

### Notes for xmlreader (done)
- Exports via `OOO_DLLIMPLEMENTATION_XMLREADER` / `SAL_DLLPUBLIC_EXPORT` — no DEF file needed
- Needs cppu_headers even though build.lst only lists `sal` (generated udkapi headers pull in cppu types)

### Notes for stoc (done)
- javavm/javaloader/jvmfwk/jvmaccess skipped — only C++ URE components needed
- `bootstrap.uno.dll`: 9 sub-components merged (bootstrap, security, servicemanager,
  simpleregistry, defaultregistry, implementationregistration, loader, registry_tdprovider, tdmanager)
- `stocservices.uno.dll`: stocservices + typeconv + uriproc
- `unistd.h` included unconditionally in implreg.cxx — satisfied by `main/soltools/winunistd/unistd.h` stub
  (add `/Imain/soltools/winunistd` to copts wherever this pattern appears)
- DEF exports: `component_getImplementationEnvironment`, `component_getFactory`, `component_canUnload`
  (standard UNO unloadable component pattern — same for all future component DLLs)

### Notes for comphelper (done)
- Deps: sal, cppu, cppuhelper, salhelper, ucbhelper, vos, udkapi, offapi, stlport, boost.legacy
- Requires `/Zc:wchar_t-` (sal_Unicode = unsigned short)
- Requires `snprintf=_snprintf` define (VS2008 MSVCRT only exports `_snprintf`)
- Private source headers: `/Imain/comphelper/source/inc`
- ucbhelper_implib filegroup added to ucbhelper BUILD.bazel in this session

### Notes for configmgr (done)
- Single DLL: `configmgr.uno.dll`
- Deps: sal, cppu, cppuhelper, salhelper, comphelper, xmlreader, udkapi, offapi, stlport, boost.legacy
- Requires `/Zc:wchar_t-` and stlport (uses boost::unordered_map, hash_map)
- Exports via SAL_DLLPUBLIC_EXPORT — no DEF file

### Notes for expat (done)
- @expat//:expat_utf8 (fastsax) and @expat//:expat_utf16 (expwrap)
- Bazel module at ext_libraries/modules/expat/2.5.0/
- Patch `aoo-vs2008.patch` (patch_strip=2): removes expat_config.h include on Windows,
  adds C89 compat fixes (variable declarations before code), isnan=_isnan, stdbool/stdint workarounds
- `expat_config.h` in tarball root is for Linux; the patch guards XML_DEV_URANDOM with `#if !_WIN32`
- winconfig.h patched to add XML_NS, XML_DTD, XML_CONTEXT_BYTES, BYTEORDER for Windows

### Notes for sax (done)
- 3 DLLs: sax.dll (tools), expwrap.dll (expat utf16), fastsax.dll (expat utf8)
- sax.dll: tools/converter, fastattribs, fastserializer, fshelper; links comphelper
- expwrap.dll: expatwrap/ sources; uses @expat//:expat_utf16
- fastsax.dll: fastparser/ + xml2utf.cxx; uses @expat//:expat_utf8; links sax.dll
- All need `/Zc:wchar_t-` and stlport

### Notes for zlib (done)
- @zlib//:zlib at ext_libraries/modules/zlib/1.3.2/
- gz*.c files excluded (gzread, gzwrite, gzclose use EWOULDBLOCK not in VS2008)
- Built with Z_PREFIX define (all symbols prefixed with z_)
- Consumers must also define Z_PREFIX and SYSTEM_ZLIB

### Notes for package (done)
- 2 DLLs: package2.dll (zip/ODF), xstor.dll (compound doc storage)
- package2 uses zlib (with Z_PREFIX + SYSTEM_ZLIB defines)
- Private headers in source/package/zippackage/, zipapi/, manifest/, xstor/ — all need /I copts
- Requires vos_headers for HashMaps.hxx (vos/ref.hxx)
- Requires /Zc:wchar_t- and stlport

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
            
