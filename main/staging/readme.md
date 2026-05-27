# main/staging — runnable OpenOffice install layout

Assembles all Bazel build outputs into a runnable OpenOffice install tree.

## Build

```
bazel build //main/staging:install
```

## Run

```
bazel-bin\main\staging\install\program\soffice.exe
```

## Staged layout

Mirrors the real OpenOffice install tree (`C:\Program Files (x86)\OpenOffice 4\`):

```
bazel-bin/main/staging/install/
  program/           ← all DLLs, EXEs, RDB files, INI files
  program/resource/  ← binary .res resource files (one per UI module, e.g. vclen-US.res)
  share/registry/    ← *.xcd configuration packages
  share/setup/       ← setup_osl.inf (consumed by instsetoo_native, still deferred)
```

All build outputs go into `program/` so that the bootstrap INI variables work
as written: `${ORIGIN}` resolves to `program/` and `${ORIGIN}/..` to the
install root, matching the layout `utl::Bootstrap` and `scp2` expect.

## Run

```
bazel-bin\main\staging\install\program\soffice.exe
```

## Implementation

- `collect_files_aspect.bzl` — aspect that collects transitive `DefaultInfo` + PDB files; `collect_outputs` rule aggregates them; `flat_install` copies to `program/`; `res_stage` copies `.res` files to `program/resource/` with locale-renamed filenames; `tree_install` copies a subtree preserving relative paths.
- ATL-guarded targets (`embedserv`, `winaccessibility`) included via `select({"//build:has_atl": [...], "//conditions:default": []})`
- Debug CRT DLLs (`MSVCR90D`, `MSVCP90D`) and private-assembly manifest staged only for `--compilation_mode=dbg` builds via `select({"//build:dbg_build": [...]})`

## Known gaps (deferred)

- **INI files are hand-written stand-ins** for what `instsetoo_native` would generate at
  install time from the scp2 `.par` files.  `scp2` is now built and staged at
  `share/setup/setup_osl.inf`; the remaining step is `instsetoo_native` (still deferred)
  which would read that `.inf` and produce the Windows MSI and the INI files currently
  maintained by hand here.
- **Silent crash on bootstrap failure**: if `UserInstallation` or `BaseInstallation` cannot be
  resolved, `UserInstall::finalize()` returns `E_Unknown` and the application exits without any
  error message or dialog. The proper fix is in `desktop/source/app/app.cxx` — the `E_Unknown`
  case in the `UserInstall::finalize()` error handler should show a meaningful error before exit,
  the same way `E_NoDiskSpace` and `E_NoWriteAccess` already do.

## Fixes applied during staging

- `//main/i18npool/pool:i18npool` and `:i18nsearch` — correct subpackage path (not `//main/i18npool:`)
- Added `package(default_visibility = ["//visibility:public"])` to ~75 BUILD files
- `unopkg` — added `//main/sal:sal_headers` dep
- Launcher EXEs (`swriter` etc.) — added `user32.lib`
- `rebaseoo` — `WinMain` entry point requires `/SUBSYSTEM:WINDOWS`, not CONSOLE; added `user32.lib`
- `rebasegui` — added `user32.lib`
- `guistdio` — added `user32.lib`
- `quickstart` — removed `UNICODE`/`_UNICODE` (source uses narrow Win32 APIs)
- UCB sub-packages — added per-source-subdir `/I` copts for `cached1`, `srtrs1`, `ucpexpand1`, `ucpext`, `ucphier1`, `ucppkg1`, `ucptdoc1`
- `sdfilt` — added `/Imain/sd/source/filter/ppt` and `/eppt`
- `msword` — added `//main/writerfilter:writerfilter_headers` dep
- `i18nsearch` — added `//main/comphelper:comphelper_headers`
- `writerfilter_gen_headers` — added `".."` to `includes` so `writerfilter/doctok/sprmids.hxx` (generated) resolves via `bin/main` search path

## Resource files (.res)

`ResMgrContainer::init()` scans `$OOO_BASE_DIR/program/resource/` on startup. The `res_stage`
rule in `collect_files_aspect.bzl` copies each `rsc_res` output to that directory with the
locale-named filename the runtime expects (`<prefix>en-US.res`, e.g. `vclen-US.res`).

**Byte-order requirement**: `ResMgr::GetLong`, `GetShort`, and `GetUInt64` are unconditionally
big-endian readers. The `.res` pipeline must pass `-BIGENDIAN` to `rsc2` (the rsc2 default).
Using `-LITTLEENDIAN` causes `InternalResMgr::Create()` to misread the content-table length at
the end of the file, causing `rtl_allocateMemory` to fail (returns NULL), then
`ResMgr::GetUInt64(NULL)` crashes immediately on startup.
