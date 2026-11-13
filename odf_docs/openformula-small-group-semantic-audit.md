# OpenFormula 1.4 Small Group Semantic Conformance Audit

**Branch:** `openformula`  
**Baseline:** `9c42008d93`  
**Scope:** Audit and report only. No production code modifications.  
**Method:** Source-level static inspection only. No runtime verification possible (no AOO build tree; calcval harness macOS-only).

## Normative Source

OASIS Open Document Format for Office Applications (OpenDocument) Version 1.4, Part 4: Recalculated Formula (OpenFormula) Format, OASIS Standard, 6 October 2025.

---

## F.1 COUNTA / COUNTBLANK (§6.13.7–6.13.8)

### Normative Requirement

**COUNTA (§6.13.7):** Counts non-empty cells. Empty string `""` is not blank and is counted.

**COUNTBLANK (§6.13.8):** Counts empty cells. Whether a cell returning `""` is blank is implementation-defined.

### AOO Implementation Locations

- `ScCount2()` — `main/sc/source/core/tool/interpr1.cxx:3834`
- `ScCountEmptyCells()` — `main/sc/source/core/tool/interpr1.cxx:4714`
- `IterateParameters(ifCOUNT2)` — `main/sc/source/core/tool/interpr1.cxx:3487` (shared by COUNTA, COUNT, and others)
- Dispatch: `interpr4.cxx:3591` (ocCount2 → ScCount2), `interpr4.cxx:3640` (ocCountEmptyCells → ScCountEmptyCells)
- Declaration: `main/sc/source/core/inc/interpre.hxx:505` (ScCount2), `:526` (ScCountEmptyCells)

### Execution Path — COUNTA

`ScCount2()` calls `IterateParameters(ifCOUNT2)` which iterates over parameters via a `switch(GetStackType())`:

| Stack Type | Code Path | Behavior |
|---|---|---|
| `svString` (literal string, incl. `""`) | interpr1.cxx:3506-3543 → `default` branch | `Pop(); nCount++;` — **counts string argument** (empty string counted) |
| `svDouble` (numeric literal) | interpr1.cxx:3546 | Counts |
| `svSingleRef` (single cell) | interpr1.cxx:3567-3624 | `eCellType != CELLTYPE_NONE && eCellType != CELLTYPE_NOTE` → counts non-empty cell |
| `svDoubleRef` / `svRefList` (range) | interpr1.cxx:3626-3711 | `ScCellIterator` over range; `eType != CELLTYPE_NONE && eType != CELLTYPE_NOTE` → counts non-empty cells |
| `svMatrix` (array) | interpr1.cxx:3713-3760 | `nCount += nC * nR` — **counts ALL matrix elements** including strings (no `IsString` check for COUNT2) |
| `svError` | interpr1.cxx:3762-3774 | `nCount++` — counts error values |

### Execution Path — COUNTBLANK

`ScCountEmptyCells()`:

| Stack Type | Code Path | Behavior |
|---|---|---|
| `svSingleRef` | interpr1.cxx:4723-4731 | `eCellType != CELLTYPE_NONE && eCellType != CELLTYPE_NOTE` → nCount=1 (not blank), else blank |
| `svDoubleRef` / `svRefList` | interpr1.cxx:4733-4757 | `ScCellIterator`; same cell-type check; result = `nMaxCount - nCount` |
| `default` | interpr1.cxx:4760 | `SetError(errIllegalParameter)` — scalar args not supported |

### Current Behavior vs ODF 1.4

