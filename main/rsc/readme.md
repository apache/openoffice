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
| `rsc2` | The actual compiler: parses preprocessed `.src` text → binary `.res`. Uses the bison-generated parser. |
| `rsc` | Thin launcher that orchestrates rscpp → rsc2 (not used in the Bazel pipeline). |

## The bison grammar (yyrscyacc.y)

`source/parser/yyrscyacc.y` is the LALR(1) grammar for the `.src` language — class
declarations, string tables, dialog layouts, etc. It is an AOO-specific format; nothing
outside AOO/LibreOffice uses it.

### Pre-generated output

`source/parser/yyrscyacc.cxx` is generated from `yyrscyacc.y` by bison and **checked into
the tree** (same approach as idlc). It is not regenerated during the Bazel build.

Rationale: the `.src` grammar is completely frozen — AOO has not added a new resource type in
over a decade and is in maintenance mode. The pre-generation approach eliminates bison as a
build-time dependency entirely.

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
| String tables in `.src` | `strings.po` translated via translation page |

Replacing rsc in AOO would require porting all 703 `.src` files — a complete rewrite of the UI
layer. It is out of scope for this migration.

## Migration status

`rscpp`, `rsc`, `rsc2` ✅ — `main/rsc/BUILD.bazel`

Dependencies: `//main/sal`, `//main/tools` (tl), `//main/i18npool` (i18nisolang1), `//main/vos` (vos3), `//main/stlport` (for `std::hash_map` in rschash.hxx)

### Build notes
- `cppmain.c` excluded from `rscpp`: on MSVC the `MAIN` macro in `cpp.h` expands to `__cdecl main`, so `cpp1.c` already provides `main()` — compiling `cppmain.c` causes a duplicate symbol.
- `yyrscyacc.cxx` and `yyrscyacc.hxx` are pre-generated and checked in. A `cc_library` wrapper (`rsc2_parser`) is needed because `cc_binary` does not support `textual_hdrs`; `rscyacc.cxx` `#include`s `yyrscyacc.cxx` directly.
- Include path `/Imain/rsc/source/parser` added to shared copts so `rsclex.cxx` can find `<yyrscyacc.hxx>` (angle-bracket include).

## rsc_pipeline.bzl — Bazel rule for .res compilation

The shared rule lives at `//build/rules:rsc_pipeline.bzl`.

### Two-step pipeline (rscpp + rsc2 directly)

The Bazel rule does NOT use the `rsc.exe` launcher. Instead it calls rscpp and rsc2
as separate Bazel actions, giving full control over each action's inputs, outputs, and
environment. This avoids subprocess-spawning issues (CWD inheritance, path resolution)
that arise when rsc.exe forks child processes.

**Step 1 — RscPreprocess** (one action per `.src` file):
```
rscpp.exe  -I<inc>...  -D<def>...  input.src  output.src.srs
```

**Step 2 — Rsc2Compile** (one action for all intermediates):
```
rsc2.exe  -fs=<name>.res  -lgEN_US  -BIGENDIAN
          -I<inc>...  -D<def>...
          -lip=<tools_dir/X/res>  -lip=<tools_dir/Y/res>  ...
          -subimages=<tools_dir>
          <all intermediate .src.srs files>
```

> **Why `-BIGENDIAN`?**  `GetLong`, `GetShort`, and `GetUInt64` in `tl!ResMgr` are unconditionally
> big-endian readers — the `.res` binary format is always big-endian regardless of host platform.
> `rsc2`'s own default is also `BIGENDIAN`.  Passing `-LITTLEENDIAN` causes a byte-order mismatch:
> the content-table length stored at the end of the file is misread as ~2.8 GB, `rtl_allocateMemory`
> returns `NULL`, `nEntries` is non-zero (huge), and `GetUInt64(NULL)` crashes immediately.

#### Image lookup: -lip=, -subimages=, and images_root

`BitmapEx(ResId)` (the runtime image loader) reads a **filename string** from the `.res` file,
then calls `ImplImageTree::loadImage(name, style, bitmap)` which does an **exact name lookup**
inside `share/config/images.zip`.  The path stored in the `.res` must therefore match the zip
entry name exactly.

`images.zip` is built with `strip_prefix = "main/default_images"`, so entries are stored at paths
like `"framework/res/backing.png"` or `"res/odt_32.png"`.

`images_root = "main/default_images"` in every `rsc_res` rule causes images to be staged in
`<name>_tools/<zip-entry-path>` (e.g. `tools/framework/res/backing.png`).  The rsc_pipeline.bzl
then passes one `-lip=<dir>` flag per unique staging subdirectory so rsc2 can find each image by
its bare `File = "backing.png"` declaration.  `-subimages=<tools_dir>` strips the tools prefix
from the found path, leaving the relative portion (`"framework/res/backing.png"`) which is what
gets written into the `.res` and later matched exactly in images.zip.

