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

## The global `MSC` define — now injected by the toolchain

Upstream defines `MSC`
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

`MSC` is now injected globally from `default_compile_flags_list` in
`build/toolchain/windows_cc_toolchain_config.bzl`, and the 21 per-module copies
are gone. It is **hardcoded there rather than made an attribute** because it
names the compiler, and every toolchain sharing that config rule is MSVC —
unlike `arch_defines` (per target CPU) or `crt_defines` (per CRT).

It flips a live code branch tree-wide and appears on every compile command line,
so **per-module builds cannot check it**. **Validated 2026-08-11 by a full green
`//main/staging:install --config=winXP-x86`** — the only check that covers it.
Found while checking why upstream's clang port never hit the missing-`<time.h>`
blocker.

`tools/source/fsys/dirent.cxx` keeps its `#include <time.h>`: with `MSC` defined
Windows no longer compiles the line that needed it, but the include is correct
for the non-Windows branch and inert here.

This one is a correction owed to `common/20-build-conventions.md` as well.

## Next up — remove dynamic exception specifications tree-wide

Agreed on the dev list 2026-08-12. Self-contained; everything needed to start is
here. **Owed to `../migration/frontier.md`** when this branch merges — it is
tree-wide work, not win10-specific, and it is only recorded here because a topic
branch may not write the shared migration record.

**Two separate changes. Do not combine them.**

- **(a) `throw(X)` → delete outright.** Removed in C++17; MSVC never enforced it;
  GCC/Sun/SGI never even saw it (`SAL_THROW` expands to nothing there).
- **(b) `throw()` → migrate to `noexcept`, not delete.** MSVC *does* honour this
  one (`__declspec(nothrow)`: elided unwind paths plus a terminate-on-throw
  contract worth keeping on `acquire()`/`release()` and destructors). C++17 keeps
  it as a deprecated spelling of `noexcept`; C++20 removes it. Deleting would
  lose something the compiler actually implements.

Size, measured 2026-08-12 under `main/`:

| Spelling | Count | Goes to |
| --- | ---: | --- |
| `SAL_THROW( () )` | 3,677 | (b) |
| `SAL_THROW( (Type) )` | 827 | (a) |
| literal `throw(…RuntimeException…)` | ~32,600 | (a) |

The last figure is soft — it includes declaration/definition duplication. The
other two are exact.

### The constraint that decides the approach — it cannot be incremental

The specifications are **generated**. `InterfaceType::dumpExceptionSpecification()`
in `main/codemaker/source/cppumaker/cpputype.cxx` (~line 2044) writes `" throw ("`
onto every UNO interface method, so every generated `.hpp` carries them. For a
non-destructor virtual function, an override with no specification may throw
anything, which is **less** restrictive than a base declaring
`throw(RuntimeException)` ⇒ ill-formed. Stripping hand-written overrides while
generated bases keep theirs only trades one error set for another. **cppumaker and
the sources must move in one commit.** ("Remove them a module at a time" is the
natural instinct, and it does not work.)

**Safe:** exception specifications do not participate in name mangling ⇒ not an
ABI break. Backward compatible for extensions too — once bases lose their
specifications, a third-party component still declaring `throw(RuntimeException)`
on its overrides is merely *more* restrictive, which is legal, so external UNO
components keep compiling unchanged.

### Why it is urgent rather than cosmetic

Since C++11 a destructor with no written specification is implicitly `noexcept`.
A class inheriting from a UNO base (`~OWeakObject`, declared
`throw(RuntimeException)`) **and** an ordinary base (implicitly non-throwing)
therefore claims both "may throw" and "will not throw" about destroying the same
object. No annotation resolves it — `noexcept(false)` gives the identical error.
Three such destructors produced **6,690 errors across 25 modules / 242 classes**
on a modern MSVC. All three bodies are `{}`: the specification described a
possibility the implementation never took.

### Already done, and superseded by this task

Commit `4ee3de1fe6` added `SAL_THROW_DTOR()` in `main/sal/inc/sal/types.h` (empty
on `_MSC_VER >= 1900`, defers to `SAL_THROW` otherwise) and applied it to exactly
those three destructors — `~OWeakObject` (`cppuhelper/inc/cppuhelper/weak.hxx` +
`source/weak.cxx`), `~OWeakAggObject` (`weakagg.hxx` + `source/weak.cxx`),
`~OComponentHelper` (`component.hxx` + `source/component.cxx`). That is an
**interim gate** to unblock the win10 toolchain. When (a) lands, `SAL_THROW_DTOR`
and its three uses should be deleted — they become redundant.

**Sequencing:** land (a) first (it is the C++17 blocker and it is mechanical),
then (b) separately. Verify both `--config=winXP-x86` and `--config=win10-x64`
after each; (a) needs a full `//main/staging:install` because it touches
generated headers, i.e. every TU.
