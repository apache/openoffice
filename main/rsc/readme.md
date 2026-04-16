# rsc — Resource Compiler

## What rsc does

rsc is a **UI resource compiler** — an AOO-specific build tool (not shipped to end users).
It processes `.src` files that describe UI resources (strings, dialogs, menus, bitmaps) and
produces binary `.res` files that VCL loads at runtime to draw the application UI.

Example `.src` input:
```c
String AVMEDIA_STR_PLAY { Text [ en-US ] = "Play" ; };
ToolBox TB_CONTROLS { Button BTN_PLAY { ... }; };
```

There are 703 `.src` files in the AOO tree.

## Three executables

| Executable | Role |
|---|---|
| `rscpp` | Standalone C preprocessor — handles `#include` and `#define` in `.src` files. Six plain C files, no bison. |
| `rsc2` | The actual compiler: parses preprocessed `.src` text → binary `.res` (and optionally `.srs`, `.hxx`, `.cxx`). Uses the bison-generated parser. |
| `rsc` | Thin launcher that orchestrates rscpp → rsc2. |

## The bison grammar (yyrscyacc.y)

`source/parser/yyrscyacc.y` is the LALR(1) grammar for the `.src` language — class
declarations, string tables, dialog layouts, etc. It is an AOO-specific format; nothing
outside AOO/LibreOffice uses it.

### Pre-generated output

`source/parser/yyrscyacc.cxx` is generated from `yyrscyacc.y` by bison and **checked into
the tree** (same approach as idlc). It is not regenerated during the Bazel build.

Rationale: the `.src` grammar is completely frozen — AOO has not added a new resource type in
over a decade and is in maintenance mode. bison would never be invoked again in this codebase's
lifetime. The pre-generation approach eliminates bison as a build-time dependency entirely.

To regenerate if ever needed:
```bash
bison -o source/parser/yyrscyacc.cxx source/parser/yyrscyacc.y
```

`rscyacc.cxx` wraps the output with MSVC warning suppression:
```cpp
#if defined _MSC_VER
#pragma warning(push, 1)
#pragma warning(disable:4129 4273 4701)
#endif
#include "yyrscyacc.cxx"
#if defined _MSC_VER
#pragma warning(pop)
#endif
```

## What a modern rsc would look like

LibreOffice (the active fork) has already replaced this stack:

| Old (AOO / rsc) | Modern (LibreOffice) |
|---|---|
| `.src` + `rsc2` → `.res` | `.po` files → GNU gettext `msgfmt` → `.mo` |
| Binary VCL resource loading | UI description in `.ui` XML (GtkBuilder / Glade format) |
| Inline dialog layout in `.src` | `vcl/uiconfig/*.ui` |
| String tables in `.src` | `strings.po` translated via Weblate |

Replacing rsc in AOO would require porting all 703 `.src` files — a complete rewrite of the UI
layer. It is out of scope for this migration.

## Migration status

`rscpp`, `rsc`, `rsc2` ✅ — `main/rsc/BUILD.bazel`

Dependencies: `//main/sal`, `//main/tools` (tl), `//main/i18npool` (i18nisolang1), `//main/vos` (vos3), `//main/stlport` (for `std::hash_map` in rschash.hxx)

### Build notes
- `cppmain.c` excluded from `rscpp`: on MSVC the `MAIN` macro in `cpp.h` expands to `__cdecl main`, so `cpp1.c` already provides `main()` — compiling `cppmain.c` causes a duplicate symbol.
- `yyrscyacc.cxx` and `yyrscyacc.hxx` are pre-generated and checked in. A `cc_library` wrapper (`rsc2_parser`) is needed because `cc_binary` does not support `textual_hdrs`; `rscyacc.cxx` `#include`s `yyrscyacc.cxx` directly.
- Include path `/Imain/rsc/source/parser` added to shared copts so `rsclex.cxx` can find `<yyrscyacc.hxx>` (angle-bracket include).
