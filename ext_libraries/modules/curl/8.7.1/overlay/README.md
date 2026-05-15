# curl 8.7.1 — Bazel build notes

## What this builds

`@curl//:curl` — static library (`libcurl.lib`) with OpenSSL for TLS.
No shared DLL; consumers must define `CURL_STATICLIB`.

## CMake configuration

Built with `rules_foreign_cc` `cmake()`. Key decisions:

- **No zlib** (`CURL_ZLIB=OFF`): AOO's internal zlib uses `Z_PREFIX` (all
  symbols prefixed `z_`); curl expects standard names, so the two are
  incompatible. curl is built without compression support.
- **OpenSSL** (`CURL_USE_OPENSSL=ON`): `rules_foreign_cc` wires `@openssl//:ssl`
  via `CMAKE_PREFIX_PATH` automatically.
- **`CMAKE_DEBUG_POSTFIX=""`**: CMake's MSVC default appends `-d` to library
  names in debug builds, producing `libcurl-d.lib`. `out_static_libs` is
  static, so the suffix is suppressed to keep the output name `libcurl.lib`
  in all configurations.
- **`/Zc:wchar_t-`**: required for ABI compatibility with AOO's `sal_Unicode`.

## Consuming this target

```python
deps = ["@curl//:curl"],
defines = ["CURL_STATICLIB"],
```

`CURL_STATICLIB` must be defined — without it, `curl.h` uses
`__declspec(dllimport)`, which breaks the static link.
