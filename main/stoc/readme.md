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

# Notes for stoc (done)

- javaloader + javavm now BUILT (see below).  jvmfwk/jvmaccess are their own
  modules and were already migrated.

## bootstrap.uno.dll

9 sub-components merged into one DLL:
bootstrap, security, servicemanager, simpleregistry, defaultregistry,
implementationregistration, loader, registry_tdprovider, tdmanager

## stocservices.uno.dll

stocservices + typeconv + uriproc

## unistd.h stub

`unistd.h` included unconditionally in implreg.cxx — satisfied by `main/soltools/winunistd/unistd.h` stub.
Add `/Imain/soltools/winunistd` to copts wherever this pattern appears in other modules.

## DEF exports

Standard UNO unloadable component pattern — same for all future component DLLs:
`component_getImplementationEnvironment`, `component_getFactory`, `component_canUnload`

## javaloader.uno.dll + javavm.uno.dll — enabling `com.sun.star.loader.Java2`

These two are what makes loading a UNO component *written in Java* possible at
all:

- **`javavm.uno.dll`** — `com.sun.star.comp.stoc.JavaVirtualMachine`: locates
  and starts the JVM through `jvmfwk`, hands out a `jvmaccess::VirtualMachine`.
- **`javaloader.uno.dll`** — `com.sun.star.comp.stoc.JavaComponentLoader`: the
  Java2 loader; builds a class loader over a component's jar and calls its
  `RegistrationClassName`.

Until now **neither was built**, while both were registered in `services.rdb`
*mapped to `bootstrap.uno.dll`* as a placeholder. That could never work —
neither implementation is in that DLL — so any Java component would have failed
to instantiate however it was registered. Both are now built, staged, and
registered at their real DLLs.

Upstream gates both behind `SOLAR_JAVA`; here they are unconditional, because
the Bazel build always has a JDK toolchain (`rules_java` is a hard dep).

### Two landmines, both in the C++/JNI seam

**`SOLAR_JAVA` is load-bearing, not a feature switch.**
`jvmaccess/virtualmachine.hxx` `#include`s the real `<jni.h>` *only* under it,
and otherwise declares stubs (`struct JNIEnv;`, `typedef void * jobject;`).
Without it every `JNIEnv->` call is `C2027 "use of undefined type 'JNIEnv'"` —
even in `javaloader.cxx`, which `#include`s `"jni.h"` itself, because the
incomplete stub is already in scope by then. Same landmine as the java_uno JNI
bridge in `//main/bridges`.

**`javavm` needs `/Zc:wchar_t-`; `javaloader` does not.** `javavm.cxx` passes
`sal_Unicode*` straight into JNI calls (`NewString`, `GetStringRegion`). On
Windows `sal_Unicode` *is* `wchar_t` (`sal/types.h`), and with native `wchar_t`
that is a distinct type from `jchar` (`unsigned short`) — so every such call is
`C2664 "types pointed to are unrelated"`. `/Zc:wchar_t-` makes
`wchar_t == unsigned short == jchar`. `javaloader` escapes this because it only
ever hands JNI plain UTF-8 (`NewStringUTF`).

### The runtime chain — now wired

Having the DLLs is necessary but not enough. The rest of the chain, in the order
`javavm.cxx::getJavaVM` walks it, is now in place:

#### 1. Bootstrap variables — `program/fundamental.ini`

`URE_INTERNAL_JAVA_DIR` must be expandable by
`com.sun.star.util.theMacroExpander`, which resolves against the `URE_BOOTSTRAP`
file — i.e. `program/fundamental.ini`, **not** `uno.ini`. `uno.ini` had the
variables and `fundamental.ini` did not, which is the one that counts (same trap
the extension-path macros already hit). Added there, with
`URE_MORE_JAVA_TYPES` and the two `URE_OVERRIDE_JAVA_JFW_*_DATA` paths.
`URE_INTERNAL_JAVA_CLASSPATH` is explicitly allowed to be unexpandable;
`URE_INTERNAL_JAVA_DIR` is not.

#### 2. `program/classes/` — how the jars actually reach the class loader

This was the open question. The answer is that there are **two** channels, and
conflating them is what made `URE_INTERNAL_JAVA_CLASSPATH` look optional:

- **Hardcoded names.** `javavm.cxx` builds a plain `URLClassLoader` over the
  single literal `"$URE_INTERNAL_JAVA_DIR/unoloader.jar"`, loads
  `com.sun.star.lib.unoloader.UnoClassLoader` from it, and instantiates that with
  `base` = the same directory. `UnoClassLoader.createUrls()` then appends four
  more literals — `java_uno.jar`, `juh.jar`, `jurt.jar`, `ridl.jar`. **No
  manifest, no classpath variable and no `UNO-Type-Path` is involved** for these
  five; the filename *is* the lookup key.
