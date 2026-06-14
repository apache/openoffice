# RDF stack — Bazel migration (Redland / raptor2 / rasqal + unordf)

Status as of **2026-06-14**: the full RDF library stack and the `unordf.dll` UNO
component **build, link, and stage** under Bazel (MSVC VS2008, x86, debug CRT).
The document-metadata *runtime* path (saving an ODF writes `manifest.rdf`) is the
remaining open item — see [Open items](#open-items).

## Why this exists

ODF document metadata (RDF/XML in `manifest.rdf`, package metadata) is provided
by the `librdf_Repository` UNO service in **`unordf.dll`** (`main/unoxml/source/rdf/`).
That component is backed by the **Redland** C library (`librdf`), which in turn
depends on **raptor2** (RDF parsers/serializers) and **rasqal** (SPARQL query).

Without `unordf.dll` the `librdf_Repository` service cannot activate, so saving a
Writer/Calc/Impress document fails (the file dialog re-prompts / save silently
aborts). The component had been **deferred** in the original migration with the
note *"redland not yet on Windows"*.

## Layout

| Module | Kind | Output | BUILD style |
|--------|------|--------|-------------|
| `@raptor2` (`ext_libraries/modules/raptor2/2.0.15`) | third-party C | `raptor2.lib` (static) | native cc_library |
| `@rasqal` (`ext_libraries/modules/rasqal/0.9.33`) | third-party C | `rasqal.lib` (static) | native cc_library |
| `@redland` (`ext_libraries/modules/redland/1.0.17`) | third-party C | `redland.lib` (static) | native cc_library |
| `//main/unoxml:unordf` | first-party C++ | `unordf.dll` | cc_binary (UNO component) |

`unordf.dll` links all three static libs + `@libxml2` + `@libxslt`, plus the usual
UNO implibs (sal/cppu/cppuhelper/comphelper/ucbhelper/sax).

## The central decision: native-cc, not configure_make

The migrator originally wrapped all three libraries with **`configure_make`**
(rules_foreign_cc). That is a dead end in this environment and was reverted to
**native `cc_library`** for every lib:

- `configure_make` needs a bash + `make` toolchain. rules_foreign_cc tries to
  **bootstrap GNU Make from source with MSVC**, which fails on VS2008
  (`w32os.c` uses C99 mid-block declarations the VS2008 C compiler rejects) →
  no `gnumake.exe`.
- Even with a `make`, `./configure` would have to find the **MSVC-built**
  raptor2/rasqal via pkg-config (no `.pc` files) — a cross-toolchain mismatch.
- It directly contradicts the project's primary constraint: **eliminate the
  Cygwin/bash/make dependency**.

Native-cc means: no configure, no make, no bash; one consistent MSVC toolchain;
clean Bazel `deps = ["@raptor2//:raptor2", ...]`. Source lists and `-D` defines are
taken **verbatim from each library's dmake recipe** (`src/makefile.mk` added by the
`*.patch.dmake` patch — `SLOFILES` + `CDEFS`), so the build matches what AOO
shipped on Windows.

## How the libraries are wired (the `overlay` mechanism)

