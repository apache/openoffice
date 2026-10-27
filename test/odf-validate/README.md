# ODF validation harness

This harness has OpenOffice save a corpus of documents as ODF, checks every
saved package against the OASIS ODF 1.2 and ODF 1.4 RELAX NG schemas with
jing, and reports the errors grouped by kind. Comparing those reports against
a baseline shows whether an export change fixed errors or introduced new ones.

It runs on macOS only (zsh scripts, `hdiutil`, `.app` layout).

## Requirements

- An AOO macOS build (`.dmg`).
- The Python that the office's pyuno links against. Check with
  `otool -L <app>/Contents/program/libpyuno.dylib`, then set `ODFV_PYTHON`.
- `jing.jar` (set `ODFV_JING`) and `java`; `ODFV_ODFVALIDATOR` and `ODFV_JAVA` for `crosscheck.py`.
- `gtimeout` (GNU coreutils), used by `drive.sh`.

Defaults for `ODFV_APP`, `ODFV_PYTHON`, `ODFV_JING` and `ODFV_SOLVER` are in
`env.sh`.

## Setup

    ./fetch-schemas.sh                 # OASIS schemas, checksum-verified, into schema/
    ./setup-app.sh <path-to-AOO.dmg>   # copies OpenOffice.app into app/
    ./fetch-odfvalidator.sh            # optional: odfvalidator for crosscheck.py
    python3 -I selftest.py             # validator negative controls

The schemas are not committed because they carry OASIS copyright terms.

## Running a corpus

    ./make-jobs.sh runs/base_strict/out > runs/base_strict/jobs.tsv
    ./drive.sh strict 2013 runs/base_strict/jobs.tsv runs/base_strict/done.tsv
    python3 -I validate.py runs/base_strict/out runs/base_strict/val.json
    python3 -I report.py runs/base_strict/val.json baseline/strict.kinds.json 12

The steps:
- `make-jobs.sh` collects the test documents under `main/` (doc, docx, rtf,
  xls, ppt, sx*, od*).
- `drive.sh <ext|strict> <port> <jobs> <done>` saves them in ODF
  1.2 Extended (`DefaultVersion=3`) or strict ODF 1.2 (`DefaultVersion=4`).
  - It runs a headless office on the given port, using a profile under `runs/`.
  - It resumes where it left off, kills a hung office and continues, and
    stops at the first crash.
- `validate.py` checks content, styles, meta, settings and manifest streams
  against 1.2 as written, against 1.4 with `office:version` rewritten, and
  against 1.2 as an extended document (`12x`): `foreign.py` first removes
  foreign elements and attributes as Part 1 §2.2.2 B.1 and §3.17 describe (the
  manifest is left as written). Use `report.py <val.json> <kinds.json> 12x` to
  measure what is wrong apart from foreign content.
  Embedded objects whose `content.xml` is MathML are skipped (ODF 1.2 Part 1
  §2.2.1 B.4).
- `report.py` groups the errors by normalized message.
- `pkgcheck.py <outdir> <pkg.json>` checks the ODF 1.2 Part 3 package rules
  (§2.2.1, 3.2, 3.3, 3.4.1): zip methods, `mimetype` first / stored / no extra
  field / equal to the `/` media type, one manifest entry per file, META-INF
  contents, `manifest:size` and STORED on encrypted entries, and signature
  files (well-formed, root, `OpenDocument-v1.2-os-dsig-schema.rng`). Findings
  are `shall:` or `should:`; only `shall` findings are non-conformances.
- `doctypecheck.py <outdir> <dt.json>` checks Part 1 §2.2.1 A.2/E and
  §2.2.3-2.2.10: the `mimetype` file (or `office:mimetype` of a flat file), the
  root element and the `office:body` child must agree on one document type.
  Embedded sub documents are compared with their manifest media type
  (`should:` findings). Two deviations from the literal text of §2.2.5: it
  spells the drawing values `application.vnd.oasis.opendocument.graphics` and
  `...graphic-template`; the registered `...graphics` and `...graphics-template`
  of Part 1 appendix C are accepted. `...text-web` and `...formula-template`
  are registered but no §2.2.x allows them, so they are reported.
- `formulacheck.py <outdir> <formulas.json>` parses the `style:condition`,
  `table:condition`, `table:expression`, `table:formula` and `text:formula`
  values (Part 1 §2.2.1 D.3, §2.2.4 D/E) with the OpenFormula grammar (Part 2
  chapter 5) and the condition syntaxes of Part 1 §19.468 and §19.596.
  - Syntax only; no semantics, function arity or type checking.
  - A prefix bound to another namespace is out of Part 2's reach: `should:` in
    general, `shall:` for `table:formula` inside `office:spreadsheet` (§2.2.4 D).
  - Non-ASCII name characters use Unicode categories instead of the XML 1.0
    4th edition tables; an IRI is checked by character class, not the full
    RFC 3987 grammar; `openformula-functions.txt` holds the Part 2 function
    names.
  - The condition syntaxes follow the prose of Part 1, which has no grammar:
    `,` between arguments, lowercase ` and `, `!=` as the relational operator.
  - `formula-cases.tsv` is the table of good and bad values that `selftest.py`
    runs.
