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

## Toolchain
- Custom MSVC toolchain at //build/toolchain (VS2008, x86)
  - windows_cc_toolchain_config.bzl: default_cpp_std disabled (no /std: flag), remove_unreferenced_code disabled (no /Zc:inline)
  - tool_bin_path = VC\bin (not msvc_env_path) sets PATH for actions
  - BAZEL_DO_NOT_DETECT_CPP_TOOLCHAIN=1 disables auto-detection

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
- never manipulate the cache directly, if you have to reset it use: bazel mod deps --lockfile_mode=refresh or ask the user for cleanup.
- add a migration summary to the module as readme.md
- let the user build
- update frontier after build is successfull.

## Cross-cutting compiler flags & defines
These apply to many packages — check before building any new module:

- `/Zc:wchar_t-` — required for any module using `sal_Unicode`; VS2008 native `wchar_t`
  differs from `unsigned short`, breaking `Sequence<sal_Unicode>` / `cppu_detail_getUnoType`
- `snprintf=_snprintf` — VS2008 MSVCRT only exports `_snprintf`, not `snprintf`
- `snwprintf=_snwprintf` — same for the wide-char variant; needed by framework and potentially others
- `stlport` dep — required for modules using `boost::unordered_map` or `hash_map` via boost
- `Z_PREFIX` + `SYSTEM_ZLIB` — required for all zlib consumers (all symbols prefixed with `z_`)
- `/Imain/soltools/winunistd` — for modules that `#include <unistd.h>` unconditionally
- UNO component DEF exports: `component_getImplementationEnvironment`, `component_getFactory`,
  `component_canUnload` (standard unloadable component pattern for all future component DLLs)
# dependency notes
icu - ext_libraries\modules\icu\Readme.md
redland - ext_libraries\modules\redland\README
rhino - ext_libraries\modules\rhino\README
sal_pch - was merged with sal_headers
## Current frontier