These modules were set up with `"build_file": "BUILD.bazel"` in `source.json`,
which is **ignored** in this bzlmod local-registry setup (proven: `@@rasqal+` had
no BUILD; rasqal's checked-in `BUILD.bazel` was even 0 bytes). The working
mechanism is the icu-proven **`overlay`**:

```
ext_libraries/modules/<lib>/<ver>/
  source.json        # url, integrity, strip_prefix, overlay{path: sha256}, patches{}
  MODULE.bazel       # registry metadata (bazel_deps the BUILD uses)
  overlay/
    BUILD.bazel      # the real build (referenced by SHA-256 in source.json)
    MODULE.bazel
    src/...          # any added/patched source or config files
```

Each file in the `overlay` map is referenced by its SHA-256; after editing any
overlay file you must recompute the hash, update `source.json`, delete the stale
`external/<lib>+` dir, and rebuild with `--lockfile_mode=refresh`. (See the
"Bazel 9.0.1 last-patch-section bug" memo — patches are avoided in favour of
overlay files for the same reason.)

## Per-library gotchas (the ones that bit)

These are the non-obvious things; each was a separate build failure.

### Common to raptor2 / rasqal / redland

1. **`<LIB>_INTERNAL` define is mandatory.** Each `*_internal.h` wraps its entire
   body (structs, typedefs, `RAPTOR_CALLOC`/etc. macros) in
   `#ifdef RAPTOR_INTERNAL` / `RASQAL_INTERNAL` / `LIBRDF_INTERNAL`. Without it
   every internal type is "undefined" in every source. Autotools supplies it via
   `@<LIB>_INTERNAL_CPPFLAGS@`.

2. **`HAVE_CONFIG_H` + dual config headers.** Sources do
   `#ifdef HAVE_CONFIG_H #include <lib_config.h>` *and*
   `#ifdef WIN32 #include <win32_lib_config.h>`. dmake defines **both** and copies
   the `.win32`-patched `lib_config.h.in` → `lib_config.h`. We reproduce that
   generated header as an **overlay file** (apply the patch's config hunk with
   `patch -p2`, stage the result), and define `HAVE_CONFIG_H`. `win32_*_config.h`
   carries the MSVC shims (`snprintf`→`_snprintf`, `stricmp`, `access`→`_access`,
   `RASQAL_INLINE __inline`, `struct timeval`, …).

3. **`local_defines`, not `defines`.** Build-private macros (`HAVE_*`, `PACKAGE`,
   `VERSION`, `*_INTERNAL`, parser/serializer flags) must be `local_defines` — as
   `defines` they propagate to every dependent and clobber its config. Concretely:
   raptor2's `HAVE_STDINT_H=1` leaked into rasqal and forced
   `#include <stdint.h>`, which VS2008 lacks. **Only the `*_STATIC` guard is a
   propagating `defines`** entry — consumers' public headers need it (see #4).

4. **`*_STATIC` for static linking.** `librdf.h`/`raptor2.h`/`rasqal.h` gate their
   API macro: `#ifdef WIN32 → #ifdef <LIB>_STATIC → (empty) #elif <LIB>_INTERNAL →
   dllexport #else → dllimport`. Building static libs with plain C symbols requires
   `REDLAND_STATIC` / `RAPTOR_STATIC` / `RASQAL_STATIC`, and the **consumer**
   (`unordf`) must see the same or it links against `__imp_*` (dllimport) symbols
   that the static `.lib` doesn't have. Hence each lib exports its `*_STATIC` via
   propagating `defines`.

5. **Empty `unistd.h` stub.** Flex lexers and a few sources `#include <unistd.h>`,
   which MSVC lacks. dmake satisfied this with the empty soltools/winunistd stub on
   `-I<solver>/inc`; we drop the same empty stub into each lib's `overlay/src/`
   (found via the existing `-Isrc`).

### raptor2-specific

- **`expand_template` prefix collisions.** `raptor_config.h` is generated from
  `raptor_config.h.in` via `expand_template`, which does *sequential substring*
  replacement. Keys that are a prefix of a longer key corrupt the longer line
  (`#undef HAVE_GETOPT` → `/* #undef HAVE_GETOPT */_H`). Fix: newline-terminate the
  shorter colliding keys (`"#undef HAVE_GETOPT\n"`). Colliders: `HAVE_GETOPT`
  (⊂`_H`,`_LONG`), `HAVE_SETJMP` (⊂`_H`), `PACKAGE` (⊂`_NAME`/`_STRING`/…),
  `RAPTOR_SERIALIZER_RDFXML` (⊂`_ABBREV`). The template is LF.
- **`RAPTOR_WWW_NONE`.** raptor's libxml HTTP backend (`raptor_www_libxml.c`) calls
  `xmlNanoHTTP*`, which our libxml2 was built without. ODF metadata is local-file
  only, so select `RAPTOR_WWW_NONE` (the file then compiles to nothing).
- **`S_ISREG`** is not a macro on MSVC (`raptor_uri.c` uses it). Injected
  `#define S_ISREG(m) (((m) & _S_IFMT) == _S_IFREG)` via the windows config
  (`_S_IFMT`/`_S_IFREG` come from `<sys/stat.h>`).
- **`strcasecmp.c` omitted** — MSVC has `stricmp`, so the autotools STRCASECMP
  condition is false; `HAVE_STRICMP` maps `raptor_strcasecmp`→`stricmp`. Compiling
  `strcasecmp.c` would redefine `stricmp` and clash with the CRT.
- **`raptor_uri_win32.c` is a phantom** — it does not exist in the tarball; Windows
  URI handling is inline in `raptor_uri.c` under `#ifdef WIN32`.
- **genrule `copy` + `+`.** The bzlmod repo dir is `raptor2+`; `cmd.exe copy`
  treats `+` as the file-concatenation operator, so the `raptor_config.h` copy
  genrule uses `type "$<" > "$@"` instead.

### rasqal-specific

- **`SV_CONFIG`** define — `libsv/sv.c` only includes `sv_config.h` under
  `#ifdef SV_CONFIG`, which renames `sv_*`→`rasqal_sv_*`. Without it, link mismatch
  with `rasqal_format_sv.c`.
- **`src/rasqal_config.h` overlay** is required even though sources gate it on
  `HAVE_CONFIG_H`, because `libsv/sv_config.h` includes it *unconditionally*.
- **`rasqal_ntriples.c`** is the one source upstream that lacks the
  `#ifdef WIN32 #include <win32_rasqal_config.h>` line (it needs `struct timeval`);
  the `.win32` patch adds it — reproduced as an overlay file.
- Library source set = `glob(src/*.c)` minus `rasqal_query_test.c`, plus
  `libsv/sv.c` (the one unconditional `LIBADD`); libmtwist/getopt skipped (not in
  the Windows config).

### redland-specific

- 35-file source set from the dmake `SLOFILES` — SQL / mysql / postgresql / sqlite /
  tstore / virtuoso / bdb backends and `memcmp.c` are excluded.
- Overlay files: `.win32`-patched `rdf_config.h`, `win32_rdf_config.h`,
  `rdf_concepts.c`, plus the `unistd.h` stub. `.ooo_build`/`.autotools` patches only
  touch `Makefile.am`/`configure` (irrelevant to native-cc) and are skipped.
- `redland.h` itself has no dllexport/dllimport macros, but the per-subsystem
  headers use `REDLAND_API` (see #4) → `REDLAND_STATIC`.

## unordf.dll

- Own `component_getFactory` (cannot merge into `unoxml.dll` — symbol collision),
  so it is a separate cc_binary in `main/unoxml/BUILD.bazel` with `linkshared = True`.
- `deps = _DEPS + ["@redland//:redland", "@libxslt//:libxslt"]` —
  `librdf_repository.cxx` includes `<libxslt/security.h>`; redland pulls raptor2 +
  rasqal + libxml2 transitively.
- Registered in `services.rdb` via `main/postprocess/BUILD.bazel`
  (`unordf.component` → `basis_native("unordf.dll")`) and staged in
  `main/staging/BUILD.bazel`.

## Open items

- **Save runtime path.** `unordf.dll` loads, but saving an ODF document does not yet
  complete (the save dialog re-prompts). This is a runtime exception inside the
  `librdf_Repository` activation / metadata write, *not* a build problem.
  Next diagnostic step: capture the thrown UNO/C++ exception type during the save
  attempt (cdb `sxe -c "... e06d7363"` decoding the `_ThrowInfo` type descriptor)
  to determine whether a redland storage/parser/serializer factory fails to
  register at runtime, or the failure is elsewhere (file picker, IO).
- Cosmetic `.uno` infix divergence for the component DLL name is the same known
  divergence documented in `CLAUDE.md`.

## Rebuild recipe (any RDF lib change)

```powershell
# after editing an overlay file + updating its sha256 in source.json:
Remove-Item -Recurse -Force C:\Users\legin\_bazel_legin\<hash>\external\<lib>+ -ErrorAction SilentlyContinue
bazel build //main/staging:install --compilation_mode=dbg --//build:debug_info=True `
  --features=generate_pdb --jobs=1 --lockfile_mode=refresh
```
