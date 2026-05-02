# fileaccess — Bazel Migration

Builds `fileacc.dll` — the `com.sun.star.ucb.SimpleFileAccess` UNO service component.

## Targets

| Target | Output |
|--------|--------|
| `//main/fileaccess:fileacc` | `fileacc.dll` |

## Notes

- Single source file (`source/FileAccess.cxx`) with all class implementations inline.
- `FILEACCESS_DLLIMPLEMENTATION` define activates `SAL_DLLPUBLIC_EXPORT` in `inc/fileaccess/dllapi.h`.
- DEF exports: `component_getImplementationEnvironment`, `component_getFactory` (no `component_canUnload`).
- Include path `/Imain/fileaccess/inc` needed for the `"fileaccess/dllapi.h"` include.
