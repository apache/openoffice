# C++ unit-test infrastructure — Bazel migration

AOO's C++ tests are **GoogleTest 1.7.0**-based (not CppUnit, apart from a few
holdouts).  This brings the test layer onto Bazel so suites run under
`bazel test`.

## Pieces

| Piece | Path | Role |
|-------|------|------|
| `@gtest` | `ext_libraries/modules/gtest/1.7.0/` | GoogleTest 1.7.0 bzlmod wrap (zip cached in `ext_sources`). Built with `/Zc:wchar_t-` so its `wchar_t` ABI matches `sal_Unicode` test code. |
| `gtest_test` rule | [//build/rules:gtest_test.bzl](../../build/rules/gtest_test.bzl) | Reusable runnable-test rule. The `/MD` toolchain embeds no manifest, so a bare `cc_test` exe can't launch (DLLs land in runfiles subdirs; loose CRT → R6034). This stages the exe + runtime DLLs + VC90 CRT + an external `<exe>.manifest` into ONE flat dir (the test analog of `//main/idl:svidl_bundle`). |
| `libtest` | [//main/test:test](BUILD.bazel) | `test.dll` — `test::OfficeConnection` + arg/url helpers, for *subsequent* (UNO) tests that bootstrap a running soffice over URP. Built; not yet exercised. |
| `sal_qa_test` macro | [//main/sal:sal_qa.bzl](../sal/sal_qa.bzl) | Thin `gtest_test` wrapper for the sal/qa suites (common copts/deps + per-dir `*_Const.h` include). |

## Two test categories

1. **Standalone unit tests** — own `main()`, link sal/lib only, no office.
   Run directly via `gtest_test`. Examples:
   - `//main/o3tl:o3tl_test`
   - `//main/tools:tools_pathutils`
   - `//main/sal:sal_tests` (green gate, 22 targets)
2. **Subsequent / UNO tests** — use `test::OfficeConnection` to launch a real
   soffice. Need the staged install wired as a fixture. **Not yet wired.**

## Gotchas (learned the hard way)

- **`/Zc:wchar_t-` must be consistent across gtest and every test TU.** gtest
  declares `PrintTo(wchar_t)`; with `/Zc:wchar_t-` (`wchar_t == unsigned short`)
  on one side only, that mangles vs `PrintTo(unsigned short)` → `LNK2019`. Fixed
  in gtest's overlay copts AND baked into the `gtest_test` rule.
- **Editing a registry overlay file requires updating its sha256 in
  `source.json` + `bazel mod deps --lockfile_mode=refresh`.** Otherwise Bazel
  reuses the cached extraction and silently ignores the edit.
- **`$(TESTSHL2LIB)` in the dmake `*STDLIBS` is a no-op** — AOO retired
  `testshl2`; the variable is undefined (expands empty). So those suites are not
  blocked by a missing lib.

## Excluded from the sal green gate

Wired and individually runnable, but kept out of `//main/sal:sal_tests` because
they fail on their own merits (source is out of scope, so not fixed):

- `rtl_str` / `rtl_ustr` / `rtl_string` / `rtl_OUString2` — `*_000` cases pass
  `NULL` into `rtl_*_compare` etc., which dereference it → `0xC0000005`.
- `testHelperFunctions` — `test_t_abs64` computes `(2147483648 << 1)` in 32-bit
  → overflow to 0 (test-logic bug on this platform).
- `rtl_logfile` — writes/reads `c:/temp` and asserts on it (env/permission).
- `rtl_textcvt` — text-conversion / mime / codepage table expectations
  (sal's conversion works in the running app; this is test-data drift).
- `osl_Security` — Win32 SID/registry APIs (needs `<windows.h>` + advapi32).

Also deferred: cppunit suites (`osl/socket`, `rtl_strings`) → need a CppUnit
external dep; child-process tests (`osl/process`, `rtl/bootstrap`, `rtl/process`)
→ need helper-exe staging; resource/IPC tests (`osl/file`, `osl/module`,
`osl/pipe`); `systools/test_comtools` (COM init).
