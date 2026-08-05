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

### NOT yet sufficient to run a Java component

Having the DLLs is necessary but not enough. The remaining chain, in the order
`javavm.cxx::getJavaVM` walks it:

1. **`URE_INTERNAL_JAVA_DIR` must be expandable** by
   `com.sun.star.util.theMacroExpander`, which resolves against the
   `URE_BOOTSTRAP` file — i.e. `program/fundamental.ini`, **not** `uno.ini`.
   `main/staging/uno.ini` already sets `URE_INTERNAL_JAVA_DIR=${ORIGIN}/classes`
   and `URE_INTERNAL_JAVA_CLASSPATH=${URE_MORE_JAVA_CLASSPATH_URLS}`, but
   `fundamental.ini` does not — and that is the one that counts. Same trap the
   extension-path macros already hit; see the long comment in
   `main/staging/fundamental.ini`. `URE_INTERNAL_JAVA_CLASSPATH` is allowed to
   be unexpandable, `URE_INTERNAL_JAVA_DIR` is not.
2. **`program/classes/` staging does not exist.** `getJavaVM` bootstraps from
   `$URE_INTERNAL_JAVA_DIR/unoloader.jar` → `URLClassLoader` →
   `com.sun.star.lib.unoloader.UnoClassLoader`, which then builds the real
   classpath. Every jar target already exists — `//main/ridljar:unoloader`,
   `//main/jurt:jurt`, `//main/javaunohelper:juh_jar`,
   `//main/bridges:java_uno_jar`, `//main/unoil:unoil` — but
   `stage_install.bzl` has no `classes/` destination for them.
3. **`jvmfwk` needs a JRE to find.** `javavendors.xml` plus the `jvmfwk3`
   config decide which JVM is selected; without them `jvmfwk` reports no JRE
   and `javavm` throws. Not yet staged.

So the next Java-bucket step is **staging, not compilation**.
