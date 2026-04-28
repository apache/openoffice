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
- `CURL_STATICLIB` — required for any target using `@curl//:curl`; without it curl.h uses `__declspec(dllimport)` which breaks static linking
- MASM `.asm` files: list directly in `srcs`; toolchain `assemble` action uses `ml.exe` with
  `masm_flags` feature (`/c /coff /Cx`). `/Cx` is critical — without it MASM uppercases all
  PUBLIC symbols, breaking the link. Build-system define `SUPD=680` (Solar Update version).
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
canvas        ✅  (canvastools, canvasfactory, nullcanvas, simplecanvas, vclcanvas, directx9canvas, gdipluscanvas) — main/canvas/readme.md
basic         ✅  (oovbaapi, svtools, xmlscript, framework, salhelper)
connectivity  ✅  (dbtools.dll, sdbc2.dll, dbpool2.dll) — main/connectivity/readme.md
── Layer 9 ──────────────────────────────────────────────────────────────
sfx2          ✅  (basic, xmlscript, framework, shell, sax + libxml2 ext) — main/sfx2/readme.md
cppcanvas     ✅  (cppcanvas.dll) — main/cppcanvas/readme.md
── Layer 10 ─────────────────────────────────────────────────────────────
avmedia       ✅  (avmedia.dll, avmediawin.dll) — main/avmedia/readme.md
── Layer 11 ─────────────────────────────────────────────────────────────
drawinglayer  ✅  (vcl, svtools, basegfx, avmedia, canvas, cppcanvas)
── Layer 12 ─────────────────────────────────────────────────────────────
svx           ✅  (svxcore.dll, svx.dll, textconversiondlgs.dll) — main/svx/readme.md
msfilter      ✅  (msfilter.dll — prereq for oox export) — main/filter/BUILD.bazel
oox           ✅  (oox.dll) — main/oox/readme.md
── Layer 13 ─────────────────────────────────────────────────────────────
writerfilter  ✅  (writerfilter.dll, writerfilter_debug.dll) — main/writerfilter/readme.md
filter        ✅  (xmlfa, xfld, svgfilter, pdffilter, flash, placeware, t602filter,
                   filterconfig1, xsltfilter, xsltdlg, filtertracer + 22 graphic
                   filter DLLs) — main/filter/readme.md
vbahelper     ✅  (vbahelper.dll, msforms.dll) — main/vbahelper/readme.md
── Layer 14 — pre-app infrastructure ─────────────────────────────────────
ucb           ✅  (ucb1, cached1, srtrs1, ucpexpand1, ucpext, ucpfile1, ucphier1,
                   ucppkg1, ucptdoc1, ucpftp1, ucpdav1) — main/ucb/readme.md
unoxml        ✅  (unoxml.dll — DOM/XPath/Events; unordf deferred: redland not yet on Windows) — main/unoxml/readme.md
eventattacher ✅  (evtatt.dll) — main/eventattacher/readme.md
UnoControls   ✅  (ctl.dll) — main/UnoControls/readme.md
dtrans        ✅  (dtrans.dll, mcnttype.dll, dnd.dll, ftransl.dll, sysdtrans.dll) — main/dtrans/readme.md
svgio         ✅  (svgio.dll) — main/svgio/readme.md
lingucomponent ✅ (spell.dll, hyphen.dll, lnth.dll, guesslang.dll) — main/lingucomponent/readme.md
fpicker       ⬜  (vcl, toolkit, svtools — file picker dialog)
mysqlc        ⬜  (connectivity — MySQL/MariaDB native connector)
── Layer 15 ─────────────────────────────────────────────────────────────
formula       ⬜  (svl, svx, tools, i18npool — formula bar/parser)
animations    ⬜  (vcl, comphelper, tools — drawing layer animations)
chart2        ⬜  (svx, xmloff, editeng, vcl, oox, filter, formula)
embeddedobj   ⬜  (sfx2, svx, tools — OLE embedded object framework)
xmlsecurity   ⬜  (xmloff, sfx2, tools, libxmlsec — XML digital signatures)
uui           ⬜  (svtools, vcl, toolkit, sfx2 — user interaction/auth dialogs)
fileaccess    ⬜  (ucb, comphelper — file access UNO service)
accessibility ⬜  (vcl, toolkit, svx, cppu — a11y UNO API)
── Layer 16 ─────────────────────────────────────────────────────────────
forms         ⬜  (svx, toolkit, vcl, formula, connectivity — form controls)
scripting     ⬜  (sfx2, basic, vcl, comphelper — macro scripting framework)
embedserv     ⬜  (embeddedobj, sfx2, tools — OLE embedding server)
winaccessibility ⬜ (accessibility, vcl — Windows a11y bridge)
── Layer 17 ─────────────────────────────────────────────────────────────
cui           ⬜  (svx, svtools, toolkit, sfx2, vcl, editeng, chart2, scripting
                   — common UI dialogs shared by all apps)
