# sdext — Bazel Migration

Builds three UNO extension DLLs from the `sdext` module.

## Targets

| Target | Output | Description |
|--------|--------|-------------|
| `//main/sdext:PresentationMinimizer` | `PresentationMinimizer.uno.dll` | Impress Presentation Minimizer extension |
| `//main/sdext:PresenterScreen` | `PresenterScreen.uno.dll` | Impress Presenter Console (dual-screen) |
| `//main/sdext:pdfimport` | `pdfimport.uno.dll` | PDF Import filter extension |

## Notes

### PresentationMinimizer
Simple UNO component. `pppoptimizertoken.cxx` uses `std::hash_map` → requires `stlport`.

### PresenterScreen
Simple UNO component. `PresenterClock.cxx` is **excluded** from the build: it is absent
from the original makefile's `SLOFILES` list (work-in-progress file that was never finished).
It references `PresenterController::GetViewBackgroundColor` and the full definition of
`PresenterCanvasHelper`, neither of which exist in the current headers.

### pdfimport
More complex: contains 6 internal sub-modules (pdfparse, misc, odf, sax, tree, wrapper)
compiled together into one DLL. Key points:
- `BOOST_SPIRIT_USE_OLD_NAMESPACE` required by `pdfparse/*.cxx`
- `PDFI_IMPL_IDENTIFIER=com.sun.star.PDFImport-wnt` used by `wrapper.cxx` via `BOOST_PP_STRINGIZE`
- `wrapper/hash.cxx` is `#include`d by `wrapper.cxx` (gperf table); declared as `textual_hdrs`
- Links against basegfx (for b2dpolygon geometry used in PDF content tree)
- `Z_PREFIX` + `SYSTEM_ZLIB` + `@zlib` for PDF stream decompression

### xpdfimport.exe (skipped)
The GPL-licensed xpdfimport subprocess wrapper (`xpdfwrapper/`) requires the Poppler library,
which is not available in this Bazel setup. Skipped. The pdfimport DLL spawns this at
runtime; absence only disables runtime PDF-to-image conversion, not compilation.
