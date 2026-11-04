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

# AOO test suite

A pytest suite for an installed Apache OpenOffice 5 or later. It starts the office you point it at, drives it over UNO, and checks what comes out: ODF conformance in each save mode, import of ODF 1.2 and 1.3 files, document conversion, and a smoke test of the main components.

The only required input is where the office is installed. The suite runs the same way on any platform and works with an install, so no source tree or build is needed.

## What it checks

| Directory | Needs an office | What |
|---|---|---|
| `tests/selftest` | no | The checkers (schema validation, package rules, document types, xml ids, formula syntax, decryption) against documents built to fail. |
| `tests/harness` | no | Install discovery, the worker protocol and process handling, against fake installs and a fake worker. |
| `tests/smoke` | yes | Start, connect, new/save/reload per application and save mode, conversion, and the old smoke test document (Writer, Calc, Impress, Draw, HTML, chart, Math, Bibliography, extensions). |
| `tests/odf` | yes | Documents saved in each ODF save mode and checked against the ODF 1.2, 1.3 and 1.4 schemas and the package rules, with a second validator (odfvalidator) as a cross-check; documents the suite builds, saved and read back; saving twice for stable output; the error kinds of the extended modes compared with a stored baseline; password-protected documents decrypted by an independent implementation; about 300 OpenFormula cases evaluated in Calc; and, with a corpus, ODF 1.2 and 1.3 twins compared after import. Everything but the formula cases is marked `slow`. |

## What it does not include

Nothing third party and no binary documents are stored here.

- Test documents come from the office under test: the templates and documents it ships, and documents it creates when the tests run. See [USAGE.md](USAGE.md#source-documents).
- Some documents the suite can build are left out of the default run because converting them kills or stalls the office: Calc documents with tracked changes and all Base documents. They are listed with the reason in `data/built-excluded.tsv` and run with `python3 -m pytest -m crash`.
- The OASIS schemas, jing and odfvalidator are downloaded once into a cache and checked against pinned sha256 values. Without them the tests that need them skip, with a reason.

## Requirements

- Python 3.10 or later and pytest 8 or later. Nothing else is needed to run the tests.
- An installed AOO 5 or later.
- A Python that can import `uno` from that install. The suite finds one itself, preferring the install's own `program/python`; see [USAGE.md](USAGE.md#pyuno).
- Java, for the schema checks. Without it those items skip.

## Quick start

From this directory:

    python3 -m pytest -m selftest                    # no office needed
    PYTHONPATH=src python3 -m aootest.fetch          # once: schemas and jing, about 20 MB
    AOO_OFFICE=/path/to/install python3 -m pytest -m "not slow"
    AOO_OFFICE=/path/to/install python3 -m pytest    # everything, several minutes

`AOO_OFFICE` can be an `.app` bundle, the install root, its `program` directory, or the `soffice` launcher.

## Where it has been run

macOS arm64 (an installed 5.1.0 and a trunk build) and Ubuntu 24.04 aarch64 (a trunk build). The Windows and FreeBSD code paths are covered by tests against fake installs only; nobody has run the suite on those systems yet.

See [USAGE.md](USAGE.md) for options, expectation files and how to read results.
