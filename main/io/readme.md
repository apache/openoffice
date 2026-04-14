# Notes for io (done)
- 5 DLLs: streams, acceptor, connector, textinstream, textoutstream
- All use SAL_DLLPUBLIC_EXPORT — no DEF files needed
- LIBXSLT in build.lst is a legacy entry — none of the source files use it
- Requires `/Zc:wchar_t-` so sal_Unicode = unsigned short (not native wchar_t);
  without it Sequence<sal_Unicode> fails to resolve cppu_detail_getUnoType overload
