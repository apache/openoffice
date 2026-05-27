# more_fonts — Bazel Migration

Provides bundled TTF fonts for `share/fonts/truetype/`. Each font family is an
external download fetched during the dmake build from SourceForge or similar hosts.

## Font families

| Directory | Font | License |
|---|---|---|
| `fonts/ttf_dejavu/` | DejaVu 2.37 | Bitstream Vera / public domain |
| `fonts/ttf_carlito/` | Carlito (Calibri-compatible) | Apache 2.0 |
| `fonts/ttf_croscore/` | Croscore (Arimo, Tinos, Cousine) | Apache 2.0 |
| `fonts/ttf_crosextra/` | Crosextra (Caladea, Cambria-compatible) | Apache 2.0 |
| `fonts/ttf_gentium/` | Gentium | SIL OFL 1.1 |

## Status: Deferred

**Blocker:** SourceForge uses per-request token redirect URLs that change on every
download. Bazel's `http_archive` requires stable, content-addressed URLs — it
fetches the URL twice (for integrity verification) and the token expires between
requests, causing a SHA-256 mismatch error.

**Workaround options:**
1. Mirror the tarballs to a stable host (e.g. GitHub releases, Google Storage)
   and add `http_archive` rules in `MODULE.bazel` with verified SHA-256 hashes.
2. Vendor the extracted TTF files directly into `ext_libraries/`.

## OpenSymbol

The critical OOo symbol font (`opens___.ttf`) is shipped in the source tree under
`main/extras/source/truetype/symbol/` and is staged via `//main/extras:opensymbol_ttf`.
It does not depend on any external download.

## Windows note

On Windows, OOo's VCL font scanner registers fonts found in `share/fonts/truetype/`
alongside system fonts. The bundled fonts are most important on Linux where system
font availability varies. A Windows build can function without the DejaVu/Carlito
families as long as metric-compatible system fonts are present.
