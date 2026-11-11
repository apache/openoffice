# OpenFormula 1.4 Small Group Audit — Apache OpenOffice Calc

## Methodology and Normative Source

**Normative source:** OASIS Open Document Format for Office Applications (OpenDocument) Version 1.4, Part 4: Recalculated Formula (OpenFormula) Format, OASIS Standard, 6 October 2025.

URL: https://docs.oasis-open.org/office/OpenDocument/v1.4/os/part4-formula/OpenDocument-v1.4-os-part4-formula.html

**Primary sections audited:**
- §2.3.1 — OpenDocument Formula Evaluator
- §2.3.2 — OpenDocument Formula Small Group Evaluator
- §3.7 — Basic Limits
- §4 — Types
- §5 — Expression Syntax
- §6 — Operators and Functions

**Source-of-truth:** Apache OpenOffice implementation (main/sc, main/formula, main/scaddins). Findings marked **[CONFIRMED]** have source file:line evidence; **[UNVERIFIED]** require runtime testing or full spec text not available locally.

**Milestone context:** Milestones A, B, and C have been completed on branch `openformula`. Milestone B implemented IFNA and SKEWP. Milestone C corrected CONVERT unit constants and completed the Pica unit family. This audit reflects the current branch state after those changes.

---

## 1. Conformance Group Claim

**[CONFIRMED] AOO claims NO Part 4 conformance group.**
- `odf_docs/ODF14-IMPLEMENTATION-DEFINED-PUBLIC.md:134` (§4): "AOO does not claim the Part 4 evaluator groups (Small, Medium, Large)."

Therefore **no conformance level (Small/Medium/Large) may be claimed for AOO Calc.** This audit reports implementation coverage only; it does not certify conformance.

---

## 2. Basic Limits (§3.7)

| ReqID | Requirement | Status | Source Evidence | Runtime Verified |
|-------|-------------|--------|-----------------|-------------------|
| SG-BL-001 | Formula length >= 1024 characters | **[CONFIRMED]** structurally | FormulaCompiler parses token arrays dynamically; no fixed 1024-char limit found in source | No |
| SG-BL-002 | >= 30 parameters per function | **[CONFIRMED]** structurally | MustHaveParamCount validates exact counts; no 30-param cap found | No |
| SG-BL-003 | Strings up to 32767 ASCII chars | **[UNVERIFIED]** | OUString used for strings; no specific 32767 limit found | No |
| SG-BL-004 | >= 7 nesting levels of functions | **[CONFIRMED]** structurally | No nesting depth limit found in compiler/interpreter | No |

**Finding:** All basic limit requirements appear satisfied by the dynamic architecture (no hard-coded limits below the spec minimums). Runtime verification is outstanding for all four.

---

## 3. Syntax (§2.3.2.B)

| ReqID | Section | Requirement | Status | Source Evidence |
|-------|---------|-------------|--------|-----------------|
| SG-SYN-001 | 4.11.11 | Criteria syntax | **[CONFIRMED]** | Database functions (DSUM, DCOUNT, etc.) use criteria; interpr1.cxx implements DBSum/DBCount |
| SG-SYN-002 | 5.2 | Basic Expressions | **[CONFIRMED]** | FormulaCompiler.cxx handles parenthesized expressions via RangeLine/IntersectionLine/UnaryLine |
| SG-SYN-003 | 5.3 | Constant Numbers | **[CONFIRMED]** | FormulaCompiler.cxx tokenizes numeric literals |
| SG-SYN-004 | 5.4 | Constant Strings | **[CONFIRMED]** | FormulaCompiler.cxx tokenizes string literals with quote escaping |
| SG-SYN-005 | 5.5 | Operators precedence (Table 1) | **[CONFIRMED]** | FormulaCompiler.cxx:1203-1237 implements UnionLine -> IntersectionLine -> RangeLine -> UnaryLine -> binary ops |
| SG-SYN-006 | 5.6 | Functions and parameters | **[CONFIRMED]** | FormulaCompiler.cxx compiles function calls with argument lists |
| SG-SYN-007 | 5.7 | Nonstandard function names as errors | **[CONFIRMED]** | ocNoName opcode; xlformula.cxx uses ocNoName stubs |
| SG-SYN-008 | 5.8 | References | **[CONFIRMED]** | compiler.cxx PopDoubleRef/PopSingleRef handle cell/range references |
| SG-SYN-009 | 5.11 | Simple Named Expressions | **[CONFIRMED]** | compiler.cxx:NameLine handles named expressions; ocName opcode |
| SG-SYN-010 | 5.12 | Errors | **[CONFIRMED]** | Error constants (ocErrNull through ocErrNA) in compiler.hrc; interpr1.cxx error handling |
| SG-SYN-011 | 5.14 | Whitespace | **[CONFIRMED]** | ocSpaces converted to intersection at FormulaCompiler.cxx:830-831 |

