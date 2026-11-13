# OpenFormula 1.4 Small Group Test Plan

## Purpose

Establish reproducible ODF 1.4 Part 4 Small Group conformance tests before making any
implementation changes. Tests are prepared for later execution once the implementation
milestones are complete. No production evaluator changes are made in this milestone.

## Test Files

| File | Purpose |
|------|---------|
| `odf_docs/openformula-small-group-1.4-tests.tsv` | Requirement-referenced test inventory (148 ReqIDs; includes E.1 corrections and added `SG-SYN-001` criteria test) |
| `test/odf-validate/calcval-cases14.tsv` | Existing calcval cases, extended with Small Group semantic tests |
| `openformula-small-group-test-plan.md` | This document: execution instructions and outstanding runtime requirements |

## Execution Instructions

### Prerequisites

A configured AOO build tree is required. The `sc` and `scaddins` modules must be built:

1. Configure AOO (`autogen.sh`/`configure`, then `build.pl --all`)
2. Build the `sc` module (Calc core)
3. Build the `scaddins` module (Analysis Add-in for CONVERT)

### Running calcval Tests

The `test/odf-validate/` directory contains the calcval test harness (designed for macOS):

```bash
# Start the calcval server
test/odf-validate/start.sh calcval <port>

# Run test cases
test/odf-validate/runpy.sh calcval.py <port> calcval-cases14.tsv <out.json>

# Compare results
python3 -I calcval.py --compare <out.json> <expected.json>
```

### Test Case Format

Each calcval case has 5 tab-separated columns:
1. **id**: Unique test identifier
2. **formula**: AOO API syntax (English function names, ';' separators)
3. **mode**: `cell` (plain cell), `cell@N` (scratch cell), `array[:CxR]` (array formula)
4. **expected**: Expected result ('-', number with tolerance, '#N/A', '#ERR', 's:text')
5. **note**: Normative section reference and description

### Priority Order for Test Execution

| Priority | Area | Test Cases | Rationale |
|----------|------|------------|-----------|
| 1 | COUNTA/COUNTBLANK empty cells / empty string | `COUNTA.empty`, `COUNTA.emptystr`, `COUNTBLANK.empty`, `COUNTBLANK.range` (calcval14) and corresponding SG tests | ODF 1.4 §6.13.7 counts `""` as non-blank; §6.13.8 `""`-in-cell is impl-defined and not directly reproducible with current calcval fixtures |
| 2 | DATE out-of-constraint/pre-1583 | `DATE.pre1583` / `FN-016-DATE-pre1583` | §6.10.2 constrains Year>=1904; `DATE(1500;1;1)` is impl-defined/diagnostic (not mandatory `#ERR`) |
| 3 | INDEX row-vector | `INDEX.rowvec.single`, `INDEX.rowvec.colarg`, `INDEX.row.outrange` | Milestone A fix; regression risk for legacy documents |
| 4 | Bitwise functions | `BITLSHIFT.neg`, `BITRSHIFT.neg`, `BITAND.3params` | Parameter count and negative shift behavior |
| 5 | Parser/operators | `SYN-005-precedence`, `SYN-005-power`, `OP-006-div0` | Operator precedence and associativity |

## Outstanding Runtime Requirements

The following cannot be verified until a configured AOO build tree exists:

| Requirement | Reason |
|-------------|--------|
| All 4 Basic Limits (§3.7) | No runtime test execution possible |
| All 11 Syntax items (§5) | No runtime test execution possible |
| All 5 Implicit Conversions (§6.3) | No runtime test execution possible |
| All 14 Operators (§6.4) | No runtime test execution possible |
| All 110 Functions (§2.3.2.E) | No runtime test execution possible |
| ODF save/reload round-trip | No built office available |
| Excel import/export | No built office available |

## Calcval Harness Portability Constraints

The `test/odf-validate/` calcval launcher is macOS-only and is NOT directly Linux-compatible. It is not ported in this milestone.

- **zsh `${0:A:h}` path expansion**: `start.sh`, `runpy.sh`, and `env.sh` rely on the zsh-specific `${0:A:h}` parameter expansion for path resolution; `start.sh` and `runpy.sh` use `#!/bin/zsh` shebangs, while `env.sh` (sourced, no shebang) also uses zsh-specific syntax at `test/odf-validate/env.sh:23`.
- **macOS application bundle path**: `env.sh` sets `APP=${ODFV_APP:-$D/app/OpenOffice.app}/Contents`, and `start.sh:25` launches `$APP/MacOS/soffice` (an `.app` bundle structure that does not exist on Linux).
- **Apple Python Framework default**: `env.sh:28` defaults `PY=/usr/local/Library/Frameworks/Python.framework/Versions/3.14/bin/python3.14` (macOS Python Framework path).
- **External `lsof` dependency**: `start.sh:28` polls with `lsof -nP -iTCP:$2 -sTCP:LISTEN` to wait for the office socket; `lsof` itself is available on Linux, but the harness depends on its presence.

No production source or runtime behavior changed. All corrections are test data/docs only.

## Relationship to Milestones

- **Milestone D**: Established the 110-function Small Group inventory
- **Milestone D.1**: Corrected audit inconsistencies (statuses = IMPLEMENTED_UNVERIFIED)
- **Milestone E (this)**: Created test inventory and test cases
- **Milestone F**: Implement confirmed semantic corrections (narrowly scoped)
- **Milestone G**: ODF import/export and round-trip conformance
- **Milestone H**: Final consolidated verification

## What Must NOT Change

- No production evaluator changes in this milestone
- No new runtime dependencies
- No mandatory `sc` or `scaddins` builds during this milestone
- Tests use existing `test/odf-validate` tooling and `calcval-cases14.tsv` conventions
- Tests are NOT added to the normal production build


## E.1 Corrections

- `SG-BL-001` / `BL-001-1024chars`: formula expression length 1026 chars (`=1` + `+1`*512), expected 513.
- `SG-BL-004` / `BL-004-7nesting`: 7 genuinely nested `IF` function levels, balanced parens, expected 1.
- `SG-SYN-001` / `SYN-001-criteria`: added Criteria syntax §4.11.11 test using `DCOUNT(R1:T6;;V1:V2)` against existing database/criteria fixtures (expected 4).
- `SG-SYN-009` / `SYN-009-named`: requires pre-defined named expression `MYNAME`; not directly executable in calcval harness without name setup.
- `SG-FN-013` / `FN-016` tests updated: COUNTA covers empty cells and empty string `""`; COUNTBLANK covers truly empty cells; `""`-in-cell blankness is implementation-defined and not directly reproducible with calcval fixtures.
- `SG-FN-016` / `FN-016-DATE-pre1583`: reclassified as impl-defined/diagnostic (Year < 1904 outside §6.10.2 constraints).

**Static validation only** (no AOO build tree; calcval macOS-only): TSV structure/columns, duplicate IDs, char/nesting counts, fixture references, and count reconciliation checked. Runtime execution not performed.
