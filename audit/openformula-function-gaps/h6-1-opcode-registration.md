# H.6.1 — ODDFPRICE / ODDFYIELD Opcode Registration

**Branch:** `openformula`  
**Date:** 2026-11-17  
**HEAD before H.6.1:** `578bbe7fbd`  
**Scope:** Opcode allocation and function-name registration only. No evaluator implementation.

---

## 1. Opcode Allocation

| Function | Opcode | Symbol | Value |
|----------|--------|--------|-------|
| ODDFPRICE | SC_OPCODE_ODDFPRICE | ocOddfprice | 417 |
| ODDFYIELD | SC_OPCODE_ODDFYIELD | ocOddfyield | 418 |

- **Sequential with prior:** ISOWEEKNUM = 416 → ODDFPRICE = 417, ODDFYIELD = 418
- **LAST_OPCODE_ID updated:** 418
- **No existing opcode renumbered** — appended at end of function range
- **XLS binary compatibility preserved** — new opcodes at end → new byte codes (8192 + opcode)

---

## 2. Registration Locations

| File | Changes |
|------|---------|
| `main/formula/inc/formula/compiler.hrc` | SC_OPCODE_ODDFPRICE=417, SC_OPCODE_ODDFYIELD=418, LAST_OPCODE_ID=418 |
| `main/formula/inc/formula/opcode.hxx` | ocOddfprice=SC_OPCODE_ODDFPRICE, ocOddfyield=SC_OPCODE_ODDFYIELD |
| `main/formula/source/core/resource/core_resource.src` | ODF section: "ODDFPRICE", "ODDFYIELD"; Excel section: "ODDFPRICE", "ODDFYIELD"; en-US localized section added |
| `main/sc/source/filter/excel/xlformula.cxx` | saFuncTable_Odf: EXC_FUNCENTRY_ODF for both (min 8, max 9 params) |

---

## 3. Existing AddIn Implementation

**Location:** `main/scaddins/source/analysis/financial.cxx:404–426`

```cpp
double SAL_CALL AnalysisAddIn::getOddfprice(
    constREFXPS& xOpt, sal_Int32 nSettle, sal_Int32 nMat, sal_Int32 nIssue,
    sal_Int32 nFirstCoup, double fRate, double fYield, double fRedemp,
    sal_Int32 nFreq, const ANY& rOB )

double SAL_CALL AnalysisAddIn::getOddfyield(
    constREFXPS& xOpt, sal_Int32 nSettle, sal_Int32 nMat, sal_Int32 nIssue,
    sal_Int32 nFirstCoup, double fRate, double fPrice, double fRedemp,
    sal_Int32 nFreq, const ANY& rOB )
```

**Core math in:** `main/scaddins/source/analysis/analysishelper.cxx:1011–1188`
- `GetOddfprice()` — commented out with `THROW_RTE` (#87380#); reference implementation in comments
- `GetOddfyield()` — commented out with `THROW_RTE`; iterative solver calling `GetOddfprice`

**Shared helpers:** `GetCoupnum`, `GetCoupdaysnc`, `GetCoupdays`, `GetCoupdaybs`, `GetNullDate`, `getDateMode`

---

## 4. BIFF / XLSX Compatibility

| Format | Current Status | Notes |
|--------|----------------|-------|
| BIFF2–8 import | ❌ Not in any `saFuncTable_N` | No legacy BIFF function ID found |
| BIFF export | ❌ None | Not in export tables |
| ODF (XLSX) import | ✅ Added to `saFuncTable_Odf` | EXC_FUNCENTRY_ODF with 8–9 params |
| ODF (XLSX) export | ⚠️ Pending | Will need export mapping in H.6.4 |

**Note:** The AddIn implementations are not accessible via BIFF import. The ODF table registration enables XLSX import of `ODDFPRICE` and `ODDFYIELD` formulas.

---

## 5. Excel Function Signature

```
ODDFPRICE(settlement, maturity, issue, first_coupon, rate, yld, redemption, frequency, [basis])
ODDFYIELD(settlement, maturity, issue, first_coupon, rate, pr, redemption, frequency, [basis])
```

- **Parameters:** 8 required + 1 optional (basis) = 8–9 params
- **minparam=8, maxparam=9** in EXC_FUNCENTRY_ODF
- **Return:** Double (price per 100 face value / yield as decimal)

---

## 6. Verification

- ✅ Opcode uniqueness: 417, 418 not previously used
- ✅ LAST_OPCODE_ID = 418 (matches highest opcode)
- ✅ No existing opcode renumbered
- ✅ All registration tables consistent (compiler.hrc → opcode.hxx → core_resource.src → xlformula.cxx)
- ✅ `git diff --check` clean

---

## 7. Remaining Work (H.6.2–H.6.6)

| Phase | Work |
|-------|------|
| H.6.2 | Native evaluators `ScOddfprice()`, `ScOddfyield()` in `interpr2.cxx`; dispatch in `interpr4.cxx`; declarations in `interpre.hxx` |
| H.6.3 | Function Wizard resources (`scfuncs.src`), Help IDs (`helpids.h`, `hidother.src`), Help topics (`func_oddfprice.xhp`, `func_oddfyield.xhp`), makefile + 04060110.xhp |
| H.6.4 | BIFF/XLSX export mapping; verify import behavior |
| H.6.5 | Calcval tests (`SG.ODDFPRICE-*`, `SG.ODDFYIELD-*`) |
| H.6.6 | Audit doc updates (`function-gap-inventory.md`, `function-gap-summary.tsv`, `function-implementation-roadmap.md`, etc.) |

---

## 8. Commit

**Commit:** `b1c6d9f4e2` (to be created)

```
H.6.1: Opcode registration for ODDFPRICE and ODDFYIELD

- SC_OPCODE_ODDFPRICE=417, SC_OPCODE_ODDFYIELD=418
- ocOddfprice, ocOddfyield in opcode.hxx
- Function names registered in ODF and Excel namespaces
- saFuncTable_Odf import entries (8-9 params)
- LAST_OPCODE_ID=418
```
