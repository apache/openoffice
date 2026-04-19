# officecfg — Bazel Migration

## What this module builds

Configuration registry for Apache OpenOffice: 101 component-schema (`.xcs`)
files and 73 component-data (`.xcu`) files, processed into deployable form.

## Pipeline

### Schema trimming
Each `.xcs` file is processed by `util/schema_trim.xsl` via `xsltproc`:
- Removes `<author>` elements
- Removes `<desc>`/`<label>` for deprecated items
- Removes `constraints` for deprecated items
- Strips `@xsi:schemaLocation` attributes
- Removes `<value xsi:nil="true">` elements

Output: `trimmed/registry/schema/**/*.xcs` (one per source file).

### Locale extraction (alllang.xsl)
Files listed in `_LOCALIZED` in `BUILD.bazel` carry embedded multilingual
strings in `<value xml:lang="...">` attributes.  `util/alllang.xsl` extracts
the values for a single target locale:

```
xsltproc --novalid --stringparam locale en-US -o out.xcu alllang.xsl in.xcu
```

LOCALIZEDFILES are derived from each `registry/data/**/makefile.mk` that lists
`LOCALIZEDFILES=`.  Four subdirectories contribute:
- `registry/data/org/openoffice/` — `Setup.xcu`
- `registry/data/org/openoffice/Office/` — 12 files
- `registry/data/org/openoffice/Office/UI/` — 32 files
- `registry/data/org/openoffice/TypeDetection/` — none

### Language pack zip
Each `registry_<locale>.zip` assembles a full locale data pack using
`@bazel_tools//tools/zip:zipper` (Bazel's built-in cross-platform zip tool):
1. All non-localized `.xcu` files from `registry/data/` (base layer)
2. Locale-extracted spool files from `spool/<locale>/` (alllang.xsl output)
3. Entries stored as `org/openoffice/...` at the zip root

### Known warnings
`install:module` namespace prefix is used in several XCU source files without
a corresponding `xmlns:install` declaration.  libxml2 reports parse warnings
but continues; output is still produced.  The platform filtering (unxwnt/macosx)
is silently disabled — harmless for a Windows/MSVC build since those entries are
filtered at runtime by the OOo registry loader anyway.

### xsltproc
Built from the libxslt source as `@libxslt//:xsltproc` (a `cc_binary` added
to `ext_libraries/modules/libxslt/1.1.35/BUILD.bazel`).  Also adds
`libxslt/attributes.c` to `LIBXSLT_SRCS` (was missing; needed for full XSLT
`<xsl:attribute-set>` support).

## Key targets

| Target | Description |
|--------|-------------|
| `//main/officecfg:officecfg` | Aggregate: trimmed schema + data + en-US zip |
| `//main/officecfg:registry_schema_trimmed` | All trimmed .xcs files |
| `//main/officecfg:registry_zip_en_US` | `registry_en-US.zip` |
| `@libxslt//:xsltproc` | xsltproc binary (new) |

## Dependency

`soltools` from `build.lst` provides only build infrastructure utilities
(`mkdepend`, `giparser`, etc.) — none are called directly by officecfg's
data pipeline, so there is no Bazel dep on `//main/soltools`.

## Language selection (replaces alllangiso)

In the nmake era you set `alllangiso=en-US de fr` before building.
In Bazel each locale is a `bool_flag` in `//build`:

```
# user.bazelrc
build --//build:lang_de=True
build --//build:lang_fr=True
build --//build:lang_zh_CN=True   # zh-CN → lang_zh_CN
```

`en-US` is on by default.  Every `registry_zip_<locale>` target is always
defined; the `officecfg` aggregate filegroup includes a zip only when its
flag is True.  The full language list lives in `build/langs.bzl`.

## Build

```
# English only (default)
bazel build //main/officecfg:officecfg

# English + German + French
bazel build --//build:lang_de=True --//build:lang_fr=True //main/officecfg:officecfg

# One specific zip
bazel build //main/officecfg:registry_zip_de
```