`-subimages=<dir>` is equally required: rsc2's `GetImageFilePath()` only sets `bFound=true` when
a `-sub<key>=<path>` replacement matches as a prefix of the found file's path.  Without it, every
image is found on disk but never recorded, producing the f268 "could not be found" error.

**Critical invariant:** `images_root` must equal the `strip_prefix` used when building
`images.zip` (`"main/default_images"`).  A module-specific subdirectory
(e.g. `"main/default_images/framework/res"`) stages images flat → stores bare basenames →
mismatches the zip's full relative paths → all `BitmapEx(ResId)` loads return empty bitmaps.

#### .res vs .srs output mode

rsc2 has two mutually exclusive output modes:
- **Normal mode** (no `-s` flag): writes `.res` only — used by `rsc_res`
- **NOLINK mode** (`-s` flag): writes `.srs` only, skips `.res`

The `rsc_res` rule runs in normal mode and declares only `.res` as the output.

### Tool staging

Each `rsc_res` rule stages its own `<name>_tools/` directory containing:
- `rscpp.exe` + `rsc2.exe` (the two tool executables)
- All runtime DLLs: `sal3`, `tl`, `cppu3`, `cppuhelper3MSC`, `salhelper3MSC`,
  `comphelpMSC`, `ucbhelperMSC`, `basegfx`, `vos3MSC`, `i18nisolang1MSC`, CRT
- `.manifest` files for rscpp.exe and rsc2.exe (VS2008 SxS CRT loading)
- All image files (`.png`/`.bmp`) staged at their relative path from `images_root`
  (e.g. `tools/framework/res/backing.png` for a file from `framework/res/`)

`env = {"PATH": tools_dir}` on every action; Windows DLL loader finds everything.

### Usage example

```python
load("//build/rules:rsc_pipeline.bzl", "rsc_res")

# Module with images loaded at runtime via BitmapEx(ResId) — must use
# images_root = "main/default_images" to match images.zip's strip_prefix.
rsc_res(
    name        = "framework_res",
    srcs        = glob(["source/**/*.src"]),
    hdrs        = glob(["inc/**/*.hrc"]) + ["//main/svl:svl_hrc"],
    includes    = ["main/framework/inc", "main/svl/inc"],
    images      = [
        "//main/default_images:framework_res_images",
        "//main/default_images:shared_images",
    ],
    images_root = "main/default_images",   # must match images.zip strip_prefix
    visibility  = ["//visibility:public"],
)

# Module whose .res only contains strings/dialogs (no Bitmap{File=} resources):
# omit images and images_root entirely.
rsc_res(
    name     = "vcl_res",
    srcs     = glob(["source/src/*.src"]),
    hdrs     = glob(["inc/**/*.hrc"]) + ["//main/svl:svl_hrc"],
    includes = ["main/vcl/inc", "main/svl/inc"],
    visibility = ["//visibility:public"],
)
```

The default `defines` list (`WNT GUI WIN32 INTEL _X86_=1 CPPU_ENV=msci`) covers
all currently-migrated modules; override with an explicit `defines = [...]` if
a module needs additional preprocessor symbols.

### Modules wired up

| Module | Target |
|---|---|
| `accessibility` | `//main/accessibility:accessibility_res` |
| `avmedia` | `//main/avmedia:avmedia_res` |
| `basic` | `//main/basic:basic_res` |
| `basctl` | `//main/basctl:basctl_res` |
| `chart2` | `//main/chart2:chart2_res` |
| `connectivity` | `//main/connectivity:connectivity_res` |
| `cui` | `//main/cui:cui_res` |
| `dbaccess` | `//main/dbaccess:dbaccess_res` |
| `desktop` | `//main/desktop:desktop_res` |
| `editeng` | `//main/editeng:editeng_res` |
| `filter` | `//main/filter:filter_res` |
| `forms` | `//main/forms:forms_res` |
| `formula` | `//main/formula:formula_res` |
| `fpicker` | `//main/fpicker:fpicker_res` |
| `framework` | `//main/framework:framework_res` |
| `reportdesign` | `//main/reportdesign:reportdesign_res` |
| `sc` | `//main/sc:sc_res` |
| `scaddins` | `//main/scaddins:scaddins_res` |
| `sccomp` | `//main/sccomp:sccomp_res` |
| `sd` | `//main/sd:sd_res` |
| `sdext` | `//main/sdext:sdext_res` |
| `sfx2` | `//main/sfx2:sfx2_res` |
| `scripting` | `//main/scripting:scripting_res` |
| `starmath` | `//main/starmath:starmath_res` |
| `svl` | `//main/svl:svl_res` |
| `svtools` | `//main/svtools:svtools_res` |
| `svx` | `//main/svx:svx_res` |
| `sw` | `//main/sw:sw_res` |
| `uui` | `//main/uui:uui_res` |
| `vcl` | `//main/vcl:vcl_res` |
| `xmlsecurity` | `//main/xmlsecurity:xmlsecurity_res` |
