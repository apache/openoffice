# Milestone G — OpenFormula Small Group Semantic Conformance: Implementation Sequence

**Branch:** `openformula`  
**Baseline:** `9c42008d93` (post-E.1.1)  
**Source:** `openformula-small-group-semantic-audit.md`, `openformula-small-group-semantic-gaps.tsv`  
**Scope:** Implementation planning only. No code changes in this document.

## Current Status

| Phase | Status | Commit | Notes |
|---|---|---|---|
| G.1 | Complete | `41da3628f9` + Help topics commit | STRING_MAXLEN documented; ScDBCount empty-cell fix applied via `CountRecords()`; Help topics added for IFNA, SKEWP, BITAND, BITOR, BITXOR, BITLSHIFT, BITRSHIFT |
| G.2 | Implemented (source-level) | `efd87262f2` + regression tests | `ScDBCount()` missing-field path uses `CountRecords()` (dociter.cxx:685-716) which iterates row-by-row via `IsQueryValid()`; `ScDBCount2()` same pattern; calcval `DB.*` regression cases added | 
| G.3 | Complete (decision documented) | `929fe7a9ce` | COUNTBLANK formula-cell `""` behavior decision: `lcl_IsCountBlankEmpty()` (interpr1.cxx:4745-4758) inspects formula results and treats formula-cell-returning-`""` as blank — implementation-defined per §6.13.8; no code change needed |
| G.4 | Compatibility audit complete (decision documented) | — | `bRowArray` shortcut present in `ScIndex()` (interpr1.cxx:7588-7612); `INDEX(H1:L1;3)` returns column value instead of ODF 1.4 Error; kept as legacy-compatible deviation |
| G.5 | Complete (source-level audit) | `fb522d42ea` | All 4 ODF 1.4 §3.7 Basic Limits source-audited; no blocking limits found; all classified RUNTIME_PENDING. Formula length: no cap (MAXCODE=2048 tokens). Parameters: no 30-param cap (MAX uses MustHaveParamCountMin(1)). String length: STRING_MAXLEN=65,535 > 32,767. Nesting depth: no depth limit. Runtime verification blocked (no build tree) |
| G.6 | Complete (source-level audit) | `fb522d42ea` | All 11 ODF 1.4 §2.3.2.B syntax requirements source-audited; implementation present for all; no blocking limits; all classified RUNTIME_PENDING. Includes Basic Expressions, Constant Numbers, Constant Strings, Operators, Functions, Nonstandard Function Names, References, Simple Named Expressions, Errors, Whitespace. Runtime verification blocked (no build tree) |

**Blocker:** No configured AOO build tree; `sc`/`scaddins` not built; calcval harness cannot run. Runtime verification deferred to consolidated Calc testing phase. All G.1-G.6 source-level work is complete; runtime verification for G.2-G.6 awaits build environment.


## Principles

1. **Audit-first:** Implementation follows the F-audit findings; no speculative fixes.
2. **Dependency-ordered:** Changes with no dependencies first; dependent changes after.
3. **Risk-ordered:** Lower-risk, self-contained changes before higher-risk behavior changes.
4. **No LibreOffice code:** All fixes derived from AOO source inspection only.
5. **No production changes without runtime validation:** All changes deferred until a configured AOO build tree exists and calcval can run (currently macOS-only).

## Phase G.1 — Zero-Risk Documentation & Test Harness (No Code Changes)

**STATUS: COMPLETE** (committed at `41da3628f9`)

These items require no production source modification and can be completed immediately.

| Order | Gap ID | Area | Action | Rationale | Status |
|---|---|---|---|---|---|
| G.1.1 | F-GAP-008 | Basic Limits — strings | Locate `STRING_MAXLEN` definition; confirm value >= 32767 or document actual limit | Static lookup; no code change | Complete — `main/tools/inc/tools/string.hxx:110-120`; default `STRING_MAXLEN = 65,535` (0xFFFF without STRING32), exceeds ODF 1.4 32,767 minimum |
| G.1.2 | F-GAP-012 | Syntax — named expressions | Document named expression test requirement (pre-defined `MYNAME` not available in calcval harness) | Test infrastructure note | Complete — documented in F-GAP-012; SYN-009-named marked not directly executable |
| G.1.3 | F-GAP-015 | COUNTA/COUNTBLANK | Document scalar `""` vs formula-cell `""` distinction as implementation-defined | Audit documentation | Complete — documented in F-GAP-001, F-GAP-002, F-GAP-015 |
| G.1.4 | — | Help documentation | Add Help topics for IFNA, SKEWP, BITAND, BITOR, BITXOR, BITLSHIFT, BITRSHIFT; verify HID_FUNC_* → topic mapping | `main/helpcontent2/source/text/scalc/01/func_*.xhp` (7 files) + `scfuncs.src` metadata | Complete — Help topics created, Function Wizard HIDs resolve, scfuncs.src descriptions corrected |

