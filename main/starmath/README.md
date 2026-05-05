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

# Math

## Bazel migration

Produces **sm.dll** (Math formula editor) and **smd.dll** (format detector).

### Targets

| Target | Output | Description |
|---|---|---|
| `//main/starmath:sm` | `sm.dll` | Math formula editor — parser, renderer, XML import/export, UNO document model |
| `//main/starmath:sm_implib` | `sm.lib` | Import library for sm.dll |
| `//main/starmath:smd` | `smd.dll` | Math format detector (ExtendedTypeDetection UNO service) |
| `//main/starmath:smd_implib` | `smd.lib` | Import library for smd.dll |
| `//main/starmath:sm_headers` | headers | Public `inc/` headers |
| `//main/starmath:smslots_headers` | headers | Generated SDI slot headers (`smslots.hxx`) |

### SDI pipeline

`sdi/smslots.sdi` (master) includes `sfx2`, `svx`, and `sdi/smath.sdi` (starmath slot definitions).
`sdi_target` runs `svidl.exe` → `smslots_sdi_inc/smslots.hxx`.

### Key fixes

- `/Imain/starmath/source` required: source-private headers (`mathtype.hxx`,
  `smdetect.hxx`, etc.) live flat in `source/` and are included by name without
  a path prefix — MSVC needs an explicit `/I` to find them from the execroot.
- No `.map` file existed; `util/sm.def` and `util/smd.def` were created with the
  two standard UNO component exports (`component_getImplementationEnvironment`,
  `component_getFactory`).
