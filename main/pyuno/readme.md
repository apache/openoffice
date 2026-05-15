# pyuno — Bazel Module

Builds the Python-UNO bridge from `main/pyuno/`.

## Targets

| Target | Output |
|--------|--------|
| `//main/pyuno:pyuno` | `pyuno.dll` — core bridge (also `pyuno_implib` for linking) |
| `//main/pyuno:pythonloader` | `pythonloader.dll` — UNO component loader |
| `//main/pyuno:pyuno_headers` | `inc/pyuno/pyuno.hxx` — public C++ bridge headers |
| `//main/pyuno:pyuno_implib` | `pyuno.if.lib` — import library for downstream consumers |

## Sources

### pyuno.dll
Built from `source/module/`:
- `pyuno_runtime.cxx`, `pyuno.cxx`, `pyuno_callable.cxx`, `pyuno_module.cxx`
- `pyuno_type.cxx`, `pyuno_util.cxx`, `pyuno_except.cxx`, `pyuno_adapter.cxx`, `pyuno_gc.cxx`

`pyuno_dlopenwrapper.c` is *nix-only (uses `dlopen`/`dladdr`) and is **not** compiled on Windows.

### pythonloader.dll
Built from `source/loader/pyuno_loader.cxx`.  
UNO component; exports `component_getImplementationEnvironment` and
`component_getFactory` via `SAL_DLLPUBLIC_EXPORT` — no DEF file needed.

## Key build decisions

### pyconfig.h overlay — no auto-link pragma
Python 2.7's `PC/pyconfig.h` contains:
```c
#pragma comment(lib,"python27.lib")
```
This fires for any MSVC extension that includes `Python.h` (when
`Py_BUILD_CORE` is not defined).  Bazel names the import library
`python27.if.lib`, not `python27.lib`, so MSVC's linker fails with
LNK1104.

**Fix**: an overlay file at
`ext_libraries/modules/python/2.7.18/overlay/PC/pyconfig.h` removes
the pragma block.  The overlay hash is registered in `source.json`.
`python27.if.lib` is supplied explicitly via `additional_linker_inputs`
and `$(execpath @python//:python27_implib)` in linkopts.

### /Zc:wchar\_t-
Required throughout — `sal_Unicode` is `unsigned short`; native
`wchar_t` would break UNO type-system specialisations.

### stlport
`pyuno_impl.hxx` uses `::std::hash_map` / `::std::hash_set`; stlport
supplies these in `namespace std` on MSVC 2008.

### Debug build: `_invalid_parameter_noinfo` unresolved
In debug mode (`/MDd`) MSVC's `std::vector::operator[]` calls
`_invalid_parameter_noinfo` for bounds checking.  VS2008's `msvcrtd.dll`
does not export this symbol (it was added in VS2015), so the debug link
fails with LNK2019.

**Fix**: `/D_HAS_ITERATOR_DEBUGGING=0 /D_SECURE_SCL=0` are injected in
the `dbg_build` select branch to disable that checking path.  Both flags
are already zero by default in release, so only debug is affected.

### Windows-only — pyuno_loader (pyuno.so) skipped
`Library_pyuno_loader.mk` builds a thin `dlopen` wrapper that Python
uses on *nix to load `pyuno.so` lazily.  Windows does not have
`dlfcn.h`, and `python27.dll` is loaded implicitly via the import
library, so this target is not needed.