## Phase G.2 — Investigation Complete: Criteria Syntax Empty-Cell Fix (ScDBCount)

**STATUS: IMPLEMENTED (source-level)** — DCOUNT and DCOUNTA without a field count every matching record via `CountRecords()`; calcval `DB.*` regression cases added. Runtime verification blocked: no AOO build tree available.

The root cause is confirmed: `ScQueryCellIterator` ("run through all not empty cells") skips empty cells, and `ScDBQueryDataIterator::getCurrent()` skips `CELLTYPE_NONE` via `default` case at `dociter.cxx:655-657`. Per ODF 1.4 §6.9.3, DCOUNT without a field should count ALL matching rows including those with empty result cells.

**Fix applied** (trunk commits rebased onto `openformula`):
- `main/sc/source/core/data/dociter.cxx:685-716` — `ScDBQueryDataIterator::DataAccessInternal::countRecords()` iterates row-by-row from `nRow1` to `nRow2`, checking `aHasContent` (row has any non-NOTE cell in any database column) and `IsQueryValid()` per row. Counts all matching records regardless of cell emptiness in the field column.
- `main/sc/source/core/tool/interpr1.cxx:6871-6902` — `ScDBCount()` missing-field path calls `aRecIter.CountRecords()` instead of `ScQueryCellIterator` iteration.
- `main/sc/source/core/tool/interpr1.cxx:6905-6935` — `ScDBCount2()` (DCOUNTA) missing-field path calls `aRecIter.CountRecords()`.

**Regression tests added** to `test/odf-validate/calcval-cases14.tsv`:
- `DB.DCOUNT.f.zero` — DCOUNT with field, numeric zero vs empty: A=0 counts via COUNT, B empty does not
- `DB.DCOUNT.nf.zero` — DCOUNT without field: all matching records count regardless of cell content
- `DB.DCOUNTA.f.zero` — DCOUNTA with field, numeric zero vs empty: A=0 counts as non-empty via COUNTA, B empty does not

| Order | Gap ID | Area | File | Finding | Fix | Risk |
|---|---|---|---|---|---|---|
| G.2.1 | F-GAP-004 | Criteria — DCOUNT empty-cell matching | `interpr1.cxx:6838-6845` (was) → `interpr1.cxx:6879-6882` + `dociter.cxx:685-716` | ScDBCount missing-field path previously used `ScQueryCellIterator` which skips empty cells; DCOUNT undercounted when matching rows had empty result cells | Replaced with row-by-row `CountRecords()` using `IsQueryValid()` per row; counts all matching rows regardless of cell emptiness | Low — isolated to DCOUNT/DCOUNTA without field; documented TODO already resolved |

**Dependencies:** None.  
**Runtime validation needed:** Yes — DCOUNT/DCOUNTA with empty result cells. **Blocked:** No AOO build tree available; cannot build/test.

## Phase G.3 — Medium-Risk: COUNTA/COUNTBLANK Behavior Review

**STATUS: COMPLETE (decision documented; no code change needed)**

ODF 1.4 §6.13.8 makes formula-cell-returning-`""` blank-counting implementation-defined. AOO already exercises this choice via `lcl_IsCountBlankEmpty()` (interpr1.cxx:4745-4758) which inspects formula results and treats formula-cell-returning-`""` as blank for COUNTBLANK. ISBLANK (§6.13.14) returns FALSE for empty string — no conflict since COUNTBLANK's behavior is explicitly implementation-defined.

