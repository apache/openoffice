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


# bean — Bazel migration notes

The OfficeBean: a `java.awt.Container` that hosts a running office window
inside a Java (AWT/Swing) application.  Migrated 2026-10-02 from
`Ant_officebean.mk` + `Library_officebean.mk` (gbuild).

| Target | Output | Staged to |
|---|---|---|
| `:officebean_jar` | `officebean.jar` (36 classes: `com.sun.star.comp.beans` + the deprecated `com.sun.star.beans`) | `program/classes/` |
| `:officebean` | `officebean.dll` (JNI) | `program/` |

Verified against the dmake `officebean.jar`: identical entry list; manifest
identical except dmake's `Solar-Version:` build stamp, which is not carried.

## Notes

- **Where the DLL goes is decided by jurt, not by convention.**
  `NativeLibraryLoader` looks for the library beside the jar and then one
  directory up, so `program/classes/officebean.jar` finds `program/officebean.dll`.
- **DEF file** (`util/officebean.def`): exports the four `Java_*` names
  undecorated.  `JNICALL` is `__stdcall` on Win32, so without it they would
  leave as `_Java_…@N` — same trick as `//main/jurt` `jpipe.def`.
- **Embedded manifest (RT_MANIFEST id 2) is required.**  The DLL is loaded by
  the *embedding application's* JVM — a stock `java.exe` with no VC90
  activation context — not by a JVM inside `soffice.exe`.  Without it
  `MSVCR90.dll` cannot be resolved and `System.loadLibrary` reports only
  "Can't find dependent libraries".  Same reason as `jpipe`/`jpipx`.
- **jawt** comes from `//build/third_party/jawt`, not a machine JDK's
  `lib/jawt.lib`.  It is an import library cut from a stub compiled against
  the JDK's own `jawt.h`, which reproduces the real import record on both
  arches (x86 `_JAWT_GetAWT@8`, x64 `JAWT_GetAWT`, both verified with
  `dumpbin` against Adoptium 8u452).  A `lib /DEF:` import library is wrong
  on x86: it records name type "no prefix", so the loader would look for
  `JAWT_GetAWT@8`, which `jawt.dll` does not export.  The stub DLL must never be
  staged — beside `officebean.dll` it would win the import resolution and make
  `JAWT_GetAWT` fail — so it is exposed through `import_lib_only`, which the
  staging aspect cannot traverse.
- Not migrated: `test/` (an applet demo) and `qa/complex/bean`, which needs a
  GUI fixture that embeds an office window in a JFrame.
