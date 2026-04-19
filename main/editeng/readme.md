# editeng — Bazel Migration

## Output
- `editeng.dll` — text editing engine (rich text, outliner, spelling, RTF, UNO bindings)

## Targets
| Target | Type | Description |
|---|---|---|
| `editeng_headers` | `cc_library` | Public headers from `inc/editeng/` and `inc/pch/` |
| `editeng` | `cc_binary` (shared) | `editeng.dll` |
| `editeng_implib` | `filegroup` | Import library for downstream consumers |

## Dependencies
- `svtools`, `xmloff`, `linguistic` (per build.lst)
- `vcl`, `svl`, `sot`, `tools`, `unotools`, `basegfx`, `toolkit`
- `comphelper`, `ucbhelper`, `cppuhelper`, `cppu`, `vos`, `sal`
- `i18nisolang1`, `i18npaper`, `icuuc`

## Notes
- 82 source files across 7 subdirectories: `accessibility`, `editeng`, `items`, `misc`, `outliner`, `rtf`, `uno`, `xml`
- `/Zc:wchar_t-` required (uses `sal_Unicode`)
- `snprintf=_snprintf` required (VS2008)
- No DEF file: all exports via `EDITENG_DLLIMPLEMENTATION` (`__declspec(dllexport)`)
- `editeng.dxp` lists `component_getImplementationEnvironment`/`component_getFactory` but no source implements them — file is vestigial
