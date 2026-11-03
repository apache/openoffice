# Python — Bazel module

The build uses **3.11.15**, trunk's version (`pyversion.mk`).  2.7.18 is still in
the registry, but nothing depends on it any more; trunk's pyuno is Python-3-only
since its "Python 3 migration".

`makefile.mk`, `prj/`, `pyversion*.mk` and the `*.patch` files in this directory
are the relocated dmake recipe.  Bazel reads only the `<version>/` directories.

## Registry layout

```
ext_libraries/modules/python/
  metadata.json
  3.11.15/
    MODULE.bazel
    source.json            ← URL + overlay hashes
    overlay/
      MODULE.bazel
      BUILD.bazel          ← every target; source lists are PCbuild's
      build_defs.bzl       ← freeze / deepfreeze actions, .dll → .pyd rename
      PC/pathcch.h         ← Windows 7: shadows the SDK header
      PC/pathcch_win7.c    ← Windows 7: PathCch* with fallbacks
      PC/aoo_getpath.c     ← string defines PCbuild passes per file
      PC/aoo_sysmodule.c       (one wrapper per file, each #includes
      PC/aoo_dynload_win.c      the original; no CPython file is
      PC/aoo_dl_nt.c            modified)
      PC/aoo_win7_compat.h ← Windows 7: Windows 8 constants, force-included
      PC/aoo_dllhmodule_static.c ← PyWin_DLLhModule for _bootstrap_python
      PC/aoo_bootstrap_python.c  ← _bootstrap_python, honouring PYTHONHOME
  2.7.18/                  ← previous version, unused
```

## Targets

| Target | Output |
|--------|--------|
| `@python//:python311` | `python311.dll` |
| `@python//:python311_implib` | `python311.if.lib` |
| `@python//:python_headers` | `Include/**` + `PC/pyconfig.h` |
| `@python//:python` | `python.exe` (staged as `python-core-<ver>/bin/python.exe`) |
| `@python//:python_pyds` | the 14 extension modules, as `.pyd` |
| `@python//:python_lib` | `Lib/**`, without tests, IDLE and tkinter |

## What it builds

What trunk builds with PCbuild + MSBuild (`makefile.mk`, COMEX 14 branch):
`pythoncore`, `python`, and the extension modules `_socket select unicodedata
pyexpat _elementtree _multiprocessing _overlapped _asyncio _queue _uuid
_zoneinfo winsound _decimal _msi`.  Each is a native `cc_binary`, with the
sources, defines and libraries of its `.vcxproj`.  Not built, as on trunk:
`_ctypes`, `_ssl`/`_hashlib`, `_bz2`, `_lzma`, `_sqlite3`, `_tkinter`.

Trunk's four patches (`python-md5`, `python-solver-before-std`,
`python-3.11.15-sysbase`, `python-3.11.15-nohardlink`) touch only the Unix build
— `setup.py`, `Modules/Setup`, `configure` — and none applies here.

Differences from PCbuild:

- **zlib**: `zlibmodule.c` links the registry's `@zlib` (built `Z_PREFIX`)
  rather than compiling zlib's sources into the DLL.
- **Default libraries**: each `.vcxproj` lists its libraries on top of
  MSBuild's default Windows set (`%(AdditionalDependencies)`: `user32`,
  `advapi32`, `shell32`, …), which a Bazel link does not get.  Every extension
  module links that set explicitly (`_MSBUILD_DEFAULT_LIBS`).
- **`NDEBUG`**: always defined (`_BASE_DEFINES`), as in PCbuild's Release
  configuration, which is the only one built.  The toolchain adds it only
  under `-c opt`; without it a fastbuild leaves the core's `assert()`s live,
  and pyuno — which runs Python on thread states of its own rather than
  through `PyGILState_Ensure` — trips `assert(PyGILState_Check())` in
  `PyErr_Occurred` on the first macro call.
- **`_decimal`**: `CONFIG_64` + `ANSI` instead of `CONFIG_64` + `MASM`: the
  portable-C division path, so the build needs no ml64 step for
  `libmpdec/vcdiv64.asm`.

## Windows 7

Python 3.9+ supports Windows 8.1 and later (`Doc/using/windows.rst`).  Our floor
is Windows 7 SP1, so the overlay bridges the gap without touching CPython's
files:

- **API level.** `PC/pyconfig.h` asks for `_WIN32_WINNT 0x0602`, but only
  `#ifndef`.  The toolchain's `0x0601` stands, and `WINVER`/`NTDDI_VERSION` are
  matched to it (`sdkddkver.h` rejects a mismatch).  So any Windows 8 API used
  without a runtime check fails to *compile*, which is the audit.
