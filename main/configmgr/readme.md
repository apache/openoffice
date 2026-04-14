# Notes for configmgr (done)

- Single DLL: `configmgr.uno.dll`
- Deps: sal, cppu, cppuhelper, salhelper, comphelper, xmlreader, udkapi, offapi, stlport, boost.legacy
- Requires `/Zc:wchar_t-` and stlport (uses boost::unordered_map, hash_map)
- Exports via SAL_DLLPUBLIC_EXPORT — no DEF file
