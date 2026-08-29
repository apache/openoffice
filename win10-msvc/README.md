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

**Status: the build is green, end to end.** On VS2019 14.29.30133 with Windows
SDK 10.0.19041.0, `configure`, `bootstrap` and `build.pl` all run to
completion into a `wntmsci14.pro` output tree, and `instsetoo_native` produces

    openoffice450.msi
    openoffice450sdk.msi

Every bundled third-party library builds — including, for the first time on
Windows, **Python**, which trunk has never built here at all.

Two things that headline does not say, and both matter more than it does.

**Nothing has been run.** The MSI exists; nobody has installed it or started
the office. The single piece of runtime evidence points the wrong way:
`testtools/source/bridgetest` fails with `getCaughtException() failed!`, which
is UNO exception propagation through the x86 C++ bridge. See
[The open defect](#the-open-defect).

**Some of the green is subtraction.** The CLI/.NET binding is switched off
entirely, and Python is built without `_ssl`, `_ctypes`, `_bz2`, `_lzma` and
`_sqlite3`. Both are deliberate and both are described below, but a build that
is green with those absent is a narrower claim than one that is green with
them present.

That is a different claim from the one this file made when it was written, and
the difference was expensive: **forty defects so far, every one of them found
by building and none of them by reading.**
[What building found](#what-building-found) groups the forty by cause;
[Verification](#verification) says what is now measured and what is still only
argued.

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

## The old Platform SDK does not go away entirely

`--with-frame-home` must keep pointing at the **Platform SDK v7.0**, whatever
compiler is in use. It is checked by looking for `lib/mscoree.lib`
([`configure.ac:2651`](../main/configure.ac)), and the Windows 10 SDK has no
`mscoree.lib` anywhere in its `Lib/` tree — verified, not assumed. That library
is the .NET Framework import library the CLI/managed parts link against, and it
never moved into the new SDK.

The other two flags a build script typically derives from the same variable do
move:

| flag | on a modern toolset |
| --- | --- |
| `--with-frame-home` | **keep** on Platform SDK v7.0 — `lib/mscoree.lib` |
| `--with-psdk-home` | drop; the Windows 10 SDK is found automatically, and pointing this at v7.0 now warns |
| `--with-midl-path` | drop, so the Windows 10 SDK's `midl.exe` is used — it has to match the headers it is generating against |

`$FRAME_HOME/lib` is the whole v7.0 library directory, so it does put a full set
of old import libraries on `LIB`. That is safe by ordering rather than by luck:
`set_soenv.in` places the SDK's `ucrt` and `um` directories **before** it, so the
Windows 10 copies of `kernel32.lib` and friends win, and v7.0 supplies only what
the earlier directories do not have — which is `mscoree.lib`.

## Known gap: the staged CRT is still VC90

`bootstrap` calls `oowintool --msvc-copy-dlls`, which finds a compiler through
the pre-Windows-8 registry keys it knows about, and on a machine that still has
VS2008 installed that is VS2008. So `main/external/msvcp90/` receives
`msvcr90.dll`, `msvcp90.dll`, `msvcm90.dll` and `Microsoft.VC90.CRT.manifest`,
`main/external/prj/d.lst` delivers them, and the installer ships them --
while everything else is now built against the UCRT.

This does **not** stop a build: the files exist, they are copied, nothing
references them at link time. What it produces is an installation carrying
three dead VC90 DLLs and missing the runtime it actually needs
(`vcruntime140.dll`, `msvcp140.dll`, and the UCRT). On a developer machine
that runs anyway, because installing Visual Studio installs the redistributable
system-wide; on a clean machine it would not.

The modern equivalent is `VC/Redist/MSVC/<toolset>/<arch>/Microsoft.VC142.CRT`,
and note it is *not* a like-for-like swap: the UCRT is an operating-system
component on Windows 10 rather than an application-private SxS assembly, so the
manifest half of the old arrangement has no counterpart rather than a renamed
one.

`win10-64-minimal` drew its scope line in exactly the same place -- "this
branch targets compiling; the CRT/SxS story is the phase after" -- so this is
inherited, not newly introduced. It is recorded here because it is invisible
until someone installs the result on a machine without Visual Studio.

Related, same cause, harmless: `bootstrap` prints
`Can't find MS Visual Studio / VC++ at ./oowintool line 228`. That is
`find_msvs()`, which looks for the Visual Studio IDE through the same old
registry keys; VS2019 BuildTools registers none of them. Nothing downstream
needs the answer once `--with-cl-home` is given.

## What building found

Forty defects, grouped by cause rather than by module and running roughly in
the order a build meets them. The grouping is the useful part: each family has
more members further up the tree, and knowing the family is how the next one
gets recognised in one reading instead of three.

**The toolchain has to be located before it can be wrong** (4).
`--with-cl-home` and friends were being overridden by the branch's own guesses
rather than winning over them; `ml.exe` moved into a per-host subdirectory
(`bin/Hostx64/x86`) that no longer matches the flat VC9 layout; and the Windows
10 SDK search found Platform SDK v7.0 first and accepted it. The SDK search is
now one rule over an ordered candidate list.

**The UCRT stopped declaring things** (2). `PATH_MAX` in `soltools/cpp`,
`__iob_func` in ICU's `icuio`. Both had been supplied by the old CRT without
being anyone's deliberate dependency.

**A shim that became harmful** (1 more, 5 total on this branch). `libxmlsec`
joined `sal`, `uwinapi`, `redland` and `rasqal` in `#define`-ing away a name
the UCRT now declares for real. The guard is always `_MSC_VER < 1900`.

**Tools and flags that no longer exist** (4). `/OPT:NOWIN98`; `vcbuild.exe`,
which is why CoinMP went 1.7.6 → 1.8.4; `lib.exe -EXTRACT:/`, which is below
because of *how* it fails; and `/clr:oldSyntax`, which is its own entry.

**The build host is not the target** (1). NSPR took its architecture from
`uname`, which on a 64-bit Cygwin building a 32-bit product is wrong.

**Bundled third-party builds more than we need** (2). ICU's test programs and
`icuio` both fail and neither is used.

**Patch mechanics, not code** (4). A gtest patch reconstructed by un-applying
it mentally was wrong by one blank line and its hunk rejected silently; the
bundled tr1 tuple was forced tree-wide instead of where it is needed; git was
normalising CRLF inside `*.patch` files, now held by `.gitattributes`; and
CoinMP 1.8.4 ships an upstream packaging bug naming `.vcxproj` files under a
`v9` directory.

**The CRT split in three** (5). `-NODEFAULTLIB` means every part must be
named, and there turned out to be **three different ways to end up with one
third of it**: `sal`'s `kill` chooses the static CRT through `DYNAMIC_CRT`
and got a mismatched pair; `embedserv`'s in-process server *replaces* `LIBCMT`
outright, so an append made earlier is lost; and `regpatchactivex` and
`desktop`'s win32 setup never touch `LIBCMT` at all, appending `libcmt.lib` to
their own `SHL1STDLIBS`. Each needed a different fix. Plus the gbuild side and
the CoinMP link inputs.

**Modern SDKs moved things, or dropped them** (3). The 64-bit shell extension
computes its own library paths and both layouts had moved — the SDK gained a
version level and split into `um/` and `ucrt/`, the toolset renamed `amd64/`
to `x64/`. `mapix.h` is not in the Windows 10 SDK **at all**: extended MAPI
went to Outlook, and only simple MAPI stayed. Python 3 split its public
headers into `Include/` and `Include/cpython/`, and the delivery list only
knew about the first.

**An x64 source branch leaves x86 twins untouched** (1). The `<typeinfo.h>`
fix was cherry-picked onto `msvc_win64_x86-64/except.cxx` and its message says
"the file is the MSVC x64 bridge which no other platform compiles" — true
where it came from. This build compiles `msvc_win32_intel`, which holds a
second copy of the same line. Expect more of these.

**Includes VC9 supplied by accident** (2 sweeps, 42 files). `std::back_inserter`
and the iterator types live in `<iterator>`, which VC9's containers dragged in
and a modern MSVC's do not. Recorded as two entries because the first sweep
was wrong twice — see the note on sweeps below.

**Deferred work coming due** (2). Two things trunk had explicitly parked until
a modern compiler existed, which is what this branch is. The CLI binding is
Managed Extensions for C++ and needs `/clr:oldSyntax`, removed after VS2015 —
switched off here, and a C++/CLI port is its own piece of work. Python had its
entire Windows half commented out with a note saying so; it now builds through
PCbuild and MSBuild.

**Building Python turned out to be four defects, not one** (4). `find_python.bat`
probes only `py -3.10` and `py -3.9` and otherwise **downloads** a Python
through nuget, which offline cannot do — `HOST_PYTHON` is the documented way
past that. It must be a *native* Python: a cygwin one passes a version check
and still reads the native paths PCbuild hands it as relative, prepending its
own working directory. `IncludeExternals=false` looks like the way to stay
offline and also silently removes zlib from `pythoncore`, which then does not
compile. And `pcbuild.sln` carries dependencies that are editorial rather than
structural — `python.vcxproj` declares one on `_ctypes` so the IDE yields a
usable interpreter — so the projects are built individually instead.

**An awk with a space in its path** (1). `configure.ac` spells `$AWK`
unquoted in some forty places. On a Cygwin carrying `gawk-<version>.exe` but
no plain `gawk`, the only `gawk` on `PATH` is Git for Windows' copy under
`C:\Program Files`, and the build dies hundreds of lines away with
`/cygdrive/c/Program: No such file`. Fixed by taking the 8.3 form, because the
Cygwin-side fix does not survive a Cygwin update.

**Symbols a modern compiler adds** (1). `__xmm@<hex>` vector constants appear
in archive symbol tables exactly as `__real@<hex>` always has. They are merged
COMDATs, not exports, and a `.def` naming one fails to link.

### The most expensive shape: succeeding while doing nothing

Three of the forty did not fail. They reported success, wrote a plausible
output file, and left the damage to surface somewhere else entirely:

* **`makedepend`** rejects `-std:c++14` — `-s` is one of its own options — and
  exits before scanning a single `#include`. It still creates the `.d` file,
  and that file still holds its target line, so make is satisfied. Every gbuild
  C++ object in the build had an **empty dependency list**. A clean build is
  unaffected, which is why it survived a full run; an incremental one silently
  stops rebuilding on header changes.
* **`lib.exe -EXTRACT:/`** cannot extract an archive's linker member any more.
  It says so, writes nothing, and **exits 0**. The build fails later, in
  `ldump`, naming the `.def` rather than the step that broke.
* **An external module's `so_built_*` flag** is not invalidated by editing its
  `BUILD_ACTION`. The first Python build after rewriting the whole Windows
  half did nothing at all and reported success.

The common thread is that none of them is visible from a green build. They are
found by reading the noise in a log that already succeeded, which is a habit
worth keeping for the rest of the port.

### A note on sweeps

The `<iterator>` sweep was wrong twice, and both times it looked complete.
First it matched the factory functions (`std::back_inserter`) and not the
types they return, so it missed a file whose only use is
`typedef back_insert_iterator<contents_t> inserter_t`. Widened to bare names
for files saying `using namespace std`, it missed the same file again, because
that line reads `using namespace ::std;` and the pattern did not allow the
leading `::`.

A criterion narrower than the defect finds a tidy subset and reads like
completeness. Both misses were found by the build rather than by re-reading
the pattern.

## The open defect

    getCaughtException() failed!
    dmake:  Error code 1, while making 'runtest'

`testtools/source/bridgetest` is the only thing on this branch that has
actually been *run*, and it fails. It exercises UNO exception propagation
across the C++ bridge, which is `cppu::getCaughtException()` reaching into
MSVC's own exception machinery to recover the thrown object.

The implementation is
`bridges/source/cpp_uno/msvc_win32_intel/except.cxx`, and it decodes MSVC's
`ThrowInfo` / `CatchableTypeArray` structures using layout assumptions that
were true for VC9. `win10-64-minimal` rewrote the **x64** copy of that file
against the modern layout — it carries its own SEH decoding and a long comment
diagramming `EXCEPTION_RECORD` — and the x86 copy never received the same
treatment, because that branch never compiled it.

So this is the same family as the `<typeinfo.h>` miss, and a far deeper
instance of it. It is not a build fix and has not been attempted here.

It is worth being clear about the consequence: exception propagation is not a
corner of this product. Until this works, a build that produces an installer
should not be read as a product that runs.

## Verification

Honest accounting, because the gap still matters more than the list of changes.

**Now measured, by building:**

* `configure` runs and completes, and `configure.ac` therefore expands —
  `autoconf` was not available when this file was first written, so the whole
  file had never been through `m4`.
* `bootstrap` runs, `set_soenv.in` produces a `winenv.set.sh` the build sources
  without complaint, and `COMEX=14` selects a `wntmsci14.pro` output tree.
* Every bundled third-party library builds and delivers, Python included.
* Every module builds and delivers, and `instsetoo_native` assembles both
  installers.
* The awk fix is measured, not argued: `configure` now completes with Git for
  Windows on `PATH`, and logs
  `.../Program Files/Git/usr/bin/gawk has a space in it, using
  /cygdrive/c/PROGRA~1/Git/usr/bin/gawk.exe instead`. Both checks that used to
  die there — the environment sanity check and the GNU make version check —
  pass.
* `-Zm500` and `-safeseh` are still accepted; `-NODEFAULTLIB` plus the named
  CRT libraries is the complete set in both the static and the dynamic model.
* The `<../include/NAME>` retarget works. Four of the six shadowing headers —
  `list`, `map`, `set`, `vector` — have now been compiled through on this
  compiler, which the C4464 warnings in the log record by name. `map` and `set`
  are also two of the three whose branch order was wrong, so that fix is
  measured rather than argued.

**Still only argued:**

* **The product has never been started.** Building is not running, and the one
  runtime test that exists fails. See [The open defect](#the-open-defect).
* **The CLI/.NET binding is absent**, not fixed. Six places had to be taught
  that — three `util/makefile.pmk` files, the SDK's file list, the SDK's
  checker, bridgetest's IDL, and the installer via `SCPDEFS`.
* **Python is missing five extension modules**, because their sources are
  downloaded by `get_externals.bat` and an offline build cannot run it.
* The **VC9 regression claim** is by construction, not by measurement: every
  toolchain edit adds a branch in front of existing code rather than rewriting
  it, and every source edit is legal C++03. Nothing on this branch has been
  compiled with VC9. That is a strong argument and not a green build.
* `functional` and `numeric`, the other two shadowing headers, have not been
  reached yet.
* Manifest handling (`mt.exe`, `_VC_MANIFEST_*`) — nothing built so far
  exercises it.
* Nothing has been **run**. The build produces libraries; whether the product
  starts is a question this branch has not asked. See also
  [Known gap: the staged CRT is still VC90](#known-gap-the-staged-crt-is-still-vc90),
  which is a runtime problem by construction.

The rate is the useful number here. On `win10-64-minimal` the last mile was six
defects in four families; this branch took forty in a dozen to reach a green
build, and a green build is not the finish line — it is the point at which the
runtime questions start.