- **Windows 8 constants.** A scan of every name the 10.0.19041.0 SDK declares
  under a Windows 8+ guard, against the compiled sources, found one real use:
  `LCMAP_SORTHANDLE`/`LCMAP_HASH` in `_winapi.LCMapStringEx`, which only
  rejects those flags.  `PC/aoo_win7_compat.h`, force-included with `/FI`,
  defines them.  Constants only; a function belongs in the pathcch pattern
  below.
- **`MSC`.** The toolchain defines it globally.  The SDK uses it as a member
  name (`ws2bth.h`), so every Python target passes `/UMSC`.
- **`pathcch.lib`.** pythoncore links it.  Its API set,
  `api-ms-win-core-path-l1-1-0`, does not exist on Windows 7, so
  `python311.dll` would not even load there.  `PC/pathcch.h` sits in `PC/`,
  which is on the include path ahead of the SDK, so `#include <pathcch.h>`
  finds it instead.  `PC/pathcch_win7.c` defines the three functions Python
  calls: `PathCchSkipRoot`, `PathCchFindExtension` and `PathCchCombineEx`.
  Each one calls KernelBase.dll's real export when it exists (Windows 8 and
  later; KernelBase is always loaded, so this loads nothing and searches no
  path).  Only on Windows 7 does a fallback run, written from the documented
  contract.  `PathCchCombineEx` falls back to shlwapi's `PathCombineW`,
  limited to `MAX_PATH`, which is Windows 7's own limit without `\\?\`.
  `pathcch.lib` is not linked.
- **`LOAD_LIBRARY_SEARCH_*`.** `Python/dynload_win.c` loads every `.pyd` with
  these flags.  Windows 7 SP1 accepts them with update **KB2533623**
  installed, the same class of prerequisite as the UCRT update (KB2999226)
  that any VS2019-built binary already needs.  `AddDllDirectory` and
  `RemoveDllDirectory` are looked up at runtime by posixmodule.c, so they need
  nothing.

To check a build:
```
dumpbin /imports python311.dll | findstr /i "api-ms-win-core-path pathcch"
```
should print nothing.

## No host Python

CPython 3.11 compiles part of its stdlib into the interpreter:

1. `_freeze_module.exe` turns each module into a header,
   `Python/frozen_modules/<name>.h`.
2. `Tools/scripts/deepfreeze.py` turns those headers into
   `Python/deepfreeze/deepfreeze.c`.

PCbuild runs step 2 with a Python it finds on the machine.  Here it runs under
`_bootstrap_python.exe`, the core linked statically without `frozen.c` and
`deepfreeze.c` and with only importlib frozen, which is how the Unix build
does it.  Both tools are built for the exec platform and run through
`ctx.actions.run` (`build_defs.bzl`), with `PYTHONHOME` pointing at the
extracted source.

`_bootstrap_python` starts from an isolated config, which ignores every
`PYTHON*` variable; on Unix it needs none, because it runs inside the build
tree.  Here it sits in `bazel-out`, so `PC/aoo_bootstrap_python.c` wraps it:
it takes `home` from `PYTHONHOME` and turns bytecode writing off, so nothing is
written into the source repository.  It reads no other variable.  It also
skips `site` (as `-S` would), which reads `sys.winver`, set only in the DLL
build.

The deepfreeze rule's input list is named `read_inputs`, not `data`.
`//main/staging`'s collection aspect walks `data` and `srcs`, and this list is
the whole stdlib as `.py` files, which staging would otherwise install flat
into `program/`.

## Linking against python311.dll

`PC/pyconfig.h` asks for `python311.lib` by `#pragma comment(lib)` in every
translation unit outside the core.  Bazel's import library is
`python311.if.lib`, so a consumer passes it explicitly and adds
`/NODEFAULTLIB:python311.lib`, or the link fails with LNK1104.  The 2.7.18
overlay rewrote `pyconfig.h` for this; 3.11.15 leaves the file alone.

Under `_DEBUG`, `pyconfig.h` also defines `Py_DEBUG`, which changes the object
layout.  pyuno undefines `_DEBUG` in dbg builds, because it always links the
release DLL.

## Version in one place

`//main/pyuno:python_version.bzl` holds `PYTHON_VERSION`.  It names
`program/python-core-<ver>`, which the launcher (`zipcore/pyversion.hxx`), the
staging layout and the generated `pythonloader.uno.ini` all read.  It must
match the `python` bazel_dep in `MODULE.bazel`.

## Changing an overlay file

1. Recompute its hash: `openssl dgst -sha256 -binary overlay/FILE | base64`.
2. Put `"sha256-<base64>"` into `source.json`.  A wrong hash is silently
   ignored and the old file is kept.
3. Run `bazel mod deps --lockfile_mode=refresh`.
