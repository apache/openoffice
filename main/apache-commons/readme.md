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


# apache-commons — Bazel migration notes

Four Apache Commons jars, as AOO's apache-commons module delivered them, under
their versioned names: `commons-codec-1.9.jar`, `commons-httpclient-3.1.jar`,
`commons-lang3-3.3.jar`, `commons-logging-1.1.3.jar`.  Migrated 2026-10-03.
Their one consumer is the Wiki Publisher extension (`//main/swext`), which
ships them next to `mediawiki.jar` and names them on its `Class-Path` — the
filenames are the lookup key.

The legacy dmake module lives on under `ext_libraries/modules/apache-commons`.
The classes come from four bzlmod modules, one per tarball already in
`ext_sources/` (MD5s match upstream's `TARFILE_MD5`): `@commons-codec`,
`@commons-httpclient`, `@commons-lang3`, `@commons-logging`.  This package only
merges classes + `META-INF` and stamps the manifest (`uno_jar`).

## Verified against the dmake jars

`main/solver/4117/wntmsci12.pro/bin/*.jar`: codec (219), httpclient (169) and
logging (28) are **entry-identical**; codec's resources byte-identical.
lang3 lacks only twelve `package-info.class` entries — ~135-byte empty classes
that Ant's `createMissingPackageInfoClass` synthesizes for packages whose
`package-info.java` has no annotations.  javac emits nothing there; they have
no runtime meaning, so they are not imitated.

## How each mirrors upstream

- **codec** — build.xml `jar`: `src/main/java` + resources from
  `src/main/resources` (Beider-Morse tables) and the `package.html`s.
  DIVERGENCE: upstream's build.xml copies `LICENSE.txt` to both
  `META-INF/LICENSE.txt` and `META-INF/NOTICE.txt`; AOO's `patches/codec.patch`
  fixes that but was never in `PATCH_FILES`.  The real `NOTICE.txt` ships here.
- **httpclient** — `dist`: `src/java` only; manifest from `src/conf/MANIFEST.MF`.
- **lang3** — `jar`: `src/main/java`.
- **logging** — `compile build-jar`.  The log4j, Avalon and LogKit adapters are
  not built (upstream had none of those libraries on the class path, so its
  build.xml skipped them).  `ServletContextCleaner` IS built, against the
  servlet API from `@tomcat` as a neverlink dependency — upstream likewise
  compiled it against main/tomcat's `servlet-api.jar` and never shipped that.

Manifests carry upstream's identification attributes; omitted on purpose are
`X-Compile-Source/Target-JDK` (they would misstate `--release 8`) and logging's
OSGi bundle headers.

## Landmine — source encoding

Bazel's JavaBuilder compiles UTF-8 and overrides `-encoding`.  Two files are
Latin-1 (`lang3 .../translate/EntityArrays.java`, httpclient
`HttpContentTooLargeException.java`), in COMMENTS only, and are transcoded in
the overlay like @saxon's.  Editing an overlay file means regenerating its hash
in `source.json` and `bazel mod deps --lockfile_mode=refresh`.

## @tomcat

The registry entry existed but was unusable (module named `apache-tomcat` in a
`tomcat/` directory, remote URL, `build_file` form, a stray lockfile).  It is
now the usual overlay form and provides only `servlet-api` /
`servlet-api-neverlink` — all AOO ever took from Tomcat (main/tomcat built
`servlet-api.jar` for commons-logging and nothing else).
