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

# Building Apache OpenOffice with Bazel

This document describes how to set up a development machine and run Bazel builds
of Apache OpenOffice. The Bazel build replaces the legacy Perl/dmake/gmake
orchestration layer; the **primary goal is to eliminate the Cygwin dependency**.
Source code is never modified — only the build system.

For per-module conventions, compiler flags, and the migration frontier, see
[CLAUDE.md](../CLAUDE.md) at the repository root and
[Migrated-packages.md](../Migrated-packages.md).

---

## 1. Prerequisites

The build targets **64-bit Windows 7 SP1 and later**, compiled with the
**VS2019 MSVC toolset and the Windows 10 SDK** (`win7-x64`, the only config). A
custom Bazel toolchain (`//build/toolchain`) drives the compiler directly, so the
Visual Studio IDE is not required — Build Tools are enough.

The VS2008 (VC9) toolchains — 32-bit `winXP-x86`, until then the default, and
`winXP-x64` — were retired on 2026-11-01. VS2008 and the Windows SDK v7.0 are
no longer needed on the machine for the default build: scp2's preprocessor and
every `RC.Exe` genrule use the VS2019 toolset and the Windows 10 SDK. Their
output was checked byte-identical when switched (all 94 `.par` files, every
`.res`). One opt-in exception remains: `winaccessibility`'s MIDL step, which only
builds with `--//build:atl_available=True` (§5), still calls SDK v7.0 `midl.exe`.

The VC90 runtime is gone from the Bazel build too: nothing stages
`msvcr90`/`msvcp90` or declares `Microsoft.VC90.CRT` any more. The binaries
need the VS2019 runtime (`vcruntime140`, `msvcp140`, the UCRT) from the VC++
redistributable; it is not staged. `soffice.exe` keeps one application
manifest, for Common-Controls 6 (visual styles).

