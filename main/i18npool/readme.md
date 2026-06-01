# i18npool migration notes

## Status
- `i18nisolang1.dll` ✅
- `i18npaper.dll` ✅
- `i18npool.dll` ✅
- `i18nsearch.dll` ✅ (builds alongside i18npool)
- `localedata_{en,es,euro,others}.dll` ✅ (194 locales)

## localedata — per-locale data libraries

**Why it matters:** `i18npool.dll` only carries `LocaleDataImpl` (the *loader*).
The actual locale data (calendars, currencies, collation, number formats,
character classification) lives in four data DLLs that `LocaleDataImpl::getLocaleItem()`
loads on demand with `osl_loadModule(localedata_<group>)` and queries via
`getXxx_<lang>_<CC>()` functions.  Before this work none of them were built, so
**every** locale lookup threw.  The Start Center lays out no editable text and so
survived, but opening Writer/Calc threw during view init (swallowed) and left the
document frame as dead chrome — toolbars/menus visible but inert, no caret, no
typing.  Building + staging the four DLLs fixes that.

**Pipeline** (mirrors `source/localedata/{makefile.mk,data/makefile.mk}`):

```
data/<lang>_<CC>.xml --saxparser--> localedata_<lang>_<CC>.cxx  --cl/link--> localedata_<group>.dll
```

- **`saxparser`** (`cc_binary`, this BUILD): the APP1 tool from
  `source/localedata/makefile.mk` (`saxparser.cxx` + `LocaleNode.cxx` +
  `filewriter.cxx`).  It is a *UNO-bootstrapping* executable —
  `createRegistryServiceFactory(saxparser.rdb, types.rdb)` builds a service
  manager, instantiates `com.sun.star.xml.sax.Parser`, parses one locale XML and
  writes the generated `.cxx`.  Generated functions are `SAL_DLLPUBLIC_EXPORT`
  (`__declspec(dllexport)`) so **no .def/.map is needed** on Windows.

- **`saxparser_rdb`** (`services_rdb`): registers the SAX Parser → `expwrap.dll`
  with a build-time (NONE-layer) URI
  `vnd.sun.star.expand:$OOO_INBUILD_SHAREDLIB_DIR/expwrap.dll`, resolved against
  the staged tools dir at codegen time (mirrors dmake's `expwrap.inbuild.component`).

- **`localedata_gen`** (`//build/rules:localedata_pipeline.bzl`): one saxparser
  invocation per locale XML.  Stages saxparser.exe + the DLLs its UNO bootstrap
  needs into one tools dir (all `cfg="exec"` → a self-consistent release tool set;
  only sal-allocator UNO types cross the boundary, so no CRT clash with the target
  dbg DLLs).  The bootstrap DLL set, discovered empirically, is:
  `sal3, cppu3, cppuhelper3MSC, salhelper3MSC, reg, store, expwrap,
   bootstrap.uno (SharedLibrary loader + smgr + simple registry),
   xmlreader (parses the XML components rdb), msci_uno (binary C++<=>UNO bridge)`.
  Invocation is delegated to `build/tools/run_saxparser.pl` because saxparser needs
  ABSOLUTE `file:///` URLs for the two rdbs and for `OOO_INBUILD_SHAREDLIB_DIR`
  (osl cannot resolve relative paths on Windows); the XML input and `.cxx` output
  stay relative (plain `fopen` against the execroot cwd).

- **The 4 DLLs**: group membership copied verbatim from `data/makefile.mk`
  `SHL{1,2,3,4}OBJS` (en=14, es=20, euro=75, others=85 = 194 total).
  `localedata_en` is the base; `es/euro/others` import `getXxx_en_US()` from it
  (makefile `SHL{2,3,4}STDLIBS = LINK_LOCALEDATA_EN_LIB`), so they link
  `:localedata_en_implib`.  Pure data → no deps beyond `sal_headers` (for
  `sal/types.h`) + `/Zc:wchar_t-`.  Staged into `program/` by `//main/staging`.

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
