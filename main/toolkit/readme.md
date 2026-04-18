# toolkit — Bazel Migration

## Output
- `ootk.dll` — UNO toolkit component (AWT window/control wrappers, layout engine)

## Key decisions

### Dead source file excluded
`source/awt/vclxtabpagemodel.cxx` is present on disk but not listed in `Library_tk.mk`.
It calls `GetWindow()` / `getGraphics()` from `VCLXWindow` even though `VCLXTabPageModel`
inherits from `UnoControlModel`, not `VCLXWindow`. Excluded via glob `exclude = [...]`.

### MSVC boost::bind fix (`inc/pch/boost_bind_msvc_fix.hpp`)
The original build used **MinGW/GCC** which ignores calling convention and exception
specifications in member-function-pointer template type matching.

MSVC 9.0 encodes both into the type:
- `SAL_CALL = __cdecl` is part of the member-function-pointer type
- Boost 1.55 only generates `__cdecl` mf-wrapper overloads when
  `BOOST_MEM_FN_ENABLE_CDECL` is defined; without it, `bind(R(T::*f)(B1),...)` does
  not match a `__cdecl` pointer and the call falls through to the generic path,
  which then fails at `result_traits<unspecified, F>::type` (C2825).

Fix: define `BOOST_MEM_FN_ENABLE_CDECL` before any boost include (via `/FI`
force-include of the shim). MSVC then matches the `__cdecl` overloads, and also
allows implicit conversion from `throw(RuntimeException)` to no-throw-spec pointers
within the same calling convention, covering all UNO listener method binds.

This fix is injected at the toolkit level only; it will be needed by any future
module that uses `boost::bind` with SAL_CALL member function pointers.
