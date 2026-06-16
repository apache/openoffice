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

## Resource images — `vcl_res` and `images_root` (radio/checkbox indicators)

`vcl_res` (the `//main/vcl:vcl_res` RSC target) compiles `source/src/*.src` into
`vcl_res.res`.  Among these, `source/src/images.src` declares the built-in control
indicator bitmaps as `Bitmap { File = "radio.png" }` / `"check.png"` (+ mono/win/unx/
mac/os2 variants), under resource ids `SV_RESID_BITMAP_RADIO` / `SV_RESID_BITMAP_CHECK`.

**The RSC pipeline does NOT embed bitmap pixel data into the `.res`.**  It stores only
the image *filename*.  At runtime VCL loads the bitmap by that name from
`share/config/images.zip`:

```
RadioButton::GetRadioImage()  (source/control/button.cxx)
  → LoadThemedImageList(... ResId(SV_RESID_BITMAP_RADIO) ...)
  → BitmapEx(ResId) reads "radio.png" from the .res
  → looks it up by EXACT name as a key in images.zip
```

`vcl_res.res` is therefore tiny (~23 KB = filename strings, not pixels) — confirm with
`grep -a -o '..radio.png' bazel-bin/main/vcl/vcl_res.res`.

Because of this, `vcl_res` MUST set `images_root = "main/default_images"` like every
other `rsc_res` module.  That makes RSC bake the image path *relative to that root*
(`vcl/res/radio.png`) instead of a bare basename (`radio.png`).  The bare name never
resolves: `images.zip` is built with `strip_prefix = "main/default_images"`, so its
keys are full paths (`vcl/res/radio.png`), and a lookup of bare `radio.png` always
misses → empty `ImageList` → `GetImage()` returns a 0-size image → the radio/checkbox
indicator is invisible and the control's text is laid out against a zero-width indicator
(visible as missing bullets + clipped/left-collapsed labels, e.g. Calc's *Delete Cells*
and *Insert Cells* dialogs).

This pairs with a matching requirement in `//main/default_images`: `vcl/res/**` PNG/BMP
files must be included in the `images.zip` (`_images_srcs`).  An earlier comment there
wrongly claimed vcl/res images were "baked into .res" and excluded them — that
assumption is false (see above) and was the second half of the same bug.

These indicator bitmaps are the *fallback* path: on themed Windows the radio/checkbox is
normally drawn natively (`ImplDrawRadioButtonState` → `DrawNativeControl`), and
`ImplGetRadioImageSize()` takes its size from `GetNativeControlRegion`.  The bitmap path
(and thus this bug) is exercised whenever native control drawing is not active for the
control, so the staged images must always be correct.

### Why the fallback was active at all — the debug manifest (visual styles)

`WinSalGraphics::IsNativeControlSupported` (win/source/gdi/salnativewidgets-luna.cxx)
returns TRUE only if `OpenThemeData(hWnd, L"Button")` succeeds, which requires the
process to have **Windows visual styles** enabled — i.e. a manifest declaring a
dependency on `Microsoft.Windows.Common-Controls version 6.0.0.0` (comctl32 v6).  The
*release* app manifest (`//main/desktop:util/soffice.exe.manifest`) declares it, but the
*debug* manifest (`//main/staging:soffice.exe.manifest`, used under
`--compilation_mode=dbg`) originally declared only the VC90 CRT assemblies.  So in debug
builds `OpenThemeData` returned NULL, `IsNativeControlSupported` was FALSE for every
control, and ALL radio/checkbox indicators fell to the bitmap path above — which is how
the missing-`radio.png` bug became visible.  Fixed by adding the Common-Controls
dependency to the debug manifest so debug matches release.  (Native theming being off in
debug also affected the look of every other themed control, not just radios.)

Triage: a missing toolbar/indicator/dialog image → `grep -a` the consuming `.res` for the
bare filename.  If the string is present, the `.res` *references* (does not embed) the
image, so the exact stored name MUST exist as an `images.zip` key.  A mismatch means
either `images_root` is unset (bare name baked) or the source dir was left out of
`_images_srcs`.
