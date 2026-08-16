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

# Win64 — the x86-64 build alongside x86

Folded in from the `win64` branch 2026-08-16, which was docs-only by then: the
build-system work it tracked had already landed here, and the branch's record
was the only thing left on it.

The x64 build **works**: `//main/staging:install` builds and stages, and x64
`soffice.exe` boots to the Start Center (2026-07-25). Both arches were
user-verified green as recently as 2026-08-14 (embedded database). x86 remains
the default and must stay green.

The *how* is in [`../common/10-toolchain.md`](../common/10-toolchain.md) — dual
toolchain, platform/constraint model, per-arch defines. This file is the
migration **state**: what the port cost, and what is still open.

## Still open

- **Third-party externals, the long tail.** Built and proven in the x64 staging
  closure: expat, boost (`BOOST_MEM_FN_ENABLE_CDECL` is x86-only), icu (native
  cc, needed nothing), nss/nspr (`nss_win.patch` +2 hunks + `NSS_USE_64`),
  coinmp (dropped a stale `_X86_=1`). **Not** re-exercised on x64, so revisit
  per library when a consumer needs one: openssl (`VC-WIN64A`, `-x64` lib
  suffix), curl (`_debug` suffix on amd64), mariadb, redland/raptor/rasqal,
  zlib/libxml2/libxslt.
- **Exercise the running x64 app** beyond the Start Center — open Writer and
  Calc documents.
- **Build-file audit, not a port** (from `backports.md`): crashrep must link
  psapi on x64, OpenSSL lib names carry an `-x64` suffix, coinmp x64 build
  flags, the NSS/NSPR win64 patch.

## What the port actually cost — four real defects, not plumbing

Recorded because each was invisible until x64 ran, and each took a session:

1. **`sal` `oslThreadKey`** and **vcl saldata**: `SetWindowLongPtr` /
   `WNDEXTRA` sized by `sizeof(ULONG_PTR)` — a frame pointer stored in
   window-extra-bytes was being truncated.
2. **`ImplSendMessage` `BOOL`→`LRESULT`**: `SAL_MSG_CREATEFRAME` /
   `SAL_MSG_CREATEOBJECT` return the new `SalFrame*` / `SalObject*` as the
   message `LRESULT`; `BOOL` truncated the pointer, giving an AV in
   `SalFrame::SetCallback`.
3. **The x64 bridge was genuinely broken** — the round-trip test had never run
   a generated snippet, so nothing caught it. Adopted `origin/windows-amd64`'s
   `call.asm` + `cpp2uno.cxx`: `codeSnippet` had wrong opcodes (`0x49` vs
   `0x89` for the rcx/rdx home-stores → garbage `this`), and
   `callVirtualMethod` used `.ALLOCSTACK` with no frame register so C++/SEH
   exceptions could not unwind through it (→ `.SETFRAME rbp`). Separately
   `abi.cxx`'s `return_in_hidden_param` logic was **inverted** (a >8-byte
   struct returned in a register rather than via the hidden pointer → corrupt
   nested-`Sequence` returns, e.g. `getAllExtensions`), and `except.cxx`'s
   `__type_info` carried an extra `_m_name` field, putting the mangled name at
   offset 24 instead of MSVC's 16 → a cross-DLL base-class
   `catch(uno::Exception&)` never matched by name → `getCaughtException`
   rethrow escaped uncaught and soffice exited silently after
   `CheckExtensionDependencies`.
4. **`CPPU_ENV` strip missed subpackages.** The `main/*/BUILD.bazel` glob
   skipped `main/i18npool/pool` and `main/extensions/source/oooimprovement`, so
   `i18npool.dll` compiled `msci` and LocaleData / CharacterClassification
   reported the x86 environment on x64 → "cannot get uno environments"
   FatalError (`env.is=0`).

**Diagnostic lesson from (4)**: the boot debug loop is `cdb` **attach**, not
launch — launching under cdb races the DLL loads, while `-pn` attach plus a
`bu` deferred breakpoint is reliable. A temporary `fprintf`-to-file in
`shlib.cxx`'s failure branch is what pinned `impl` + `srcEnv` + `env.is`
exactly.

## Verifying the x64 ABI

Two targets prove the bridge, and both run on either arch (the compiled
`CPPU_ENV` picks `msci_uno` or `mscx_uno`), so they are the cheap regression
check after any bridge or toolchain change:

- `//main/bridges:cppuno_roundtrip_test` — maps a C++ `XServiceInfo`
  cpp→uno (which loads the bridge DLL), invokes `getImplementationName`
  through the `uno_Interface` **dispatcher** so a real uno→cpp marshalled call
  happens, checks the returned `OUString`, and asserts cpp→uno→cpp identity
  collapse. Going through the dispatcher is the point: a test that calls the
  mapped-back C++ pointer directly proves nothing, which is why the broken x64
  bridge above went unnoticed for so long.
- `//main/bridges:inter_libs_exc_test` — a UNO/C++ exception thrown in one DLL
  and caught in another, i.e. cross-DLL SEH unwind. This is what defect (3)'s
  `.SETFRAME` and `__type_info` halves break.

## Reference

`origin/windows-amd64` (56 commits, dmake-era) is the reference for
source-level Win64 fixes — data-type widths, `SetWindowLongPtr`, `.def`
entrypoints, `_AMD64_`. Cherry-pick from it only where a real gap surfaces;
most of it is already reflected here or upstream.

Win64 bug-fixing (pointer/`long` widths, x64 asm, `.def` name decoration,
16-byte stack alignment and exception unwind in the bridge, external-library
x64 quirks) remains in scope whenever it surfaces.

## Deliberate constraints

- **Stay on VC9 (VS2008) for x64.** The cross-tools ship with VS2008
  (`VC\bin\x86_amd64\cl.exe` + `ml64.exe`, native `VC\bin\amd64\`, SDK v7.0
  `lib\x64`). Compiler modernization is a separate, later step — do not mix a
  newer toolchain in for ABI reasons.
- **Arch is chosen by target platform, not a stringly-typed flag**:
  `--config=winXP-x86` (default) / `--config=winXP-x64`, each pinning
  `--platforms` + a distinct `--platform_suffix` and `--symlink_prefix`, so the
  two output trees never clobber. Sharing one output_base is fine — per-config
  action outputs cache separately, so switching only re-analyses.
