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

# `bazel-migration` — replace the Perl/dmake/gmake layer with Bazel

The trunk of this work. Every other branch in `.agent/branches/` is based on
this one, and this is the only branch that writes `.agent/common/` and
`.agent/migration/`.

## Goal

Replace the Perl/dmake/gmake orchestration layer with Bazel. The primary
constraint is eliminating the Cygwin dependency from the build. **Source code is
not being changed — only the build system.** Where a source bug genuinely blocks
a build, raise it rather than silently patching it (see the latent-UAF and
`jni_uno` precedents in the frontier).

The 2026-06-20 demo is done; the goal is now the full migration. Buckets in the
frontier labelled "Remaining" are in-scope work, not abandoned — only "Out of
scope" and "Dropped" are excluded.

## What to read

- [`../migration/frontier.md`](../migration/frontier.md) — the working
  document. Per-module state, ordered by priority, and the landmine notes that
  each cost a session to learn. Start here.
- [`../migration/backports.md`](../migration/backports.md) — before porting
  anything from trunk. Records the `git cherry` method and, importantly, which
  commits must **not** be re-applied.
- [`../migration/localization.md`](../migration/localization.md) — only when
  touching languages; the three axes are independent and conflating them is the
  trap.
- [`../migration/dependencies.md`](../migration/dependencies.md) — pointers to
  each third-party dependency's own notes.

## Working rules specific to this branch

- Update the frontier after a build succeeds, not before — see
  [`../common/30-workflow.md`](../common/30-workflow.md).
- A migrated module gets a `readme.md` summarising its migration, and a
  one-line pointer in `MEMORY.md`. That pair is what makes the next similar
  module fast.
- Landing a fix that every branch needs (a `common/` or `migration/`
  correction, a shared rule in `build/rules/`) belongs here, so the topic
  branches inherit it on their next rebase.
