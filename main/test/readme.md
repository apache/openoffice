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
| `gtest_test` rule | [//build/rules:gtest_test.bzl](../../build/rules/gtest_test.bzl) | Reusable runnable-test rule. The `/MD` toolchain embeds no manifest, so a bare `cc_test` exe can't launch (DLLs land in runfiles subdirs; loose CRT → R6034). This stages the exe + runtime DLLs + VC90 CRT + an external `<exe>.manifest` into ONE flat dir (the test analog of `//main/idl:svidl_bundle`). |
| `libtest` | [//main/test:test](BUILD.bazel) | `test.dll` — `test::OfficeConnection` + arg/url helpers, for *subsequent* (UNO) tests that bootstrap a running soffice over URP. Built; not yet exercised. |
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
2. **Subsequent / UNO tests** — use `test::OfficeConnection` to launch a real
   soffice. Need the staged install wired as a fixture. **Not yet wired.**

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

## The sal suite is deliberately NOT a green gate

`//main/sal:sal_tests` runs **every** migrated self-contained sal/qa test — 36
targets, passing and failing alike, on the principle that failures are
information, not something to hide (the rationale lives next to the
`test_suite` in [main/sal/BUILD.bazel](../sal/BUILD.bazel)). So expect it to be
red. As of 2026-08-01, 30 pass and these 6 fail, each on its **own merits** —
none is a build or loader problem, and the source is out of scope:

| Target | Failing | Why |
| ------ | ------- | --- |
| `osl_File` | 10 | Assert on a specific drive topology (CD/floppy/RAM disk) absent here |
| `osl_Module` | 7 | Loads a module by path; depends on the dmake `solver/bin` layout |
| `tcwf` | 1 | `osl_writeFile` to a fixed path — permission bound |
| `rtl_logfile` | 1 | Writes/reads `c:/temp` and asserts on it — env/permission bound |
| `osl_Thread` | 1 | `resume_001` is a timing race; flaky, not deterministic |
| `rtl_OUString2` | 1 | `convertFromString` expects `\x80` to fail UTF-8 validation — test-data drift, same class as `rtl_textcvt` |

Earlier revisions of this file listed `rtl_str`/`rtl_ustr`/`rtl_string` as
NULL-deref crashes; those were since fixed (boundary checks in the tests plus
`NULL` guards at the rtl entry points) and now pass.

(`osl_Security` is not in the suite at all — it fails to *build* on testshl2,
see the gotcha above; it is unwired entirely, not an exclusion.)

Still unwired: cppunit suites (`osl/socket`, `rtl_strings`) → need a CppUnit
external dep; child-process tests (`osl/process`, `rtl/bootstrap`,
`rtl/process`) → they resolve their helper exe via `getExecutablePath()`+`"/../bin"`
(the dmake `solver/bin` layout), which flat Bazel staging can't satisfy without
source changes — `gtest_test` has a `companions` hook ready for when that layout
is reproduced.
