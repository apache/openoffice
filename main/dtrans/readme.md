# dtrans — Bazel migration

Provides clipboard and drag-and-drop data transfer UNO services for Windows.

## Outputs

| Target | DLL | Service |
|---|---|---|
| `//main/dtrans:dtrans` | dtrans.dll | Generic clipboard manager + fallback clipboard |
| `//main/dtrans:mcnttype` | mcnttype.dll | MIME content type factory |
| `//main/dtrans:dnd` | dnd.dll | OLE drag source + drop target |
| `//main/dtrans:ftransl` | ftransl.dll | Win32 ↔ UNO format translator |
| `//main/dtrans:sysdtrans` | sysdtrans.dll | Windows system clipboard (ClipboardW32) |

## Key notes

- `dnd` and `sysdtrans` share the `source/win32/dtobj/` OLE data-object files; each target lists them explicitly (no shared cc_library to keep symbol visibility isolated per DLL).
- `sysdtrans` additionally compiles `XNotifyingDataObject.cxx` (not in `dnd`).
- `sysdtrans` defines `UNICODE`/`_UNICODE` — clipboard and MTA OLE code use wide-char Win32 APIs.
- `/GR` (RTTI) is required for `dnd` and `sysdtrans` — the dtobj COM wrappers use `dynamic_cast` internally.
- All relative `"..\..\"` includes in the win32 source tree resolve correctly under MSVC because the compiler searches relative to the source file's location.
- DEF exports: `dtrans`, `mcnttype`, `ftransl`, `sysdtrans` export `component_getImplementationEnvironment` + `component_getFactory`; `dnd` additionally exports `component_canUnload`.
