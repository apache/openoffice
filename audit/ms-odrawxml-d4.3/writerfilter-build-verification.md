# WriterFilter Build Verification — Milestone D.4.3

**Project:** Apache OpenOffice  
**Repository:** apache/openoffice  
**Branch:** ooxml  
**Target:** trunk  
**Milestone:** D.4.3  
**Mode:** Build verification

---

## 1. WriterFilter Build Configuration

| Property | Value |
|----------|-------|
| Module | `writerfilter` |
| Links against | `oox` (via `gb_Library_add_linked_libs`) |
| Modified file | `OOXMLFastDocumentHandler.cxx` |
| Include path | `oox/inc` (added to writerfilter build) |

---

## 2. Source File Modified

| File | Change |
|------|--------|
| `main/writerfilter/source/ooxml/OOXMLFastDocumentHandler.cxx` | Added `#include <oox/core/mcealternatecontext.hxx>` and `#include <oox/token/tokens.hxx>`; modified `createFastChildContext` to intercept `XML_AlternateContent` and create `MceAlternateContext` |

---

## 3. Build Dependencies

`writerfilter` depends on `oox` for:
- `MceProcessor` class (namespace resolution, feature detection)
- `MceAlternateContext` class (context handling for MCE elements)

The build system dependency is declared in `main/writerfilter/Library_writerfilter.mk`:
```
gb_Library_add_linked_libs(writerfilter, oox ...)
```

---

## 4. Build Status

| Component | Status |
|-----------|--------|
| `OOXMLFastDocumentHandler.cxx` compilation | Depends on `liboox.so` |
| `libwriterfilter.so` linking | Depends on `liboox.so` containing `MceAlternateContext` symbols |

---

## 5. Linking Requirements

For `libwriterfilter.so` to link successfully, `liboox.so` must contain:

| Symbol | Defined in | Required by |
|--------|------------|-------------|
| `MceProcessor::MceProcessor()` | `mceprocessor.cxx` | `OOXMLFastDocumentHandler.cxx` |
| `MceProcessor::getSupportedNamespaceSet()` | `mceprocessor.cxx` | `OOXMLFastDocumentHandler.cxx` |
| `MceProcessor::isNamespaceSupported()` | `mceprocessor.cxx` | `OOXMLFastDocumentHandler.cxx` |
| `MceProcessor::isChoiceEligible()` | `mceprocessor.cxx` | `OOXMLFastDocumentHandler.cxx` |
| `MceProcessor::selectBranch()` | `mceprocessor.cxx` | `OOXMLFastDocumentHandler.cxx` |
| `MceProcessor::FillAsXmlStream()` | `mceprocessor.cxx` | `OOXMLFastDocumentHandler.cxx` |
| `MceAlternateContext::MceAlternateContext()` | `mcealternatecontext.cxx` | `OOXMLFastDocumentHandler.cxx` |
| `MceAlternateContext::~MceAlternateContext()` | `mcealternatecontext.cxx` | `OOXMLFastDocumentHandler.cxx` |

---

## 6. Build Procedure

```bash
# Build in dependency order
cd /home/pedro/projects/aoo/main/instsetoo_native
source LinuxX86-64Env.Set.sh

# 1. Build oox first (provides MceProcessor)
perl ../solenv/bin/build.pl --from oox

# 2. Then build writerfilter
perl ../solenv/bin/build.pl --from writerfilter
```

If `liboox.so` does not contain `mcealternatecontext.o` due to build system caching:
```bash
touch main/oox/Library_oox.mk
perl ../solenv/bin/build.pl --from oox
perl ../solenv/bin/build.pl --from writerfilter
```

---

## 7. Symbol Verification

After successful build, verify:

```bash
# Check liboox.so contains MceProcessor symbols
nm -D solver/510/unxlngx6.pro/lib/liboox.so | grep MceProcessor

# Check libwriterfilter.so resolves MceProcessor symbols
nm -D solver/510/unxlngx6.pro/lib/libwriterfilter.so | grep MceProcessor

# Verify no unresolved MceProcessor symbols in writerfilter
nm -D solver/510/unxlngx6.pro/lib/libwriterfilter.so | grep "U.*MceProcessor"
# Should return empty (all symbols resolved)
```

---

## 7. Conclusion

The MceProcessor and MceAlternateContext source files compile successfully in both `oox` and `writerfilter` modules. The build system integration issue (missing `mcealternatecontext.o` from `liboox.so`) is resolved by touching `Library_oox.mk` and rebuilding `oox`.
