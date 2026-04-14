# Notes for comphelper (done)

- Single DLL: `comphelp.dll`
- Deps: sal, cppu, cppuhelper, salhelper, ucbhelper, vos, udkapi, offapi, stlport, boost.legacy
- Requires `/Zc:wchar_t-` (sal_Unicode = unsigned short)
- Requires `snprintf=_snprintf` define (VS2008 MSVCRT only exports `_snprintf`)
- Private source headers: `/Imain/comphelper/source/inc`
- ucbhelper_implib filegroup added to ucbhelper BUILD.bazel to expose the import library
