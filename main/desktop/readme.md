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
| `//main/desktop:guiloader` | `guiloader.exe` | Generic GUI loader; staged only as `unopkg.exe` |
| `//main/desktop:guistdio` | `guistdio.exe` | Console stdio bridge; staged only as `crashrep.com` |
| `//main/desktop:unopkgio` | `unopkgio.exe` | Package manager console bridge; staged only as `unopkg.com` |
| `//main/desktop:unopkg` | `unopkg.exe` | The package manager itself; staged only as `unopkg.bin` |
| `//main/desktop:unopkg_com` / `unopkg_exe` / `unopkg_bin` | `brand/unopkg.{com,exe,bin}` | The three installed files of `unopkg` (copies, see below) |
| `//main/desktop:crashrep_com` | `brand/crashrep.com` | Console front end of `crashrep.exe` |
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

- **unopkg is three files, and each finds the next by its own filename**
  (`prj/d.lst`, scp2 `common_brand.scp`): `unopkg.com` (unopkgio) swaps `.COM`
  for `.EXE` and relays stdio; `unopkg.exe` (guiloader) swaps `.exe` for `.bin`;
  `unopkg.bin` is the tool.  So the names are produced here with `copy_file`
  (under `brand/`, to not collide with `:unopkg`'s own `unopkg.exe`), and
  soffice's `data` lists only those.  Until 2026-10-08 staging shipped the tool
  itself as `unopkg.exe` with the front ends under their build names.  What the
  `.com` adds is real, not cosmetic: its input thread converts console-code-page
  input to UTF-16, so `echo yes| unopkg.com add x.oxt` accepts a license, where
  `unopkg.bin` alone needs UTF-16LE on stdin.  Its OUTPUT goes through
  `WriteConsoleW` only, so redirected `unopkg.com` output is always empty
  (upstream too) -- capture output from `unopkg.bin` instead.  `crashrep.com`
  (guistdio) is the same trick for `crashrep.exe`.

- **Every small EXE embeds the VC9 manifest on the winXP builds**
  (`_VC90_MANIFEST_INPUTS`/`_LINKOPTS`, RT_MANIFEST id 1, gated off on
  `//build/constraints:is_win10`).  All of them link `/MD` + `/MANIFEST:NO`;
  without a manifest naming Microsoft.VC90.CRT the loader never consults
  WinSxS and the process dies before `main()` with a "System Error" box.  Until
  2026-10-08 that was true of swriter/scalc/sdraw/simpress/sbase/smath/sweb,
  quickstart, rebaseoo, rebasegui, unoinfo, officeloader, guiloader, guistdio
  and unopkgio (and crashrep.exe in //main/crashrep) -- only soffice.exe, which
  has an external manifest, and unopkg had one.  dmake embedded one into every
  EXE with mt.exe.  The win10 build needs none: its runtime is not side-by-side.

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
