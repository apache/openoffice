# BUILD-file Styleguide — AOO Bazel Migration

How to author `BUILD.bazel` and `.bzl` files so a new module looks like the
ones already migrated. This is the **code** styleguide for this repo — the
actual C++/Java source is out of scope (see [CLAUDE.md](CLAUDE.md) "Goal").

For *operating* rules (build commands, workflow, guardrails) see
[CLAUDE.md](CLAUDE.md). For *why* a given pattern exists, the
`MEMORY.md` milestone for that module is the primary record. This file is
the **generalization** of those ~50 milestones into copy-pasteable shape.

Canonical reference example to imitate: [main/basctl/BUILD.bazel](main/basctl/BUILD.bazel)
(a full first-party DLL with SDI, RSC, shielded includes, implib export).
Foundational library example: [main/sal/BUILD.bazel](main/sal/BUILD.bazel).

---

## 1. Module BUILD skeleton

A first-party module DLL is built in this order. Copy the shape, not the names.

```python
package(default_visibility = ["//visibility:public"])

load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")
# load only the pipelines this module actually uses (rsc_res, sdi_target, ...)

# 1. Defines and copts as module-local _DEFINES / _COPTS lists (see §2)
_DEFINES = [ "WNT", "GUI", "WIN32", ... , "snprintf=_snprintf", "SUPD=680" ]
_COPTS   = [ "/Zc:wchar_t-", "/Imain/<mod>/inc", "/Imain/<mod>/source/inc", ... ]

# 2. Header / include "shield" libraries (see §3)
cc_library(name = "<mod>_inc",        textual_hdrs = glob(["inc/**"]),        strip_include_prefix = "inc")
cc_library(name = "<mod>_source_inc", textual_hdrs = glob(["source/inc/**"]), strip_include_prefix = "source/inc")

# 3. Generated inputs (SDI slots, IDL, etc.) — only if the module uses them
sdi_target(name = "<mod>slots_sdi", ...)
cc_library(name = "<mod>slots_headers", hdrs = [":<mod>slots_sdi"], includes = ["<mod>slots_sdi_inc"])

# 4. The DLL itself
cc_binary(
    name = "<mod>",
    srcs = ["inc/pch/precompiled_<mod>.cxx"] + glob(["source/**/*.cxx", "source/**/*.hxx"]),
    copts = _COPTS,
    defines = _DEFINES,
    linkshared = True,
    win_def_file = "util/<mod>.def",      # see §5
    deps = [ ":<mod>_inc", ... , "//main/sal:sal_headers", ... ],   # headers only
    additional_linker_inputs = [ "//main/sal:sal_implib", ... ],    # implibs (see §5)
    linkopts = [ "$(execpath //main/sal:sal_implib)", ... , "shell32.lib" ],
)

# 5. Re-export this module's own implib for downstream consumers (see §5)
filegroup(name = "<mod>_implib", srcs = [":<mod>"], output_group = "interface_library")

# 6. Resources / config, if any (see §6)
rsc_res(name = "<mod>_res", ...)
filegroup(name = "uiconfig_files", srcs = glob(["uiconfig/**/*.xml"]), visibility = ["//main/postprocess:__pkg__"])
exports_files(glob(["**/*.component"]))
```

**Driving the dep graph:** parse the module's legacy `build.lst` /
`makefile.mk` to derive `deps`. `SLOFILES`/`OBJFILES` lists tell you exactly
which sources belong in the library — prefer matching them over a blind
`glob`, and `exclude` the ones a glob over-collects (see `main/sal` rtl: it
excludes `gen_makefile.cxx`, `alloc_fini.cxx`, `strtmpl.c` because they are
not in `SLOFILES`).

---

## 2. Defines & copts conventions

- Hoist into module-local `_DEFINES` and `_COPTS` lists at the top of the file
  so every target in the module shares them. Don't inline per-target.
