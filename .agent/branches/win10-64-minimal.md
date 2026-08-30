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

## Corrections owed to `.agent/common/` — LANDED

**These are done.** The branch merged into `bazel-migration` on 2026-08-30, and
the three corrections below were applied to `common/10-toolchain.md` and
`common/20-build-conventions.md` in the same series. They are kept here as the
record of what the shared docs used to say and why they were wrong; the shared
docs are now authoritative, so read those, not this list.

Per the `.agent/` contract this branch could not edit `common/` itself, so while
it was live the shared docs still described the tree as it was before it:

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

## Exception specifications — (a) done, (b) is next

Agreed on the dev list 2026-08-12 as **two separate changes**, deliberately not
combined. **Owed to `../migration/frontier.md`** when this branch merges — it is
tree-wide work, not win10-specific, and it is only recorded here because a topic
branch may not write the shared migration record.

- **(a) `throw(X)` → delete outright.**
- **(b) `throw()` → migrate to `noexcept`, not delete.** MSVC *does* honour this
  one (`__declspec(nothrow)`: elided unwind paths plus a terminate-on-throw
  contract worth keeping on `acquire()`/`release()` and destructors). C++17 keeps
  it as a deprecated spelling of `noexcept`; C++20 removes it, so deleting would
  lose something the compiler actually implements.

### (a) landed 2026-08-13 — green on both configs

72,490 sites across 5,063 files, atomic with the generator as required. Beyond
the bulk deletion:

- `InterfaceType::dumpExceptionSpecification()` (`cpputype.cxx` ~line 2044) now
  emits a specification **only when the list would be empty**, and nothing
  otherwise — so generated `acquire()`/`release()` keep `throw ()` and hand it
  to (b).
- `SAL_THROW_DTOR` and its three uses are deleted; the interim gate from
  `4ee3de1fe6` became redundant exactly as predicted.
- scaddins' `THROWDEF_RTE*` macros and their 341 uses are deleted — pure
  specification macros, so after (a) they expand to nothing.
- The nine destructor comments this branch wrote about
  `SAL_THROW( (RuntimeException) )` are retired, since nothing says that any more.

**The one deferral is now closed.** `main/unodevtools`' skeletonmaker was left
out of (a) because the module was neither built nor migrated here, so the edit
could not be verified. It is migrated on `bazel-migration` now, with a gtest
suite, so the debt was paid: `printExceptionSpecification()` and its four call
sites are gone from `cpptypemaker.cxx`, ~35 hand-written specifications are gone
from `cppcompskeleton.cxx`, and so is the `SAL_THROW((css::uno::Exception))` on
the generated `_create()` — which was the last non-empty `SAL_THROW` in any
tracked file in the tree. Three cases of
`//main/unodevtools:skeletonmaker_test` now pin the contract; reverting the
generators turns exactly those three red and leaves the Java ones green. The
empty `throw ()` on generated `acquire()`/`release()` stays, for (b).

The same record named `main/codemaker/source/bonobowrappermaker` as carrying the
debt too. **That was wrong** — it generates CORBA IDL, every `throw` in it is a
throw statement in its own code, and it emits no specification in any spelling.
It is dead code (in no `BUILD.bazel` and no `build.lst`) and needs nothing.

Found while verifying, not fixed (it is a separate change to generated output):
the generated class's private-destructor comment has no terminating newline, so
`virtual ~<Class>() {}` lands **inside** the `//` comment and every skeleton gets
an implicit public destructor. See `main/unodevtools/readme.md`.

### (b) — migrate the empty specification to `noexcept`

Size, measured 2026-08-13 over tracked C/C++ under `main/`, post-(a). Exact:

| Spelling | Count |
| --- | ---: |
| literal `throw()` | 4,367 |
| `SAL_THROW( () )` | 2,057 |
| `SAL_THROW_EXTERN_C()` | 768 |

Plus the generated headers: cppumaker's `dumpExceptionSpecification` is now the
only thing that emits one, on `acquire()`/`release()` alone.

**The constraint that decides the approach — `noexcept` cannot be spelled
literally.** VS2008 (VC9) is still the default toolchain (`--config=winXP-x86`)
and has no `noexcept`; only the win10 toolchain (`_MSC_VER >= 1900`) does. So (b)
is not a text substitution but a macro: `SAL_THROW( () )` already *is* that macro
in the right position, and the shortest honest form of (b) is to make one
spelling (say `SAL_NOEXCEPT`, or `SAL_THROW( () )` itself) expand to `noexcept`
where available and `throw()` otherwise, then converge the 4,367 literal `throw()`
spellings and cppumaker's emission onto it. Deciding the spelling is the design
question; the sweep after it is mechanical. Note `SAL_THROW_EXTERN_C()` is a
**third** case, not a variant of the other two: it is for `extern "C"` functions
and already expands to nothing when compiled as C, so whatever (b) picks must
keep that arm.

**Safe, and (a) proved it:** exception specifications do not participate in name
mangling ⇒ not an ABI break, and third-party UNO components keep compiling.

### How to do a sweep this size safely — the method (a) validated

Write a comment/string-aware classifier, not a regex. Treat `throw (…)` as a
**specification** only when all four hold:

1. the argument is a comma-separated list of type names, counting `typename`,
   template args, and a macro line-continuation *inside* the list;
2. the previous significant token is `)`, `const` or `volatile`, with
   `\`+newline skipped as whitespace so macro bodies anchor correctly;
3. the argument is non-empty — for (b), invert this;
4. the **following** token is one of `{ ; = : , ) #`, otherwise it is a throw
   statement whose operand carries a C-style cast (`throw (UINT) ERROR_…;` in
   `desktop/win32/source/setup/setup_main.cxx` is the single site in the tree
   that needs rule 4).

When editing, do not swallow whitespace across a `//` comment (it comments out
the rest of the line) nor across a macro's `\` (it leaves a stray one mid-line).

**Verify by invariant, not by reading 5,000 diffs:** a deletion-only rewrite must
leave the per-file code-level counts of `{`, `}` and `;` unchanged and drop `(`
and `)` equally. That check caught real bugs and is cheap. Every site the
classifier **rejects** must be inspected by hand — there were 30 in (a), and one
was a genuine false positive.

Verify both `--config=winXP-x86` and `--config=win10-x64` with a full
`//main/staging:install`: like (a), (b) touches generated headers, i.e. every TU.
