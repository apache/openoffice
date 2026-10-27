# VBA Compatibility Test Suite for Symphony VBA Branch

## Status: Phase 0 — Complete; Phase 1-2 in progress

## 1. Overview

This directory (`main/sc/qa/vba/`) houses a compatibility regression test suite for the
VBA-compatible APIs added by the `symphony-vba` branch of Apache OpenOffice.

The suite verifies that VBA object model implementations in `main/sc/source/ui/vba/`
behave compatibly with Microsoft Excel VBA, using recorded Excel results as the reference
point of truth (inspired by the ReflexGrid compatibility methodology — no ReflexGrid
source code is copied).

**Primary test mechanism**: Java JUnit tests using UNO to drive a headless AOO instance.
Test macros are BASIC programs that use VBA-compatible syntax (`Option VBASupport 1`) and
exercise VBA object model APIs. Results are written to cell values and verified by Java
assertions through UNO.

## 2. Repository State

- Branch: `symphony-vba`, HEAD commit: `6e794ba5bce70d4146e34be8bbbdb2d0fa26372e`
- Git author: Pedro Giffuni <pfg@apache.org>
- License: Apache 2.0

## 3. APIs Under Test

| API | IDL | Implementation | Priority |
|-----|-----|----------------|----------|
| FormatCondition / FormatConditions | `XFormatCondition.idl` | `vbaformatcondition.cxx`, `vbaformatconditions.cxx` | P1 |
| FileSearch / FoundFiles | `XFileSearch.idl`, `XFoundFiles.idl` | `vbafilesearch.cxx`, `vbafoundfiles.cxx` | P2 |
| Workbook.SaveAs | `XWorkbook.idl` | `vbaworkbook.cxx` | P2 |
| Application props (ScreenUpdating, Iteration, CalculationState) | `XApplication.idl` | `vbaapplication.cxx` | P2/P4 |
| QueryTable | `XQueryTable.idl` | `vbaquerytable.cxx` | P3 |
| Range methods (Intersect, Union, Value2) | `XRange.idl` | `vbarange.cxx` | P4 |
| FileDialog (Show) | `XFileDialog.idl` | `vbafiledialog.cxx` | P5 (interactive) |

## 4. Test Infrastructure

### 4.1 Build Registration

- `main/sc/JunitTest_sc_vba.mk` — gbuild JUnit test registration
- `main/sc/Module_sc.mk` — adds `JunitTest_sc_vba` under `OOO_JUNIT_JAR` guard
- Platform makefiles (`main/solenv/gbuild/platform/linux.mk:444-446`) automatically
  set `org.openoffice.test.arg.soffice`, `org.openoffice.test.arg.env`, and
  `org.openoffice.test.arg.user` DEFS for all JUnit tests

### 4.2 Test Connection

`OfficeConnection` (in `main/test/java/test/`) starts AOO with `--headless --accept=pipe,...`
and connects via UNO URL resolver. The `soffice` path points to the installed AOO binary.

### 4.3 VBA Macro Execution

1. `VbaCompatTest` base class creates a Calc document with `VBACompatibilityMode=true`
2. BASIC test modules are inserted via `XStarBasicAccess.addModule()`
3. Macros are executed via `XScriptProviderSupplier.getScriptProvider().getScript().invoke()`
4. Test results are written to cell Z1, Z2, etc. (columns 25+)
5. Java test reads values via `XCell.getValue()` / `XCell.getString()`

### 4.4 BASIC Module Convention

VBA compatibility is enabled via `Option VBASupport 1` at the top of each BASIC module.
VBA object model methods (`ActiveSheet.Range`, `Application`, `ThisWorkbook`, etc.)
are available within these modules.

Result channels (BASIC → Java):
- Cell Z1 (column 25, row 0): numeric result (primary)
- Cell Z2 (column 25, row 1): string result
- Cells Z3-Z10: additional results

## 5. File Layout

```
main/sc/qa/vba/
  makefile.mk                     — legacy build (not used in gbuild; kept for reference)
  src/
    VbaCompatTest.java            — base class with OfficeConnection, document setup, macro execution
    VbaFormatConditionTest.java   — P1: FormatCondition/FormatConditions tests (7 test methods)
main/sc/JunitTest_sc_vba.mk       — gbuild JUnit test registration
main/sc/Module_sc.mk              — (modified) adds JunitTest_sc_vba to build
```

## 6. Test Results

| Test | Status |
|------|--------|
| FormatCondition.Add (xlExpression) | Written |
| FormatCondition.Add (xlCellValue) | Written |
| FormatCondition.Modify | Written |
| FormatConditions.Delete (all) | Written |
| FormatCondition.Delete (single) | Written |
| FormatConditions.Item access | Written |
| FormatConditions enumeration | Written |
| FormatCondition.Formula2 | Written |
| FormatConditions.Delete on empty | Written |

## 7. Next Steps

- Phase 2: Add `VbaFileSearchTest.java` (P2)
- Phase 2: Add `VbaSaveAsTest.java` (P2)
- Phase 2: Add `VbaApplicationPropsTest.java` (P2/P4)
- Phase 3: Add `VbaQueryTableTest.java` (P3)
- Phase 4: Add `VbaRangeTest.java` (P4)
