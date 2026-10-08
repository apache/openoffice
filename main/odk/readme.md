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

# Notes for odk (in progress)

The Office Development Kit (SDK).  Upstream builds it in three layers
(`prj/build.lst`); this migration is taking them one at a time.

| Layer | Upstream | State |
|---|---|---|
| What odk COMPILES | `source/` | **done 2026-10-08**, all three configs |
| The SDK tree and its zips | `pack/copying`, `util` | **assembled 2026-10-08** (minus the autodoc/CLI-blocked parts) |
| The generated API reference | `pack/gendocu` (autodoc + javadoc) | IDL reference done 2026-10-08; C++/Java references + packaging pending |

## What is built

| Target | Upstream | What it is |
|---|---|---|
| `:loader_classes` | `source/com/sun/star/lib/loader` | `com.sun.star.lib.loader.*` as loose classes, `--release 8`.  An SDK client ships them in its own jar; they find an office and put its UNO jars on a class loader. |
| `:unowinreg` | `source/unowinreg/win` | `unowinreg.dll`, the JNI half of `WinRegKey`. |
| `:unoapploader` | `source/unoapploader/win` | `unoapploader.exe`.  A client copies it beside its own exe; it finds the office, puts it on `PATH` and starts `_<its own name>.exe`. |
| `:uno_loader_classes` | `pack/copying` `MYZIPTARGET` | `uno_loader_classes.zip`: `com/...` + `win/unowinreg.dll`, delivered by `prj/d.lst`. |

## Landmines

- **Both native targets link the static CRT**, as upstream (`DYNAMIC_CRT=`
  empty for unoapploader, `STDSHL=` + `LIBCMT` for unowinreg): they run in
  processes the office does not own (a client's java.exe, a client's launcher),
  before any office or VC runtime has been found.  So they need no manifest, and
  `features = ["static_link_msvcrt"]` instead.
