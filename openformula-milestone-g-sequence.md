# Milestone G — OpenFormula Small Group Semantic Conformance: Implementation Sequence

**Branch:** `openformula`  
**Baseline:** `9c42008d93` (post-E.1.1)  
**Source:** `openformula-small-group-semantic-audit.md`, `openformula-small-group-semantic-gaps.tsv`  
**Scope:** Implementation planning only. No code changes in this document.

## Principles

1. **Audit-first:** Implementation follows the F-audit findings; no speculative fixes.
2. **Dependency-ordered:** Changes with no dependencies first; dependent changes after.
3. **Risk-ordered:** Lower-risk, self-contained changes before higher-risk behavior changes.
4. **No LibreOffice code:** All fixes derived from AOO source inspection only.
5. **No production changes without runtime validation:** All changes deferred until a configured AOO build tree exists and calcval can run (currently macOS-only).

## Phase G.1 — Zero-Risk Documentation & Test Harness (No Code Changes)

These items require no production source modification and can be completed immediately.

| Order | Gap ID | Area | Action | Rationale |
|---|---|---|---|---|
| G.1.1 | F-GAP-008 | Basic Limits — strings | Locate `STRING_MAXLEN` definition; confirm value >= 32767 or document actual limit | Static lookup; no code change |
| G.1.2 | F-GAP-012 | Syntax — named expressions | Document named expression test requirement (pre-defined `MYNAME` not available in calcval harness) | Test infrastructure note |
| G.1.3 | F-GAP-015 | COUNTA/COUNTBLANK | Document scalar `""` vs formula-cell `""` distinction as implementation-defined | Audit documentation |

## Phase G.2 — Low-Risk: Criteria Syntax Empty-Cell Fix (ScDBCount)

This is a targeted fix with a clear TODO in source and contained blast radius.

| Order | Gap ID | Area | File | Change | Risk |
|---|---|---|---|---|---|
| G.2.1 | F-GAP-004 / F-GAP-005 | Criteria — DCOUNT empty-cell matching | `main/sc/source/core/tool/interpr1.cxx:6838-6845` | Address ScDBCount TODO: ensure empty cells matched by query are counted when field is missing; adapt ScQueryCellIterator callers as noted | Low — isolated to DCOUNT without field; documented TODO already exists |

**Dependencies:** None.  
**Runtime validation needed:** Yes — DCOUNT with empty result cells.

## Phase G.3 — Medium-Risk: COUNTA/COUNTBLANK Behavior Review

These are behavior decisions, not necessarily code changes. ODF 1.4 permits implementation-defined behavior for formula cells returning `""`.

| Order | Gap ID | Area | Decision Point | Action If Change Needed | Risk |
|---|---|---|---|---|---|
| G.3.1 | F-GAP-001 | COUNTA matrix | Confirm matrix empty-string handling is acceptable | None needed (ODF 1.4 silent) | Low |
| G.3.2 | F-GAP-002 | COUNTBLANK formula-cell `""` | Decide whether to inspect formula result for `""` | Modify `ScCountEmptyCells()` cell-type check to also check formula result | Medium — behavior change; document as compatibility option |

**Dependencies:** G.2.1 (empty-cell iteration patterns shared).  
**Runtime validation needed:** Yes — COUNTBLANK with formula cells returning `""`.

## Phase G.4 — Medium-Risk: INDEX Row-Vector Compatibility

No code change needed for ODF 1.4 conformance (Milestone A already fixed). Compatibility option is optional.

| Order | Gap ID | Area | Decision Point | Action If Needed | Risk |
|---|---|---|---|---|---|
| G.4.1 | F-GAP-003 | INDEX row-vector | Assess legacy document regression from Milestone A fix | Add document-compatibility option if corpus testing shows impact | Medium — affects existing documents |

**Dependencies:** None (Milestone A already applied).  
**Runtime validation needed:** Yes — corpus regression test against existing documents.

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
G.1 (docs/tests, no code) ──┐
                            ├── G.2 (ScDBCount empty-cell) ──┐
G.5 (limits verification) ──┤                                ├── G.3 (COUNTBLANK decision)
G.6 (syntax verification) ──┤                                │
G.4 (INDEX compatibility) ──┘                                └── (runtime-validated)
```

## Estimated Implementation Complexity

| Phase | Complexity | Rationale |
|---|---|---|
| G.1 (docs) | Trivial | No code changes |
| G.2 (ScDBCount) | Low | Isolated TODO; contained scope |
| G.3 (COUNTBLANK) | Medium | Behavior decision; may need compatibility option |
| G.4 (INDEX compat) | Medium | Corpus regression analysis needed |
| G.5/G.6 (verification) | Low | Test execution only; no code changes |

## Definition of Done for Milestone G

- All G.1 documentation items complete
- G.2 ScDBCount empty-cell fix implemented (if corpus testing confirms need)
- G.3 COUNTBLANK behavior documented (change or no-change decision recorded)
- G.4 INDEX compatibility impact assessed
- G.5/G.6 all runtime tests executed against built AOO
- No production changes without corresponding calcval test coverage
- All changes verified against ODF 1.4 normative expected results

## Deferred Items (Post-G)

- Full function-by-function semantic verification of all 110 Small Group functions (requires runtime)
- ODF save/reload round-trip testing
- Excel import/export function name alignment
