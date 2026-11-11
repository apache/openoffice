# Apache OpenOffice Calc — OpenFormula 1.4 Compliance and Modernization Audit — Final Report

Scope: AUDIT ONLY. No production source code was modified. No dependencies introduced. No third-party formula engines. Reference = Apache OpenOffice's own implementation (main/sc, main/formula). ODF 1.4 Part 4 = OASIS Standard, 6 October 2025. Branch: `openformula` (created from repository default).

## 1. Method and constraints

- Source inspected: `main/sc/source/core/tool/{interpr1-6,compiler,parclass}.cxx`, `main/sc/inc/{interpre,compiler}.hxx`, `main/formula/inc/formula/{opcode.hxx,compiler.hrc,grammar.hxx}`, `main/formula/source/core/api/FormulaCompiler.cxx`, `main/formula/source/core/resource/core_resource.src`, `main/sc/source/ui/src/scfuncs.src`, `main/sc/source/filter/excel/xlformula.cxx`.
- AOO implementation-defined / known-limitations docs: `odf_docs/ODF14-IMPLEMENTATION-DEFINED-PUBLIC.md`, `odf_docs/ODF14-KNOWN-LIMITATIONS-PUBLIC.md`.
- Test inventories: `test/aoo-test/data/openformula-functions.txt`, `test/odf-validate/openformula-functions.txt`, `test/aoo-test/data/calcval-cases.tsv`, `test/odf-validate/{calcval-cases,calcval-cases14}.tsv`.
- Constraint honored: a function was NOT classified as missing solely because an Excel interop table uses `ocNoName`; actual source (opcode, interpr dispatch, scfuncs, core_resource) was checked.
- Limitation: the full ODF 1.4 Part 4 normative text was not available locally; requirements inventory and conformance are therefore partly derived from AOO's own implementation-defined docs and the ODF 1.2 baseline. Items requiring the spec text are marked [UNVERIFIED].

## 2. Deliverables produced (all in /home/pedro/projects/aoo/)

| # | Deliverable | Content |
|---|-------------|---------|
| 1 | openformula-requirements.tsv | Machine-readable requirements inventory (47 entries) from ODF 1.4 Part 4 chapters 2-8 and Appendix A, with ReqID, section, description, mandatory/optional, conformance group, ODF version, suggested test. |
| 2 | openformula-functions.tsv | Function inventory: every opcode/function from `compiler.hrc`/`core_resource.src` (~200 rows) with Function, StandardName, ODFVersion, Opcode, OpcodeValue, implementation source, compiler mapping, resource mapping, Status (FULL/PARTIAL/MISSING/MAPPING/UNVERIFIED), notes. Includes confirmed-missing IFNA and SKEWP. |
| 3 | openformula-operators.tsv | Operator/grammar audit: all operators, precedence, associativity, arity, parser/compiler/evaluator files, ODF section, status, notes. |
| 4 | openformula-semantics.md | Calculation semantics: conversions, empty cells, string vs numeric, error propagation, short-circuit, arrays, dates, complex numbers, precision, locale; discrepancy table. |
| 5 | openformula-odf13-14-diff.md | ODF 1.3 vs 1.4 migration audit for EASTERSUNDAY, CONVERT, COUNTA, INDEX, IS*, NPER, PMT, year 1583. |
| 6 | openformula-conformance.md | Conformance assessment: AOO claims NO Small/Medium/Large group (source: IMPLEMENTATION-DEFINED:134); coverage by group; unsupported optional vs actual failures. |
| 7 | openformula-test-gap-analysis.md | Existing test audit: outdated ODF 1.2 inventories, missing function/semantic/parser/round-trip tests, false-confidence tests, proposed versioned inventory and generation strategy. |
| 8 | openformula-modernization-plan.md | Prioritized P1-P4 plan, dependencies, complexity estimates, regression risks, definition of done. |
| 9 | openformula-final-report.md | This report. |

