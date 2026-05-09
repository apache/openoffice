# postprocess — Bazel migration

## What this module produces

| Target | Output | Notes |
|---|---|---|
| `//main/postprocess:services_rdb` | `services.rdb` + `services.input` | Merges ~130 `.component` files via `packcomponents.xslt` |
| `//main/postprocess:ooo_services_rdb` | `ooo_services_rdb.rdb` + `.input` | Merges mailmerge component only |
| `//main/postprocess:uiconfig_zip` | `uiconfig.zip` | ZIP of all UI config XML files from all modules |
| `//main/postprocess:all` | All of the above | Convenience filegroup |
| `//main/postprocess:all_xcd` | 20 `.xcd` registry packages | See XCD section below |
| Individual XCDs | `base.xcd`, `calc.xcd`, `main.xcd`, … | See XCD section below |

## How to build

```
bazel build //main/postprocess:services_rdb
bazel build //main/postprocess:ooo_services_rdb
bazel build //main/postprocess:uiconfig_zip
bazel build //main/postprocess:all
bazel build //main/postprocess:all_xcd
```

## Architecture

### services.rdb

The legacy build ran:
```
xsltproc packcomponents.xslt services.input > services.rdb
```
where `services.input` is an XML `<list>` of `<filename>` elements pointing at each
`.component` file.

The Bazel implementation uses a custom `services_rdb` Starlark rule in
`postprocess.bzl`. At analysis time it generates a `.bat` script that:
1. Writes `<list>…</list>` XML with `file:///` URIs built from `%cd%` (exec root)
   using delayed expansion (`!F:\=/!`) so backslashes become forward slashes.
2. Calls xsltproc to merge them into the output `.rdb`.

`file:///` URIs are required because xsltproc's `document()` resolves relative
paths against the input XML's base URI (the bazel-out path), not the working directory.

### uiconfig.zip

A `uiconfig_zip` Starlark rule generates a PowerShell script at analysis time.
The script uses `System.IO.Compression.ZipFile` to create the archive, stripping
the path up to and including `\uiconfig\` so ZIP entries start at the module/menu
level (e.g. `calc/menubar/menubar.xml`).

Each module with uiconfig files exposes a `filegroup(name="uiconfig_files", ...)`.

### XCD files (packregistry)

Each `.xcd` file is a compiled registry package produced by `packregistry.xslt` from a
list of `.xcs` (schema) and `.xcu` (data) files. XCS files must precede XCU files in the
list (configmgr requirement).

The `pack_registry` Starlark rule in `postprocess.bzl`:
1. Calls `make_services_input.pl` to write an XML `<list>` with `file:///` absolute URIs
2. Calls `xsltproc packregistry.xslt` to produce the `.xcd`

The 20 XCD packages and their contents:

| XCD | XCS schema files | XCU data sources |
|-----|-----------------|-----------------|
| `base_xcd` | — | fcfg_database + officecfg module extractions |
| `calc_xcd` | CalcCommands, CalcWindowState | fcfg_calc + connectivity calc.xcu + module extractions |
| `cjk_xcd` | — | Office/Common + Office/Writer cjk modules |
| `ctl_xcd` | — | Office/Common ctl module |
| `draw_xcd` | DrawWindowState | fcfg_draw + module extractions |
| `forcedefault_xcd` | — | Office/Linguistic ForceDefaultLanguage module |
| `graphicfilter_xcd` | — | fcfg_drawgraphics + fcfg_impressgraphics |
| `impress_xcd` | Effects, ImpressWindowState, PresenterScreen | fcfg_impress + sdext presenter XCU |
| `korea_xcd` | — | Office/Common korea module |
| `lingucomponent_xcd` | — | hyphenator/spellchecker/thesaurus XCU |
| `main_xcd` | 84 XCS files | 86 XCU files (raw + FCFGMerge + module extractions + DataAccess) |
| `math_xcd` | MathCommands, MathWindowState | fcfg_math + module extractions |
| `oooimprovement_xcd` | — | extensions/oooimprovement Jobs.xcu + Logging.xcu |
| `palm_xcd` | — | fcfg_palm |
| `pocketexcel_xcd` | — | fcfg_pocketexcel |
| `pocketword_xcd` | — | fcfg_pocketword |
| `pyuno_xcd` | — | Office/Scripting python module |
| `w4w_xcd` | — | fcfg_w4w (109 Word-for-Windows filter fragments) |
| `writer_xcd` | 7 WriterWindowState XCS files | fcfg_global + fcfg_web + fcfg_writer + module extractions |
| `xsltfilter_xcd` | — | fcfg_xslt |

**FCFGMerge pipeline** (`fcfg_merge` rule + `fcfg_merge.pl`):

The old Java/Python `FCFGMerge` tool is replaced by `fcfg_merge.pl`. Each target in
`main/filter/source/config/fragments/BUILD.bazel` merges raw `.xcu` fragments from
`types/`, `filters/`, `frameloaders/`, `contenthandlers/`, and `internalgraphicfilters/`
subdirectories into a single combined XCU with the proper `<oor:component-data>` wrapper.

**Module extraction pipeline** (`alllang_module_*` genrules in `main/officecfg/BUILD.bazel`):

The old `alllang.xsl` `--stringparam module <suffix>` pass extracts nodes with
`install:module=<suffix>` attributes into per-module XCU spool files. Outputs go to
`spool_module/<module>/org/openoffice/<xcu>` to avoid collision with per-locale spool files.

## Files changed

- `main/postprocess/postprocess.bzl` — `services_rdb`, `uiconfig_zip`, `fcfg_merge`, `pack_registry` rules
- `main/postprocess/BUILD.bazel` — all targets including 20 XCD pack_registry calls
- `main/solenv/bin/BUILD.bazel` — `exports_files` for `packcomponents.xslt` + `packregistry.xslt`
- `main/extensions/BUILD.bazel` — `exports_files` for `.component` files + `uiconfig_files` filegroup
- `main/xmlhelp/BUILD.bazel` — `exports_files` for `.component` files
- `main/basctl/BUILD.bazel` etc. (11 modules) — `uiconfig_files` filegroup appended
- `main/filter/source/config/fragments/BUILD.bazel` — new; all `fcfg_merge` targets
- `main/officecfg/BUILD.bazel` — `exports_files` for individual XCS/XCU + `alllang_module_*` genrules
- `main/connectivity/BUILD.bazel` — `exports_files` for DataAccess XCU files
- `main/extensions/source/oooimprovement/BUILD.bazel` — new; `exports_files` for Jobs/Logging XCU
- `main/sdext/BUILD.bazel` — `exports_files` for presenter Jobs/ProtocolHandler XCU
- `main/lingucomponent/BUILD.bazel` — `exports_files` for hyphenator/spell/thesaurus XCU
- `build/tools/fcfg_merge.pl` — new; Perl FCFGMerge replacement
- `build/tools/BUILD.bazel` — added `fcfg_merge.pl` to `exports_files`
