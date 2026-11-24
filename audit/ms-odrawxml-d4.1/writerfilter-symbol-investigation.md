# WriterFilter Symbol Investigation — Milestone D.4.1

**Project:** Apache OpenOffice  
**Repository:** apache/openoffice  
**Branch:** ooxml  
**Target:** trunk  
**Milestone:** D.4.1  
**Mode:** Build integration fix and verification

---

## 1. Undefined Reference

```
OOXMLFastDocumentHandler.cxx:(.text+0x23a): undefined reference to
`oox::core::MceAlternateContext::MceAlternateContext(
    com::sun::star::uno::Reference<
        com::sun::star::xml::sax::XFastContextHandler> const&)'
```

Location: `main/writerfilter/source/ooxml/OOXMLFastDocumentHandler.cxx`, line 122

## 2. Symbol Investigation

### Declared Symbol

**File:** `main/oox/inc/oox/core/mcealternatecontext.hxx`
```cpp
MceAlternateContext(
    const ::com::sun::star::uno::Reference<
        ::com::sun::star::xml::sax::XFastContextHandler >& xDelegate );
```

### Defined Symbol

**File:** `main/oox/source/core/mcealternatecontext.cxx`
```cpp
MceAlternateContext::MceAlternateContext(
    const ::com::sun::star::uno::Reference<
        ::com::sun::star::xml::sax::XFastContextHandler >& xDelegate ) :
    ...
```

### Mangled Symbol Name

```
_ZN3oox4core19MceAlternateContextC1ERKN3com3sun4star3uno9ReferenceINS4_3xml3sax19XFastContextHandlerEEE
```

### Symbol in liboox.so

**Status:** NOT FOUND — `mcealternatecontext.o` is not linked into `liboox.so`

**Reason:** See `oox-link-investigation.md` — build system dependency tracking issue prevents `mcealternatecontext.o` from being included in the link.

## 3. Dependency and Link Analysis

### Library Dependency Chain

```
writerfilter → links against → oox
```

### Link Command

The `writerfilter` library links against `liboox.so` via `gb_Library_add_linked_libs(writerfilter, oox ...)` in `main/writerfilter/Library_writerfilter.mk`.

The linker command includes `-loox` which resolves to `liboox.so`. Since `MceAlternateContext::MceAlternateContext()` is not defined in `liboox.so`, the linker reports an undefined reference.

### Symbol Visibility

The `MceAlternateContext` class uses default visibility (no explicit visibility attributes). When linked into `liboox.so`, its symbols should be exported by default. However, since the object file is not included in the library at all, the symbols are completely absent.

## 4. Resolution

### Primary Fix

Rebuild `liboox.so` with `mcealternatecontext.o` included:

```bash
# Touch Library_oox.mk to force make to regenerate object lists
touch main/oox/Library_oox.mk

# Rebuild oox module
cd main/instsetoo_native
source LinuxX86-64Env.Set.sh
perl ../solenv/bin/build.pl --from oox

# Then rebuild writerfilter
perl ../solenv/bin/build.pl --from writerfilter
```

### Alternative Fix: Add mcealternatecontext to writerfilter directly

If rebuilding `oox` doesn't work due to build system caching, add `mcealternatecontext.cxx` directly to `writerfilter`'s build:

**File:** `main/writerfilter/Library_writerfilter.mk`

Add `oox/source/core/mcealternatecontext` to the `gb_Library_add_exception_objects()` call for `writerfilter`.

However, this is not ideal because:
1. It duplicates the object file in two libraries
2. It creates a circular dependency risk (writerfilter links against oox, but oox doesn't link against writerfilter)
3. The cleaner fix is to ensure `liboox.so` includes the object file

## 5. Symbol Verification After Fix

After rebuilding both libraries, verify:

```bash
# Check liboox.so contains the constructor
nm -D solver/510/unxlngx6.pro/lib/liboox.so | grep MceAlternateContext

# Check libwriterfilter.so resolves the symbol
nm -D solver/510/unxlngx6.pro/lib/libwriterfilter.so | grep MceAlternateContext

# Verify no unresolved MCE symbols remain
nm -D solver/510/unxlngx6.pro/lib/libwriterfilter.so | grep "U.*MceAlternateContext"
```

Expected results:
1. `liboox.so`: Contains `T` (text) symbols for `MceAlternateContext` methods
2. `libwriterfilter.so`: Contains `U` (undefined) symbols that are resolved by `liboox.so` at link time
3. No unresolved symbols remain after linking

## 6. Build Environment Fixes

Two additional fixes were required for GCC 15/Clang 21 compatibility:

### Fix 1: `__cxa_get_globals` declaration conflict

**File:** `main/bridges/source/cpp_uno/gcc3_linux_x86-64/share.hxx`

Removed the conflicting declaration `extern "C" __cxa_eh_globals *__cxa_get_globals () throw();` which conflicts with GCC 15's `cxxabi.h` declaration.

**File:** `main/bridges/source/cpp_uno/gcc3_linux_x86-64/uno2cpp.cxx`

Updated line 450 to use `::__cxxabiv1::__cxa_get_globals()` directly:
```cpp
fillUnoException( reinterpret_cast< CPPU_CURRENT_NAMESPACE::__cxa_eh_globals* >(
    ::__cxxabiv1::__cxa_get_globals() )->caughtExceptions, ...);
```

Added `#include <cxxabi.h>` at the top of the file.

### Fix 2: xsltproc wrapper

Created `/tmp/aoo-tools/xsltproc` as a Python wrapper since `xsltproc` is not installed on the system.