## 3. Actual implementation coverage

### Confirmed present (FULL) — opcodes + implementation + scfuncs + core_resource mapping

All ODF 1.2 baseline functions in `test/aoo-test/data/openformula-functions.txt` have opcodes and implementations EXCEPT IFNA and SKEWP. Notable present functions (representative, full list in `openformula-functions.tsv`):

- Logical: AND, OR, XOR, NOT, IF, CHOOSE, TRUE, FALSE, NA.
- IS*: ISBLANK, ISTEXT, ISNONTEXT, ISLOGICAL, ISREF, ISNUMBER, ISFORMULA, ISNA, ISERR, ISERROR, ISEVEN, ISODD, TYPE, CELL, N, ERROR.TYPE.
- Text: CONCATENATE, LEFT, RIGHT, MID, LEN, TRIM, UPPER, LOWER, PROPER, TEXT, VALUE, FIND, SEARCH, REPLACE, SUBSTITUTE, REPT, CHAR, CODE, UNICODE, UNICHAR, JIS, ASC, CLEAN, DOLLAR, FIXED, T.
- Math/trig: ABS, SIGN, INT, TRUNC, ROUND, ROUNDUP, ROUNDDOWN, CEILING, FLOOR, MOD, GCD, LCM, POWER, SQRT, EXP, LN, LOG, LOG10, SIN/COS/TAN/..., PI, DEGREES, RADIANS.
- Statistical: SUM, AVERAGE, COUNT, COUNTA, MIN, MAX, STDEV/VAR families, MEDIAN, MODE, RANK, PERCENTILE, LARGE/SMALL, SKEW, KURT, HARMEAN, GEOMEAN, AVEDEV, DEVSQ, etc.
- Date/time: DATE, TIME, DATEVALUE, TIMEVALUE, YEAR/MONTH/DAY/HOUR/MINUTE/SECOND, WEEKDAY, ISOWEEKNUM, EASTERSUNDAY, DAYS, DAYS360, NOW, TODAY.
- Financial: PV, FV, PMT, NPER, RATE, IPMT, PPMT, NPV, IRR, MIRR, ISPMT, SLN, DB, DDB, SYD, VDB, PDURATION, CUMIPMT, CUMPRINC, EFFECT, NOMINAL, DURATION, PRICE*, YIELD*, COUPD*, etc.
- Lookup/reference: VLOOKUP, HLOOKUP, LOOKUP, MATCH, INDEX, OFFSET, INDIRECT, ADDRESS, ROW/COLUMN/ROWS/COLUMNS/SHEET/SHEETS, AREAS, CHOOSE, HYPERLINK, GETPIVOTDATA, DDE.
- Database: DSUM, DCOUNT, DCOUNTA, DAVERAGE, DGET, DMAX, DMIN, DPRODUCT, DSTDEV*, DVAR*.
- Matrix: MDETERM, MINVERSE, MMULT, TRANSPOSE, MUNIT, SUMPRODUCT, SUMSQ, SUMX2MY2, SUMX2PY2, SUMXMY2, MVALUE.
- Bitwise: BITAND, BITOR, BITXOR, BITLSHIFT, BITRSHIFT (all present, see below).
- Engineering/Complex: COMPLEX/IM* family, BIN/DEC/HEX/OCT conversions, CONVERT, DECIMAL, BASE, ARABIC, ROMAN.
- Information: INFO, CELL, TYPE, ERROR.TYPE, FORMULA, ISBLANK/ISREF/ISFORMULA.

### Confirmed missing (MISSING)

