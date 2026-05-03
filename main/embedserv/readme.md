# embedserv — Bazel migration notes

## Outputs
- `emser.dll` — UNO component OLE embedding server; registers 10 CLSIDs for
  Writer/Calc/Draw/Impress/Math (6.0 and OASIS formats) so other Windows
  applications can embed OpenOffice documents via OLE in-place activation.
- `inprocserv.dll` — companion in-process COM handler registered under
  `HKLM\...\InprocHandler32` for lightweight OLE embedding without launching
  the full OO process.

## ATL dependency
Both DLLs require Microsoft ATL headers (`VC\ATLMFC\INCLUDE`: `atlbase.h`,
`atlcom.h`, `atlctl.h`, `atldbcli.h`). These are only present in Visual Studio
**Professional or Enterprise** editions — not Express or BuildTools.

The targets use `target_compatible_with = _ATL_REQUIRED` which mirrors the old
build system's `DISABLE_ATL` guard: on machines without ATLMFC the targets are
silently skipped by `bazel build //...`.

To build on a qualifying machine, add to `user.bazelrc`:
```
build --//build:atl_available=True
```

## Future: proper ATL external library
When `winaccessibility` and `extensions/activex` are migrated they will need the
same ATL headers. At that point the right solution is an
`ext_libraries/modules/atl/` entry wrapping the **ReactOS ATL implementation**
(LGPL, compatible with Apache License 2.0 for header-only compile-time use).
ReactOS provides complete, working implementations of `CComModule`,
`CComPtr<T>`, the object-map macros, and the full COM class-factory registration
machinery — not stubs. This work should be done before those layers are
attempted.
