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

# Draw / Impress

## Bazel Migration

### Deliverables

| Target | Output | Description |
|--------|--------|-------------|
| `//main/sd:sdd` | `sdd.dll` | Format detector (sddetect.cxx) |
| `//main/sd:sd` | `sd.dll` | Main Draw/Impress library (~381 sources) |
| `//main/sd:sdui` | `sdui.dll` | UI dialogs (31 sources) |
| `//main/sd:sdfilt` | `sdfilt.dll` | PowerPoint import/export (ppt/ + eppt/) |

### SDI Pipeline

Two slot headers via `sdi_target`:
- `sdslots_sdi` → `sdslots.hxx` (Impress/Draw slots, `sdslots.sdi`)
- `sdgslots_sdi` → `sdgslots.hxx` (Draw-only slots, `sdgslots.sdi`)

Both use `sdraw.sdi` as the exports file and depend on
`//main/sfx2:sfx2_sdi_pkg` and `//main/svx:svx_sdi_pkg`.

### Notes

- `ins_paste.cxx`, `buttonset.cxx`, `bulmaper.cxx` are compiled into both
  `sd` and `sdui` (matches original gbuild; each cc_binary has its own copy).
- No `.def` file: exports are via `SD_DLLPUBLIC` / `SD_DLLIMPLEMENTATION`
  (`__declspec(dllexport)`) macros defined in `inc/sddllapi.h`.
