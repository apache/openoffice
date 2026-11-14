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
| G.5 | Pending | — | Runtime verification only; no build available |
| G.6 | Pending | — | Runtime verification only; no build available |

**Blocker:** No configured AOO build tree; `sc`/`scaddins` not built; calcval harness cannot run. Runtime verification deferred to consolidated Calc testing phase. G.2 source-level fix is complete and verified statically; runtime verification awaits build environment.


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

## Phase G.5 — Low-Risk: Basic Limits Runtime Verification

No code changes expected (no hard limits found). Verification only.

| Order | Gap ID | Area | Test | Expected Result |
|---|---|---|---|---|
| G.5.1 | F-GAP-006 | Formula length | 1024+ char formula (1026-char `=1+1+...+1`) | Accepted, result 513 |
| G.5.2 | F-GAP-007 | Parameter count | 30-parameter MAX call | Accepted, result 30 |
| G.5.3 | F-GAP-008 | String length | 32767-char string (after STRING_MAXLEN confirmed) | Accepted |
| G.5.4 | F-GAP-009 | Nesting depth | 7 nested IFs | Accepted, result 1 |

**Dependencies:** None.  
**Runtime validation needed:** Yes — all four tests.

## Phase G.6 — Low-Risk: Syntax Verification

| Order | Gap ID | Area | Test | Expected Result |
|---|---|---|---|---|
| G.6.1 | F-GAP-010 | Operator precedence | `2+3*4=14`, `2^3^2=512` | Correct precedence/associativity |
| G.6.2 | F-GAP-011 | References | Cell and range resolution | Correct values |
| G.6.3 | F-GAP-013 | Error propagation | `=NA()` → #N/A; error args propagate | Correct |
| G.6.4 | F-GAP-014 | Whitespace | `=2 + 3 * 4` → 14 | Whitespace-transparent |

**Dependencies:** None.  
**Runtime validation needed:** Yes — all syntax tests.

## Dependency Summary

```
G.1 (docs, COMPLETE) ──┐
                        ├── G.2 (ScDBCount empty-cell, IMPLEMENTED source-level) ──┐
G.5 (limits verification) ──┤                                                        ├── G.3 (COUNTBLANK decision, COMPLETE)
G.6 (syntax verification) ──┤                                                        │
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
| G.5/G.6 (verification) | Low | Test execution only; no code changes | Pending build |

## Definition of Done for Milestone G

- All G.1 documentation items complete (committed at `41da3628f9`; Help topics for 7 functions committed separately)
- G.1 Help topics: IFNA, SKEWP, BITAND, BITOR, BITXOR, BITLSHIFT, BITRSHIFT — all HID_FUNC_* IDs resolve to topics, discoverable via Help index/search
- G.2 ScDBCount/DCOUNTA empty-cell fix implemented at source level: `CountRecords()` in `dociter.cxx:685-716`, used by `ScDBCount()` at `interpr1.cxx:6879-6882` and `ScDBCount2()` at `interpr1.cxx:6913-6917`; calcval `DB.*` regression cases added to `calcval-cases14.tsv` (DB.DCOUNT.f.zero, DB.DCOUNT.nf.zero, DB.DCOUNTA.f.zero)
- G.2 runtime verification: blocked — no AOO build tree available; calcval cannot run
- G.3 COUNTBLANK behavior: decision documented — `lcl_IsCountBlankEmpty()` (interpr1.cxx:4745-4758) inspects formula results and treats formula-cell-returning-`""` as blank for COUNTBLANK (implementation-defined per §6.13.8); ISBLANK (§6.13.14) returns FALSE for empty string — no conflict. No code change needed.
- G.4 INDEX compatibility: compatibility audit complete — `bRowArray` shortcut present in `ScIndex()` (interpr1.cxx:7588-7612); `INDEX(H1:L1;3)` returns column value instead of ODF 1.4 Error; kept as documented legacy-compatible deviation |
- G.5/G.6 all runtime tests: prepared, awaiting build environment
- No production changes without corresponding calcval test coverage
- All G.1-G.3 requirements verified at source level against ODF 1.4 normative text; G.4 INDEX documented as known deviation (bRowArray shortcut, not ODF 1.4 compliant); runtime verification for G.5/G.6 pending build environment

## Deferred Items (Post-G)

- Full function-by-function semantic verification of all 110 Small Group functions (requires runtime)
- ODF save/reload round-trip testing
- Excel import/export function name alignment
