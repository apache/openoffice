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
| SG-BL-001 | Formula length >= 1024 characters | **RUNTIME_PENDING** | No formula-text length limit; MAXCODE=2048 tokens (compiler.hxx:59), MAXSTRLEN=1024 per symbol (compiler.hxx:60). 1026-char formula needs ~1025 tokens < 2048. Single string literal limited to 1023 chars (MAXSTRLEN). | No |
| SG-BL-002 | >= 30 parameters per function | **RUNTIME_PENDING** | No global 30-param cap; MAX uses MustHaveParamCountMin(nParamCount, 1) (interpr1.cxx:3396-3400). SUM supports 255 args (calcval SG.BL-255args). Param count via GetByte() supports up to 255; MAXCODE=2048 tokens is practical ceiling. | No |
| SG-BL-003 | Strings up to 32767 ASCII chars | **RUNTIME_PENDING** | STRING_MAXLEN=65,535 (string.hxx:119, STRING32 not defined) > 32,767. CheckStringResultLen enforces at concatenation (interpre.hxx:930-936). REPT("a";32767) creates 32,767-char string at runtime. String literals limited to 1023 chars (MAXSTRLEN=1024). | No |
| SG-BL-004 | >= 7 nesting levels of functions | **RUNTIME_PENDING** | No nesting depth limit found (no MAXNEST or recursion depth guard). Parser uses recursive descent without depth guard (compiler.cxx:4134). Evaluator stack-based without depth limit. MAXCODE=2048 tokens is practical ceiling. | No |

**Finding:** All four Basic Limits requirements have source-level evidence supporting compliance, but none have been runtime-verified. No blocking limitations identified. STRING_MAXLEN=65,535 exceeds ODF's 32,767 minimum; MAXCODE=2048 tokens supports formulas with 1024+ characters when composed of short tokens.

---

## 3. Syntax (§2.3.2.B)

| ReqID | Section | Requirement | Status | Source Evidence | Runtime Verified |
|-------|---------|-------------|--------|-----------------|-------------------|
| SG-SYN-001 | 4.11.11 | Criteria syntax | **UNVERIFIED** | Database functions (DSUM, DCOUNT, etc.) use criteria; interpr1.cxx implements DBSum/DBCount | No |
| SG-SYN-002 | 5.2 | Basic Expressions | **UNVERIFIED** | FormulaCompiler.cxx handles parenthesized expressions via RangeLine/IntersectionLine/UnaryLine | No |
| SG-SYN-003 | 5.3 | Constant Numbers | **UNVERIFIED** | FormulaCompiler.cxx tokenizes numeric literals | No |
| SG-SYN-004 | 5.4 | Constant Strings | **UNVERIFIED** | FormulaCompiler.cxx tokenizes string literals with quote escaping | No |
| SG-SYN-005 | 5.5 | Operators precedence (Table 1) | **UNVERIFIED** | FormulaCompiler.cxx:1203-1237 implements UnionLine -> IntersectionLine -> RangeLine -> UnaryLine -> binary ops | No |
| SG-SYN-006 | 5.6 | Functions and parameters | **UNVERIFIED** | FormulaCompiler.cxx compiles function calls with argument lists | No |
| SG-SYN-007 | 5.7 | Nonstandard function names as errors | **UNVERIFIED** | ocNoName opcode; xlformula.cxx uses ocNoName stubs | No |
| SG-SYN-008 | 5.8 | References | **UNVERIFIED** | compiler.cxx PopDoubleRef/PopSingleRef handle cell/range references | No |
| SG-SYN-009 | 5.11 | Simple Named Expressions | **UNVERIFIED** | compiler.cxx:NameLine handles named expressions; ocName opcode | No |
| SG-SYN-010 | 5.12 | Errors | **UNVERIFIED** | Error constants (ocErrNull through ocErrNA) in compiler.hrc; interpr1.cxx error handling | No |
| SG-SYN-011 | 5.14 | Whitespace | **UNVERIFIED** | ocSpaces converted to intersection at FormulaCompiler.cxx:830-831 | No |

**Specific unverified grammar cases:**
- Operator precedence and associativity (e.g., `2+3*4`, `2^3^2`) — parser structure exists but no runtime verification of correct precedence
- Automatic intersection (whitespace between references) — ocSpaces → ocIntersect conversion at FormulaCompiler.cxx:830-831 exists but runtime behavior unverified
- Reference union `~` — NOT required for Small Group (§2.3.2.G), so absence is acceptable
- Quoted labels as implicit intersection (ODF 1.4 §3.9/§5.10) — not confirmed in source

**Finding:** All syntax requirements have parser/compiler infrastructure present, but none have been verified at runtime against ODF 1.4 normative expected results.

