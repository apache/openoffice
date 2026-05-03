# cui — Common UI Dialogs

Builds `cui.dll` — the shared dialog library used by all OpenOffice applications.

## Outputs

| Target | Output |
|---|---|
| `//main/cui:cui` | `cui.dll` |
| `//main/cui:cui_implib` | `cui.lib` (import library) |

## Exports

- `component_getImplementationEnvironment`, `component_getFactory` — UNO component (ColorPicker service)
- `CreateDialogFactory` — returns a `VclAbstractDialogFactory*` for all shared dialogs

## Source layout

| Directory | Contents |
|---|---|
| `source/customize/` | Toolbar/menu customization dialogs |
| `source/dialogs/` | General-purpose dialogs (character map, spell, gallery, hyperlink…) |
| `source/factory/` | Dialog factory and resource manager |
| `source/options/` | Tools → Options dialog pages |
| `source/tabpages/` | Shared tab pages (border, background, paragraph, area…) |
| `source/uno/services/` | UNO component registration |

## Key BUILD notes

- `cui_source_inc` (with `strip_include_prefix = "source/inc"`) must be listed **first** in deps so that `"helpid.hrc"` resolves to cui's version rather than the identically-named header pulled in transitively via `framework_headers`.
- `cui_source_hrc` declares the per-subdirectory `.hrc` files as textual inputs; they are located at compile time via the per-subdir `/I` copts entries.
- `stlport` required: `source/customize/cfg.cxx` uses `::std::hash_map`.
- `.src` resource files (88 files) are not yet compiled — deferred until the rsc pipeline is migrated.
