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

# Running the ODF test suites

Two Python harnesses check how the office reads and writes ODF 1.2, 1.3 and 1.4. They are separate from the Java/Ant tests described in [README.md](README.md).

| | `aoo-test/` | `odf-validate/` |
|---|---|---|
| What it is | A pytest suite | A regression gate over a document corpus |
| Platforms | macOS, Linux, Windows | macOS only (zsh, `hdiutil`, `.app`) |
| Office | Any installed AOO 5 or later | A macOS `.dmg` copied into `odf-validate/app/`, optionally rebuilt libraries |
| Needs a source tree | No | Yes: the corpus is the test documents under `main/` |
| Use it to | Check a build or install | Compare a change in the export code with an earlier run |
| Details | [aoo-test/README.md](aoo-test/README.md), [aoo-test/USAGE.md](aoo-test/USAGE.md) | [odf-validate/README.md](odf-validate/README.md) |

Both validate the saved packages against the OASIS RELAX NG schemas with jing, check the package rules, document types, `xml:id` and formula syntax, and cross-check with the Apache ODF Toolkit `odfvalidator`. Java is needed for the schema checks. The schemas are not stored in the repository; each harness downloads them and checks them against pinned sha256 values.

## 1. The pytest suite (`aoo-test`)

Needs Python 3.10 or later, pytest 8 or later, and Java.

    cd test/aoo-test
    python3 -m pytest -m selftest                       # checkers against negative controls, no office
    PYTHONPATH=src python3 -m aootest.fetch             # once: schemas, jing, odfvalidator into a per-user cache
    AOO_OFFICE=/path/to/install python3 -m pytest -m "not slow"
    AOO_OFFICE=/path/to/install python3 -m pytest       # everything, takes a while

`AOO_OFFICE` is an `.app` bundle, the install root, its `program` directory, or the `soffice` launcher. Without it, the tests that need an office skip and say why; add `--require-office` to make that a failure. The suite uses private profiles and never touches yours.

- The documents it saves come from the install itself (`share/template`, `share/samples`) and from documents it builds through UNO. Add your own with `--documents DIR`.
- It saves each document in strict ODF 1.2, 1.2 Extended, 1.3, 1.3 Extended, 1.4 and 1.4 Extended mode and the product default. A build that predates the ODF 1.3 or 1.4 work skips those modes; `--require-odf13` and `--require-odf14` make that a failure.
- Known deviations are rows in `data/expect/*.tsv`. `--strict-expect` fails the run if a row listed as failing starts to pass. Error kinds of the extended modes are compared with `data/baseline/`.
- `python3 -m pytest -m crash` runs the documents that crash the office; expect crash reports.
- Every option and the other test families (cross-check, stability, encrypted packages, about 300 OpenFormula cases, import twins) are in [aoo-test/USAGE.md](aoo-test/USAGE.md).

## 2. The ODF validity corpus (`odf-validate`)

Each step below is described in [odf-validate/README.md](odf-validate/README.md). The scripts are zsh.

**Set up once**

    cd test/odf-validate
    ./fetch-schemas.sh                     # OASIS schemas into schema/, checksum-verified
    ./setup-app.sh <path-to-AOO.dmg>       # copies OpenOffice.app into app/
    ./fetch-odfvalidator.sh                # optional, for the cross-check
    python3 -I selftest.py                 # validator negative controls, no office

The defaults in `env.sh` are the author's. Set `ODFV_PYTHON` to the Python that the office's pyuno links against (`otool -L <app>/Contents/program/libpyuno.dylib` shows which), and `ODFV_JING` to a `jing.jar`. The jar that `aootest.fetch` downloads (`jing/` in its cache, `~/Library/Caches/aootest` on macOS) works. `crosscheck.py` also reads `ODFV_ODFVALIDATOR` and `ODFV_JAVA`, and `drive.sh` needs `gtimeout` from GNU coreutils.

**Run the gate**

    python3 -I gate.py <tag> --stages corpus,validate,dump,diff,fixtures --modes ext,strict,v14,x14 --port 2900
    python3 -I gate.py <tag2> --prev runs/gate-<tag> --stages corpus,validate,dump,diff,fixtures --modes ext,strict,v13,x13,v14,x14 --port 2900

The default stage list also has `refresh`, which copies rebuilt libraries from `main/solver` into `app/` and fails if there is no build, so leave it out unless you built the office. The default modes are `ext,strict`.

`gate.py` saves the corpus (the documents found under `main/` by `make-jobs.sh`) in each mode, validates the packages, reloads and dumps every saved document, and compares with an earlier run. It writes `runs/gate-<tag>/results.json`, resumes where it stopped, and exits 0 only if every stage passed. Read the `diff` entry of `results.json` as well as the exit status.

- `runs/` is ignored by git and grows to many GB, so it holds the only copy of the earlier runs a gate compares with.
- `baseline/` has error kinds for `ext`, `strict`, `x14` and an empty `v14`. A gate in `v13` or `x13` mode has nothing to compare with until you pass `--prev` from a run that has them, so the first command leaves those modes out.
- The gate takes the list of documents from `--prev` when given, so the same documents are saved each time.
- Do not start your own office on the gate's ports: the `--port` value up to that value plus 35.
- To test rebuilt modules, add `refresh` to `--stages` (and `build` with `--build <modules>`), or copy libraries into `app/` with `./refresh-app.sh`, with no office running. The default `--libs` are `libxo`, `libsw`, `libsc`, `libsd` and `libsfx`.

**Without the gate**, the individual steps (`make-jobs.sh`, `drive.sh`, `validate.py`, `report.py`, `compare.py`) are listed under "Running a corpus" and "Checking a change" in the README. `calcval.py` evaluates the OpenFormula cases in Calc.

## What has been run

The selftests of both harnesses pass from a clean export of `trunk`. The office-driven parts of `aoo-test` and the gates were run in the development tree on macOS. Not run when this page was written: either harness from a clean export with an office, and a first gate with no `--prev` (including whether its `diff` stage passes without an earlier run).