---

## 4. Implicit Conversions (§2.3.2.C)

| ReqID | Section | Requirement | Status | Source Evidence | Runtime Verified |
|-------|---------|-------------|--------|-----------------|-------------------|
| SG-CONV-001 | 6.3.14 | Conversion to Text | **IMPLEMENTED_UNVERIFIED** | ScInterpreter::GetString() (interpre.hxx:374), ScText() (interpr1.cxx) | No |
| SG-CONV-002 | 6.3.5 | Conversion to Number | **IMPLEMENTED_UNVERIFIED** | ScInterpreter::GetDouble() (interpre.hxx:370), ConvertStringToValue() (interpre.hxx:228) | No |
| SG-CONV-003 | 4.8 | Reference conversion | **IMPLEMENTED_UNVERIFIED** | ScInterpreter::GetCellValueOrZero() (interpre.hxx:231), PopSingleRef/PopDoubleRef | No |
| SG-CONV-004 | 6.3.12 | Conversion to Logical | **IMPLEMENTED_UNVERIFIED** | ScInterpreter::GetBool() returns GetDouble() != 0.0 (interpre.hxx:373) | No |
| SG-CONV-005 | 5.12 | Error propagation | **IMPLEMENTED_UNVERIFIED** | nGlobalError (interpre.hxx:196), SetError() (interpre.hxx:854), Push() substitutes FormulaErrorToken (interpre.hxx:263) | No |

**Distinction:** Implementation presence confirmed (conversion functions exist in source), but semantic conformance to ODF 1.4 conversion rules (locale sensitivity, edge cases, error conditions) is **unverified**.

**Finding:** All conversion requirements have implementation entry points, but none have been verified at runtime against ODF 1.4 normative expected results.

---

## 5. Operators (§2.3.2.D)

All operators required by §2.3.2.D (all operators except reference union `~`) are implemented:

| ReqID | Section | Operator | Status | Source |
|-------|---------|----------|--------|--------|
| SG-OP-001 | 6.4.9 | Ordered comparison (<, <=, >, >=) | **IMPLEMENTED_UNVERIFIED** | interpr1.cxx:ScLess/ScLessEqual/ScGreater/ScGreaterEqual | No |
| SG-OP-002 | 6.4.10 | Concatenation (&) | **IMPLEMENTED_UNVERIFIED** | interpr5.cxx:ScAmpersand | No |
| SG-OP-003 | 6.4.2 | Addition (+) | **IMPLEMENTED_UNVERIFIED** | interpr5.cxx:ScAdd | No |
| SG-OP-004 | 6.4.3 | Subtraction (-) | **IMPLEMENTED_UNVERIFIED** | interpr5.cxx:ScSub | No |
| SG-OP-005 | 6.4.4 | Multiplication (*) | **IMPLEMENTED_UNVERIFIED** | interpr5.cxx:ScMul | No |
| SG-OP-006 | 6.4.5 | Division (/) | **IMPLEMENTED_UNVERIFIED** | interpr5.cxx:ScDiv; division by zero -> errDivisionByZero (interpre.hxx:955-959) | No |
| SG-OP-007 | 6.4.6 | Power (^) | **IMPLEMENTED_UNVERIFIED** | interpr5.cxx:ScPow; right-associative | No |
| SG-OP-008 | 6.4.7 | Equality (=) | **IMPLEMENTED_UNVERIFIED** | interpr1.cxx:ScEqual | No |
| SG-OP-009 | 6.4.8 | Not equal (<>) | **IMPLEMENTED_UNVERIFIED** | interpr1.cxx:ScNotEqual | No |
| SG-OP-010 | 6.4.14 | Postfix % | **IMPLEMENTED_UNVERIFIED** | interpr1.cxx:ScPercentSign | No |
| SG-OP-011 | 6.4.15 | Prefix + | **IMPLEMENTED_UNVERIFIED** | Unary plus handled by compiler; ocAdd with single operand | No |
| SG-OP-012 | 6.4.16 | Prefix - | **IMPLEMENTED_UNVERIFIED** | interpr1.cxx:ScNeg | No |
| SG-OP-013 | 6.4.12 | Reference intersection (!) | **IMPLEMENTED_UNVERIFIED** | interpr1.cxx:ScIntersect | No |
| SG-OP-014 | 6.4.11 | Reference range (:) | **IMPLEMENTED_UNVERIFIED** | interpr1.cxx:ScRangeFunc | No |

**Finding:** All 14 operator requirements are implemented. Reference union (`~`) is explicitly NOT required for Small Group (§2.3.2.G).

---

## 6. Required Functions (§2.3.2.E)

