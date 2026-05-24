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
  share/registry/   ← *.xcd configuration packages
  share/setup/      ← setup_osl.inf (consumed by instsetoo_native, still deferred)
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

# Facts from the debug session:

1. Throw site confirmed
The C++ exception is thrown at cppuhelper3MSC!cppu::ImplHelper_query+0x3c8, called from cppu::bootstrapInitialSF. ImplHelper_query is UNO's QueryInterface — it throws RuntimeException when a type is not recognized.

2. DLLs genuinely load twice at different addresses with different sizes

cppuhelper3MSC.dll: 1.4 MB at 00bf0000, then 508 KB at 00d60000
cppu3.dll, comphelpMSC.dll, svxcore.dll, icuuc.dll, basegfx.dll, sot.dll and others all load twice
Different sizes = two genuinely different physical DLL files with the same name. This is the core problem.

3. Two-phase load pattern
Phase 1 (static import resolution before loader break): sofficeapp.dll → deploymentgui.dll → svxcore.dll → pulls in a large transitive dep tree including cppuhelper3MSC, cppu3, etc.

Phase 2 (after g, during UNO bootstrap): bootstrap.uno.dll, msci_uno.dll load — their deps reload the same DLLs again at new addresses.

4. Result
Two separate copies of the UNO type system global state. Bootstrap's type descriptors are unknown to the first instance → RuntimeException in ImplHelper_query → during unwind, uno_any_clear hits a null pType → AV.

Root cause in one line: deploymentgui.dll pulls svxcore.dll into process before UNO bootstrap runs, causing a second copy of all UNO runtime DLLs to load at different addresses, splitting the type system.

Take the break. When you're back, the fix direction is clear: either remove deploymentgui/deploymentmisc from sofficeapp.dll's static import list (defer it to runtime LoadLibrary) or remove it from the staging deps entirely if the deployment check isn't needed for a dev build.
