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

- **`20-build-conventions.md` — `snprintf`/`snwprintf` are no longer per-module.**
  Both are injected by the VC9 toolchains through the `crt_defines` attr
  (`build/toolchain/BUILD.bazel`), and are deliberately **empty** on the modern
  toolchain: the UCRT declares the real `snprintf` and refuses to compile with
  the name taken (`C1189`). Which CRT is in play is a toolchain property, so a
  module BUILD cannot know it. The 130 per-module copies were swept out
  2026-08-11.

## Known gap — the global `MSC` define

**Not yet fixed; it needs a full product build to land.** Upstream defines `MSC`
for every TU on MSVC (`solenv/inc/settings.mk:878`, `CDEFS= … -D$(COM) …`, where
`$(COM)` is `MSC`) — the same line this toolchain already borrows `$(CPUNAME)`
(`INTEL`/`X86_64`) and `CPPU_ENV` from. We inject those two and missed `$(COM)`:
`MSC` is set in only 10 module BUILD files.

26 source files test it, and three modules test it **without** defining it, so
they silently take the non-Windows branch. This affects `winXP-x86` today, not
just the win10 port:

- `tools/source/fsys/dirent.cxx` — **live**. Temp filenames come from
  `clock()`+`getpid()` instead of `GetTickCount()`+`_getpid()`. It is also the
  only reason the win10 port needed `#include <time.h>` there: with `MSC`
  defined, Windows never compiles that line — which is why upstream's
  clang/Apple-Silicon port never hit the blocker either.
- `svl/source/inc/poolio.hxx` — debug-only (`DBG_UTIL && MSC`, the `SFX_TRACE`
  macro).
- `vcl/source/gdi/sallayout.cxx` — dead (inside `#ifdef MULTI_SL_DEBUG`, which
  is commented out).

The fix is to inject `MSC` from all three toolchains — it names the **compiler**,
so neither `arch_defines` nor `crt_defines` — and drop the 10 per-module copies.
Deferred here because it flips a live code branch tree-wide: per-module builds
cannot validate it. Found 2026-08-11 while checking why upstream's clang port
never hit the missing-`<time.h>` blocker.
