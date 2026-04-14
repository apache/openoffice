# Notes for bridges (done)

- DLL name: `msci_uno` (from COMNAME=msci on WNT-INTEL-msci)
- jurt/jvmaccess are Java bridge deps — skipped (not needed for C++ bridge)
- Sources: `source/cpp_uno/shared/*.cxx` + `source/cpp_uno/msvc_win32_intel/*.cxx`
  - shared/: 7 platform-neutral files (bridge, component, vtablefactory, etc.)
  - msvc_win32_intel/: 4 MSVC x86 files (cpp2uno, uno2cpp, except, dllinit)
- Local private headers (component.hxx, guardedarray.hxx, msci.hxx) accessed via copts `/I`
- Exports: 3 C-linkage functions via `util/msci_uno.def`:
  `component_canUnload`, `uno_initEnvironment`, `uno_ext_getMapping`
  — no RTTI symbols (bridge_exports.map symbols are GCC-mangled, don't apply to MSVC)
- Links: cppu3.if.lib + sal.if.lib only (matches SHL1STDLIBS in makefile.mk)
- VS2008 native STL provides `<hash_map>` without stlport (harmless warning only)