── Layer 18 — sw ─────────────────────────────────────────────────────────
sw            ✅  (swd.dll + sw.dll) — main/sw/README.md
── Layer 19 ─────────────────────────────────────────────────────────────
basctl        ⬜  (basic, scripting, sfx2, sw — Basic IDE)
sd            ⬜  Draw/Impress (sw, svx, sfx2, filter, oox, avmedia, canvas,
                   cppcanvas, chart2, animations, embeddedobj, forms, svgio, cui)
── Layer 20 ─────────────────────────────────────────────────────────────
slideshow     ⬜  (sd, canvas, cppcanvas, vcl, svx — Impress slideshow engine)
sdext         ⬜  (sd — PDF import, Presenter Console, minimizer)
sc            ⬜  Calc (sw, svx, sfx2, filter, oox, connectivity, vbahelper,
                   chart2, formula, forms, scripting, embeddedobj, cui, ucb)
── Layer 21 ─────────────────────────────────────────────────────────────
scaddins      ⬜  (sc, tools, comphelper — Calc add-in infrastructure)
sccomp        ⬜  (sc, connectivity — Calc compatibility components)
solver        ⬜  (sc — linear programming solver)
dbaccess      ⬜  Base (sc, sw, connectivity, svx, sfx2, svtools, xmloff,
                   ucb, forms, cui)
── Layer 22 ─────────────────────────────────────────────────────────────
reportdesign  ⬜  (dbaccess, svx, sfx2, sc, vcl — Base report designer)
reportbuilder ⬜  (reportdesign, dbaccess — Java-based report engine)
starmath      ⬜  (svx, sfx2, editeng, vcl, sw — Math formula editor)
── Layer 23 — desktop ────────────────────────────────────────────────────
desktop       ⬜  (sw, sd, sc, dbaccess, starmath, sfx2, vcl, framework,
                   ucb, scripting, basctl, fpicker, dtrans, uui, accessibility
                   — soffice.exe launcher + splash, quickstarter)
── Layer 24 — .NET interop (deferred) ────────────────────────────────────
cli_ure       ⬜  (cppu, cppuhelper, sal, codemaker, stoc, udkapi, bridges)
                   Blockers: C# rules (csc.exe), C++/CLI (/clr toolchain),
                   AL.exe policy assemblies, sn.exe strong-name signing.
unoil         ⬜  update climaker
── Already done, not yet listed ─────────────────────────────────────────
ooxml         ✅  (done with oox migration; openssl_shim removed, now uses @openssl//:ssl)
stlport       ✅  (boost hash_map shim, referenced as dep)
── Deferred: Java-based ─────────────────────────────────────────────────
bean          ⬜  (Java bean component)
stax          ⬜  (StAX XML streaming API)
saxon         ⬜  (XSLT 2.0 processor)
wizards       ⬜  (Java-based document wizards)
xmerge        ⬜  (document format converter, Java)
javainstaller2 ⬜ (Java installer UI)
swext         ⬜  (Writer Java extensions e.g. mediawiki)
unodevtools   ⬜  (UNO component inspector, Java)
── Deferred: Linux/macOS — not applicable for Windows build ─────────────
padmin        —   Linux printer administration
sane          —   Linux scanner interface (SANE)
unixODBC      —   Linux ODBC bridge
x11_extensions —  X11/Linux platform extensions
psprint_config —  Linux PostScript/font config
apple_remote  —   macOS Apple Remote control
macOS         —   macOS-specific platform UI
── Deferred: Installer/packaging — post-build ────────────────────────────
scp2          ⬜  (installer package/script definitions)
instsetoo_native ⬜ (native Windows installer build)
setup_native  ⬜  (Windows setup UI)
packimages    ⬜  (image packaging for install)
sysui         ⬜  (system UI integration — mime types, desktop entries)
postprocess   ⬜  (post-build registration, rdb merging)
solenv        ⬜  (legacy build environment, mostly migrated away)
── Deferred: Docs/tests/dev tooling ─────────────────────────────────────
autodoc       ⬜  (API documentation generator)
odk           ⬜  (OpenDocument/Developer Kit)
helpauthoring ⬜  (help authoring tools)
helpcontent2  ⬜  (help content sources)
xmlhelp       ⬜  (XML help runtime)
xml2cmp       ⬜  (XML component comparison tool)
qadevOOo      ⬜  (Java-based QA test framework)
test          ⬜  (C++ unit tests)
testgraphical ⬜  (graphical/visual regression tests)
testtools     ⬜  (test infrastructure)
── Deferred: Standalone/misc ─────────────────────────────────────────────
crashrep      ⬜  (crash reporter)
automation    ⬜  (test automation/macro recorder framework)
migrationanalysis ⬜ (migration analysis tool)
extensions    ⬜  (misc UNO extension infrastructure)
extras        —   templates and samples (data, not build target)
more_fonts    —   bundled fonts (data, not build target)
default_images —  bundled images (data, not build target)
ooo_custom_images — custom branding images (data, not build target)
readlicense_oo —  license files (data, not build target)
```

## Out of scope
- Modifying source code
