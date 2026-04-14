# i18npool migration notes

## Status
- `i18nisolang1.dll` ✅
- `i18npaper.dll` ✅
- `i18npool.dll` ✅
- `i18nsearch.dll` ✅ (builds alongside i18npool)

## breakiter_pipeline.bzl — symbol naming fix

**Problem (resolved):** `OpenOffice_dat.obj` had 13 unresolved externals like:
```
LNK2001: unresolved external symbol _OpenOffice_bazel_out_x64_windows_fastbuild_bin_main_i18npool_pool_char_in_patched_brk
```

**Root cause:** In `-S` (sourceTOC) mode, `gencmn` derives the C symbol name by replacing
`.`, `-`, `/` with `_` and prepending the package name.  With full exec-root paths in the
list file, every path separator became an underscore, producing a massively long symbol.
`genccode` internally calls `findBasename()` before building its symbol, so it always uses
just the filename stem.

**Fix (breakiter_pipeline.bzl line 74):** Changed gencmn list file from `b.path`
(exec-root-relative full path) to `b.basename`.  In `-S` mode gencmn never opens the
`.brk` files, so no source-directory flag is needed.

## ICU 49.1.2 setup

ICU 72.1 is the only version in ext_sources but requires C++17 — won't compile with VS2008.
ICU 49.1.2 is the newest VS2008-compatible ICU (official VS2008 project files included).

Source archive: `ext_sources/download.icu-project.org/files/icu4c/49.1.2/icu4c-49_1_2-src.tgz`

ICU module structure:
- `ext_libraries/modules/icu/49.1.2/overlay/BUILD.bazel` — cc_library targets for icuuc/icui18n/icudata
  static builds for build tools (genbrk/gencmn/genccode), shared DLLs for i18npool/i18nsearch
- `ext_libraries/modules/icu/49.1.2/overlay/MODULE.bazel` — module declaration
- `main/i18npool/breakiter_pipeline.bzl` — .txt → genbrk → .brk → genccode → _brk.c
  + gencmn → OpenOffice_dat.c

## char_in.txt patch

`char_in-icu49.patch` removes the `$Prepend` variable and its two rules.  ICU 4.6+
treats empty property sets as an error; `$Prepend` was empty in Unicode 6.0 / ICU 49.
The genrule `char_in_txt_patched` applies this via `findstr /v`.

`line.txt` and `sent.txt` are omitted entirely:
`BreakIterator_Unicode::loadICUBreakIterator()` falls back to ICU's built-in rules
when the OpenOffice-specific files cannot be loaded.

## i18nisolang1 and i18npaper

- `inwnt.cxx` is `#include`'d by `insys.cxx` via a platform guard — add
  `cc_library(textual_hdrs = [...])` dep.
- `i18npaper` links against `i18nisolang1` (for `MsLangId` class).

For VS2008/ICU version constraints see `ext_libraries/modules/icu/Readme.md`.