| Order | Gap ID | Area | Decision Point | Decision | Risk |
|---|---|---|---|---|---|
| G.3.1 | F-GAP-001 | COUNTA matrix | Confirm matrix empty-string handling is acceptable | ODF 1.4 silent; AOO counts all matrix elements including strings — acceptable | Low |
| G.3.2 | F-GAP-002 | COUNTBLANK formula-cell `""` | Decide whether to inspect formula result for `""` | Decision: inspect formula result; treat formula-cell-returning-`""` as blank. Already implemented via `lcl_IsCountBlankEmpty()`; no code change needed | Low — no behavior change; decision documents existing implementation |

**Dependencies:** None (implementation already in place).
**Runtime validation needed:** Yes — COUNTBLANK with formula cells returning `""`. **Blocked:** No AOO build tree available; calcval cannot run. Existing test case `COUNTBLANK.emptystr` (calcval-cases14.tsv:164) covers this.

## Phase G.4 — Medium-Risk: INDEX Row-Vector Compatibility

**STATUS: COMPATIBILITY AUDIT COMPLETE (no code change)**

ODF 1.4 §6.14.6 removed the ODF 1.2/1.3 row-vector shortcut where `INDEX(rowvector; n)` treated Row as a column offset. Milestone A (bb58e28fc2) removed the `bRowArray` shortcut, but it was restored by commit 8f3b3b25a2 ("INDEX on a row vector keeps the Excel rule, a documented deviation"). Current `ScIndex()` (interpr1.cxx:7588-7612) still has `bRowArray`, so `INDEX(H1:L1;3)` returns column 3 (value 30) — ODF 1.3/Excel compatible, NOT ODF 1.4 compliant.

| Order | Gap ID | Area | Decision Point | Decision | Risk |
|---|---|---|---|---|---|
| G.4.1 | F-GAP-003 | INDEX row-vector | Keep bRowArray (legacy compatible) or remove (ODF 1.4 compliant) | Keep bRowArray for now; document as known ODF 1.4 deviation. Consider compatibility option if corpus testing shows legacy document impact. | Medium — affects existing documents; ODF 1.4 non-conformance |

**Dependencies:** None.
**Runtime validation needed:** Yes — corpus regression test against existing documents to assess legacy document impact. **Blocked:** No AOO build tree available; calcval cannot run. Existing test case `INDEX.row.rowonly` (calcval-cases14.tsv:39) expects `#ERR` (ODF 1.4) but current implementation returns `30` — test/implementation mismatch noted.

## Phase G.5 — Low-Risk: Basic Limits Source Audit

**STATUS: COMPLETE (source-level audit; runtime verification pending)**

ODF 1.4 §3.7 requires: (1) formulas up to at least 1024 characters, (2) at least 30 parameters per function, (3) strings up to 32,767 ASCII characters, (4) at least 7 nesting levels of functions. Source audit found no blocking limits for any requirement. All four classified as RUNTIME_PENDING.

| Order | Gap ID | Area | Source Finding | Classification |
|---|---|---|---|---|
| G.5.1 | F-GAP-006 | Formula length | No formula-text cap; MAXCODE=2048 tokens (compiler.hxx:59), MAXSTRLEN=1024 per symbol (compiler.hxx:60). 1026-char formula needs ~1025 tokens < 2048. | RUNTIME_PENDING |
| G.5.2 | F-GAP-007 | Parameter count | No global 30-param cap; MAX uses MustHaveParamCountMin(1) (interpr1.cxx:3396-3400); SUM supports 255 args (calcval SG.BL-255args). | RUNTIME_PENDING |
| G.5.3 | F-GAP-008 | String length | STRING_MAXLEN=65,535 (string.hxx:119, STRING32 not defined) > 32,767. CheckStringResultLen enforces at concatenation (interpre.hxx:930-936). String literals limited to 1023 chars (MAXSTRLEN=1024). | RUNTIME_PENDING |
| G.5.4 | F-GAP-009 | Nesting depth | No nesting depth limit (no MAXNEST or recursion depth guard). Parser uses recursive descent without depth guard (compiler.cxx:4134). | RUNTIME_PENDING |

**Dependencies:** None.  
**Runtime validation needed:** Yes — all four calcval tests (SG.BL-001 through SG.BL-004). **Blocked:** No AOO build tree available; calcval cannot run. Source audit complete; no production code changes needed.

## Phase G.6 — Low-Risk: Syntax Verification

**STATUS: COMPLETE (source-level audit; runtime verification pending)**

