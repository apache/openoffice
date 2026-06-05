# Opening a Platform Axis in the Bazel Build

**Status:** investigation / design proposal (no build graph changes)
**Trigger:** trunk commit `5e139d9fe4` — *Native Apple Silicon (arm64) support and
bundled Python 3.10 baseline* — the first real second-platform data point for AOO
since the Bazel migration began.
**Audience:** anyone deciding whether the Bazel branch should grow from a single
Windows/x86 target into a multi-platform matrix.

---

## 0. TL;DR

The current Bazel branch builds exactly **one** point in what is naturally a
three-dimensional space:

```
        OS  ×  CPU      ×  "age" (toolchain / deployment target / dep ABI)
   ┌───────────────────────────────────────────────────────────────────┐
   │ windows   x86_64      VS2008 / WinSDK 7.0 / Python 2.7   ◀── today  │
   │ macos     arm64       clang  / macOS 11   / Python 3.10  ◀── trunk  │
   │ macos     x86_64      clang  / macOS …     …                        │
   │ linux     x86_64      gcc    / …                                    │
   └───────────────────────────────────────────────────────────────────┘
```

Bazel was built to drive this matrix from one graph. The migration so far has
*hardcoded* the single cell (MSVC flags inline in every BUILD file, one toolchain
registered unconditionally, zero `select()` on OS). The arm64 commit is the
forcing function to factor that one cell into a coordinate system.

**The key realization:** the trunk commit does not need to be "ported." It is a
*reference spec* telling us exactly what the second matrix cell looks like
(arch names, deployment target, ABI bridge, dep flags). Opening the axis is a
build-system refactor; the commit supplies the answers it must encode.

---

## 1. The three axes, and why "chips / ages" matters

Bazel models a build target along **constraints**. Two come for free from
`@platforms`; the third we define ourselves.

### Axis 1 — OS  (`@platforms//os`)
`windows`, `macos`, `linux`, … Selects: source sets (`osl/w32` vs `osl/unx`),
the cpp_uno bridge family, the VCL backend (win vs aqua/quartz), packaging.

### Axis 2 — CPU / "chip"  (`@platforms//cpu`)
`x86_64`, `arm64`, … Selects: the *ABI bridge* (the calling-convention
trampoline — the single most arch-specific component in the whole tree),
assembler dialect, atomics, and external-lib build flags (openssl
`darwin64-arm64-cc`, icu endianness).

This is where Apple Silicon lands: same OS (macos), **new chip** (arm64). The
trunk commit's `s5abi_macosx_aarch64/` bridge is precisely "the macos cell,
arm64 column."

### Axis 3 — "age" / generation  (custom constraints + flags)
The dimension people forget. It is not OS and not CPU — it's *which generation
of toolchain, OS floor, and dependency ABI* you target:

- **deployment target / OS floor** — the commit sets macOS 11 as the minimum
  (`LSMinimumSystemVersion 11.0`, `-mmacosx-version-min=11.0`). The same source
  builds differently against a 10.13 floor vs an 11.0 floor.
