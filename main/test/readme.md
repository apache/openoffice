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

# C++ unit-test infrastructure — Bazel migration

AOO's C++ tests are **GoogleTest 1.7.0**-based (not CppUnit, apart from a few
holdouts).  This brings the test layer onto Bazel so suites run under
`bazel test`.

## Pieces

| Piece | Path | Role |
| ----- | ---- | ---- |
| `@gtest` | `ext_libraries/modules/gtest/1.7.0/` | GoogleTest 1.7.0 bzlmod wrap (zip cached in `ext_sources`). Built with `/Zc:wchar_t-` so its `wchar_t` ABI matches `sal_Unicode` test code. |
| `gtest_test` rule | [//build/rules:gtest_test.bzl](../../build/rules/gtest_test.bzl) | Reusable runnable-test rule. The `/MD` toolchain embeds no manifest, so a bare `cc_test` exe can't launch (DLLs land in runfiles subdirs; loose CRT → R6034). This stages the exe + runtime DLLs + VC90 CRT + an external `<exe>.manifest` into ONE flat dir (the test analog of `//main/idl:svidl_bundle`). Pass `data_files` for fixture inputs the test opens by relative path — see "working directory" below. |
| `sal_process_init` | [//build/testsupport](../../build/testsupport/BUILD.bazel) | Migration-authored TU that runs `sal_detail_initialize()` from a dynamic initialiser, for suites whose own `main()` skips the `SAL_IMPLEMENT_MAIN` boilerplate (⇒ no `WSAStartup`). |
| `libtest` | [//main/test:test](BUILD.bazel) | `test.dll` — `test::OfficeConnection` + arg/url helpers, for *subsequent* (UNO) tests that bootstrap a running soffice over URP. Exercised by `:test_qa_officeconnection`; its Java twin is `:test_jar` (see "The JAVA side of the same fixture" below). |
| `vc90_app_manifest_res` | [//main/external/msvcp90](../external/msvcp90/BUILD.bazel) | The VC90-CRT manifest compiled to a `.res` and linked into every `gtest_test` exe at `RT_MANIFEST` id 1. See "the CRT activation context" below. |
| `sal_qa_test` macro | [//main/sal:sal_qa.bzl](../sal/sal_qa.bzl) | Thin `gtest_test` wrapper for the sal/qa suites (common copts/deps + per-dir `*_Const.h` include). |

## Two test categories

1. **Standalone unit tests** — own `main()`, link sal/lib only, no office.
   Run directly via `gtest_test`. Examples:
   - `//main/o3tl:o3tl_test`
   - `//main/tools:tools_pathutils`
   - `//main/sal:sal_tests` (green gate, 22 targets)
   - `//main/salhelper:salhelper_test`
   - `//main/comphelper:comphelper_test_string`, `:comphelper_test_weakbag`
   - `//main/sax:sax_test_converter`
   - `//main/cppu:cppu_qa_any` / `:cppu_qa_unotype` / `:cppu_qa_reference` /
     `:cppu_qa_recursion`
   - `//main/cppuhelper:cppuhelper_tests` — `:cppuhelper_qa_ifcontainer` (5),
     `:cppuhelper_qa_unourl`, `:cppuhelper_qa_weak`
   - `//main/binaryurp:binaryurp_tests` — `:binaryurp_qa_cache`,
     `:binaryurp_qa_unmarshal`
   - `//main/shell:shell_qa_zip` (3) — zipfile reader over a real `.odt`
   - `//main/sal:qa_rtl_strings` (5 TUs, one exe)
   - `//main/sw:sw_qa_bigpointerarray` (26) — `BigPtrArray` behind `SwNodes`
   - `//main/sfx2:sfx2_qa_metadatable` — `Metadatable` / `XmlIdRegistry`
   - `//main/desktop:desktop_qa_dp_version` — extension version ordering
   - `//main/sal:osl_Socket_tests`, `:osl_StreamSocket`, `:osl_DatagramSocket`,
     `:osl_AcceptorSocket` (4 of the 8 socket suites; see the socket note below)
   - `//main/sal:rtl_Process` (3), `:osl_process` (7/8), `:rtl_Bootstrap`
     (25/30) — the child-process suites; see "the `bin` layout" below

   **Tests with private IDL types** (cppu/qa has a `types.idl` defining
   Enum1/Struct1/Interface1/… used only by the tests): reuse the `idl_library`
   rule (`//build/rules:idl_pipeline.bzl`) to compile the .idl → C++ headers,
   wrap it in a `cc_library(includes=["<name>_inc"])`, and add that as a test
   dep.  The test `#include`s the generated headers by bare name (`"Enum1.hpp"`)
   because the types sit in the global IDL namespace.  `test_any` also needs
   `@boost.legacy` (boost/type_traits).

   For a test that links a module DLL (not just sal), the staged exe needs
   that DLL **and its transitive runtime DLLs** co-located (the loader only
   searches the exe's dir).  comphelp.dll, e.g., drags in
   sal3/cppu3/cppuhelper3MSC/salhelper3MSC/ucbhelperMSC/vos3MSC — list them all
   in `runtime_dlls`.  Link-time `additional_linker_inputs` must include the
   import lib of every DLL whose symbols the test TU references directly
   (e.g. `cppu3_implib` for `uno_any_destruct`/`typelib_*` pulled in by an
   `Any` destructor or `getCppuType`).
2. **Subsequent / UNO tests** — need a real UNO installation, not just
   co-located DLLs. These are **two different fixtures**, long conflated under
   the one name "OfficeConnection":

   a. **In-process bootstrap** — the test calls
      `cppu::defaultBootstrap_InitialComponentContext()` and builds its own
      component context. **No soffice process is involved.** WIRED: pass
      `uno_install = "//main/staging:install"` to `gtest_test`.

      The launcher exports a single **`URE_BOOTSTRAP`** pointing at the staged
      `program/fundamental.ini`. That is the whole trick: the dmake recipe
      (`cppuhelper/qa/propertysetmixin/makefile.mk`) sets `UNO_TYPES`,
      `UNO_SERVICES`, `URE_INTERNAL_LIB_DIR`, … by hand, but `fundamental.ini`
      resolves `${ORIGIN}` against *its own* directory, so pointing at it
      supplies all of them transitively — and they cannot drift when the ini
      changes. `URE_BOOTSTRAP` takes a `vnd.sun.star.pathname:` URL (a *native*
      path after the scheme; `sal`'s `resolvePathnameUrl` converts it).
      The launcher also `cd`s into `program/` and prepends it to `PATH` so the
      component DLLs named in `services.rdb` load.

      Green: `//main/svl:svl_qa_test_URIHelper`.

      **Cost:** the test depends on the entire staged install, so it is slow to
      build and is not a unit test in any meaningful sense. Do not reach for
      `uno_install` unless the test genuinely bootstraps UNO.

   b. **Running-office connection** — `test::OfficeConnection` (libtest) starts
      a real soffice with `-accept=pipe,name=…;urp` and resolves a remote
      context over URP. Pass `office_connection = True` **together with**
      `uno_install` (the fixture still bootstraps an in-process context to build
      the URL resolver). The launcher then creates a throwaway user installation
      and passes the two arguments the fixture reads through `rtl::Bootstrap`:
      `-env:arg-soffice=path:<soffice.exe>` and `-env:arg-user=<dir>`.

      **There is no dmake recipe to port for this.** solenv's C++ run rule
      (`_tg_app.mk`, `APP1TEST`) invokes the bare exe with nothing but
      `--gtest_output=`, so it never sets these at all — an upstream gap, not
      something the migration dropped. The complete recipe is the **Java** one,
      `installationtest.mk::javatest`, and the launcher mirrors it (including
      wiping the user installation before *and* after). One deliberate
      divergence: Java passes `arg-user` as a `file://` URL, but the C++ side
      feeds it to `toAbsoluteFileUrl()` →
      `osl::FileBase::getFileURLFromSystemPath()`, so it must be a **native
      path**.

      **Upstream's only C++ consumer cannot be built**:
      `xmlsecurity/qa/certext` includes `<neon/ne_ssl.h>` and calls
      `ne_ssl_cert_read()`, but AOO replaced neon with **curl** — WebDAV
      (`ucb/source/ucp/webdav`) is curl-based, `configure.in` has no neon
      option, and no neon source exists in the tree (only a stale `NEON3RDLIB`
      in `solenv/inc/libs.mk`). That is a dead dependency, not a migration gap,
      and it is why the fixture is exercised by a migration-authored smoke test
      (`//main/test:test_qa_officeconnection`) instead — otherwise it would ship
      unexercised and rot.

      Note `OfficeConnection::setUp()` retries the resolve in an **unbounded**
      loop, so a soffice that never accepts hangs until the Bazel test timeout.
      That is why `office_connection` defaults `size` to `medium` (300 s) —
      `small`'s 60 s is barely 3× the observed run, and the timeout is the only
      thing that ends a run where the office never comes up.

      Green: `//main/test:test_qa_officeconnection` (~13 s — a full office boot,
      URP resolve, remote service creation and clean terminate).

   c. **Own mini installation** — the test supplies its own registry data and
      builds its own service manager, typically with
      `cppu::createRegistryServiceFactory(<rdb>)`. Neither (a) nor (b): no
      default bootstrap, no soffice. This is what a test *of the configuration
      layer* needs, because it must assert on keys the real registry does not
      have and write keys without touching a real user profile.

      Four `gtest_test` attributes cover it, and none is specific to that one
      test:

      - **`data_tree = {label: "relative/path"}`** — staging at a path you
        choose rather than flat by basename. Needed when the *shape* of the tree
        is the fixture: `$ORIGIN` in a bootstrap ini resolves against that ini's
        own directory, and a mini installation has two files named
        `bootstrap.ini`.
      - **`ure_bootstrap = "relative/path"`** — point `URE_BOOTSTRAP` at one of
        those staged inis instead of the install's `program/fundamental.ini`.
        Still requires `uno_install`: only the DATA root moves, and the UNO DLL
        closure still comes from `program/` via `PATH`.
      - **`env = {…}`** and **`prerun = […]`** — extra environment variables,
        and command lines run in the launcher just before the exe (each
        `|| exit /b 1`-guarded, so a fixture that fails to build fails the test
        loudly). Both expand `$(RUNDIR)`, `$(PROGRAM)`, `$(SCRATCH)` and
        `$(SCRATCH_URL)`.

      `$(SCRATCH)` is a fresh, empty, **writable** directory under
      `TEST_TMPDIR`, wiped before *and* after the run. Everything a test writes
      belongs there: the staged directory lives in `bazel-out` and must be
      treated as read-only build output. `office_connection`'s user installation
      is just this same scratch dir.

      `prerun` exists for fixtures that can only be built **at run time**. The
      configmgr case is the archetype: its registry has to be written by
      `regcomp`, a tool that bootstraps UNO — as a build action that would need
      its own staged DLL closure and CRT manifest, and the component location it
      records is an *absolute* path, which must not end up in a cacheable
      artifact.

      **Backslash rule, restated because it cuts both ways.** An `env` value the
      test reads back through `rtl::Bootstrap::get()` is macro-expanded, so its
      backslashes must be **doubled** (`arg-user`). One read with plain
      `getenv()` must **not** be (`CONFIGMGR_UNIT_FORWARD_STRING`). Prefer a
      `file:///` URL where the consumer accepts one — forward slashes sidestep
      the question entirely.

      Wired: `//main/configmgr:configmgr_qa_unit` — **red, and not fixable
      without a source change**. The fixture machinery above works; the test
      itself bootstraps through `createRegistryServiceFactory` + `regcomp
      -register`, i.e. the pre-`.component` registration mechanism that AOO
      retired, so `regcomp` cannot find `component_writeInfo` in
      `configmgr.uno.dll`. Upstream's own GoogleTest-migration commit
      (`7231f715d2`) records that the suite already failed and "on Windows it
      doesn't start running". See `main/configmgr/readme.md` for the full
      diagnosis, the staged layout, and the two forced divergences from
      `qa/unit/makefile.mk`.

      **Lesson for the remaining fixture work:** a `qa/` dir that dmake has
      gated behind `ENABLE_UNIT_TESTS=NO` for a decade may encode a mechanism
      the product no longer has. Check the test's *bootstrap path* against
      current source before costing the fixture — the blocker recorded for this
      one ("needs a custom `ure_bootstrap` root + extra env") was read off the
      makefile and was not the real obstacle.

## Gotchas (learned the hard way)

- **The C++/UNO bridge DLL is a run-time dep that nothing links.**
  `cppu::getCaughtException()` / `cppu::throwException()` — reached from any
  `OWeakObject` dispose path, `OInterfaceContainerHelper`, proxy marshalling —
  construct a `Mapping` between the C++ and UNO environments, and cppu's
  `uno_getMapping` **`osl_loadModule()`s the bridge on demand**
  (`msci_uno.dll` on x86, `mscx_uno.dll` on x64; `lbmap.cxx::loadModule`).
  It is invisible to the linker, so it must be listed in `runtime_dlls`:

  ```starlark
  runtime_dlls = [...] + select({
      "//build:arch_x64": ["//main/bridges:mscx_uno"],
      "//conditions:default": ["//main/bridges:msci_uno"],
  })
  ```

  Omit it and you get one of two confusing failures — neither of which names
  the bridge:
  - the mapping comes back **null** and the test AVs (SEH `0xc0000005`) deep
    inside `dispose()`, looking like a source bug (this is what
    `cppuhelper_qa_weak` did);
  - or the loader finds a *stray* copy elsewhere on `PATH`, whose CRT then
    loads outside the exe's activation context → **R6034** → `DllMain` fails →
    the process dies with `0xC0000142` (`STATUS_DLL_INIT_FAILED`) before gtest
    prints a single line, so `test.log` is **empty**.

- **The CRT activation context must be embedded, not external.** The staged
  `<exe>.manifest` only reliably covers DLLs bound at process start. A test
  that `osl_loadModule()`s a UNO DLL *later* needs the context still in force,
  so `gtest_test` links `//main/external/msvcp90:vc90_app_manifest_res` into
  every exe (`RT_MANIFEST` id 1, which the loader always honours and which
  supersedes the external file). `//main/bridges:jni_test_launcher` hit the
  same wall with the DLLs a hosted JVM loads and fixed it the same way.
  Symptom without it: R6034 / `0xC0000142` under `bazel test` while the very
  same exe run by hand from a shell passes — the discriminator is the
  environment, not the binary, so it is easy to misread as flakiness.
  (Not yet handled: under `--compilation_mode=dbg` the exes link `/MDd` but
  this `.res` still carries the *release* CRT manifest.)

- **System32 shadows our bundled `icuuc.dll`, and `PATH` cannot beat it.** The
  Windows loader searches `System32` *before* the working directory and `PATH`.
  Windows ships a ~36 KB `C:\Windows\System32\icuuc.dll`; AOO bundles ICU 49.1.2
  (~1.3 MB) under the same name. Any test exe importing it transitively —
  anything linking `sw.dll`, `sfx.dll`, … — therefore binds the *system* one,
  whose exports don't match, and dies at load with **`STATUS_ENTRYPOINT_NOT_FOUND`
  (`0xC0000139`)** and an **empty test log**. `soffice.exe` never hits this
  because it lives in `program/` next to our copy, and the exe's OWN directory is
  the one thing searched ahead of `System32`.

  `gtest_test` handles it: when `uno_install` is set it co-locates the ICU DLLs
  with the test exe. `icuuc.dll` is the only collision among the 248 staged DLLs
  (verified by diffing basenames against `System32`); the other two ICU DLLs are
  staged with it to keep the loaded ICU coherent.

  Triage note: this and the R6034 bridge landmine both present as *empty
  test.log + a load-time exit code*. `0xC0000139` = a DLL loaded but an export
  was missing (wrong DLL won the search); `0xC0000142` = `DllMain` failed (the
  CRT activation-context case).

- **Never assume the test's working directory.** It is *not* the exe's
  directory, and it is *not* the execroot either — a launcher that built paths
  from `%CD%` produced "The system cannot find the path specified" even though
  those paths were correct relative to the execroot. Anything the launcher needs
  to find must be located from **`%~dp0`** (the .bat's own directory) via a
  relative path computed at analysis time — `_windows_relpath` in
  [gtest_test.bzl](../../build/rules/gtest_test.bzl) does this, and it is why
  `uno_install` works from any package depth.

- **A backslash is an ESCAPE character in an `rtl::Bootstrap` value.** Every
  value `Bootstrap::get()` returns goes through macro expansion, where `read()`
  ([sal/rtl/source/bootstrap.cxx](../sal/rtl/source/bootstrap.cxx)) turns `\X`
  into `X` and `\uXXXX` into a code point. So a raw Windows path handed to a
  bootstrap variable comes back with **every separator gone** —
  `C:\Users\x` → `C:Usersx` — and `getFileURLFromSystemPath` then fails with
  21. Double the backslashes so one survives each unescape (the `gtest_test`
  launcher does this with cmd's `%VAR:\=\\%`). The dmake/Java side never hit
  this because it passes a `file://` URL, which has no backslashes.

- **With `uno_install`, co-locating a core UNO DLL BREAKS the bootstrap.** This
  inverts the usual "list every transitive DLL in `runtime_dlls`" advice, so it
  is easy to walk into while trying to be careful. cppuhelper finds the
  directory to load `bootstrap.uno.dll` (and the other stoc bootstrap
  components) from via `get_this_libpath()` — `Module::getUrlFromAddress()` on
  *itself*, in [cppuhelper/source/bootstrap.cxx](../cppuhelper/source/bootstrap.cxx).
  The loader searches the exe's own directory before `PATH`, so a co-located
  `cppuhelper3MSC.dll` makes that path the *test staging dir* and the bootstrap
  dies with `loading component library failed: …/<test>.run/bootstrap.uno.dll`.
  Everything in the office closure is already reachable through `program/` on
  `PATH` — leave it there. Both `//main/svl:svl_qa_test_URIHelper` and
  `//main/test:test_qa_officeconnection` list nothing but their own test-only
  DLL for exactly this reason.

  (`bootstrap.uno.dll` is also one of the hardcoded-name cases CLAUDE.md warns
  about for the `.uno` infix divergence — but it is staged under the upstream
  name, so the infix is *not* what bites here. The directory is.)

- **"`../bin`" was a misreading — it is the identity.** The child-process
  suites were long recorded as unwirable without source changes because they
  "resolve their helper exe via `getExecutablePath()`+`/../bin`". They don't.
  The idiom (`osl_process.cxx::getExecutablePath`, `rtl_Process.cxx` /
  `rtl_Bootstrap.cxx::getModulePath`) is

  ```
  dir-of(own module) → strip the last path component → append "bin"
  ```

  and under dmake both parent and child lived in `solver/bin`, so that
  round-trip resolves back to the *same* directory. Naming the staging dir
  `bin` reproduces it exactly — that is all `gtest_test`'s `bin_layout` does
  (it also implies `run_in_staged_dir`). No source change, no `companions`
  layout gymnastics. Worth remembering as a class: a path expression that looks
  like it reaches out of the staging dir may just be a no-op round-trip that
  the old layout satisfied trivially.

- **Staging a data file beside the exe is not enough.** Co-located DLLs resolve
  regardless (the loader searches the exe's own path), which makes it easy to
  assume relative file opens will too — they don't. A test that opens a fixture by bare relative name
  (`//main/shell:shell_qa_zip` → `simpledocument.odt`) throws
  file-not-found while passing when run by hand from the staged dir. Pass such
  inputs as `data_files`: they are staged beside the exe *and* the target
  switches to a `.bat` launcher that `cd`s there first and forwards the exit
  code.

- **`/Zc:wchar_t-` must be consistent across gtest and every test TU.** gtest
  declares `PrintTo(wchar_t)`; with `/Zc:wchar_t-` (`wchar_t == unsigned short`)
  on one side only, that mangles vs `PrintTo(unsigned short)` → `LNK2019`. Fixed
  in gtest's overlay copts AND baked into the `gtest_test` rule.
- **Editing a registry overlay file requires updating its sha256 in
  `source.json` + `bazel mod deps --lockfile_mode=refresh`.** Otherwise Bazel
  reuses the cached extraction and silently ignores the edit.
- **`$(TESTSHL2LIB)` in the dmake `*STDLIBS` is a no-op** — AOO retired
  `testshl2`; the variable is undefined (expands empty). So most suites are not
  blocked by a missing *lib*.  But a few still `#include` testshl2 *headers*
  (`cmdlinebits.hxx` → `getForwardString()`, plus its `WIN_BYTE`/`WIN_BOOL`
  typedefs) — those genuinely can't compile without a shim.  `osl_Security` is
  the one that bites: it's left **unwired** (not a green-gate exclusion — it
  won't build at all).  Don't be fooled by the half-fix: `/FIwindows.h` +
  advapi32 satisfies its Win32 SID/registry calls but not the testshl2 include.

## The JAVA side of the same fixture — `uno_junit_test`

Everything above is C++. The `qa/` directories also hold a large body of **Java**
suites — `qa/complex/*` (hand-written JUnit) and `qa/unoapi` (the qadevOOo
UNOAPI runner) — which drive a real office over UNO. They are the *only* form of
coverage for anything that exists solely as a UNO service, and they are wired
through [//build/rules:junit_test.bzl](../../build/rules/junit_test.bzl).

| Piece | Path | Role |
| ----- | ---- | ---- |
| `@junit_jar` | `MODULE.bazel` → [//build/third_party/junit](../../build/third_party/junit/BUILD.bazel) | JUnit **4.10** from Maven Central — upstream's `OOO_JUNIT_JAR`, never bundled in the tree. 4.10 and not later: it *embeds* hamcrest-core, so there is no second jar to keep in sync (upstream forks on `HAMCREST_CORE_JAR` in every recipe). |
| `test.jar` | [//main/test:test_jar](BUILD.bazel) | `org.openoffice.test.OfficeConnection` — the Java twin of `test::OfficeConnection`. |
| `OOoRunner.jar` | [//main/qadevOOo:OOoRunner](../qadevOOo/BUILD.bazel) | The UNOAPI framework, **now carrying its `objdsc/*.csv` resources** — see below. |
| `uno_junit_test` | [//build/rules:junit_test.bzl](../../build/rules/junit_test.bzl) | `java_library` + a launcher `.bat` mirroring `installationtest.mk::javatest`. |

### The two OfficeConnections are the same fixture, spelled differently

| | C++ (`gtest_test(office_connection=True)`) | Java (`uno_junit_test`) |
| --- | --- | --- |
| arguments | `arg-soffice` / `arg-user` via `rtl::Bootstrap` → the **environment** | `org.openoffice.test.arg.soffice` / `.user` via `System.getProperty` → **`-D` on the command line** |
| user installation | a **native path** (fed to `getFileURLFromSystemPath`) | a **`file:///` URL** (fed to `-env:UserInstallation=`) |
| transport | socket | **named pipe** (`pipe,name=oootest<uuid>`) |
| consequence | — | no port to reserve, so no `exclusive` tag: these suites run concurrently |

The recipe ported is `solenv/inc/installationtest.mk`'s `javatest` (identical in
`solenv/gbuild/JunitTest.mk`), including its wipe of the user installation
*before* as well as after.

### Which JVM, and why it is not the build JDK

`//build:jre.bzl` now also exports `jre_java_exe()` / `jre_home_native()`. The
test JVM has to match the **target** arch even though the office is a separate
process, because `OfficeConnection` connects over a named pipe and jurt's
`PipeConnection` is JNI: the JVM loads `jpipe.dll` out of the staged `program/`,
and a 64-bit JVM cannot load the 32-bit one a default build produces. `JAVA_HOME`
is pinned to the same JDK, because `OfficeConnection` always passes
`-env:UNO_JAVA_JFW_ENV_JREHOME=true`, which is what the *office's* jvmfwk reads.

### Three things were missing from the product, not from the test

Each of these was invisible until a foreign process loaded our DLLs:

1. **`jpipe.dll` / `jpipx.dll` were never staged.** Nothing in the office links
   or loads them — it is the *client* JVM that does — so their absence broke
   nothing visible. Any external Java program talking to a running office over a
   pipe needs them; upstream ships both in the URE lib dir. Now in
   [//main/staging](../staging/BUILD.bazel).
2. **A /MD DLL loaded by a process we do not build needs its OWN embedded
   manifest.** This tree links `/MANIFEST:NO` throughout, which is invisible
   inside `soffice.exe` (its manifest covers the whole process) and fatal for a
   DLL a stock `java.exe` loads: nothing supplies an activation context, the
   `MSVCR90.dll` import cannot be resolved, and `System.loadLibrary` reports the
   uninformative **"Can't find dependent libraries"**. Putting a loose
   `msvcr90.dll` on `PATH` only trades that for R6034. The fix is
   `//main/external/msvcp90:vc90_dll_manifest_res` — the same manifest at
   `RT_MANIFEST` **id 2** (the DLL slot; id 1 is the exe slot) — linked into both
   pipe DLLs. Upstream gets this for free: solenv embeds a manifest in every
   DLL.
3. **`OOoRunner.jar` had no object descriptions.** `helper/APIDescGetter` finds
   a description either under a `-objdsc` directory or, when that argument is
   absent, as the classpath resource `/objdsc/<module>` (it has an explicit
   `JarURLConnection` branch for exactly that). The `qa/unoapi` `Test.java`
   adapters pass no `-objdsc`, and upstream's Ant `jar` target does not include
   `*.csv` — so against upstream's jar every unoapi suite dies at its first
   object with `couldn't find module '<module>'`. Bundling them is a deliberate
   divergence, and it is what makes the whole unoapi category runnable.

### Green

| Target | What it covers | Time |
| --- | --- | --- |
| `//main/qadevOOo:qa_complex_junitskeleton` | upstream's worked example, and therefore the whole fixture: connect → `XMultiServiceFactory` → qadevOOo `TestParameters` → resolve a fixture document by relative path → **load it into the office** → close → ask the office for its temp dir (3 tests) | ~14 s |
| `//main/svl:qa_complex_passwordcontainer` | `com.sun.star.task.PasswordContainer` over UNO: per-URL credentials with and without a master password, persistent and session-only, driven through the test's own `XInteractionHandler` (3 tests) | ~13 s |
| `//main/svtools:qa_unoapi` | the first UNOAPI suite ever run here — 26 interface/property checks on `svtools.AccessibleTabBar` | ~25 s |
| `//main/qadevOOo:qa_unoapi` | the runner testing itself (`qadevOOo.SelfTest`) — the guard on the framework the other unoapi suites are built out of. Needs `fixture_starts_office`, see below | ~16 s |

The other 16 `qa/unoapi` suites are wired but have not been run yet — see
[the category section](#the-whole-qaunoapi-category-all-18-wired) below.

### When the office will not shut down: `fixture_starts_office`

`qadevOOo:qa_unoapi` is the suite that motivated this. With the faithful
fixture its assertions all pass — scenario green in 3 s — and then the run hangs
until the 300 s timeout with nothing printed after the job. Diagnosed rather
than guessed:

- the JVM's main thread is in `Process.waitFor()` (`OfficeConnection:126`), so
  `XDesktop.terminate()` has already returned and the office has not exited;
- the office is still alive, idle, has **no visible window**, and no longer
  answers a fresh UNO connection;
- a non-invasive `cdb -pv` stack dump shows the office's **main thread inside a
  WinProc dispatch that entered a nested VCL message wait**
  (`Application::Execute` → `DispatchMessageW` → … → `GetMessageW`) — so it
  holds the SolarMutex — while **two URP threads block in
  `vos::OMutex::acquire`**: an incoming `binaryurp` request into fwk's
  `LockHelper`, and an `sw` proxy release coming back through `msci_uno`.

That is an office **shutdown deadlock** between the solar mutex and in-flight
bridge calls, not a fixture defect — the suites that hold no stale proxies at
teardown terminate cleanly. Fixing it is a source change in the office.

`fixture_starts_office = True` makes the **launcher** start and kill the office
and the test attach to it (`connect:` instead of `path:`), which leaves
`OfficeConnection.tearDown()` a no-op — `process` is null, so it neither
terminates nor waits. The verdict becomes the JUnit result, and the shutdown
becomes a `taskkill`. Same division of labour as `server_args` in `gtest_test`.

Two mechanics worth knowing before reusing it:

- **the readiness wait is one PowerShell process, not a batch loop.** cmd cannot
  list the pipe namespace at all (`dir \\.\pipe\` answers "invalid parameter" —
  it is a device path), and osl's pipes appear as
  `\\.\pipe\OSL_PIPE_<SID>_<name>`, so the probe matches the name at the end.
  It is not optional: `OfficeConnection`'s resolve loop has **no sleep** when it
  did not start the process itself, so it would spin a core.
- **the kill matches the unique pipe name on the command line**, never
  `taskkill /im soffice.exe` — which would take down a developer's session and
  any concurrently running suite. All four suites run in parallel today.

It is **opt-in and costs something**: `tearDown`'s check that the office
terminates cleanly and exits 0. Use it only where that check is the thing that
is broken, and record why.

### The whole `qa/unoapi` category: all 18 wired

Every `qa/unoapi` directory in the tree is now a target, all named
`//main/<module>:qa_unoapi`. They are not 18 different tests — they are 18 sets
of *data* for one runner, which is why they are one macro, `unoapi_test`
(`build/rules/junit_test.bzl`), over the four things that actually differ:

| | |
| --- | --- |
| `module` | the source directory name, which is also the adapter's package — `org.openoffice.<module>.qa.unoapi.Test` for all 18 without exception |
| `sce` | the scenario: one `-o <object>` per line, commented-out lines carrying the issue number that broke that object |
| `xcl` | the matching exclusion list of individual interface tests |
| `tdoc` | the module's `qa/unoapi/testdocuments` directory, when it has one |

That mirrors the `JunitTest_<module>_unoapi.mk` files being ported, which differ
only in their `-Dorg.openoffice.test.arg.*` lines.

**`-tdoc` is per-module, and an earlier note here had it wrong.** It is *not*
`qadevOOo/testdocs` — every makefile that sets it points at
`$(SRCDIR)/<module>/qa/unoapi/testdocuments`, 10 small directories totalling
about 570 KB (`linguistic` is the odd one, pointing at `qa/unoapi` itself, which
is a way of passing something non-null: neither of its two objects opens a
document). `qadevOOo/testdocs` is only the *fallback* `util.utils`
`getFullTestDocName` reaches for when the argument is absent and `SRC_ROOT` is
set — a dmake-tree assumption, and not a path any wired suite takes.

The gate this did need was **`data_dirs`**, the directory-preserving companion
to `data_tree`, now in `gtest_test.bzl` and shared by both launchers. `data_tree`
maps one label to one staged path, which is right when the exact placement of a
file is the thing under test (a mini UNO installation has two files named
`bootstrap.ini` and only their directories tell them apart). It is the wrong
shape for a document *root*: the runner joins a document name onto `-tdoc` at
run time, so which documents a scenario opens is not a build-time fact and the
directory has to go over whole — including `dbaccess`'s and `forms`'s
`testdocuments/TestDB/`, a nested directory `data_tree` cannot express at all.
Each entry stages every file the label provides under the chosen directory, at
its path relative to the **longest common directory** of that label's files; the
strip prefix is derived rather than declared, which is exact for the
`glob(["<dir>/**"])` filegroup the macro generates and keeps the call site to the
one fact it knows — where the tree goes.

Expect reds, and expect them to be informative rather than structural — this is
the first time these suites have run here at all.

#### First one run: `toolkit`, red on an **orphaned mutex in `acc.dll`**

The fixture works — it connects, stages `testdocuments/`, and the first object
(`toolkit.AccessibleDropDownComboBox`, the Find toolbar's combo box) passes four
whole interfaces. Then the office **hangs**, and takes the other 52 objects with
it. Diagnosed with a non-invasive `cdb -pv -p <pid>` against the live hang:

- Windows logs an **Application Hang** (event 1002), *not* a crash; the office
  stops pumping messages and `Process.Responding` is `False`. The Java side sees
  only the consequence — `EOFException` on the URP pipe, then `DisposedException`
  out of `tearDown`.
- The **main thread holds the SolarMutex** (inside a WinProc dispatch under
  `Application::Execute` → `Yield` → `DispatchMessageW`) delivering a focus
  event: `Window::ImplGrabFocus` → `ImplCallActivateListeners` →
  `VclEventListeners::Call` → `ootk!VCLXAccessibleComponent::WindowEventListener`
  → `acc.dll` → `sal3!osl_acquireMutex`, where it blocks.
- A **`binaryurp` worker** — the test's incoming UNO call — blocks on the *same*
  `acc.dll` mutex.
- `!locks` names the contended critical section (`LockCount 2`, exactly those two
  waiters) and its `OwningThread` — **and that thread ID is not in the live
  thread list.**

So it is *not* a lock-order inversion, and not the solar-mutex-vs-URP shutdown
deadlock recorded above for `qadevOOo`: the lock is **orphaned**. A thread left
`acc.dll`'s mutex held and exited, and an osl mutex is a plain Windows
`CRITICAL_SECTION` — once orphaned it can never be acquired again, so the first
VCL event *and* the first UNO call that reach accessibility both block forever.
Nothing recovers it in-process.

That is a real product defect reachable by any assistive technology that attaches
to a running office, not just by this test, and fixing it is a source change in
`main/accessibility` — out of scope here. The suite stays wired and red, the same
disposition as `//main/bridges:test_any_jni`. Upstream's `knownissues.xcl`
excludes `toolkit.AccessibleComboBox` but **not** `AccessibleDropDownComboBox`,
i.e. upstream considers this object testable — consistent with a `qa/` directory
gated behind `ENABLE_UNIT_TESTS=NO` for a decade that has simply never run.

Two more reds are known in advance:

- **`dbaccess`** is the only adapter taking `-ini`, and its `dbaccess.props`
  names a MySQL server (`jdbc.url=mysql://unoapi:3306/testDB`) no developer
  machine has. `ORowSet` and `OSingleSelectQueryComposer` reach for it; the
  other seven objects do not. Wired anyway — a partial red that names which
  objects need a database beats an unwired suite, and the props file is staged,
  so pointing it at a real server is a one-line change.
- **`sc`** has 43 active objects and **67 commented out**, each with its issue
  number. That ratio is the honest upstream state of the Calc suite, not
  something lost in migration; the lines are left exactly as they are.

The long poles are `sw` (75 objects), `toolkit` (53), `sc` (43) and `forms`
(34) — every object is instantiated in a running office, so those four take
`size = "large"`.

### What is left

That leaves the **24** `qa/complex` directories, which unlike unoapi are
hand-written suites with no shared shape, so they come one at a time. Two things
gate them:

- **per-module fixtures** — `svl/qa/complex/ConfigItems` needs a C++ helper
  component built alongside;
- **several were never wired upstream either** — `writerfilter/qa/complex` and
  others have no `makefile.mk` at all.

## The sal suite is deliberately NOT a green gate

`//main/sal:sal_tests` runs **every** migrated self-contained sal/qa test — 48
targets, passing and failing alike, on the principle that failures are
information, not something to hide (the rationale lives next to the
`test_suite` in [main/sal/BUILD.bazel](../sal/BUILD.bazel)). So expect it to be
red. As of 2026-08-04, 35 pass and these 13 fail, each on its **own merits** —
none is a build or loader problem, and the source is out of scope:

| Target | Failing | Why |
| ------ | ------- | --- |
| `osl_File` | 10 | Assert on a specific drive topology (CD/floppy/RAM disk) absent here |
| `osl_Module` | 7 | Loads a module by path; depends on the dmake `solver/bin` layout |
| `tcwf` | 1 | `osl_writeFile` to a fixed path — permission bound |
| `rtl_logfile` | 1 | Writes/reads `c:/temp` and asserts on it — env/permission bound |
| `osl_Thread` | 1 | `resume_001` is a timing race; flaky, not deterministic |
| `rtl_OUString2` | 1 | `convertFromString` expects `\x80` to fail UTF-8 validation — test-data drift, same class as `rtl_textcvt` |
| `qa_rtl_strings` | 1 | `Convert.convertToString` — same text-conversion drift |
| `osl_process` | 1 | `osl_execProc_merged_child_environment` asserts the merged vars come back in the order the test listed them (`std::equal` over a vector), but Windows returns the environment block sorted case-insensitively — `PAT`, `Patha`, `PATHb` vs the expected `PAT`, `PATHb`, `Patha`. Ordering bug in the test |
| `rtl_Bootstrap` | 5 | 4 of them (`getFrom_004_1`, `setIniFilename_002`, `testOverride`, `testNonexisting`) read the **default** bootstrap context and expect `testshl2.ini` — the default ini is `<exename>.ini`, and under the retired testshl2 harness the process *was* `testshl2.exe` (the log shows ours resolving `rtl_Bootstrap.ini`). `expandMacrosFrom_002_2` is ini cross-reference precedence |
| `osl_SocketOld` | 10 | see socket note below |
| `osl_SocketAddr` | 3 | see socket note below |
| `osl_Socket2` | 7 | see socket note below |
| `osl_ConnectorSocket` | 1 | see socket note below |

### The osl/socket suites

Long recorded as "blocked on a CppUnit external dep" — that was simply wrong.
All 8 apps in `qa/osl/socket/makefile.mk` are plain GoogleTest suites with no
testshl2 include; they only ever needed `ws2_32`.

What they *did* need is `//build/testsupport:sal_process_init.cxx`. Each
declares its own bare `main()` instead of going through `SAL_IMPLEMENT_MAIN`,
and on Windows that macro is what calls `sal_detail_initialize()` →
**`WSAStartup()`** (sal3.dll's `DllMain` only ever calls the matching
`WSACleanup()`, never `WSAStartup`). Without it Winsock is never initialised,
every osl socket call returns `WSANOTINITIALISED`, and the suites collapse in
ways that implicate everything except the real cause — `SocketAddr`
constructors quietly yield unusable addresses, `getLocalHostname()` fails.
These are testshl2-era suites; that harness used to supply `main()` for them.
The shim does the init from a dynamic initialiser (which the CRT runs before
`main`), using `__argc`/`__argv` — `osl_setCommandArgs` asserts `argc > 0`.

With it, 4 of the 8 are fully green (`osl_Socket_tests`, `osl_StreamSocket`,
`osl_DatagramSocket`, `osl_AcceptorSocket`) and the rest go from
near-total failure to a small residual set — none of it a build problem:

- **Upstream already knew.** `connect_003` and `getLocalHost_001` are listed in
  the module's own `.xsce` exclusion files *specifically for `wntmsci`*, our
  exact platform; `ctors_family_Ipx` (IPX is dead on modern Windows) and
  `getHostname_002` are excluded on every platform.
- **Needs Administrator.** `ctors_TypeRaw` / `getType_003` create a
  `osl_Socket_TypeRaw` socket — privileged on Windows. The test's own message
  admits it doesn't pass on Linux/Solaris either.
- **Unsupported socket option.** `setOption_001` / `getOption_simple_001` set
  `SO_DONTROUTE`; the test even comments "maybe asAcceptorSocket is not right
  initialized".
- **DNS/host dependent.** `getHostname_001`, `getSocketAddrHandle_002`,
  `getLocalPort_002` resolve names against whatever network the machine is on.

### Red on their own fixture, wired anyway

- `//main/writerfilter:writerfilter_qa_doctok` — `testInitUno` passes (a real
  in-process UNO bootstrap + UCB ContentBroker), but `testOpenFile` /
  `testEvents` read `<cwd>/test.doc` and **no `test.doc` exists anywhere in the
  tree** — the sample was never checked in (the qa dir holds only the `.cxx` and
  `doctok.job`). `testEvents` then trips a `shared_ptr` assert on the null
  document. That is the suite's own missing fixture, not the build.

Earlier revisions of this file listed `rtl_str`/`rtl_ustr`/`rtl_string` as
NULL-deref crashes; those were since fixed (boundary checks in the tests plus
`NULL` guards at the rtl entry points) and now pass.

(`osl_Security` is not in the suite at all — it fails to *build* on testshl2,
see the gotcha above; it is unwired entirely, not an exclusion.)

`rtl_strings` was also filed under "needs CppUnit". It does not: it includes
`testshl/tresstatewrapper.hxx`, i.e. the retired **testshl** harness, and its own
`readme.txt` says it is "the old test implementation of rtl::XString",
superseded by `sal/qa/rtl/ostring` + `sal/qa/rtl/oustring` — both of which are
already wired (`rtl_OString2`, `rtl_str`, `rtl_string`, `rtl_OUString2`,
`rtl_ustr`). So it is **dead weight, not a blocker**; there is nothing to gain
by migrating it.

The child-process tests (`osl/process`, `rtl/process`, `rtl/bootstrap`) were
listed here as unwired for exactly that reason; they are wired now via
`bin_layout` + `companions` (see the gotcha above). Their helper exes are built
by `sal_qa_helper_exe` in [sal_qa.bzl](../sal/sal_qa.bzl) — the same compile
environment as `sal_qa_test` minus gtest.

Both TUs of the `osl/process` pair walk the environment block with `LPTSTR` /
`GetEnvironmentStrings()` / `_tcslen()` but **neither includes `<windows.h>`**,
and nothing they include reaches it (`precompiled_sal.hxx` is empty; no rtl
header pulls it in). That is bit-rot, not a migration gap — dmake gates every
`qa/` dir behind `ENABLE_UNIT_TESTS`, which defaults to NO, so these had not
compiled in years. Supplied with `/FIwindows.h`, but the two want **opposite**
flavours and getting it backwards compiles cleanly while silently walking an
ANSI block as wide chars: the parent defines neither `UNICODE` nor `_UNICODE`
and does `std::string(p)` → ANSI; the child defines both itself and does
`reinterpret_cast<wchar_t*>` → needs `UNICODE` defined *before* `windows.h`
arrives, hence `/DUNICODE=` (empty, so the source's own `#define UNICODE` is an
identical redefinition rather than a C4005). `/DNOMINMAX` is already global in
the toolchain, so force-including `windows.h` ahead of `<algorithm>` is safe.

Still unwired: nothing in sal/qa except `osl_Security`, which does not compile
(testshl2 header — see the gotcha above).
