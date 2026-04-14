# Notes for expat (done)

- Module at ext_libraries/modules/expat/2.5.0/
- Two build variants: `@expat//:expat_utf8` (for fastsax) and `@expat//:expat_utf16` (for expwrap)

## VS2008 patch: aoo-vs2008.patch (patch_strip=2)
- Removes `expat_config.h` include on Windows (that file is Linux-only in the tarball root)
- Adds C89 compat fixes: variable declarations before code
- `isnan=_isnan`, stdbool/stdint workarounds

## winconfig.h
Patched to add: `XML_NS`, `XML_DTD`, `XML_CONTEXT_BYTES`, `BYTEORDER`

## expat_config.h guard
The patch guards `XML_DEV_URANDOM` with `#if !_WIN32` to prevent it being compiled on Windows.
