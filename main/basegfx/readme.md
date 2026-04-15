# basegfx — Bazel Migration Notes

## Output
`basegfx.dll` — base graphics primitives (2D/3D geometry, polygons, colour, matrix math)

## Targets
| Label | Output |
|---|---|
| `//main/basegfx:basegfx` | `basegfx.dll` |
| `//main/basegfx:basegfx_headers` | public headers under `inc/` |
| `//main/basegfx:basegfx_implib` | `basegfx.if.lib` (for downstream linkopts) |

## Key decisions

### o3tl dependency
`b2dclipstate.hxx` includes `<o3tl/cow_wrapper.hxx>`. Added `//main/o3tl:o3tl` to both
`basegfx_headers` and the `basegfx` binary.

### `/Zc:wchar_t-`
`stringconversiontools.hxx` uses `sal_Unicode` directly — the VS2008 native `wchar_t` vs
`unsigned short` mismatch requires this flag.

### No stlport needed
basegfx uses boost (shared_ptr, scoped_ptr, bind, mem_fn, tuple) but not `unordered_map` /
`hash_map`, so `//main/stlport:stlport` is not required.

### No DEF file
Exports are via `BASEGFX_DLLPUBLIC` / `SAL_DLLPUBLIC_EXPORT` (`__declspec(dllexport)`) — no
`.map` / `.def` file exists in the source tree and none is needed.

### precompiled_basegfx.hxx
The PCH header is essentially empty (`#ifdef PRECOMPILED_HEADERS` with no content). It is
referenced by all `.cxx` files via `#include "precompiled_basegfx.hxx"` but resolved through
`/Imain/basegfx/inc/pch` which maps to the empty stub — no action needed.

## Dependencies
- `//main/sal:sal_headers` + implib
- `//main/cppu:cppu3_implib`
- `//main/cppuhelper:cppuhelper_implib`
- `//main/o3tl:o3tl` (header-only)
- `//main/udkapi:udkapi_idl_headers` (com/sun/star geometry, rendering, awt)
- `//main/offapi:offapi_idl_headers`
- `@boost.legacy//:boost.legacy`