| Function | ODF Version | Evidence |
|----------|-------------|----------|
| IFNA | ODF 1.3 | No opcode (`compiler.hrc` max = 408 = BITRSHIFT), no `ScIfNA` in any `interpr*.cxx`, no `scfuncs.src` entry, no `core_resource.src` entry. Only `main/sc/source/filter/excel/xlformula.cxx:404` `EXC_FUNCENTRY_ODF( ocNoName, 2, 2, 0, "IFNA" )` — an Excel-filter stub with `ocNoName` (no opcode). |
| SKEWP | ODF 1.3 | No opcode/implementation/metadata. Only `xlformula.cxx:415` `EXC_FUNCENTRY_ODF( ocNoName, 1, MX, 0, "SKEWP" )`. `SKEW` (sample skewness, `SC_OPCODE_SCHIEFE=325`, `interpr3.cxx:793`) is present; SKEWP (population) is not. |

### Partial implementations / semantic discrepancies

| Function/Issue | Status | Detail | Source |
|----------------|--------|--------|--------|
| CONVERT `atm` | PARTIAL | AOO uses 101324.996583 Pa; ODF 1.4 table = 1.0132510E+5 Pa (rel diff 1e-6) | `odf_docs/ODF14-KNOWN-LIMITATIONS-PUBLIC.md:72`; `interpr2.cxx` ScConvert |
| INDEX row-vector single-arg | DISCREPANCY | AOO returns column; ODF 1.4 requires error | `KNOWN-LIMITATIONS:71`; `interpr1.cxx` ScIndex |
| COUNTA / COUNTBLANK empty cells | PARTIAL/UNVERIFIED | ODF 1.4 changed empty-cell semantics; AOO `ScCount2`/`ScCountEmptyCells` not verified against ODF 1.4 | `interpr1.cxx`; `KNOWN-LIMITATIONS` 6.13.6-6.13.8 |
| BITAND/BITOR/BITXOR | PARTIAL | Exactly 2 params enforced (`MustHaveParamCount(...,2)`); ODF 1.4 parameter count to verify | `interpr1.cxx:1180` |
| BITLSHIFT/BITRSHIFT | POTENTIAL BUG | Negative shift inverts direction (`<<` for RSHIFT, `>>` for LSHIFT) at `interpr1.cxx:1231-1234`; ODF 1.4 rule to verify | `interpr1.cxx:1229-1243` |
| DATE pre-1583 | Documented impl-defined | `#VALUE!` before 15 Oct 1582; YEAR/MONTH/DAY proleptic Gregorian | `KNOWN-LIMITATIONS:74`; `IMPLEMENTATION-DEFINED:155`; `interpr2.cxx:273-277` (EASTERSUNDAY enforces 1583-9956) |

### Incorrect function mappings (MAPPING)

AOO writes non-standard names for some functions (ODF 1.4 strict mode should use standard names):

| AOO-written name | Standard name | Opcode | Source |
|------------------|---------------|--------|--------|
| `ORG.OPENOFFICE.EASTERSUNDAY` (legacy read-alias; standard `EASTERSUNDAY` is written per core_resource.src) | EASTERSUNDAY | ocEasterSunday=380 | `compiler.cxx:2542`; `core_resource.src:333` |
| `ORG.OPENOFFICE.CURRENT` | CURRENT | ocCurrent=72 | `core_resource.src:69` vs `:411` |
| `ORG.OPENOFFICE.ERRORTYPE` | ERROR.TYPE | ocErrorType=151 | `core_resource.src:146` vs `:488` |
| `ORG.OPENOFFICE.STYLE` | STYLE | ocStyle=368 | `core_resource.src:336` vs `:678` |
| `ORG.OPENOFFICE.MULTIRANGE` | `~` (union) | ocMultiArea=288 | `core_resource.src:251` vs `:257` |
| `COM.MICROSOFT.BAHTTEXT` | BAHTTEXT | ocBahtText=156 | `core_resource.src:345` vs `:687` |
| `LEGACY.NORMSDIST` / `LEGACY.NORMSINV` / `LEGACY.FDIST` / `LEGACY.CHIDIST` / `LEGACY.CHIINV` / `LEGACY.FINV` / `LEGACY.CHITEST` / `LEGACY.TDIST` | NORMSDIST/NORMSINV/FDIST/CHIDIST/CHIINV/FINV/CHITEST/TDIST | various | `core_resource.src` ODFF vs non-ODFF columns |
| `B` | BINOM.DIST.RANGE | ocB=235 | `compiler.cxx:2540` alias |

