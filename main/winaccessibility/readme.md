# winaccessibility — Bazel Migration

Produces two DLLs, both gated on `--//build:atl_available=True` (VS Professional/Enterprise required):

| Target | Output | Description |
|--------|--------|-------------|
| `//main/winaccessibility:uacccom` | `UAccCOM.dll` | IAccessible2 COM wrapper (ATL-based) |
| `//main/winaccessibility:winaccessibility` | `winaccessibility.dll` | MSAA UNO service (`MSAAService`) |

## IDL pipeline

`source/UAccCOMIDL/` contains 6 IDL files defining IAccessible2 COM interfaces.
The `uacccom_idl` genrule runs `midl.exe` (Windows SDK v7.0) in dependency order:

1. `AccessibleKeyBinding.idl`, `AccessibleKeyStroke.idl`, `Charset.idl`, `defines.idl` — no imports
2. `ia2_api_all.idl` — imports the four above
3. `UAccCOM.idl` — imports `ia2_api_all.idl` and `defines.idl`

Generated outputs (`UAccCOM.h`, `UAccCOM_i.c`, `ia2_api_all_i.c`, etc.) are exposed via the
`uacccom_idl_headers` cc_library with `includes = ["."]`, making `#include "UAccCOM.h"` work
for both DLL targets.

## ATL guard

Both targets use `target_compatible_with = _ATL_REQUIRED` which resolves to
`@platforms//:incompatible` unless `--//build:atl_available=True` is set.

## Not yet migrated

- `.rgs` registry script merge + `mt.exe` manifest embedding (COM registration activation context)
- `.rc` resource file compilation (`UAccCOM.rc` → version resource)
- `.component` XML descriptor generation (needed by the UNO component loader)
