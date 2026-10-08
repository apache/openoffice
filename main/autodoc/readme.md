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

# Notes for autodoc

The API documentation generator: it parses the C++ and UNO-IDL sources and
emits the HTML reference.  `//main/autodoc:autodoc` → `autodoc.exe`.

One `cc_binary`: 263 `.cxx` across `source/{ary,ary_i,cosv,display,parser,
parser_i,tools,udm,exes}`.  Mirrors `Executable_autodoc.mk`.

## What is built

A single self-contained tool.  Despite `prj/build.lst` listing `sal soltools
stlport`, autodoc uses its **own** `cosv` OS-abstraction library, not sal/osl —
no autodoc source includes a `sal/`, `osl/` or `rtl/` header — so it links no
first-party DLL.  The only real dependency is the systemstl shim
(`//main/stlport:stlport`, gbuild's `stl`); `sal`/`soltools` in build.lst are
build-ordering, not link/header needs (`<unistd.h>` is reached only on the UNX
branch; WNT uses `<io.h>`).

Verified 2026-10-08 (winXP-x86): builds (263 TUs) and runs — `autodoc.exe`
with no args prints its usage and exits, so the embedded CRT manifest and
startup are sound.

## Landmines

- **Every compiled source's own directory is on the include path.**  The
  sources bare-include their siblings with angle brackets (`#include
  <c_slots.hxx>` from `source/ary/cpp/c_slots.cxx`), which MSVC resolves only
  via `-I`, not the source dir.  gbuild adds each object's directory
  implicitly; the BUILD lists every `.cxx` directory explicitly in
  `_SOURCE_DIRS`, appended AFTER the eight `inc/` parents.
- **`precomp.h` exists three times** (`source/inc`, `source/cosv/inc`,
  `source/udm/inc`) and is bare-included; `source/inc` must come first on `-I`
  (it does — the parents lead, and no `.cxx` dir carries a `precomp.h`).
- **Five duplicate header basenames** (`parsenv2.hxx`, `tokintpr.hxx`,
  `tokproct.hxx`, `tokrecv.hxx`, plus `precomp.h`) live in more than one
  directory, but are always included QUALIFIED (`<s2_luidl/parsenv2.hxx>`,
  `<tokens/tokproct.hxx>`) or quoted — never bare — so adding every `.cxx` dir
  to `-I` cannot resolve them wrongly.
- **Six `.cxx` are NOT in the build** and are globbed out: `source/cosv/unittest/**`,
  `source/udm/unittest/**`, `source/cosv/service/commandline.cxx` (the built one
  is `comdline.cxx`), `source/display/idl/hfi_linklist.cxx`.
- **VC90-CRT manifest embedded** at RT_MANIFEST id 1 (same as
  uno-skeletonmaker/regcompare): autodoc is both a build tool (odk gendocu runs
  it) and an SDK `bin/` tool (odk pack/copying ships it), so it carries its own
  activation context rather than relying on a staged external `.manifest`.

## Next

Wire `gendocu` (odk `pack/gendocu`): run autodoc over the `udkapi`/`offapi`
IDL to produce `docs/common/ref` (the IDL reference `module-ix.html`), and the
C++ reference.  That unblocks the two odk pieces held back in pack/copying —
`bin/autodoc` is now available, and the IDL reference tree can be generated and
folded into `odkcommon.zip`.