- The cross-cutting flags (`/Zc:wchar_t-`, `snprintf=_snprintf`,
  `stlport` dep, `Z_PREFIX`, `CURL_STATICLIB`, …) are catalogued in
  [CLAUDE.md](CLAUDE.md) "Cross-cutting compiler flags & defines". **Check that
  list before building any new module** — most "mysterious" link/ABI failures
  are a missing entry from it.
- `_HAS_ITERATOR_DEBUGGING=0` is set **globally** in the toolchain — do **not**
  re-add it per module (redundant; see CLAUDE.md).
- Use a `_SLO_DEFINES = _DEFINES + ["SHAREDLIB", "_DLL_"]` derived list when a
  module builds both shared- and static-lib object sets (see `main/sal`).

---

## 3. Idioms to copy

### Shield (`_inc`) libraries — beat header-shadowing by name
Many modules have identically-named headers (`helpid.hrc`, `*sh.hrc`). Expose a
module's own `inc/` and `source/inc/` as **separate** `cc_library` targets with
`strip_include_prefix`, listed **first** in `deps`, so they resolve before any
transitively-added same-named header. Use `textual_hdrs` (not `hdrs`) for
`.hrc`/`.tab`/`#include`d data files so Bazel doesn't try to compile-check them.
Reference: [main/basctl/BUILD.bazel](main/basctl/BUILD.bazel) `basctl_inc` /
`basctl_source_inc`, and the cui/basctl milestones in `MEMORY.md`.

### Per-subdir `/I` copts for flat sibling includes
Source that includes a sibling header by bare name (`<baside2.hrc>`,
`"precompiled_sal.hxx"`) needs that exact directory on the include path. Add a
`/Imain/<mod>/<subdir>` copt per such subdir (basctl adds `/Imain/basctl/source/basicide`).

### `_headers` libraries are the public contract
A module exposes its API to others as `//main/<mod>:<mod>_headers` (headers +
`strip_include_prefix` + `includes`), never by depending on its `cc_binary`.
Downstream `deps` carry **only** `_headers`; linking is via implibs (§5).

### Generated headers get a wrapper `cc_library`
SDI/IDL/MIDL output is wrapped: `hdrs = [":<gen>"]`, `includes =
["<gen>_inc"]` (or `includes = ["."]` / `[".."]` for generated-header dirs).
Reference: `basslots_headers`, and the winaccessibility/writerfilter milestones.

### `textual_hdrs` for included data tables
`.tab` files (`main/sal` textenc), `hash.cxx` (sdext pdfimport), private
`types.idl`-derived headers — anything `#include`d rather than compiled — goes
in `textual_hdrs`.

---

## 4. Naming conventions

| Target | Pattern | Example |
|---|---|---|
| Public header lib | `<mod>_headers` | `sal_headers`, `tools_headers` |
| Private include shield | `<mod>_inc`, `<mod>_source_inc` | `basctl_inc` |
| Resource-constant shield | `<mod>_source_hrc`, `<mod>_hrc` | `basctl_source_hrc`, `svl_hrc` |
| The DLL | `<mod>` (bare) | `basctl`, `sal` |
| Re-exported implib | `<mod>_implib` (or `<linkname>_implib`) | `basctl_implib`, `comphelpMSC_implib` |
| SDI slot gen | `<name>slots_sdi` + `<name>slots_headers` | `basslots_sdi` |
| RSC resource | `<mod>_res` | `basctl_res` |
| DEF file | `util/<linkname>.def` | `util/basctl.def` |

The **implib name often differs from the module name** because it matches the
upstream link-name, not the directory (`comphelpMSC_implib`, `tl_implib`,
`utl_implib`, `svxcore_implib`). When in doubt, check the consumer's
`additional_linker_inputs` list.

---

## 5. The DLL link triad (DEF + implib in + implib out)

A first-party DLL has three linking concerns that must stay consistent:

