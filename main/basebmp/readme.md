# basebmp — Bazel migration notes

## Output
`basebmp.dll` — pixel-format-agnostic in-memory bitmap rendering library.

## Key decisions

### vigra external dependency
vigra 1.6.0 is a header-only image-processing template library. It was already
registered in `ext_libraries/modules/vigra/1.6.0/` but was missing an overlay
with a BUILD.bazel. Added:
- `ext_libraries/modules/vigra/1.6.0/overlay/BUILD.bazel` — `cc_library` exposing
  `include/vigra/**` with `includes = ["include"]`
- `ext_libraries/modules/vigra/1.6.0/overlay/MODULE.bazel` — declares
  `bazel_dep(name = "rules_cc", ...)`
- Updated `ext_libraries/modules/vigra/1.6.0/MODULE.bazel` to declare the
  rules_cc dep (needed for Bzlmod resolution)
- Updated `ext_libraries/modules/vigra/1.6.0/source.json` to add the `overlay`
  key with SHA256 hashes, and removed the `patches` entry (the original patch
  was for autoconf configure scripts, irrelevant for header-only use under MSVC)
- Added `bazel_dep(name = "vigra", version = "1.6.0")` to root `MODULE.bazel`

### stlport dependency
`accessortraits.hxx` uses `std::project2nd` and `intconversion.hxx` uses
`std::identity` — both SGI STL extensions not in MSVC's stdlib. stlport provides
them via its STL implementation.

### No DEF file
Exports are driven by `BASEBMP_DLLPUBLIC` / `SAL_DLLPUBLIC_EXPORT`
(`__declspec(dllexport)`) in the headers. No `.map` file exists in the source tree.

### source/intconversion.hxx
Listed in `srcs` (not `hdrs`) so Bazel tracks it as part of the compilation unit
for `bitmapdevice.cxx` which `#include`s it directly.
