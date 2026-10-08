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

# Notes for xml2cmp (done)

Migrated 2026-10-08.  Green on all three configs (`winXP-x86`, `winXP-x64`,
`win10-x64`).

## What it builds

| Target | Upstream | What it is |
|---|---|---|
| `//main/xml2cmp:xml2cmp` | `Executable_xml2cmp.mk` | Reads an old-style UNO component description (`<module-description>` XML) and writes its type list (`-types`), a C++ `component_getDescriptionFunc()` (`-func`) or HTML (`-html`). |
| `//main/xml2cmp:srvdepy` | `Executable_srvdepy.mk` | Same parser over a directory of descriptions; answers "which libraries and services does service X need". |
| `:xml2cmp_common` | — | The 12 sources both exes share.  Upstream compiles them into each exe separately; one static library is equivalent. |

A standalone developer tool: no UNO, no sal, only the C++ runtime and stlport.
`source/x2cclass/` is not built upstream either (it would need `tools`), so it
is not built here.

## Not staged, on purpose

`prj/d.lst` is empty and nothing in scp2 ships either exe.  Their one in-build
consumer was dmake itself (`solenv/inc/settings.mk` `COMPnTYPELIST`,
`rules.mk` `-func`), which no migrated module uses, so the Bazel build
consumes neither.

## Landmines

- **Angle-bracket same-directory includes.**  `parse.cxx` includes
  `<parse.hxx>`, `sistr.cxx` includes `<sistr.hxx>`, and so on.  MSVC never
  resolves `<...>` relative to the including file, so each source subdirectory
  (`source/inc`, `source/support`, `source/xcd`) gets its own `/I`.
- **srvdepy is interactive and never sees EOF.**  `dep_main.cxx` loops
  `std::cin >> sInput` until it reads `#`.  With stdin at EOF (or `/dev/null`)
  it spins forever printing `Error: Service "" not found.` — that wrote 1 GB of
  log in two minutes here.  Always feed it a terminator:
  `printf 'ConnectionPool\n#\n' | srvdepy.exe <dir>`.  Upstream behaviour, not
  a migration defect.

## Verified

x86: `-types`, `-func`, `-html` against in-tree descriptions
(`basctl/util/basctl.xml`, `connectivity/source/cpool/dbpool.xml`);
`srvdepy` resolves `ConnectionPool` to library `dbpool`.  x64 (both): `-func`
output identical in shape; PE32+ subsystem 5.02 (winXP) / 6.00 (win10).
