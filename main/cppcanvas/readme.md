# cppcanvas — Bazel Migration

## Output
- `cppcanvas.dll` — C++ wrapper over the UNO canvas API

## Targets
- `cppcanvas_headers` — public headers (`inc/cppcanvas/`, `inc/pch/`)
- `cppcanvas` — shared library
- `cppcanvas_implib` — import library filegroup for downstream linkers

## Dependencies
comphelper, cppuhelper, offapi, tools, vcl, basegfx, canvas (canvastools), i18nisolang1

## Notes
- No DEF file; exports via `CPPCANVAS_DLLPUBLIC` (`SAL_DLLPUBLIC_EXPORT` decoration)
- Internal headers in `source/inc/` included via `/Imain/cppcanvas/source/inc`
