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


# helpcontent2 — Bazel migration notes

The office's F1 help, en-US.  Migrated 2026-10-03.  `//main/helpcontent2:help`
produces `help/en/` (staged by `//main/staging:_install_help`): for each of the
9 modules (sbasic scalc schart sdatabase sdraw shared simpress smath swriter)

| File | Made by | Used for |
|---|---|---|
| `<m>.db` `<m>.ht` `<m>.key` | HelpLinker | page lookup (id → jar + path + title), help-text tooltips, keyword index |
| `<m>.jar` | the pages, zipped | the page sources the viewer renders |
| `<m>.cfg` | `source/auxiliary/en-US` | module settings |
| `<m>.tree` | `helpers/update_tree.pl` | the Contents tab |
| `<m>.idxl/` | HelpIndexerTool (Lucene) | the Find tab |

plus shared's stylesheets and `err.html`; `help/` itself gets the viewer's
three stylesheets from `//main/xmlhelp:helpxsl`.

**Verified in a running office, x86 and x64:** pages render through the help
UCB (`vnd.sun.star.help://swriter/text%2Fswriter%2Fmain0000.xhp?...`), full-text
and heading search return hits through LuceneHelpWrapper, and opening a hit
renders it.  Page ids are URL-ENCODED (`text%2Fswriter%2F...`) — as the `.db`
keys and the search hits are; an id with raw slashes finds nothing, and a page
addressed under module `shared` finds nothing (shared links no pages; shared
pages are reached through the application modules).

## Pipeline

`build/rules/help_pipeline.bzl` (rules) + `build/rules/help_link.pl` (driver).

1. `help_xhp_tree` re-roots the pages as `xhp/en-US/text/...`.  HelpLinker
   resolves every page **and every cross-module embed** under
   `<src>/<lang>/`, so each module's link sees the whole tree.  For en-US
   upstream's "merge" (helpex) is a plain copy, so this is only re-rooting.
2. `help_trees` runs upstream's `update_tree.pl` once: each topic title is
   replaced by the page's real `<title>`, and topics whose page is gone are
   commented out (`<!-- removed ... -->`).  It writes to
   `$ENVPRJ/$INPATH/misc/en-US/`, so `INPATH` is a relative path from the
   package into the output tree.
3. `help_module` (one per module): zip the jar, run HelpLinker, run
   HelpIndexerTool, then copy out **exactly the declared files** — the driver
   fails on any undeclared product or any missing one, so a change in either
   tool's output is a loud error, not a quietly incomplete help directory.
   Everything happens in a private work dir: HelpLinker writes `caption/` and
   `content/` into its `-zipdir`, and modules sharing one would race.

Which pages each module links (`LINKLINKFILES`), what it adds
(`LINKADDEDFILES`), what its jar holds (`ZIP1LIST`) and which pages exist at
all (`XHPFILES` of every `source/text/**/makefile.mk`) are upstream's lists,
verbatim, generated into `help_modules.bzl` by `gen_help_modules.py` — rerun it
after changing any of those makefiles.

## Landmines

- **The page set is `XHPFILES`, not a glob.**  Two pages exist on disk that no
  makefile lists (`shared/guide/error_report.xhp`,
  `shared/optionen/improvement.xhp`); upstream's merge never delivers them, so
  they are in no jar and not in HelpLinker's tree.
- **A jar is `ZIP1LIST`, not always `text/<m>/*`.**  sdatabase's jar holds ONE
  page, `text/shared/explorer/database/main.xhp`.  Directory entries exist only
  below the glob root (`zip -r text/<m>/*` never adds `text/` or `text/<m>/`).
- **Never walk the output tree.**  The driver zips an explicit page list:
  Bazel does not delete outputs a rule stopped declaring, so a walk of the
  `xhp/` tree picked up pages from an earlier build.
- **`-extension` is the indexer flag to use, but not for a module that links
  nothing.**  The flag only changes the epilogue — index in place and delete
  `caption/`+`content/` instead of zipping — the index itself is identical.
  For an empty module, upstream's zip mode DELETES the empty index (no
  `shared.idxl`), while `-extension` would keep `segments.gen`+`segments_1`;
  so the indexer is simply not run there.
- **Lucene's file set is fixed:** `_0.cfs _0.cfx segments.gen segments_2` for
  every linking module (create + optimize + close), verified against the
  dmake output on both arches.  It is declared, so a Lucene change fails loudly.
- **HelpLinker runs as the EXEC platform (x64) even for an x86 build.**
  Harmless: the `.db`/`.ht`/`.key` format is text with hex lengths; the x86 and
  x64 builds produce byte-identical files.
- **Reproducibility:** everything is byte-identical across builds except each
  index's `segments_2`, where Lucene 2.9 seeds `SegmentInfos.version` from
  `System.currentTimeMillis()` (not fixable without patching Lucene).  Jar
  member times are pinned to the DOS epoch by setting the DOS field directly —
  `setLastModFileDateTimeFromUnix` goes through LOCAL time, i.e. the build
  machine's time zone.
- **The dmake oracle is an OLDER source revision** (solver/4117 = AOO41X).
  Byte comparison fails for content-dependent files purely from source drift:
  commit eac72b6ce8 stripped trailing whitespace from every `.xhp`, and
  381b0f8e4c renamed Impress's taskpanel.xhp to sidebar.xhp.  What does
  match: the 89-file set, every jar's entry names (bar that rename), the
  `.cfg` files byte for byte, and the `.tree` files up to those trailing spaces.
