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

# UNO fact graph — the contract

Two halves are produced independently and joined by `uno_graph.py`:

| Half | Producer | Content |
| --- | --- | --- |
| Type graph | `//main/registry:regview` over a built `.rdb` | services, interfaces, methods, params, supertypes |
| Registration + provenance | `//build/rules:unograph.bzl%uno_provenance_aspect` | component → impl → library → Bazel target → staged path |

The join key is the **service name string** — the same literal that appears in
`createInstance("com.sun.star.frame.Desktop")`.

## Producing the inputs

    bazel build //main/offapi:offapi_idl //main/udkapi:udkapi_idl //main/registry:regview
    bazel build //main/staging:install \
        --aspects=//build/rules:unograph.bzl%uno_provenance_aspect \
        --output_groups=uno_facts

## Fact fragments (JSON lines, one object per file)

    {"fact":"provenance","label":"//main/framework:fwk","kind":"cc_binary",
     "staged":false,"outputs":["main/framework/fwk.dll"]}

    {"fact":"registration","label":"//main/postprocess:services_rdb",
     "components":[{"component":"main/framework/util/fwk.component",
                    "label":"//main/framework:util/fwk.component",
                    "uri":"vnd.sun.star.expand:$OOO_BASE_DIR/program/fwk.dll"}]}

`staged: true` marks a `flat_install`/`tree_install`/`res_stage` target, whose
outputs are staged paths rather than build outputs.

## URI forms

| Prefix | Artifact |
| --- | --- |
| `vnd.sun.star.expand:$OOO_BASE_DIR/program/<x>` | native DLL |
| `vnd.sun.star.expand:$OOO_BASE_DIR/program/classes/<x>` | Java jar |
| `vnd.sun.star.expand:$URE_INTERNAL_JAVA_DIR/<x>` | URE Java jar |
| `vnd.openoffice.pymodule:<x>` | Python module |

## Boundary rule

`uno_graph.py` reads only the JSON fragments, the regview dump, and the
`.component` XML they name. It must never import Bazel internals or hardcode
`bazel-out` layout — that boundary is what keeps a future editor extension a
cheap extraction rather than a rewrite.

## Format assumptions worth failing loudly on

- regview never emits a multi-line value: long docs are escaped onto one
  line as a literal backslash-u-000A sequence. A line-oriented parser is
  only sound because of this.
- The full-tree dump nests keys by **indentation**, so the `Data` block's
  column varies with depth. Parse stripped lines, never fixed columns.
- The dump is UTF-8 with BOM and CRLF line endings.

`schema_version` in the `meta` table is bumped whenever fragment shape changes.
