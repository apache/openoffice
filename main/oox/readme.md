# oox — Bazel migration

Produces `oox.dll` — the OOXML filter library (Excel, PowerPoint, Word import, DrawingML, VML, OLE/VBA).

## Targets

| Target | Output |
|---|---|
| `//main/oox:oox` | `oox.dll` |
| `//main/oox:oox_implib` | `oox.lib` (import library) |
| `//main/oox:oox_headers` | public `inc/oox/**` headers |
| `//main/oox:oox_token_headers` | generated `oox/token/{tokens,namespaces,properties}.hxx` |
| `//main/oox:oox_svx_shims` | forwarding headers for old-build `<svx/escherex.hxx>` etc. |

## Token code generation

Three Perl scripts in `source/token/` generate C++ constant headers from plain-text tables at build time:

- `tokens.pl 1 tokens.txt` → identifiers (`const sal_Int32 XML_xxx = N;`)
- `namespaces.pl 1 namespaces.txt` → namespace shift-encoded constants
- `properties.pl 1 properties.txt` → property index constants

Each is wrapped with a `.hxx.head` / `.hxx.tail` file by three `genrule` targets and exposed via `oox_token_headers` with `includes = ["."]` so `#include "oox/token/tokens.hxx"` resolves against `bazel-bin/main/oox/`.

## svx shim headers

The public export headers (`inc/oox/export/vmlexport.hxx`, `drawingml.hxx`) include `<svx/escherex.hxx>` and `<svx/svxenum.hxx>` using the old-build OUTDIR namespace where several unrelated modules were merged under `svx/`. Forwarding headers generated at `bazel-bin/main/oox/svx/` redirect these to their real locations:

- `<svx/escherex.hxx>` → `<filter/msfilter/escherex.hxx>`
- `<svx/svxenum.hxx>` → `<editeng/svxenum.hxx>`

## Export sources not in oox.dll

`source/export/{drawingml,shapes,vmlexport}.cxx` are compiled separately in the old build (`AUTOSEG=true`) but are **not** linked into `oox.dll` — `Library_oox.mk` does not list them. They will be linked into the application DLLs (sw, sc, sd) at Layer 14+. `vmlexport.cxx` was written against an older `EscherEx` constructor API that differs from the current `filter/msfilter/escherex.hxx`.

## Dependencies

- First-party: sal, cppu, cppuhelper, comphelper, basegfx, sax, xmlscript (xcr), tools, unotools, vos, vcl
- External: `//main/openssl_shim:ssl` (RC4/MD5/SHA for BIFF codec), `@boost.legacy//:boost`

## Notes

- `OOX_DLLIMPLEMENTATION` activates `OOX_DLLPUBLIC = __declspec(dllexport)` — no DEF file needed
- Private headers in `source/{docprop,drawingml,ppt,shape}/` are added to `srcs` and their directories to `_COPTS`
- `ws2_32.lib`, `crypt32.lib`, `advapi32.lib` in `linkopts` for OpenSSL Windows runtime
- `/D_HAS_ITERATOR_DEBUGGING=0` required: VS2008 debug STL validates heterogeneous comparators
  symmetrically (calls `comp(val, range)` as well as `comp(range, val)`); `ValueRangeComp` in
  `containerhelper.cxx` only implements the latter, and `ValueRange`'s `explicit` constructor
  prevents the implicit conversion that the debug check expects.