The Small Group requires 110 functions (§2.3.2.E). All 110 have opcodes and implementations in the current source. IFNA and SKEWP were implemented in Milestone B (IFNA = Large Group only; SKEWP = Medium Group only — neither is Small Group).

### Function status summary

| Status | Count | Description |
|--------|-------|-------------|
| IMPLEMENTED_UNVERIFIED | 110 | All 110 Small Group functions have opcodes, implementations, dispatch entries, and metadata, but runtime semantics have NOT been verified against ODF 1.4. Implementation presence confirmed; semantic equivalence not established. |
| IMPLEMENTED_MATCH | 0 | No function has been verified as semantically matching ODF 1.4 at runtime. |
| PARTIAL | 0 | None identified in this audit. |
| MISSING | 0 | All 110 required functions are present in source. |
| MAPPING_ONLY | 0 | None identified in this audit. |
| NOT_APPLICABLE | 4 | The four explicit allowances (§2.3.2.F/G) - inline arrays, complex numbers, multi-area references, reference union (~). |

**Key conceptual distinction:** An opcode, evaluator implementation, dispatch entry, and function metadata establish **implementation presence**, not **semantic equivalence**. All 110 functions are marked IMPLEMENTED_UNVERIFIED because no runtime tests have been executed against the ODF 1.4 normative expected results.

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
| Basic Limits | 4 | Awaiting runtime | 0 verified, 4 awaiting |
| Syntax | 11 | Awaiting runtime | 0 verified, 11 awaiting (SG-SYN-001 criteria §4.11.11 now included in E.1 test inventory) |
| Conversions | 5 | IMPLEMENTED_UNVERIFIED | 0 verified, 5 UNVERIFIED |
| Operators | 14 | IMPLEMENTED_UNVERIFIED | 0 verified, 14 UNVERIFIED |
| Functions | 110 | IMPLEMENTED_UNVERIFIED | 0 verified, 110 UNVERIFIED |
| Allowances | 4 | NOT_APPLICABLE | 4 documented (§2.3.2.F/G explicit permissions to omit) |

**Grand total: 148 inventory entries — 144 applicable requirements awaiting runtime verification (Basic Limits 4 + Syntax 11 + Conversions 5 + Operators 14 + Functions 110), 4 explicit allowances marked NOT_APPLICABLE (§2.3.2.F/G: inline arrays, complex numbers, multi-area references, reference union), 0 runtime-verified.**

---

## 9. Known Semantic Differences (ODF 1.4 vs AOO)

| Function/Area | Section | AOO Behavior | ODF 1.4 Normative | Severity | Source |
|---------------|---------|-------------|-------------------|----------|--------|
| INDEX row-vector single-arg | 6.14.6 | Returns error (Milestone A fix: removed ODF 1.3 row-vector shortcut) | Error (ODF 1.4 removed rule) | Fixed in Milestone A; regression risk | interpr1.cxx; KNOWN-LIMITATIONS:71 |
| CONVERT atm | 6.16.18 | 101325.1 Pa (Milestone C fix: corrected from 101324.996583 Pa) | 101325.1 Pa | Fixed in Milestone C | interpr2.cxx; KNOWN-LIMITATIONS:72 |
| DATE pre-1583 (FN-016-DATE-pre1583) | 6.10.2 | AOO observed #VALUE! for `DATE(1500;1;1)` (outside 1904 ≤ Year ≤ 9956) | §6.10.2 constrains Year/Month/Day; evaluators may evaluate out-of-constraint but `DATE(1500;1;1)` is implementation-defined/diagnostic, not a mandatory `#ERR`. Normative expected value is `-` (not an ODF conformance error assertion); calcval `DATE.pre1583` retains `#ERR` only as explicitly identified AOO-specific diagnostic. | Impl-defined/diagnostic (E.1 reclassified) | interpr2.cxx; IMPLEMENTATION-DEFINED:155 |
| COUNTA empty cells / empty string | 6.13.7 | ODF 1.3 semantics | ODF 1.4 §6.13.7: counts non-blank cells; empty string `""` is not blank. E.1 adds fixtures for empty cells and `""` (where harness-expressible). | UNVERIFIED | interpr1.cxx:ScCount2 |
| COUNTBLANK "" in cell | 6.13.8 | Unverified | §6.13.8: implementation-defined whether a cell returning `""` is blank. `FN-014-COUNTBLANK-empty-cells` covers truly empty cells; `FN-014-COUNTBLANK-empty-string` retained as diagnostic only with expected `-` (NOT testing `""`-in-cell). `""`-in-cell blankness is impl-defined; calcval harness cannot place `""` into a data cell. | Impl-defined/UNVERIFIED | interpr1.cxx:ScCountEmptyCells |

