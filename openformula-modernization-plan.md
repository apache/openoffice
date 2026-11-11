# OpenFormula Modernization Plan — Apache OpenOffice Calc

AUDIT ONLY — no production code changes authorized in this phase. This plan is a prioritized roadmap derived from source evidence. Priority is by interoperability impact, not implementation convenience.

Source-of-truth: `openformula-functions.tsv`, `openformula-operators.tsv`, `openformula-semantics.md`, `openformula-odf13-14-diff.md`, `openformula-conformance.md`, `openformula-test-gap-analysis.md`.

## 1. Prioritized implementation plan

### P1 — Interoperability-critical missing functions

**P1.1 IFNA (ODF 1.3, Large Group)** — Implemented in Milestone B.
- `SC_OPCODE_IFNA=409`, `ScIfna()` in `interpr1.cxx`, dispatch in `interpr4.cxx:3451`, `scfuncs.src`, `core_resource.src:357`, `xlformula.cxx` stub replaced.
- Status: Complete.

**P1.2 SKEWP (ODF 1.3, Medium Group)** — Implemented in Milestone B.
- `SC_OPCODE_SKEWP=410`, `ScSkewP()` in `interpr3.cxx`, dispatch in `interpr4.cxx:3695`, `scfuncs.src`, `core_resource.src:358`, `xlformula.cxx` stub replaced.
- Status: Complete.

### P2 — Semantic discrepancies vs ODF 1.4

**P2.1 CONVERT `atm` unit value** — Corrected in Milestone C.
- `atm` changed from 101324.996583 Pa to 101325.1 Pa per ODF 1.4 Table 24.
- Files: `main/scaddins/source/analysis/analysishelper.cxx`.
- Status: Complete.

**P2.2 INDEX row-vector single-argument** — Fixed in Milestone A.
- AOO previously returned column (legacy); ODF 1.4 requires error for single-arg row vector.
- Work: Regression-test against existing document corpus; consider compatibility option if needed.
- Files: `main/sc/source/core/tool/interpr1.cxx` (ScIndex).
- Status: Implemented; regression risk documented.

**P2.3 BITAND/BITOR/BITXOR variadic & BITLSHIFT/BITRSHIFT negative shift**
- Gap: `ScBitArithmeticOps` enforces exactly 2 params (`interpr1.cxx:1180`); `ScBitShiftOps` handles negative shift counts.
- Work: Verify ODF 1.4 parameter count; confirm shift direction correctness.
- Files: `main/sc/source/core/tool/interpr1.cxx`.
- Status: UNVERIFIED — requires ODF 1.4 Part 4 spec text review.

### P3 — Metadata / naming alignment

**P3.1 EASTERSUNDAY alias documentation**
- Behavior is correct under standard name `EASTERSUNDAY`; `ORG.OPENOFFICE.EASTERSUNDAY` is a legacy read-only alias (`compiler.cxx:2542`). Confirm standard name is written on export (`core_resource.src:333`); document alias.
- Files: `main/sc/source/core/tool/compiler.cxx`, `main/formula/source/core/resource/core_resource.src`.

**P3.2 Other AOO-extension names written instead of standard**
- `ORG.OPENOFFICE.CURRENT` vs `CURRENT` (`core_resource.src:69` vs non-ODFF `:411`); `ORG.OPENOFFICE.ERRORTYPE` vs `ERROR.TYPE` (`:146` vs `:488`); `ORG.OPENOFFICE.STYLE` vs `STYLE` (`:336` vs `:678`); `ORG.OPENOFFICE.MULTIRANGE` vs `~` (`:251` vs `:257`); `COM.MICROSOFT.BAHTTEXT` vs `BAHTTEXT` (`:345` vs `:687`).
- Work: Decide which names to write in strict ODF 1.4 mode vs extended; align export with ODF 1.4.

### P4 — Test infrastructure (non-production build)

**P4.1 Versioned function inventories**
- Create `test/odf-validate/openformula-functions-1.4.txt` (and -1.3) from ODF 1.4 Part 4 chapter 6; keep current ODF 1.2 files as -1.2.
- Wire `formulacheck.py` to select version.
- Files: `test/odf-validate/`, `test/odf-validate/formulacheck.py`.

**P4.2 Semantic and parser test cases**
- Add calcval cases for COUNTA empty cells, DATE pre-1583, INDEX row-vector regression, BIT ops, short-circuit AND/OR, operator precedence.
- Do NOT add to normal production build (per audit constraint).

**P4.3 Round-trip tests**
- Function-name export/import aliases; error cell value type (`office:value-type="error"` vs string).

## 2. Dependencies between proposed changes

1. COUNTA/COUNTBLANK change should be coordinated with any document-compatibility option; test against existing corpus before enabling.
2. Test inventories depend on having the ODF 1.4 function list extracted; do P3.1 before P3.2/P3.3.

## 3. Estimated implementation complexity

| Item | Complexity | Rationale |
|------|------------|-----------|
| COUNTA/COUNTBLANK empty-cell semantics | Medium | Behavior change; regression risk medium; needs corpus testing. |
| DATE pre-1583 documentation | Low | Document existing impl-defined behavior; no code change needed. |
| INDEX row-vector regression testing | Medium | Corpus regression run; may need compatibility option. |
| Test inventories | Medium | Requires ODF 1.4 Part 4 text extraction; tooling exists (formulacheck.py). |
| Semantic/parser/round-trip tests | Medium-High | Many cases; runtime validation needed. |

## 4. Regression risks

| Risk | Area | Mitigation |
|------|------|------------|
| Existing documents rely on INDEX column-return legacy | INDEX (Milestone A) | Version-gate or document-compatibility option; corpus regression run. |
| Changing COUNTA empty-cell semantics alters results | COUNTA/COUNTBLANK | Verify tolerance; document change; spot-check against existing documents. |
| Export name changes break round-trip with older AOO | Symbol map | Keep legacy aliases as read-only (already done for EASTERSUNDAY); write standard names only in strict 1.4. |

## 5. What must NOT change (per audit constraints)

- No production source modifications in this phase.
- No copying of LibreOffice implementation code.
- No third-party formula engines.
- Existing Calc functionality and legacy formula compatibility preserved.

## 6. Definition of done for P1

- COUNTA/COUNTBLANK: verified against ODF 1.4 §6.13.7/§6.13.8; documented any semantic differences.
- DATE pre-1583: documented impl-defined behavior per ODF 1.4 §7.4.
- INDEX row-vector: regression-tested against document corpus.
- Versioned test inventories created (not in production build).
- All findings documented with file:line references (this audit's deliverables).
