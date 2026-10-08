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

# Notes for odk (in progress)

The Office Development Kit (SDK).  Upstream builds it in three layers
(`prj/build.lst`); this migration is taking them one at a time.

| Layer | Upstream | State |
|---|---|---|
| What odk COMPILES | `source/` | **done 2026-10-08**, all three configs |
| The SDK tree and its zips | `pack/copying`, `pack/unzip_udk`, `pack/check*`, `util` | not yet |
| The generated API reference | `pack/gendocu` (autodoc + javadoc) | not yet; blocked on `autodoc` |

## What is built

| Target | Upstream | What it is |
|---|---|---|
| `:loader_classes` | `source/com/sun/star/lib/loader` | `com.sun.star.lib.loader.*` as loose classes, `--release 8`.  An SDK client ships them in its own jar; they find an office and put its UNO jars on a class loader. |
| `:unowinreg` | `source/unowinreg/win` | `unowinreg.dll`, the JNI half of `WinRegKey`. |
| `:unoapploader` | `source/unoapploader/win` | `unoapploader.exe`.  A client copies it beside its own exe; it finds the office, puts it on `PATH` and starts `_<its own name>.exe`. |
| `:uno_loader_classes` | `pack/copying` `MYZIPTARGET` | `uno_loader_classes.zip`: `com/...` + `win/unowinreg.dll`, delivered by `prj/d.lst`. |

## Landmines

- **Both native targets link the static CRT**, as upstream (`DYNAMIC_CRT=`
  empty for unoapploader, `STDSHL=` + `LIBCMT` for unowinreg): they run in
  processes the office does not own (a client's java.exe, a client's launcher),
  before any office or VC runtime has been found.  So they need no manifest, and
  `features = ["static_link_msvcrt"]` instead.
- **`_SNPRINTF_DLLIMPORT=` (empty) on those targets.**  `sal/config.h` includes
  `systools/win32/snprintf.h`, which declares `snprintf` (renamed `_snprintf` by
  the VC9 toolchain's `crt_defines`) as `__declspec(dllimport)` -- right for the
  DLL CRT only.  Against libcmt that is C2375.  The empty macro is the header's
  own switch; no source change.
- **unowinreg exports the undecorated `Java_*` names** through
  `util/unowinreg.def` (= `unowinreg.dxp`).  On x86 the functions are
  `__stdcall`, and link.exe binds each DEF name to its `_Java_...@N` symbol --
  the jurt `jpipe.def` trick.  Verified by actually loading it in a 32-bit JVM.
- **`WinRegKey` loads the DLL out of the zip**: it reads the resource
  `win/unowinreg.dll` from its own class loader and extracts it to a temp file.
  So the zip's layout is the interface, not cosmetic.
- `findsofficepath.c` is cppuhelper's (exported from //main/cppuhelper), compiled
  a second time here -- upstream delivers its `.obj` alone for exactly this.
- `javac_classes` now roots `-d` at the class-path root, so packaged classes
  land at their declared `com/sun/.../X.class` paths (it used to take the first
  output's directory, which only worked for a default-package class).

## Verified (2026-10-08)

On all three configs, with the matching JVM (32-bit Temurin 8 for x86, the
build JDK 21 for x64):

- `WinRegKey` reads `HKLM\...\Windows NT\CurrentVersion\ProductName` through the
  DLL extracted from `uno_loader_classes.zip`.
- `java -cp uno_loader_classes.zip;client com.sun.star.lib.loader.Loader
  UnoClient` with `UNO_PATH` at the staged `program/`: the client loads
  `com.sun.star.uno.UnoRuntime` from the office's `classes/ridl.jar`.
- `unoapploader.exe` installed as `foo.exe` beside a `_foo.exe` starts it with the
  staged `program/` first on `PATH`.

## Next

`pack/copying` is a file list more than a build: `bin/` (cppumaker, idlc,
javamaker, regcompare, ucpp, uno-skeletonmaker, unoapploader; climaker and
autodoc not yet migrated), `lib/` import libraries under their SDK names
(`isal.lib`, `icppu.lib`, ...), `include/`, `idl/`, `classes/`, `settings/`,
docs and examples, then `odkcommon.zip`/`odkexamples.zip`.
