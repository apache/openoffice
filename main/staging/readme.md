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
  program/          ← all DLLs, EXEs, RDB files, and INI files
  share/registry/   ← *.xcd configuration packages (deferred)
```

All build outputs go into `program/` so that the bootstrap INI variables work
as written: `${ORIGIN}` resolves to `program/` and `${ORIGIN}/..` to the
install root, matching the layout `utl::Bootstrap` and `scp2` expect.

## Run

```
bazel-bin\main\staging\install\program\soffice.exe
```

## Implementation

- `stage_install.bzl` — Starlark rule using `ctx.actions.declare_directory`; builds a manifest at analysis time, runs `stage_install.pl` at execution time
- `build/tools/stage_install.pl` — Perl copy script reading `subdir TAB src [TAB destname]` manifest
- ATL-guarded targets (`embedserv`, `winaccessibility`) included via `select({"//build:has_atl": [...], "//conditions:default": []})`

## Known gaps (deferred)

- **INI files are hand-written stand-ins** for what `scp2` + `instsetoo_native` would generate at
  install time. When those modules are migrated the static files in this directory should be
  replaced by generated outputs.
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
