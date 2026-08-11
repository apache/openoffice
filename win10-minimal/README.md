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

# win10-64-minimal — the shortest road to a Win10 toolchain build

Branched from `bazel-migration` at `1bdcd7a4de`, the same point as
`win10-64-support`. **Nothing from that branch is cherry-picked here.**

## Goal, and what is deliberately not the goal

**Goal: `--config=win10-x64` compiles the product with modern MSVC and the Windows 10 SDK.**

Not here, on purpose:

- **The DPI/HiDPI work.** It is incomplete on `win10-64-support` — several phases are written and
  reasoned but have never run on a machine — so it is not something to pick up and carry. This
  branch changes no rendering behaviour at all.
- Manifests, theming, the capability port, the SOLID experiment. All Track A. All out of scope.
- Running, installing, shipping. This branch targets **compiling**. The CRT/SxS story is the phase
  after, and it is untouched.

The `winXP-x86` / `winXP-x64` targets remain the regression baseline and must keep building
unchanged, exactly as on the other branch.

## What was measured before writing any code

A modern `cl.exe` was pointed at real product source by replaying existing Bazel command lines
through it with `/Zs` (syntax check, no code generation). No Bazel, no toolchain, no build. The
script is disposable; the numbers are not.

| | |
| --- | --- |
| Compiler | MSVC **14.29.30133** (`cl` 19.29.30159), VS2019 BuildTools |
| SDK | Windows 10 **10.0.19041.0** |
| Flags | `/std:c++14 /Zc:__cplusplus /Zc:wchar_t- /wd4996` |
| Sampled | **216 files** across `sal`, `cppu`, `cppuhelper`, `comphelper`, `tools`, `vcl`, `svl`, `svtools`, `sfx2` |

### Result: 4 files fail, from 2 root causes

Once the two systematic issues below were removed, the sample came out:

| Module | Files | Clean | Failing |
| --- | ---: | ---: | ---: |
| `sal` | 25 | 25 | 0 |
| `cppu` | 19 | 19 | 0 |
| `cppuhelper` | 22 | 21 | 1 |
| `comphelper` | 25 | 25 | 0 |
| `tools` | 25 | 24 | 1 |
| `vcl` | 25 | 25 | 0 |
| `svl` | 25 | 25 | 0 |
| `svtools` | 25 | 24 | 1 |
| `sfx2` | 25 | 24 | 1 |

