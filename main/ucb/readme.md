# ucb — Universal Content Broker

## Bazel migration summary

13 shared libraries migrated to `cc_binary(linkshared=True)`.

| Target | DLL | Notes |
|--------|-----|-------|
| `ucb1` | ucb1.dll | Core UCB broker |
| `cached1` | cached1.dll | Content result-set caching |
| `srtrs1` | srtrs1.dll | Content result-set sorting |
| `ucpexpand1` | ucpexpand1.dll | Macro-expand UCP |
| `ucpext` | ucpext.dll | Extension UCP |
| `ucpfile1` | ucpfile1.dll | File-system UCP |
| `ucphier1` | ucphier1.dll | Hierarchy UCP |
| `ucppkg1` | ucppkg1.dll | Package-archive UCP |
| `ucptdoc1` | ucptdoc1.dll | Transient-document UCP |
| `ucpftp1` | ucpftp1.dll | FTP UCP — needs `@curl` |
| `ucpdav1` | ucpdav1.dll | WebDAV UCP — needs `@curl` + `@openssl` |

## External dependencies added to MODULE.bazel

- `curl 8.7.1` — static lib; `@curl//:curl` used by ucpftp1 and ucpdav1
- `openssl 3.0.13` — static lib; `@openssl//:ssl` used by ucpdav1

## Key decisions

- All DEF files live in `util/`; each exports only
  `component_getImplementationEnvironment` and `component_getFactory`
  (no `component_canUnload` — none of the ucb components implement it).
- The `_ZTIN...` RTTI symbols in the `.dxp` files are GCC-mangled names
  and have no equivalent MSVC export; they are omitted from the DEF files.
- GIO (`ucpgio1`) and GVFS (`ucpgvfs1`) providers are Linux/GNOME-only
  and are not built on Windows.
- `source/inc/regexpmap.tpt` is a template file `#include`d by
  `regexpmap.hxx`; it is listed in `srcs` of ucb1 so Bazel tracks it.
- ucpdav1 links `tl` (tools) because comphelper logging pulls it in at
  link time even though no tools headers are used directly.

## curl/openssl notes

Both are built from source via the Bazel registry overlay at
`ext_libraries/modules/`.

- `CURL_STATICLIB` must be defined by every consumer of `@curl//:curl`.
  Without it, curl.h uses `__declspec(dllimport)` which produces
  `__imp__curl_*` references that can't be satisfied by a static lib.
- OpenSSL static libs require `crypt32.lib`, `advapi32.lib`, `user32.lib`;
  these are declared in `openssl/3.0.13/overlay/BUILD.bazel` `linkopts`
  and propagate automatically to consumers.
- `ucpext` requires `@boost.legacy` (for `boost/optional.hpp`) and
  `//main/tools:tools_headers` (for `tools/diagnose_ex.h`).
