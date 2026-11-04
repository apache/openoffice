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

Two Apache Commons jars, as AOO's apache-commons module delivers them, under
their versioned names: `commons-lang3-3.3.jar` and `commons-logging-1.1.3.jar`.
Migrated 2026-10-03 with four; trunk 33efb51f76 (2026-11-03 here) dropped
`commons-codec-1.9.jar` and `commons-httpclient-3.1.jar`, because the Wiki
Publisher (`//main/swext`), their consumer until then, now talks to the
MediaWiki API over the JDK's own HTTP stack and bundles no Commons jar at all.
What is left is built upstream for the report builder (`ENABLE_REPORTBUILDER`),
which is not migrated yet — so nothing in this tree consumes these jars today.

The legacy dmake module lives on under `ext_libraries/modules/apache-commons`
(trunk's change applied there too).  The classes come from two bzlmod modules,
one per tarball already in `ext_sources/` (MD5s match upstream's
`TARFILE_MD5`): `@commons-lang3`, `@commons-logging`.  This package only merges
classes + `META-INF` and stamps the manifest (`uno_jar`).

## Verified against the dmake jars

`main/solver/4117/wntmsci12.pro/bin/*.jar` (measured with all four; codec and
httpclient are gone since): logging (28) was **entry-identical** — one fewer
now that `ServletContextCleaner` is not built.
lang3 lacks only twelve `package-info.class` entries — ~135-byte empty classes
that Ant's `createMissingPackageInfoClass` synthesizes for packages whose
`package-info.java` has no annotations.  javac emits nothing there; they have
no runtime meaning, so they are not imitated.

## How each mirrors upstream

- **lang3** — `jar`: `src/main/java`.
- **logging** — `compile build-jar`.  The log4j, Avalon and LogKit adapters are
  not built (upstream had none of those libraries on the class path, so its
  build.xml skipped them).  Nor is `ServletContextCleaner`: trunk's
  `patches/logging.patch` excludes it, since it was the only reason Tomcat was
  built (for its servlet API), and Tomcat is gone.

Manifests carry upstream's identification attributes; omitted on purpose are
`X-Compile-Source/Target-JDK` (they would misstate `--release 8`) and logging's
OSGi bundle headers.

## Landmine — source encoding

Bazel's JavaBuilder compiles UTF-8 and overrides `-encoding`.  One file is
Latin-1 (`lang3 .../translate/EntityArrays.java`), in COMMENTS only, and is
transcoded in the overlay like @saxon's.  Editing an overlay file means
regenerating its hash in `source.json` and
`bazel mod deps --lockfile_mode=refresh`.
