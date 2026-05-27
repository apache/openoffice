# extensions — Bazel Migration Notes

## Migrated Components

| DLL | Source module | Description |
|-----|---------------|-------------|
| `log.dll` | `source/logging` | Logging framework UNO services (LoggerPool, handlers, formatters) |
| `xmx.dll` | `source/xmlextract` | XML data extraction service |
| `res.dll` | `source/resource` | Office resource loader (VclStringResourceLoader, OfficeResourceLoader) |
| `scn.dll` | `source/scanner` | Scanner/TWAIN interface (Windows: scnserv + scanner + scanwin + twain) |
| `oooimprovement.dll` | `source/oooimprovement` | OOo improvement telemetry jobs |
| `oooimprovecore.dll` | `source/oooimprovecore` | OOo improvement core controller |
| `updatefeed.dll` | `source/update/feed` | Online update information provider |
| `bib.dll` | `source/bibliography` | Bibliography frame loader and toolbar |
| `dbp.dll` | `source/dbpilots` | Database form control wizards (Grid, ListCombo, GroupBox) |
| `abp.dll` | `source/abpilot` | Address book data source pilot wizard |
| `preload.dll` | `source/preload` | OEM preload / wizard service |
| `pcr.dll` | `source/propctrlr` | Property controller / form design inspector |
| `updchk.dll` | `source/update/check` | Online update checker (always built; uses curl) |
| `updchkui.dll` | `source/update/ui` | Online update check notification UI |

## Resource Files (.res)

Resource files compiled via `rsc_res` rule and staged to `program/resource/`:

| Target | Staged as |
|--------|-----------|
| `bib_res` | `biben-US.res` |
| `dbp_res` | `dbpen-US.res` |
| `abp_res` | `abpen-US.res` |
| `preload_res` | `preloaden-US.res` |
| `pcr_res` | `pcren-US.res` (needs `extensions_res_images` for button icons) |
| `upd_res` | `upden-US.res` |
| `updchkui_res` | `updchken-US.res` (needs `extensions_update_images` for status icons) |

## Deferred / Windows-Only

- **`oleautobridge.uno.dll`** (`source/ole`) — OLE/COM automation bridge; requires ATL headers
  (VS Professional only). Guarded by `//build:has_atl`. Enable with
  `build --//build:atl_available=True` in `user.bazelrc`.

- **`config/ldap`** (`ldapbe.dll`) — LDAP user profile backend. Requires the LDAP SDK
  headers and libraries. Deferred until LDAP SDK is wrapped in Bazel.

- **`mozbootstrap.uno`** — Linux/macOS only (makefile explicitly skips Windows). Not built.

- **`source/activex`** (`msidl` + `main`) — Windows COM ActiveX host. Requires MIDL compiler
  and special COM registration; deferred.

## Key Build Notes

- Scanner (`scn.dll`): Windows sources only (`scnserv.cxx`, `scanner.cxx`, `scanwin.cxx`,
  `twain.cxx`). SANE/TWAIN Linux sources excluded. Needs `/Imain/twain/inc` for `twain/twain.h`.

- `update/check` (`updchk.dll`): In the old dmake build this was conditional on
  `ENABLE_ONLINE_UPDATE=YES`. In Bazel we always build it; runtime behaviour is controlled
  by configuration XCU. Requires `curl` static lib (`CURL_STATICLIB` define).

- `COMPMOD_NAMESPACE` define: required by dbpilots (`dbp`) and abpilot (`abp`) to scope
  module-level singletons.

- `oooimprovecore` vs `oooimprovement`: two separate DLLs — core is the heavy one (needs
  sfx2/svx deps), improvement is the lightweight telemetry jobs DLL.
