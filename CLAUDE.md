<!--
 Licensed to the Apache Software Foundation (ASF) under one
 or more contributor license agreements.  See the NOTICE file
 distributed with this work for additional information
 regarding copyright ownership.  The ASF licenses this file
 to you under the Apache License, Version 2.0 (the
 "License"); you may not use this file except in compliance
 with the License.  You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing,
 software distributed under the License is distributed on an
 "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 KIND, either express or implied.  See the License for the
 specific language governing permissions and limitations
 under the License.
-->

# Apache OpenOffice — Bazel Migration

Migrated packages have been moved to: Migrated-packages.md

## Current frontier

Demo done 2026-06-20 → goal is now FULL migration (see [[project-full-migration-goal]]).
Buckets below labelled "Remaining" are in-scope work, not abandoned; only
"Out of scope" and "Dropped" are excluded.  Ordered by priority — testing of
already-migrated modules is the active front-line task.

── Active: test coverage for already-migrated modules ───────────────────
test          🔨  C++ unit-test infra runnable — NOW THE FRONT-LINE TASK: bring up
                   per-module qa/ tests across every ALREADY-MIGRATED module and keep
                   them green as the migration proceeds (regression net for the rest).
                   @gtest 1.7.0 bzlmod wrap (built /Zc:wchar_t- to match sal_Unicode);
                   build/rules/gtest_test.bzl (svidl_bundle analog: stages /MD exe +
                   DLLs + CRT + external manifest into one dir or R6034); libtest
                   (test.dll) builds.  GREEN: o3tl_test (5), tools_pathutils,
                   //main/sal:sal_tests (22) via sal_qa_test macro, salhelper_test,
                   comphelper_test_string + comphelper_test_weakbag, sax_test_converter,
                   cppu_qa_{any,unotype,reference,recursion} (private types.idl → headers
                   via idl_library reuse), cppuhelper_tests (ifcontainer/unourl/weak),
                   binaryurp_tests (cache/unmarshal).  See main/test/readme.md.
                   LANDMINE (cost a whole session): the C++/UNO bridge DLL
                   (msci_uno/mscx_uno) is a RUNTIME dep nothing links — cppu
                   osl_loadModule()s it from any Mapping (getCaughtException etc.).
                   Unstaged ⇒ either a null mapping AV or R6034 → 0xC0000142 with an
                   EMPTY test.log; and the CRT manifest must be EMBEDDED (RT_MANIFEST
                   id 1, now linked into every gtest_test exe) not just staged as an
                   external <exe>.manifest, or late DLL loads fall outside the
                   activation context.  NEXT: sweep qa/ across the remaining migrated
                   modules.  DONE since: 8 sal osl/socket suites + shell_qa_zip.
                   "Needs a CppUnit external dep" was largely a MYTH — NOTHING checked
                   so far actually uses cppunit.  osl/socket and shell/qa are plain
                   GoogleTest (now wired); writerfilter/qa/cppunittests is misnamed —
                   only doctok is GoogleTest, the other 4 (odiapi/qname/sl/xxml) use
                   the RETIRED testshl harness, as does rtl_strings, which is also
                   superseded by qa/rtl/{ostring,oustring} — treat all of those as dead
                   weight, not blockers, and do not migrate them.  Two reusable hooks came
                   out of it, both in gtest_test: //build/testsupport:sal_process_init
                   (suites with a bare main() skip SAL_IMPLEMENT_MAIN ⇒ no WSAStartup
                   ⇒ every osl socket call is WSANOTINITIALISED) and data_files (a
                   staged fixture is NOT enough — bazel test's CWD is the execroot, so
                   relative opens need the cd-first .bat launcher).  NEXT: what is
                   genuinely left is not standalone — svl/qa/complex + svtools/qa/unoapi
                   + sfx2 + writerfilter/qa/complex are Java/UNO; svl/qa/test_URIHelper,
                   configmgr/qa/unit and cppuhelper/qa/propertysetmixin bootstrap a UNO
                   component context.  So the front line is now ONE fixture:
                   OfficeConnection (running-soffice).  test.dll is built and its
                   arg plumbing is rtl::Bootstrap "arg-soffice=path:…"/"arg-user=…",
                   so the missing piece is a rule that points it at //main/staging:install
                   and gives the test exe its own UNO bootstrap.
