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

# Project — goal, environment, scope

## Goal

Replace the Perl/dmake/gmake orchestration layer with Bazel.
Primary constraint: eliminate the Cygwin dependency from the build.
Source code is NOT being changed — only the build system.

## Environment

- OS: Windows 11
- Shell during migration work: bash (via Cygwin, being phased out)
- Compiler: MSVC VS2008 (VC9) — both **x86** and **x64** (VC9 cross-tools); configured via user.bazelrc
- Bazel configured via MODULE.bazel and user.bazelrc

## Build strategy

- Third-party deps: wrap with rules_foreign_cc (cmake/make as appropriate)
- First-party modules: migrate to native Bazel cc_library/cc_binary
- Goal: eliminate gmake and dmake from first-party builds entirely
- rules_foreign_cc is a bridge for external code, not a destination

## External dependencies are linked SHARED

An external library is reached through an import library and shipped as its own
DLL.  It is never statically bound into a product binary.  Static linking is
acceptable only for a build-time tool that never ships and must be
self-contained at build-action time — ICU's `genbrk`/`gencmn`/`genccode` are the
worked example.

Two reasons, and the second is the one that bites later:

- **A dependency we do not maintain must stay replaceable.**  Swapping one for a
  security fix, a newer version, or a distro's own copy should not mean
  relinking the product.
- **Distribution.**  A Linux packager needs `--with-system-<lib>`, and that is a
  PROVENANCE choice — expressible only if the consumer links against an
  interface rather than absorbing the implementation.  It is what the
  `//build/config:*_system` branch of `//build/deps` exists for.  A statically
  bound dependency has no such seam.

Static linking also pulls AOO-side knowledge of how the dependency was built
into our BUILD files — the `*_STATIC` / `*_INTERNAL` define families — and that
coupling is what makes an AOO-specific patch feel cheap and a version bump
expensive.  A registry that offers only static targets (the BCR `icu` module is
static-only and propagates `U_STATIC_IMPLEMENTATION` on MSVC) is therefore a
dependency we cannot consume yet, not a reason to go static.

**This describes the direction, not the tree.**  Measured 2026-08-30: of the 36
modules in `ext_libraries/modules`, four build a shared library — icu (3),
nss (8), coinmp (1), python (1) — and the rest are static.  Every module touched
for another reason is an opportunity to move one more.

## Out of scope

- Modifying source code
