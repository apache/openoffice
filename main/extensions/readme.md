# extensions — Bazel Migration Notes

## Migrated Components

| DLL | Source module | Description |
|-----|---------------|-------------|
| `log.dll` | `source/logging` | Logging framework UNO services (LoggerPool, handlers, formatters) |
| `xmx.dll` | `source/xmlextract` | XML data extraction service |
| `res.dll` | `source/resource` | Office resource loader (VclStringResourceLoader, OfficeResourceLoader) |
| `scn.dll` | `source/scanner` | Scanner/TWAIN interface (Windows: scnserv + scanner + scanwin only) |
| `oooimprovement.dll` | `source/oooimprovement` | OOo improvement telemetry jobs |
| `oooimprovecore.dll` | `source/oooimprovecore` | OOo improvement core controller |
| `updatefeed.dll` | `source/update/feed` | Online update information provider |
| `bib.dll` | `source/bibliography` | Bibliography frame loader and toolbar |
| `dbp.dll` | `source/dbpilots` | Database form control wizards (Grid, ListCombo, GroupBox) |
| `abp.dll` | `source/abpilot` | Address book data source pilot wizard |
| `preload.dll` | `source/preload` | OEM preload / wizard service |
| `pcr.dll` | `source/propctrlr` | Property controller / form design inspector |
| `updchk.dll` | `source/update/check` | Online update checker (always built; uses curl + OpenSSL) |
| `updchkui.dll` | `source/update/ui` | Online update check notification UI |

## Resource Files (.res)

Resource files compiled via `rsc_res` rule and staged to `program/resource/`:

| Target | Staged as | Notes |
|--------|-----------|-------|
| `bib_res` | `biben-US.res` | Explicit srcs (not glob) — `hidother.src` and `menu.src` excluded (not in dmake SLOFILES); needs svx/sfx2/editeng hrc headers |
| `dbp_res` | `dbpen-US.res` | |
| `abp_res` | `abpen-US.res` | |
| `preload_res` | `preloaden-US.res` | Needs `extensions_res_images` for `arrow.png` / `m_arrow.png` |
| `pcr_res` | `pcren-US.res` | Needs `extensions_res_images` for button icons |
| `upd_res` | `upden-US.res` | |
| `updchkui_res` | `updchken-US.res` | Needs `extensions_update_images` for status icons |

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

### componentmodule.cxx — shared #include pattern
`source/inc/componentmodule.cxx` is `#include`d (not compiled standalone) by the
module-driver file of each sub-component: `moduledbp.cxx`, `moduleabp.cxx`,
`modulepreload.cxx`. Exposed via the `extensions_source_inc` `cc_library`
(`textual_hdrs`). Include path `/Imain/extensions/source/inc` is in `_COPTS`.

### twain.h include path
`//main/twain:twain_headers` exposes `main/twain/inc/twain.h` as `<twain/twain.h>` via
`strip_include_prefix = "inc"` + `include_prefix = "twain"`, matching the old build's
`Package_inc.mk` which staged it to `inc/external/twain/twain.h`.

### Scanner (`scn.dll`)
Windows SLOFILES: `scnserv.cxx`, `scanner.cxx`, `scanwin.cxx` only. `twain.cxx` is NOT
compiled — `ImpTwain` is defined directly inside `scanwin.cxx`. `twain.hxx` is a textual
header included by `scanwin.cxx`. Needs `user32.lib` for window management symbols.

### bib_res .src file selection
`bib_res` uses an explicit srcs list (`bib.src`, `datman.src`, `sections.src`,
`toolbar.src`) rather than a glob. `hidother.src` uses new-style string HelpIDs with
`hidspecial` that rsc2 cannot parse; `menu.src` was never in dmake SLOFILES either.

### updchk.dll — Winsock2
`updchk.dll` links curl + OpenSSL statically and requires `ws2_32.lib` for all Winsock2
symbols (`WSAStartup`, `send`, `recv`, `closesocket`, etc.).

### update/check (`updchk.dll`)
In the old dmake build this was conditional on `ENABLE_ONLINE_UPDATE=YES`. In Bazel we
always build it; runtime behaviour is controlled by configuration XCU. Requires `curl`
static lib (`CURL_STATICLIB` define).

### COMPMOD_NAMESPACE
Required by `dbp`, `abp`, and `preload` to scope module-level singletons.

### oooimprovecore vs oooimprovement
Two separate DLLs — core is the heavy one (needs sfx2/svx deps), improvement is the
lightweight telemetry jobs DLL.
