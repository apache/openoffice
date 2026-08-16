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
  adds a `_Container_proxy` to std::vector/hash_map, changing layout.  It MUST be identical across
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