testtools     ⬜  (bridgetest — pure-C++ UNO bridge round-trip; cli/pyuno/java variants
                   need rules_java — see Java bucket)
qadevOOo      🔨  OOoRunner.jar built (//main/qadevOOo:OOoRunner — qadevOOo QA
                   framework, ~2137 classes; classpath ridl/unoil/jurt/juh_jar/
                   java_uno_jar; .csv objdsc NOT jarred, manifest omitted).
                   Unblocks bridges java_uno tests (acquire, java_remote).  STILL
                   ⬜: OOoRunnerLight, qa/complex, JunitTest_qadevOOo_unoapi,
                   testdocs — need running-soffice/OfficeConnection fixture.
testgraphical ⬜  (graphical/visual regression tests; needs instsetoo_native + qadevOOo)

── Remaining: Java-based ────────────────────────────────────────────────
NOTE: rules_java 8.11.0 IS now wired (MODULE.bazel) and the core Java UNO
runtime is migrated & green — ridljar/jurt/jvmaccess/javaunohelper/jvmfwk/
bridges (incl. the java_uno JNI bridge: java_uno.dll + java_uno.jar, done
2026-06-26)/stoc/io/remotebridges/unotools/unoil/pyuno.  The items below are
the higher-level Java apps/extensions still to do (no longer gated on rules_java
itself; per-module UNO-Java component/packaging rules may still be needed).
reportbuilder ⬜  (pure Java .oxt extension; blockers: JFreeReport suite not on Maven, SourceForge ZIPs have token-based URLs; wizards dep also deferred)
bean          ⬜  (Java bean component)
saxon         ⬜  (XSLT 2.0 processor — IN SCOPE for full migration.  Feeds the
                   Java xsltfilter UNO component (UOF/DocBook/user XSLT-2.0 filters);
                   without it those filters silently drop (upstream gates the whole
                   xsltfilter lib behind DISABLE_SAXON).  Leaf of the Java bucket —
                   only useful once Java/UNO-component rules exist, so sequence AFTER
                   rules_java enablement, not before.  saxon9.jar = 3rd-party tarball
                   source-9.0.0.7-bj (ant/build.xml) → http_archive + java build or
                   vendored jar.  When built, compile against the toolchain JDK's
                   javax.xml.stream — no bundled stax jar.  See Dropped: stax below.)
wizards       ⬜  (Java-based document wizards)
xmerge        ⬜  (document format converter, Java)
javainstaller2 ⬜ (Java installer UI)
swext         ⬜  (Writer Java extensions e.g. mediawiki)
unodevtools   ⬜  (UNO component inspector, Java)

── Remaining: .NET interop ──────────────────────────────────────────────
cli_ure       ⬜  (cppu, cppuhelper, sal, codemaker, stoc, udkapi, bridges)
                   Blockers: C# rules (csc.exe), C++/CLI (/clr toolchain),
                   AL.exe policy assemblies, sn.exe strong-name signing.
unoil         ⬜  update climaker

── Remaining: Installer/packaging ───────────────────────────────────────
instsetoo_native ⬜ (native Windows installer build)
setup_native  ⬜  (Windows setup UI)
packimages    ⬜  (image packaging for install)
sysui         ⬜  (system UI integration — mime types, desktop entries)
solenv        ⬜  (legacy build environment, mostly migrated away)

── Remaining: Docs/dev tooling ──────────────────────────────────────────
autodoc       ⬜  (API documentation generator)
odk           ⬜  (OpenDocument/Developer Kit)
helpauthoring ⬜  (help authoring tools)
helpcontent2  ⬜  (help content sources)
xml2cmp       ⬜  (XML component comparison tool)

