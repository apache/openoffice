# sfx2 — Bazel Migration

## Output
- `sfx.dll` — SFX application framework (document/view/dispatch infrastructure)

## Dependencies
Internal: basic (sb), xmlscript, framework (fwe), shell, sax, tools, svl, svtools, vcl,
toolkit, vos, sot, ucbhelper, unotools, comphelper, cppu, cppuhelper, sal, i18npool
External: libxml2 (DocumentMetadataAccess.cxx uses `<libxml/tree.h>`)

## SDI pipeline
`sdi_target` rule (`build/rules/sdi_target.bzl`) runs `svidl.exe` on `sdi/sfxslots.sdi`
to produce `sfxslots.hxx`.  The generated header is exposed as `//main/sfx2:sfxslots_headers`
with `includes = ["sfxslots_sdi_inc"]` so sources reach it via `#include "sfxslots.hxx"`.

svidl needs 10 runtime DLLs staged alongside it (sal3, tl, cppu3, comphelp,
cppuhelper3MSC, salhelper3MSC, ucbhelper, vos3, basegfx, i18nisolang1).

## Include path notes
- `source/inc/` is exposed as `:sfx2_source_inc` with `strip_include_prefix = "source/inc"`,
  making it a direct dep so its virtual-includes path appears before the transitive
  `framework_headers` path.  Without this, `#include "helpid.hrc"` finds framework's
  identically-named root-level header instead of sfx2's.
- `source/**/*.hrc` (appl, dialog, doc, view subdirs) declared in `:sfx2_source_hrc`
  so Bazel tracks them as action inputs.

## toolkit additions
- `toolkit_headers` extended with `inc/layout/**` (layout.hxx etc.)
- `toolkit_awt_vclxdialog` cc_library exposes `source/awt/vclxdialog.hxx` and
  `source/layout/core/*.hxx` at their expected `<toolkit/awt/...>` / `<layout/core/...>`
  include paths via `strip_include_prefix = "source"`.

## Excluded sources
- `source/appl/shutdowniconOs2.cxx` — OS/2 only
- `source/appl/shutdowniconaqua.mm` — macOS (Objective-C++)
- `source/appl/shutdowniconunx.cxx` — Linux/GTK
- `source/sidebar/SidebarPanel.cxx` — dead code; IDL was refactored (getHeightForWidth
  replaced getCanvas/getPositionOnScreen/getThemeProperties) but .cxx was not updated
- `source/layout/**` — ENABLE_LAYOUT=YES experimental feature; without the flag the
  classes duplicate source/dialog/ and cause multiply-defined symbol errors at link time

## Notes
- `ENABLE_QUICKSTART_APPLET` defined unconditionally (WNT branch in Library_sfx.mk)
- No `.def` file needed — exports use `SFX2_DLLIMPLEMENTATION` / `__declspec(dllexport)`
- `precompiled_sfx2.cxx` compiled as a regular source (no actual PCH)
