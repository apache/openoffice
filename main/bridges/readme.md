# Notes for bridges (done)

## cpp_uno (C++↔UNO bridge) → `msci_uno.dll`

- DLL name: `msci_uno` (from COMNAME=msci on WNT-INTEL-msci)
- jurt/jvmaccess are Java bridge deps — skipped (not needed for C++ bridge)
- Sources: `source/cpp_uno/shared/*.cxx` + `source/cpp_uno/msvc_win32_intel/*.cxx`
  - shared/: 7 platform-neutral files (bridge, component, vtablefactory, etc.)
  - msvc_win32_intel/: 4 MSVC x86 files (cpp2uno, uno2cpp, except, dllinit)
- Local private headers (component.hxx, guardedarray.hxx, msci.hxx) accessed via copts `/I`
- Exports: 3 C-linkage functions via `util/msci_uno.def`:
  `component_canUnload`, `uno_initEnvironment`, `uno_ext_getMapping`
  — no RTTI symbols (bridge_exports.map symbols are GCC-mangled, don't apply to MSVC)
- Links: cppu3.if.lib + sal.if.lib only (matches SHL1STDLIBS in makefile.mk)
- VS2008 native STL provides `<hash_map>` without stlport (harmless warning only)

## jni_uno (Java↔UNO bridge) → `java_uno.dll` + `java_uno.jar`

The java_uno bridge is what makes Java UNO components talk to the C++ runtime.
Two cooperating artifacts, linked only at runtime (jurt's `NativeThreadPool`
calls `System.loadLibrary("java_uno")`), so there is no build-time cycle.

- **`//main/bridges:java_uno` → `java_uno.dll`** — native JNI bridge.
  Staged into `program/` via `//main/staging` (`collect_outputs`).
  - 6 SLOFILES: `jni_info`, `jni_data`, `jni_uno2java`, `jni_java2uno`,
    `jni_bridge`, `nativethreadpool` (from `makefile.mk` SLOFILES).
  - Links (SHL1STDLIBS): jvmaccess + cppu3 + sal + salhelper implibs.
  - `jni.h` via `@rules_java//toolchains:jni`; `precompiled_bridges_java_uno.hxx`
    resolved by `/Imain/bridges/inc/pch`.
  - **Three build landmines** (none hit by the sibling `msci_uno`, which doesn't
    touch Java/JNI):
    1. **`SOLAR_JAVA` define is mandatory.** `jvmaccess/virtualmachine.hxx` gates
       `#include <jni.h>` behind `#ifdef SOLAR_JAVA`; in the `#else` it emits STUB
       typedefs (`jint=int`, `jobject=void*`). Without the define those stubs clash
       with the real jni.h these sources include → `jint/JNIEnv/JavaVM`
       "redefinition; different basic types". (Also why `jvmaccess`/`javaunohelper`
       set `SOLAR_JAVA`.)
    2. **`/Zc:wchar_t-`** — `jni_info.h`/`jni_bridge.cxx` pass `sal_Unicode*` to JNI
       `jchar*` params (e.g. `GetStringRegion`). `jchar` is `unsigned short`; native
       `wchar_t` makes `sal_Unicode` a distinct type → C2664. The flag makes
       `wchar_t == unsigned short == sal_Unicode == jchar`.
    3. **stlport** — `jni_info.h` uses `::std::hash_map`, which only lives in `std::`
       under stlport (VS2008 native is `stdext::hash_map`).
  - **`salhelper_headers`** must be an explicit dep: `jvmaccess_headers` doesn't
    propagate it even though `unovirtualmachine.hxx` includes
    `salhelper/simplereferenceobject.hxx` (same workaround as `jvmaccess3MSC`).
  - **Exports** (`util/java_uno.def`, converted from `java_uno.map`): the 3 UNO
    C entry points (`uno_initEnvironment`, `uno_ext_getMapping`,
    `component_canUnload`) **plus** the JNI natives. JNICALL is `__stdcall` on
    Win32, so the linker would decorate `Java_*` as `_Java_...@N`, which the JVM
    cannot resolve — the DEF lists the undecorated names (same trick as jurt's
    `jpipe.def`). The `_1` in names like
    `..._JNI_1proxy_dispatch_1call` is JNI's encoding of `_` in the
    `com.sun.star.bridges.jni_uno` package / `JNI_proxy` class.
- **`//main/bridges:java_uno_jar` → `java_uno.jar`** — the Java side:
  `JNI_proxy` + `JNI_info_holder`. Deps: jurt (`AsynchronousFinalizer`,
  `NativeLibraryLoader`) + ridl (`com.sun.star.uno.*`); `--release 8`.
  Sealed manifest is a staging concern, not handled here.

## Tests (`bridges/test/`)

| Test | Kind | Bazel status |
|------|------|--------------|
| `inter_libs_exc` | single C++ process | ✅ `//main/bridges:inter_libs_exc_test` — runs & passes |
| `java_uno/any`, `…/equals`, `…/nativethreadpool` | 2-process socket URP bridge (native `uno` ↔ java) | ⬜ deferred — multi-process orchestration fixture |
| `java_uno/acquire` | same + `OOoRunner.jar` | ⬜ blocked on qadevOOo |
| `com/sun/star/lib/uno/bridges/java_remote/*` | pure-Java JUnit-via-OOoRunner | ⬜ blocked on qadevOOo |
| `performance` | UNO perf harness | ⬜ deferred (needs running UNO env) |
| `testserver`/`testclient`/`testcomp`/`testsameprocess` | legacy CORBA-era (`com.sun.star.corba.giop`) | ⬜ likely obsolete |

- **`inter_libs_exc_test`** — proves a `com.sun.star.lang.IllegalArgumentException`
  thrown in `thrower.dll` is caught across the shared-library boundary in
  `starter.dll` (an MSVC RTTI/ABI check).  `inter.exe` loads both via
  `osl::Module("thrower"/"starter")`, so the cc_binary target names MUST be the
  bare `thrower`/`starter` (the dll basenames it hard-codes).  Run via the generic
  `staged_run_test` (the non-gtest form of `gtest_test`'s staging rule): inter.exe
  + both dlls + sal3/cppu3 + VC90 CRT are staged into one dir; the test passes iff
  the exe exits 0.  No UNO connection, no Java — the only test here that runs
  without a qadevOOo / running-soffice fixture.
- **Why the rest are deferred**: the `java_uno/*` tests were *manual* harnesses
  upstream (each `readme.txt`: "run `…-server &`, sleep 3, run `…-client`") — the
  makefile only builds the components + emits run-scripts.  The native `uno`
  runner exists (`//main/cpputools:uno`), but automating them under `bazel test`
  needs a 2-process socket fixture (port allocation, server-ready wait).
  `acquire` + `java_remote` additionally need `OOoRunner.jar` from **qadevOOo**,
  which is not migrated (frontier).  These belong with the deferred
  OfficeConnection/subsequent-test bucket.
