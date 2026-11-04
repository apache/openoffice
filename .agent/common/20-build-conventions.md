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

# Key conventions

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
- VC90 CRT manifests are a VC9 artifact and the VC9 toolchains are retired
  (2026-11-01).  The modern runtime (vcruntime140/UCRT) is not side-by-side, so
  a new EXE needs no manifest for the CRT.  The VC9-era lesson still holds for
  anything side-by-side: a `/MANIFEST:NO` EXE whose import needs an activation
  context dies before main() with a "System Error" box, and nothing in the build
  notices -- 15 launchers sat broken that way until 2026-10-08.  REMAINING
  CLEANUP: ~25 rules still stage the VC90 DLLs or embed
  `//main/external/msvcp90:vc90_app_manifest_res` unconditionally (idl/rsc/
  merge_rdb/java/localedata pipelines, gtest_test, registry, cpputools, pyuno,
  staging); on VS2019 binaries these are dead weight, not a requirement.
- IDL pipeline: tools link /MD (dynamic CRT); stage msvcr90/msvcp90 DLLs +
  external .manifest files alongside EXEs (no mt.exe embedding); use
  ctx.actions.symlink (pure Bazel, no shell) for staging
- cppumaker output dir: prefix -O with "./" so osl's convertToFileUrl uses
  getAbsoluteFileURL (relative-to-workdir) instead of failing on relative paths
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
  rule in postprocess.bzl (//main/postprocess:uiconfig_tree) + staging `_install_uiconfig`;
  source <module>/uiconfig/<short>/... → modules/<short>/..., chart2 → modules/schart.

## Cross-cutting compiler flags & defines

These apply to many packages — check before building any new module:

- `/Zc:wchar_t-` — required for any module using `sal_Unicode`; VS2008 native `wchar_t`
  differs from `unsigned short`, breaking `Sequence<sal_Unicode>` / `cppu_detail_getUnoType`
- `snprintf` / `snwprintf` — NO LONGER a per-module define.  Both are injected by
  the toolchain through the `crt_defines` attr (`build/toolchain/BUILD.bazel`),
  because which CRT is in play is a toolchain property that a module BUILD cannot
  know.  The 130 per-module copies were swept out 2026-08-11.  The two halves are
  not symmetric: the retired VC9 toolchains got `snprintf=_snprintf` (its MSVCRT
  exports only the underscored name) AND `snwprintf=_snwprintf`; the modern
  toolchain gets ONLY the wide one — the UCRT declares a real `snprintf` and
  refuses to compile with the name taken (C1189), while it declares no
  `snwprintf` at all, since the wide C99 name never existed in any MSVC CRT.
- `MSC` — likewise global, hardcoded in windows_cc_toolchain_config.bzl (it names
  the compiler, and every toolchain sharing that config rule is MSVC).  Upstream
  defines it for every TU on MSVC (`solenv/inc/settings.mk:878`, the same line
  `$(CPUNAME)` and `CPPU_ENV` come from).  26 source files test it and three
  modules tested it WITHOUT defining it, silently taking the non-Windows branch —
  live in `tools/source/fsys/dirent.cxx`, where temp filenames came from
  `clock()`+`getpid()` instead of `GetTickCount()`+`_getpid()`.  It flips live
  code tree-wide and appears on every command line, so per-module builds cannot
  check it; validated by a full green `//main/staging:install --config=winXP-x86`
  (the then-default VC9 build).
- `stlport` dep — required for modules using `boost::unordered_map` or `hash_map` via boost
- `Z_PREFIX` + `SYSTEM_ZLIB` — required for all zlib consumers (all symbols prefixed with `z_`)
- `/Imain/soltools/winunistd` — for modules that `#include <unistd.h>` unconditionally
- UNO component DEF exports: `component_getImplementationEnvironment`, `component_getFactory`,
  `component_canUnload` (standard unloadable component pattern for all future component DLLs)
- `CURL_STATICLIB` — required for any target using `@curl//:curl`; without it curl.h uses `__declspec(dllimport)` which breaks static linking
- MASM `.asm` files: list directly in `srcs`; toolchain `assemble` action uses `ml.exe` with
  `masm_flags` feature (`/c /coff /Cx`). `/Cx` is critical — without it MASM uppercases all
  PUBLIC symbols, breaking the link. Build-system define `SUPD` (Solar Update version) is `SUPD_DEFINE` from
  `@product_info` — never a literal; see the product-version bullet below.
- PRODUCT VERSION — set in ONE place, MODULE.bazel `module(version = "x.y.z")`,
  and distributed from there; no BUILD, .bzl or ini file may carry a copy.
  `//build:product.bzl` derives every dependent value into `@product_info`
  (`PRODUCT_VERSION`, `PRODUCT_BASE_VERSION`, `PRODUCT_USERDIR_VERSION`,
  `PRODUCT_KEY`, `PRODUCT_SUPD`, `SUPD_DEFINE`).  Text files take them through
  `expand_template` (`main/staging/{bootstrap,fundamental}.ini.in`).  Before
  2026-11-03 copies had drifted: SUPD=680 in 28 BUILD files (upstream SUPD is
  RSCVERSION, e.g. 510 — live in Basic `GetSolarVersion()` and the office pipe
  name), ProductKey "4.1", the profile folder "4bazel", odk "4.5.0".
- `_HAS_ITERATOR_DEBUGGING=0` — set GLOBALLY in build/toolchain/windows_cc_toolchain_config.bzl
  (default_compile_flags_list).  This is an STL-ABI macro: at =1 (the /MDd _DEBUG default) MSVC
  adds a `_Container_proxy` to std::vector/hash_map, changing layout.  It MUST be identical across
  every DLL or a container built in one (e.g. comphelper::SequenceAsHashMap, exported from
  comphelp.dll) and read inline in another (e.g. desktop/spl FirstStart::execute) indexes a
  garbage bucket → "vector subscript out of range" assert (debug only; release defaults to 0 so
  it never showed).  Cannot be =1: ~8 modules (comphelper, connectivity, sw, oox, fpicker, tools,
  svtools, pyuno) don't compile at =1 (heterogeneous comparators).  Global =0 matches upstream
  solenv; the per-module /D_HAS_ITERATOR_DEBUGGING=0 in those BUILD files are now redundant.
- rsc_res `images` must cover EVERY image its .src names: since 2026-10-08 the rule
  runs upstream's rsc2 -s normalisation, so the one-line `Bitmap { File = ...; }`
  form is resolved too, and an image it cannot find fails the build ("The
  image(s) <...> could not be found").  Take the set from upstream's
  `RESLIBnIMAGES` (dmake) or `set_reslocation` (gbuild) plus the global
  `//main/default_images:shared_images`.  images.zip is built from the `.ilst`
  each rsc_res declares (output group `ilst`) — see main/packimages/readme.md.
- rsc_res `images_root` MUST be `"main/default_images"` for every module that uses
  `Bitmap { File = "xxx.png" }` resources.  `BitmapEx(ResId)` reads the filename from .res and
  looks it up by exact name in `images.zip`.  `images.zip` uses `strip_prefix="main/default_images"`
  so entries are stored as `"framework/res/backing.png"`, `"res/odt_32.png"`, etc.  Setting
  `images_root` to a module-specific subdirectory (e.g. `"main/default_images/framework/res"`)
  stages images flat → .res stores bare basenames → exact-name lookup always fails → all
  `BitmapEx(ResId)` loads return empty bitmaps → Start Center shows no buttons or text.
