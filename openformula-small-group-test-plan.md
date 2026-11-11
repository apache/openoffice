# OpenFormula 1.4 Small Group Test Plan

## Purpose

Establish reproducible ODF 1.4 Part 4 Small Group conformance tests before making any
implementation changes. Tests are prepared for later execution once the implementation
milestones are complete. No production evaluator changes are made in this milestone.

## Test Files

| File | Purpose |
|------|---------|
| `test/odf-validate/openformula-small-group-1.4.tsv` | Requirement-referenced test inventory (148 ReqIDs from D inventory) |
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
| 1 | COUNTA/COUNTBLANK empty cells | `COUNTA.range`, `COUNTA.text`, `COUNTBLANK.range` | ODF 1.4 changed empty-cell semantics; can silently alter spreadsheet results |
| 2 | DATE pre-1583 | `DATE.pre1583` | ODF 1.4 §7.4 impl-defined; verify AOO behavior |
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