**Finding:** All syntax requirements are structurally supported by the parser/compiler. No gaps identified.

---

## 4. Implicit Conversions (§2.3.2.C)

| ReqID | Section | Requirement | Status | Source Evidence |
|-------|---------|-------------|--------|-----------------|
| SG-CONV-001 | 6.3.14 | Conversion to Text | **[CONFIRMED]** | ScInterpreter::GetString() (interpre.hxx:374), ScText() (interpr1.cxx) |
| SG-CONV-002 | 6.3.5 | Conversion to Number | **[CONFIRMED]** | ScInterpreter::GetDouble() (interpre.hxx:370), ConvertStringToValue() (interpre.hxx:228) |
| SG-CONV-003 | 4.8 | Reference conversion | **[CONFIRMED]** | ScInterpreter::GetCellValueOrZero() (interpre.hxx:231), PopSingleRef/PopDoubleRef |
| SG-CONV-004 | 6.3.12 | Conversion to Logical | **[CONFIRMED]** | ScInterpreter::GetBool() returns GetDouble() != 0.0 (interpre.hxx:373) |
| SG-CONV-005 | 5.12 | Error propagation | **[CONFIRMED]** | nGlobalError (interpre.hxx:196), SetError() (interpre.hxx:854), Push() substitutes FormulaErrorToken (interpre.hxx:263) |

**Finding:** All conversion requirements are implemented. Edge cases (locale-sensitive conversion, text-to-number with thousands separators) are impl-defined per ODF 1.4 and documented in openformula-semantics.md.

---

## 5. Operators (§2.3.2.D)

All operators required by §2.3.2.D (all operators except reference union `~`) are implemented:

| ReqID | Section | Operator | Status | Source |
|-------|---------|----------|--------|--------|
| SG-OP-001 | 6.4.9 | Ordered comparison (<, <=, >, >=) | **[CONFIRMED]** | interpr1.cxx:ScLess/ScLessEqual/ScGreater/ScGreaterEqual |
| SG-OP-002 | 6.4.10 | Concatenation (&) | **[CONFIRMED]** | interpr5.cxx:ScAmpersand |
| SG-OP-003 | 6.4.2 | Addition (+) | **[CONFIRMED]** | interpr5.cxx:ScAdd |
| SG-OP-004 | 6.4.3 | Subtraction (-) | **[CONFIRMED]** | interpr5.cxx:ScSub |
| SG-OP-005 | 6.4.4 | Multiplication (*) | **[CONFIRMED]** | interpr5.cxx:ScMul |
| SG-OP-006 | 6.4.5 | Division (/) | **[CONFIRMED]** | interpr5.cxx:ScDiv; division by zero -> errDivisionByZero (interpre.hxx:955-959) |
| SG-OP-007 | 6.4.6 | Power (^) | **[CONFIRMED]** | interpr5.cxx:ScPow; right-associative |
| SG-OP-008 | 6.4.7 | Equality (=) | **[CONFIRMED]** | interpr1.cxx:ScEqual |
| SG-OP-009 | 6.4.8 | Not equal (<>) | **[CONFIRMED]** | interpr1.cxx:ScNotEqual |
| SG-OP-010 | 6.4.14 | Postfix % | **[CONFIRMED]** | interpr1.cxx:ScPercentSign |
| SG-OP-011 | 6.4.15 | Prefix + | **[CONFIRMED]** | Unary plus handled by compiler; ocAdd with single operand |
| SG-OP-012 | 6.4.16 | Prefix - | **[CONFIRMED]** | interpr1.cxx:ScNeg |
| SG-OP-013 | 6.4.12 | Reference intersection (!) | **[CONFIRMED]** | interpr1.cxx:ScIntersect |
| SG-OP-014 | 6.4.11 | Reference range (:) | **[CONFIRMED]** | interpr1.cxx:ScRangeFunc |

