# Notes for package (done)

2 DLLs:

## package2.dll
- zip/ODF container support
- Uses zlib (requires `Z_PREFIX` + `SYSTEM_ZLIB` defines — see ext_libraries/modules/zlib/readme.md)
- Private headers: source/package/zippackage/, zipapi/, manifest/ — all need /I copts

## xstor.dll
- Compound document storage
- Private headers: source/xstor/ — needs /I copts

Both require `/Zc:wchar_t-`, stlport, and vos_headers (for HashMaps.hxx via vos/ref.hxx).
