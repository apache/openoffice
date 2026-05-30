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
- `//main/fileaccess:fileacc` — was registered in `services.rdb` but never staged; added to the UCB component block (see "Every services.rdb component DLL must be staged" below)

## Every services.rdb component DLL must be staged

`services.rdb` (built by `//main/postprocess:services_rdb`) maps each UNO implementation to a
loader URL via `postprocess.bzl`'s `basis_native("X.dll")` → `vnd.sun.star.expand:$OOO_BASE_DIR/program/X.dll`.
If a `.component` file is listed in `postprocess/BUILD.bazel` but the corresponding `X.dll` target
is **not** a staging dep, the registration exists but the DLL is absent from `program/`. At runtime,
the first service from that DLL that someone instantiates throws
`com.sun.star.loader.CannotActivateFactoryException("loading component library failed: …/X.dll")`.

Concrete failure seen 2026-05-30: `fileacc.dll` (`com.sun.star.ucb.SimpleFileAccess`) was registered
but unstaged. On startup `Desktop::Main` → `syncRepositories` → `getSharedRepository` →
`dp_registry::create` instantiates the deployment **help backend**, which needs `SimpleFileAccess`.
The load failed, the exception was wrapped at `dp_manager.cxx:482` as
`WrappedTargetRuntimeException("[context=\"shared\"] caught unexpected exception!")`, propagated to the
outer `catch (Exception&)` in `Desktop::Main`, and became a `FatalError` dialog → silent exit.

**Rule:** for every `basis_native("X.dll")` in `postprocess/BUILD.bazel`, `X.dll` must resolve to a
staging dep here. After changing either file, verify each registered DLL exists in
`bazel-bin/main/staging/program/`.

**How it was diagnosed (CDB, no private symbols):** decode the thrown C++ class post-mortem from the
MSVC `ThrowInfo` (3rd parameter of the `e06d7363` exception record):
`r $t0=poi(<throwinfo>+0c); r $t1=poi($t0+4); r $t2=poi($t1+4); da $t2+8` →
`.?AVCannotActivateFactoryException@loader@…`. Then capture the message live with
`bp MSVCR90D!_CxxThrowException "du poi(poi(esp+4))+8 L80; gc"` — the message names the exact DLL.

## Resource files (.res)

`ResMgrContainer::init()` scans `$OOO_BASE_DIR/program/resource/` on startup. The `res_stage`
rule in `collect_files_aspect.bzl` copies each `rsc_res` output to that directory with the
locale-named filename the runtime expects (`<prefix>en-US.res`, e.g. `vclen-US.res`).

**Byte-order requirement**: `ResMgr::GetLong`, `GetShort`, and `GetUInt64` are unconditionally
big-endian readers. The `.res` pipeline must pass `-BIGENDIAN` to `rsc2` (the rsc2 default).
Using `-LITTLEENDIAN` causes `InternalResMgr::Create()` to misread the content-table length at
the end of the file, causing `rtl_allocateMemory` to fail (returns NULL), then
`ResMgr::GetUInt64(NULL)` crashes immediately on startup.