### Parser and grammar discrepancies

- Operator precedence/associativity structure present in `FormulaCompiler.cxx`: `UnionLine` -> `IntersectionLine` -> `RangeLine` -> `UnaryLine` (lines 1203-1237), matching OpenFormula (union lowest, then intersection, range, unary, binary).
- Automatic intersection: `ocSpaces` converted to `ocIntersect` at `FormulaCompiler.cxx:830-831`.
- Range list `(A1;A2)` converted to union `(A1~A2)` at `FormulaCompiler.cxx:914-922`.
- `[UNVERIFIED]` Quoted labels as implicit intersection (ODF 1.4 §3.9) — not confirmed in source.
- `[UNVERIFIED]` Formula length and nesting limits documented.

## 4. Confirmed missing functions

1. **IFNA** (ODF 1.3) — absent from opcode map, interpreter, `scfuncs.src`, `core_resource.src`; only Excel-filter `ocNoName` stub (`xlformula.cxx:404`).
2. **SKEWP** (ODF 1.3) — absent from opcode map/interpreter/`scfuncs.src`/`core_resource.src`; only Excel-filter `ocNoName` stub (`xlformula.cxx:415`). Sample `SKEW` present.

No other function in the ODF 1.2 baseline is missing from AOO's opcode map.

## 5. Partial implementations

CONVERT (`atm` unit), COUNTA/COUNTBLANK (ODF 1.4 empty-cell semantics unverified), BITAND/BITOR/BITXOR (exactly-2-params), BITLSHIFT/BITRSHIFT (negative-shift handling), INDEX (row-vector rule), all IS* functions (ODF 1.4 edge cases impl-defined). See `openformula-functions.tsv` Status column and `openformula-semantics.md`.

## 6. Incorrect function mappings

See section 3 table. The most significant for ODF 1.4 strict export: AOO's ODFF symbol map writes several `ORG.OPENOFFICE.*` and `LEGACY.*` names where ODF 1.4 standard names exist. `EASTERSUNDAY` itself is written as the standard name per `core_resource.src:333`; `ORG.OPENOFFICE.EASTERSUNDAY` (`compiler.cxx:2542`) is a legacy read-only alias.

## 7. Calculation semantic discrepancies

See `openformula-semantics.md` discrepancy table:
- DATE pre-1583 (`#VALUE!` before 15 Oct 1582; ODF 1.4 requires support from 1583, earlier impl-defined) — documented.
- CONVERT `atm` 1e-6 relative difference.
- INDEX row-vector single-argument (returns column vs ODF 1.4 error).
- Trailing empty parameter `NPER(r;p;pv;;)` error except IF — documented consistent.
- BIT op negative-shift direction (possible inversion).
- Short-circuit AND/OR unverified.

## 8. ODF 1.4 migration gaps

See `openformula-odf13-14-diff.md` gap table:
- EASTERSUNDAY: FULL, standard name works; legacy alias documented — no gap.
- CONVERT: `atm` unit mismatch — low gap.
- COUNTA: empty-cell semantics unverified — medium gap.
- INDEX: row-vector rule — high gap (behavior change, regression risk).
- IS* / NPER / PMT: present, ODF 1.4 edge cases unverified — low/medium gap.
- Year 1583: documented impl-defined, consistent — no gap.

## 9. Conformance assessment (Small / Medium / Large)

**[CONFIRMED] AOO claims NO Part 4 conformance group.** (`odf_docs/ODF14-IMPLEMENTATION-DEFINED-PUBLIC.md:134`.) Therefore no conformance level (Small/Medium/Large) can be claimed for AOO Calc.

