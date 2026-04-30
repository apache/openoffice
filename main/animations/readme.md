# animations — Bazel Migration

## Output
- `animcore.dll` — UNO animation core component

## Targets
- `//main/animations:animations_headers` — public headers under `inc/animations/`
- `//main/animations:animcore` — animcore.dll
- `//main/animations:animcore_implib` — import library

## Dependencies
- cppuhelper, cppu, sal (UNO runtime)
- comphelper (broadcasthelper, sequence utilities in targetpropertiescreator)
- stlport (hash_map in targetpropertiescreator)
- udkapi + offapi IDL headers

## Notes
- No util/ map file in original source; DEF file created with standard UNO component exports
- `/Zc:wchar_t-` required for sal_Unicode / UNO type compatibility
