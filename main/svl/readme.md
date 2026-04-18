# svl — Bazel Migration

## Outputs
- `svl.dll` — main SVL library (item framework, number formatting, notify, misc utilities)
- `fsstorage.dll` — file system storage UNO component
- `passwordcontainer.dll` — password container UNO component

## Targets
| Target | Type | Notes |
|---|---|---|
| `svl_headers` | `cc_library` | Public headers from `inc/svl/**`; also exposes `inc/pch/precompiled_svl.hxx` |
| `svl` | `cc_binary` (linkshared) | Main DLL |
| `svl_implib` | `filegroup` | `.if.lib` for downstream linkers |
| `fsstorage` | `cc_binary` (linkshared) | UNO file system storage |
| `fsstorage_implib` | `filegroup` | `.if.lib` |
| `passwordcontainer` | `cc_binary` (linkshared) | UNO password container |
| `passwordcontainer_implib` | `filegroup` | `.if.lib` |

## Key decisions

### Multiple include paths needed
Several subdirectories use private headers from their own directories via bare `"file.hxx"` includes.
Added `/I` flags for each: `source/config`, `source/numbers`, `source/notify`, `source/items`.

### Textual includes (`nranges.cxx`, `rngitem_inc.cxx`)
Two `.cxx` files are `#include`'d by other TUs (not compiled standalone):
- `source/items/nranges.cxx` — included 2× by `itemset.cxx`
- `source/items/rngitem_inc.cxx` — included 3× by `rngitem.cxx` (macro-parameterized template pattern)

Both go in `_included_cxx_fragments` with `textual_hdrs`.

### DDE (Windows-only)
`source/svdde/` files are Windows-specific DDE support. Since we're building for Windows only, they're unconditionally included. The Unix stub (`unx/source/svdde/ddedummy.cxx`) is omitted.

### rsc_headers
`svl/srchitem.hxx` includes `rsc/rscsfx.hxx`. Added `rsc_headers` cc_library to `//main/rsc` exposing `inc/rsc/rscsfx.hxx`.

### UNO exports
All three DLLs use `SAL_DLLPUBLIC_EXPORT` / `SVL_DLLPUBLIC` (`__declspec(dllexport)`) for `component_getFactory` and `component_getImplementationEnvironment` — no `.def` files needed.

### Precompiled headers
The PCH stub (`inc/pch/precompiled_svl.cxx`) is compiled as an empty TU (the `.hxx` it includes is guarded by `#ifdef PRECOMPILED_HEADERS`). It's kept in srcs to avoid diverging from the original build list.

## Dependencies
- `//main/sal`, `//main/cppu`, `//main/cppuhelper` — core UNO
- `//main/sot`, `//main/tools`, `//main/unotools`, `//main/ucbhelper`, `//main/comphelper` — utility layers
- `//main/basegfx`, `//main/vos` — graphics/threading
- `//main/i18npool` (i18nisolang1), `//main/i18nutil` — locale/number formatting
- `//main/jvmfwk` — JVM framework integration
- `//main/rsc:rsc_headers` — `rscsfx.hxx` for item types
- `//main/offapi`, `//main/udkapi` — UNO API headers
