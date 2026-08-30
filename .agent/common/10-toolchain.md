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

# Toolchain — three MSVC toolchains across two Windows floors

- **Three** custom MSVC toolchains at //build/toolchain, all registered in
  MODULE.bazel.  The name states compiler and arch in a fixed order,
  `aoo_msvc_<compiler>_<arch>` — which is what let a third one fit the shape
  instead of inventing a new one:
  - `aoo_msvc_vs2008_x86_def` — VC9, `VC\bin\{cl,ml,link}.exe`.  The DEFAULT
    build and the regression baseline; must remain green.
  - `aoo_msvc_vs2008_x64_def` — VC9 cross-tools (`x86_amd64` cl/ml64 + SDK v7.0
    `lib\x64`).  The Win64 port; boots to the Start Center.
  - `aoo_msvc_vs2019_x64_def` — the modern toolset + Windows 10 SDK, discovered
    by //build:vs_config_repo.bzl rather than spelled out.  Same config rule,
    same `arch_defines`, same /Z7-embed debug story as the VC9 x64 toolchain;
    only the tool and search paths differ.  Two shape differences follow from
    the SDK layout: INCLUDE is five directories (toolset + ucrt/um/shared/winrt)
    where SDK v7.0 had one, and LIB is three.
- Arch ALONE does not pick a toolchain — `winXP-x64` and `win10-x64` are the same
  os+cpu.  The **target platform** picks it, and `//build/constraints:win_version`
  is the only thing separating those two.  Toolchain resolution auto-picks the
  matching cc_toolchain:
  `//build/platforms:winXP-x86` (→ @platforms//cpu:x86_32, default),
  `winXP-x64` (→ x86_64 + `:winxp`), `win10-x64` (→ x86_64 + `:win10`).
  Target space = {winxp, win10} × {x86, x64}; a new Windows floor is a new
  constraint value, not a new config.
- `.bazelrc` convenience configs: `--config=winXP-x86` (default) /
  `--config=winXP-x64` / `--config=win10-x64`, each pinning `--platforms` + a
  distinct `--platform_suffix` (separate bazel-out trees, no clobber) +
  `--symlink_prefix` (`bazel-winXP-x86-` / `bazel-winXP-x64-` /
  `bazel-win10-x64-`).  A win10 build can never clobber the baseline.
- All three target platforms are also registered as **execution** platforms.  A
  target platform that is not an exec platform builds but cannot run its tests —
  there is no exec platform to resolve a test toolchain against.  ORDER MATTERS
  (register_execution_platforms prepends); win10-x64 is registered last so it
  does not displace winXP-x64 as the exec platform for cfg="exec" tools.
- Per-arch defines are injected **globally by the toolchain** (not per-module),
  through the `arch_defines` attr, and they are properties of the TARGET rather
  than of the compiler — so the two x64 toolchains pass the same set:
  x86 → `_X86_=1`/`INTEL`/`CPPU_ENV=msci`, x64 → `_AMD64_=1`/`X86_64`/`CPPU_ENV=mscx`;
  `WIN32` stays defined on BOTH arches (the x64 compiler auto-defines `_WIN64`).
- `MSC` is injected globally too, hardcoded in windows_cc_toolchain_config.bzl
  rather than made an attribute: it names the compiler, and every toolchain
  sharing that config rule is MSVC.  Upstream defines it for every TU on MSVC
  (`solenv/inc/settings.mk:878`); we had it in only 10 module BUILD files, and
  three modules TESTED it without defining it, silently taking the non-Windows
  branch.  It flips live code tree-wide, so per-module builds cannot check it.
- The CRT shim is a **toolchain** property, via `crt_defines`, because a module
  BUILD cannot know which CRT is in play.  The two halves are NOT symmetric:
  - VC9 gets both `snprintf=_snprintf` and `snwprintf=_snwprintf`.
  - vs2019 gets **only** `snwprintf=_snwprintf`.  The UCRT declares a real
    `snprintf` and refuses to compile with that name taken (C1189), so that
    mapping must not be inherited.  It declares no `snwprintf` at all — the wide
    C99 name never existed in any MSVC CRT — so the sources calling it bare need
    the same mapping on both.
- The modern toolchain compiles at **`/std:c++14`**, passed explicitly in
  `cxx_flags` (not `all_compile_flags`, so C compiles never see a C++ std flag).
  It also sets `/Zc:__cplusplus` and
  `_SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS` — the tree uses `<hash_map>`/
  `<hash_set>` widely through the stlport shims, and a modern MSVC makes their
  inclusion a hard #error otherwise.  c++14 rather than c++17 was originally the
  measurement that kept the conformance tail short (dynamic exception
  specifications are legal at c++14, ill-formed at c++17); those have since been
  removed tree-wide, so the ceiling is worth re-measuring before it is quoted.
- No mspdbsrv wrapper on the modern toolchain: LINK_WRAPPER exists solely because
  the VC9 linker has one mspdbsrv and no /FS (LNK1318 under parallel /DEBUG
  links).  The modern link.exe is the one the VC9 toolchains borrow for exactly
  that reason, so it links its own PDBs in parallel unaided.
- windows_cc_toolchain_config.bzl: default_cpp_std disabled (no /std: flag), remove_unreferenced_code disabled (no /Zc:inline)
- tool_bin_path = VC\bin (not msvc_env_path) sets PATH for actions
- BAZEL_DO_NOT_DETECT_CPP_TOOLCHAIN=1 disables auto-detection

Migration state for the x64 build — what is still to re-exercise, and the four
defects the port turned up — is in
[`../migration/win64.md`](../migration/win64.md).
