# scp2 — Bazel migration

## What this package does

`scp2` defines the installer package metadata for Apache OpenOffice.  It
contains declarative `.scp` files that describe every file, directory, registry
key, Start-menu entry, module, and profile item that the installer must handle.
There is one `.scp` source module per application component (Writer, Calc, etc.)
plus a large `ooo` module for shared infrastructure.

## Build pipeline

```
macros/macro.pl ─────────────────────────────────────────► langmacros.inc
                       (COMPLETELANGISO_VAR=en-US)

source/templates/modules.pl + module_langpack*.sct ──────► alllangmodules_*.inc
                       (COMPLETELANGISO_VAR=en-US)

source/*/*.scp
  │
  ▼ scp_preprocess.pl         (Perl wrapper: filter SCP2 comments, then cl.exe)
  │  ├─ strips #Keyword lines (SCP2 line comments not valid in cl.exe)
  │  └─ cl.exe /EP /Tc via cmd.exe (stdout redirect, cl.exe found via PATH)
  │     -I main/scp2/inc      (macros.inc + langmacros.inc + alllangmodules_*.inc)
  │     -D_MSC -DWNT -DCOMID=MSC …
  ▼
*.pre  (preprocessed text)
  │
  ▼ pre2par.pl -s *.pre -o *.par
  │  (SOLARENV=main/solenv)
  ▼
osl/*.par  (structured package archive per .scp file)
  │
  ▼ par2script.pl @@listfile -i osl/ -o setup_osl.inf   [setup_osl target]
  ▼
setup/setup_osl.inf  (consumed by instsetoo_native — still deferred)
```

## Bazel targets

| Target | Output | Notes |
|---|---|---|
| `:langmacros_inc` | `langmacros.inc` | Language macro expansion (en-US) |
| `:alllangmodules_incs` | `alllangmodules_*.inc` | Per-module language pack entries |
| `:ooo` | `osl/installation_ooo.par` … (35 files) | Core OOo metadata |
| `:calc`, `:writer`, `:impress`, `:draw`, `:base`, `:math` | `osl/*.par` | Per-app modules |
| `:canvas`, `:graphicfilter`, `:xsltfilter`, `:python` | `osl/*.par` | Extension modules |
| `:activex`, `:quickstart`, `:winexplorerext` | `osl/*.par` | Windows-only modules |
| `:setup_osl` | `setup/setup_osl.inf` | Aggregate installer script |

## Modules not built (deferred or N/A)

| Module | Reason |
|---|---|
| `gnome`, `kde` | Linux-only, not applicable on Windows |
| `javafilter` | Requires Java; Java deferred |
| `crashrep`, `onlineupdate` | Deferred (post-build) |
| `sdkoo` | SDK packaging deferred |
| `layout` | Experimental layout engine deferred |

## Key design decisions

- **cl.exe as preprocessor with SCP2 comment filtering**: The legacy build used
  `cpp.lcc` (a patched GCC cpp) which silently ignores unknown `#Keyword` lines.
  cl.exe fails with C1021 on these. `scp_preprocess.pl` pre-filters the input:
  any line where `#` is followed by an identifier that isn't a C preprocessor
  keyword (`define`, `include`, `ifdef`, etc.) is replaced with a blank line.
  Only `common_brand.scp` uses this pattern (`#File`/`#End` comment block).

- **Perl → cmd.exe → cl.exe chain**: MSYS2 Perl's fork/exec fails for PE
  executables in paths with spaces (`C:/Program Files.../cl.exe`). The Perl
  wrapper uses `system($comspec, '/c', $cmd)` so cmd.exe (no spaces in path)
  does the process creation. cl.exe is referenced by name (not full path) so
  cmd.exe finds it via PATH. `MSYS2_ARG_CONV_EXCL=*` prevents MSYS2 from
  rewriting the `/c` flag to `C:`.

- **alllangmodules via `modules.pl`**: The legacy build generated
  `alllangmodules_*.inc` files from `.sct` templates in `source/templates/`
  using `modules.pl`. The Bazel `alllangmodules_target` rule does the same,
  outputting all `.inc` files to the same directory as `langmacros.inc` so
  the existing `-I` include path covers them.

- **English-only localization**: Both `macro.pl` and `modules.pl` are invoked
  with `COMPLETELANGISO_VAR=en-US`. The ULF→MLF localization pipeline is not
  yet implemented; pre2par.pl is invoked without `-l` so it skips localization.

- **`local = True`**: All actions use `execution_requirements = {"local": "1"}`
  so that MSYS2 Perl and cl.exe are reachable through the user's shell PATH.

- **`SOLARENV=main/solenv`**: pre2par.pl and par2script.pl use
  `use lib ("$ENV{SOLARENV}/bin/modules")` to find their Perl modules.
  Setting `SOLARENV` to a workspace-relative path works because Bazel's action
  working directory is the exec-root where `main/solenv/` is accessible.

## Version constants encoded in scp2.bzl

These replace the `.mk` version includes the legacy build used:

| Constant | Value | Source |
|---|---|---|
| `ICU_MAJOR/MINOR/MICRO` | 4 / 2 / 1 | `ext_libraries/modules/icu/legacy/icuversion.mk` |
| `ISOLANG_MAJOR` | 1 | `main/i18npool/version.mk` |
| `LIBXSLT_MAJOR` | 1 | `ext_libraries/modules/libxslt/legacy/libxsltversion.mk` |
| `RAPTOR_MAJOR` | 0 | `ext_libraries/modules/redland/redlandversion.mk` |
| `RASQAL_MAJOR` | 3 | same |
| `REDLAND_MAJOR` | 0 | same |
