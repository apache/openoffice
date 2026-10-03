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

# dependency notes

icu - ext_libraries\modules\icu_xp\Readme.md  (the 49.1.2 module; the
      modern axis is ext_libraries\modules\icu_modern)
redland / raptor2 / rasqal (RDF stack, native-cc static) + unordf.dll - rdf-readme.md
  (the modules\<lib>\README files are the original autotools docs; the Bazel
   migration notes — overlay mechanism, *_INTERNAL/HAVE_CONFIG_H defines,
   local_defines vs defines, `*_STATIC`, RAPTOR_WWW_NONE/S_ISREG — are in rdf-readme.md)
rhino - ext_libraries\modules\rhino\README
saxon 9.0.0.7 (bzlmod, Java) - main/saxon/readme.md  (Latin-1 sources transcoded)
lucene 2.9.4 (bzlmod, Java) - main/xmlhelp/readme.md  (long_path.patch regenerated)
sal_pch - was merged with sal_headers

## Open question — shared vs static linkage (deferred, 2026-08-31)

Not a rule.  A direction that has been agreed in principle and deliberately NOT
adopted yet, because acting on it now would widen unrelated work.  Recorded here
so the measurement does not have to be redone.

An external library we do not maintain is easier to replace, and possible for a
distributor to substitute, when it is reached through an import library and
shipped as its own DLL rather than absorbed into a product binary.  It is also
what a `--with-system-<lib>` switch needs: that is a PROVENANCE choice, and it is
only expressible where the consumer links an interface instead of the
implementation — the `//build/config:*_system` branch `//build/deps` was designed
to grow.

Where the tree actually is, measured 2026-08-31: of the 36 modules under
`ext_libraries/modules`, **four** build a shared library — icu (3 DLLs), nss (8),
coinmp (1), python (1).  The other 32 are statically bound into the product, and
seven carry an explicit `*_STATIC` or `*_INTERNAL` define in our own BUILD files:
expat, libxml2, libxslt, mysqlcppconn, raptor2, rasqal, redland.  Those defines
are the tell — they are AOO-side knowledge of how somebody else's library was
compiled, living in our build.

The RDF trio is the most interesting candidate whenever this is picked up:
Debian ships libraptor2, librasqal and librdf as shared libraries, and
`unordf.dll` currently absorbs all three.

Build-time tools are outside this either way.  A generator that runs as a build
action has no staged CRT or DLL beside it and must be self-contained; ICU's
genbrk/gencmn/genccode are the worked example.
