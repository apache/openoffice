# xmloff — Bazel Migration

## Outputs
- `xo.dll` — core XML import/export library (chart, draw, forms, meta, script, style, table, text, xforms)
- `xof.dll` — OOo↔OASIS XML format transformation filters

## Dependencies
- First-party: basegfx, comphelper, cppu, cppuhelper, i18npool, sal, salhelper, svl, tools, unotools, vos
- External: stlport (boost)
- UDK/API: udkapi, offapi (IDL headers only)

## Notes
- No `.map` files existed; DEF files created at `util/xo.def` and `source/transform/xof.def`
- Both DLLs export standard UNO component entry points only
- `/Zc:wchar_t-` required for `sal_Unicode` compatibility
- `xof.dll` links `xo.implib` — must build `xo` first
