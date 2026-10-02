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


# saxon — Bazel migration notes

Saxon-B 9.0.0.7, the XSLT 2.0 processor behind the Java XSLT filter.
Migrated 2026-10-02.  The classes are built in the `@saxon` bzlmod module
(`ext_libraries/modules/saxon/9.0.0.7`) from the tarball already in
`ext_sources/` (MD5 matches upstream's `TARFILE_MD5`); this package assembles
`saxon9.jar` and is staged to `program/classes/`.

Verified against the dmake-built `saxon9.jar`: **identical 1046-entry list**,
and the Latin-1 string constants (month/day names in `Numberer_de/fr/sv`…)
byte-identical.

## How it mirrors upstream's Ant build

AOO's patch only *adds* `build.xml`; the sources are untouched.  Its
`compile-bj` + `jar-bj` targets reduce to:

- **compile set** = `net/**` minus the optional bindings (`ant`, `dom`,
  `dom4j`, `dotnet`, `javax`, `jdom`, `s9api`, `sql`, `xom`, `xpath`, `xqj`).
- **the `/*DOTNETONLY*/` → `//` replace** in `Configuration.java` is a no-op
  for 9.0.0.7 (the token does not occur), so nothing reproduces it.
- **manifest**: `Project-Name: Saxon-B`, `Main-Class: net.sf.saxon.Transform`.
- **`META-INF/services/javax.xml.transform.TransformerFactory`** =
  `net.sf.saxon.TransformerFactoryImpl`.  This is what makes JAXP return Saxon
  wherever `saxon9.jar` is on the class loader — and it reaches every jar that
  names `saxon9.jar` in its `Class-Path` (`XSLTFilter.jar`, `commonwizards.jar`).
- No stax jar: `pull/PullToStax` and `pull/StaxBridge` compile against the
  JDK's `javax.xml.stream` (stax is Dropped, see the frontier).

## Landmine — source encoding

Upstream compiles with `encoding="ISO-8859-1"`, and ten files really are
Latin-1 — in the `Numberer_*` ones inside **string literals**, so the bytes are
behaviour.  Bazel's JavaBuilder compiles UTF-8 and **overrides `-encoding` in
`javacopts`**, so passing upstream's flag does nothing.  Those ten are
transcoded `iconv -f ISO-8859-1 -t UTF-8` in the overlay and the originals
excluded.  The list is exhaustive (every other source is ASCII); a new
non-ASCII file fails loudly as "unmappable character".

Editing the overlay `BUILD.bazel` means regenerating its sha256 in
`source.json` and running `bazel mod deps --lockfile_mode=refresh` — a wrong
overlay hash is silently ignored.