- `crosscheck.py <outdir> <val.json> <cross.json>` runs the Apache ODF Toolkit
  `odfvalidator` 1.1.8 (`fetch-odfvalidator.sh`; MSV instead of jing, the same
  OASIS schemas) in conformance mode (`-c`) and extended mode (`-e`) and
  compares each stream with the `12` and `12x` results of `validate.py`.
  - Error counts differ between engines; it compares whether a stream has
    any error and, in `-c` mode, where its first error is.
  - It exits if odfvalidator stops before finishing every package. Its
    failures are silent (an invalid schema prints "Validation aborted"; a
    crash on one package aborts the run): crashes are skipped and listed.
  - odfvalidator's own schema lookup fails on current JDKs, so the schemas are
    passed explicitly; its jar has no MathML 2 schema, so a schema that accepts
    anything stands in (as `validate.py` skips MathML).
  - Known leniencies: it truncates a `draw:points` over 2048 characters and
    adapts old OOo namespaces. It is stricter than Part 3 §3.2 in wanting
    `META-INF/documentsignatures.xml` listed in the manifest. It checks ID
    uniqueness, which `jing -i` does not.
- `fixtures/doctypes/` (`run.sh`, `make.py`, `derive-chart.py`), `fixtures/encrypted/`, `fixtures/signed/`, `fixtures/base/`: documents
  of the kinds the corpus lacks (templates, master, Writer/Web, Impress as Draw, Math, chart, encrypted, signed, Base), saved
  in strict and extended mode under `runs/task12/gen-*`. `decrypt.py <in-dir> <out-dir> <password>` writes plain copies of
  encrypted packages for the schema checkers; `pkgcheck.py` must still run on the real ones. Signing needs a visible
  (`-invisible`) office because `-headless` cancels the dialog: see the header of `fixtures/signed/make.py`.
- `fixtures/{writer,calc,draw,chart,styles-text,styles-gfx,leak}/make.py`, run with `fixtures/gen-run.sh <area> <strict|ext> <port> <abs-outdir>`:
  documents built through UNO to exercise the elements, attributes and enumerated values that `cov_report.py` lists as gaps
  (`leak` classifies the names AOO emits that the schema does not define). Use an absolute `<outdir>`.
- `cov_schema.py`, `cov_source.py`, `cov_corpus.py`, `cov_report.py`: schema-coverage report. The schema universe
  (ODF 1.2 elements, element/attribute pairs, enumerated values), the names the export code in `main/` can emit (static
  C++ scan; a lower bound, table-driven names are missed) and what a corpus exercises are joined by namespace URI and
  local name into the gaps worth generating documents for. Outputs live under `runs/`.
- `idcheck.py <outdir> <ids.json>` parses every stream namespace-aware and
  checks `xml:id` syntax and uniqueness per stream (ODF 1.2 Part 1 §2.2.1
  D.1, D.2). `jing -i` does not check ID uniqueness.

## The regression gate

    python3 -I gate.py <tag> --prev runs/gate-<earlier tag>      # everything
    python3 -I gate.py <tag> --build xmloff,sw --prev ...        # rebuild first
    python3 -I gate.py smoke --limit 8 --prev ... --stages corpus,validate,dump,diff

`gate.py` runs the stages below in `runs/gate-<tag>/`, records `results.json`,
skips stages already done (`--force` reruns) and exits 0 only if everything
that ran passed.

| stage | does | passes if |
|---|---|---|
| `build` | `build.pl` and `deliver.pl` for the `--build` modules (environment script: `ODFV_BUILD_ENV`) | exit codes 0 |
| `refresh` | copies `--libs` from the solver into `app/` (`refresh-app.sh`) | copies identical |
| `corpus` | saves the job list in `ext` and `strict` mode | no crash, every job accounted for, OK/FAIL counts equal `--prev` |
| `validate` | `validate.py`, `report.py` and the four checkers per mode | kinds not worse than `baseline/<mode>.kinds.json` or than `--prev` (checked both ways) |
| `dump` | reloads every saved package with `dump7.py` | no office failure; hangs and crashes are listed |
| `diff` | `diff7.py` on the dumps and `cmp7.py` on the packages against `--prev` | every difference is a document in `gate-volatile.tsv` |
| `fixtures` | `gate-fixtures.tsv`: saves in `v00 v10 v11 ext strict`, runs the reload checks, validates | saved, checked, and unchanged against `--prev` |

- Inputs are frozen: without `--jobs` the list of `--prev` is reused (outputs
  are rewritten), so the same documents are saved when files under `main/`
  change. A later run's `--prev` may be any earlier gate directory or a
  directory with the same layout (`<mode>/out`, `<mode>/dump`).
- The dump stage kills the office only when the dump makes no progress for
  `--stall` seconds (default 300), then writes a `HANG` marker for that
  document alone. A long batch that keeps progressing is never killed.
- `gate-volatile.tsv` names the documents whose dumps differ between runs
  without an export change (volatile Calc formulas, redline timestamps, the
  `0238` loader order, the intermittent Basic library loss of `0048`). A new
  entry needs a reason.
- The first fixtures run has nothing to compare with: its mismatch pattern is
  only recorded. By design, strict, ODF 1.0 and ODF 1.1 mode fail the 14
  reload rows that need gradient or transparency fills.

## Checking a change

    python3 -I compare.py baseline/strict.kinds.json runs/new/strict.kinds.json

`compare.py` exits 1 if any kind is new or any kind got worse.

To test a rebuilt module without repackaging, copy its libraries from the
solver into the app copy:

    ./refresh-app.sh libxo.dylib

`roundtrip.py <port> <file> <checks.json>` loads a document and compares UNO
expressions with expected values; `--capture <out.json>` records them instead.

## Notes

- AOO has no `-convert-to`; conversion goes over UNO (`convert.py`).
- jing must run with `-i`, otherwise it rejects the ODF schemas themselves.
- A jing fatal error can hide later errors. Such lines are reported as
  `UNPARSED` kinds, and `compare.py` fails on any new `UNPARSED` kind.
- jing stops at a fatal error (a stream that is not well-formed);
  `validate.py` records it and carries on with the remaining files.
- `setup-app.sh` renames an existing `app/OpenOffice.app` to
  `OpenOffice.app.old.<timestamp>` instead of deleting it.