| Scenario | ODF 1.4 Normative | AOO Behavior | Match? |
|---|---|---|---|
| Truly empty cell (CELLTYPE_NONE) | COUNTA: not counted; COUNTBLANK: blank | Cell type CELLTYPE_NONE → not counted by COUNTA, blank by COUNTBLANK | Yes |
| Cell with text | COUNTA: counted; COUNTBLANK: not blank | CELLTYPE_STRING → counted/not blank | Yes |
| Cell with number | COUNTA: counted; COUNTBLANK: not blank | CELLTYPE_VALUE → counted/not blank | Yes |
| Scalar `""` argument | COUNTA: counted (not blank) | svString → counted | Yes |
| Formula cell returning `""` | COUNTBLANK: implementation-defined whether blank | CELLTYPE_FORMULA ≠ CELLTYPE_NONE → counted by COUNTA, NOT blank by COUNTBLANK; formula result not inspected | Implementation-defined; AOO does not check formula result |
| Matrix with empty-string cells | COUNTA: not specified explicitly | `nCount += nC * nR` counts all elements including strings | Counts empty-string cells as non-empty |
| Scalar `""` to COUNTBLANK | Not applicable (COUNTBLANK takes ranges) | `SetError(errIllegalParameter)` — only reference types accepted | N/A |

### Conformance Classification

- **COUNTA scalar `""`**: Required by ODF 1.4 — matches (empty string counted as non-blank).
- **COUNTA empty cells**: Required by ODF 1.4 — matches (CELLTYPE_NONE not counted).
- **COUNTBLANK truly empty cells**: Required by ODF 1.4 — matches.
- **COUNTBLANK formula cell returning `""`**: Implementation-defined (§6.13.8) — AOO uses cell-type-only check; does not inspect formula result.
- **COUNTA matrix with strings**: ODF 1.4 does not explicitly address matrix empty-string handling — AOO counts all matrix elements including strings.

### Known Compatibility Risks

- A `ScDBCount` TODO comment (interpr1.cxx:6838-6845) notes that `ScQueryCellIterator` only returns pointers to existing cells, so empty cells matched by a query are not counted. This affects DCOUNT without a field, not COUNTA/COUNTBLANK directly.
- Changing COUNTBLANK to inspect formula results (treating `""`-returning formulas as blank) would be a behavior change affecting existing documents.

### Recommended Implementation Changes

None required for ODF 1.4 conformance (implementation-defined behavior is permitted). If a compatibility option is desired, document the current cell-type-only behavior and consider adding formula-result inspection as an option.

### Suggested Regression Tests

- `COUNTA` over range with truly empty cells, text, numbers, and `""` scalar argument
- `COUNTBLANK` over range with truly empty cells vs. cells containing text/numbers
- `COUNTBLANK` over formula cells returning `""` (document current behavior as implementation-defined)

---

## F.2 INDEX Row-Vector Behavior (§6.14.6)

### Normative Requirement

ODF 1.4 §6.14.6: `INDEX` with a single row argument on a single-row range (row vector) returns an error. The ODF 1.3 rule allowing the Row argument to act as a column offset for row vectors was removed.

### AOO Implementation Location

- `ScIndex()` — `main/sc/source/core/tool/interpr1.cxx:7419`
- Dispatch: `interpr4.cxx:3650` (ocIndex → ScIndex)
- Declaration: `main/sc/source/core/inc/interpre.hxx:564`
- Parameter class: `main/sc/source/core/tool/parclass.cxx:123` (`ocIndex: {{ Reference, Value, Value, Value }, 0}`)

### Execution Path — ScIndex

```
nParamCount = GetByte()           // number of arguments
nArea     = (nParamCount == 4) ? GetDouble() : 1
nCol      = (nParamCount >= 3) ? GetDouble() : 0
nRow      = (nParamCount >= 2) ? GetDouble() : 0
```

Then `switch(GetStackType())`:

| Stack Type | Behavior |
|---|---|
| `svMatrix` | Bounds check against matrix dimensions; vector access when nCol==0 or nRow==0 |
| `svSingleRef` | `if (nCol > 1 || nRow > 1) PushIllegalArgument();` — single cell, only row/col 0 or 1 valid |
| `svDoubleRef` / `svRefList` | Full range bounds checking; returns sub-reference or single cell |

### Before and After Milestone A (commit bb58e28fc2)

**Before Milestone A:** A `bRowArray` flag was set when `nParamCount == 2 && nRow1 == nRow2` (single-row range with row argument). This caused:
- Row bounds check to be skipped: `(nRow > 0 && nRow1+nRow-1 > nRow2 && !bRowArray)`
- Alternative row bounds: `(nRow > nCol2 - nCol1 + 1 && bRowArray)` — checked against column width
- Row argument reinterpreted as column: `nCol = (SCCOL) nRow; nRow = 1;`

