# lingucomponent — Bazel migration

Produces four UNO component DLLs for OpenOffice linguistic services.

## Targets

| Target | Output | Library |
|--------|--------|---------|
| `//main/lingucomponent:spell` | `spell.dll` | `@hunspell` (1.3.3) |
| `//main/lingucomponent:hyphen_uno` | `hyphen.dll` | `@hyphen` (2.7.1) |
| `//main/lingucomponent:lnth` | `lnth.dll` | `@mythes` (1.2.0) |
| `//main/lingucomponent:guesslang` | `guesslang.dll` | `@libtextcat` (2.2) |

Shared utility static library `ulingu` (from `source/lingutil/lingutil.cxx`) is
linked into all four DLLs.

## External libraries added to registry

All four libs are in `ext_libraries/modules/` as full Bazel registry entries:

- **hunspell 1.3.3** — spellchecker; patch: integer overflow fix in `affixmgr.cxx`; `includes = ["src/hunspell", "src/win_api"]` (raw `/I` copts are execroot-relative, `includes` is package-relative)
- **hyphen 2.7.1** — hyphenator; minimal overlay (MODULE.bazel + BUILD.bazel)
- **mythes 1.2.0** — thesaurus; minimal overlay (MODULE.bazel + BUILD.bazel)
- **libtextcat 2.2** — language guesser; all changes in overlay (no patches — Bazel's Java patcher has no fuzz tolerance and cannot create new files):
  - `src/config.h`: `#define inline __inline` for VS2008 C89 mode
  - `src/constants.h`: adds `_UTF8_`, fixes `INVALID(c)` cast, `MINDOCSIZE=6`, `DEFAULT_FINGERPRINTS_PATH ""`
  - `src/common.h` / `src/common.c`: `#ifdef HAVE_GETTIMEOFDAY` guards (not available on Windows)
  - `src/fingerprint.c`: `issame()` made `static inline` (avoids duplicate symbol with utf8misc.obj); includes `utf8misc.h`
  - `src/fingerprint.h`: adds `extern "C"` guards so C++ callers get correct C linkage for `fp_Name` etc.
  - `src/textcat.c` / `src/textcat.h`: adds `special_textcat_Init(conffile, prefix)` for prefix-based fingerprint loading and per-language disable flags
  - `src/utf8misc.h` / `src/utf8misc.c`: UTF-8 character utilities (new files, not in original tarball)

## libtextcat include path

Consumer code uses `<libtextcat/textcat.h>` etc.  The registry BUILD uses
`strip_include_prefix = "src"` + `include_prefix = "libtextcat"` to map
`src/textcat.h` → `<libtextcat/textcat.h>`.

## Key flags

- `WNT` + `GUI` — required by `sal/types.h` to find platform type sizes
- `CPPU_ENV=msci` + `INTEL` + `_X86_=1` — required by `uno/lbnames.h`
- `/Zc:wchar_t-` — required for `sal_Unicode` compatibility
- `snprintf=_snprintf` — MSVCRT compatibility
- `SUPD=680` — Solar Update version define
- PCH headers (`inc/pch/*.hxx`) are no-ops unless `PRECOMPILED_HEADERS` is defined