1. **Exports** — convert the upstream `<name>.map` GNU-ld script to a Windows
   `util/<name>.def` and set `win_def_file`. UNO component DLLs export exactly
   `component_getImplementationEnvironment`, `component_getFactory`,
   `component_canUnload`. If there is no `.map`, hand-write a minimal `.def`
   (see starmath milestone: `sm.def`/`smd.def`, no `canUnload`).
2. **Consuming others' implibs** — list each dependency's implib in **both**
   `additional_linker_inputs` (so Bazel stages the file) **and** `linkopts` as
   `$(execpath //main/<dep>:<dep>_implib)`. Both are required; the
   `additional_linker_inputs` entry alone won't put it on the link line.
   Plain system libs (`shell32.lib`, `user32.lib`, `ws2_32.lib`) go straight
   in `linkopts`.
3. **Exposing your own implib** — end the file with
   `filegroup(name="<mod>_implib", srcs=[":<mod>"], output_group="interface_library")`
   so downstream modules can link you.

---

## 6. Resources, config, components

- **RSC** (`rsc_res`): `srcs` = `.src`, `hdrs` = `.hrc` (own + cross-module
  `*_inc_hrc` filegroups), `includes` = every `.hrc` search dir, and
  **`images_root = "main/default_images"`** — this exact value is mandatory for
  any module with `Bitmap { File = "x.png" }` (see CLAUDE.md; a module-specific
  `images_root` silently empties every `BitmapEx(ResId)`).
- **uiconfig**: expose as a `filegroup` visible to `//main/postprocess` only;
  it is staged as a **folder tree**, never a zip.
- **`.component`** files: `exports_files(glob(["**/*.component"]))` so the
  postprocess/services.rdb pipeline can read them.

---

## 7. `.bzl` custom rules

When a build step has no `cc_*`/`genrule` fit, it gets a Starlark rule under
`build/rules/<thing>.bzl` (existing: `idl_pipeline`, `rsc_pipeline`,
`sdi_target`, `images`, `scp2`, `localedata_pipeline`, `gtest_test`, …).
Conventions, by example:

- Prefer `ctx.actions.run` with explicit tool inputs over `run_shell`; use
  `ctx.actions.symlink` (pure Bazel) for staging, not a shell `cp`.
- Tools that link `/MD` need their CRT DLLs + **external** `.manifest` staged
  beside the EXE (the `svidl_bundle` pattern — replicated in `gtest_test.bzl`;
  miss it and you get R6034). See the test-infra and IDL milestones.
- A rule that emits headers must advertise the include dir via an `includes`
  provider/attr so the wrapper `cc_library` can surface it.
- Keep the rule's migration rationale in its module `readme.md` and a one-line
  `MEMORY.md` pointer — that is where future-you will look.

---

## 8. Anti-patterns (do NOT do)

- ❌ Put a BUILD file in `prj/`. First-party modules need `glob()` over sources
  → BUILD lives at `main/<mod>/BUILD.bazel`.
- ❌ Depend on another module's `cc_binary` to get its headers. Use `_headers`.
- ❌ Module-specific `images_root`. Always `"main/default_images"`.
- ❌ Re-add `_HAS_ITERATOR_DEBUGGING=0` per module (global already).
- ❌ Put an implib only in `additional_linker_inputs` (also needs `linkopts`).
- ❌ Stage uiconfig as a zip (must be a folder tree).
- ❌ Manipulate the Bazel cache directly. Use
  `bazel mod deps --lockfile_mode=refresh` or ask the user.
- ❌ Modify C++/Java source to make a build pass. Source is out of scope; fix
  the build description instead. (If a source bug is genuinely blocking, raise
  it — see the latent-UAF precedent — don't silently patch.)

---

## 9. After a module builds

Per [CLAUDE.md](CLAUDE.md) workflow: **let the user build** (don't run the build
yourself), add a `readme.md` migration summary to the module, add a one-line
`MEMORY.md` milestone pointer, and **only then** update the frontier in
CLAUDE.md. The `MEMORY.md` entry is what makes the next similar module fast.
