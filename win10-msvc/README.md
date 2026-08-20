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

# `win10-msvc-trunk` — building trunk on Windows again

**Status: not built yet.** Every claim below is either a reading of the tree or
a check that could be run without a Windows toolchain. Nothing here has been
compiled. The [Verification](#verification) section says exactly what was
checked and what was not.

## Why trunk does not build on Windows

Not one regression — a scissors motion between two changes that were each
reasonable alone.

* `main/configure.ac` errors out on any compiler that is not VS2008:
  `Compiler too old. Use Microsoft C/C++ .NET 2008.`
* Trunk's bundled dependencies moved past what VS2008 can compile. Boost is
  **1.84** (`main/boost/makefile.mk`), which requires C++11; VC9 has no C++11
  mode at all. Bundled Python is **3.11.15** (`main/python/pyversion.mk`),
  which requires VS2017 or newer to build on Windows.

So the configured compiler cannot build the configured dependencies, and there
is no combination of the two that works. Every other platform moved: Linux and
FreeBSD were raised to `gnu++11` in `7ce5b5df31`, macOS was already at C++11.
Windows is the one that was left pinned.

The fix is to let Windows use a modern MSVC, which is what this branch does.

## Where the work came from

The source half is **cherry-picked from `win10-64-minimal`**, a branch that
already did this against a Bazel build of the same tree and reached a product
that compiles and runs. That is the branch's value here: the conformance
failures a modern MSVC finds in this source have already been found, once, by
building — rather than being guessed at from a survey.

Every source commit carries its `(cherry picked from commit ...)` line.

### What was taken, and what was dropped from each

Four of the picked commits were mixed — Bazel `BUILD.bazel`/`.bzl` changes
alongside source changes — and only the source half applies here. Their
messages still describe the whole commit as it was on the other branch, so the
reconciliation is here rather than rewritten into history:

| commit | taken here | dropped |
| --- | --- | --- |
| `97bc46e96b` (M3) | `cppuhelper/factory.cxx` C2694 fix, `uwinapi/sntprintf.c` guard, the stlport shim split | 7 `BUILD.bazel` files, the toolchain config, `win10-minimal/README.md` |
| `82bbd76e57` (M4) | `comphelper` C2694 + `<iterator>`, `tools/dirent.cxx` `<time.h>`, the stlport `_MSC_VER >= 1900` arms | 88 `BUILD.bazel` files — the Bazel-side `snprintf` sweep, which has no dmake equivalent |
| `e5b57b0a09` | `bridges/.../except.cxx` `<typeinfo>`, `stoc/javavm.cxx` `_tzname` | the bzlmod overlays for libxml2, rasqal and redland — replaced here by `b0fbb14822`, see below |
| `d01dcefb61` | 5,069 of 5,070 files | `bridges/test/cppuno_roundtrip_test.cxx`, which exists only on the Bazel branch |

Three commits from that branch were **not** picked at all:

* the Python 2.7.18 fixes (`b6f8696d08`, `8ee3bd8d0f`) — trunk is on Python
  3.11, so they are moot;
* the NSS `<stdint.h>` overlay (`c06bca0c46`) — that shim was the Bazel
  overlay's own invention. Trunk's `main/nss/nss_win.patch` already guards its
  `<stdint.h>` substitute on `_MSC_VER < 1600` and takes the real header on
  anything newer;
* the Bazel-only commits (toolchain discovery, platform registration, the gtest
  tuple define), which have no counterpart in a dmake tree.

The toolchain half is new. `win10-64-minimal` expressed it as a third Bazel
`cc_toolchain`; trunk needs it in `configure.ac`, `set_soenv.in` and
`solenv/`, which share no code with that.

## What is in the branch

### The source conformance set (from `win10-64-minimal`)

Roughly 64 files, all of it legal C++03 and inert on VC9. Five families:

* **C2694 — a destructor's exception specification.** Under C++03 a
  destructor with nothing written carries no specification, so nothing could
  conflict. From C++11 on the compiler supplies one, and at any class
  inheriting from both a pre-UNO hierarchy and a UNO one, three promises meet
  and only one was written by a person. None of these destructors throws; it
  is a paperwork conflict, and the fix changes no generated code. Fixed at the
  base classes — `SfxBroadcaster`, `SfxListener`, `OWeakObject` and friends —
  so one edit covers every derived class, and with a forward declaration
  rather than a UNO include, so no coupling changes.
* **C3848 — a comparator must be callable on a `const` comparator.**
  `std::set`'s const members hold the comparator by const reference. A
  standard requirement, not a modern-MSVC opinion; VC9's `<xtree>` reached it
  through a non-const path and never checked. Eight sites in `sd`, `sdext`
  and `sw`.
* **C2280 — an output iterator must be CopyAssignable.** `chart2`'s
  `DialogModel.cxx` defines two custom output iterators holding their
  destination by reference, which deletes the implicit copy assignment. This
  is the one item that is a latent defect rather than paperwork: the type was
  never a conforming output iterator. Both now hold a pointer.
* **Named includes.** `<time.h>` in `tools/dirent.cxx`, `<iterator>` in
  `comphelper`, `xmloff` and `xmlhelp` — headers the code already depends on
  and used to get transitively from VC9's `<algorithm>`. Only the ones a build
  actually named were added; a sweep found ~25 candidates and most still
  compile, so adding those would be churn in a diff meant to stay
  backportable.
* **The `snprintf` guards**, in `sal/inc/systools/win32/snprintf.h` and in
  `uwinapi/sntprintf.c` — the declarations *and* the implementations, which
  are two sites of one root cause.

### The exception-specification removal

`d01dcefb61`, 5,069 files. Dynamic exception specifications with a type list
are gone from the tree, together with the two generators that emitted them
(`cppumaker`'s `dumpExceptionSpecification`, and the skeletonmaker in
`unodevtools`). The empty specification — `throw()`, `SAL_THROW( () )`,
`SAL_THROW_EXTERN_C()` — is untouched: MSVC implements that one as
`__declspec(nothrow)` and it is worth keeping.

This is **not** required to compile: at `/std:c++14` the typed specifications
are still legal. It is here because it is what the source branch actually
ships, and because it makes the whole C2694 family evaporate rather than
needing per-site annotation.

`main/solenv/bin/throwspec.py` is the tool that did it, carried along so the
next sweep does not have to re-derive the four rules that separate a
specification from a `throw` statement. Run against this branch it reports no
remaining typed specification, 7,202 empty ones (the follow-up task), 16 real
throw statements, and the eight sites it rejects by design.

### The toolchain

`COMEX` gains the value **14**: the UCRT generation, VS2015 (`cl 19.00`) and
everything since. One value rather than one per Visual Studio release, because
nothing in this tree distinguishes VS2017 from VS2022 — what they share, and
what VC9 does not, is the CRT and the SDK layout. `COMEX` already selects the
platform makefile, the `CVER` define and the output tree, so 14 carries all
three (`CVER=M1900`, output tree `wntmsci14` / `wntmscx14`).

Four shape differences, each handled where it belongs:

| | VC9 | VS2015+ |
| --- | --- | --- |
| `cl.exe` | `VC/bin`, cross tools at `bin/amd64` | `bin/Host<host>/<target>` |
| mspdb | elsewhere — `Common7/IDE`, or the SDK's bin | next to `cl.exe`, so the hunt is skipped |
| headers | one `Include/`, one `Lib/` | four include trees (`ucrt`, `um`, `shared`, `winrt`), two lib trees, both under a version |
| ATL/MFC | in the Platform SDK | in the toolset, with a per-architecture lib directory |

SDK version selection is "newest complete", compared component by component as
integers — a string compare sorts `10.0.9xxxx` above `10.0.19041`, and
completeness matters because `bin/` routinely holds more versions than
`Include/` and `Lib/` do, so choosing on `bin/` alone can pick a version with
no headers at all. Either half can be pinned:
`--with-windows-sdk-home`, `--with-windows-sdk-version`.

Which layout is in play is detected from the directory, and then cross-checked
against what `cl.exe` reports: a `bin/Host*` tree whose compiler says VC9, or a
VC9 tree whose compiler says 19.x, is an error rather than a guess.

`solenv/inc/wntmsc14.mk` is the entire compiler delta, included at the end of
`wntmsci11.mk` (x86) and `wntmscx.mk` (x64). It has no conditional inside it,
because the file *is* the condition — which is what makes a VC9 build
byte-identical. `solenv/gbuild/platform/windows.mk` carries the same block for
the gbuild half; the two must agree, since a module built by one links against
libraries built by the other.

The one non-obvious flag choice is **`/std:c++14`, not 17**. Three things
depend on the dialect and all three say 14: `std::tr1`, which the MSVC library
still ships but only while `_HAS_CXX17` is 0 and which `boost/tr1` and the
stlport shims name directly; the empty exception specification, deprecated in
C++17 and removed in C++20, of which thousands remain; and
`<hash_map>`/`<hash_set>`, which become a hard `#error` under C++17. There is
no falling back further — a modern `cl` rejects `/std:c++03` outright (D9002),
so C++14 is the floor whether or not it is chosen.

### The STL shims

`main/stlport/systemstl/` holds nine headers that pull STL types into `std`.
Six of them **shadow** a standard header, and a shadowing header can only reach
what it shadows through a path that resolves against the compiler's own include
directory. VC9's spelling is `<../../VC/include/list>`; a modern toolset has no
such directory, so each of the six gained a `_MSC_VER >= 1900` arm spelling it
`<../include/list>`.

**Branch order is what makes that work, and it was wrong in three of them.**
`map`, `set` and `numeric` tested `__cplusplus` before `_MSC_VER` — and with
`/Zc:__cplusplus` that arm is now true, and it does `#include_next`, a GCC
extension MSVC does not have. The modern arm is now first in all six.

This deliberately diverges from `win10-64-minimal`, which moved four of them
into a `vc9/` subdirectory kept off the modern include path. Trunk's
`stlport/makefile.mk` carries its install rule as its first and only target,
which is also the module's default goal, so splitting that rule in two is not a
safe edit here.

The three that shadow nothing (`hash_map`, `hash_set`, `slist`) are untouched:
they forward to `<unordered_map>` and friends with a plain `#include`.

### Third-party

`redland` and `rasqal` ship hand-written win32 config headers full of shims for
what old MSVC lacked. Three of those stopped being redundant and became
harmful once the UCRT started **declaring** the real function, because a macro
rewrites that declaration as it is being parsed. `round` is the instructive
one: it is declared in `corecrt_math.h`, so `#define round(x) floor(x+0.5)`
makes the header stop parsing with C2059/C2143, some distance from anything
that mentions `round`.

Delivered as new `*.patch.ucrt` files listed after the existing
`*.patch.win32`, so the base patches stay reviewable against upstream.

`libxml2` needed nothing — 2.9.10 already guards its own `snprintf` shim on
`_MSC_VER < 1900`. `nss` needed nothing either.

## Verification

Honest accounting, because the gap matters more than the list of changes.

**Checked:**

* `configure.ac` — both edited regions extracted, macros stubbed, and parsed
  with `sh -n`. Two pre-existing unbracketed `AC_MSG_CHECKING(...)` calls
  elsewhere in the file defeat that check beyond the edited regions; they are
  not new.
* `set_soenv.in` — `perl -c` after substituting the `@...@` placeholders.
* the two new third-party patches — `patch --dry-run -p2`, run from the
  directory the build applies them in. Both apply cleanly.
* `throwspec.py report` over the whole tree, post-sweep: no typed specification
  remains, including in the files trunk added since the source branch diverged.
* the source cherry-picks — 5,069 of 5,070 files in the sweep applied without
  conflict, and every other picked commit applied clean.

**Not checked — this is the whole of it:**

* **Nothing has been compiled.** No `configure` run, no `bootstrap`, no build,
  on either compiler.
* `autoconf` is not available here, so `configure.ac` has never been expanded.
* The **VC9 regression claim** is by construction, not by measurement: every
  toolchain edit adds a branch in front of existing code rather than rewriting
  it, and every source edit is legal C++03. That is a strong argument and not a
  green build.
* Details that only a link exposes: manifest handling (`mt.exe`,
  `_VC_MANIFEST_*`), whether `-Zm500` and `-safeseh` are still accepted, and
  whether `-NODEFAULTLIB` plus the three named CRT libraries is the complete
  set.
* The `<../include/NAME>` retarget is proven for `vector` and `functional` on
  the source branch and unproven for the other four, though the mechanism is
  identical.

Expect a tail. On `win10-64-minimal` the last mile was six defects in four
families, and every one of them was found by building rather than by reading —
which is the strongest reason to treat the list above as a starting position
rather than a finished port.
