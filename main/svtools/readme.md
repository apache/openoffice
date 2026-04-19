# svtools — Bazel Migration

## Targets

| Target | Output | Description |
|--------|--------|-------------|
| `:svt` | `svt.dll` | Main UI toolkit library (controls, dialogs, filters, graphics) |
| `:svt_implib` | `svt.lib` | Import library for downstream consumers |
| `:hatchwindowfactory` | `hatchwindowfactory.dll` | Embedded-object hatch-window UNO component |
| `:hatchwindowfactory_implib` | `hatchwindowfactory.lib` | Import library |
| `:svtools_headers` | — | Public headers (`inc/svtools/`) |

## Notes

- `source/filter/jpeg/jpegc.c` is a plain C file compiled alongside C++ sources.
- Depends on `@jpeg//:jpeg` (libjpeg-8d, registered in `ext_libraries/modules/jpeg/`).
- `@jpeg` registry entry added in this migration: `ext_libraries/modules/jpeg/8d/`.
  - Overlay provides patched `jconfig.h` (Windows-compatible) and `jmorecfg.h`
    (adds `#ifndef _SOLAR_H` guard to prevent INT32 redefinition with OOo headers).
- `icuuc` linked for collation/text services in control and i18n code.
- Windows system libs: `advapi32`, `gdi32`, `ole32`, `oleaut32`, `user32`, `uuid`.