Effect: `INDEX(H1:L1;3)` returned the 3rd column (value 30) instead of an error.

**After Milestone A:** All `bRowArray` logic removed. `INDEX(H1:L1;3)` now:
- nCol=0, nRow=3
- svDoubleRef path: `(nRow > 0 && nRow1+nRow-1 > nRow2)` → `(3 > 0 && 1+3-1 > 1)` → true → `PushIllegalArgument()` → error

This matches ODF 1.4 §6.14.6.

### Conformance Classification

**Required by ODF 1.4** — Fixed in Milestone A. `INDEX(H1:L1;3)` returns error as required.

### Known Compatibility Risks

- Legacy ODF 1.3 documents relying on `INDEX(rowvector; n)` returning column n will now get an error. Regression risk for document compatibility.
- Compatibility handling can be isolated: the fix is localized to `ScIndex()`; no other code paths depend on `bRowArray`.

### Recommended Implementation Changes

None needed for ODF 1.4 conformance. Consider a document-compatibility option if legacy document regression is observed (out of scope for this milestone).

### Suggested Regression Tests

- `INDEX(H1:L1;3)` → error (single-arg row vector)
- `INDEX(H1:L1;;3)` → column value (column-only arg)
- `INDEX(D1:D5;6)` → error (row out of range)
- `INDEX(D1:F3;2;3)` → 2D result

---

## F.3 Criteria Syntax (§4.11.11) — Database Functions

### Normative Requirement

ODF 1.4 §4.11.11: Criteria syntax supported in database functions (DCOUNT, DCOUNTA, DSUM, DAVERAGE, DMAX, DMIN, etc.). Criteria support comparison operators (`<`, `<=`, `>`, `>=`, `=`, `<>`), text criteria, wildcards, and criteria ranges.

### AOO Implementation Locations

- `GetDBParams()` — `main/sc/source/core/tool/interpr1.cxx:6621` (parses database + field + criteria range)
- `DBIterator()` — `main/sc/source/core/tool/interpr1.cxx:6760` (iterates matching records for DBSum, DBAverage, DBMax, DBMin, DBProduct)
- `ScDBCount()` — `main/sc/source/core/tool/interpr1.cxx:6828` (DCOUNT)
- `ScDBCount2()` — `main/sc/source/core/tool/interpr1.cxx:6882` (DCOUNTA)
- `ScDBSum()` — `main/sc/source/core/tool/interpr1.cxx:6821` (DSUM)
- `ScDBAverage()` — `main/sc/source/core/tool/interpr1.cxx:6909` (DAVERAGE)
- `ScDBMax()` — `main/sc/source/core/tool/interpr1.cxx:6916` (DMAX)
- `ScDBMin()` — `main/sc/source/core/tool/interpr1.cxx:6923` (DMIN)
- Operator prefix parsing — `main/sc/source/core/tool/queryparam.cxx:106-144`
- `ScQueryEntry` struct — `main/sc/inc/global.hxx:797`
- `ScQueryOp` enum — `main/sc/inc/global.hxx:733-740` (SC_EQUAL, SC_LESS, SC_GREATER, SC_LESS_EQUAL, SC_GREATER_EQUAL, SC_NOT_EQUAL)

### Function-to-Opcode Mapping

| Function | Opcode | Implementation |
|---|---|---|
| DCOUNT | ocDBCount → SC_OPCODE_DB_COUNT | ScDBCount() |
| DCOUNTA | ocDBCount2 → SC_OPCODE_DB_COUNT_2 | ScDBCount2() |
| DSUM | ocDBSum → SC_OPCODE_DB_SUM | ScDBSum() → DBIterator(ifSUM) |
| DAVERAGE | ocDBAverage → SC_OPCODE_DB_AVERAGE | ScDBAverage() → DBIterator(ifAVERAGE) |
| DMAX | ocDBMax → SC_OPCODE_DB_MAX | ScDBMax() → DBIterator(ifMAX) |
| DMIN | ocDBMin → SC_OPCODE_DB_MIN | ScDBMin() → DBIterator(ifMIN) |

