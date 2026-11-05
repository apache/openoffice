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

# Toolchain — one MSVC toolchain, Windows 7 SP1 x64

- **One** custom MSVC toolchain at //build/toolchain, registered in
  MODULE.bazel: `aoo_msvc_vs2019_x64_def` — the VS2019 toolset + Windows 10
  SDK, discovered by //build:vs_config_repo.bzl rather than spelled out.  INCLUDE
  is five directories (toolset + ucrt/um/shared/winrt), LIB is three.  The name
  states compiler and arch in a fixed order, `aoo_msvc_<compiler>_<arch>`, so a
  further toolchain fits the shape.
- **The VS2008 (VC9) toolchains are RETIRED (2026-11-01)**: `aoo_msvc_vs2008_x86`
  (the default build and regression baseline until then, `winXP-x86`) and
  `aoo_msvc_vs2008_x64` (the Win64 port, `winXP-x64`), with their platforms,
  configs and the `winxp` constraint value.  32-bit is gone with them.  Two
  things did NOT change: the SOURCE stays C++03-compatible, because AOO 4.1.x
  takes security backports and still builds with VS2008; and the VC90 runtime
  layer (msvcr90/msvcp90 staging, Microsoft.VC90.CRT manifests on soffice.exe,
  tool exes and a few DLLs) was removed 2026-11-05 — nothing imported it.
- **VS2008 / SDK v7.0 are no longer needed on the machine** (2026-11-05):
  vs_config_repo reads neither VS_PATH nor SDK_PATH.  scp2's `cl /EP` uses the
  VS2019 cl.exe, and every RC.Exe genrule (msvcp90 ×2, bridges ×2, desktop,
  crashrep) uses `RC_EXE` from @vs_config, the Win10 SDK rc.exe.  Both were
  checked byte-identical before switching: 94/94 `.par`, every `.res`.  The VC90
  debug CRT (@msvc_debug_crt, Microsoft.VC90.DebugCRT manifest) is gone; a /MDd
  build imports ucrtbased/vcruntime140d, which a VS2019 machine has in System32.
  Sole leftover: winaccessibility's MIDL genrule calls SDK v7.0 midl.exe, and
  only builds with `--//build:atl_available=True`.
- The **target platform** picks the toolchain: `//build/platforms:win7-x64`
  (→ x86_64 + `//build/constraints:win7`), the default via `--config=win7-x64`
  in .bazelrc.  `//build/constraints:win_version` stays a dimension of its own,
  so a new Windows floor is a new constraint value, not a new config.
- The modern floor is **Windows 7 SP1**, renamed from `win10` 2026-10-31 because
  nothing in the build required 10: the toolchain compiles at
  `_WIN32_WINNT=0x0601` and links subsystem 6.00.  Two rules keep it true:
  - **Do not raise the Windows API defines** (`_WIN32_WINNT`/`WINVER`) while
    AOO 4.1.x is still being patched — the source must stay backport-compatible.
  - The real floor is set by the **VC v14 runtime** shipped beside the binaries.
    Current releases install on Windows 10+ only (Microsoft's "latest supported
    VC redist" page), so Win7 needs a runtime release that still supports Win7
    SP1, and a toolset no newer than that runtime.  `vs_config_repo.bzl` picks
    whatever modern toolset it finds; that is not yet pinned.
- `.bazelrc` config `--config=win7-x64` (applied by default) pins `--platforms`
  + `--platform_suffix` (its own bazel-out tree) + `--symlink_prefix`
  (`bazel-win7-x64-`).  Old `bazel-winXP-*` trees are no longer written.
- win7-x64 is also registered as the **execution** platform.  A target platform
  that is not an exec platform builds but cannot run its tests — there is no
  exec platform to resolve a test toolchain against.  Since the retirement,
  cfg="exec" tools (idlc, regmerge, rsc, …) are built with VS2019; before it,
  winXP-x64 came first and built them with VC9 even in a win7 build.  If a
  second platform is added, ORDER MATTERS again (register_execution_platforms
  prepends; the first entry gets every exec-tool action).
- Per-arch defines are injected **globally by the toolchain** (not per-module),
  through the `arch_defines` attr, and they are properties of the TARGET rather
  than of the compiler: x64 → `_AMD64_=1`/`X86_64`/`CPPU_ENV=mscx`; `WIN32`
  stays defined too (the compiler auto-defines `_WIN64`).  The x86 set was
  `_X86_=1`/`INTEL`/`CPPU_ENV=msci`; `//build:arch_x86` selects in module BUILD
  files are now dead branches, left for a cleanup pass.
- `MSC` is injected globally too, hardcoded in windows_cc_toolchain_config.bzl
  rather than made an attribute: it names the compiler, and every toolchain
  sharing that config rule is MSVC.  Upstream defines it for every TU on MSVC
  (`solenv/inc/settings.mk:878`); we had it in only 10 module BUILD files, and
  three modules TESTED it without defining it, silently taking the non-Windows
  branch.  It flips live code tree-wide, so per-module builds cannot check it.
- The CRT shim is a **toolchain** property, via `crt_defines`, because a module
  BUILD cannot know which CRT is in play.  vs2019 gets **only**
  `snwprintf=_snwprintf`.  The UCRT declares a real `snprintf` and refuses to
  compile with that name taken (C1189).  It declares no `snwprintf` at all — the
  wide C99 name never existed in any MSVC CRT — so the sources calling it bare
  need the mapping.  (The retired VC9 toolchains also mapped
  `snprintf=_snprintf`; their MSVCRT exported only the underscored name.)
- The modern toolchain compiles at **`/std:c++14`**, passed explicitly in
  `cxx_flags` (not `all_compile_flags`, so C compiles never see a C++ std flag).
  It also sets `/Zc:__cplusplus` and
  `_SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS` — the tree uses `<hash_map>`/
  `<hash_set>` widely through the stlport shims, and a modern MSVC makes their
  inclusion a hard #error otherwise.  c++14 rather than c++17 was originally the
  measurement that kept the conformance tail short (dynamic exception
  specifications are legal at c++14, ill-formed at c++17); those have since been
  removed tree-wide, so the ceiling is worth re-measuring before it is quoted.
- No mspdbsrv wrapper: LINK_WRAPPER existed solely because the VC9 linker has
  one mspdbsrv and no /FS (LNK1318 under parallel /DEBUG links).  The modern
  link.exe links its own PDBs in parallel unaided.  LINK_WRAPPER/PDB_LINK were
  removed from vs_config_repo.bzl 2026-11-05.
- windows_cc_toolchain_config.bzl: default_cpp_std disabled (the explicit
  /std:c++14 lives in cxx_flags), remove_unreferenced_code disabled (no /Zc:inline)
- tool_bin_path (the toolset bin dir, not msvc_env_path) sets PATH for actions
- BAZEL_DO_NOT_DETECT_CPP_TOOLCHAIN=1 disables auto-detection

Migration state for the x64 build — the four defects the port turned up — is in
[`../migration/win64.md`](../migration/win64.md).