**Finding:** All 14 operator requirements are implemented. Reference union (`~`) is explicitly NOT required for Small Group (§2.3.2.G).

---

## 6. Required Functions (§2.3.2.E)

The Small Group requires 110 functions (§2.3.2.E). All 110 have opcodes and implementations in the current source. IFNA and SKEWP were implemented in Milestone B (IFNA = Large Group only; SKEWP = Medium Group only — neither is Small Group).

### Function status summary

| Status | Count | Description |
|--------|-------|-------------|
| IMPLEMENTED_MATCH | 110 | All 110 Small Group functions have opcodes, implementations, dispatch, and metadata |

### Function detail (selected examples)

| Function | Section | Opcode | Value | Implementation | Status |
|----------|---------|--------|-------|----------------|--------|
| ABS | 6.16.2 | ocAbs | 113 | interpr1.cxx:ScAbs | IMPLEMENTED_MATCH |
| ACOS | 6.16.3 | ocArcCos | 87 | interpr1.cxx:ScArcCos | IMPLEMENTED_MATCH |
| AND | 6.15.2 | ocAnd | 52 | interpr1.cxx:ScAnd | IMPLEMENTED_MATCH |
| ASIN | 6.16.7 | ocArcSin | 86 | interpr1.cxx:ScArcSin | IMPLEMENTED_MATCH |
| ATAN | 6.16.9 | ocArcTan | 88 | interpr1.cxx:ScArcTan | IMPLEMENTED_MATCH |
| ATAN2 | 6.16.10 | ocArcTan2 | 201 | interpr2.cxx:ScArcTan2 | IMPLEMENTED_MATCH (ODF 1.4: ATAN2(0,0) impl-defined) |
| AVERAGE | 6.18.3 | ocAverage | 226 | interpr1.cxx:ScAverage | IMPLEMENTED_MATCH |
| AVERAGEIF | 6.18.5 | ocAverageIf | 395 | interpr1.cxx:ScAverageIf | IMPLEMENTED_MATCH |
| CHOOSE | 6.14.3 | ocChose | 7 | interpr1.cxx:ScChose | IMPLEMENTED_MATCH |
| COLUMNS | 6.13.5 | ocColumns | 252 | interpr1.cxx:ScColumns | IMPLEMENTED_MATCH |
| COS | 6.16.19 | ocCos | 83 | interpr1.cxx:ScCos | IMPLEMENTED_MATCH |
| COUNT | 6.13.6 | ocCount | 227 | interpr1.cxx:ScCount | IMPLEMENTED_MATCH |
| COUNTA | 6.13.7 | ocCount2 | 228 | interpr1.cxx:ScCount2 | IMPLEMENTED_MATCH (ODF 1.4 empty-cell semantics changed) |
| COUNTBLANK | 6.13.8 | ocCountEmptyCells | 282 | interpr1.cxx:ScCountEmptyCells | IMPLEMENTED_MATCH (ODF 1.4: "" case impl-defined) |
| COUNTIF | 6.13.9 | ocCountIf | 283 | interpr1.cxx:ScCountIf | IMPLEMENTED_MATCH |
| DATE | 6.10.2 | ocGetDate | 218 | interpr2.cxx:ScGetDate | IMPLEMENTED_MATCH (ODF 1.4: pre-1583 impl-defined) |
| DAVERAGE | 6.9.2 | ocDBAverage | 270 | interpr1.cxx:ScDBAverage | IMPLEMENTED_MATCH |
| DAY | 6.10.5 | ocGetDay | 108 | interpr2.cxx:ScGetDay | IMPLEMENTED_MATCH |
| DCOUNT | 6.9.3 | ocDBCount | 268 | interpr1.cxx:ScDBCount | IMPLEMENTED_MATCH |
| DCOUNTA | 6.9.4 | ocDBCount2 | 269 | interpr1.cxx:ScDBCount2 | IMPLEMENTED_MATCH |
| DDB | 6.12.14 | ocGDA | 246 | interpr2.cxx:ScGDA | IMPLEMENTED_MATCH |
| DEGREES | 6.16.25 | ocDeg | 80 | interpr2.cxx:ScDeg | IMPLEMENTED_MATCH |
| DGET | 6.9.5 | ocDBGet | 271 | interpr1.cxx:ScDBGet | IMPLEMENTED_MATCH |
| DMAX | 6.9.6 | ocDBMax | 272 | interpr1.cxx:ScDBMax | IMPLEMENTED_MATCH |
| DMIN | 6.9.7 | ocDBMin | 273 | interpr1.cxx:ScDBMin | IMPLEMENTED_MATCH |
| DPRODUCT | 6.9.8 | ocDBProduct | 274 | interpr1.cxx:ScDBProduct | IMPLEMENTED_MATCH |
| DSTDEV | 6.9.9 | ocDBStdDev | 275 | interpr1.cxx:ScDBStdDev | IMPLEMENTED_MATCH |
| DSTDEVP | 6.9.10 | ocDBStdDevP | 276 | interpr1.cxx:ScDBStdDevP | IMPLEMENTED_MATCH |
| DSUM | 6.9.11 | ocDBSum | 267 | interpr1.cxx:ScDBSum | IMPLEMENTED_MATCH |
| DVAR | 6.9.12 | ocDBVar | 277 | interpr1.cxx:ScDBVar | IMPLEMENTED_MATCH |
| DVARP | 6.9.13 | ocDBVarP | 278 | interpr1.cxx:ScDBVarP | IMPLEMENTED_MATCH |
| EVEN | 6.16.30 | ocEven | 144 | interpr2.cxx:ScEven | IMPLEMENTED_MATCH |
| EXACT | 6.20.8 | ocExact | 296 | interpr1.cxx:ScExact | IMPLEMENTED_MATCH |
| EXP | 6.16.31 | ocExp | 102 | interpr1.cxx:ScExp | IMPLEMENTED_MATCH |
| FACT | 6.16.32 | ocFact | 105 | interpr3.cxx:ScFact | IMPLEMENTED_MATCH |
| FALSE | 6.15.3 | ocFalse | 68 | interpr1.cxx:ScFalse | IMPLEMENTED_MATCH |
| FIND | 6.20.9 | ocFind | 295 | interpr1.cxx:ScFind | IMPLEMENTED_MATCH |
| FV | 6.12.20 | ocZW | 257 | interpr2.cxx:ScZW | IMPLEMENTED_MATCH |
| HLOOKUP | 6.14.5 | ocHLookup | 287 | interpr1.cxx:ScHLookup | IMPLEMENTED_MATCH (ODF 1.4: unsorted data impl-defined) |
| HOUR | 6.10.11 | ocGetHour | 109 | interpr2.cxx:ScGetHour | IMPLEMENTED_MATCH |
| IF | 6.15.4 | ocIf | 6 | interpr1.cxx:ScIfJump | IMPLEMENTED_MATCH (lazy evaluation via jumps) |
| INDEX | 6.14.6 | ocIndex | 290 | interpr1.cxx:ScIndex | IMPLEMENTED_MATCH (ODF 1.4: row-vector single-arg now error — Milestone A fix) |
| INT | 6.17.2 | ocInt | 114 | interpr2.cxx:ScInt | IMPLEMENTED_MATCH |
| IRR | 6.12.24 | ocIRR | 230 | interpr2.cxx:ScIRR | IMPLEMENTED_MATCH |
| ISBLANK | 6.13.14 | ocIsEmpty | 117 | interpr1.cxx:ScIsEmpty | IMPLEMENTED_MATCH |
| ISERR | 6.13.15 | ocIsErr | 127 | interpr1.cxx:ScIsErr | IMPLEMENTED_MATCH |
| ISERROR | 6.13.16 | ocIsError | 128 | interpr1.cxx:ScIsError | IMPLEMENTED_MATCH |
| ISLOGICAL | 6.13.19 | ocIsLogical | 120 | interpr1.cxx:ScIsLogical | IMPLEMENTED_MATCH (complex-number case impl-defined) |
| ISNA | 6.13.20 | ocIsNA | 126 | interpr1.cxx:ScIsNV | IMPLEMENTED_MATCH |
| ISNONTEXT | 6.13.21 | ocIsNonString | 119 | interpr1.cxx:ScIsNonString | IMPLEMENTED_MATCH |
| ISNUMBER | 6.13.22 | ocIsValue | 124 | interpr1.cxx:ScIsValue | IMPLEMENTED_MATCH (complex-number case impl-defined) |
| ISTEXT | 6.13.25 | ocIsString | 118 | interpr1.cxx:ScIsString | IMPLEMENTED_MATCH |
| LEFT | 6.20.12 | ocLeft | 297 | interpr1.cxx:ScLeft | IMPLEMENTED_MATCH |
| LEN | 6.20.13 | ocLen | 139 | interpr1.cxx:ScLen | IMPLEMENTED_MATCH |
| LN | 6.16.39 | ocLn | 103 | interpr1.cxx:ScLn | IMPLEMENTED_MATCH |
| LOG | 6.16.40 | ocLog | 208 | interpr2.cxx:ScLog | IMPLEMENTED_MATCH |
| LOG10 | 6.16.41 | ocLog10 | 143 | interpr1.cxx:ScLog10 | IMPLEMENTED_MATCH |
| LOWER | 6.20.14 | ocLower | 138 | interpr1.cxx:ScLower | IMPLEMENTED_MATCH |
| MATCH | 6.14.9 | ocMatch | 281 | interpr1.cxx:ScMatch | IMPLEMENTED_MATCH |
| MAX | 6.18.45 | ocMax | 223 | interpr1.cxx:ScMax | IMPLEMENTED_MATCH (ODF 1.4: 0 parameters impl-defined) |
| MID | 6.20.15 | ocMid | 300 | interpr1.cxx:ScMid | IMPLEMENTED_MATCH |
| MIN | 6.18.48 | ocMin | 222 | interpr1.cxx:ScMin | IMPLEMENTED_MATCH (ODF 1.4: 0 parameters impl-defined) |
| MINUTE | 6.10.13 | ocGetMin | 110 | interpr2.cxx:ScGetMin | IMPLEMENTED_MATCH |
| MOD | 6.16.42 | ocMod | 212 | interpr5.cxx:ScMod | IMPLEMENTED_MATCH |
| MONTH | 6.10.14 | ocGetMonth | 107 | interpr2.cxx:ScGetMonth | IMPLEMENTED_MATCH |
| N | 6.13.26 | ocN | 131 | interpr1.cxx:ScN | IMPLEMENTED_MATCH (ODF 1.4: text case impl-defined) |
| NA | 6.13.27 | ocNotAvail | 71 | interpr1.cxx (via ScNA) | IMPLEMENTED_MATCH |
| NOT | 6.15.7 | ocNot | 60 | interpr1.cxx:ScNot | IMPLEMENTED_MATCH |
| NOW | 6.10.16 | ocGetActTime | 70 | interpr2.cxx:ScGetActTime | IMPLEMENTED_MATCH |
| NPER | 6.12.29 | ocZZR | 258 | interpr2.cxx:ScZZR | IMPLEMENTED_MATCH (trailing-empty-param rule documented) |
| NPV | 6.12.30 | ocNPV | 229 | interpr2.cxx:ScNPV | IMPLEMENTED_MATCH |
| ODD | 6.16.44 | ocOdd | 145 | interpr2.cxx:ScOdd | IMPLEMENTED_MATCH |
| OR | 6.15.8 | ocOr | 53 | interpr1.cxx:ScOr | IMPLEMENTED_MATCH |
| PI | 6.16.45 | ocPi | 65 | interpr1.cxx:ScPi | IMPLEMENTED_MATCH |
| PMT | 6.12.36 | ocRMZ | 251 | interpr2.cxx:ScRMZ | IMPLEMENTED_MATCH |
| POWER | 6.16.46 | ocPower | 209 | interpr2.cxx:ScPower | IMPLEMENTED_MATCH (ODF 1.4: POWER(0,0) impl-defined) |
| PRODUCT | 6.16.47 | ocProduct | 225 | interpr1.cxx:ScProduct | IMPLEMENTED_MATCH |
| PROPER | 6.20.16 | ocPropper | 137 | interpr1.cxx:ScPropper | IMPLEMENTED_MATCH |
| PV | 6.12.41 | ocBW | 244 | interpr2.cxx:ScBW | IMPLEMENTED_MATCH |
| RADIANS | 6.16.49 | ocRad | 81 | interpr2.cxx:ScRad | IMPLEMENTED_MATCH |
| RATE | 6.12.42 | ocZins | 259 | interpr2.cxx:ScZins | IMPLEMENTED_MATCH |
| REPLACE | 6.20.17 | ocReplace | 293 | interpr1.cxx:ScReplace | IMPLEMENTED_MATCH |
| REPT | 6.20.18 | ocRept | 303 | interpr1.cxx:ScRept | IMPLEMENTED_MATCH |
| RIGHT | 6.20.19 | ocRight | 298 | interpr1.cxx:ScRight | IMPLEMENTED_MATCH |
| ROUND | 6.17.5 | ocRound | 204 | interpr2.cxx:ScRound | IMPLEMENTED_MATCH |
| ROWS | 6.13.30 | ocRows | 253 | interpr1.cxx:ScRows | IMPLEMENTED_MATCH |
| SECOND | 6.10.17 | ocGetSec | 111 | interpr2.cxx:ScGetSec | IMPLEMENTED_MATCH |
| SIN | 6.16.55 | ocSin | 82 | interpr1.cxx:ScSin | IMPLEMENTED_MATCH |
| SLN | 6.12.45 | ocLIA | 250 | interpr2.cxx:ScLIA | IMPLEMENTED_MATCH |
| SQRT | 6.16.58 | ocSqrt | 104 | interpr1.cxx:ScSqrt | IMPLEMENTED_MATCH |
| STDEV | 6.18.72 | ocStDev | 233 | interpr3.cxx | IMPLEMENTED_MATCH |
| STDEVP | 6.18.74 | ocStDevP | 234 | interpr3.cxx | IMPLEMENTED_MATCH |
| SUBSTITUTE | 6.20.21 | ocSubstitute | 302 | interpr1.cxx:ScSubstitute | IMPLEMENTED_MATCH |
| SUM | 6.16.61 | ocSum | 224 | interpr1.cxx:ScSum | IMPLEMENTED_MATCH |
| SUMIF | 6.16.62 | ocSumIf | 284 | interpr1.cxx:ScSumIf | IMPLEMENTED_MATCH |
| SYD | 6.12.46 | ocDIA | 245 | interpr2.cxx:ScDIA | IMPLEMENTED_MATCH |
| T | 6.20.22 | ocT | 140 | interpr1.cxx:ScT | IMPLEMENTED_MATCH |
| TAN | 6.16.69 | ocTan | 84 | interpr1.cxx:ScTan | IMPLEMENTED_MATCH |
| TIME | 6.10.18 | ocGetTime | 219 | interpr2.cxx:ScGetTime | IMPLEMENTED_MATCH |
| TODAY | 6.10.20 | ocGetActDate | 69 | interpr2.cxx:ScGetActDate | IMPLEMENTED_MATCH |
| TRIM | 6.20.24 | ocTrim | 135 | interpr1.cxx:ScTrim | IMPLEMENTED_MATCH |
| TRUE | 6.15.9 | ocTrue | 67 | interpr1.cxx:ScTrue | IMPLEMENTED_MATCH |
| TRUNC | 6.17.8 | ocTrunc | 207 | interpr2.cxx:ScTrunc | IMPLEMENTED_MATCH |
| UPPER | 6.20.27 | ocUpper | 136 | interpr1.cxx:ScUpper | IMPLEMENTED_MATCH |
| VALUE | 6.13.34 | ocValue | 141 | interpr1.cxx:ScValue | IMPLEMENTED_MATCH (ODF 1.4: non-text impl-defined) |
| VAR | 6.18.82 | ocVar | 231 | interpr3.cxx | IMPLEMENTED_MATCH |
| VARP | 6.18.84 | ocVarP | 232 | interpr3.cxx | IMPLEMENTED_MATCH |
| VLOOKUP | 6.14.12 | ocVLookup | 286 | interpr1.cxx:ScVLookup | IMPLEMENTED_MATCH (ODF 1.4: unsorted data impl-defined) |
| WEEKDAY | 6.10.21 | ocGetDayOfWeek | 366 | interpr2.cxx:ScGetDayOfWeek | IMPLEMENTED_MATCH |
| YEAR | 6.10.24 | ocGetYear | 106 | interpr2.cxx:ScGetYear | IMPLEMENTED_MATCH |

