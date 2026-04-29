# formula — Bazel migration

Two DLLs: `for.dll` (formula core + UNO component) and `forui.dll` (Function Wizard UI).

## Targets

| Target | Output | Notes |
|---|---|---|
| `//main/formula:for` | `for.dll` | Core formula compiler, token types, UNO `FormulaOpCodeMapper` service |
| `//main/formula:for_implib` | `for.lib` | Import library for `for.dll` |
| `//main/formula:forui` | `forui.dll` | Function Wizard dialog UI |
| `//main/formula:forui_implib` | `forui.lib` | Import library for `forui.dll` |
| `//main/formula:formula_headers` | (headers) | Public `inc/formula/` API headers |

## Key notes

- `for.dll` exports `component_getFactory` / `component_getImplementationEnvironment` via
  `SAL_DLLPUBLIC_EXPORT`; no DEF file needed.
- Both DLLs define `FORMULA_DLLIMPLEMENTATION` (controls `__declspec(dllexport)` in
  `formuladllapi.h`).
- `precompiled_formula.cxx` is listed as a source in `for`; `PRECOMPILED_HEADERS` is not
  defined so the `#ifdef` block in the PCH header is a no-op.
- `forui` sources include `formdlgs.hrc` and dialog-local headers from `source/ui/dlg/`;
  covered by `/Imain/formula/source/ui/dlg` in copts.
- `.src` resource files (`core_resource.src`, `formdlgs.src`, `parawin.src`) are runtime
  resources loaded by `ResMgr::CreateResMgr("for"/"forui")`; not compiled by Bazel.
- No svx link dependency despite `prj/build.lst` listing it — neither library makefile
  actually links against svx.
