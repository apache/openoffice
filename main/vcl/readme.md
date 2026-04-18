# vcl — Bazel Migration

## Output
- `vcl.dll` — Visual Class Library, the OpenOffice GUI abstraction layer

## Target
```
bazel build //main/vcl:vcl
```

## Sources
Cross-platform code in `source/` (app, components, control, fontsubset, gdi, helper, salmain, window).
Windows platform implementation in `win/source/` (app, gdi, window).

`source/glyphs/` is excluded — those files are unx-only (freetype/graphite font rasterizer).
On Windows, font layout uses the native GDI path (`win/source/gdi/winlayout.cxx`).

## Key decisions
- Entry point `/ENTRY:LibMain@12`: `win/source/app/salshl.cxx` defines both `DllMain` and
  `LibMain` (LibMain delegates to DllMain); the old makefile used this entry point for MSVC x86.
- ICU: links only `icuuc` (ubidi + uchar in `source/gdi/sallayout.cxx`). No `icule` needed —
  on Windows font layout goes through GDI, not ICU Layout Engine.
- `_WIN32_WINNT=0x0500` (XP compat) kept; `win/wincompat.h` force-included via `/FI` to supply
  APPCOMMAND constants SDK 7.0 guards with `>= 0x0600` but SDK 6.0A exposed unconditionally.
- `CUI_DLL_NAME` defined as `"cui.dll"` — replaces the version-stamped name the old build system
  computed from gbuild macros.
- `version.lib` required for `VerQueryValueW`/`GetFileVersionInfoW` in `win/source/gdi/winlayout.cxx`.
- `/Zc:wchar_t-` required for `sal_Unicode` compatibility (see CLAUDE.md cross-cutting flags).
- Private headers in `inc/`, `source/window/`, `source/gdi/`, `source/fontsubset/` are included
  via copts `/I` paths and not exported in `vcl_headers`.
- `//main/icc:srgb_icc` provides `<icc/sRGB-IEC61966-2.1.hxx>` used by `source/gdi/pdfwriter_impl.cxx`.
