# OpenFormula Test Gap Analysis — Apache OpenOffice Calc

Source-of-truth: `test/aoo-test/`, `test/odf-validate/`, and AOO source. Findings marked **[CONFIRMED]** have file:line evidence; **[UNVERIFIED]** require runtime execution.

## 1. Existing test inventories

### test/aoo-test/data/openformula-functions.txt
- 414 lines (414 function names + header comments).
- Header: "Names of the functions defined in OpenDocument 1.2 Part 2 (OpenFormula) chapter 6, one per line. Used by formulacheck.py to tell standard functions from nonstandard ones (Part 2 5.7)."
- **[CONFIRMED]** Includes IFNA (line 183), SKEWP (line 350), BITAND/BITOR/BITXOR/BITLSHIFT/BITRSHIFT (lines 59-63).
- **[CONFIRMED]** Does NOT include EASTERSUNDAY — despite EASTERSUNDAY being an implemented ODF 1.3/1.4 function (`interpr2.cxx:261`, `core_resource.src:333`).

### test/odf-validate/openformula-functions.txt
- 393 lines.
- Same header (ODF 1.2 Part 2 chapter 6).
- **[CONFIRMED]** Includes IFNA (line 162), SKEWP (line 329), BIT functions.
- **[CONFIRMED]** Does NOT include EASTERSUNDAY.

**[CONFIRMED] Both inventories are ODF 1.2-based baselines.** They are stale relative to ODF 1.4 (2025-10-06): EASTERSUNDAY (ODF 1.3/1.4) is absent from both. They list IFNA/SKEWP as "standard" but AOO does not implement them (see `openformula-functions.tsv`), so the inventories themselves contain functions AOO lacks — false negatives for conformance.

### test/aoo-test/data/calcval-cases.tsv
- Present (test/aoo-test/data/). Formula evaluation test cases.

### test/odf-validate/calcval-cases.tsv and calcval-cases14.tsv
- Present. `calcval-cases14.tsv` is ODF 1.4-specific.

### test/odf-validate tooling
- `formulacheck.py`, `calcval.py`, `cmp7.py`, `compare.py`, `diff7.py`, `dump7.py`, `dump14.py`, `roundtrip.py`, `cov_*.py`, `crosscheck.py`, `gate.py`, `report.py`.

## 2. Identified gaps

### Outdated specification inventories
- Both `openformula-functions.txt` files are explicitly ODF 1.2 chapter 6. No ODF 1.3 or ODF 1.4 function inventory exists in test data.
- **[GAP]** No machine-readable ODF 1.4 Part 4 function/operator/requirements inventory in `test/`.

### Missing function tests
- **[CONFIRMED]** IFNA: listed in inventories but NOT implemented in AOO (`xlformula.cxx:404` ocNoName stub only). No AOO function test can pass; inventory lists it as standard — false expectation.
- **[CONFIRMED]** SKEWP: listed in inventories but NOT implemented (`xlformula.cxx:415` ocNoName stub only). `SKEW` (sample) is implemented; SKEWP (population) missing.
- **[CONFIRMED]** EASTERSUNDAY: implemented (`interpr2.cxx:261`) but NOT in either function inventory. No dedicated test coverage confirmed.

### Missing semantic tests
- No confirmed semantic test for CONVERT `atm` discrepancy (101324.996583 Pa vs ODF 1.4 1.0132510E+5).
- No confirmed semantic test for INDEX row-vector single-argument (ODF 1.4: error; AOO: returns column).
- No confirmed semantic test for DATE pre-1583 (#VALUE! before 15 Oct 1582; proleptic Gregorian for YEAR/MONTH/DAY).
- No confirmed semantic test for BITAND/BITOR/BITXOR/BITLSHIFT/BITRSHIFT variadic params or negative-shift handling (interpr1.cxx:1231-1234 appears inverted).
- No confirmed semantic test for short-circuit AND/OR.

### Missing parser tests
- No confirmed parser test inventory for operator precedence/associativity (e.g. `2+3*4`, `2^3^2`).
- No confirmed parser test for automatic intersection (whitespace), reference union `~`, intersection `!`, range `:`.
- Quoted labels as implicit intersection (ODF 1.4 §3.9) — UNVERIFIED whether tested.

### Missing ODF round-trip tests
- No confirmed round-trip test verifying function names written on ODF export (e.g. `ORG.OPENOFFICE.CURRENT` vs `CURRENT`, `ORG.OPENOFFICE.ERRORTYPE` vs `ERROR.TYPE`, `ORG.OPENOFFICE.EASTERSUNDAY` vs `EASTERSUNDAY`, `ORG.OPENOFFICE.STYLE` vs `STYLE`, `ORG.OPENOFFICE.MULTIRANGE` vs `~`, `MULTIRANGE` legacy).
- `odf_docs/ODF14-KNOWN-LIMITATIONS-PUBLIC.md` notes formula cells with error results written as `office:value-type="string"` (ODF 1.4 adds `error` type) — no confirmed round-trip test for error value type.

### Existing tests that produce false confidence
- `openformula-functions.txt` lists IFNA/SKEWP as standard ODF 1.2 functions, but AOO does not implement them — a test that only checks "name in inventory" passes while implementation is absent.
- Inventories are ODF 1.2, so they do not flag ODF 1.4 regressions (INDEX row-vector, CONVERT atm, COUNTA empty cells).

## 3. Proposed versioned test inventory

| File | Version | Content | Source |
|------|---------|---------|--------|
| openformula-functions-1.4.txt | ODF 1.4 | Full normative function list from Part 4 chapter 6 | To be extracted from ODF 1.4 Part 4 |
| openformula-functions-1.3.txt | ODF 1.3 | ODF 1.3 function list | To be extracted |
| openformula-functions-1.2.txt | ODF 1.2 | Existing baseline (rename current files) | Current test files |
| openformula-operators-1.4.txt | ODF 1.4 | Operator/grammar inventory | To be extracted |
| openformula-semantics-1.4.tsv | ODF 1.4 | Conversion/error/short-circuit/array cases | To be derived from spec |

## 4. Strategy for auto-generating tests from normative data

- Parse ODF 1.4 Part 4 (or a machine-readable rendering) to extract function names, signatures, parameter classifications, and operator definitions.
- Emit versioned `openformula-functions-<ver>.txt` inventories for formulacheck.py consumption.
- Emit calcval-style cases from normative examples (each spec section's examples become expected-result cases).
- Do NOT add tests to the normal production build (per audit constraint); keep in `test/odf-validate/` or a separate conformance suite.

## 5. Gap summary

| Category | Gap | Severity |
|----------|-----|----------|
| Spec inventory | No ODF 1.3/1.4 function/operator inventory in test/ | High |
| Missing functions | IFNA, SKEWP not implemented (inventories falsely list them) | High |
| Implemented but uninventoried | EASTERSUNDAY not in either test inventory | Medium |
| Semantic tests | CONVERT atm, INDEX row-vector, DATE pre-1583, bit ops, short-circuit | High |
| Parser tests | Precedence, intersection, union, quoted labels | Medium |
| Round-trip tests | Function-name export aliases, error value type | Medium |
| False confidence | ODF 1.2 inventories listing unimplemented IFNA/SKEWP | High |
