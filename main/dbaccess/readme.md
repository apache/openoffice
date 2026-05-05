# dbaccess — Bazel Migration

Builds 6 shared libraries that implement OpenOffice Base (database front-end and back-end).

## Targets

| Target | DLL | Description |
|--------|-----|-------------|
| `//main/dbaccess:dba` | `dba.dll` | Core database backend (RowSet, DataSource, DatabaseContext) |
| `//main/dbaccess:dbu` | `dbu.dll` | Database UI (dialogs, query/table/relation designers, wizards) |
| `//main/dbaccess:dbaxml` | `dbaxml.dll` | ODB XML filter (import/export of .odb documents) |
| `//main/dbaccess:sdbt` | `sdbt.dll` | SDB tools (connection utilities, object-name helpers) |
| `//main/dbaccess:dbmm` | `dbmm.dll` | Macro migration wizard |
| `//main/dbaccess:adabasui` | `adabasui.dll` | Legacy Adabas database UI extension |

## Key notes

- `source/shared/registrationhelper.cxx` is `#include`'d textually by `dbu_reghelper.cxx`
  (for dbu) and `flt_reghelper.cxx` (for dbaxml); exposed via `_registrationhelper`
  cc_library with `textual_hdrs`.
- `adodatalinks.cxx` requires OLE DB/ADO SDK headers (`msdasc.h`, `adoid.h`); included
  unconditionally for Windows (these headers are present in the Windows 7 SDK).
- All six DLLs export the standard UNO component API:
  `component_getImplementationEnvironment` + `component_getFactory`.
- `stlport` dep required by dba, dbu, dbaxml, sdbt, dbmm, adabasui (all use
  `boost::unordered_map` / `hash_map` via stlport shims).
