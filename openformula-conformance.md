# OpenFormula Conformance Assessment — Apache OpenOffice Calc

Source-of-truth: AOO source (main/sc) and `odf_docs/ODF14-IMPLEMENTATION-DEFINED-PUBLIC.md`. Findings marked **[CONFIRMED]** have source:line evidence; **[UNVERIFIED]** require the full ODF 1.4 Part 4 normative text and/or runtime testing.

## 1. Conformance-group claim

**[CONFIRMED] AOO claims NO Part 4 conformance group.**
- `odf_docs/ODF14-IMPLEMENTATION-DEFINED-PUBLIC.md:134` (§4, Part 4): "AOO does not claim the Part 4 evaluator groups (Small, Medium, Large), and Part 4 contains no requirement that a producer document these values; §2.4 only says applications 'should document all implementation-defined variances'. The 58 sections that contain the words are listed, with no AOO value except for §7.4. No Part 4 evaluator group is claimed, as for 1.3."

Therefore **no conformance level (Small/Medium/Large) may be claimed for AOO Calc.** This audit reports implementation coverage only; it does not certify conformance.

## 2. Coverage of mandatory functions by group (based on function inventory)

The ODF 1.2 function baseline (used by AOO's own `test/aoo-test/data/openformula-functions.txt` and `test/odf-validate/openformula-functions.txt`, titled "OpenDocument 1.2 Part 2 chapter 6") lists 414 function names. AOO's opcode map (`main/formula/inc/formula/compiler.hrc`, last opcode id 408) covers all of those except two that appear only as Excel-filter `ocNoName` stubs:

| Function | Status | Evidence |
|----------|--------|----------|
| IFNA | MISSING | No opcode (compiler.hrc max = 408 = BITRSHIFT), no `ScIfNA`, no `scfuncs.src` entry, no `core_resource.src` entry. Only `main/sc/source/filter/excel/xlformula.cxx:404` `EXC_FUNCENTRY_ODF( ocNoName, 2, 2, 0, "IFNA" )`. |
| SKEWP | MISSING | No opcode, no `ScSkewP`, no `scfuncs.src`/`core_resource.src` entry. Only `xlformula.cxx:415` `EXC_FUNCENTRY_ODF( ocNoName, 1, MX, 0, "SKEWP" )`. `SKEW` (sample skewness, `SC_OPCODE_SCHIEFE=325`) is present. |

All other ODF 1.2 baseline functions have opcodes, implementations in `interpr*.cxx`, `scfuncs.src` metadata, and `core_resource.src` name mappings (see `openformula-functions.tsv`).

**[UNVERIFIED]** Whether every mandatory function in each conformance group (Small/Medium/Large) is semantically correct per ODF 1.4. Presence of an opcode does not imply correct semantics (per audit mandate).

## 3. Small Group

**[UNVERIFIED]** Cannot certify Small Group. AOO does not claim it. Coverage indicators:
- Core IS*/logical/text/math/date/financial functions largely present (opcodes exist).
- Confirmed missing from baseline: IFNA, SKEWP (both are ODF 1.3+, likely Medium/Large or optional depending on the group definitions in ODF 1.4 Part 4).
- Operators present (range, intersection, union, arithmetic, comparison, logical, concatenation, percent).

## 4. Medium Group

**[UNVERIFIED]** Cannot certify Medium Group. Coverage indicators:
- Array functions (MMULT, MDETERM, MINVERSE, TRANSPOSE, MUNIT, SUMPRODUCT) present.
- Statistical/distribution functions present.
- Lookup/reference functions (VLOOKUP, HLOOKUP, INDEX, MATCH, OFFSET, INDIRECT) present.
- IFNA and SKEWP missing (may be Medium-group mandatory in ODF 1.4).

## 5. Large Group

**[UNVERIFIED]** Cannot certify Large Group. Coverage indicators:
- Financial functions (PV, FV, PMT, NPER, RATE, IPMT, PPMT, NPV, IRR, MIRR, etc.) present.
- Engineering/bitwise (BITAND/BITOR/BITXOR/BITLSHIFT/BITRSHIFT) present.
- Complex-number (IM*) functions present.
- Database (D*) functions present.

## 6. Unsupported optional capabilities vs actual failures

**[CONFIRMED]** Per odf_docs §4, AOO documents implementation-defined variances rather than claiming groups. The following are **documented implementation-defined / known-limitation items**, not necessarily conformance failures (since no group is claimed):

| Item | AOO behavior | ODF 1.4 status | Source |
|------|-------------|----------------|--------|
| DATE pre-1583 | #VALUE! before 15 Oct 1582 | impl-defined (§7.4) | IMPLEMENTATION-DEFINED:155; KNOWN-LIMITATIONS:74 |
| CONVERT atm | 101324.996583 Pa | ODF 1.4 table = 1.0132510E+5 | KNOWN-LIMITATIONS:72 |
| INDEX row-vector single-arg | returns column | ODF 1.4: error | KNOWN-LIMITATIONS:71 |
| Trailing empty param (`NPER(;;)`) | error except IF | ODF 1.4 rule | KNOWN-LIMITATIONS:73 |
| Error cells on save | written as `office:value-type="string"` | ODF 1.4 adds `error` type (no `office:error` attr) | KNOWN-LIMITATIONS producer:2 |

## 7. Conformance conclusion

- **Claimed conformance group: none.** AOO does not claim Small/Medium/Large.
- **Confirmed missing mandatory-candidate functions:** IFNA, SKEWP.
- **Confirmed semantic discrepancies vs ODF 1.4:** CONVERT `atm` (1e-6 relative), INDEX row-vector rule, EASTERSUNDAY naming alias (behavior correct).
- **Cannot certify any group** without checking every mandatory requirement against the ODF 1.4 Part 4 normative text, which was not available in full for this audit.

## 8. Recommended next step for conformance

Obtain ODF 1.4 Part 4, enumerate the mandatory function/operator set per group, and execute each against AOO with expected results. Until then, report only coverage (see `openformula-functions.tsv`) and discrepancies (see `openformula-semantics.md`, `openformula-odf13-14-diff.md`).
