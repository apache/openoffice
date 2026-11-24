# Dependency Recovery — Milestone D.4.3

**Project:** Apache OpenOffice  
**Repository:** apache/openoffice  
**Branch:** ooxml  
**Target:** trunk  
**Milestone:** D.4.3  
**Mode:** Dependency recovery analysis

---

## 1. Missing Dependencies Summary

| Dependency | Required By | Status | Resolution Path |
|------------|-------------|--------|-----------------|
| `libboost-dev` (boost/bind.hpp) | `sal/osl/all` | Missing | Install `libboost-dev` or build bundled Boost |
| `xsltproc` | Package registration | Missing | Install package or provide wrapper script |
| UNO headers (`com/sun/star/xml/sax/XFastAttributeList.hpp`) | OOXML source files | Missing | Build `idlc` tool and run IDL compiler |
| `libuno_sal.so` | All OO modules | Not built | Build `sal` module |
| `libuno_cppu.so` | All OO modules | Not built | Build `cppu` module |

---

## 2. Boost Headers Issue

### Problem
The `sal/osl/all` module requires `boost/bind.hpp` which is not installed on the system. The `libboost-dev` package is not installed.

### Resolution Options

**Option A: Install system Boost headers**
```bash
apt-get install libboost-dev
```
This provides `boost/bind.hpp` and other Boost headers needed by AOO.

**Option B: Build bundled Boost**
AOO bundles Boost source in `main/boost/`. Building Boost from source would provide the headers without system installation.

**Resolution:** Option A is preferred as it is simpler and faster.

---

## 3. UNO Headers Issue

### Problem
UNO IDL files (e.g., `com/sun/star/xml/sax/XFastAttributeList.idl`) exist in `main/offapi/` but are not compiled into C++ headers. The `idlc` tool (IDL compiler) needs to be built first, then used to generate C++ headers from IDL files.

### Resolution
Build the `idlc` module first, then run the IDL compiler to generate headers:

```bash
# Build idlc module
cd main/instsetoo_native
source LinuxX86-64Env.Set.sh
perl ../solenv/bin/build.pl --from idlc

# Then build modules that depend on UNO headers
perl ../solenv/bin/build.pl --from oox
```

---

## 4. Build Order Dependency

The correct build order for the modules needed by MCE is:

1. `sal` - Provides basic types (rtl::OUString, etc.)
2. `cppu` - Provides UNO core (Reference template, etc.)
3. `oox` - Contains MceProcessor and MceAlternateContext
4. `writerfilter` - Contains OOXMLExportFilter that uses MceProcessor/MceAlternateContext

Attempting to build `oox` before `sal` and `cppu` will fail because the UNO headers don't exist yet.

---

## 5. Recovery Procedure

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

## 6. Bridge Compatibility Fixes

Two files were modified for GCC 15/Clang 21 compatibility:

### `main/bridges/source/cpp_uno/gcc3_linux_x86-64/share.hxx`
Removed the conflicting `__cxa_get_globals` declaration that conflicted with GCC 15's `cxxabi.h` declaration.

### `main/bridges/source/cpp_uno/gcc3_linux_x86-64/uno2cpp.cxx`
Updated `__cxa_get_globals()` usage to use `::__cxxabiv1::__cxa_get_globals()` directly and added `#include <cxxabi.h>`.

These changes are necessary for building AOO with GCC 15 and are unrelated to the MCE implementation.