- **dependency generation** — Python **2.7 → 3.10** (the commit's second half),
  **NSS 3.39 too old for arm64** (deferred in the commit). These are ABI/version
  knobs independent of OS+CPU.
- **compiler generation** — VS2008 today; a clang version on mac.

Modeling "age" as a first-class axis is what lets one branch say *"build the
arm64 mac against the macOS-11 / Python-3.10 generation"* without forking the
tree. It is the difference between a matrix and a pile of `#ifdef`s.

---

## 2. How Bazel drives the matrix (the four primitives)

All four already exist in this repo — we just use them for ATL/lang/debug today,
not for platforms.

### 2.1 `platform()` — a named point in the matrix
```python
# build/platforms/BUILD.bazel  (new)
platform(
    name = "windows_x86",
    constraint_values = ["@platforms//os:windows", "@platforms//cpu:x86_32"],
)
platform(
    name = "macos_arm64",
    constraint_values = [
        "@platforms//os:macos",
        "@platforms//cpu:arm64",
        "//build/constraints:macos_11",   # axis-3 "age" constraint
    ],
)
```
Select a cell at the command line: `bazel build --platforms=//build/platforms:macos_arm64 //main/bridges:s5abi_uno`.

### 2.2 `toolchain()` — auto-resolved per target platform
You already do this. Today's [build/toolchain/BUILD.bazel](build/toolchain/BUILD.bazel)
registers the VS2008 toolchain with `target_compatible_with = [windows, x86_64]`.
Add a sibling and Bazel's toolchain resolution picks the right one automatically
based on `--platforms`:
```python
toolchain(
    name = "cc_toolchain_macos_arm64_def",
    exec_compatible_with  = ["@platforms//os:macos", "@platforms//cpu:arm64"],
    target_compatible_with = ["@platforms//os:macos", "@platforms//cpu:arm64"],
    toolchain      = ":cc_toolchain_macos_clang_arm64",
    toolchain_type = "@bazel_tools//tools/cpp:toolchain_type",
)
```
`register_toolchains(...)` in MODULE.bazel lists both. **The Windows build is
untouched** — when `--platforms` is windows, the macos toolchain is simply not
compatible and never selected.

> **Native vs cross.** The mac toolchain's `exec_compatible_with` is `macos`,
> meaning those actions must *run on a Mac* (a CI runner or dev box, or a remote
> executor). You cannot emit Mach-O/arm64 from the Windows VS2008 host. This is
> a logistics fact, not a Bazel limitation: the same `bazel build` invocation on
> a Mac produces the mac artifacts; on Windows it produces the win artifacts.
> Remote execution (`--remote_executor`) can later let one machine drive both.

### 2.3 `config_setting` — turn a platform into something `select()` can read
Mirror the existing `has_atl` / `build_de` idiom:
```python
# build/BUILD.bazel  (extend)
config_setting(name = "on_windows", constraint_values = ["@platforms//os:windows"])
config_setting(name = "on_macos",   constraint_values = ["@platforms//os:macos"])
config_setting(name = "macos_arm64",
    constraint_values = ["@platforms//os:macos", "@platforms//cpu:arm64"])
```

### 2.4 `select()` — the per-package payload
This is the bulk of the work, but it's mechanical. See §3.

---

## 3. The refactor pattern, concretely (bridges as the worked example)

[main/bridges/BUILD.bazel](main/bridges/BUILD.bazel) is the perfect specimen
because the bridge is *the* most platform-specific module. Today it is
unconditionally MSVC/x86:

```python
# TODAY — one hardcoded cell
_COPTS   = ["/Zm500", "/EHsc", "/Dsnprintf=_snprintf",
            "/Imain/bridges/source/cpp_uno/msvc_win32_intel", ...]
_DEFINES = ["WNT", "WIN32", "CPPU_ENV=msci", "_X86_=1", ...]

cc_binary(
    name = "msci_uno",
    srcs = [ ...shared..., "source/cpp_uno/msvc_win32_intel/cpp2uno.cxx", ... ],
    win_def_file = "util/msci_uno.def",
)
```

The platform-axis form keeps the shared sources common and `select()`s the rest:

```python
# REFACTORED — coordinate-driven
_SHARED_SRCS = [ "source/cpp_uno/shared/bridge.cxx", ... ]   # platform-neutral

cc_binary(
    name = "cpp_uno_bridge",                         # one logical target
    srcs = _SHARED_SRCS + select({
        "//build:on_windows": glob(["source/cpp_uno/msvc_win32_intel/*.cxx",
                                     "source/cpp_uno/msvc_win32_intel/*.hxx"]),
        "//build:macos_arm64": glob(["source/cpp_uno/s5abi_macosx_aarch64/*.cxx",
                                     "source/cpp_uno/s5abi_macosx_aarch64/*.hxx",
                                     "source/cpp_uno/s5abi_macosx_aarch64/*.s"]),
    }),
    copts = _COMMON_COPTS + select({
        "//build:on_windows": _MSVC_COPTS,     # /EHsc /Zc:wchar_t- /Imsvc_win32_intel ...
        "//build:on_macos":   _CLANG_COPTS,    # -fexceptions -mmacosx-version-min=11.0 -I.../s5abi_macosx_aarch64
    }),
    defines = select({
        "//build:on_windows": ["WNT", "WIN32", "CPPU_ENV=msci", "_X86_=1"],
        "//build:macos_arm64": ["MACOSX", "AARCH64", "CPPU_ENV=s5abi"],
    }),
    # MSVC needs a .def; Mach-O uses an exported-symbols list passed via linkopts.
    win_def_file = select({
        "//build:on_windows": "util/msci_uno.def",
        "//conditions:default": None,
    }),
    linkopts = select({
        "//build:on_windows": [...implibs...],
        "//build:on_macos":   ["-Wl,-exported_symbols_list,$(location util/s5abi_uno.exp)"],
    }),
)
```

The recurring translation table (applies to ~every module, not just bridges):

| Concern              | Windows (today)                       | macOS (to add)                              |
|----------------------|---------------------------------------|---------------------------------------------|
| Exception model      | `/EHsc`                               | `-fexceptions`                              |
| `sal_Unicode` ABI    | `/Zc:wchar_t-`                        | (native, no flag)                           |
| Symbol export        | `win_def_file = *.def`                | `-exported_symbols_list *.exp`              |
| Assembler            | `ml.exe` `.asm` (toolchain `assemble`)| clang `.s` (GAS) — already a `.s` in commit |
| `snprintf` shim      | `/Dsnprintf=_snprintf`                | (none)                                      |
| Defines              | `WNT WIN32 _X86_=1 CPPU_ENV=msci`     | `MACOSX AARCH64 CPPU_ENV=s5abi`             |
| C runtime            | `/MD` + SxS manifest staging          | `-stdlib=libc++`                            |

None of this changes the Windows output: every `select()` keeps the existing
Windows arm verbatim and *adds* a macOS arm. The factoring is the cost; the
Windows cell is byte-stable if the windows branch is copied exactly.

---

## 4. What each part of commit 5e139d9fe4 becomes under the axis

| Commit file(s)                                   | Axis role                          | Bazel action |
|--------------------------------------------------|------------------------------------|--------------|
| `bridges/source/cpp_uno/s5abi_macosx_aarch64/*` (7 src) | macos × arm64 ABI bridge   | new `srcs` arm in the bridge `select()` (§3) |
| `bridges/Library_cpp_uno.mk`, `makefile.mk`      | dmake — being deleted              | **ignore** (reference only) |
| `sal/osl/unx/interlck.c`, `jvmfwk/.../util.cxx`  | macos source sets                  | enter `osl/unx` / jvmfwk `select()` arms (don't exist yet) |
| `sal/rtl/source/macro.hxx`                       | AARCH64 arch string                | already harmless; no action |
| `openssl/makefile.mk`, `icu/*.patch`             | external build, per-chip           | arm64 Configure variant in the openssl/icu bzlmod/foreign_cc wrappers |
| `set_soenv.in`, `source_soenv.sh`, `solenv/inc/unx*.mk`, `gbuild/platform/macosx.mk`, `configure.ac` | dmake/autoconf env | **ignore** — these are exactly the layer Bazel replaces; read them as the *spec* for the axis-3 constants (CPU=R, OUTPATH=unxmaccr, deploy 11.0) |
| `solenv/bin/modules/installer/*.pm`, `osarch.pm`, `update_module_ignore_lists.pl` | Perl installer | ignore (post-build packaging, separately deferred) |
| `setup_native/.../Info.plist*`, `sysui/.../Info.plist` | mac packaging          | feeds a future mac staging/bundle rule |
| `python/pyversion*.mk`                           | axis-3 "age" knob (dmake)          | becomes a Bazel `--//build:python_gen` flag / dep version |
| `pyuno/.../pythonloader.py`, `scripting/.../pythonscript.py`, `mailmerge.py` | **shipped Py3 source** | **the only cross-platform-relevant files** — see §5 |

---

## 5. The Python "age" axis — the one piece that touches *today's* build

Everything else above is dormant until a mac target exists. The three `.py`
files are different: they are **staged into the install on every platform**, and
the commit rewrites them for **Python 3.10** (`imp`→`types.ModuleType`,
`file()`→`open()`, Py3 `email` imports).

Today's Bazel branch bundles **Python 2.7** — [main/pyuno/BUILD.bazel](main/pyuno/BUILD.bazel)
links `@python//:python27_implib` and `MODULE.bazel` pins `python 2.7.18`.
The commit explicitly does **not** do the Windows Python-3 build. So:

> Merging those three files in isolation = Py3-only loader scripts shipped against
> a Py2.7 interpreter = broken pyuno/scripting on Windows.

This is the cleanest illustration of why "age" must be an explicit axis: the
`.py` content is a function of the *Python generation*, which is independent of
OS and CPU. The correct model is a `python_gen` selector that picks **both** the
interpreter dep (`@python//:...` 2.7 vs 3.10 implib) **and** the matching loader
sources — so you never ship py3 scripts against a py2 runtime. Until that knob
exists, the `.py` changes stay out of the Windows build.

---

## 6. Why this is worth doing — what the axis unlocks

1. **One graph, every target.** `bazel build --platforms=//build/platforms:macos_arm64 //main/sal`
   builds sal for mac; flip the flag for Windows. No second checkout, no fork.
2. **Incremental, module-by-module ports.** Because each `select()` defaults to
   the Windows arm, you can land mac arms one package at a time (sal → cppu →
   bridges → …) with the Windows build green the entire way. The migration
   frontier in CLAUDE.md gains a *second column* instead of a second branch.
3. **CI matrix for free.** A GitHub Actions matrix of {windows-runner,
   macos-arm64-runner} each runs `bazel test //...` against its `--platforms`.
   The bridge especially becomes *unit-testable* (bridgetest) per arch — the
   commit notes bridgetest was never run; under Bazel it's a `cc_test`.
4. **"Ages" become switchable, not forked.** Python 2.7↔3.10, macOS-11 floor,
   future compiler bumps — each a flag, not a code fork. You can prove a
   Python-3 migration on a branch flag while trunk-equivalent stays on 2.7.
5. **Universal binaries later, cheaply.** Once macos×{x86_64,arm64} both build,
   a Bazel *transition* + `lipo` action produces a fat binary from the same
   targets — a natural extension of the axis, not new architecture.
6. **Hermetic cross-builds down the road.** Splitting exec vs target platforms
   (already expressed in every `toolchain()`) is the same lever that, with a
   clang cross-toolchain + macOS SDK sysroot, eventually lets Linux/remote
   executors build mac artifacts — removing the "must own a Mac" constraint.
7. **It documents the platform contract.** The translation table in §3 is
   currently tribal knowledge buried in CLAUDE.md's cross-cutting-flags section.
   Making it a `select()` turns the contract into enforced, reviewable code.

---

## 7. Suggested sequence (each step keeps Windows green)

```
0. Scaffold (no compile): build/platforms/, build/constraints/ (os_version,
   python_gen), config_settings in build/BUILD.bazel. Windows build identical.
1. Register a macos clang/arm64 cc_toolchain (target_compatible_with macos+arm64).
   Verify Windows toolchain resolution is unaffected (it is — incompatible).
2. Pilot one leaf with no platform divergence to prove the select() plumbing.
3. Port sal: introduce the osl/w32 ↔ osl/unx srcs select(); add interlck.c arm64.
4. Port the bridge (§3) — the headline arm64 deliverable; add cpp2uno/uno2cpp/
   call.s/abi.cxx as the macos_arm64 arm. Stand up bridgetest as a cc_test.
5. Externals: openssl darwin64-arm64-cc + icu arm64 arms in their wrappers.
6. Decide the Python "age" axis (§5) independently of mac — it's the only piece
   with Windows-build consequences.
7. Everything past the bridge (VCL aqua backend, packaging .app bundle,
   code-signing) is genuinely new module work — track as a second frontier
   column, not a port.
```

The honest scope: steps 0–2 are days. Steps 3–5 are the real refactor (touching
many BUILD files, but mechanically). Step 7 (aqua VCL + signing) is the large,
genuinely-new platform bring-up the commit itself flags as out of scope
("Code-signing infra … NOT included", aqua backend untouched).

---

## 8. What does *not* change

- The Windows/x86 build output — every `select()` preserves the current arm; the
  VS2008 toolchain stays the registered default; nothing in the 31-file commit is
  in today's Bazel graph.
- The "no source changes" rule — opening the axis is pure build-system work. The
  arm64 bridge sources are *new files from upstream*, not edits to existing code.
- The migration philosophy — this extends the dmake-elimination goal to a second
  platform; it does not reintroduce gmake/dmake (those `.mk` files stay ignored).
