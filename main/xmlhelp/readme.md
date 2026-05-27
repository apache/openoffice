# xmlhelp — Bazel Migration Notes

Migrated to Bazel as two `cc_binary` (linkshared) targets.

## Targets

| Target | Output | Description |
|---|---|---|
| `//main/xmlhelp:tvhlp1` | `tvhlp1.dll` | TreeView help hierarchy data source |
| `//main/xmlhelp:ucpchelp1` | `ucpchelp1.dll` | UCB content provider for compiled help |

## Source layout

```
source/treeview/        tvhlp1 sources (tvfactory.cxx, tvread.cxx)
source/cxxhelp/provider/ ucpchelp1 provider sources (13 files)
source/cxxhelp/qe/      DocGenerator.cxx (query engine, used by ucpchelp1)
source/cxxhelp/util/    Decompressor.cxx (used by ucpchelp1)
source/cxxhelp/inc/     Internal headers: qe/, util/, excep/ (angle-bracket includes)
inc/pch/                precompiled_xmlhelp.hxx (empty PCH placeholder)
util/                   DEF files + component files
```

## Key decisions

- **stlport**: `databases.hxx` uses `std::hash_map`/`std::hash_set` — requires stlport.
- **libxslt**: `urlparameter.cxx` uses `<libxslt/xslt.h>` and related XSLT API.
- **libxml2**: `ucpchelp1` uses libxml2 (via libxslt transitive dependency and direct use).
- **l10ntools helplinker_headers**: `databases.cxx` includes `<l10ntools/compilehelp.hxx>`.
- **HAVE_EXPAT_H**: define required by `ucpchelp1` as in the original makefile.
- **vos_headers**: both DLLs include `<vos/diagnose.hxx>`.
- **DEF files**: created at `util/tvhlp1.def` and `util/ucpchelp1.def`; both export only
  `component_getImplementationEnvironment` and `component_getFactory` (no `component_canUnload`).

## Include paths (copts)

**tvhlp1**: `/Imain/xmlhelp/inc`, `/Imain/xmlhelp/inc/pch`, `/Imain/xmlhelp/source/treeview`

**ucpchelp1**: `/Imain/xmlhelp/inc`, `/Imain/xmlhelp/inc/pch`,
`/Imain/xmlhelp/source/cxxhelp/inc`, `/Imain/xmlhelp/source/cxxhelp/provider`
