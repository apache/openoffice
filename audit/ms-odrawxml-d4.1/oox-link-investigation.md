# OOX Link Investigation — Milestone D.4.1

**Project:** Apache OpenOffice  
**Repository:** apache/openoffice  
**Branch:** ooxml  
**Target:** trunk  
**Milestone:** D.4.1  
**Mode:** Build integration fix and verification

---

## 1. Effective Target

The `oox` library target is defined in `main/oox/Library_oox.mk`:

```
$(eval $(call gb_Library_Library,oox))
```

This creates a shared library `liboox.so` delivered to:
```
main/solver/510/unxlngx6.pro/lib/liboox.so
```

The library links object files listed via `gb_Library_add_exception_objects()` in `Library_oox.mk`, which forwards to `gb_LinkTarget_add_exception_objects()` in `main/solenv/gbuild/LinkTarget.mk`, which adds them to the `CXXOBJECTS` make variable for the link target.

## 2. Object-List Investigation

### Registered Object Files

The `Library_oox.mk` file contains both MCE source files:

| Line | Entry |
|------|-------|
| 76 | `oox/source/core/mcealternatecontext` |
| 77 | `oox/source/core/mceprocessor` |

### Compiled Object Files

| Object File | Path | Status |
|-------------|------|--------|
| `mceprocessor.o` | `solver/510/unxlngx6.pro/workdir/CxxObject/oox/source/core/mceprocessor.o` | Compiled successfully |
| `mcealternatecontext.o` | `solver/510/unxlngx6.pro/workdir/CxxObject/oox/source/core/mcealternatecontext.o` | Compiled successfully |

### Linked Object Files in liboox.so

| Object File | In liboox.so? | Notes |
|-------------|---------------|-------|
| `mceprocessor.o` | Not found | Not linked despite being registered |
| `mcealternatecontext.o` | Not found | Not linked despite being registered |

## 3. Root Cause: Missing Object in liboox.so

### Build System Behavior

The AOO build system (gbuild) generates object lists from `.mk` files at build time. The flow is:

1. `Library_oox.mk` calls `gb_Library_add_exception_objects(oox, oox/source/core/mcealternatecontext oox/source/core/mceprocessor)`
2. This forwards to `gb_LinkTarget_add_exception_objects()`, which calls `gb_LinkTarget_add_cxxobject()` for each object
3. `gb_LinkTarget_add_cxxobject()` adds the object to `CXXOBJECTS` make variable and creates a dependency on the object file
4. The link command uses `$(CXXOBJECTS)` to list all object files to link

### Problem

When `mcealternatecontext` and `mceprocessor` were added to `Library_oox.mk` AFTER the initial build of `oox`, the build system did not regenerate the object lists. The `CXXOBJECTS` variable was already set during the initial build and did not include the new objects.

### Evidence

1. `mcealternatecontext.o` and `mceprocessor.o` exist as compiled object files in `workdir/CxxObject/oox/source/core/`
2. `liboox.so` exists but does not contain `MceAlternateContext` symbols
3. The link command for `liboox.so` does not include `mcealternatecontext.o` or `mceprocessor.o`

## 4. Fix

Touch `Library_oox.mk` to force make to regenerate dependency files, then rebuild `oox`:

```bash
touch main/oox/Library_oox.mk
cd main/instsetoo_native
source LinuxX86-64Env.Set.sh
perl ../solenv/bin/build.pl --from oox
```

Alternatively, clean and rebuild:
```bash
rm -f solver/510/unxlngx6.pro/lib/liboox.so
find solver/510/unxlngx6.pro/workdir/CxxObject/oox -name "*.o" -delete
cd main/instsetoo_native
source LinuxX86-64Env.Set.sh
perl ../solenv/bin/build.pl --from oox
```

## 5. Additional Build Environment Fixes

Two additional fixes were required for GCC 15/Clang 21 compatibility:

### Fix 1: `__cxa_get_globals` declaration conflict

**File:** `main/bridges/source/cpp_uno/gcc3_linux_x86-64/share.hxx`

The original declaration `extern "C" __cxa_eh_globals *__cxa_get_globals () throw();` conflicts with GCC 15's `cxxabi.h` declaration `__cxa_get_globals() _GLIBCXX_NOTHROW __attribute__ ((__const__))`.

**Fix:** Removed the declaration from `share.hxx` and updated `uno2cpp.cxx` to use `::__cxxabiv1::__cxa_get_globals()` directly:

```cpp
// In share.hxx: removed the declaration
// In uno2cpp.cxx line 450:
fillUnoException( reinterpret_cast< CPPU_CURRENT_NAMESPACE::__cxa_eh_globals* >(
    ::__cxxabiv1::__cxa_get_globals() )->caughtExceptions, ...);
```

### Fix 2: xsltproc wrapper

Created `/tmp/aoo-tools/xsltproc` as a Python wrapper since `xsltproc` is not installed on the system.

## 6. Build Commands

```bash
# Configure with GCC
cd /home/pedro/projects/aoo/main
source LinuxX86-64Env.Set.sh
CC=/usr/bin/gcc CXX=/usr/bin/g++ JAVA_HOME=/usr/lib/jvm/java-8-openjdk-amd64 ./configure \
  --disable-epm --disable-gtk --disable-randr --without-junit --disable-cups \
  --disable-gstreamer --disable-dbus --disable-gconf --disable-gnome-vfs \
  --disable-online-update --disable-odk --disable-systray --disable-nss-module \
  --disable-pdfimport --disable-wiki-publisher --disable-report-builder \
  --disable-beanshell --disable-javascript --disable-coinmp --disable-saxon \
  --disable-fetch-external

# Bootstrap
./bootstrap

# Build oox module
cd ../instsetoo_native
source LinuxX86-64Env.Set.sh
perl ../solenv/bin/build.pl --from oox
```

## 7. Verification

After successful build, verify:

```bash
nm -D solver/510/unxlngx6.pro/lib/liboox.so | grep MceAlternateContext
```

Expected output:
```
0000000000000000 T _ZN3oox4core19MceAlternateContextC1ERKN3com3sun4star3uno9ReferenceINS4_3xml3sax19XFastContextHandlerEEE
0000000000000000 T _ZN3oox4core19MceAlternateContextC2ERKN3com3sun4star3uno9ReferenceINS4_3xml3sax19XFastContextHandlerEEE
```
