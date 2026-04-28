# fpicker — Bazel Migration

## Targets

| Target | Output | Description |
|--------|--------|-------------|
| `//main/fpicker:fps` | `fps.dll` | Windows system file picker (IFileOpenDialog on Vista+, GetOpenFileName fallback) |
| `//main/fpicker:fop` | `fop.dll` | Windows system folder picker (SHBrowseForFolder) |
| `//main/fpicker:fps_office` | `fps_office.dll` | Office-native file/folder picker dialog |
| `//main/fpicker:fpicker` | `fpicker.dll` | Generic bridge: dispatches to system or office picker based on desktop environment |

## Notes

- `fps` and `fop` both compile `source/win32/misc/` (WinImplHelper, AutoBuffer, resourceprovider) as shared utility sources rather than a separate library, matching the original gmake build.
- `fps` targets Vista+ via `/D_WIN32_WINNT=0x0600` (overriding the module default 0x0500). On Vista+, `VistaFilePicker` (IFileOpenDialog COM API) is used; `CFilePicker` (GetOpenFileName) is the pre-Vista fallback.
- The `Fps.rc` Windows resource file (dialog templates for the pre-Vista picker) is not compiled; rc.exe support is not configured in the custom MSVC toolchain and the Vista picker path doesn't need these templates.
- `resourceprovider.cxx` requires `svtools/svtools.hrc` for string resource IDs; deps include `//main/svtools:svtools_headers`.
- All four DLLs export the standard UNO component entry points: `component_getImplementationEnvironment` and `component_getFactory`.
