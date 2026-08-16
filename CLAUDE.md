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

# Apache OpenOffice

Agent context lives in [`.agent/`](.agent/README.md), split so that branches
cannot collide. This file is a loader: it is byte-identical on every branch and
should not be edited to record branch- or migration-specific facts.

## Always applies

@.agent/common/00-project.md
@.agent/common/10-toolchain.md
@.agent/common/20-build-conventions.md
@.agent/common/30-workflow.md
@.agent/common/40-debugging.md

## Branch context — read this first

Run `git branch --show-current`, then read `.agent/branches/<branch>.md` if it
exists. That file states what the branch is for and may deliberately narrow
scope; where it disagrees with the migration notes below, **it wins**.

If there is no file for the current branch, the branch inherits the shared
context above and nothing else.

## Migration state

`bazel-migration` and its topic branches share one migration record. Read the
part you need rather than all of it — the frontier alone is ~885 lines:

- [`.agent/migration/frontier.md`](.agent/migration/frontier.md) — per-module
  state: what is migrated, what is in flight, and the landmines each cost.
- [`.agent/migration/backports.md`](.agent/migration/backports.md) — trunk
  backports: what was taken, what was deliberately not, and why.
- [`.agent/migration/win64.md`](.agent/migration/win64.md) — the x86-64 build:
  what still needs re-exercising on x64, and the four real defects the port
  turned up.
- [`.agent/migration/localization.md`](.agent/migration/localization.md) — the
  three language axes and the missing Pootle→SDF rule.
- [`.agent/migration/dependencies.md`](.agent/migration/dependencies.md) —
  where each third-party dependency's notes live.

[`STYLEGUIDE.md`](STYLEGUIDE.md) is the BUILD-authoring reference; the
copy-pasteable patterns are there, the operating rules are in `.agent/common/`.
