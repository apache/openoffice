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
- `jing.jar` (set `ODFV_JING`) and `java`.
- `gtimeout` (GNU coreutils), used by `drive.sh`.

Defaults for `ODFV_APP`, `ODFV_PYTHON`, `ODFV_JING` and `ODFV_SOLVER` are in
`env.sh`.

## Setup

    ./fetch-schemas.sh                 # OASIS schemas, checksum-verified, into schema/
    ./setup-app.sh <path-to-AOO.dmg>   # copies OpenOffice.app into app/
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
  against 1.2 as written and against 1.4 with `office:version` rewritten.
  Embedded objects whose `content.xml` is MathML are skipped (ODF 1.2 Part 1
  §2.2.1 B.4).
- `report.py` groups the errors by normalized message.
- `idcheck.py <outdir> <ids.json>` parses every stream namespace-aware and
  checks `xml:id` syntax and uniqueness per stream (ODF 1.2 Part 1 §2.2.1
  D.1, D.2). `jing -i` does not check ID uniqueness.

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