- **The classPath argument.** `unoil.jar` is *not* one of the five. It reaches
  the loader only as the `classPath` constructor argument, which is
  `$URE_INTERNAL_JAVA_CLASSPATH` → `URE_MORE_JAVA_TYPES` (upstream:
  `$ORIGIN/classes/unoil.jar $ORIGIN/classes/ScriptFramework.jar` + each
  extension's `UNO_JAVA_CLASSPATH`, from `scp2` `profileitem_ooo.scp`). So an
  empty `URE_INTERNAL_JAVA_CLASSPATH` does not merely lose extensions — it loses
  every `com.sun.star.*` office API type.

`UNO-Type-Path` matters at a different point entirely:
`UnoClassLoader.getClassLoader()` reads it when *javaloader* opens a **component**
jar, to decide which extra type jars to hoist into the root loader. juh's
deliberately empty value suppresses that; an absent one falls back to `"<>"`,
which re-adds the jar itself — already present via `createUrls()`, so a harmless
duplicate. That is why the juh manifest is reproduced only for
`RegistrationClassName` (load-bearing: `juh.jar` is a registered component in
`services.rdb`) and not for the rest.

The three build-side problems are solved by one new rule,
`uno_jar` in `build/rules/java_pipeline.bzl` (singlejar):

| target | produces | note |
| --- | --- | --- |
| `//main/ridljar:unoloader_jar` | `unoloader.jar` | |
| `//main/ridljar:ridl_jar` | `ridl.jar` | **merge** of `:ridl` + `:udkapi_java_jar` |
| `//main/jurt:jurt_jar` | `jurt.jar` | |
| `//main/javaunohelper:juh_runtime_jar` | `juh.jar` | + `RegistrationClassName` |
| `//main/bridges:java_uno_runtime_jar` | `java_uno.jar` | |
| `//main/unoil:unoil_jar` | `unoil.jar` | reaches the loader via the classPath channel |

- **Exact names**: `out =` names the output at the producing target, so the name
  lives next to the code that justifies it instead of in a staging rename table.
- **`ridl.jar` is a MERGE, not a rename.** Upstream's single jar holds the
  javamaker-generated UDK types *and* the 27 hand-written sources; here they are
  two targets and `java_library(exports = [":udkapi_java"])` wires them together
  only at COMPILE time, so `libridl.jar` alone has no `com.sun.star.uno.*`.
  (Verified: the merged jar has 436 classes including `XInterface`, `TypeClass`
  and `UnoRuntime`.)
- **Manifests**: neither `java_library` nor `jar_from_directory` (a bare
  `jar cf`) writes one.

Staged flat into `program/classes/` by `//main/staging:_install_classes`, using a
new `flatten` option on `tree_install` (the six jars come from five packages, so
no single `strip_prefix` applies).

**DIVERGENCE — `Sealed:` is omitted.** Upstream seals these jars, with jurt's
manifest un-sealing `com/sun/star/lib/util/` and `com/sun/star/uno/` because
those packages are split across `jurt.jar` and `ridl.jar`. A per-package manifest
section cannot be expressed through singlejar's `--deploy_manifest_lines`, which
writes the main section only; a blanket `Sealed: true` would therefore be
*stricter* than upstream and throw `SecurityException` on exactly that split.
Sealing only ever restricts and never enables, so omitting it is the safe
direction. Reversible if the jars ever need integrity sealing.

#### 3. `jvmfwk` needs a JRE to find — and had two of its own gaps

- **`jvmfwk.dll` was built without `SOLAR_JAVA`**, which is not a feature switch
  here either: `framework.cxx` compiles `jfw_startVM()` down to a bare
  `return JFW_E_ERROR;` under `#ifndef SOLAR_JAVA`. Every caller would have got a
  generic error no matter how correct the configuration was. Now defined (the
  exported ABI is unchanged — the JNI types appear only behind pointers, so
  consumers that include `jvmfwk/framework.h` without it still link).
- **`sunjavaplugin.dll` was never built.** `jvmfwk` itself contains no JRE
  knowledge; `javavendors.xml` maps every supported vendor to
  `vnd.sun.star.expand:$URE_INTERNAL_LIB_DIR/sunjavaplugin.dll`, which jvmfwk
  `osl_loadModule()`s. It is a private plugin interface (4 exports from
  `sunjavaplugin.map`), *not* a UNO component — no `component_getFactory`, and
  nothing in `services.rdb`. Nothing links it, so it must be listed explicitly in
  staging. See `main/jvmfwk/readme.md`.
- **`javavendors.xml` and `jvmfwk3.ini`** are now staged into `program/`. The ini
  name is not ours to choose: `jvmfwk/source/fwkutil.hxx` hardcodes
  `SAL_CONFIGFILE("/jvmfwk3")` relative to its own library directory, so it stays
  `jvmfwk3.ini` even though the library is built as `jvmfwk.dll` here.

#### Verified end to end

`//main/cppuhelper:cppuhelper_qa_propertysetmixin` is **6/6 GREEN** as of
2026-08-05 — the first Java UNO component this tree has ever loaded. The three
`testJava*` cases start a real JVM through jvmfwk, have javaloader build a class
loader over `qa_propertysetmixin.uno.jar`, instantiate
`test.cppuhelper.propertysetmixin.comp.JavaSupplier`, and round-trip UNO calls
into it. They share their test bodies with the three `testCpp*` cases, so the
same assertions now pass through both a C++ and a Java implementation of the
same interfaces.

Getting there turned up four more gaps beyond the staging above; see
`main/jvmfwk/readme.md` for the JRE-recognition ones and the commit history for
the rest. The one worth repeating here, because it will bite any future
`gtest_test`:

**A literal `%` in a `gtest_test` `env` value was eaten by the launcher.** The
launcher is a `.bat`, where `%` is a metacharacter — and percent-digit is the
silent case, since `cmd` reads `%2` as the script's (empty) second argument and
drops it. A `%20`-escaped file URL is exactly that shape, so
`file:///C:/Program%20Files%20(x86)/…` arrived as
`file:///C:/Program0Files0(x86)/…`. `_expand_tokens` in
`build/rules/gtest_test.bzl` now doubles `%` before substituting its own
`%VAR%` references. Nothing reported an error at any layer; the only symptom was
jvmfwk saying the JRE "could not be recognized".
