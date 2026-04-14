# Notes for jvmfwk (done)

- Single DLL: `jvmfwk.dll`
- Deps: cppu, cppuhelper, sal, udkapi_idl_headers, stlport, boost.legacy, @libxml2//:libxml2, advapi32
- Exports: 21 C-linkage `jfw_*` functions from `inc/jvmfwk/` via `jvmfwk.def`

## libxml2 overlay fixes for VS2008
- `config.h` overlay: win32/VC10/config.h as base, HAVE_STDINT_H guarded to VS2010+, SEND_ARG2_CAST defined empty
- `libxml2-configure.patch` disables FTP, HTTP, iconv, debug, zlib, lzma in xmlversion.h
- nanoftp.c and nanohttp.c excluded from LIBXML2_SRCS (networking not needed)
- After any overlay/patch hash change: `bazel mod deps --lockfile_mode=refresh`