| Tool | Version / Notes |
| --- | --- |
| **Bazel** | bzlmod-based (`MODULE.bazel`). Install via [Bazelisk](https://github.com/bazelbuild/bazelisk). ⚠️ Bazel 9.0.1 has a patch-section bug — see [§7](#7-known-issues). |
| **VS2019 Build Tools** | The MSVC toolset (`cl.exe`, `link.exe`, `lib.exe`, `ml64.exe`), found under `VS_MODERN_PATH`. Pin a version with `MSVC_TOOLSET_VERSION`. |
| **Windows 10 SDK** | System headers/libs, `rc.exe`, `mt.exe`, found under `WIN10_SDK_PATH`. Pin a version with `WIN10_SDK_VERSION`. |
| **Windows SDK v7.0** (optional) | Only for `winaccessibility`'s `midl.exe` under `--//build:atl_available=True`; see above. |
| **Perl** | Used by IDL/rsc/installer codegen scripts. A Strawberry Perl is pulled in via bzlmod for *build deps*, but some local genrules invoke a host Perl set via `PERL_PATH` (e.g. `C:\msys64\usr\bin\perl.exe`). |
| **ATLMFC** (optional) | Only needed for ATL modules (`embedserv`, `winaccessibility`, `extensions/activex`). VS Express / BuildTools do **not** ship ATLMFC — those modules are skipped unless you opt in. See [§5](#5-optional-features). |

> **Cygwin is being phased out.** New build steps must not depend on it. Where a
> POSIX tool is still needed, it is invoked through a pinned `PERL_PATH` or a
> bzlmod-provided toolchain, not the system Cygwin shell.

---

## 2. One-time machine setup

All per-machine paths are configured in **`user.bazelrc`** (git-ignored,
`try-import`ed by `.bazelrc`). The toolchain reads them via `--repo_env` and the
`@vs_config` repository rule ([build/vs_config_repo.bzl](vs_config_repo.bzl)).

Create / edit `user.bazelrc` at the repository root:

```bash
# Disable Bazel's automatic MSVC detection — we register our own toolchain.
build --repo_env=BAZEL_DO_NOT_DETECT_CPP_TOOLCHAIN=1

# Per-machine tool locations (use FORWARD SLASHES, and double-quote the value so
# bazelrc does not strip backslashes). Edit if yours differ from the defaults.
build --repo_env="VS_MODERN_PATH=C:/Program Files (x86)/Microsoft Visual Studio/2019/BuildTools"
build --repo_env="WIN10_SDK_PATH=C:/Program Files (x86)/Windows Kits/10"
build --repo_env="WIN10_SDK_VERSION=10.0.19041.0"   # optional pin
build --repo_env="MSVC_TMP=C:/Users/<you>/AppData/Local/Temp"
build --repo_env="PERL_PATH=C:/msys64/usr/bin/perl.exe"
```

Defaults (used when a `--repo_env` is omitted) are defined in
[build/vs_config_repo.bzl](vs_config_repo.bzl):

| Variable | Default | Meaning |
| --- | --- | --- |
| `VS_MODERN_PATH` | VS2019 Build Tools root | Where the MSVC toolset is discovered. |
| `WIN10_SDK_PATH` | `C:\Program Files (x86)\Windows Kits\10` | Where the Windows 10 SDK is discovered. |
| `MSVC_TMP` | `C:\Temp` | Scratch dir for MSVC actions. |
| `PERL_PATH` | `C:\msys64\usr\bin\perl.exe` | Host Perl for codegen genrules. |

The generated `@vs_config//:paths.bzl` exposes `MSVC_TMP`, `PERL`, the toolset
constants (`MSVC_*`), the SDK constants (`SDK10_*`) and `RC_EXE`. The toolchain
BUILD file ([build/toolchain/BUILD.bazel](toolchain/BUILD.bazel)) consumes the
first three groups; scp2 and the `RC.Exe` genrules use the rest. `VS_PATH` and
`SDK_PATH` are no longer read; delete them from an older `user.bazelrc`.

---

## 3. How the build is wired

### MODULE.bazel + registries

Dependencies are declared in [MODULE.bazel](../MODULE.bazel) (bzlmod). Two
registries are consulted, in order ([.bazelrc](../.bazelrc)):

1. **Local registry** `file:///%workspace%/ext_libraries` — AOO's third-party
   libraries (icu, redland, boost.legacy, nss, coinmp, mdds, …), each as a
   Bazel module with its own `BUILD.bazel`.
2. **BCR** `https://bcr.bazel.build` — public modules (rules_cc, skylib, curl,
   openssl, zlib, …).

`--check_direct_dependencies=off` is set because the legacy version pins do not
all match BCR's resolution.

### Toolchain

One custom toolchain lives in [build/toolchain](toolchain/), registered in
`MODULE.bazel`:

```python
register_toolchains("//build/toolchain:aoo_msvc_vs2019_x64_def")
```

Which toolchain Bazel uses is decided by the **target platform**, not a flag.
The platform `win7-x64` in [build/platforms/BUILD.bazel](platforms/BUILD.bazel)
combines `@platforms//cpu:x86_64` with the `//build/constraints:win_version`
constraint, and toolchain resolution picks the `cc_toolchain` that matches both.
A further Windows floor is a new constraint value, not a new stringly-typed
config. The same platform is registered as the execution platform, so build
tools (idlc, regmerge, rsc, …) are compiled by the same toolchain.

The arch defines are injected **globally by the toolchain**, so module BUILD
files carry no arch literals: `_AMD64_=1` / `X86_64` / `CPPU_ENV=mscx`, with
`WIN32` defined as well (the compiler auto-defines `_WIN64`).

Key toolchain choices (see [.agent/common/10-toolchain.md](../.agent/common/10-toolchain.md)
for the rationale): `/std:c++14`, `/Zc:wchar_t-`, `_HAS_ITERATOR_DEBUGGING=0` set
globally, `/Z7` (no `mspdbsrv.exe` for fastbuild), MASM via `ml64.exe`.

### Where BUILD files live

**`main/<package>/BUILD.bazel`** — at the *module root*, **not** under `prj/`.
`cc_library`/`cc_binary` need `glob()` access to sources, which requires the
BUILD file at the module root. The legacy `prj/build.lst` is still parsed by hand
to derive `deps`, and `prj/d.lst` describes the legacy delivery layout, but
neither is the Bazel build entry point.

### Custom rules

Reusable Starlark rules live in [build/rules/](rules/): the SDI compiler
(`sdi_target.bzl`), IDL pipeline (`idl_pipeline.bzl`), resource compiler
(`rsc_pipeline.bzl`), locale data codegen (`localedata_pipeline.bzl`), scp2
installer archives (`scp2.bzl`), and RDB merge (`merge_rdb.bzl`).  Image packing
is `main/packimages/packimages.bzl`, which runs upstream's own `packimages.pl`.

---

## 4. Running builds

> Independent commands print to stderr; you do not need `2>&1`.

### Configuration

There is one configuration, `win7-x64`, and it is the default, so no `--config`
is needed. It pins `--platforms` plus its own `--platform_suffix` and
`--symlink_prefix` (defined in [.bazelrc](../.bazelrc)), so outputs land under
`bazel-win7-x64-bin/`.

The retired `--config=winXP-x86` / `--config=winXP-x64` no longer exist. Their
`bazel-winXP-*` output trees are not cleaned up automatically.

### Build a single module

```bash
bazel build //main/sal:sal
bazel build //main/sw:sw
bazel build //main/cui:cui
```

### Build everything migrated

```bash
bazel build //main/...
```

### Assemble a runnable install tree

The [//main/staging](../main/staging/readme.md) package collects every build
output into a real OpenOffice install layout:

```bash
bazel build //main/staging:install
```

Output lands under `bazel-win7-x64-bin/main/staging/install/`:

```
install/
  program/           ← all DLLs, EXEs, RDB/INI files, python core
  program/resource/  ← binary .res files (one per UI module, e.g. vclen-US.res)
  share/registry/    ← *.xcd configuration packages
  share/config/      ← soffice.cfg UI config tree, images.zip, …
```

To also stage the full Python standard library, build `//main/staging:install_all`.

### Run it

```bash
bazel-win7-x64-bin\main\staging\install\program\soffice.exe
```

### Inspect the dependency graph

```bash
bazel query "deps(//main/sw:sw)"
bazel cquery //main/staging:install --output=files
```

---

## 5. Optional features

### Languages / locales

The default build is **en-US only** (the demo baseline). Locale flags are
defined in [build/BUILD.bazel](BUILD.bazel) from [build/langs.bzl](langs.bzl).
Enable additional UI languages in `user.bazelrc` or on the command line:

```bash
build --//build:lang_de=True --//build:lang_fr=True
```

> Today these flags wire **axes 1+2** (which locales are installable + the
> default UI language). Translated *strings* (axis 3) need the Pootle→SDF
> bridge, which is not yet implemented — see the *Localization* section of
> [CLAUDE.md](../CLAUDE.md).

### ATL modules

ATL modules need ATLMFC (VS Pro/Enterprise). On such an install, opt in:

```bash
build --//build:atl_available=True
```

When `False` (the default), `embedserv`, `winaccessibility`, and
`extensions/activex` are dropped from the build and from staging, mirroring the
old `DISABLE_ATL` guard.

---

## 6. Debug builds

A debuggable build is the combination of two **independent** things: (a) the
compiler/linker must emit symbols, and (b) those symbols must end up next to the
binaries you actually run. Bazel keeps these on separate knobs, so you set both.

### 6.1 The four knobs

Defined in [user.bazelrc](../user.bazelrc) and [build/BUILD.bazel](BUILD.bazel):

| Flag | What it changes | Why |
| --- | --- | --- |
| `--compilation_mode=dbg` | Switches the toolchain to the **debug CRT** (`/MDd`) and turns on `/Z7` + `/Od` (embedded debug info, no optimization). | Lets the debugger map addresses to source and show real variable values instead of optimized-away ones. |
| `--features=generate_pdb` | Makes `link.exe` write a **separate `.pdb`** next to each DLL/EXE. | `/Z7` embeds line info in the `.obj`s but the *linker* still won't produce a `.pdb` on its own — WinDbg/CDB/VS want the `.pdb`. |
| `--//build:debug_info=True` | Tells staging (`flat_install` in [collect_files_aspect.bzl](../main/staging/collect_files_aspect.bzl)) to **copy the `.pdb` files into `program/`** alongside the DLLs. | Without this the `.pdb`s exist deep in `bazel-out/` but not where `soffice.exe` runs, so the debugger can't find them. |
| `--jobs=1` | Serializes all actions. | **Mandatory whenever PDBs are generated.** `mspdbsrv.exe` is a single shared server that cannot handle concurrent PDB writers; parallel links abort with `LNK1318 RPC_S_SERVER_UNAVAILABLE (0x6BA)`. |

> The toolchain ([build/toolchain/BUILD.bazel](toolchain/BUILD.bazel)) sets
> `dbg_mode_debug_flag = ""` / `fastbuild_mode_debug_flag = ""` on purpose:
> `/Z7` already embeds debug info in the `.obj`, so no `/Zi` + `mspdbsrv` is used
> during *compilation*. The single-server `mspdbsrv` contention only appears at
> the **link** step, which is why `--jobs=1` is paired with `generate_pdb`, not
> with `compilation_mode=dbg` alone.

### 6.2 Recommended: set it once in user.bazelrc

For a debugging session, uncomment the block already present in
[user.bazelrc](../user.bazelrc) so every `bazel build //main/staging:install`
produces a fully-symbolized, staged tree with no extra flags:

```bash
build --//build:debug_info=True --compilation_mode=dbg
build --features=generate_pdb
build --jobs=1
```

Remove (or move back under a `build:dbg` config) those lines when you're done —
`--jobs=1` makes every build single-threaded and slow.

### 6.3 Or pass the flags ad-hoc

Full-symbol staged debug build in one command:

```bash
bazel build //main/staging:install \
  --//build:debug_info=True --compilation_mode=dbg \
  --features=generate_pdb --jobs=1
```

Just one module's symbols (faster, no staging):

```bash
bazel build //main/sw:sw --compilation_mode=dbg --features=generate_pdb --jobs=1
```

### 6.4 The debug CRT requirement

`--compilation_mode=dbg` links against the **debug CRT** (`/MDd`). With the
VS2019 toolset that is `ucrtbased.dll` and `vcruntime140d.dll` (+
`msvcp140d.dll`); they are not redistributable and are not staged. A machine with
the VS2019 Build Tools and the Windows 10 SDK has them; a debug build is for
such a machine only. They need no manifest.

The VC9-era arrangement — `MSVCR90D.dll`/`MSVCP90D.dll` copied from a VS2008
install by an `@msvc_debug_crt` repository, plus a private
`Microsoft.VC90.DebugCRT` assembly manifest — was removed with VS2008; it never
matched what a VS2019 `/MDd` binary imports. A debug `soffice.exe` gets the same
application manifest as a release one.

### 6.5 Running under a debugger

After the staged debug build, attach to the launcher:

```bash
# Visual Studio / WinDbg / CDB — symbols are auto-found because the .pdb files
# sit next to the DLLs in program\.
cdb -g bazel-win7-x64-bin\main\staging\install\program\soffice.exe
```

Note the process layout differs from a stock OpenOffice install. Upstream ships
a small `soffice.exe` **loader** that re-launches the real app as `soffice.bin`.
This Bazel build does **not** produce a `soffice.bin`: the `soffice` cc_binary
([main/desktop/BUILD.bazel](../main/desktop/BUILD.bazel)) *is* the office
application (`source/app/main.c` → `soffice_main()` in `sofficeapp.dll`) and is
staged directly as `program/soffice.exe`. The separate loader is staged under
its own name, `officeloader.exe`, and is **not** the entry point you run. So you
can attach the debugger straight to `program\soffice.exe` — there is usually no
loader→`.bin` hop to follow. (UNO components still load as in-process DLLs, so if
you launch via `officeloader.exe` instead, enable child-process debugging —
CDB `.childdbg 1` or VS "Debug child processes".) The `_HAS_ITERATOR_DEBUGGING=0` macro is forced
globally even in debug builds (see [CLAUDE.md](../CLAUDE.md)) so STL container
layout stays ABI-consistent across DLLs — do not flip it on for debugging or
containers passed between DLLs will read garbage.

> **No private symbols?** [main/staging/readme.md](../main/staging/readme.md)
> documents a CDB technique for decoding a thrown C++ exception class and
> capturing its message live (`bp MSVCR90D!_CxxThrowException ...`; with the
> VS2019 debug runtime that is `VCRUNTIME140D!_CxxThrowException`) — useful for
> the silent-exit startup failures described in [§7](#7-known-issues).

---

## 7. Known issues

- **Bazel 9.0.1 patch-section bug** — Bazel always fails on the *last* file
  listed in a module's `remote_patches`. Workaround: patch via an `overlay`
  for *all* affected files and omit the `patches` section entirely.
- **Never edit the Bazel cache by hand.** To refresh dependency resolution:

  ```bash
  bazel mod deps --lockfile_mode=refresh
  ```

  Commit `MODULE.bazel` and `MODULE.bazel.lock` together.
- **Silent exit on bootstrap failure** — if `UserInstallation` /
  `BaseInstallation` can't be resolved, `soffice.exe` exits with no dialog.
  See [main/staging/readme.md](../main/staging/readme.md) for details.

---

## 8. Adding a new module (workflow)

1. Read `main/<module>/prj/build.lst` to determine its dependencies.
2. Create `main/<module>/BUILD.bazel` (module root) with `cc_library`/`cc_binary`
   targets; map `build.lst` deps to `deps = [...]`.
3. Check the **cross-cutting flags** in [CLAUDE.md](../CLAUDE.md) (e.g.
   `/Zc:wchar_t-`, `snprintf=_snprintf`, `stlport`, zlib `Z_PREFIX`) before
   building.
4. Let the user run the build. Iterate on errors.
5. Add a `readme.md` to the module summarizing the migration.
6. Once the build succeeds, update the **frontier** in [CLAUDE.md](../CLAUDE.md)
   and move the module into [Migrated-packages.md](../Migrated-packages.md).

---

## 9. Helper scripts in `build/tools/`

Most files here are **build-step helpers**: a Bazel rule runs them as part of an
action, and they are listed in `build/tools/BUILD.bazel` under `exports_files`
so a rule can name them as a label (`fcfg_merge.pl`,
`stage_install.pl`, …). Adding one means adding it to that list.

`throwspec.py` is the exception and is **not** in `exports_files`. It is a
**one-shot source migration tool**: no rule refers to it, it edits the source
tree in place, and it is run by hand. It lives here because the knowledge in it
is worth more than the script.

### `throwspec.py` — C++ dynamic exception specifications

```bash
python build/tools/throwspec.py report   # classify every site, change nothing
python build/tools/throwspec.py apply    # delete the specifications with a type list
python build/tools/throwspec.py verify   # check a finished rewrite against HEAD
```

It removed 72,490 `throw(X)` / `SAL_THROW( (X) )` specifications across 5,063
files in one commit — which had to be one commit, because the specifications are
also **generated** (`InterfaceType::dumpExceptionSpecification` in
`main/codemaker/source/cppumaker/cpputype.cxx`), and an override carrying no
specification is *less* restrictive than a base that has one. Stripping
hand-written overrides while generated bases keep theirs only trades one error
set for another.

**What it is really for.** A regular expression cannot tell these apart — both
are the token `throw`, whitespace, `(`, a name, `)`:

```cpp
void foo( int n ) throw (RuntimeException);     // specification -> delete
if ( bad ) throw (UINT) ERROR_ALREADY_RUNNING;  // statement     -> keep
```

So the script lexes each file into code / comment / string first, then accepts a
site as a specification only when the argument parses as a type list, the
preceding token is `)` / `const` / `volatile`, the argument is non-empty, **and**
the following token is one of `{ ; = : , ) #`. That last condition exists for
exactly one site in the whole tree (`desktop/win32/source/setup/setup_main.cxx`,
a throw statement whose operand carries a C-style cast) and every weaker rule
gets it wrong. The rules are commented in full at the top of the file — read
them there before changing any of them.

Everything the classifier rejects is **reported, never silently skipped**; there
were 30 such sites and all 30 want human eyes.

**`verify` is the reason a change this size is reviewable.** The rewrite only
ever deletes, so per file the counts of `{`, `}` and `;` must come out unchanged
and `(` / `)` must drop equally. That covers a five-thousand-file diff in
seconds. Files you also edited by hand will show up in its output — reconcile
the list against what you touched deliberately rather than expecting it empty.

**Next use.** The follow-up task migrates the *empty* specification (`throw()`,
which MSVC implements as `__declspec(nothrow)`) to `noexcept`; those sites are
already reported under kind `EMPTY`. Select those instead of `SPEC` and give
`rewrite()` a substituting variant — the span bookkeeping is identical, only the
replacement text differs. `noexcept` still cannot be written literally: VS2008
is retired from this build, but the source stays compatible with AOO 4.1.x for
security backports, and 4.1.x still builds with VS2008.
