# svgio — Bazel Migration

## Output
- `svgio.dll` — UNO component providing SVG parsing (`com.sun.star.graphic.SvgTools`)

## Targets
| Label | Description |
|---|---|
| `//main/svgio:svgio` | svgio.dll (cc_binary, linkshared) |
| `//main/svgio:svgio_implib` | import library for downstream linkers |
| `//main/svgio:svgio_headers` | public headers under `inc/svgio/` |

## Build
```
bazel build //main/svgio:svgio
```

## Notes
- 31 SVG element node implementations in `source/svgreader/`
- 2 UNO bridge files in `source/svguno/` (`svguno.cxx`, `xsvgparser.cxx`)
- `svgtoken.hxx` uses `std::hash_map` → requires `//main/stlport:stlport`
- DEF file exports: `component_getImplementationEnvironment`, `component_getFactory`
