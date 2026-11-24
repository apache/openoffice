# Build Environment Investigation — Milestone D.4.3

**Project:** Apache OpenOffice  
**Repository:** apache/openoffice  
**Branch:** ooxml  
**Target:** trunk  
**Milestone:** D.4.3  
**Mode:** Build environment investigation

---

## 1. Environment State

| Property | Value |
|----------|-------|
| Branch | `ooxml` |
| Starting commit | `34a6fe28b4` |
| Compiler | g++ 15.2.0-16ubuntu1 (Ubuntu) |
| Configure | Successfully completed |
| Bootstrap | Successfully completed |
| Build system | AOO gbuild (dmake + build.pl) |
| Build tool | perl-based build.pl |

### Configure Options
```
--disable-epm --disable-gtk --disable-randr --without-junit --disable-cups
--disable-gstreamer --disable-dbus --disable-gconf --disable-gnome-vfs
--disable-online-update --disable-odk --disable-systray --disable-nss-module
--disable-pdfimport --disable-wiki-publisher --disable-report-builder
--disable-beanshell --disable-javascript --disable-coinmp --disable-saxon
--disable-fetch-external
```

---

## 2. Build Environment Status

| Component | Status | Notes |
|-----------|--------|-------|
| `autoconf` | Success | Configure script regenerated |
| `configure` | Success | All options accepted |
| `bootstrap` | Success | All external libraries present |
| `oox` module build | Partial | Source files compile, but linking blocked by missing object list regeneration |
| `writerfilter` module build | Partial | Depends on `liboox.so` which lacks MceAlternateContext |

---

## 3. Build Failure Analysis

### Failure 1: Missing Boost Headers

**Error:** `boost/bind.hpp not found` during `sal/osl/all` build

**Root cause:** The `libboost-dev` package is not installed on the system. The `sal/osl/all` module requires Boost headers which are not present.

**Resolution:** Install `libboost-dev` package (requires root access) or build Boost from the bundled source in `main/boost/`.

### Failure 2: Missing `xsltproc`

**Error:** `xsltproc` not found during package registration

**Root cause:** The `xsltproc` utility is not installed on the system.

**Resolution:** Install `xsltproc` package or provide a wrapper script (as was done in `/tmp/mce-test/xsltproc`).

### Failure 3: Missing UNO Headers

**Error:** Headers like `com/sun/star/xml/sax/XFastAttributeList.hpp` not found

**Root cause:** UNO IDL files exist in `main/offapi/` but are not compiled into C++ headers. The `idlc` tool (IDL compiler) needs to be built first, then used to generate C++ headers from IDL files.

**Resolution:** Build `idlc` module first, then run the IDL compiler to generate headers.

---

## 4. Build File Configuration

### `main/oox/Library_oox.mk`

Added `mcealternatecontext` and `mceprocessor` to `gb_Library_add_exception_objects`:

```makefile
oox/source/core/mcealternatecontext \
oox/source/core/mceprocessor \
```

### `main/writerfilter/Library_writerfilter.mk`

Added `oox/inc` include path to allow inclusion of `oox/core/mcealternatecontext.hxx`.

---

## 5. Build Order Dependency

The correct build order for the modules needed by MCE is:

1. `sal` - Provides basic types (rtl::OUString, etc.)
2. `cppu` - Provides UNO core (Reference template, etc.)
3. `oox` - Contains MceProcessor and MceAlternateContext
4. `writerfilter` - Contains OOXMLExportFilter that uses MceProcessor/MceAlternateContext

Attempting to build `oox` before `sal` and `cppu` will fail because the UNO headers don't exist yet.

---

## 6. Recovery Procedure

To successfully build the MCE implementation:

```bash
# 1. Ensure build environment is configured
cd /home/pedro/projects/aoo/main
source LinuxX86-64Env.Set.sh

# 2. Install missing system dependencies
sudo apt-get install libboost-dev xsltproc

# 3. Bootstrap if not already done
./bootstrap

# 4. Build in dependency order
cd instsetoo_native
perl ../solenv/bin/build.pl --from sal
perl ../solenv/bin/build.pl --from cppu
perl ../solenv/bin/build.pl --from oox
perl ../solenv/bin/build.pl --from writerfilter

# 5. Touch Library_oox.mk if MceExport was added after initial build
touch ../oox/Library_oox.mk
perl ../solenv/bin/build.pl --from oox
perl ../solenv/bin/build.pl --from writerfilter
```

---

## 7. Bridge Compatibility Fixes

Two files were modified for GCC 15/Clang 21 compatibility:

### `main/bridges/source/cpp_uno/gcc3_linux_x86-64/share.hxx`
Removed the conflicting `__cxa_get_globals` declaration that conflicted with GCC 15's `cxxabi.h` declaration.

### `main/bridges/source/cpp_uno/gcc3_linux_x86-64/uno2cpp.cxx`
Updated `__cxa_get_globals()` usage to use `::__cxxabiv1::__cxa_get_globals()` directly and added `#include <cxxabi.h>`.

These changes are necessary for building AOO with GCC 15 and are unrelated to the MCE implementation.