Coverage indicators by group (presence-based, NOT semantic certification):
- Small: core logical/text/math/date/IS* functions present; IFNA/SKEWP missing (likely Medium/Large+).
- Medium: arrays, statistics, lookups present; IFNA/SKEWP missing may block Medium if mandatory in ODF 1.4.
- Large: financial, bitwise, complex, database present; IFNA/SKEWP missing.

Cannot certify any group without checking every mandatory requirement against the full ODF 1.4 Part 4 normative text. Unsupported optional capabilities are documented as implementation-defined rather than claimed failures.

## 10. Prioritized implementation plan (see openformula-modernization-plan.md)

P1: implement IFNA and SKEWP (opcodes, interpreter, scfuncs, core_resource).
P2: fix CONVERT `atm`, INDEX row-vector rule, BIT op param count / negative shift.
P3: align export names (ORG.OPENOFFICE.* / LEGACY.* vs standard).
P4: versioned test inventories (not in production build), semantic/parser/round-trip cases.

Dependencies: single opcode-block allocation for IFNA+SKEWP; INDEX change needs corpus regression test before enabling.

## 11. Estimated implementation complexity

- IFNA: Low-Medium. SKEWP: Medium. CONVERT atm: Low. INDEX row-vector: Medium (high regression risk). BIT ops: Low-Medium. Naming alignment: Medium. Test inventories: Medium. Semantic/parser/round-trip tests: Medium-High.

## 12. Regression risks

- INDEX behavior change: existing workbooks relying on column-return legacy.
- CONVERT `atm`: 1e-6 result change.
- Opcode space extension for IFNA/SKEWP.
- BIT op negative-shift fix changing prior results.
- Export name changes breaking round-trip with older AOO (mitigated by keeping legacy aliases as read-only).

## 13. Confirmed vs unverified

**Confirmed (source file:line evidence):** IFNA missing (`xlformula.cxx:404`), SKEWP missing (`xlformula.cxx:415`), EASTERSUNDAY implemented (`interpr2.cxx:261`, `compiler.cxx:2542`, `core_resource.src:333`), BIT functions implemented (`interpr1.cxx:1142-1249`, `interpr4.cxx:3769-3773`, `core_resource.src:352-356`), CONVERT `atm` discrepancy (`KNOWN-LIMITATIONS:72`), INDEX row-vector discrepancy (`KNOWN-LIMITATIONS:71`), DATE pre-1583 (`KNOWN-LIMITATIONS:74`, `IMPLEMENTATION-DEFINED:155`), no Part 4 group claimed (`IMPLEMENTATION-DEFINED:134`), test inventories are ODF 1.2 and lack EASTERSUNDAY (`test/aoo-test/data/openformula-functions.txt`, `test/odf-validate/openformula-functions.txt`).

**Unverified (require ODF 1.4 Part 4 spec text and/or runtime testing):** full per-group mandatory coverage, COUNTA empty-cell semantics, IS* edge cases, short-circuit AND/OR, array evaluation rules, quoted labels, formula length/nesting limits, numerical precision beyond 48-bit, negative-shift BIT rule, PMT/NPER ODF 1.4 conformity.

## 14. Conclusion

AOO Calc implements the large majority of the ODF 1.2 OpenFormula function set, all bitwise functions (BITAND/BITOR/BITXOR/BITLSHIFT/BITRSHIFT), and EASTERSUNDAY under the standard name (with a legacy `ORG.OPENOFFICE.EASTERSUNDAY` read-alias). Two ODF 1.3 standard functions are confirmed missing: IFNA and SKEWP. There are documented ODF 1.4 semantic discrepancies (CONVERT `atm`, INDEX row-vector, DATE pre-1583) and several AOO-extension export names that diverge from ODF 1.4 standard names. AOO claims no Small/Medium/Large Part 4 conformance group. The highest-impact, lowest-effort work is implementing IFNA and SKEWP and correcting CONVERT `atm`; the highest regression risk is the INDEX row-vector behavior change.

End of report.