The four are two causes: `C2694` (a destructor's exception specification is less restrictive than
its base's) in `cppuhelper/source/factory.cxx` and `comphelper/inc/comphelper/propstate.hxx` — the
latter a *header*, so it accounts for both `svtools` and `sfx2` — and `C3861` (`clock` undeclared)
in `tools/source/fsys/dirent.cxx`, which VC9's headers used to drag in transitively.

### What is NOT a problem — every one of these was feared, and none of them bite

This is the more valuable half of the measurement. The `win10-64-support` B0 survey listed these as
the unknowns that could dominate the schedule:

| Feared | Measured |
| --- | --- |
| ~46,000 dynamic exception specifications | **accepted** at `/std:c++14`. They are only ill-formed in C++17, which we are not using |
| boost **1.55** (2013) against VS2019 | **compiles**. `shared_ptr`, `bind`, `scoped_ptr`, `noncopyable` and friends all fine |
| `std::tr1` removed from the modern STL | **still present** — `_HAS_TR1_NAMESPACE` defaults to `!_HAS_CXX17` |
| the Windows API floor at `0x0500` (Win2000) | **accepted** by the Win10 SDK. A0 is *not* a prerequisite |
| `/Zc:wchar_t-` on a modern toolchain | **works**, as B2 predicted |
| a large C++ conformance sweep (B3) | **2 root causes in 216 files** |

So the phase the charter called "the real work" is, on this evidence, a short tail. That could still
change as coverage widens — 216 files is a sample, not the tree — but it is a very different
starting position from the one the survey assumed.

## The blockers, and what each costs

In the order they are hit:

| # | Blocker | Fix | Size |
| --- | --- | --- | --- |
| 1 | `/Dsnprintf=_snprintf` collides with the UCRT's real `snprintf` (`C1189`) | do not pass the define on `win10` | one flag |
| 2 | `sal/inc/systools/win32/snprintf.h` redeclares `snprintf` with different linkage (`C2375`) | guard the declarations on `_MSC_VER < 1900` | **done**, 1 header |
| 3 | six of the nine `stlport` shims use `#include_next`, which MSVC lacks (`C1021`/`C1083`) | the include-path split from ADR-009 on `win10-64-support` | 1 BUILD file |
| 4 | `/permissive-` rejects string-literal-to-non-const-pointer (`C2440`) | do not pass `/permissive-` | one flag |
| 5 | destructor exception specifications (`C2694`) | add the base's specification to the derived destructor | small family |
| 6 | `clock` undeclared (`C3861`) | `#include <time.h>` | 1 file |

Nothing on that list is architectural. Items 1, 3 and 4 are build configuration; 2, 5 and 6 are
small, mechanical source changes that are inert on VC9 and therefore backportable.

## Plan

| Phase | Deliverable |
| --- | --- |
| **M0** | *(done)* Measure. This document. |
| **M1** | *(done — see below)* Toolchain discovery: find modern `cl.exe`/`lib.exe`/`link.exe` under `VS_MODERN_PATH` and the newest Windows 10 SDK, and expose the paths. |
| **M2** | A third `cc_toolchain` on those paths, the `win10-x64` platform uncommented, `--config=win10-x64` in `.bazelrc` with its own `--platform_suffix`/`--symlink_prefix` so its output tree never touches the `winXP-*` ones. |
| **M3** | Build **`//main/sal`** and climb: `sal` → `salhelper` → `store` → `registry` → `cppu` → `cppuhelper`. Bottom of the stack first, same order the Win64 work used. Fix blockers 1–6 as they are actually hit rather than preemptively. |
| **M4** | Widen to the rest of the tree, module by module. This is where the real conformance number is learned; the 216-file sample only says where to expect trouble. |

**Definition of done for every phase**: `--config=winXP-x86` unchanged. Same rule as the other
branch, same reason.

## M1 — what discovery exposes

All of it lives in `build/vs_config_repo.bzl`, which already generated the VC9 constants; M1 adds
two more installs to the same generated `@vs_config//:paths.bzl`.

| Constant | Contents |
| --- | --- |
| `MSVC_TOOLSET_VER` / `MSVC_TOOLSET` / `MSVC_HOST` | the selected toolset, e.g. `14.29.30133`, and which `Host*` tree it is driven from |
| `MSVC_BIN` / `MSVC_LIB` | dicts keyed `"x86"`/`"x64"` — `cl`/`link`/`lib`/`ml64` live in the first, the vcruntime import libs in the second |
| `MSVC_INCLUDE` | the toolset's own C++ standard library headers |
| `SDK10_VER` / `SDK10_ROOT` | the selected Windows 10 SDK |
| `SDK10_INCLUDE` | a **list**, already in the order it belongs on `INCLUDE`: `ucrt`, `um`, `shared`, `winrt` |
| `SDK10_LIB` | dict keyed by arch, each a list: `ucrt/<arch>`, `um/<arch>` |
| `SDK10_BIN` | dict keyed by arch — `rc.exe`, `mt.exe` |

Three things are worth knowing before M2 consumes them.

**The SDK layout is genuinely different, not just renamed.** SDK v7.0 has one `include\` and one
`lib\`; the Windows 10 SDK has *four* include trees and *two* lib trees per version, so the single
`msvc_env_include` / `msvc_env_lib` strings the VC9 toolchains build by concatenation become a join
over a list. That is why these are exposed as lists and dicts rather than pre-joined strings — the
toolchain, not the discovery, decides the separator and the order it needs.

**Version selection is "newest *complete*", numerically.** A plain string compare sorts `14.9` above
`14.29`, so versions are compared component-by-component as integers. Completeness matters more than
it sounds: on the reference machine `Windows Kits\10\bin` holds **six** versions while `Include` and
`Lib` hold **two**, so selecting on `bin\` alone would pick a version with no headers at all. A
toolset must have `cl.exe` plus its own headers; an SDK version must have both an `Include\<ver>` and
a `Lib\<ver>`. Non-version siblings (`bin\x64`, `bin\arm64`) are filtered by the same numeric parse
rather than by hardcoded names. Either can be pinned — `MSVC_TOOLSET_VERSION`, `WIN10_SDK_VERSION`.

**This branch pins the SDK to `10.0.19041.0` rather than taking the newest.** The reference machine
also has `10.0.26100.0`, but 19041 is what the M0 measurement above was taken against. Staying on it
keeps any M3 compile failure attributable to the tree instead of to an SDK the 216-file sample never
saw. The pin is a `--repo_env` line in `user.bazelrc`; dropping it moves to the newest.

Not found is reported as the sentinel strings `MODERN_MSVC_TOOLSET_NOT_FOUND` /
`WIN10_SDK_NOT_FOUND`, not `""` — an empty string silently degrades into a valid-looking relative
path, while a sentinel puts its own name into the first error message.

**Verified**, with a clean `INCLUDE`/`LIB` so nothing could leak in from an ambient `vcvars`: the
discovered `cl.exe` (x64 *and* the x86 cross), `lib.exe`, `link.exe` and the SDK's `rc.exe` compile,
archive, link and run a program using `<windows.h>`, `<string>` and `<time.h>` under the same
`/std:c++14 /Zc:__cplusplus /Zc:wchar_t-` the M0 measurement used. `PDB_LINK`/`PDB_LINK_X64` — which
the two VC9 toolchains use for their `generate_pdb` link path, and which now come from the shared
toolset walk instead of a second copy of it — are byte-identical to before.

## How this branch differs from `win10-64-support`

They are not competing. `win10-64-support` is the long road — the SOLID experiment, the capability
port, HiDPI, theming — and it carries the design record: `win10-support/` there holds ADRs 001-009
and the B0 portability survey, which is where the reasoning behind blocker 3 lives.

This branch borrows two things from it and nothing else: **ADR-009**, for the shim split, and the
**B0 survey's method**, for how to measure before deciding. Everything else there is behaviour
work that this branch has no opinion about.
