# Python 2.7.18 — Bazel Module

Builds `python27.dll` from source using MSVC via the local file registry at
`ext_libraries/modules/python/`.

## Registry layout

```
ext_libraries/modules/python/
  metadata.json
  2.7.18/
    MODULE.bazel
    source.json          ← URL + overlay hashes
    overlay/
      MODULE.bazel
      BUILD.bazel        ← cc_binary + filegroup + cc_library
      PC/config.c        ← minimal built-in module table (replaces stock PC/config.c)
      PC/pyconfig.h      ← removes #pragma comment(lib,"python27.lib") auto-link
```

## Targets

| Target | Output |
|--------|--------|
| `@python//:python27` | `python27.dll` (linkshared) |
| `@python//:python27_implib` | `python27.if.lib` (interface_library filegroup) |
| `@python//:python_headers` | `Include/*.h` + `PC/pyconfig.h` |

## Key build decisions

### No DEF file
Python 2.7.18's tarball does not include `PC/python27.def`. Exports are
produced via `__declspec(dllexport)` through the `Py_BUILD_CORE` →
`PyAPI_FUNC` macro chain. No `win_def_file` is needed.

### PC/pyconfig.h — no ./configure needed
The Windows pre-generated `PC/pyconfig.h` is used directly. No autoconf step.

### Defines
- `Py_BUILD_CORE` — enables `__declspec(dllexport)` on all public API
- `NTDDI_VERSION=0x06010000` — pre-empts `pyconfig.h`'s XP-era `#ifndef`
  which conflicts with the toolchain's `/D_WIN32_WINNT=0x0601` (Vista)
- `MS_WINDOWS`, `WITH_THREAD`, `NT_THREADS` — omitted; `PC/pyconfig.h`
  already defines them (adding them again causes C4005 redefinition warnings)
- `snprintf=_snprintf` — omitted; `Python/pyport.h` handles this for MSVC

### include paths
- `Include/` — public Python headers (`Python.h` etc.)
- `PC/` — `pyconfig.h` (Windows pre-generated)
- `Python/` — internal headers (`importdl.h`, used by `PC/import_nt.c`)

### Excluded source files

**Python/ (Unix-only or conflicts):**
| File | Reason |
|------|--------|
| `dynload_aix/atheos/beos/dl/hpux/os2/shlib/stub.c` | OS-specific dynloaders |
| `dynload_next.c` | macOS/NeXTStep — requires `mach-o/dyld.h` |
| `mactoolboxglue.c` | macOS — requires `Carbon/Carbon.h` |
| `getcwd.c` | POSIX fallback; Windows uses `_getcwd` from CRT |
| `dup2.c`, `strdup.c` | POSIX shims not needed on Windows |
| `pyfpe.c` | SIGFPE handling (Unix-only) |
| `sigcheck.c` | Stub superseded by `Modules/signalmodule.c` |
| `intrcheck.c` | Stub superseded by `Modules/signalmodule.c` |
| `frozenmain.c` | Frozen-exe launcher — unresolved `PyWinFreeze_*` |

**Parser/ (pgen build-tool files not part of the runtime DLL):**
| File | Reason |
|------|--------|
| `pgenmain.c` | pgen `main()` — duplicates `pythonrun`/`errors` symbols |
| `tokenizer_pgen.c` | pgen tokenizer — duplicates `tokenizer.c` symbols |
| `pgen.c` | Parser generator — build tool only |
| `printgrammar.c` | pgen utility — build tool only |
| `intrcheck.c` | Stub superseded by `Modules/signalmodule.c` |

Note: `Parser/intrcheck.c` is distinct from `Python/intrcheck.c` — both exist
in the source tree. Both must be excluded; only the `Parser/` one was causing
the linker conflict (it is picked up by `glob(["Parser/*.c"])`).

### getopt.c — kept
`Python/getopt.c` is Python's own command-line option parser providing
`_PyOS_GetOpt`, `_PyOS_optind`, `_PyOS_optarg` etc. required by
`Modules/main.c`. It is NOT a POSIX shim and must NOT be excluded.

### Overlay PC/config.c
The stock `PC/config.c` references modules not compiled into this minimal DLL
(CJK codecs, zlib, mmap, audioop, imageop, lsprof, hotshot). An overlay
`PC/config.c` is provided that lists only the modules actually compiled in.

### Overlay cache update procedure
After any change to an overlay file:
1. Recompute SHA-256: `cat overlay/FILE | openssl dgst -sha256 -binary | base64`
2. Update `source.json` with `"sha256-<base64>"` for the changed file
3. Run `bazel mod deps --lockfile_mode=refresh`
4. Run `bazel build @python//:python27`

If the analysis cache persists stale (observed with glob changes to external
modules), run `bazel shutdown` before `bazel mod deps --lockfile_mode=refresh`.