- **`_SNPRINTF_DLLIMPORT=` (empty) on those targets.**  `sal/config.h` includes
  `systools/win32/snprintf.h`, which declares `snprintf` (renamed `_snprintf` by
  the VC9 toolchain's `crt_defines`) as `__declspec(dllimport)` -- right for the
  DLL CRT only.  Against libcmt that is C2375.  The empty macro is the header's
  own switch; no source change.
- **unowinreg exports the undecorated `Java_*` names** through
  `util/unowinreg.def` (= `unowinreg.dxp`).  On x86 the functions are
  `__stdcall`, and link.exe binds each DEF name to its `_Java_...@N` symbol --
  the jurt `jpipe.def` trick.  Verified by actually loading it in a 32-bit JVM.
- **`WinRegKey` loads the DLL out of the zip**: it reads the resource
  `win/unowinreg.dll` from its own class loader and extracts it to a temp file.
  So the zip's layout is the interface, not cosmetic.
- `findsofficepath.c` is cppuhelper's (exported from //main/cppuhelper), compiled
  a second time here -- upstream delivers its `.obj` alone for exactly this.
- `javac_classes` now roots `-d` at the class-path root, so packaged classes
  land at their declared `com/sun/.../X.class` paths (it used to take the first
  output's directory, which only worked for a default-package class).

## Verified (2026-10-08)

On all three configs, with the matching JVM (32-bit Temurin 8 for x86, the
build JDK 21 for x64):

- `WinRegKey` reads `HKLM\...\Windows NT\CurrentVersion\ProductName` through the
  DLL extracted from `uno_loader_classes.zip`.
- `java -cp uno_loader_classes.zip;client com.sun.star.lib.loader.Loader
  UnoClient` with `UNO_PATH` at the staged `program/`: the client loads
  `com.sun.star.uno.UnoRuntime` from the office's `classes/ridl.jar`.
- `unoapploader.exe` installed as `foo.exe` beside a `_foo.exe` starts it with the
  staged `program/` first on `PATH`.

## pack/copying — the SDK tree and its zips (2026-10-08)

`pack/copying` is a file list more than a build, so it is assembled with the
existing `ext_files`/`oxt_package` rules (archive paths, no on-disk tree) rather
than a new rule.  Two deliverables, both in `//main/odk`:

- **`:odkcommon`** → `odkcommon.zip`, the whole SDK tree (`util` PRODUCTZIPFILE).
- **`:odkexamples`** → `odkexamples.zip`, the example SOURCES (`util` ZIP1TARGET).

What each part maps to (the lists are pack/copying's `makefile.mk` +
`util/makefile.pmk`):

| Tree path | Source |
|---|---|
| `bin/` | `//main/codemaker:{cppumaker,javamaker}`, `//main/idlc:idlc`, `//main/registry:regcompare` (**new target**), `//main/unodevtools:uno-skeletonmaker`, `@ucpp//:ucpp`, `:unoapploader` |
| `lib/` | each DLL's `<stem>.if.lib` renamed to the SDK `i<name>.lib` (`isal`, `isalhelper`, `icppu`, `ipurpenvhelper3MSC`, `icppuhelper`, `istore`, `ireg`) |
| `include/` | each owning module's new `:sdk_headers` filegroup (the exact SDK subtree, no `inc/pch` or `inc/internal`), re-rooted `inc/<dir>` → `include/<dir>`; `include/stl` = stlport's systemstl shim; `include/udkversion.mk` from solenv |
| `idl/` | `//main/udkapi:sdk_idl` + `//main/offapi:sdk_idl` (new filegroups: the raw `.idl` source, the full tree — the compile-time exclusions are an idlc batching concern, not a delivery one) |
| `classes/` | `:loader_classes` + `win/unowinreg.dll` |
| `settings/` | `settings/*.mk` + `component.uno.def`; `dk.mk` with `@@RELEASE@@` substituted |
| `docs/`, root scripts | docs html (`%PRODUCT_RELEASE%`), css, images; `index.html`; `setsdkenv_windows.bat` (`__SDKNAME__`), `.template`, `setsdkname.bat`, `cfgWin.js` |

### Deliberately absent (not guessed)

- **`bin/autodoc` + `docs/common/ref`** (the generated IDL reference, and the
  `module-ix.html` index): blocked on the unmigrated `autodoc`.
- **`cli/` + `bin/climaker`**: blocked on `cli_ure`/.NET (see the frontier's
  .NET-interop bucket).
- **`lib/irmcxt.lib`**: *no module in this source tree produces `rmcxt`.*  The
  only trace is `bridges/version.mk`'s `RMCXT_TARGET=rmcxt`, a version stamp
  nothing consumes; `bridges` delivers no `.lib`.  Shipped without it rather
  than mapping it to the wrong DLL — an open question to resolve (or confirm it
  is dead) before the SDK's own `pack/check` content check can pass.
- `dk.mk`'s `@@BUILDID@@`/`@@SCMREVISION@@` are left empty: a wall-clock/VCS
  stamp defeats a reproducible build (see the frontier's release-identity note),
  and they are informational only.

### Open items

- `pack/check*` (checkbin/checkidl/checkinc) is the upstream completeness
  oracle; not yet ported.  Until it is, the gaps above are the known deltas.
- CRLF: `ext_files` byte-copies, where upstream `MY_TEXTCOPY` strips `\r`.  Fine
  for the Windows SDK; revisit if a cross-platform SDK is ever built here.

## pack/gendocu — the generated API reference (2026-10-08)

autodoc is migrated (`//main/autodoc:autodoc`), so the reference trees it drives
can be generated.  The pipeline rule is `//build/rules:gendocu.bzl`'s
`autodoc_html` (one `ctx.actions.run`, output is a tree artifact since the HTML
file set depends on the API).

- **IDL reference — DONE.**  `//main/odk:idl_reference` → the `docs/common/ref`
  tree (8156 files: `com/…`, `index-files/`, `module-ix.html`).  This is
  pack/copying's `IDL_DOCU_INDEX_FILE`, the one pack/copying step deferred while
  autodoc was unmigrated.  autodoc runs over the **udkapi + offapi** source IDL
  as two `-t` roots (it merges them — offapi types reference udkapi types).
  Green on `--config=winXP-x86`, which also builds autodoc for the **x64 exec**
  platform (no arch issues).

- **C++ reference — DONE.**  `//main/odk:cpp_reference` → the `docs/cpp/ref`
  tree (1156 files: `names/…`, `index-files/`, `index.html`).  Same
  `autodoc_html` rule, but the C++ `-p <proj> <root> -t <sub>…` form goes
  entirely in `args` (roots = []): each project's root is that module's own
  `inc/` (upstream uses one INCOUT holding every subtree; here they are
  separate), the `-t` subdirs are relative to the preceding `-p`, and the two
  `-ignoredefine` flags drop the DLL-export macros so the C++ parser does not
  trip.  Reuses the six `sdk_headers` filegroups plus a new one in `bridges`.
  Green on `--config=winXP-x86`.

- **Java reference — DONE.**  `//main/odk:java_reference` → the `docs/java/ref`
  tree (the UNO Java *runtime* reference: `com.sun.star.{comp.helper,
  lib.uno.helper,lib.unoloader,uno}` + `UrlToFileMapper`).  New rule
  `javadoc_html` (JDK runtime toolchain, same tree-artifact shape).  Upstream
  unzips `*_src.zip` into one `JAVA_SRC_DIR`; here javadoc reads the source
  trees in place via a multi-root `-sourcepath`, with `ridl`/`jurt`/`juh` jars
  on the classpath; the `-linkoffline` cross-links and the `index.html` patch
  are dropped (they only add external links).  New `sdk_javadoc_srcs`
  filegroups in `jurt`/`javaunohelper`/`ridljar`.  Green on
  `--config=winXP-x86` (the toolchain JDK's javadoc, so modern HTML).

### Remaining gendocu work

- **idl.css / cpp.css overlay**: pack/copying replaces autodoc's generated css
  with the styled `docs/{common,cpp}/ref/*.css`.  Left as autodoc's own for now.
- **Packaging into `odkcommon.zip`**: all three references are tree artifacts
  and `zipper` cannot recurse a directory, so folding `docs/common/ref`,
  `docs/cpp/ref` and `docs/java/ref` into odkcommon needs a tree→zip step (an
  `Args.add_all` with `expand_directories = True`, or a small zip-the-tree
  tool).  Until then the three `*_reference` targets are standalone.
