# l10ntools — Bazel migration notes

## Targets

| Target | Output | Description |
|--------|--------|-------------|
| `transex3` | `transex3.exe` | Extractor/merger for `*.src` and `*.hrc` resource files |
| `helpex` | `helpex.exe` | Extractor/merger for help XML files; uses expat |
| `ulfex` | `ulfex.exe` | Extractor/merger for `*.ulf` / `*.lng` locale files |
| `gsiconv` | `gsiconv.exe` | Encoding converter for `*.gsi` files |
| `gsicheck` | `gsicheck.exe` | Tag checker / validator for `*.gsi` files |
| `cfgex` | `cfgex.exe` | Extractor/merger for `*.xcd` / `*.xcu` / `*.xcs` config files |
| `xrmex` | `xrmex.exe` | Extractor/merger for `*.xrm` / `*.xml` property files |
| `localize_sl` | `localize_sl.exe` | Source-tree localiser; orchestrates all the above tools |
| `HelpLinker` | `HelpLinker.exe` | Standalone help compilation binary (libxslt + expat) |
| `helplinker` | `helplinker.dll` | DLL form of HelpLinker; exports `compileExtensionHelp` for sfx2 |
| `helplinker_implib` | `helplinker.if.lib` | Import lib for `helplinker.dll` |

Internal-only cc_library targets (`transex_lib`, `src_scanner`, `cfg_scanner`, `xrm_scanner`) are
not listed above.

## Key implementation notes

### Pre-generated flex scanners
Three lex grammars (`srclex.l`, `cfglex.l`, `xrmlex.l`) produce C scanners. flex is not
available in the Bazel action environment, so the output is generated once from Cygwin and
checked in as `source/src_yy.c`, `source/cfg_yy.c`, `source/xrm_yy.c` — identical to the
idlc/rsc approach.

The original build compiled a wrapper (`src_yy_wrapper.c` etc.) that just does
`#include "src_yy.c"` to allow warning suppression. In Bazel the wrapper is the `srcs` entry
and the generated file is a `textual_hdrs` entry; `/Imain/l10ntools/source` ensures the
`#include "src_yy.c"` resolves correctly from the workspace root.

`YY_NO_UNISTD_H` is required globally — the flex-generated code includes `<unistd.h>` unless
this macro is defined.

### `std::hash_map` and `<unistd.h>` in public headers
Both `inc/export.hxx` and `inc/tagtest.hxx` use `std::hash_map` (from `<hash_map>`) and
include `<unistd.h>` unconditionally. Because these are pulled in by nearly every source file:
- `//main/stlport:stlport` is in `_TOOL_DEPS` (shared by all targets)
- `/Imain/soltools/winunistd` is in `_COPTS` (shared by all targets)

### `directory.cxx` — Windows vs POSIX
`inc/l10ntools/directory.hxx` guards `dirent.h` / `unistd.h` with `#ifndef WNT`.
`source/directory.cxx` has a full `#ifdef WNT` block using `FindFirstFile`/`FindNextFile`
so no extra compat header is needed for this file.

### `localize_sl` — ICU dependency
`localize.cxx` uses `<unicode/regex.h>` and `<unicode/unistr.h>` (ICU).
`localize_sl` links `@icu//:icuuc_implib`, `@icu//:icui18n_implib`, `@icu//:icudata_implib`.

### `helplinker.dll` DEF file
`source/help/helplinker.def` exports a single symbol:
```
compileExtensionHelp @1
```
The `HELPLINKER_DLLIMPLEMENTATION` define switches `HELPLINKER_DLLPUBLIC` from
`SAL_DLLPUBLIC_IMPORT` to `SAL_DLLPUBLIC_EXPORT` (see `compilehelp.hxx`).

### libxslt `attributes.c` missing from upstream overlay
The Bazel overlay for `libxslt@1.1.35` at
`ext_libraries/modules/libxslt/1.1.35/overlay/BUILD.bazel` was missing
`libxslt/attributes.c` from `LIBXSLT_SRCS`, causing five unresolved externals at link time
(`xsltApplyAttributeSet`, `xsltAttribute`, etc.). Added and `source.json` hash updated.

### Java targets not migrated
`java/jpropex`, `source/filter/merge` (FCFGMerge.jar), `source/filter/utils`, and
`source/help/HelpIndexerTool.jar` are Java build artefacts. They are not yet wired into Bazel
— the Java build pipeline (ridljar, jurt, etc.) is still pending.
