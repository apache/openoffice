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

# sc — Apache OpenOffice Calc

Bazel migration of the Calc spreadsheet module.

## Targets

| Target | Output | Description |
|--------|--------|-------------|
| `:sc` | `sc.dll` | Main Calc library — core data model, formula engine, UNO objects, XML filter, Accessibility |
| `:scd` | `scd.dll` | Calc format detector (UNO component) |
| `:scfilt` | `scfilt.dll` | Filter library — Excel (XLS), Lotus, Quattro Pro, DIF, HTML, RTF, StarCalc |
| `:scui` | `scui.dll` | UI dialog factory and option pages |
| `:vbaobj` | `vbaobj.dll` | VBA Excel object model (Workbook, Sheet, Range, Charts, etc.) |

## Key patterns

- **SDI pipeline**: `scslots_sdi` runs `svidl.exe` on `sdi/scslots.sdi` (with `sdi/scalc.sdi` as exports), generating `scslots_sdi_inc/scslots.hxx`. Consumed by `source/ui/app/typemap.cxx`.
- **SC_DLLIMPLEMENTATION**: defined for `sc.dll` and `scd.dll`; flips `SC_DLLPUBLIC` from `dllimport` to `dllexport`.
- **SC_INFO_OSVERSION**: passed as `"WNT"` string literal via `/D` copts (used in `source/core/tool/interpr5.cxx`).
- **scfilt/scui/vbaobj link sc**: these DLLs link against `sc.dll` at build time via `:sc_implib`.
- **oovbaapi**: `vbaobj.dll` needs `//main/oovbaapi:oovbaapi_idl_headers` for `<ooo/vba/...>` generated headers.
- **icuuc**: `sc.dll` uses ICU for string/locale operations in the formula interpreter.
- **stlport + boost**: both used for `hash_map`; included via `_SC_DEPS`.
- **mdds 0.3.1**: header-only library (flat_segment_tree) used by `source/core/data/segmenttree.cxx`; registered as a local bzlmod module under `ext_libraries/modules/mdds/0.3.1/`.
- **per-subdir /I copts**: all source subdirectories (sidebar, view, drawfunc, dbgui, excel, xml, etc.) are listed in `_COPTS` so MSVC resolves same-directory quoted includes in the Bazel sandbox.