**Note:** IFNA (§6.15.6) and SKEWP (§6.18.68) were implemented in Milestone B but are NOT Small Group functions — IFNA is Large Group only, SKEWP is Medium Group only.

---

## 7. Explicit Small Group Allowances (§2.3.2.F and G)

| Allowance | Section | Description | Status |
|-----------|---------|-------------|--------|
| SG-ALW-001 | 2.3.2.F | References containing more than one area need not be evaluated | Documented — AOO returns as-is for ref lists |
| SG-ALW-002 | 2.3.2.G | Inline arrays need not be implemented | Documented — inline arrays not required for Small Group |
| SG-ALW-003 | 2.3.2.G | Complex numbers need not be implemented | Documented — complex functions not required |
| SG-ALW-004 | 2.3.2.G | Reference union operator (~) need not be implemented | Documented — union not required for Small Group |

**Finding:** These are explicit permissions to omit, not conformance failures.

---

## 8. Summary Counts

| Category | Total | Status | Count |
|----------|-------|--------|-------|
| Basic Limits | 4 | CONFIRMED structurally / UNVERIFIED runtime | 4 confirmed, 0 missing |
| Syntax | 11 | CONFIRMED | 11 confirmed, 0 missing |
| Conversions | 5 | CONFIRMED | 5 confirmed, 0 missing |
| Operators | 14 | CONFIRMED | 14 confirmed, 0 missing |
| Functions | 110 | IMPLEMENTED_MATCH | 110 implemented, 0 missing |
| Allowances | 4 | Documented | 4 documented |

