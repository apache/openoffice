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

# `win10-64-minimal` — compile the product with a modern MSVC

The shortest road to `--config=win10-x64`: a third toolchain beside the two VC9
ones, and the source conformance a modern compiler demands. **The full charter,
the measurements it was planned from, and the per-milestone record are in
[`../../win10-minimal/README.md`](../../win10-minimal/README.md)** — read that
first; this file only carries what the shared context in `../common/` and
`../migration/` does not yet know.

`winXP-x86` and `winXP-x64` remain the regression baseline and must keep
building unchanged.

## Corrections owed to `.agent/common/`

Per the `.agent/` contract this branch may not edit `common/`, so the shared
docs still describe the tree as it was before this branch. Land these on
`bazel-migration` when the branch merges:

- **`10-toolchain.md` — the VC9 toolchains are renamed.** They are
  `aoo_msvc_vs2008_x86_def` and `aoo_msvc_vs2008_x64_def`, not
  `cc_toolchain_x86_vs2008_def` / `cc_toolchain_x64_vs2008_def`. The name now
  states compiler and arch in a fixed order, which is what makes a third
  toolchain (`aoo_msvc_vs2019_x64_def`) fit the same shape instead of inventing
  one.
