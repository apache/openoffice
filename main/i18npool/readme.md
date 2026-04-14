# Current situation — active linking bug

**Problem:** i18npool.dll fails to link; `OpenOffice_dat.obj` has 13 unresolved externals like:
```
LNK2001: unresolved external symbol _OpenOffice_bazel_out_x64_windows_fastbuild_bin_main_i18npool_pool_char_in_patched_brk
```

**Root cause:** Naming inconsistency between genccode and gencmn:
- `genccode` receives `brk.path` (full path) but internally calls `findBasename()` → symbol is `OpenOffice_char_in_patched_brk`
- `gencmn` list file uses `b.path` (full path) → symbol is `OpenOffice_bazel_out_..._char_in_patched_brk`
- The compiled `_brk.c` objects and `OpenOffice_dat.c` use two different symbol sets.

**Fix needed** in [main/i18npool/breakiter_pipeline.bzl](breakiter_pipeline.bzl) line 74:
Change the gencmn list file from `b.path` (full exec-root paths) to basenames.
gencmn must then be run from the directory containing the `.brk` files.

**Solution options:**
1. Use `ctx.actions.run_shell` to `cd` into the `.brk` output directory before running gencmn (basename-only list)
2. Find a gencmn flag that strips paths from symbol names

---

# Notes for i18npool ICU setup (pending)
ICU 72.1 is the only version in ext_sources but requires C++17 — won't compile with VS2008.
ICU 49.1.2 is the newest VS2008-compatible ICU (official VS2008 project files included).

To unblock i18npool.dll:
1. Download `icu4c-49_1_2-src.tgz` from https://download.icu-project.org/files/icu4c/49.1.2/
2. Place at `ext_sources/download.icu-project.org/files/icu4c/49.1.2/icu4c-49_1_2-src.tgz`
3. Compute SHA256: `openssl dgst -sha256 -binary <file> | openssl base64`
4. Update `ext_libraries/modules/icu/49.1.2/source.json` with the real integrity hash
5. Uncomment `bazel_dep(name = "icu", version = "49.1.2")` in MODULE.bazel
6. Run `bazel mod deps --lockfile_mode=refresh` then build `//main/i18npool/pool:i18npool` 

# Notes for i18nisolang1 and i18npaper (done)
- inwnt.cxx is #included by insys.cxx via platform guard — add `cc_library(textual_hdrs = [...])` dep
- i18npaper links against i18nisolang1 (for MsLangId class)

For VS2008/ICU version constraints and long-term compiler options, see ext_libraries/modules/icu/Readme.md.

ICU module structure already set up:
- `ext_libraries/modules/icu/49.1.2/overlay/BUILD.bazel` — cc_library targets for icuuc/icui18n/icudata
  static builds for build tools (genbrk/gencmn/genccode), shared DLLs for i18npool/i18nsearch
- `ext_libraries/modules/icu/49.1.2/overlay/MODULE.bazel` — module declaration
- `main/i18npool/breakiter_pipeline.bzl` — Starlark rule: .txt → genbrk → .brk → genccode → _brk.c
  + gencmn → OpenOffice_dat.c (no bash genrule — uses ctx.actions.run)
- `main/i18npool/pool/BUILD.bazel` still TODO (i18npool.dll and i18nsearch.dll targets)