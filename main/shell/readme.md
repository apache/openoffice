# shell — Bazel migration notes

## Targets

| Target | Output | Description |
|--------|--------|-------------|
| `syssh.uno` | `syssh.uno.dll` | UNO component: SystemShellExecute (opens URLs via Windows shell) |
| `localebe1.uno` | `localebe1.uno.dll` | UNO component: LocaleBackend (reads system locale) |
| `wininetbe1.uno` | `wininetbe1.uno.dll` | UNO component: WinInetBackend (reads proxy settings via WinInet) |
| `shlxthdl` | `shlxthdl.dll` | COM shell extension: property sheets, info tips, column info, thumbnails for ODF files |
| `ooofilt` | `ooofilt.dll` | COM shell extension: IFilter implementation for Windows Search indexing |
| `propertyhdl` | `propertyhdl.dll` | COM shell extension: IPropertyStore handler for Vista+ Explorer Details pane |
| `lngconvex` | `lngconvex.exe` | Build tool: converts .ulf locale files to .rc resource files |
| `regsvrex` | `regsvrex.exe` | Utility: registers/unregisters shell extension DLLs |

## Key implementation notes

### minizip (`@zlib//:minizip`)
`shell/inc/internal/zipfile.hxx` includes `<external/zlib/unzip.h>` (OOo solver path convention).
The `@zlib//:minizip` target in the zlib overlay exposes contrib/minizip headers under
`external/zlib/` via `strip_include_prefix = "contrib/minizip"` + `include_prefix = "external/zlib"`.

`@zlib//:zlib_compat_hdrs` similarly exposes `zlib.h`/`zconf.h` under `external/zlib/` for
`shell/inc/internal/types.hxx` which does `#include <external/zlib/zlib.h>`.

### `NOMINMAX` + min/max
The toolchain sets `/DNOMINMAX` globally, so `<windows.h>` does not define `min`/`max` macros.
GdiplusTypes.h (used by thumbviewer) and several shell sources use bare `min(a,b)`.
Fix: `_SHLXT_DEFINES` maps `min=__min` and `max=__max` to the MSVC intrinsics.

### `stream_helper.cxx` shared between ooofilt and propertyhdl
In the original dmake build both share the same `$(SLO)/stream_helper.obj`.
In Bazel, `propertyhdl` lists `source/win32/shlxthandler/ooofilt/stream_helper.cxx`
directly in its `srcs` to get the `PrepareIStream` symbol.

### localebe1 / wininetbe1 local includes
The `cdef.cxx` files include local headers with bare names (e.g., `"localebackend.hxx"`).
Bazel compiles from workspace root, so explicit `/Imain/shell/source/backends/<name>` is needed.

### lngconvex resource pipeline
`lngconvex.exe` converts `.ulf` files → `.rc` for `shlxthdl.dll`.
The `.rc` → `.res` compilation step (MSVC `rc.exe`) and the resource link are not yet
wired into Bazel — `shlxthdl.dll` builds without embedded resources for now.
