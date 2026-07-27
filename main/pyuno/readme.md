# pyuno — Bazel Module

Builds the Python-UNO bridge from `main/pyuno/`.

## Targets

| Target | Output |
|--------|--------|
| `//main/pyuno:pyuno` | `pyuno.dll` — core bridge (also `pyuno_implib` for linking) |
| `//main/pyuno:pyuno_pyd` | `pyuno.pyd` — the same file under the name Python imports; **this** is what gets staged |
| `//main/pyuno:pyuno_py` | `pythonloader.py` / `uno.py` / `unohelper.py` → `program/` |
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

## Runtime requirements (staging) — three separate landmines

Building the DLLs is not enough; `com.sun.star.loader.Python` is instantiated by
anything that touches a Python script provider (Tools ▸ Macros, script-provider
enumeration), and each missing piece fails differently:

1. **`program/pythonloader.uno.ini`** — read by `pyuno_loader.cxx` `CreateInstance()`
   for `PYUNO_LOADER_PYTHONHOME` / `PYUNO_LOADER_PYTHONPATH` (scp2
   `profileitem_python.scp`).  Missing ⇒ `Py_Initialize()` finds no stdlib ⇒
   `Py_FatalError` ⇒ **`exit()` kills the whole soffice process**, no exception,
   no crash dump — just `NtTerminateProcess` right after `pyuno` loads.
   Source: `main/staging/pythonloader.uno.ini`.
2. **`program/python-core-2.7.18/lib`** — the stdlib itself, same failure mode.
   Staged by `//main/staging:install_python_lib`, part of `:install`.
3. **`pyuno.pyd` naming** — see below.

The `.py` files (`pythonloader.py`, `uno.py`, `unohelper.py` here;
`pythonscript.py`, `officehelper.py`, `mailmerge.py` from
`//main/scripting:pyprov_py`) must sit flat in `program/`, the last entry of
`PYUNO_LOADER_PYTHONPATH` (`$ORIGIN`).

## Key build decisions

### `.pyd`, not `.dll` — and exactly one copy
Python 2.7's Windows extension loader accepts only one suffix:

```c
/* Python/dynload_win.c */
const struct filedescr _PyImport_DynLoadFiletab[] = {
#ifdef _DEBUG
    {"_d.pyd", "rb", C_EXTENSION},
#else
    {".pyd", "rb", C_EXTENSION},
#endif
    {0, 0}
};
```

`uno.py` opens with `import pyuno`, so a `pyuno.dll` yields
`ImportError: No module named pyuno`, `import pythonloader` fails with it, and
`ScriptProviderForPython` can never be instantiated.  The symptom is quiet: the
macro organizer still shows the **Python** language node (that comes from the
staged `pyuno.xcd`), but the tree is empty and *Create* stays greyed out —
`SvxScriptOrgDialog::CheckButtons` enables it from the browse node's
`Creatable` property, and there is no node to ask.

Upstream builds one file called `pyuno.pyd` (`source/module/makefile.mk`
`DLLPOST=.pyd`), which is also what `pythonloader` links against.  Bazel cannot
do that directly — a `linkshared` `cc_binary` is always `<target>.dll` — so:

* `util/pyuno.def` carries a single `LIBRARY pyuno.pyd` statement, which sets
  the DLL's internal name and hence the name recorded in `pyuno.if.lib` and in
  `pythonloader.dll`'s import table.  `/OUT` still wins for the file on disk.
* `copy_file` (`//build/rules:copy_file.bzl`) re-exposes that file as
  `pyuno.pyd`, which is what staging depends on.

`pyuno.dll` still lands in `program/` too, as a by-product: the staging aspect
walks `additional_linker_inputs`, so `pythonloader`'s `:pyuno_implib`
(`filegroup(srcs = [":pyuno"], output_group = "interface_library")`) drags the
`cc_binary`'s own output along.  Inert — with the `LIBRARY` override nothing
names `pyuno.dll` any more, so it is never loaded; `pyuno.pyd` is the module.

VC9 warns `LNK4070: /OUT:pyuno.pyd directive in .EXP differs from output
filename 'pyuno.dll'; ignoring directive` — expected, and it ignores the
directive only for the output filename.  Verified against link.exe
9.00.30729.01: export directory, import-library members, and a consumer's
import table all read `pyuno.pyd`.  Check after a rebuild with
`link /dump /imports bazel-bin/main/pyuno/pythonloader.dll`.

Staging both names would be wrong, not merely redundant: `pyuno.cxx` has a
file-level `static PyTypeObject PyUNOType` and `pyuno_runtime.cxx` a
`static PyTypeObject RuntimeImpl_Type`.  Two HMODULEs ⇒ two distinct type
objects ⇒ each half rejects the other's objects (`getRuntimeImpl` looks up
`pyuno_runtime` in the builtins dict and type-checks it).  Upstream gets one
instance for free because `pythonloader`'s import and Python's `import pyuno`
resolve to the same path.

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
