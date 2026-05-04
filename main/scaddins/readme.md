# scaddins — Bazel migration

Two UNO add-in DLLs providing spreadsheet functions for Calc.

## Outputs

| Target | Output |
|---|---|
| `//main/scaddins:analysis` | `analysis.dll` — Analysis functions (Bessel, financial, complex numbers, etc.) |
| `//main/scaddins:date` | `date.dll` — Date/misc functions (DiffWeeks, DiffMonths, ROT13, etc.) |

## Local IDL pipeline

Both DLLs define UNO interfaces not present in offapi:

- `analysisadd.idl` → `com.sun.star.sheet.addin.XAnalysis`
- `dateadd.idl` → `com.sun.star.sheet.addin.XDateFunctions`, `XMiscFunctions`

Each gets its own `idl_library` + `cc_library` wrapper (`analysisadd_idl_headers`,
`dateadd_idl_headers`). The `extra_rdbs` (udkapi + offapi) are passed to cppumaker so
the generated `*_idl_inc/` directory contains all headers the source needs, including
offapi types like `XAddIn`, `XCompatibilityNames`, etc.

## Key notes

- `/Imain/scaddins/source/analysis` and `.../datefunc` for quoted local includes
- `/Zc:wchar_t-` required for `sal_Unicode` compatibility
- Deps: `tl` (tools), `cppu3`, `cppuhelper3MSC`, `sal` — matching the original `SHL1STDLIBS`
- No stlport needed (no `hash_map`/`boost::unordered_map` usage)
- Resources (`.src` → `.res`) not yet compiled (rsc pipeline deferred)