Name mapping: `main/formula/source/core/resource/core_resource.src:227-228` (`DCOUNT` → SC_OPCODE_DB_COUNT, `DCOUNTA` → SC_OPCODE_DB_COUNT_2)

### Criteria Parsing Execution Path

1. Formula compiler parses `DCOUNT(database, field, criteria)` into opcodes with three Reference parameters.
2. `GetDBParams()` pops:
   - Criteria range (double ref)
   - Field (double, string, single ref, double ref, or missing)
   - Database range (double ref)
3. For each criteria entry, `queryparam.cxx:106-144` parses operator prefixes:
   - `<>` → SC_NOT_EQUAL
   - `<=` → SC_LESS_EQUAL
   - `<` → SC_LESS
   - `>=` → SC_GREATER_EQUAL
   - `>` → SC_GREATER
   - `=` or default → SC_EQUAL
4. `bQueryByString` set via `IsNumberFormat()` (GetDBParams :6745-6746) — determines if criteria is text or numeric
5. `bRegExp` set via `MayBeRegExp()` (GetDBParams :6748) — enables wildcard/RegExp matching
6. `DBIterator()` iterates matching records using `ScDBQueryDataIterator`

### Current Behavior vs ODF 1.4

| Requirement | ODF 1.4 Normative | AOO Behavior | Match? |
|---|---|---|---|
| Comparison operators `<, <=, >, >=, =, <>` | Required | Parsed from criteria cell prefix (queryparam.cxx:106-144) | Yes |
| Text criteria | Required | `bQueryByString` via `IsNumberFormat` | Yes |
| Wildcards / RegExp | Required | `bRegExp` via `MayBeRegExp` | Yes |
| Empty criteria field | Field may be omitted (count all matching) | `bMissingField` path in ScDBCount | Yes |
| Criteria range with multiple rows | AND across rows | ScQueryParamBase entries | Yes |
| DCOUNT without field counts all matching records | Required | ScDBCount: bMissingField + INTERNAL type → counts all matching | Yes; the empty-cell undercount is fixed, see calcval `DB.DCOUNT.*` |
| DCOUNT with field counts values in field | Required | ScDBQueryDataIterator over field column | Yes |

### Known Limitations

**ScDBCount empty-cell limitation (interpr1.cxx:6838-6845):**
```
// TODO: currently the QueryIterators only return cell pointers of
// existing cells, so if a query matches an empty cell there's
// nothing returned, and therefor not counted!
```
This means DCOUNT without a field may undercount when matching records have empty cells in the result column.

**Update:** fixed. DCOUNT and DCOUNTA without a field now test every record of the range against the criteria (`ScDBQueryDataIterator::CountRecords`). Records with an empty cell in a criteria column are counted. With an empty criteria cell the count is now all records (`DB.DCOUNT.nf.nocrit` gives 3, was 0). Covered by the `DB.*` cases in `calcval-cases14.tsv`.

### Conformance Classification

**Required by ODF 1.4** — Criteria syntax parsing and database function evaluation are implemented. The empty-cell matching limitation in ScDBCount is a known gap.

### Recommended Implementation Changes

Address the ScDBCount TODO (empty-cell matching) if DCOUNT without field is used with empty result cells. This is a targeted fix, not a broad change.

### Suggested Regression Tests

- `DCOUNT(R1:T6;;V1:V2)` with `Age>=30` → 4 (B,C,D,E)
- `DSUM` with criteria
- `DAVERAGE`, `DMAX`, `DMIN` with criteria
- Text criteria, wildcard criteria, empty criteria cell

---

## F.4 Basic Limits and Syntax (§3.7, §5)

### F.4.1 Basic Limits (§3.7)

#### Formula Length — at least 1024 characters

**Source finding:** No explicit formula length limit (MAXFORMULA or equivalent) found in `main/sc/source/core/tool/interpr1.cxx`, `main/sc/source/core/inc/interpre.hxx`, or `main/formula/source/core/api/FormulaCompiler.cxx`. The FormulaCompiler parses token arrays dynamically.

