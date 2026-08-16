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

# `.agent/` — agent context, split so branches cannot collide

Tool-neutral. Nothing reads this directory natively; the root `CLAUDE.md` and
`AGENTS.md` are thin pointers into it, which is what keeps the content itself
free of any one vendor's conventions.

## Why it is split this way

Everything used to live in one root `CLAUDE.md`, and every topic branch
replaced or forked that file. That single shared path was the cause of
essentially every rebase conflict on this repo: of eleven branches rebased on
2026-08-16, the nine carrying their own `CLAUDE.md` all conflicted, and the two
that left it alone (`sec/aoo-2026-001`, `sec/aoo-2026-006`) rebased clean.

Splitting one file into several does **not** fix that on its own — if two
branches both write `.agent/branches/context.md`, the conflict has only moved.
What fixes it is **path ownership**: the branch file is named after the branch,
so a topic branch's diff touches a path `bazel-migration` never writes. A
conflict then isn't unlikely, it's structurally impossible.

## Layout and owners

| Path | Written by | Content |
| --- | --- | --- |
| `common/` | `bazel-migration` only | True on every branch: goal, toolchain, BUILD conventions, workflow, debugging |
| `migration/` | `bazel-migration` only | Migration state: frontier, trunk backports, localization, dependency notes |
| `branches/<branch>.md` | that branch **only** | What this branch is for, and its working notes |

## The contract

1. A topic branch may add and edit exactly one tracked context file:
   `.agent/branches/<its-own-branch-name>.md`. Never any other branch's.
2. `common/` and `migration/` change **only** on `bazel-migration`. A topic
   branch that wants to correct them lands that fix on `bazel-migration` (or
   proposes it), so every branch gets it on the next rebase.
3. `CLAUDE.md` and `AGENTS.md` are frozen loaders. They are byte-identical on
   every branch and should essentially never change — that is what stops the
   entry point itself from becoming the new conflict.
4. A branch file is free to say "ignore the migration frontier, this branch is
   only about X". Narrowing context is the point; several branches deliberately
   did that before, and this preserves it without forking the shared docs.

## Adding a branch

Create `.agent/branches/<branch>.md` stating the goal, the scope boundaries,
and which of `migration/` (if any) is worth reading. Nothing else needs editing
— the loader finds it by branch name.
