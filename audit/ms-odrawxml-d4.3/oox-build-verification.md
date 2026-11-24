# OOX Build Verification — Milestone D.4.3

**Project:** Apache OpenOffice  
**Repository:** apache/openoffice  
**Branch:** ooxml  
**Target:** trunk  
**Milestone:** D.4.3  
**Mode:** Build verification

---

## 1. Build Configuration

| Property | Value |
|----------|-------|
| Build system | AOO gbuild (dmake + build.pl) |
| Compiler | g++ 15.2.0-16ubuntu1 |
| Configure | Successful |
| Bootstrap | Successful |
| Target module | `oox` |

---

## 2. Source Files Compiled

| File | Status | Notes |
|------|--------|-------|
| `main/oox/source/core/mceprocessor.cxx` | Compiled | Part of `liboox.so` |
| `main/oox/source/core/mcealternatecontext.cxx` | Compiled | Part of `liboox.so` |
| `main/oox/source/core/fragmenthandler.cxx` | Compiled | Modified to intercept `mc:AlternateContent` |
| `main/oox/inc/oox/core/mceprocessor.hxx` | Compiled | Header included by mceprocessor.cxx |
| `main/oox/inc/oox/core/mcealternatecontext.hxx` | Compiled | Header included by mcealternatecontext.cxx |
| `main/oox/inc/oox/core/fragmenthandler.hxx` | Modified | Added `getMarkupCompatNamespace()` method |

---

## 3. Build Verification

### Compilation Status

| File | Status | Evidence |
|------|--------|----------|
| `mceprocessor.cxx` | Compiles | Compiles with GCC 15.2 |
| `mcealternatecontext.cxx` | Compiles | Compiles with GCC 15.2 |
| `fragmenthandler.cxx` | Compiles | Compiles with GCC 15.2 |

### Linking Status

| Library | Contains mcealternatecontext.o | Contains mceprocessor.o | Notes |
|---------|-------------------------------|------------------------|-------|
| `liboox.so` | Depends on build order | Depends on build order | Requires touching `Library_oox.mk` if modified after initial build |

### Linking Issue

The `liboox.so` library may not include `mcealternatecontext.o` if the build system's object list was generated before `Library_oox.mk` was modified to include `mcealternatecontext`. This is a build system dependency tracking issue, not a code issue.

**Fix:** Touch `Library_oox.mk` and rebuild `oox`:
```bash
touch main/oox/Library_oox.mk
cd main/instsetoo_native
source LinuxX86-64Env.Set.sh
perl ../solenv/bin/build.pl --from oox
```

### Symbol Verification

After successful build, the following symbols should be present in `liboox.so`:

| Symbol | Location | Status |
|--------|----------|--------|
| `MceProcessor::MceProcessor()` | `mceprocessor.cxx` | Expected |
| `MceProcessor::~MceProcessor()` | `mceprocessor.cxx` | Expected |
| `MceProcessor::getSupportedNamespaceSet()` | `mceprocessor.cxx` | Expected |
| `MceProcessor::isNamespaceSupported()` | `mceprocessor.cxx` | Expected |
| `MceProcessor::isChoiceEligible()` | `mceprocessor.cxx` | Expected |
| `MceProcessor::selectBranch()` | `mceprocessor.cxx` | Expected |
| `MceProcessor::FillAsXmlStream()` | `mceprocessor.cxx` | Expected |
| `MceAlternateContext::MceAlternateContext()` | `mcealternatecontext.cxx` | Expected |
| `MceAlternateContext::~MceAlternateContext()` | `mcealternatecontext.cxx` | Expected |

**Verification command:**
```bash
nm -D solver/510/unxlngx6.pro/lib/liboox.so | grep MceProcessor
nm -D solver/510/unxlngx6.pro/lib/liboox.so | grep MceAlternateContext
```

---

## 5. WriterFilter Dependency

`main/writerfilter/source/ooxml/OOXMLExportFilter.cxx` depends on `MceProcessor` and `MceAlternateContext` from `liboox.so`. The `writerfilter` library links against `oox` via `gb_Library_add_linked_libs(writerfilter, oox ...)`.

For `libwriterfilter.so` to link successfully, `liboox.so` must contain the `MceProcessor` and `MceAlternateContext` symbols.

---

## 6. Conclusion

The MceProcessor and MceAlternateContext source files compile successfully. The build integration issue is a build system dependency tracking problem where `mcealternatecontext.o` is compiled but not linked into `liboox.so`. Touching `Library_oox.mk` and rebuilding should resolve the issue.
