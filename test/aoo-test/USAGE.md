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

# Using the suite

Run everything from this directory. `python3 -m pytest` is enough; `uv` is not required.

## Choosing the office

An office is only started when you ask for one, with `AOO_OFFICE` or `--office`. Both take any of:

- an `.app` bundle (macOS)
- the install root
- the `program` directory
- the `soffice` launcher

Builds before 5.0 are refused. Without an office, the tests that need one skip and say why; add `--require-office` to make that a failure.

The suite never touches your own profile. Each ODF save mode gets its own private profile and a headless office under pytest's temporary directory. The offices are stopped at the end, and the session fails if one is still running.

## Tools for the schema checks

The ODF schema checks need Java, jing and the OASIS schemas. Fetch them once:

    PYTHONPATH=src python3 -m aootest.fetch            # download what is missing
    PYTHONPATH=src python3 -m aootest.fetch --list     # show what is cached
    PYTHONPATH=src python3 -m aootest.fetch --offline  # download nothing; exit 1 if something is missing

Each download is checked against its pinned sha256 before it is used. The cache is `~/Library/Caches/aootest` on macOS, `$XDG_CACHE_HOME/aootest` or `~/.cache/aootest` on Linux and `%LOCALAPPDATA%\aootest\Cache` on Windows; `AOO_TEST_CACHE` overrides it. To use your own copies instead, set `AOO_JING`, `AOO_ODFVALIDATOR`, `AOO_SCHEMA_DIR` or `AOO_JAVA`.

## Running

    python3 -m pytest -m selftest         # checkers against negative controls, no office
    python3 -m pytest tests/harness       # install and worker handling, no office
    python3 -m pytest -m smoke            # quick office checks
    python3 -m pytest -m "not slow"       # all of the above
    python3 -m pytest tests/odf           # save modes and the other families, slow
    python3 -m pytest -m crash            # known crashes of the office, kept out of the default run
    python3 -m pytest -k convert          # any pytest selection works

## Options

| Option | Environment | Meaning |
|---|---|---|
| `--office PATH` | `AOO_OFFICE` | The install to test. |
| `--uno-python PATH` | `AOO_UNO_PYTHON` | A Python that can import `uno`. Skips the search; if it fails the probe, that is an error. |
| `--connect socket\|pipe` | | How the worker reaches the office. Default `socket`. |
| `--max-restarts N` | | Office restarts allowed per save mode before the session aborts. Default 5. |
| `--start-timeout S` | | Seconds an office may take to start and accept a connection. Default 240; starts have taken from 5 to 90 seconds on a busy desktop. |
| `--require-office` | | Fail instead of skip when there is no usable office. |
| `--require-odf13` | | Fail instead of skip the ODF 1.3 save modes when the build cannot write 1.3. |
| `--documents DIR` | | Extra ODF documents to run through every save mode. |
| `--shipped-limit N` | | How many shipped templates and documents to use. Default 40. |
| `--twin-corpus DIR` | `AOO_TWIN_CORPUS` | Corpus for the import twin tests. |
| `--strict-expect` | | A test that an expectation row says should fail must fail. |

## Pyuno

The tests talk to the office through a small worker process that runs under a Python able to import `uno`. The suite tries these in order and keeps the first that imports `uno`: the install's own Python (`program/python`, shipped by Linux and FreeBSD builds), the Python that `libpyuno` is linked against, the Python running pytest, then `python3.N` on the path. A macOS build that links a system Python needs that Python installed. If none works, the office tests skip and list what was tried. `--uno-python` names one directly.

## Source documents

The save-mode tests run over documents that are not stored in this repository:

