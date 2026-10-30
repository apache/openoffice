# tools13: foreign ODF 1.2 / 1.3 corpus

`make-foreign-corpus.py` converts the same input documents with another popular Open Source Office suite
(headless) once per output version, giving ODF 1.2 / 1.3 twins for an import smoke test.

## Version values (observed)

The output version comes from the profile setting `/org.openoffice.Office.Common/Save/ODF`, property
`DefaultVersion`, written to the private profile's `user/registrymodifications.xcu`. Found black-box by converting one
small document per value (`--probe`) and reading `office:version` / `manifest:version` and the declared namespaces back
from the result. "Ext" means loext, calcext, officeooo, drawooo, tableooo and the ooo-ms-interop `field` namespace are
declared.

| value | office:version | extension namespaces |
|---|---|---|
| 1 | none (ODF 1.0) | no |
| 2 | 1.1 (manifest has no version) | no |
| 4 | 1.2 | no |
| 8, 9 | 1.2 | yes |
| 10 | 1.3 | no |
| 11 | 1.3 | yes |
| 12 | 1.4 | no |
| 13 | 1.4 | yes |
| 0, 3, 5, 6, 7, 14-20 | 1.4 | yes (acts as the default) |

Modes used: `v12` = 4, `v12ext` = 8, `v13` = 10, `v13ext` = 11. Observed on one build (26.8.1.1); re-run `--probe`
for another one. Plain-mode outputs still carry the legacy `ooo`, `oooc`, `ooow`, `rpt`, `formx` namespaces.

## Run

    python3 -I make-foreign-corpus.py <path to the other suite's soffice binary>

Defaults: 55 documents chosen from `runs/gate-t16f/jobs.tsv` (see the comment above `QUOTA` in the script;
`--list-selection` prints them), output root `test/odf-validate/runs/phase3/foreign/`. Other options: `--inputs LIST`
(one path per line, first tab column used), `--root`, `--modes`, `--batch`, `--timeout`,
`--probe WORKDIR [--probe-range 0-20]` for the value table only.

Outputs: `<root>/<mode>/<name>.<odt|ods|odp|odg>` (legacy inputs map to their ODF type, e.g. `x.doc` -> `x.odt`; two
inputs that would give the same output name are refused), `<root>/manifest.tsv` (input, mode, output, office:version,
manifest:version, ok/fail, note), `<root>/logs/`, and one private profile per mode (`<root>/foreign-prof-<mode>`).
Every output is checked for the expected `office:version` (in content, styles, meta, settings) and
`manifest:version`; plain modes must declare no extension namespaces, ext modes must. Any mismatch or missing
output gives a non-zero exit. Timed-out batches are retried file by file.

Process safety: only processes whose command line carries one of its own profile URLs are ever killed, and the
script refuses to start if such a process already exists.

## Use

A later import smoke test loads `<root>/v13/X` and `<root>/v12/X` (and the `ext` pair) and compares what the importer
produced from each, so a 1.3-specific import difference shows up against the 1.2 twin of the same source.

## Clean-room note

Only the other suite's output files and its public behaviour are observed. Its source code and its installed
resource/configuration files are not read, and its name is not used in files here; the binary path is a command
line argument only.
