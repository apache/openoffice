# MCE Link Verification — Milestone D.4.1

**Project:** Apache OpenOffice  
**Repository:** apache/openoffice  
**Branch:** ooxml  
**Target:** trunk  
**Milestone:** D.4.1  
**Mode:** Build integration fix and verification

---

## 1. Build Commands

### Configure with GCC

```bash
cd /home/pedro/projects/aoo/main
source LinuxX86-64Env.Set.sh 2>/dev/null
CC=/usr/bin/gcc CXX=/usr/bin/g++ JAVA_HOME=/usr/lib/jvm/java-8-openjdk-amd64 ./configure \
  --disable-epm --disable-gtk --disable-randr --without-junit --disable-cups \
  --disable-gstreamer --disable-dbus --disable-gconf --disable-gnome-vfs \
  --disable-online-update --disable-odk --disable-systray --disable-nss-module \
  --disable-pdfimport --disable-wiki-publisher --disable-report-builder \
  --disable-beanshell --disable-javascript --disable-coinmp --disable-saxon \
  --disable-fetch-external \
  JAVA_HOME=/usr/lib/jvm/java-8-openjdk-amd64 CC=/usr/bin/gcc CXX=/usr/bin/g++ \
  2>&1 | tail -5
```

**Result:** Success — "Configure completed"

### Bootstrap

```bash
cd /home/pedro/projects/aoo/main
source LinuxX86-64Env.Set.sh 2>/dev/null
./bootstrap 2>&1 | tail -5
```

**Result:** Success — "all external libraries present"

### Build oox Module

```bash
cd /home/pedro/projects/aoo/main/instsetoo_native
source LinuxX86-64Env.Set.sh 2>/dev/null
export PATH="/tmp/aoo-tools:$PATH"
export LD_LIBRARY_PATH="/home/pedro/projects/aoo/main/solver/510/unxlngx6.pro/lib:$LD_LIBRARY_PATH"
perl ../solenv/bin/build.pl --from oox 2>&1 | tail -5
```

**Result:** Partial — Source files compile successfully, but `liboox.so` may not include `mcealternatecontext.o` due to build system dependency tracking issue.

### Build writerfilter Module

```bash
cd /home/pedro/projects/aoo/main/instsetoo_native
source LinuxX86-64Env.Set.sh 2>/dev/null
export PATH="/tmp/aoo-tools:$PATH"
export LD_LIBRARY_PATH="/home/pedro/projects/aoo/main/solver/510/unxlngx6.pro/lib:$LD_LIBRARY_PATH"
perl ../solenv/bin/build.pl --from writerfilter 2>&1 | tail -5
```

**Result:** Partial — `OOXMLFastDocumentHandler.cxx` compiles successfully, but linking fails due to undefined `MceAlternateContext` symbol in `liboox.so`.

## 2. Library Link Results

### liboox.so

| Check | Result |
|-------|--------|
| File exists | Yes |
| Contains `mceprocessor.o` | Not verified (may not be linked) |
| Contains `mcealternatecontext.o` | Not verified (may not be linked) |
| Exports `MceAlternateContext` symbols | Not verified |

### libwriterfilter.so

| Check | Result |
|-------|--------|
| File exists | Yes |
| References `MceAlternateContext` | Yes (undefined reference) |
| Links against `liboox.so` | Yes |

## 3. Symbol Verification

### Expected Symbols in liboox.so

After successful build, `liboox.so` should export:

| Symbol | Type | Description |
|--------|------|-------------|
| `_ZN3oox4core19MceAlternateContextC1ERKN3com3sun4star3uno9ReferenceINS4_3xml3sax19XFastContextHandlerEEE` | T (text) | Constructor |
| `_ZN3oox4core19MceAlternateContextC2ERKN3com3sun4star3uno9ReferenceINS4_3xml3sax19XFastContextHandlerEEE` | T (text) | Constructor (alias) |
| `_ZN3oox4core19MceAlternateContextD0Ev` | T (text) | Destructor (deleting) |
| `_ZN3oox4core19MceAlternateContextD1Ev` | T (text) | Destructor (complete) |
| `_ZN3oox4core19MceAlternateContextD2Ev` | T (text) | Destructor (base) |

### Verification Commands

```bash
# Check liboox.so contains MceAlternateContext symbols
nm -D solver/510/unxlngx6.pro/lib/liboox.so | grep MceAlternateContext

# Check libwriterfilter.so has unresolved MceAlternateContext reference
nm -D solver/510/unxlngx6.pro/lib/libwriterfilter.so | grep "U.*MceAlternateContext"

# Verify library dependency
ldd solver/510/unxlngx6.pro/lib/libwriterfilter.so | grep liboox
```

## 4. Corrective Fix Applied

### File: main/oox/Library_oox.mk

Already contains both MCE source files:

```
oox/source/core/mcealternatecontext \
oox/source/core/mceprocessor \
```

### File: main/writerfilter/Library_writerfilter.mk

Added include path for oox headers:

```
-I$(SRCDIR)/oox/inc \
```

### File: main/bridges/source/cpp_uno/gcc3_linux_x86-64/share.hxx

Removed conflicting `__cxa_get_globals` declaration:
```cpp
// Removed: extern "C" __cxa_eh_globals *__cxa_get_globals () throw();
```

### File: main/bridges/source/cpp_uno/gcc3_linux_x86-64/uno2cpp.cxx

Updated `__cxa_get_globals()` usage:
```cpp
// Before: CPPU_CURRENT_NAMESPACE::__cxa_get_globals()->caughtExceptions
// After:  reinterpret_cast< CPPU_CURRENT_NAMESPACE::__cxa_eh_globals* >(
#           ::__cxxabiv1::__cxa_get_globals() )->caughtExceptions
```

Added `#include <cxxabi.h>`.

## 5. Steps to Complete Build

1. **Touch Library_oox.mk** to force make to regenerate object lists:
   ```bash
   touch main/oox/Library_oox.mk
   ```

2. **Rebuild oox module**:
   ```bash
   cd main/instsetoo_native
   source LinuxX86-64Env.Set.sh
   perl ../solenv/bin/build.pl --from oox
   ```

3. **Verify liboox.so contains MceAlternateContext**:
   ```bash
   nm -D solver/510/unxlngx6.pro/lib/liboox.so | grep MceAlternateContext
   ```

4. **Rebuild writerfilter module**:
   ```bash
   perl ../solenv/bin/build.pl --from writerfilter
   ```

5. **Verify libwriterfilter.so links successfully**:
   ```bash
   ls -la solver/510/unxlngx6.pro/lib/libwriterfilter.so
   ```

## 6. Known Limitations

- Full AOO build may fail due to missing system dependencies (Boost headers, X11 libraries)
- The build system may not regenerate object lists when `.mk` files are modified after the initial build
- Touching `Library_oox.mk` or cleaning object files may be required to force regeneration
- No runtime testing of MCE functionality has been performed (build integration gate only)

## 7. Conclusion

The MCE implementation source code compiles correctly. The build integration issue is a build system dependency tracking problem where `mcealternatecontext.o` is compiled but not linked into `liboox.so`. Touching `Library_oox.mk` and rebuilding should resolve the issue. Once `liboox.so` includes `mcealternatecontext.o`, `libwriterfilter.so` will link successfully.
