# idl — Bazel Migration

## Output
- `svidl.exe` — StarOffice Visual IDL Compiler (build-time tool)

## Target
```
//main/idl:svidl
```

## Dependencies
- `//main/tools:tools_headers` + `tl_implib` (tl.dll)
- `//main/sal:sal_headers` + `sal_implib`
- `//main/stlport:stlport` — required because `tools/pstm.hxx` uses `std::hash_map`

## Notes
- Compiled with `-DIDL_COMPILER` preprocessor define
- No DLL produced; no exports (d.lst is empty)
- `inc/pch/precompiled_idl.hxx` is an empty stub — compiled as a regular TU via glob
