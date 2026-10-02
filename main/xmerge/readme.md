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


# xmerge — Bazel migration notes

The Java document-conversion framework behind the Palm / Pocket PC filters
(AportisDoc, Pocket Word, Pocket Excel).  Migrated 2026-10-02 from the
`Ant_*.mk` gbuild targets.

| Jar | Role |
|---|---|
| `xmerge.jar` | the framework (plugin registry, DOM/merge utilities) |
| `XMergeBridge.jar` | UNO component `com.sun.star.documentconversion.XMergeBridge`, registered in services.rdb |
| `aportisdoc.jar`, `pexcel.jar`, `pocketword.jar` | the plugins, each with `META-INF/converter.xml` |

All five go to `program/classes/`.  Verified against the dmake jars: identical
entry lists and manifests for all five.  **Exercised in a running office**
(2026-10-02, x86): a Writer document stored through "PocketWord File" produced
a valid `.psw`.

## Why the location is fixed

- `XMergeBridge.jar` names `xmerge.jar` by relative `Class-Path`.
- The plugins are on NO class path.  `XMergeBridge` builds
  `$(progurl)/` + `UserData[1]` (via `com.sun.star.config.SpecialConfigManager`,
  svl) and the filter configs say `classes/<plugin>.jar`, so the filter
  configuration decides where they live.  `ConverterInfoReader` then opens
  `jar:<url>!/META-INF/converter.xml`.

## Why this mattered

The palm / pocketword / pocketexcel `.xcd` packs were staged long before this,
so those filters were offered in the UI with no `XMergeBridge` behind them.
`MiniCalc (Palm)` has a filter fragment naming `classes/minicalc.jar`, but it
is not in any packed `.xcd` and upstream ships no `minicalc.jar` — consistent.

## Not built (matching scp2 `file_javafilter.scp`)

`htmlsoff` (built upstream, never installed), the javadoc target,
`source/minicalc` + `source/wordsmith` (not in `Module_xmerge.mk`), and
`xmergesync.dll` — a prebuilt 32-bit ActiveSync binary checked in under
`source/activesync/BIN`, for syncing with a Windows CE device.

`xmerge.jar`'s `Class-Path` names `xml-apis.jar xercesImpl.jar serializer.jar`,
which AOO never shipped either; the JVM skips missing entries and the
framework runs on the JDK's JAXP.
