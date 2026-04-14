# Notes for zlib (done)

- Module at ext_libraries/modules/zlib/1.3.2/
- Target: `@zlib//:zlib`

## VS2008 compatibility
- gz*.c files excluded from build (gzread, gzwrite, gzclose use `EWOULDBLOCK` not available in VS2008)

## Z_PREFIX
- Built with `Z_PREFIX` define — all exported symbols are prefixed with `z_`
- **All consumers must also define `Z_PREFIX` and `SYSTEM_ZLIB`** in their copts/defines
