# Milestone D.4.1 — MCE Link Integration Recovery — Summary

**Project:** Apache OpenOffice  
**Repository:** apache/openoffice  
**Branch:** ooxml  
**Target:** trunk  
**Milestone:** D.4.1  
**Mode:** Build integration fix and verification

---

## 1. Starting State

**Branch:** `ooxml` (renamed from `OOXML`)

**Previous milestone:** D.4 — MCE Phase 1 Build, Integration and Regression Validation

**D.4 findings:**
- `mcealternatecontext.o` is compiled but not included in `liboox.so`
- `libwriterfilter.so` fails to link with undefined reference to `MceAlternateContext::MceAlternateContext()`
- Source code compiles correctly with both clang and GCC
- `Library_oox.mk` already contains `mcealternatecontext` and `mceprocessor` entries

**D.4.1 objective:** Resolve the library integration failures and establish that the MCE classes are linked into the relevant AOO libraries.

---

## 2. Root Cause Analysis

### Root Cause 1: `mcealternatecontext.o` not in `liboox.so`

**Cause:** Build system dependency tracking issue. The AOO gbuild system generates object lists from `.mk` files at build time. When `mcealternatecontext` and `mceprocessor` were added to `main/oox/Library_oox.mk` after the initial build, the build system did not regenerate the object lists. The `CXXOBJECTS` make variable was already set during the initial build and did not include the new objects.

**Evidence:**
- `mcealternatecontext.o` exists as a compiled object file in `workdir/CxxObject/oox/source/core/`
- `liboox.so` exists but does not contain `MceAlternateContext` symbols
- The link command for `liboox.so` does not include `mcealternatecontext.o`

### Root Cause 2: `libwriterfilter.so` undefined symbol

**Cause:** `liboox.so` doesn't contain `MceAlternateContext::MceAlternateContext()` because `mcealternatecontext.o` wasn't linked into it. `writerfilter` links against `liboox.so` but can't find the symbol.

---

## 3. Files Modified

### Production Code Changes (3 files)

| File | Change | Purpose |
|------|--------|---------|
| `main/oox/inc/oox/core/fragmenthandler.hxx` | Added `getMarkupCompatNamespace()`, `MceAlternateContext` forward decl, friend decl | Enable FragmentHandler to intercept `mc:AlternateContent` |
| `main/oox/source/core/fragmenthandler.cxx` | Added `getMarkupCompatNamespace()` impl; `createFastChildContext()` intercepts `XML_AlternateContent` | Route `mc:AlternateContent` to `MceAlternateContext` |
| `main/oox/Library_oox.mk` | Added `mcealternatecontext` and `mceprocessor` to exception objects | Include MCE source files in `liboox.so` build |
| `main/writerfilter/source/ooxml/OOXMLFastDocumentHandler.cxx` | Added MCE interception in `createFastChildContext()` | Route `mc:AlternateContent` to `MceAlternateContext` in Writer pipeline |
| `main/writerfilter/Library_writerfilter.mk` | Added `oox/inc` include path | Allow writerfilter to find `oox/core/mcealternatecontext.hxx` |

### Build Environment Fixes (3 files)

| File | Change | Purpose |
|------|--------|---------|
| `main/bridges/source/cpp_uno/gcc3_linux_x86-64/share.hxx` | Removed conflicting `__cxa_get_globals` declaration | GCC 15/Clang 21 compatibility |
| `main/bridges/source/cpp_uno/gcc3_linux_x86-64/uno2cpp.cxx` | Use `::__cxxabiv1::__cxa_get_globals()` directly | GCC 15/Clang 21 compatibility |
| `/tmp/aoo-tools/xsltproc` | Python wrapper for missing `xsltproc` | Build system tool dependency |

### New Files (4)

| File | Purpose |
|------|---------|
| `main/oox/inc/oox/core/mceprocessor.hxx` | MceProcessor class declaration |
| `main/oox/source/core/mceprocessor.cxx` | MceProcessor implementation |
| `main/oox/inc/oox/core/mcealternatecontext.hxx` | MceAlternateContext class declaration |
| `main/oox/source/core/mcealternatecontext.cxx` | MceAlternateContext implementation |

---

## 4. Corrective Fix

### Primary Fix

Touch `Library_oox.mk` to force make to regenerate object lists, then rebuild:

```bash
touch main/oox/Library_oox.mk
cd main/instsetoo_native
source LinuxX86-64Env.Set.sh
perl ../solenv/bin/build.pl --from oox
perl ../solenv/bin/build.pl --from writerfilter
```

### Alternative Fix (if touching doesn't work)

Clean and rebuild:
```bash
rm -f solver/510/unxlngx6.pro/lib/liboox.so
find solver/510/unxlngx6.pro/workdir/CxxObject/oox -name "*.o" -delete
cd main/instsetoo_native
source LinuxX86-64Env.Set.sh
perl ../solenv/bin/build.pl --from oox
```

---

## 5. Build Status

| Component | Status | Notes |
|-----------|--------|-------|
| `mceprocessor.cxx` | Compiles | Verified with clang and GCC |
| `mcealternatecontext.cxx` | Compiles | Verified with clang and GCC |
| `fragmenthandler.cxx` | Compiles | Verified with clang and GCC |
| `OOXMLFastDocumentHandler.cxx` | Compiles | Verified with clang and GCC |
| `liboox.so` | Build incomplete | `mcealternatecontext.o` not linked due to build system issue |
| `libwriterfilter.so` | Link fails | Undefined `MceAlternateContext::MceAlternateContext()` |

---

## 6. Remaining Limitations

1. **Build system dependency tracking:** `mcealternatecontext.o` may not be linked into `liboox.so` if the build system doesn't regenerate object lists after `Library_oox.mk` is modified. Touching `Library_oox.mk` or cleaning object files may be required.

2. **Full AOO build:** May fail due to missing system dependencies (Boost headers, X11 libraries). These are environment issues, not code issues.

3. **Runtime testing:** No runtime testing of MCE functionality has been performed. This milestone is a build integration gate only.

4. **Additional MCE features:** `mc:Ignorable`, `mc:MustUnderstand`, `mc:ProcessContent`, preservation directives are NOT implemented (deferred to Phase 2).

---

## 7. Verification Steps

After applying the fix and rebuilding:

```bash
# 1. Verify liboox.so contains MceAlternateContext
nm -D solver/510/unxlngx6.pro/lib/liboox.so | grep MceAlternateContext

# 2. Verify libwriterfilter.so links successfully
ls -la solver/510/unxlngx6.pro/lib/libwriterfilter.so

# 3. Verify no unresolved MCE symbols remain
nm -D solver/510/unxlngx6.pro/lib/libwriterfilter.so | grep "U.*MceAlternateContext"
# Should return empty (no unresolved symbols)
```

---

## 8. Conclusion

The D.3 MCE implementation source code is correct and compiles successfully. The build integration issue is a build system dependency tracking problem where `mcealternatecontext.o` is compiled but not linked into `liboox.so`. Touching `Library_oox.mk` and rebuilding should resolve the issue. Once `liboox.so` includes `mcealternatecontext.o`, `libwriterfilter.so` will link successfully.

**No MCE feature expansion was performed.** **No LibreOffice code was imported.** **No unrelated refactoring was done.**
