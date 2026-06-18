# desktop — Bazel Migration

## Outputs

| Target | Output | Notes |
|---|---|---|
| `//main/desktop:deploymentmisc` | `deploymentmisc.dll` | Deployment C++ helper; exports via `DESKTOP_DEPLOYMENTMISC_DLLIMPLEMENTATION` |
| `//main/desktop:deploymentgui` | `deploymentgui.uno.dll` | Extension Manager GUI; extra export `handleVersionException` (extern "C") |
| `//main/desktop:deployment` | `deployment.uno.dll` | Main deployment UNO component; links all registry/manager sub-libs |
| `//main/desktop:sofficeapp` | `sofficeapp.dll` | Core office app DLL; exports `soffice_main` |
| `//main/desktop:spl` | `spl.dll` | Splash screen UNO component; includes migration objects |
| `//main/desktop:socomp` | `socomp.dll` | SO component UNO service |
| `//main/desktop:offacc` | `offacc.dll` | Office acceptor UNO component |
| `//main/desktop:migrationoo2` | `migrationoo2.uno.dll` | OOo 2.x migration UNO component |
| `//main/desktop:unopkgapp` | `unopkgapp.dll` | Package manager app DLL; exports `unopkg_main` |
| `//main/desktop:soffice` | `soffice.exe` | Main OOo launcher EXE |
| `//main/desktop:officeloader` | `officeloader.exe` | Bootstrap loader (delivered as soffice.exe) |
| `//main/desktop:guiloader` | `guiloader.exe` | Generic GUI loader (also: unopkg.exe, testtool.exe) |
| `//main/desktop:guistdio` | `guistdio.exe` | Console stdio bridge (also: crashrep.com) |
| `//main/desktop:unopkgio` | `unopkgio.exe` | Package manager console bridge (also: unopkg.com) |
| `//main/desktop:unopkg` | `unopkg.exe` | Package manager launcher |
| `//main/desktop:unoinfo` | `unoinfo.exe` | UNO environment info |
| `//main/desktop:swriter` | `swriter.exe` | Writer app launcher |
| `//main/desktop:scalc` | `scalc.exe` | Calc app launcher |
| `//main/desktop:sdraw` | `sdraw.exe` | Draw app launcher |
| `//main/desktop:simpress` | `simpress.exe` | Impress app launcher |
| `//main/desktop:sbase` | `sbase.exe` | Base app launcher |
| `//main/desktop:smath` | `smath.exe` | Math app launcher |
| `//main/desktop:sweb` | `sweb.exe` | Writer Web app launcher |
| `//main/desktop:rebaseoo` | `rebaseoo.exe` | DLL rebasing tool |
| `//main/desktop:rebasegui` | `rebasegui.exe` | DLL rebasing GUI tool |
| `//main/desktop:quickstart` | `quickstart.exe` | System tray quickstart |

## Key migration notes

- **Deployment sub-libs**: `deployment_manager_lib`, `deployment_registry_lib`, and 7 registry
  backend libs all compile with `alwayslink = True` and are linked into `deployment.uno.dll`.
  `dp_services.cxx` declares `extern` symbols referencing each backend so the linker pulls them.

- **Migration library**: `migration_lib` (pages/wizard/migration/cfgfilter) is compiled once and
  linked `alwayslink` into both `sofficeapp.dll` and `spl.dll`.

- **DEF files** in `util/`: `sofficeapp.def` (exports `soffice_main`), `unopkgapp.def` (exports
  `unopkg_main`), plus standard 2-export UNO DEFs for deployment/spl/socomp/offacc/migrationoo2
  and 3-export DEF for deploymentgui (adds `handleVersionException`).

- **deploymentmisc.dll** uses `DESKTOP_DEPLOYMENTMISC_DLLIMPLEMENTATION` → `__declspec(dllexport)`
  automatically; no DEF file needed.

- **deploymentgui.uno.dll** uses `DESKTOP_DEPLOYMENTGUI_DLLIMPLEMENTATION` → `handleVersionException`
  auto-exports via `__declspec(dllexport)`; the DEF file additionally lists the UNO entry points.

- **hash_map usage**: stlport dep is included via `desktop_source_hdrs` for all targets that use
  `<hash_map>` (deployment manager, registry, app dispatcher).

- **unistd.h**: `source/app/app.cxx` includes `<unistd.h>`; `/Imain/soltools/winunistd` is in
  `sofficeapp` copts only.

- **App launchers** (swriter/scalc/sdraw/simpress/sbase/smath/sweb): each links `launcher.cxx` +
  `<app>.cxx`, uses `UNICODE`/`_UNICODE`, links only `shell32.lib`.

- **RSC resource libraries — THREE separate `.res`, not one**: the desktop module's strings are
  loaded at runtime by `ResMgr::CreateResMgr("<name>")` → `<name>en-US.res`, so each ResMgr name
  needs its own `.res` file. Upstream builds three (`util/makefile.mk` → `dkt`,
  `source/deployment/makefile.mk` → `deployment`, `source/deployment/gui/makefile.mk` →
  `deploymentgui`). The three `rsc_res` targets are `:desktop_res` (→ `dkten-US.res`,
  app `desktop.src` + migration `wizard.src`), `:deployment_res` (→ `deploymenten-US.res`,
  registry/manager/misc/unopkg `.src`, gui excluded) and `:deploymentgui_res`
  (→ `deploymentguien-US.res`, the `dp_gui_*.src` Extension Manager dialogs). `hdrs`/`includes`/
  `images` are shared (a strict partition of the same inputs).
  HISTORY: a single `glob(["source/**/*.src"])` once merged everything into one `.res` staged as
  both `dkten-US` and `deploymenten-US`, so `deploymentguien-US.res` never existed →
  `ResMgr::CreateResMgr("deploymentgui")` returned NULL → `DeploymentGuiResMgr::get()` NULL-deref
  in `ResMgr::GetResource` (`[eax+20h]`, eax=0) the instant Extension Manager opened.
