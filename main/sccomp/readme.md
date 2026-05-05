# sccomp — Bazel migration

Builds `solver.dll`: the UNO linear programming solver for Calc, backed by COIN-OR CoinMP.

## Targets

| Target | Output |
|--------|--------|
| `//main/sccomp:solver` | `solver.dll` |

## External dependencies (new bzlmod modules)

Five new modules were created under `ext_libraries/modules/`:

| Module | Version | GitHub source |
|--------|---------|---------------|
| `coinmp` | 1.7.6 | coin-or/CoinMP |
| `coin-coinutils` | 2.9.11 | coin-or/CoinUtils |
| `coin-osi` | 0.106.5 | coin-or/Osi |
| `coin-clp` | 1.15.6 | coin-or/Clp |
| `coin-cgl` | 0.58.5 | coin-or/Cgl |
| `coin-cbc` | 2.8.8 | coin-or/Cbc |

Each module fetches its GitHub release tarball, applies overlay `BUILD.bazel` + `MODULE.bazel` + shared `BuildTools/headers/configall_system{_msc}.h`.

## Key issues resolved

- **Archive structure**: GitHub COIN-OR releases wrap sources under `{LibName}/` — all glob paths must be prefixed (e.g. `Clp/src/*.cpp`, not `src/*.cpp`).
- **`*_BUILD` defines**: Each sub-library needs its own `{LIB}_BUILD` define (e.g. `CLP_BUILD`) so `{Lib}Config.h` includes `config_default.h` → `configall_system.h` → `HAVE_CMATH` etc. Without these, headers fire `#error "don't have header file for math"`.
- **`MAXINT` / `BaseTsd.h`**: CoinMP's `CoinOption.h` defines `MAXINT=2100000000L`; `BaseTsd.h` (included via CRT) redefines it as `((INT)((MAXUINT)>>1))`. Added `_X86_=1` + `/FIwindef.h` copts to CoinMP so `INT`/`UINT` are defined before the macro expands.
- **`ClpCholeskyUfl.cpp`**: Requires AMD/CHOLMOD — excluded from clp build.
- **`CbcClpUnitTest`**: Defined in excluded `unitTestClp.cpp` but referenced by `CbcSolver.cpp` (used by CoinMP via `CbcMain1`). Added `stubs.cpp` overlay to coin-cbc providing a stub that returns -1.
- **`.if.lib` extension**: `cc_binary(linkshared=True)` produces `CoinMP.if.lib`. `cc_library` rejects `.if.lib` in srcs — coinmp exposes a header-only `cc_library` target + separate `filegroup(output_group="interface_library")` for the implib, consumed via `additional_linker_inputs` + `linkopts`.
- **BCR overlay vs. module graph**: `MODULE.bazel` at the version directory root (not in `overlay/`) is what Bazel uses for dependency graph resolution. The `overlay/MODULE.bazel` is injected into the extracted source archive. Both must declare `bazel_dep` entries.