ODF 1.4 §2.3.2.B requires Small Group Evaluators to implement syntax from 11 sections: Criteria §4.11.11 (covered under F-GAP-004/G.4); Basic Expressions §5.2; Constant Numbers §5.3; Constant Strings §5.4; Operators §5.5; Functions and Function Parameters §5.6; Nonstandard Function Names §5.7; References §5.8; Simple Named Expressions §5.11; Errors §5.12; Whitespace §5.14. Source audit found implementation presence for all 11 requirements with no blocking limitations. All classified as RUNTIME_PENDING.

| Order | Gap ID | Area | Source Finding | Classification |
|---|---|---|---|---|
| G.6.1 | F-GAP-016 | Basic Expressions §5.2 | `Formula ::= Intro? Expression`; Expression supports Number, String, Array, PrefixOp, InfixOp, parenthesized, function call, Reference, QuotedLabel, AutomaticIntersection, NamedExpression, Error (`compiler.cxx:3570`, :3780) | RUNTIME_PENDING |
| G.6.2 | F-GAP-017 | Constant Numbers §5.3 | `Number ::= StandardNumber | '.' [0-9]+ ([eE] [-+]? [0-9]+)?`; decimal '.', scientific E/e (`compiler.cxx:2650+`) | RUNTIME_PENDING |
| G.6.3 | F-GAP-018 | Constant Strings §5.4 | `String ::= '"' ([^"#x00] | '""')* '"'`; double-quote escaped by duplication (`compiler.cxx:2682`) | RUNTIME_PENDING |
| G.6.4 | F-GAP-010 | Operators §5.5 | PrefixOp '+', '-'; PostfixOp '%'; InfixOp: Arithmetic, Comparison, String, Reference. Precedence: ! > : > prefix +,- > ^ > *,/ > +,- > & > comparison (`compiler.cxx` RangeLine→binary ops; `compiler.hxx:61` MAXJUMPCOUNT=32 CHOOSE only) | RUNTIME_PENDING |
| G.6.5 | F-GAP-019 | Functions §5.6 | `FunctionName ::= LetterXML (LetterXML | DigitXML | '_' | '.' | CombiningCharXML)*`; case-insensitive; ';' separator; empty params allowed (`compiler.cxx:3648`, `interpre.hxx:897-927`) | RUNTIME_PENDING |
| G.6.6 | F-GAP-020 | Nonstandard Function Names §5.7 | Unknown functions → Error other than #N/A; names need prefix (`compiler.cxx` errNoName; `interpr4.cxx:3106`, :3205) | RUNTIME_PENDING |
| G.6.7 | F-GAP-011 | References §5.8 | `Reference ::= '[' (Source? RangeAddress) \| ReferenceError ']'`; supports cell, row, column, whole-row, whole-column (`compiler.cxx` IsReference, PopDoubleRef/PopSingleRef) | RUNTIME_PENDING |
| G.6.8 | F-GAP-012 | Simple Named Expressions §5.11 | `SimpleNamedExpression ::= Identifier | '$$' (Identifier | SingleQuoted)`; global scope, case-consistent (`compiler.cxx` NameLine/ocName) | RUNTIME_PENDING |
| G.6.9 | F-GAP-013 | Constant Errors §5.12 | `#N/A` required; others optional; Error syntax `'#' [A-Z0-9]+ ...` (`compiler.hrc:30-36`; `interpr4.cxx` PushError) | RUNTIME_PENDING |
| G.6.10 | F-GAP-014 | Whitespace §5.14 | Whitespace ::= #x20 | #x09 | #x0a | #x0d; ignored outside strings, around operators, before/after =, before function names (`compiler.cxx:3583-3593`, :3655-3667) | RUNTIME_PENDING |

**Dependencies:** None.  
**Runtime validation needed:** Yes — all syntax tests (SYN-001 through SYN-011). **Blocked:** No AOO build tree available; calcval cannot run. Source audit complete; no production code changes needed.

## Dependency Summary

```
G.1 (docs, COMPLETE) ──┐
                        ├── G.2 (ScDBCount empty-cell, IMPLEMENTED source-level) ──┐
G.5 (limits audit, COMPLETE source-level) ──┤                                        ├── G.3 (COUNTBLANK decision, COMPLETE)
G.6 (syntax audit, COMPLETE source-level) ──┤                                        │
G.4 (INDEX compatibility) ──┘                                                        └── (runtime-validated)
```
G.4 compatibility audit: `bRowArray` shortcut present (interpr1.cxx:7588-7612); `INDEX(H1:L1;3)` returns column value instead of ODF 1.4 Error. Kept as documented deviation; removal would break legacy ODF 1.3 documents.

