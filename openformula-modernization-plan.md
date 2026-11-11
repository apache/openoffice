# OpenFormula Modernization Plan — Apache OpenOffice Calc

AUDIT ONLY — no production code changes authorized in this phase. This plan is a prioritized roadmap derived from source evidence. Priority is by interoperability impact, not implementation convenience.

Source-of-truth: `openformula-functions.tsv`, `openformula-operators.tsv`, `openformula-semantics.md`, `openformula-odf13-14-diff.md`, `openformula-conformance.md`, `openformula-test-gap-analysis.md`.

## 1. Prioritized implementation plan

### P1 — Interoperability-critical missing functions

**P1.1 IFNA (ODF 1.3, §6.x)**
- Gap: No opcode, no implementation, no `scfuncs.src`, no `core_resource.src` entry. Only Excel-filter `ocNoName` stub (`xlformula.cxx:404`).
- Work: Add opcode `SC_OPCODE_IFNA` to `compiler.hrc`/`opcode.hxx`; map name "IFNA" in `core_resource.src`; implement `ScInterpreter::ScIfNA` in `interpr1.cxx`/`interpr4.cxx` dispatch; add `scfuncs.src` metadata; add to ODF export symbol map.
- Depends on: opcode value allocation (next free after 408; note `SC_OPCODE_LAST_OPCODE_ID=408`, `SC_OPCODE_DATA_TOKEN_1=10000`).
- Files: `main/formula/inc/formula/compiler.hrc`, `main/formula/inc/formula/opcode.hxx`, `main/formula/source/core/resource/core_resource.src`, `main/sc/source/core/tool/interpr1.cxx`, `main/sc/source/core/tool/interpr4.cxx`, `main/sc/inc/interpre.hxx`, `main/sc/source/ui/src/scfuncs.src`.
- Interoperability impact: High — IFNA is widely used and ODF 1.3/1.4 standard.

**P1.2 SKEWP (ODF 1.3, population skewness)**
- Gap: No opcode/implementation/metadata. Only Excel-filter stub (`xlformula.cxx:415`). `SKEW` (sample, `SC_OPCODE_SCHIEFE=325`) present.
- Work: Add opcode `SC_OPCODE_SKEWP`; map "SKEWP"; implement `ScSkewP` (population skewness, distinct from sample `ScSkew`); dispatch; metadata.
- Depends on: opcode value allocation; `CalculateSkew` helper (`interpr5.cxx:712`) may be reusable.
- Files: same as IFNA plus `interpr5.cxx`.
- Interoperability impact: High — statistical interop.

### P2 — Semantic discrepancies vs ODF 1.4

**P2.1 CONVERT `atm` unit value (Low severity, high visibility)**
- Gap: AOO uses 101324.996583 Pa; ODF 1.4 table = 1.0132510E+5 Pa (relative 1e-6) (`odf_docs/ODF14-KNOWN-LIMITATIONS-PUBLIC.md:72`).
- Work: Update CONVERT unit table in `ScConvert` (`interpr2.cxx`) to ODF 1.4 values; verify all affected units.
- Files: `main/sc/source/core/tool/interpr2.cxx` (ScConvert), unit data source.

**P2.2 INDEX row-vector single-argument (High severity)**
- Gap: AOO returns the column (legacy); ODF 1.4 removed that rule → error (`KNOWN-LIMITATIONS:71`).
- Work: Change `ScIndex` (`interpr1.cxx:ScIndex`) to return error when called with only a row argument on a row vector per ODF 1.4.
- Regression risk: High — existing documents/workbooks may rely on legacy column-return behavior; needs a compatibility option or version-gated behavior.
- Files: `main/sc/source/core/tool/interpr1.cxx`.

