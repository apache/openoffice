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

# Toolchain — dual-arch (32-bit x86 + 64-bit x64), VS2008/VC9

- **Two** custom MSVC toolchains at //build/toolchain, both registered in MODULE.bazel:
  `cc_toolchain_x86_vs2008_def` (x86, the DEFAULT) and `cc_toolchain_x64_vs2008_def`
  (x64, VC9 cross-tools: `x86_amd64` cl/ml64 + SDK v7.0 `lib\x64`).  x86 stays the
  default build and must remain green; x64 is the Win64 port (boots to the Start Center).
- Arch is chosen by **target platform**, not a stringly-typed flag.  Toolchain
  resolution auto-picks the matching cc_toolchain from the platform:
  `//build/platforms:winXP-x86` (→ @platforms//cpu:x86_32, default) vs `winXP-x64`
  (→ x86_64), each composed with the `//build/constraints:win_version` constraint
  (target space = {winxp, win10} × {x86, x64}; a future win10 target is a new
  constraint value, not a new config).
- `.bazelrc` convenience configs: `--config=winXP-x86` (default) / `--config=winXP-x64`,
  each pinning `--platforms` + a distinct `--platform_suffix` (separate bazel-out
  trees, no clobber) + `--symlink_prefix` (`bazel-winXP-x86-` / `bazel-winXP-x64-`).
- Per-arch defines are injected **globally by the toolchain** (not per-module):
  x86 → `_X86_=1`/`INTEL`/`CPPU_ENV=msci`, x64 → `_AMD64_=1`/`X86_64`/`CPPU_ENV=mscx`;
  `WIN32` stays defined on BOTH arches (the x64 compiler auto-defines `_WIN64`).
- windows_cc_toolchain_config.bzl: default_cpp_std disabled (no /std: flag), remove_unreferenced_code disabled (no /Zc:inline)
- tool_bin_path = VC\bin (not msvc_env_path) sets PATH for actions
- BAZEL_DO_NOT_DETECT_CPP_TOOLCHAIN=1 disables auto-detection

Migration state for the x64 build — what is still to re-exercise, and the four
defects the port turned up — is in
[`../migration/win64.md`](../migration/win64.md).
