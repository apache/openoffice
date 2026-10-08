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

# Writer Extensions

## Bazel migration notes

The Wiki Publisher extension, `wiki-publisher.oxt`, migrated 2026-10-03:
`bazel build //main/swext:wiki-publisher`.  It is a **deliverable, not part
of the install** -- nothing in scp2 ships it, upstream distributes it
separately, and a user adds it with the Extension Manager or `unopkg add`.

Upstream builds it in three layers, all reproduced in `BUILD.bazel`:

| upstream | Bazel |
|---|---|
| `mediawiki/build.xml` (Ant): javac, `mediawiki.jar`, `uno-package` zip | `mediawiki_classes` + `uno_jar` → `mediawiki.jar`; `oxt_package` |
| `mediawiki/src/registry/**/makefile.mk` (tg_config.mk): `schema_trim.xsl`, `alllang.xsl` | `wikiextension_xcs` genrule; `xcu_default` |
| `mediawiki/help/makefile.mk` (extension_helplink.mk): sed + HelpLinker extension mode + HelpIndexerTool | `ext_files` with substitutions; `help_extension` |
| Ant `<filterset>` (`@WIKIEXTENSIONPRODUCTNAME@` etc.) | `ext_files(substitutions = TOKENS)` |

New shared rules: `//build/rules:extension.bzl` (`ext_files`, `xcu_default`,
`oxt_package`, all keyed by **archive path** so nothing is collected by walking
a directory) and `help_extension` in `//build/rules:help_pipeline.bzl`.

Dependencies, each its own package: the four Apache Commons jars under their
delivered names (`//main/apache-commons`, built from the `@commons-*` bzlmod
modules; `@tomcat` supplies the servlet API to compile commons-logging) and the
MathML stylesheets (`//main/xsltml`, `@xsltml`).

### Verified

- **Against the dmake-built `.oxt`** (`wntmsci12.pro/bin/wiki-publisher.oxt`):
  identical 59-entry list.  The five processed XCUs are **byte-identical** to
  dmake's en-US output (`misc/mediawiki/registry/data`) once the tokens are
  applied; `filter/math/*`, `description.xml`, `help.db_`, the template and
  `THIRDPARTYLICENSEREADME.html` are byte-identical in the archive.  The other
  differences are explained: the oracle was built from an OLDER source revision
  (trailing-whitespace and typo cleanups since, LICENSE year), its configuration
  carries every language (cfgex merge, needs `WITH_LANG`), its pages are
  helpex-reformatted (`.ht_`/`.key_` hold the same records in another order),
  Lucene's `segments_2` is time-seeded, and the jars are stored uncompressed by
  singlejar.  `mediawiki.jar` and three of the Commons jars are entry-identical.
- **Live, x86 and x64**: `unopkg add` installs it (all 11 items "is registered:
  yes"); in a running office `com.sun.star.wiki.WikiEditor` instantiates from
  the extension's jar, and storing a Writer document with `FilterName=MediaWiki`
  writes correct markup -- heading, bold, a table, and a formula as
  `<math>{a}^{2}+{b}^{2}={c}^{2}</math>`, which proves `filter/math/` is reached.
  The HTTP stack was exercised outside the office: a client with only the
  INSTALLED `mediawiki.jar` on the class path did an httpclient GET (200) and
  used codec and lang3, i.e. the manifest `Class-Path` resolves all four jars.
  Publishing to a real MediaWiki server was not tried.

### Decisions and divergences

- **en-US configuration = `alllang.xsl` default output, not the cfgex merge.**
  Ant's tmpdir step takes `misc/<id>/merge/` when it exists, which needs
  `WITH_LANG`; in an en-US build its xcumerge step fills `merge/` from
  `registry/data`, i.e. exactly what `xcu_default` produces.
- **TypeDetection `Filter.xcu`/`Types.xcu` are the SOURCE files**, as upstream:
  build.xml says the processed ones lose the `x-default` tag (issue 99378).
- **Help in two languages, en-US and en**, as upstream (`uniq en
  $(alllangiso)`): the extension manager falls back from en-GB etc. to `en`,
  never to en-US.  The `en` pages are the en-US text with `xml-lang="en-US"`
  rewritten to `xml-lang="en"` -- what helpex does for an untranslated language.
- **No `UNO-Type-Path` in `mediawiki.jar`'s manifest**, as upstream.  Per the
  wizards finding an ABSENT header makes `UnoClassLoader` add the jar itself to
  the shared UNO class loader; that is upstream's behaviour for this extension,
  and reproducing upstream's manifest was preferred over "fixing" it here.
- **`mediawiki_develop.zip` (Ant `development-package`) is not built**: nothing
  in the tree consumes it.
- Installing needs the license accepted: upstream's `suppress-on-update="true"`
  means `unopkg add -s` does NOT skip it on a first install.  Non-interactively,
  feed `yes` on stdin **as UTF-16LE** -- `dp_misc::readConsole()` reads raw
  wide characters (it expects `unopkg.com` in front); an 8-bit `yes` is never
  accepted and the prompt loops, writing hundreds of MB.

### Defects found in the staged install while verifying (fixed)

Each one alone made any extension with a Java component unusable:

1. `program/unopkg.exe` died at load (0xC0000135): linked `/MANIFEST:NO` with
   no external manifest.  Now carries the VC90 manifest at RT_MANIFEST id 1.
2. `program/version.ini` did not exist, so `OOOPackageVersion` was empty and
   every extension with a version dependency was refused ("unsatisfied
   dependencies").  Now generated from MODULE.bazel's version.
3. `program/uno.exe` was not staged; the extension manager starts it to
   register an actively-registered component ("unknown error!" from
   `raiseProcess`).  Now staged, with an embedded manifest.
4. `fundamental.ini` never chained the extension layers' `unorc` into
   `URE_MORE_SERVICES`/`_TYPES`/`_JAVA_TYPES`: extensions installed and their
   configuration applied, but their services returned null.  Fixed to
   upstream's chain -- and `UNO_TYPES`/`UNO_SERVICES` moved out of
   `fundamental.ini` (to `uno.ini` only, as upstream), because a lookup in a
   missing `unorc` falls back to `fundamental.ini` and would recurse into a
   stack overflow at startup.

Since 2026-10-08 `program/` has upstream's three files -- `unopkg.com` ->
`unopkg.exe` (guiloader) -> `unopkg.bin` -- so `echo yes| unopkg.com add
wiki-publisher.oxt` accepts the license with plain 8-bit input (verified on all
three configs).  See main/desktop/readme.md for how the chain works and why
`unopkg.com`'s output cannot be redirected.

### Testing it

Headless with a fresh `-env:UserInstallation` needs `-nofirststartwizard`;
without it the office exits 0 within seconds (the first-start wizard cannot
run headless).  The default profile, whose first start was completed long ago,
does not need it -- which is what makes this look like an extension problem.