---



## E.1 Corrections (this milestone)

- **SG-BL-001**: corrected to formula expression length 1026 chars (`=1` + `+1`*512), expected 513 (replaces `REPT("a";1024)` which measured string length, not formula length).
- **SG-BL-004**: corrected to 7 genuinely nested function levels (`IF(TRUE;IF(...IF(TRUE;1;2)...))`, 7 nested IF calls), balanced parens, deterministic result 1.
- **All 4 Basic Limits (§3.7)**: Reclassified from MET/PARTIAL to RUNTIME_PENDING based on G.5 source audit. Source evidence supports compliance for all four requirements (no formula-text cap, no 30-param cap, STRING_MAXLEN=65,535 > 32,767, no nesting depth limit), but runtime verification blocked (no AOO build tree; calcval macOS-only).
- **SG-SYN-001**: added Criteria syntax §4.11.11 test `SYN-001-criteria` (`=DCOUNT(R1:T6;;V1:V2)`, expected 4) using existing calcval database/criteria fixtures.
- **SG-SYN-009**: marked not directly executable in calcval harness (requires pre-defined named expression `MYNAME`); test remains UNVERIFIED with explicit note.
- **SG-FN-013/COUNTA**: fixtures added for truly empty cells and empty string "" per §6.13.7 (empty string non-blank). Harness-expressible via inline args; empty-cell cases use unset D6:D7.
- **SG-FN-014/COUNTBLANK**: FN-014-COUNTBLANK-empty-cells (=COUNTBLANK(D6:D7), expected 2) covers truly empty cells. FN-014-COUNTBLANK-empty-string is retained as diagnostic only with expected - — it uses =COUNTBLANK(D6:D7) (genuinely empty cells, not ""), clearly labeled as NOT testing a cell containing ""; the required ""-in-cell fixture cannot be established by calcval harness (scratch cleared each case; no pre-populated "" data cells). ""-in-cell blankness is impl-defined per §6.13.8.
- **SG-FN-016/FN-016-DATE-pre1583**: reclassified as implementation-defined/diagnostic (out-of-constraint Year < 1904), not mandatory #ERR. Normative expected value is - (not an ODF conformance error assertion). The calcval DATE.pre1583 row retains #ERR only as an explicitly identified AOO-specific diagnostic assertion (AOO observed #VALUE!), clearly labeled as not an ODF conformance assertion.
- **BITAND arity**: confirmed `BITAND.3params` (`=BITAND(6;3;1)` expected `#ERR`) remains valid per §6.6.2 (`BITAND(Integer X; Integer Y)`); no classification change.
- **Runtime verification**: not performed (no AOO build tree; calcval harness macOS-only). Only static/syntax/fixture checks applied.

## 10. Runtime-Verification Limitations

The following could not be verified at runtime because no configured AOO build tree exists in this environment:
- All 4 Basic Limits (§3.7) — source evidence supports compliance but runtime execution not performed
- Runtime behavior of all 110 Small Group functions
- Error propagation edge cases
- Locale-sensitive conversions
- ODF save/reload round-trip

Static source tracing confirms all opcodes, implementations, dispatch, and metadata are present, and no blocking limits found for Basic Limits requirements.

---

## 11. Prioritized Candidate Gaps for Next Implementation Milestone

| Priority | Gap | Category | Rationale | Effort |
|----------|-----|----------|-----------|--------|
| 1 | COUNTA ODF 1.4 empty-cell semantics | Semantic difference | ODF 1.4 changed COUNTA/COUNTBLANK empty-cell handling; AOO uses ODF 1.3 semantics | Medium |
| 2 | COUNTBLANK "" impl-defined | Semantic difference | ODF 1.4 marks "" case as impl-defined; AOO behavior unverified | Low |
| 3 | DATE out-of-constraint/pre-1583 behavior | Semantic difference (impl-defined) | `DATE(1500;1;1)` outside §6.10.2 constraints; result is impl-defined/diagnostic (AOO observed #VALUE!), not mandatory error. | Low |
| 4 | INDEX row-vector compatibility | Semantic difference | Milestone A fixed ODF 1.4 behavior; legacy ODF 1.3 documents may break | High (regression risk) |

**Recommended next milestone:** Small Group semantic conformance — verify and correct COUNTA/COUNTBLANK empty-cell semantics per ODF 1.4, verify DATE pre-1583 behavior, and assess INDEX row-vector regression risk.

---

## 12. What Was NOT Checked

- Runtime execution of any function (no build available)
- ODF save/reload round-trip
- Excel import/export of function names
- Function wizard integration
- Numerical precision beyond 48-bit integer handling