── Remaining: Standalone/misc ───────────────────────────────────────────
automation    ⬜  (test automation/macro recorder framework)
migrationanalysis ⬜ (migration analysis tool)
more_fonts    ⬜  bundled fonts; blocked: SourceForge token URLs prevent http_archive;
                   OpenSymbol staged via extras; other fonts (DejaVu, Carlito…) pending
readlicense_oo ⬜  readme.html/txt; blocked: needs xsltproc + l10ntools merge

── Out of scope: Linux/macOS — not applicable to the Windows build ──────
padmin        —   Linux printer administration
sane          —   Linux scanner interface (SANE)
unixODBC      —   Linux ODBC bridge
x11_extensions —  X11/Linux platform extensions
psprint_config —  Linux PostScript/font config
apple_remote  —   macOS Apple Remote control
macOS         —   macOS-specific platform UI

── Dropped — do NOT migrate ─────────────────────────────────────────────
stax          ❌  StAX (JSR-173 / javax.xml.stream) has been part of the JDK since
                   Java SE 6; bundling/building stax-1.2.0.jar is dead weight.  Only
                   saxon ever depended on it (ooxml already uses the JDK's
                   javax.xml.stream directly).  Since Bazel replaces the dmake/configure
                   layer entirely (where the stax module lived), there is nothing to
                   migrate — saxon must just rely on the toolchain JDK's
                   javax.xml.stream.  Supersedes upstream PR apache/openoffice#87
                   (stax removal against dmake tree, now moot).

── Innovation (NOT a migration — no dmake equivalent to port) ───────────
release-identity 💡 Replaces the old configure --with-build-version="$(date) -
                   uname" + --with-vendor.  A wall-clock build string defeats
                   reproducible builds, so do NOT port it.  Build instead a
                   deterministic release identity:
                     • //build:channel string_flag (dev|beta|release, default dev)
                       — successor to --with-vendor; gates branding via
                       product.bzl → Setup.xcu (Apache OpenOffice vs …Dev/Snapshot).
                     • version = `git describe --tags` captured as a STABLE
                       workspace-status key (STABLE_*, rebuild-triggering); ties the
                       binary to an exact commit and only changes with source.
                     • keep volatile keys (BUILD_TIMESTAMP) OUT of release artifacts;
                       if a date is needed, derive SOURCE_DATE_EPOCH from the commit
                       date, never `date +%s`.
                     • the actual "valid release" proof = a deterministic buildinfo
                       provenance manifest (commit, channel, MSVC/SDK/Bazel versions),
                       optionally SLSA attestation — verifiable, unlike a timestamp.
                   Post-migration: needs the Installer/packaging bucket landed first
                   to have artifacts to stamp.  (The old --enable-win-x64-shellext is
                   NOT carried — it's a cross-bit hack that dissolves once the office
                   itself is ported to a 64-bit toolchain.)