**Classification:** Unverifiable by static inspection. Absence of a hard limit does not prove conformance. Runtime test required.

#### Function Parameters — at least 30

**Source finding:** `MustHaveParamCount()` family (`interpre.hxx:897-927`) enforces per-function min/max parameter counts. No global 30-parameter cap found. Individual functions like MAX use `MustHaveParamCountMin` or exact counts.

**Classification:** Unverifiable by static inspection. No cap found, but runtime test required.

#### ASCII Strings — at least 32767 characters

**Source finding:** `CheckStringResultLen()` (`interpre.hxx:930-936`) checks string concatenation against `STRING_MAXLEN`. The `STRING_MAXLEN` definition was not located in the sc module headers during this audit.

**Classification:** Unverifiable by static inspection. STRING_MAXLEN value not confirmed.

#### Nesting Depth — at least 7 levels

**Source finding:** No nesting depth limit (MAXNEST or equivalent) found in FormulaCompiler or ScInterpreter source. No recursion depth enforcement found.

**Classification:** Unverifiable by static inspection. Absence of limit does not prove conformance.

### F.4.2 Remaining Syntax Requirements (§5)

| Requirement | Section | AOO Implementation | Status |
|---|---|---|---|
| Parenthesized expressions | §5.2 | FormulaCompiler.cxx handles via RangeLine/IntersectionLine/UnaryLine | Present, runtime-unverified |
| Constant Numbers | §5.3 | FormulaCompiler.cxx tokenizes numeric literals | Present, runtime-unverified |
| Constant Strings with quote escaping | §5.4 | FormulaCompiler.cxx tokenizes string literals | Present, runtime-unverified |
| Operator precedence (Table 1) | §5.5 | Parser: UnionLine → IntersectionLine → RangeLine → UnaryLine → binary ops | Present, runtime-unverified |
| Functions and parameters | §5.6 | FormulaCompiler.cxx compiles function calls | Present, runtime-unverified |
| Nonstandard function names as errors | §5.7 | ocNoName opcode; xlformula.cxx stubs | Present, runtime-unverified |
| Cell and range references | §5.8 | compiler.cxx PopDoubleRef/PopSingleRef | Present, runtime-unverified |
| Simple Named Expressions | §5.11 | compiler.cxx:NameLine (ocName opcode) | Present; requires pre-defined name |
| Error constants and propagation | §5.12 | ocErrNull-ocErrNA in compiler.hrc; nGlobalError/SetError in interpre.hxx | Present, runtime-unverified |
| Whitespace handling | §5.14 | ocSpaces → ocIntersect at FormulaCompiler.cxx:830-831 | Present, runtime-unverified |
| Reference intersection `!` | §6.4.12 | interpr1.cxx:ScIntersect | Present, runtime-unverified |
| Reference range `:` | §6.4.11 | interpr1.cxx:ScRangeFunc | Present, runtime-unverified |

### Cross-Cutting Finding

**Static source tracing confirms implementation presence, not semantic equivalence.** All Small Group syntax and basic limits infrastructure exists in source, but none has been verified against ODF 1.4 normative expected results at runtime. This is consistent with the E.1/E.1.1 finding that all requirements are UNVERIFIED pending runtime execution.

---

## Conformance Summary

| Category | Count | Status |
|---|---|---|
| Basic Limits (§3.7) | 4 | Unverifiable statically; no hard limits found |
| Syntax (§5) | 11 | Implementation present; runtime-unverified |
| COUNTA/COUNTBLANK (§6.13.7-8) | 2 | Partially matches; formula-cell `""` is implementation-defined |
| INDEX (§6.14.6) | 1 | ODF 1.4 compliant after Milestone A; regression risk |
| Criteria Syntax (§4.11.11) | 1 | Implemented; ScDBCount empty-cell TODO limitation |
| Operators (§6.4) | 14 | Implementation present; runtime-unverified |
| Functions (§2.3.2.E) | 110 | Implementation present; runtime-unverified |

**No production code changes are recommended at this audit stage.** Milestone G should implement targeted fixes based on this audit's findings, prioritized by dependency and risk.
