# postprocess — Bazel migration

## What this module produces

| Target | Output | Notes |
|---|---|---|
| `//main/postprocess:services_rdb` | `services.rdb` + `services.input` | Merges ~130 `.component` files via `packcomponents.xslt` |
| `//main/postprocess:ooo_services_rdb` | `ooo_services_rdb.rdb` + `.input` | Merges mailmerge component only |
| `//main/postprocess:uiconfig_zip` | `uiconfig.zip` | ZIP of all UI config XML files from all modules |
| `//main/postprocess:all` | All of the above | Convenience filegroup |

## How to build

```
bazel build //main/postprocess:services_rdb
bazel build //main/postprocess:ooo_services_rdb
bazel build //main/postprocess:uiconfig_zip
bazel build //main/postprocess:all
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

### Deferred: xcd files (packregistry)

The `.xcd` registry files (`base.xcd`, `calc.xcd`, `main.xcd`, …) require
spool-generated intermediates:
- `fcfg_*.xcu` from the filter module's `FCFGMerge` tool
- `Common-brand.xcu` and similar from `officecfg`'s `install:module=` XSLT extraction

These spool pipelines are not yet implemented in Bazel. `xcd` generation is deferred.

## Files changed

- `main/postprocess/postprocess.bzl` — new; `services_rdb` + `uiconfig_zip` Starlark rules
- `main/postprocess/BUILD.bazel` — new; all three targets
- `main/solenv/bin/BUILD.bazel` — new; `exports_files` for `packcomponents.xslt` + `packregistry.xslt`
- `main/extensions/BUILD.bazel` — new; `exports_files` for `.component` files + `uiconfig_files` filegroup
- `main/xmlhelp/BUILD.bazel` — new; `exports_files` for `.component` files
- `main/basctl/BUILD.bazel` etc. (11 modules) — `uiconfig_files` filegroup appended