```

## Trunk backports
Periodically port upstream trunk fixes that bazel-migration lacks.  METHOD:
`git cherry bazel-migration origin/trunk <merge-base>` — `-` marks commits ALREADY
present by patch-id (our own win64 + sal fixes went upstream, so they show `-`;
never re-apply those), `+` marks real candidates.  Then `git cherry-pick -x` so the
upstream hash is recorded in the message.

BACKPORT #1 LANDED 2026-07-26 (branch trunk-backports off bazel-migration, 14 commits,
GREEN per user build).  Two groups:
  • cross-platform bug fixes: package ZipPackageFolder (HSQLDB doc corruption+crash
    regression, 7f52a288cb), package ZipFile bounds check, extensions/update crash+
    uninit-return, sc dbdocimp check, ww8scan missing includes, ww8graf invalid-enum
    removal, connectivity jdbc Tools.java cyclic-cause recursion (Java, inert until
    the Java bucket lands).
  • Damjan's Win64 SOURCE fixes, complementary to our x64 port (NOT duplicates):
    ADO driver types (2), unixODBC/OFunctiondefs types (2), dbase Min() types,
    avmedia IMediaEvent::GetEvent, and winaccessibility amd64 (7d7b15ea9e — changes
    PUBLIC XMSAAService.idl long→hyper; offapi + UAccCOM MIDL regen; merged cleanly
    with our existing x64 salframe.cxx edits).
  DEVIATION: 9d1a529d5e's macOS-only s5abi_macosx_x86-64/cpp2uno.cxx hunk was DROPPED
  (depends on upstream 11db77d7e7 macOS x86 bridge rework, not carried here); only its
  extensions/update files were taken.  Noted in that commit's body.

BACKPORT #2 LANDED 2026-07-29 (idxf DXF import filter, SQUASHED to one commit).
Unlike #1 this work ORIGINATED here: it was developed on the local branch
dxf-filter-fix using Bazel purely as the fast iteration harness, went upstream as
apache/openoffice#491 (merge c082241a9b, 2026-07-28), and was then brought back from
trunk.  LANDMINE for the `git cherry` method above: because it was squashed, the 11
upstream commits have NO patch-id match here and WILL show as `+` candidates
(999d0e5d3c 22079cd995 65c7e1576f e345bd9383 030749d53e 82ba6d6560 c4b51fde1e
5ed424c6ce 3931960203 05e2156ffd 96b0524be2).  Do NOT re-apply them — they are
already in, verified tree-identical to trunk's post-PR state; the list also lives in
the squash commit's body.  Source-only, 11 files:
main/filter/source/graphicfilter/idxf/* (scaling/extents, arc+ellipse+spline geometry,
LWPOLYLINE bulge, linetype dots, HATCH patterns, 3D OCS/WCS + VPORT, $DWGCODEPAGE
encoding) + main/svtools/source/filter/filter.cxx (DXF detector tolerates leading 999
comments).  Root causes per defect: main/filter/source/graphicfilter/idxf/Readme.md.
  DEVIATION: the PR's .gitignore hunk was DROPPED — bazel-migration's .gitignore
  already covers bazel-*/user.bazelrc/.vscode, and trunk's `*.lock` + whole-dir
  `.claude/` are wrong here (MODULE.bazel.lock is tracked).
  NOT carried to AOO42X/AOO41X yet.  The 13.5 MB Bugzilla sample corpus and the
  still-open cases (99892 Phase B encoding-chooser UI, text-dense thumbnail) live on
  the local branch bugfix-dxf-filter-open, deliberately out of this tree.

DO NOT PORT (VC9 island landmines — see the trunk drift audit above): boost 1.84
(2e60be5056), pyuno→Python 3 (27dee00ef8), C++11 floor (7ce5b5df31), autoconf 2.72,
the macOS/Apple-Silicon batch, GTK/iODBC/lld/icc-Clang16.

STILL OPEN — dmake-only trunk commits with NO cherry-pick equivalent, but whose Win64
facts our Bazel targets may be missing: crashrep must link psapi on x64 (7e339f4bc5),
OpenSSL lib names carry an `-x64` suffix (2cc1933ab8 + 6a1563a7a3 CPUNAME in SCPDEFS),
coinmp x64 build flags (23448c919c), NSS/NSPR win64 patch (22f8d3ac55).  These are a
BUILD-file audit, not a port.

## Goal
Replace the Perl/dmake/gmake orchestration layer with Bazel.
Primary constraint: eliminate the Cygwin dependency from the build.
Source code is NOT being changed — only the build system.

## Environment
- OS: Windows 11
- Shell during migration work: bash (via Cygwin, being phased out)
- Compiler: MSVC VS2008 (VC9) — both **x86** and **x64** (VC9 cross-tools); configured via user.bazelrc
- Bazel configured via MODULE.bazel and user.bazelrc

## Build strategy
- Third-party deps: wrap with rules_foreign_cc (cmake/make as appropriate)
- First-party modules: migrate to native Bazel cc_library/cc_binary
- Goal: eliminate gmake and dmake from first-party builds entirely
- rules_foreign_cc is a bridge for external code, not a destination

## Toolchain — dual-arch (32-bit x86 + 64-bit x64), VS2008/VC9
- **Two** custom MSVC toolchains at //build/toolchain, both registered in MODULE.bazel:
  `cc_toolchain_x86_vs2008_def` (x86, the DEFAULT) and `cc_toolchain_x64_vs2008_def`
  (x64, VC9 cross-tools: `x86_amd64` cl/ml64 + SDK v7.0 `lib\x64`).  x86 stays the
  default build and must remain green; x64 is the Win64 port (boots to the Start Center).
- Arch is chosen by **target platform**, not a stringly-typed flag.  Toolchain
  resolution auto-picks the matching cc_toolchain from the platform:
  `//build/platforms:winXP-x86` (→ @platforms//cpu:x86_32, default) vs `winXP-x64`
  (→ x86_64), each composed with the `//build/constraints:win_version` constraint
  (target space = {winxp, win10} × {x86, x64}; a future win10 target is a new
  constraint value, not a new config).
