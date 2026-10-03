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


# xsltml — Bazel migration notes

The XSLT MathML Library 2.1.2: seven MathML → LaTeX stylesheets, shipped as
`filter/math/` inside the Wiki Publisher extension (`//main/swext`), whose
`odt2mediawiki.xsl` includes `math/mmltex.xsl` to turn formulas into `<math>`
markup.  Migrated 2026-10-03; `//main/xsltml:xsltml` aliases `@xsltml//:xsltml`.

upstream (makefile.mk): unpack the flat zip, convert line ends
(`CONVERTFILES`), apply `xsltml_2.1.2.patch`, deliver the `*.xsl`.

**The overlay carries the seven patched files, not the patch.**  The patch
touches every delivered file, and a multi-file bzlmod patch can silently drop a
hunk (the recorded last-file bug).  The overlay files are dmake's output byte
for byte (CRLF, as `CONVERTFILES` leaves them) and identical to `filter/math/`
in the dmake-built `wiki-publisher.oxt`.  Cross-checked independently: the
pristine zip, LF-converted and GNU-patched with `-l`, differs from them only
in line ends.  Verified live: a formula exported through the MediaWiki filter
comes out as `<math>{a}^{2}+{b}^{2}={c}^{2}</math>`.
