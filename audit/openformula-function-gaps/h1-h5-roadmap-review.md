# H.5.2 — OpenFormula Implementation Roadmap Review

**Branch:** `openformula`  
**Date:** 2026-11-17  
**HEAD:** `9bc678ecbc` (after H.5.1 Priority 2 fixes)  
**Scope:** Read-only audit of remaining function gaps and recommended H.6 target.

---

## 1. Current Implementation Status (H.1–H.5 Complete)

| Milestone | Function(s) | Status | Commit | ODF 1.4 Defined |
|-----------|-------------|--------|--------|-----------------|
| H.1 | NUMBERVALUE | ✅ Complete | `0224e3dc6c` + fixes | Yes (Large Group) |
| H.2 | FINDB, SEARCHB, REPLACEB | ✅ Complete | `0224e3dc6c` + fixes | No (Excel B-functions) |
| H.3 | DATEDIF | ✅ Complete | `f8b15cbbe3` + `62e7f86e31` | Yes (Large Group, §6.10.3) |
| H.4 | ERROR.TYPE | ✅ Complete | `cf30ea8b32` | Yes (Large Group) |
| H.5 | ISOWEEKNUM | ✅ Complete | `517a2e3b21` | Yes (Large Group) |

**All seven functions now have:**
- Native opcode registration (compiler.hrc, opcode.hxx)
- Interpreter declarations and dispatch (interpre.hxx, interpr4.cxx)
- Function Wizard resources (scfuncs.src)
- Help IDs and English help topics (.xhp + makefile + 04060110.xhp)
- Calcval test coverage (10–41 cases each)
- Updated audit documentation

---

## 2. Remaining Function Inventory (Post H.5)

### P2 — Excel Compatibility (Next Candidates)

| Function | ODF Defined | Native Opcode | Evaluator | Help | Key Gap |
|----------|-------------|---------------|-----------|------|---------|
| ODDFPRICE | No | ❌ (AddIn only) | getOddfprice() | AddIn (German) | Not native; no opcode |
| ODDFYIELD | No | ❌ (AddIn only) | getOddfyield() | AddIn (German) | Not native; no opcode |
| RRI | No | ✅ ocZGZ | ScZGZ() | German only | No validation; export naming |
| GETPIVOTDATA | No | ✅ ocGetPivotData | ScGetPivotData() | English | XLSX import/export missing |
| FDIST | No | ✅ ocFDist | ScFDist() | German only | Param count (3 vs 3–4); export naming |
| FINV | No | ✅ ocFInv | ScFInv() | English | Export naming (_xlfnodf) |

### P3 — Minor Documentation

| Function | ODF Defined | Native Opcode | Evaluator | Help | Key Gap |
|----------|-------------|---------------|-----------|------|---------|
| HARMEAN | Yes (verify) | ✅ ocHarmMean | ScHarMean() | German only | BIFF mapping unverified |

### DEFER — Platform/Application Level

| Function | Reason |
|----------|--------|
| DDE | DDE subsystem; security; disabled in server mode |

---

## 3. Priority Ranking for H.6

| Rank | Function | Rationale |
|------|----------|-----------|
| 1 | **ODDFPRICE** | Highest compatibility value: Excel-native odd-period pricing; shares code path with ODDFYIELD/ODDLPRICE/ODDLYIELD; Analysis AddIn implementation exists as reference |
| 2 | **ODDFYIELD** | Same codebase as ODDFPRICE; implement together for efficiency |
| 3 | **RRI** | Native evaluator exists; needs validation + help + export fix; low risk |
| 4 | **GETPIVOTDATA** | Native evaluator exists; XLSX mapping gap; DataPilot dependency |
| 5 | **FDIST/FINV** | Native evaluators exist; param count + export naming; statistical functions |
| 6 | **HARMEAN** | Native evaluator correct; only German help + BIFF mapping |

---

## 4. Dependency Analysis

### ODDFPRICE / ODDFYIELD
- **Shared infrastructure:** `main/scaddins/source/analysis/financial.cxx` contains `getOddfprice()` and `getOddfyield()` (lines 404–426)
- **Dependencies:** Use `getPrice()`, `getYield()` helpers; no new infrastructure needed
- **Conflict risk:** Low — adding native opcodes alongside existing AddIn entries
- **File touches:** compiler.hrc, opcode.hxx, core_resource.src, helpids.h, hidother.src, scfuncs.src, interpr*.cxx, help .xhp, makefile, 04060110.xhp, tests

### RRI
- **Existing evaluator:** `ScZGZ()` in `interpr2.cxx:1198` — computes `pow(fV/fP, 1/n) - 1`
- **Validation gaps:** No check for `fP <= 0`, `n <= 0`, overflow
- **Export issue:** Exports as `_xlfnodf.RRI` but Excel has native `RRI` → may cause round-trip issues
- **Help:** Only German `ZGZ` (HID_FUNC_ZGZ); need English `RRI`

### FDIST / FINV
- **FDIST param count:** Current `ScFDist()` enforces exactly 3 params; ODF allows 3–4 (cumulative flag)
- **Export naming:** Both export as `_xlfnodf.FDIST`/`_xlfnodf.FINV` but Excel has native names
- **Help:** FDIST has German `FVERT` only; FINV has English help

### GETPIVOTDATA
- **Evaluator:** `ScGetPivotData()` in `interpr2.cxx:2972` — mature implementation
- **XLSX gap:** Only BIFF8 import/export (ID 358); not in `saFuncTable_Oox`
- **Dependency:** Requires DataPilot (pivot table) subsystem

---

## 5. Comparison: ODDFPRICE, ODDFYIELD vs RRI