- `.bazelrc` convenience configs: `--config=winXP-x86` (default) / `--config=winXP-x64`,
  each pinning `--platforms` + a distinct `--platform_suffix` (separate bazel-out
  trees, no clobber) + `--symlink_prefix` (`bazel-winXP-x86-` / `bazel-winXP-x64-`).
- Per-arch defines are injected **globally by the toolchain** (not per-module):
  x86 → `_X86_=1`/`INTEL`/`CPPU_ENV=msci`, x64 → `_AMD64_=1`/`X86_64`/`CPPU_ENV=mscx`;
  `WIN32` stays defined on BOTH arches (the x64 compiler auto-defines `_WIN64`).
- windows_cc_toolchain_config.bzl: default_cpp_std disabled (no /std: flag), remove_unreferenced_code disabled (no /Zc:inline)
- tool_bin_path = VC\bin (not msvc_env_path) sets PATH for actions
- BAZEL_DO_NOT_DETECT_CPP_TOOLCHAIN=1 disables auto-detection

## Key conventions
- BUILD-file authoring conventions (module skeleton, naming, idioms, anti-patterns):
  see STYLEGUIDE.md.  The rules below are the operating/landmine notes; STYLEGUIDE.md
  is the copy-paste pattern reference for new modules.
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
- KNOWN DIVERGENCE — component DLL naming: Bazel emits UNO component libs as bare
  <name>.dll where upstream/scp2 uses <name>.uno.dll (e.g. acceptor.dll vs
  acceptor.uno.dll, binaryurp/fastsax/expwrap/fpicker/fps_office/fsstorage/...).
  Harmless because the `.uno` infix has NO loader semantics: the SharedLibrary
  loader opens whatever name is registered in services.rdb, and both names are
  produced by the same Bazel pipeline so they stay consistent (proven by boot).
  Could only bite where a lib is named by a hardcoded string OUTSIDE generated
  services.rdb: remote-UNO/URP bootstrap (uno.exe -c acceptor), third-party .oxt
  manifests, or a deferred module's config referencing the canonical .uno.dll name
  → same "loading component library failed" class as fileacc/uui. To reach upstream
  parity (cosmetic): apply .uno infix in the link/staging step AND regenerate
  services.rdb together so both sides move in lockstep. Left as-is for the en-US demo.
- IDL pipeline: tools link /MD (dynamic CRT); stage msvcr90/msvcp90 DLLs +
  external .manifest files alongside EXEs (no mt.exe embedding); use
  ctx.actions.symlink (pure Bazel, no shell) for staging
- cppumaker output dir: prefix -O with "./" so osl's convertToFileUrl uses
  getAbsoluteFileURL (relative-to-workdir) instead of failing on relative paths
