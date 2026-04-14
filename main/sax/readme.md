# Notes for sax (done)

3 DLLs built from this module:

## sax.dll
- Sources: tools/converter, fastattribs, fastserializer, fshelper
- Links: comphelper

## expwrap.dll
- Sources: expatwrap/
- Uses: @expat//:expat_utf16

## fastsax.dll
- Sources: fastparser/ + xml2utf.cxx
- Uses: @expat//:expat_utf8
- Links: sax.dll

All three require `/Zc:wchar_t-` and stlport.
See ext_libraries/modules/expat/readme.md for expat overlay/patch details.
