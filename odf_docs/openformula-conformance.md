# OpenFormula Conformance Assessment — Apache OpenOffice Calc

Source-of-truth: AOO source (main/sc) and `odf_docs/ODF14-IMPLEMENTATION-DEFINED-PUBLIC.md`. Findings marked **[CONFIRMED]** have source:line evidence; **[UNVERIFIED]** require the full ODF 1.4 Part 4 normative text and/or runtime testing.

## 1. Conformance-group claim

**[CONFIRMED] AOO claims NO Part 4 conformance group.**
- `odf_docs/ODF14-IMPLEMENTATION-DEFINED-PUBLIC.md:134` (§4, Part 4): "AOO does not claim the Part 4 evaluator groups (Small, Medium, Large), and Part 4 contains no requirement that a producer document these values; §2.4 only says applications 'should document all implementation-defined variances'. The 58 sections that contain the words are listed, with no AOO value except for §7.4. No Part 4 evaluator group is claimed, as for 1.3."

Therefore **no conformance level (Small/Medium/Large) may be claimed for AOO Calc.** This audit reports implementation coverage only; it does not certify conformance.

## 2. Coverage of mandatory functions by group

The ODF 1.2 function baseline (used by AOO's own `test/aoo-test/data/openformula-functions.txt` and `test/odf-validate/openformula-functions.txt`, titled "OpenDocument 1.2 Part 2 chapter 6") lists 414 function names. AOO's opcode map (`main/formula/inc/formula/compiler.hrc`, last opcode id 410 = SKEWP) covers all of those. IFNA (opcode 409) and SKEWP (opcode 410) were implemented in Milestone B and are no longer missing.

All 414 baseline functions have opcodes, implementations in `interpr*.cxx`, `scfuncs.src` metadata, and `core_resource.src` name mappings (see `openformula-functions.tsv`).

**[CONFIRMED] IFNA and SKEWP are now implemented (Milestone B).**
- IFNA: `SC_OPCODE_IFNA=409` in `compiler.hrc`, `ocIfna` in `opcode.hxx`, `ScInterpreter::ScIfna()` in `interpr1.cxx:392`, dispatch `case ocIfna` in `interpr4.cxx:3451`, `scfuncs.src:1027`, `core_resource.src:357`.
- SKEWP: `SC_OPCODE_SKEWP=410` in `compiler.hrc`, `ocSkewP` in `opcode.hxx`, `ScInterpreter::ScSkewP()` in `interpr3.cxx`, dispatch `case ocSkewP` in `interpr4.cxx:3695`, `scfuncs.src:5300`, `core_resource.src:358`.

**[UNVERIFIED]** Whether every mandatory function in each conformance group (Small/Medium/Large) is semantically correct per ODF 1.4. Presence of an opcode does not imply correct semantics (per audit mandate).

## 3. Small Group

AOO does not claim the Small Group. However, the Small Group audit (`openformula-small-group-audit.md`) found that all 110 required Small Group functions (§2.3.2.E) are implemented with opcodes, dispatch, and metadata. Coverage indicators:

- Core IS*/logical/text/math/date/financial functions present and implemented (Milestone B added IFNA/SKEWP, though neither is Small Group).
- All Small Group operators present (range, intersection, arithmetic, comparison, logical, concatenation, percent).
- No Small Group functions are missing.
- **[UNVERIFIED]** Semantic correctness per ODF 1.4 not runtime-tested; see `openformula-small-group-audit.md` for details.

## 4. Medium Group

**[UNVERIFIED]** Cannot certify Medium Group. AOO does not claim it. Coverage indicators:
- Array functions (MMULT, MDETERM, MINVERSE, TRANSPOSE, MUNIT, SUMPRODUCT) present.
- Statistical/distribution functions present.
- Lookup/reference functions (VLOOKUP, HLOOKUP, INDEX, MATCH, OFFSET, INDIRECT) present.
- SKEWP implemented in Milestone B (`SC_OPCODE_SKEWP=410`, `ScSkewP` in `interpr3.cxx`) — SKEWP is a Medium Group function (§2.3.3 Medium Group), not Small Group.

## 5. Large Group

**[UNVERIFIED]** Cannot certify Large Group. AOO does not claim it. Coverage indicators:
- Financial functions (PV, FV, PMT, NPER, RATE, IPMT, PPMT, NPV, IRR, MIRR, etc.) present.
- Engineering/bitwise (BITAND/BITOR/BITXOR/BITLSHIFT/BITRSHIFT) present.
- Complex-number (IM*) functions present.
- Database (D*) functions present.
- IFNA implemented in Milestone B (`SC_OPCODE_IFNA=409`, `ScIfna` in `interpr1.cxx`) — IFNA is a Large Group function (§2.3.4 Large Group), not Small Group.

## 6. Unsupported optional capabilities vs actual failures

**[CONFIRMED]** Per odf_docs §4, AOO documents implementation-defined variances rather than claiming groups. The following are **documented implementation-defined / known-limitation items**, not necessarily conformance failures (since no group is claimed):

| Item | AOO behavior | ODF 1.4 status | Source |
|------|-------------|----------------|--------|
| DATE pre-1583 | #VALUE! before 15 Oct 1582 | impl-defined (§7.4) | IMPLEMENTATION-DEFINED:155; KNOWN-LIMITATIONS:74 |
| CONVERT atm | Fixed: 101325.1 Pa | Matches ODF 1.4 table | Fixed in Milestone C; KNOWN-LIMITATIONS:72 |
| INDEX row-vector single-arg | Fixed: error per ODF 1.4 | Error (ODF 1.4 removed rule) | Fixed in Milestone A; KNOWN-LIMITATIONS:71 |
| Trailing empty param (`NPER(;;)`) | error except IF | ODF 1.4 rule | KNOWN-LIMITATIONS:73 |
| Error cells on save | written as `office:value-type="string"` | ODF 1.4 adds `error` type (no `office:error` attr) | KNOWN-LIMITATIONS producer:2 |

## 7. Conformance conclusion

- **Claimed conformance group: none.** AOO does not claim Small/Medium/Large.
- **Confirmed missing functions: none.** All 110 Small Group functions (§2.3.2.E) are implemented. IFNA (Large Group) and SKEWP (Medium Group) were implemented in Milestone B.
- **Confirmed semantic discrepancies vs ODF 1.4:** INDEX row-vector rule (corrected in Milestone A), CONVERT `atm` (corrected in Milestone C), EASTERSUNDAY naming alias (behavior correct).
- **Cannot certify any group** without runtime testing every mandatory requirement against the ODF 1.4 Part 4 normative text.

## 8. Recommended next step for conformance

Runtime-test all 110 Small Group functions against ODF 1.4 Part 4 expected results. Prioritize: COUNTA/COUNTBLANK empty-cell semantics (ODF 1.4 changed), DATE pre-1583 proleptic Gregorian, and INDEX row-vector regression risk. Until runtime verification is complete, report only coverage (see `openformula-small-group-1.4.tsv` and `openformula-small-group-audit.md`) and discrepancies (see `openformula-semantics.md`, `openformula-odf13-14-diff.md`).