- never manipulate the cache directly, if you have to reset it use: bazel mod deps --lockfile_mode=refresh or ask the user for cleanup.
- add a migration summary to the module as readme.md
- let the user build
- update frontier after build is successful.
- XCU pipeline: LOCALIZED XCU files (those containing <value xml:lang="...">) must be pre-processed
  by alllang.xsl (no locale, no module) before being packed into XCDs — matches dmake XCU_DEFAULT
  pipeline in tg_config.mk.  Without this, configmgr crashes on empty xs:boolean values.
  Implemented via alllang_default_<xcu> genrules in officecfg/BUILD.bazel and _LOCALIZED_XCUS
  routing in postprocess.bzl's oc_xcu() function.  Both lists must stay in sync.
- forcedefault.xcd / ${PRODUCTLANGUAGE}: the ForceDefaultLanguage spool sets
  Linguistic/General/UILocale to the literal installer placeholder ${PRODUCTLANGUAGE},
  which the MSI (installer/languages.pm → $officestartlanguage) would substitute.
  Bazel staging never runs the MSI, so a genrule (postprocess/BUILD.bazel
  forcedefault_linguistic_xcu) substitutes it to en-US before pack_registry.
  Without it: first-start (langselect.cxx) copies the unexpanded placeholder into
  Setup/L10N/ooLocale, dp_resource.cxx::toLocale() throws "Invalid language string.",
  surfacing as '[context="user"] caught unexpected exception' + startup FatalError.
  When parameterize languages (see localization section) this literal becomes the knob.
  - uiconfig (UI config) MUST be staged as a FOLDER TREE at share/config/soffice.cfg/,
  not a zip.  framework PresetHandler (presethandler.cxx) opens soffice.cfg as a folder
  (FileSystemStorageFactory) and reads modules/<ModuleShortName>/<restype>/*.xml; dmake's
  scp2 ARCHIVE style extracts uiconfig.zip on install but Bazel skips that.  Missing folder
  ⇒ CorruptedUIConfigurationException ⇒ "error loading user interface configuration data"
  FatalError (Start Center = modules/startmodule loads first).  Implemented via uiconfig_tree
  rule in postprocess.bzl (//main/postprocess:uiconfig_tree) + staging _install_uiconfig;
  source <module>/uiconfig/<short>/... → modules/<short>/..., chart2 → modules/schart.

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
- `_HAS_ITERATOR_DEBUGGING=0` — set GLOBALLY in build/toolchain/windows_cc_toolchain_config.bzl
  (default_compile_flags_list).  This is an STL-ABI macro: at =1 (the /MDd _DEBUG default) MSVC
  adds a _Container_proxy to std::vector/hash_map, changing layout.  It MUST be identical across
  every DLL or a container built in one (e.g. comphelper::SequenceAsHashMap, exported from
  comphelp.dll) and read inline in another (e.g. desktop/spl FirstStart::execute) indexes a
  garbage bucket → "vector subscript out of range" assert (debug only; release defaults to 0 so
  it never showed).  Cannot be =1: ~8 modules (comphelper, connectivity, sw, oox, fpicker, tools,
  svtools, pyuno) don't compile at =1 (heterogeneous comparators).  Global =0 matches upstream
  solenv; the per-module /D_HAS_ITERATOR_DEBUGGING=0 in those BUILD files are now redundant.
- rsc_res `images_root` MUST be `"main/default_images"` for every module that uses
  `Bitmap { File = "xxx.png" }` resources.  `BitmapEx(ResId)` reads the filename from .res and
  looks it up by exact name in `images.zip`.  `images.zip` uses `strip_prefix="main/default_images"`
  so entries are stored as `"framework/res/backing.png"`, `"res/odt_32.png"`, etc.  Setting
  `images_root` to a module-specific subdirectory (e.g. `"main/default_images/framework/res"`)
  stages images flat → .res stores bare basenames → exact-name lookup always fails → all
  `BitmapEx(ResId)` loads return empty bitmaps → Start Center shows no buttons or text.
## Localization / language builds (planned — not yet wired)
Current staging is en-US only.  This is the deliberate demo baseline; AOO must
start first.  When adding languages, keep THREE independent axes distinct
(conflating them is the trap) — dmake drives them from separate variables:

1. Installed locales (which UI languages are SELECTABLE) — dmake `alllangiso`/
   `WITH_LANG`, a LIST.  Each lang gets a Langpack-<lang>.xcd registering it in
   Setup/Office/InstalledLocales (+ fcfg_langpack_<lang>.xcd, registry_<lang>.xcd).
   Bazel today: only langpack_en_us_xcd.  Future: a macro over OOO_LANGS emitting
   one pack_registry per lang, each added to all_xcd via
   select({"//build:build_<code>": [...], "//conditions:default": []}) so
   --//build:lang_de=True auto-includes Langpack-de.xcd.  Mirrors the
   {$(alllangiso)} brace-expansion in postprocess/packregistry/makefile.mk.
2. Default UI language (the ONE start language) — dmake `PRODUCTLANGUAGE`, a
   SCALAR (installer/languages.pm $officestartlanguage; NOT the list).  This is
   the forcedefault.xcd ${PRODUCTLANGUAGE} substitution above.  Future: replace
   the hardcoded 'en-US' with a make-var, e.g. --define=office_start_lang=de
   (default en-US) read in the forcedefault_linguistic_xcu genrule cmd.
3. Localized content (the big lift) — per-lang .src/.res (rsc pipeline),
   localized .xcu (oc_xcu alllang spool), help, autotext, wordbook.  Same
   select()-per-lang pattern, but needs the TRANSLATION DATA wired in (see SDF).

Existing scaffolding (unconsumed): build/langs.bzl (OOO_LANGS, lang_id),
build/BUILD.bazel emits //build:lang_<code> bool_flags (en-US default True) and
//build:build_<code> config_settings.  user.bazelrc can flip lang_de/lang_fr;
nothing reads them yet.

### SDF files and the missing Pootle→SDF rule
Translation data flows as SDF (a.k.a. GSI) — the build's merge-database format,
one TAB-delimited line per translatable string
(project\path\file\type\gid\lid\helpid\platform\width\langid\text\...).
- Consumption (merge): l10ntools `transex`/export.cxx and friends read a source
  file + an SDF via `-m <sdf> -l <lang>` and emit the localized resource
  (the merge step before rsc compiles .src→.res, and for helpex/cfgex/xrmex).
  l10ntools localize.cxx does the reverse — EXTRACTS source strings into one
  merged .sdf (POT-equivalent) for translators.
- The gap: Pootle stores translations as .po (per lang/module).  AOO's build
  consumes .sdf, NOT .po.  The .po↔.sdf bridge is translate-toolkit's
  oo2po (sdf→po, to seed/update Pootle) and po2oo (po→sdf, to feed the build).
  Neither translate-toolkit nor any po/sdf conversion exists in this tree
  (grep: no oo2po/po2oo/po2sdf).  So axis 3 needs a NEW Bazel rule that runs
  po2oo over the Pootle .po export to (re)generate the per-lang .sdf the merge
  tools expect — this rule does not exist yet and is a prerequisite for any
  real (translated) language build.  Until then, language builds can only do
  axes 1+2 (German/French DEFAULT UI, but strings still English).

# dependency notes
icu - ext_libraries\modules\icu\Readme.md
redland / raptor2 / rasqal (RDF stack, native-cc static) + unordf.dll - rdf-readme.md
  (the modules\<lib>\README files are the original autotools docs; the Bazel
   migration notes — overlay mechanism, *_INTERNAL/HAVE_CONFIG_H defines,
   local_defines vs defines, *_STATIC, RAPTOR_WWW_NONE/S_ISREG — are in rdf-readme.md)
rhino - ext_libraries\modules\rhino\README
sal_pch - was merged with sal_headers
## Out of scope
- Modifying source code
