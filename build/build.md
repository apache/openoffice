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

The build targets **32-bit Windows** using the **VS2008 (VC9) MSVC toolchain**.
A custom Bazel toolchain (`//build/toolchain`) drives the compiler directly, so
the Visual Studio IDE is not required — only the compiler, linker, and headers.

| Tool | Version / Notes |
| --- | --- |
| **Bazel** | bzlmod-based (`MODULE.bazel`). Install via [Bazelisk](https://github.com/bazelbuild/bazelisk). ⚠️ Bazel 9.0.1 has a patch-section bug — see [§7](#7-known-issues). |
| **Visual Studio 2008 (VC9)** | Provides `cl.exe`, `link.exe`, `lib.exe`, `ml.exe`. Default: `C:\Program Files (x86)\Microsoft Visual Studio 9.0`. |
| **Windows SDK v7.0** | System headers/libs. Default: `C:\Program Files\Microsoft SDKs\Windows\v7.0`. |
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
# Disable Bazel's automatic MSVC detection — we register our own VS2008 toolchain.
build --repo_env=BAZEL_DO_NOT_DETECT_CPP_TOOLCHAIN=1

# Per-machine tool locations (use FORWARD SLASHES, and double-quote the value so
# bazelrc does not strip backslashes). Edit if yours differ from the defaults.
build --repo_env="VS_PATH=C:/Program Files (x86)/Microsoft Visual Studio 9.0"
build --repo_env="SDK_PATH=C:/Program Files/Microsoft SDKs/Windows/v7.0"
build --repo_env="MSVC_TMP=C:/Users/<you>/AppData/Local/Temp"
build --repo_env="PERL_PATH=C:/msys64/usr/bin/perl.exe"
```

Defaults (used when a `--repo_env` is omitted) are defined in
[build/vs_config_repo.bzl](vs_config_repo.bzl):

| Variable | Default | Meaning |
| --- | --- | --- |
| `VS_PATH` | `C:\Program Files (x86)\Microsoft Visual Studio 9.0` | VS2008 root. `VC` = `VS_PATH\VC`. |
| `SDK_PATH` | `C:\Program Files\Microsoft SDKs\Windows\v7.0` | Windows SDK v7.0 root. |
| `MSVC_TMP` | `C:\Temp` | Scratch dir for MSVC actions. |
| `PERL_PATH` | `C:\msys64\usr\bin\perl.exe` | Host Perl for codegen genrules. |

The generated `@vs_config//:paths.bzl` exposes `VS`, `VC`, `SDK`, `MSVC_TMP`,
`PERL`, and `DEBUG_CRT_DIR`, which the toolchain BUILD file
([build/toolchain/BUILD.bazel](toolchain/BUILD.bazel)) consumes.

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
The custom VS2008 x86 toolchain lives in [build/toolchain](toolchain/) and is
registered in `MODULE.bazel`:

```python
register_toolchains("//build/toolchain:cc_toolchain_x86_vs2008_def")
```

Key toolchain choices (see [CLAUDE.md](../CLAUDE.md) for full rationale):
`/Zc:wchar_t-`, `_HAS_ITERATOR_DEBUGGING=0` set globally, `/Z7` (no
`mspdbsrv.exe` for fastbuild), MASM via `ml.exe` with `/Cx`.

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
installer archives (`scp2.bzl`), RDB merge (`merge_rdb.bzl`), and image packing
(`images.bzl`).

---

## 4. Running builds

> Independent commands print to stderr; you do not need `2>&1`.

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

Output lands under `bazel-bin/main/staging/install/`:

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
bazel-bin\main\staging\install\program\soffice.exe
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

`--compilation_mode=dbg` links against the **debug CRT** (`/MDd`), which needs
`MSVCR90D.dll` / `MSVCP90D.dll` at runtime. These are *not* on a normal machine's
WinSxS, so:

- Staging copies them from the `@msvc_debug_crt` repository
  ([build/debug_crt_repo.bzl](debug_crt_repo.bzl)) into `program/`, but **only**
  under `--compilation_mode=dbg` (selected via `//build:dbg_build`).
- A private SxS application manifest (`soffice.exe.manifest` +
  `Microsoft.VC90.DebugCRT.manifest`, see [main/staging/BUILD.bazel](../main/staging/BUILD.bazel))
  tells Windows to resolve the debug CRT from `program/` instead of WinSxS.
  Without it you get a runtime **R6034** ("application has made an attempt to
  load the C runtime library incorrectly").

The debug CRT path itself is derived in
[build/vs_config_repo.bzl](vs_config_repo.bzl) as
`VC\redist\Debug_NonRedist\x86\Microsoft.VC90.DebugCRT` (exposed as
`DEBUG_CRT_DIR`); make sure your VS2008 install actually has that redist folder.

### 6.5 Running under a debugger

After the staged debug build, attach to the launcher:

```bash
# Visual Studio / WinDbg / CDB — symbols are auto-found because the .pdb files
# sit next to the DLLs in program\.
cdb -g bazel-bin\main\staging\install\program\soffice.exe
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
> capturing its message live (`bp MSVCR90D!_CxxThrowException ...`) — useful for
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
```