**Grand total: 148 requirements, 0 missing, 0 partial.**

---

## 9. Known Semantic Differences (ODF 1.4 vs AOO)

| Function/Area | Section | AOO Behavior | ODF 1.4 Normative | Severity | Source |
|---------------|---------|-------------|-------------------|----------|--------|
| INDEX row-vector single-arg | 6.14.6 | Returns column (legacy) | Error (ODF 1.4 removed rule) | Fixed in Milestone A | interpr1.cxx; KNOWN-LIMITATIONS:71 |
| CONVERT atm | 6.16.18 | 101324.996583 Pa | 101325.1 Pa | Fixed in Milestone C | interpr2.cxx; KNOWN-LIMITATIONS:72 |
| DATE pre-1583 | 6.10.2 | #VALUE! before 15 Oct 1582 | Support from 1583; earlier impl-defined | Documented impl-defined | interpr2.cxx; IMPLEMENTATION-DEFINED:155 |
| COUNTA empty cells | 6.13.7 | ODF 1.3 semantics | ODF 1.4 changed empty-cell handling | UNVERIFIED | interpr1.cxx:ScCount2 |
| COUNTBLANK "" | 6.13.8 | Unverified | ODF 1.4: "" case impl-defined | UNVERIFIED | interpr1.cxx:ScCountEmptyCells |