## Estimated Implementation Complexity

| Phase | Complexity | Rationale | Status |
|---|---|---|---|
| G.1 (docs) | Trivial | No code changes | Complete |
| G.2 (ScDBCount) | Low | Fix applied via `CountRecords()`; regression tests added; runtime verification blocked | Implemented (source-level) |
| G.3 (COUNTBLANK) | Low | Decision documented; implementation already in place via `lcl_IsCountBlankEmpty()` | Complete (decision documented) |
| G.4 (INDEX compat) | Medium | Compatibility audit complete; `bRowArray` shortcut present; ODF 1.4 deviation documented | Compatibility audit complete (decision documented) |
| G.5 (limits audit) | Low | Source audit complete; no blocking limits found; all 4 classified RUNTIME_PENDING | Complete (source-level) |
| G.6 (syntax audit) | Low | Source audit complete; implementation present for all 11 syntax requirements; no blocking limits; all classified RUNTIME_PENDING | Complete (source-level) |

## Definition of Done for Milestone G

- All G.1 documentation items complete (committed at `41da3628f9`; Help topics for 7 functions committed separately)
- G.1 Help topics: IFNA, SKEWP, BITAND, BITOR, BITXOR, BITLSHIFT, BITRSHIFT — all HID_FUNC_* IDs resolve to topics, discoverable via Help index/search
- G.2 ScDBCount/DCOUNTA empty-cell fix implemented at source level: `CountRecords()` in `dociter.cxx:685-716`, used by `ScDBCount()` at `interpr1.cxx:6879-6882` and `ScDBCount2()` at `interpr1.cxx:6913-6917`; calcval `DB.*` regression cases added to `calcval-cases14.tsv` (DB.DCOUNT.f.zero, DB.DCOUNT.nf.zero, DB.DCOUNTA.f.zero)
- G.2 runtime verification: blocked — no AOO build tree available; calcval cannot run
- G.3 COUNTBLANK behavior: decision documented — `lcl_IsCountBlankEmpty()` (interpr1.cxx:4745-4758) inspects formula results and treats formula-cell-returning-`""` as blank for COUNTBLANK (implementation-defined per §6.13.8); ISBLANK (§6.13.14) returns FALSE for empty string — no conflict. No code change needed.
- G.4 INDEX compatibility: compatibility audit complete — `bRowArray` shortcut present in `ScIndex()` (interpr1.cxx:7588-7612); `INDEX(H1:L1;3)` returns column value instead of ODF 1.4 Error; kept as documented legacy-compatible deviation |
- G.5 Basic Limits: source audit complete — no blocking limits found for any of the 4 ODF 1.4 §3.7 requirements. All classified RUNTIME_PENDING. Formula length: no formula-text cap (MAXCODE=2048 tokens, MAXSTRLEN=1024 per symbol). Parameters: no global 30-param cap (MAX uses MustHaveParamCountMin(1)). String length: STRING_MAXLEN=65,535 > 32,767 minimum. Nesting depth: no depth limit found. Runtime verification blocked (no build tree).
- G.6 Syntax Verification: source audit complete — implementation present for all 11 ODF 1.4 §2.3.2.B syntax requirements (Basic Expressions, Constant Numbers, Constant Strings, Operators, Functions, Nonstandard Function Names, References, Simple Named Expressions, Errors, Whitespace) plus Criteria §4.11.11 (covered under F-GAP-004). All classified RUNTIME_PENDING. Runtime verification blocked (no build tree).
- G.5/G.6 runtime tests: prepared, awaiting build environment
- No production changes without corresponding calcval test coverage
- All G.1-G.3 requirements verified at source level against ODF 1.4 normative text; G.4 INDEX documented as known deviation (bRowArray shortcut, not ODF 1.4 compliant); runtime verification for G.5/G.6 pending build environment

## Deferred Items (Post-G)

- Full function-by-function semantic verification of all 110 Small Group functions (requires runtime)
- ODF save/reload round-trip testing
- Excel import/export function name alignment
