# basic — Bazel Migration Notes

## Output
- `sb.dll` — StarBasic runtime + UNO scripting/dialog library containers

## Source layout
- `source/basmgr/` — BasicManager, BasicManagerRepository, VBA helper
- `source/classes/` — Core Basic runtime (sb, sbxmod, image, disas, errobject, eventatt, …)
- `source/comp/` — Compiler (parser, scanner, codegen, exprtree, symtbl, token, …)
- `source/runtime/` — Interpreter (runtime, step0/1/2, iosys, methods, stdobj, dllmgr, …)
- `source/sbx/` — SBX object system (sbxvalue, sbxobj, sbxcoll, sbxform, sbxscan, …)
- `source/uno/` — UNO wrappers (namecont, scriptcont, dlgcont, sbmodule, sbservices, …)

## Assembly (wnt.asm)
`source/runtime/wnt.asm` defines `_DllMgr_call32@12` and `_DllMgr_callFp@12` — the 32-bit
x86 stdcall trampoline used by `dllmgr.cxx` to dynamically invoke Basic `Declare`-d DLL
functions. Listed directly in `srcs`; the toolchain's `assemble` action (ml.exe) handles it.

Required toolchain fix in `build/toolchain/windows_cc_toolchain_config.bzl`: add a
`masm_flags` feature that implies `/c /coff /Cx` for assembly actions.
- `/c` — assemble only (no implicit link)
- `/coff` — emit COFF object format (not OMF)
- `/Cx` — preserve case in PUBLIC/EXTERN names (without this, MASM uppercases all symbols,
  producing `_DLLMGR_CALL32@12` instead of `_DllMgr_call32@12`, causing LNK2019)

## Exports
All public API is marked with `BASIC_DLLPUBLIC` (`__declspec(dllexport)` when
`BASIC_DLLIMPLEMENTATION` is defined). No `.def` file is needed; the UNO component
entry points (`component_getImplementationEnvironment`, `component_getFactory`) are
emitted by the `IMPLEMENT_COMPONENT_LIBRARY_API` macro in `sbservices.cxx` via
`SAL_DLLPUBLIC_EXPORT`.

## UNO component
`util/sb.component` registers:
- `com.sun.star.comp.sfx2.DialogLibraryContainer`
- `com.sun.star.comp.sfx2.ScriptLibraryContainer`