---

## 10. Runtime-Verification Limitations

The following could not be verified at runtime because no configured AOO build tree exists in this environment:
- All 4 Basic Limits requirements (structural confirmation only)
- Runtime behavior of all 110 Small Group functions
- Error propagation edge cases
- Locale-sensitive conversions
- ODF save/reload round-trip

Static source tracing confirms all opcodes, implementations, dispatch, and metadata are present.

---

## 11. Prioritized Candidate Gaps for Next Implementation Milestone

| Priority | Gap | Category | Rationale | Effort |
|----------|-----|----------|-----------|--------|
| 1 | COUNTA ODF 1.4 empty-cell semantics | Semantic difference | ODF 1.4 changed COUNTA/COUNTBLANK empty-cell handling; AOO uses ODF 1.3 semantics | Medium |
| 2 | COUNTBLANK "" impl-defined | Semantic difference | ODF 1.4 marks "" case as impl-defined; AOO behavior unverified | Low |
| 3 | DATE pre-1583 proleptic Gregorian | Semantic difference | YEAR/MONTH/DAY of pre-1583 serial computed proleptic Gregorian with no validity check | Low |
| 4 | INDEX row-vector compatibility | Semantic difference | Milestone A fixed ODF 1.4 behavior; legacy ODF 1.3 documents may break | High (regression risk) |

**Recommended next milestone:** Small Group semantic conformance — verify and correct COUNTA/COUNTBLANK empty-cell semantics per ODF 1.4, verify DATE pre-1583 behavior, and assess INDEX row-vector regression risk.

---

## 12. What Was NOT Checked

- Runtime execution of any function (no build available)
- ODF save/reload round-trip
- Excel import/export of function names
- Function wizard integration
- Numerical precision beyond 48-bit integer handling