**P2.3 BITAND/BITOR/BITXOR variadic & BITLSHIFT/BITRSHIFT negative shift**
- Gap: `ScBitArithmeticOps` enforces exactly 2 params (`interpr1.cxx:1180`); ODF 1.4 may allow 1+. `ScBitShiftOps` inverts shift direction for negative `n` (`interpr1.cxx:1231-1234`).
- Work: Verify ODF 1.4 parameter count; relax or document; fix shift direction or reject negative shift per spec.
- Files: `main/sc/source/core/tool/interpr1.cxx`.

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
- Add calcval cases for CONVERT atm, INDEX row-vector, DATE pre-1583, BIT ops, short-circuit AND/OR, operator precedence.
- Do NOT add to normal production build (per audit constraint).

**P4.3 Round-trip tests**
- Function-name export/import aliases; error cell value type (`office:value-type="error"` vs string).

## 2. Dependencies between proposed changes

1. Opcode value allocation (`SC_OPCODE_LAST_OPCODE_ID`/`SC_OPCODE_DATA_TOKEN_1`) must be done once before adding IFNA and SKEWP opcodes — do both in the same opcode-block change.
2. `core_resource.src` name mapping must be added for IFNA/SKEWP before compiler recognition works.
3. `scfuncs.src` metadata depends on opcode existing.
4. INDEX behavior change (P2.2) should be coordinated with any document-compatibility option; test against existing corpus before enabling.
5. Test inventories (P4) depend on having the ODF 1.4 function list extracted; do P4.1 before P4.2/P4.3.

## 3. Estimated implementation complexity

| Item | Complexity | Rationale |
|------|------------|-----------|
| IFNA | Low-Medium | Simple 2-arg function; follows IFERROR pattern; opcode+metadata boilerplate. |
| SKEWP | Medium | Statistical computation; reuse `CalculateSkew`/`GetSortArray` helpers; population vs sample distinction. |
| CONVERT atm | Low | Single constant change in unit table; regression test all units. |
| INDEX row-vector | Medium | Behavior change; regression risk high; needs corpus testing / compat option. |
| BIT ops variadic/negative shift | Low-Medium | Small logic change; verify spec parameter count. |
| Naming/export alignment | Medium | Multiple symbol-map entry points; strict vs extended mode handling. |
| Test inventories | Medium | Requires ODF 1.4 Part 4 text extraction; tooling exists (formulacheck.py). |
| Semantic/parser/round-trip tests | Medium-High | Many cases; runtime validation needed. |

## 4. Regression risks

| Risk | Area | Mitigation |
|------|------|------------|
| Existing documents rely on INDEX column-return legacy | INDEX (P2.2) | Version-gate or document-compatibility option; corpus regression run. |
| Changing CONVERT `atm` alters results | CONVERT | Verify tolerance; document 1e-6 change; spot-check conversions. |
| Adding IFNA/SKEWP opcodes shifts opcode space | Global opcode map | Allocate contiguous values; update `SC_OPCODE_LAST_OPCODE_ID`; rebuild all symbol maps. |
| BIT op behavior change | BITAND/.../BITRSHIFT | Confirm ODF 1.4 parameter-count rule before relaxing; negative-shift fix may change results for inputs that previously "worked". |
| Export name changes break round-trip with older AOO | Symbol map | Keep legacy aliases as read-only (already done for EASTERSUNDAY); write standard names only in strict 1.4. |

## 5. What must NOT change (per audit constraints)

- No production source modifications in this phase.
- No copying of LibreOffice implementation code.
- No third-party formula engines.
- Existing Calc functionality and legacy formula compatibility preserved.

## 6. Definition of done for P1/P2

- IFNA and SKEWP: opcode, implementation, dispatch, `scfuncs.src`, `core_resource.src` mapping; smoke-tested against known expected results.
- CONVERT `atm` matches ODF 1.4 table; INDEX row-vector behaves per ODF 1.4 (with compat path if retained); BIT ops verified.
- Versioned test inventories created (not in production build).
- All findings documented with file:line references (this audit's deliverables).