| Criterion | ODDFPRICE/ODDFYIELD | RRI |
|-----------|---------------------|-----|
| **ODF defined** | No (Excel AddIn) | No (Excel 2013+) |
| **Native opcode** | ❌ Missing | ✅ ocZGZ exists |
| **Evaluator** | AddIn only (`getOddfprice/getOddfyield`) | ✅ `ScZGZ()` exists |
| **Help language** | AddIn (German) | German only (ZGZ) |
| **XLSX import** | ❌ None | ✅ ODF table |
| **XLSX export** | ❌ None | ⚠️ _xlfnodf.RRI |
| **Code complexity** | Medium (shared financial math) | Low (single pow() formula) |
| **Validation needed** | Full native implementation | Input validation only |
| **Compatibility value** | High (odd-period bonds) | Medium (rate of return) |
| **Implementation effort** | Higher (2 functions + shared helpers) | Lower (validation + help) |
| **Risk** | Low (AddIn reference exists) | Low (evaluator exists) |

---

## 6. Recommended H.6 Target

### **H.6: ODDFPRICE and ODDFYIELD (Native Implementation)**

**Rationale:**
1. **Highest compatibility gap:** These are the only functions in the P2 list with **no native opcode** — they exist only as Analysis AddIn calls, which limits their availability and Excel compatibility.
2. **Shared implementation:** Both share the same financial math code path (`financial.cxx:404–426`) and helpers (`getPrice`, `getYield`). Implementing both together is more efficient than separately.
3. **Precedent:** H.1–H.5 pattern established — add opcodes, native evaluators, help, tests. ODDFPRICE/ODDFYIELD fit this pattern cleanly.
4. **No new dependencies:** Uses existing financial math infrastructure; no platform-specific code.
4. **Clear testability:** Bond pricing has well-defined mathematical formulas; test cases can be derived from financial standards and Excel reference.

**Alternative (if lower risk preferred):** **RRI** — evaluator exists, only needs validation, English help, and export naming fix. Lower effort but lower compatibility impact.

**Recommendation:** Proceed with **ODDFPRICE + ODDFYIELD** as H.6. The Analysis AddIn implementations provide verified reference algorithms, and the opcode/evaluator pattern is now well-established.

---

## 7. Proposed H.6 Implementation Milestones

### H.6.1 — Opcode & Registration (ODDFPRICE, ODDFYIELD)
- Add `SC_OPCODE_ODDFPRICE`, `SC_OPCODE_ODDFYIELD` to `compiler.hrc` (next: 417, 418)
- Add `ocOddfprice`, `ocOddfyield` to `opcode.hxx`
- Register names in `core_resource.src` (ODF + Excel sections)
- Update `LAST_OPCODE_ID`

### H.6.2 — Evaluator Implementation
- Implement `ScOddfprice()`, `ScOddfyield()` in `interpr2.cxx` (or new file)
- Adapt `getOddfprice()`, `getOddfyield()` from `financial.cxx` to native evaluator pattern
- Add dispatch cases in `interpr4.cxx`
- Add declarations in `interpre.hxx`

### H.6.3 — Function Wizard & Help
- Add `SC_OPCODE_ODDFPRICE`/`ODDFYIELD` resources in `scfuncs.src`
- Add `HID_FUNC_ODDFPRICE`, `HID_FUNC_ODDFYIELD` to `helpids.h` + `hidother.src`
- Create `func_oddfprice.xhp`, `func_oddfyield.xhp`
- Register in `makefile.mk` and `04060110.xhp` (Text functions section)

### H.6.4 — BIFF/XLSX Mapping
- Add ODF table entries in `xlformula.cxx` (`saFuncTable_Odf`)
- Consider BIFF import if Excel BIFF IDs exist (research needed)
- XLSX export: native names (no `_xlfnodf` prefix if Excel has them)

### H.6.5 — Test Cases
- Add calcval tests: `SG.ODDFPRICE-*`, `SG.ODDFYIELD-*`
- Cover: normal bonds, odd first period, various frequencies, day count bases
- Edge cases: invalid dates, zero rates, negative prices

### H.6.6 — Documentation Updates
- Update `function-gap-inventory.md`, `function-gap-summary.tsv`
- Update `function-implementation-roadmap.md`, `function-semantic-test-plan.md`
- Update `function-source-audit.md`

---

## 8. Alternative H.6 (Lower Risk): RRI Only

If the team prefers a smaller scope:
- **H.6: RRI Validation & Help**
  - Add input validation to `ScZGZ()` (fP>0, n>0, overflow checks)
  - Add English help `func_rri.xhp`
  - Fix XLSX export naming (Excel has native `RRI` → no `_xlfnodf` prefix)
  - Add calcval tests `SG.RRI-*`
  - Estimated effort: ~1/3 of ODDFPRICE+ODDFYIELD

---

## 9. Roadmap Documentation Corrections

The following files should be updated to reflect H.1–H.5 completion:

| File | Current State | Needed Update |
|------|---------------|---------------|
| `function-implementation-roadmap.md` | Shows Groups 1–5 with ISOWEEKNUM pending | Mark all Groups 1–5 complete; update Group 3 status |
| `function-gap-inventory.md` | P2/P3/DEFER list accurate | No changes needed |
| `function-gap-summary.tsv` | Accurate | No changes needed |

No corrections needed to inventory/summary — they accurately reflect current state.

---

## 10. Conclusion

**Recommended H.6:** **ODDFPRICE + ODDFYIELD** (Group 3 — Financial Odd-Period Functions)

- Highest compatibility impact
- Shared implementation reduces per-function cost
- Follows established H.1–H.5 pattern
- No new dependencies
- Clear testability

**Alternative:** RRI (Group 4) if lower-risk/smaller scope preferred.

**Next step:** Proceed with H.6.1–H.6.6 implementation when ready.

---

*This audit is read-only. No production code was modified.*
