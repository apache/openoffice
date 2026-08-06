<!--
 Licensed to the Apache Software Foundation (ASF) under one
 or more contributor license agreements.  See the NOTICE file
 distributed with this work for additional information
 regarding copyright ownership.  The ASF licenses this file
 to you under the Apache License, Version 2.0 (the
 "License"); you may not use this file except in compliance
 with the License.  You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing,
 software distributed under the License is distributed on an
 "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 KIND, either express or implied.  See the License for the
 specific language governing permissions and limitations
 under the License.
-->

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
| ---- | ---- | ------------ |
| `inter_libs_exc` | single C++ process | ✅ `//main/bridges:inter_libs_exc_test` — runs & passes |
| `java_uno/any` | in-process JVM host (`staged_java_test`) | ❌ `//main/bridges:test_any_jni` — **red on a real bridge defect, see below** |
| `java_uno/equals`, `…/nativethreadpool` | 2-process socket URP bridge (native `uno` ↔ java) | ⬜ deferred — the fixture now exists, see below |
| `java_uno/acquire` | same + `OOoRunner.jar` | ⬜ `OOoRunner.jar` is built now; needs the 2-process fixture |
| `com/sun/star/lib/uno/bridges/java_remote/*` | pure-Java JUnit-via-OOoRunner | ⬜ `OOoRunner.jar` is built now; needs wiring |
| `performance` | UNO perf harness | ⬜ deferred (needs running UNO env) |
| `testserver`/`testclient`/`testcomp`/`testsameprocess` | legacy CORBA-era (`com.sun.star.corba.giop`) | ⬜ likely obsolete |

- **`inter_libs_exc_test`** — proves a `com.sun.star.lang.IllegalArgumentException`
  thrown in `thrower.dll` is caught across the shared-library boundary in
  `starter.dll` (an MSVC RTTI/ABI check).  `inter.exe` loads both via
  `osl::Module("thrower"/"starter")`, so the cc_binary target names MUST be the
  bare `thrower`/`starter` (the dll basenames it hard-codes).  Run via the generic
  `staged_run_test` (the non-gtest form of `gtest_test`'s staging rule): inter.exe,
  both dlls, sal3/cppu3 and the VC90 CRT are staged into one dir; the test passes
  iff the exe exits 0.  No UNO connection, no Java — the only test here that runs
  without a qadevOOo / running-soffice fixture.
- **`test_any_jni` is RED, and it is the bridge that is wrong — not the test.**
  It fails in `TestSeqSize`, on this line:

  ```
  TestSeqSize: oversized sequence rejected: [jni_uno bridge error] Java calling UNO method mapAny: out of memory!
  TestSeqSize: rejected, but not by the size guard!
  ```

  `seq_allocate()` ([jni_data.cxx:41](source/jni_uno/jni_data.cxx)) computes the
  buffer size as `SAL_SEQUENCE_HEADER_SIZE + (nElements * nSize)` in **32-bit
  signed** arithmetic. The test sends 5,000,000 elements of 1024 bytes:
  5,120,000,000 overflows `INT32_MAX` and wraps to 825,032,704, which a 32-bit
  process cannot allocate, so `rtl_mem::allocate` throws `"out of memory!"`. The
  test accepts a rejection only if it came from a size *guard* (`"out of
  range"`), and that distinction is its whole purpose: today the oversized
  sequence is refused **by luck**. Choose a count whose wrap lands small —
  4,194,304 × 1024 is exactly 2³², wrapping to **0** — and `seq_allocate`
  returns an 8-byte header, after which the loop at
  [jni_data.cxx:1115](source/jni_uno/jni_data.cxx) writes `nElements` elements
  into it (`nPos * nSize` overflowing as well, so some writes land *before* the
  buffer). That is a heap overflow reachable from any in-process Java UNO caller.

  **The fix exists, on another branch**: `7efd38098e` "jni_uno: added guard
  sequence allocation size against integer overflow" (on `security-triage`,
  `security-ASVS-Scan`, `ww8-fixes`) computes the size in `sal_uInt64`, rejects
  negatives, and throws `"sequence size out of range"` above `SAL_MAX_SIZE`.
  `799ee9fa5e` brought the *test* here when it unified the two bridge test sets
  and deliberately left the security-branch source edits behind — its own
  verification note is "both test targets **analyse** clean", i.e. analysis, not
  execution. So this branch holds the regression pin without the fix.

  Left red on purpose: it is an accurate report of a real defect, and
  bazel-migration does not change source (CLAUDE.md). Cherry-picking
  `7efd38098e` (~15 lines, one file) is the fix if this branch is ever to carry
  it.

- **Why the rest are deferred**: the `java_uno/*` tests were *manual* harnesses
  upstream (each `readme.txt`: "run `…-server &`, sleep 3, run `…-client`") — the
  makefile only builds the components + emits run-scripts.  The native `uno`
  runner exists (`//main/cpputools:uno`).  Both former blockers are now GONE:
  `OOoRunner.jar` is built (`//main/qadevOOo:OOoRunner`), and the 2-process
  socket fixture exists — `gtest_test`/`staged_run_test` grew `server_args` +
  `server_ready_port` for `//main/testtools:bridgetest_urp`, which does exactly
  this (start the server detached, wait until it listens, run the client, kill
  stragglers).  What is left is wiring, not missing machinery.  `java_remote`'s
  pure-Java suites are JUnit and can use `//build/rules:junit_test.bzl`.