- Templates and documents the install ships under `share/template` and `share/samples`, up to `--shipped-limit`. File types and directories are interleaved, so a small limit still spreads across both, and the same install always gives the same list.
- One new document per application (Writer, Calc, Impress, Draw), created by the office.
- Documents the worker builds through the API (`src/aootest_worker/ops/generate.py`): page fills, other fills, ODF 1.3 features. They are made in ODF 1.2 extended mode, which keeps every feature, then saved in each mode and read back; the values each read-back must give are in `data/checks/`.
- Documents made by the large scripts of the ODF harness (`src/aootest_worker/fixtures/`, run by `ops/scripts.py`): Writer, Calc, Draw and Impress features, charts, style properties, document types. `data/built-files.tsv` lists what each script makes. Each script runs in an office of its own, which is closed afterwards; a script that stalls is run once more. They carry no read-back checks, so they add the save-mode items only.
- Every ODF document under `--documents DIR`, if given.

`data/built-excluded.tsv` names the documents of those scripts that are left out of the default run because converting them kills or stalls the office (Calc documents with tracked changes, Base documents). `python3 -m pytest -m crash` runs each of them through every save mode in a fresh office; expect crash reports, and on macOS the "unexpectedly quit" prompt for each crash, to which `Don't Reopen` is the answer. Known deviations of the other documents are rows in `data/expect/`.

Each document is saved once per mode: strict ODF 1.2 (save mode 4), 1.2 extended (3), 1.3 (5), 1.3 extended (6) and the product default. Builds that predate the ODF 1.3 work ignore modes 5 and 6; those items skip, or fail with `--require-odf13`.

## Other families

- **Error kinds of the extended modes.** The 1.2 extended and 1.3 extended modes write extension content on purpose, so their output is not asserted to be schema valid. `tests/odf/test_kinds.py` compares the errors against the ODF 1.2 and 1.3 schemas, and the same with foreign content removed, with `data/baseline/`: a new kind of error, a higher count, a kind found in more documents, or a baseline document that is no longer covered fails the item; fewer errors pass. The baseline covers the documents the suite builds or creates, not the shipped ones, and was written with the trunk dev app, so another build can differ. After a change that is meant to alter the errors, run `python3 -m pytest tests/odf/test_kinds.py --write-kinds` and read the diff of `data/baseline/` before committing it.

- **Cross-check.** odfvalidator checks the same saved files as jing; the two must agree on which streams have errors.
- **Stability.** Each document is saved twice; `content.xml` and `styles.xml` must match apart from the cached value of date and time fields.
- **Encrypted.** Documents saved with a password under each combination of `UseSHA1InODF12` and `UseBlowfishInODF12`, in ODF 1.2 strict and extended mode. The suite's own decryptor (`aootest/checkers/decrypt.py`) must read them with the right password, reject a wrong one, and get the same content as the unencrypted copy.
- **Formulas.** `data/calcval-cases.tsv` holds about 250 OpenFormula cases with the result ODF 1.3 Part 4 requires; each is evaluated in Calc.

## Import twins

The twin tests load documents saved as ODF 1.2 and 1.3 from the same source by another producer and check that AOO imports each pair alike. They need a corpus, which is not part of the suite: a directory with `v12`, `v13`, `v12ext` and `v13ext` subdirectories holding the same file names. Point `--twin-corpus` or `AOO_TWIN_CORPUS` at it; without one the items skip. Two controls run with it: an unchanged copy must compare equal and a copy with one changed word must compare different.

## Expected failures

Known problems are rows in `data/expect/*.tsv`:

    test_name<TAB>xfail|flaky|skip<TAB>reason

`test_name` is the pytest item name including its parameters, and `*` matches any characters. The reason states what is known. An `xfail` row that starts passing shows as XPASS; with `--strict-expect` that fails the run. A `flaky` row is for a test that fails only in some runs and is never strict. At the end of a full run the suite reports rows that matched no test.

## Reading results

- A skip always carries a reason: no office, no tools, a mode the build cannot write.
- A build that predates the ODF conformance work fails schema checks and some formula cases (the installed 5.1.0 does). Those are results about the build, not suite errors.
- A hung or crashed office is recorded against the document that caused it, the office is restarted, and the run continues. Too many restarts abort the session.