```
sal           ✅  — main/sal/readme.md
salhelper     ✅
store         ✅
registry      ✅
ucpp          ✅
idlc          ✅  (+ regmerge, cppumaker) — main/idlc/readme.md
udkapi        ✅  (.idl → .urd → .rdb → .hdl/.hpp via idl_pipeline.bzl)
cppu          ✅  (cppu3, purpenvhelper3MSC, affine/unsafe/log bridge DLLs)
cppuhelper    ✅  (cppuhelper3MSC.dll)
bridges       ✅  (msci_uno.dll) — main/bridges/readme.md
rdbmaker      ✅  (rdbmaker.exe) — main/rdbmaker/readme.md
xmlreader     ✅  (xmlreader.dll) — main/xmlreader/readme.md
stoc          ✅  (bootstrap.uno.dll, stocservices.uno.dll) — main/stoc/readme.md
jvmfwk        ✅  (jvmfwk.dll) — main/jvmfwk/readme.md
io            ✅  (streams.dll, acceptor.dll, connector.dll, textinstream.dll, textoutstream.dll) — main/io/readme.md
o3tl          ✅  (header-only)
vos           ✅  (vos3.dll)
libxslt       ✅  (@libxslt//:libxslt + :libexslt)
offapi        ✅  (offapi_idl.rdb + offapi_idl_inc/ — 3718 IDL files)
binaryurp     ✅  (binaryurp.dll)
remotebridges ✅  (uuresolver.dll)
cpputools     ✅  (regcomp.exe, regsingleton.exe, sp2bv.exe, uno.exe)
i18nutil      ✅  (i18nutil.dll)
ucbhelper     ✅  (ucbhelper.dll)
comphelper    ✅  (comphelp.dll) — main/comphelper/readme.md
configmgr     ✅  (configmgr.uno.dll) — main/configmgr/readme.md
expat         ✅  — ext_libraries/modules/expat/readme.md
sax           ✅  (sax.dll, expwrap.dll, fastsax.dll) — main/sax/readme.md
zlib          ✅  — ext_libraries/modules/zlib/readme.md
package       ✅  (package2.dll, xstor.dll) — main/package/readme.md
i18npool      ✅  (i18nisolang1.dll, i18npaper.dll, i18npool.dll, i18nsearch.dll) — main/i18npool/readme.md
python        ✅  (python27.dll) — ext_libraries/modules/python/readme.md
pyuno         ✅  (pyuno.dll, pythonloader.dll) — main/pyuno/readme.md
            │
            ▼
── Layer 1 (all deps done) ──────────────────────────────────────────────
basegfx       ✅  (o3tl, sal, offapi, cppu, cppuhelper) — main/basegfx/readme.md
codemaker     ✅  (registry) — main/codemaker/BUILD.bazel
oovbaapi      ✅  (offapi) — main/oovbaapi/BUILD.bazel
external      ✅  (pre-built DLL/lib filegroups) — main/external/BUILD.bazel
── Layer 2 ──────────────────────────────────────────────────────────────
tools         ✅  (tl.dll, ooopathutils.lib) — main/tools/BUILD.bazel
basebmp       ✅  (basegfx, sal + boost, vigra ext) — main/basebmp/BUILD.bazel
ridljar       ✅  (codemaker, udkapi — Java) — main/ridljar/readme.md
── Layer 3 ──────────────────────────────────────────────────────────────
rsc           ✅  (rscpp, rsc, rsc2) — main/rsc/readme.md
unotools      ✅  (utl.dll) — main/unotools/readme.md
xmlscript     ✅  (xcr.dll) — main/xmlscript/BUILD.bazel
shell         ✅  (syssh.uno, localebe1.uno, wininetbe1.uno, shlxthdl, ooofilt, propertyhdl, lngconvex, regsvrex) — main/shell/BUILD.bazel
l10ntools     ✅  (transex3, helpex, ulfex, gsiconv, gsicheck, cfgex, xrmex, localize_sl, HelpLinker, helplinker.dll) — main/l10ntools/BUILD.bazel
icc           ✅  (create_sRGB_profile.exe → sRGB-IEC61966-2.1.hxx, srgb_icc) — main\icc\readme.md
jurt          ✅  (ridljar, sal — Java; jpipe.dll + jpipx.dll native pipe JNI) — main/jurt/readme.md
jvmaccess     ✅  (jvmaccess.dll) — main/jvmaccess/readme.md
── Layer 4 ──────────────────────────────────────────────────────────────
sot           ✅  (tools, unotools, ucbhelper) — main/sot/readme.md
unoil         ✅  (codemaker, ridljar, offapi — Java) — main/unoil/readme.md
idl           ✅  (svidl.exe) — main/idl/readme.md
── Layer 5 ──────────────────────────────────────────────────────────────
svl           ✅  (svl.dll, fsstorage.dll, passwordcontainer.dll) — main/svl/readme.md
javaunohelper ✅  (juh.jar, juh.dll, juhx.dll) — main/javaunohelper/readme.md
── Layer 6 ──────────────────────────────────────────────────────────────
vcl           ✅  (rsc, sot, unotools, svl, basegfx, basebmp, tools, shell, l10ntools,
                   icc + boost, icu, graphite ext) — main/vcl/readme.md
xmloff        ✅  (xo.dll, xof.dll) — main/xmloff/readme.md
── Layer 7 ──────────────────────────────────────────────────────────────
linguistic    ✅  (svl, xmloff, ucbhelper, vos, comphelper + icu ext) — main/linguistic/readme.md
toolkit       ✅  (ootk.dll) — main/toolkit/readme.md
svtools       ✅  (svt.dll, hatchwindowfactory.dll) — main/svtools/readme.md
officecfg     ✅  (xsltproc pipeline: schema_trim + alllang, per-locale zips) — main/officecfg/BUILD.bazel
── Layer 8 ──────────────────────────────────────────────────────────────
framework     ✅  (fwi.dll, fwe.dll, fwk.dll, fwl.dll, fwm.dll) — main/framework/readme.md
editeng       ✅  (editeng.dll) — main/editeng/readme.md
canvas        ⬜  (svtools, vcl, basegfx, unoil, javaunohelper, comphelper + cairo ext)
basic         ⬜  (oovbaapi, svtools, xmlscript, framework, salhelper)
connectivity  ⬜  (shell, svl, unoil, javaunohelper, officecfg, comphelper)
── Layer 9 ──────────────────────────────────────────────────────────────
sfx2          ⬜  (basic, xmlscript, framework, shell, sax + libxml2 ext)
cppcanvas     ⬜  (canvas, vcl, basegfx, comphelper)
── Layer 10 ─────────────────────────────────────────────────────────────
avmedia       ⬜  (tools, sfx2)
oox           ⬜  (basegfx, xmlscript, tools, vcl, sax + boost, openssl ext)
── Layer 11 ─────────────────────────────────────────────────────────────
drawinglayer  ⬜  (vcl, svtools, basegfx, avmedia, canvas, cppcanvas)
── Layer 12 ─────────────────────────────────────────────────────────────
svx           ⬜  (sfx2, oovbaapi, connectivity, xmloff, linguistic, editeng,
                   avmedia, drawinglayer, jvmfwk)
── Layer 13 ─────────────────────────────────────────────────────────────
writerfilter  ⬜  (tools, svx, oox, cppu, cppuhelper, ucbhelper, sal)
filter        ⬜  (svtools, unotools, xmloff, svx, canvas, javaunohelper,
                   jvmaccess, basegfx, tools)
vbahelper     ⬜  (oovbaapi, basic, sfx2, svx, filter, vcl, svtools, tools, unotools)
── Layer 14 — TARGET ────────────────────────────────────────────────────
sw            ⬜  (filter, connectivity, vbahelper, svx, writerfilter, stoc)
── Layer 15 — .NET interop (deferred) ───────────────────────────────────
cli_ure       ⬜  (cppu, cppuhelper, sal, codemaker, stoc, udkapi, bridges)
                   Blockers: C# rules (csc.exe), C++/CLI (/clr toolchain),
                   AL.exe policy assemblies, sn.exe strong-name signing.
                   Not on critical path to sw — do after Layer 14.
unoil         ⬜  update climaker 
```

## Out of scope
- Modifying source code
