# VBA Compatibility Test Suite for Symphony VBA Branch

## Status: Phases 1-4 Complete; Phase 5 Documented as Manual Test

## 1. Overview

This directory (`main/sc/qa/vba/`) houses a compatibility regression test suite for the
VBA-compatible APIs added by the `symphony-vba` branch of Apache OpenOffice.

The suite verifies that VBA object model implementations in `main/sc/source/ui/vba/`
behave compatibly with Microsoft Excel VBA, using recorded Excel results as the reference
point of truth.

**Primary test mechanism**: Java JUnit tests using UNO to drive a headless AOO instance.
Test macros are BASIC programs that use VBA-compatible syntax (`Option VBASupport 1`) and
exercise VBA object model APIs. Results are written to cell values and verified by Java
assertions through UNO.

## 2. Repository State

- Branch: `symphony-vba`
- Git author: Pedro Giffuni <pfg@apache.org>
- License: Apache 2.0

## 3. APIs Under Test

| API | IDL | Implementation | Priority | Status |
|-----|-----|----------------|----------|--------|
| FormatCondition / FormatConditions | `XFormatCondition.idl` | `vbaformatcondition.cxx`, `vbaformatconditions.cxx` | P1 | 8 tests written |
| FileSearch / FoundFiles | `XFileSearch.idl`, `XFoundFiles.idl` | `vbafilesearch.cxx`, `vbafoundfiles.cxx` | P2 | 6 tests written |
| Workbook.SaveAs | `XWorkbook.idl` | `vbaworkbook.cxx` | P2 | 4 tests written |
| Application props (ScreenUpdating, Iteration, CalculationState, etc.) | `XApplication.idl` | `vbaapplication.cxx` | P2/P4 | 15 tests written |
| QueryTable | `XQueryTable.idl` | `vbaquerytable.cxx` | P3 | 5 tests written |
| Range methods (Intersect, Union, Value2, Precedents, Dependents) | `XRange.idl` | `vbarange.cxx` | P4 | 12 tests written |
| Scripting.Dictionary | `XDictionary.idl` | `vbadictionary.cxx` | P3 | 12 tests written |
| FileDialog (Show) | `XFileDialog.idl` | `vbafiledialog.cxx` | P5 (interactive) | Documented as manual test |

**Total: 62 test methods across 7 test classes.**

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
4. Test results are written to cells Z1, Z2, etc. (column 25 for numeric, column 26 for string)
5. Java test reads values via `XCell.getValue()` / `XCell.getString()`

### 4.4 BASIC Module Convention

VBA compatibility is enabled via `Option VBASupport 1` at the top of each BASIC module.
VBA object model methods (`ActiveSheet.Range`, `Application`, `ThisWorkbook`, etc.)
are available within these modules.

Test modules follow these conventions:
- `'@TestModule` comment at the top of each module
- `'@TestMethod` comment above each test Sub
- Arrange-Act-Assert pattern within each test Sub
- Results written to Z column (numeric) and AA column (string)

Result channels (BASIC → Java):
- Cell Z1 (column 25, row 0): primary numeric result
- Cell Z2 (column 25, row 1): secondary numeric result
- Cell Z3-Z10 (columns 25, rows 2-9): additional results
- Cell AA1 (column 26, row 0): primary string result
- Cell AA2 (column 26, row 1): secondary string result

### 4.5 Temp File Fixtures

The base class `VbaCompatTest` provides `createTempFile()` and `createTempSubdir()`
methods to create deterministic test fixtures. These are used to:
- Provide known files for FileSearch tests (instead of relying on `Environ("TEMP")`)
- Create source files for SaveAs tests
- Test FileDialog interactively (see Section 7)

## 5. File Layout

```
main/sc/qa/vba/
  README.md                         — this file
  src/
    VbaCompatTest.java              — base class: connection, doc setup, macro execution, helpers
    VbaFormatConditionTest.java     — P1: FormatCondition/FormatConditions (8 tests)
    VbaFileSearchTest.java          — P2: FileSearch/FoundFiles (6 tests)
    VbaSaveAsTest.java              — P2: Workbook.SaveAs (4 tests)
    VbaApplicationPropsTest.java     — P2/P4: Application properties (15 tests)
    VbaQueryTableTest.java          — P3: QueryTable Refresh/Delete (5 tests)
    VbaRangeTest.java               — P4: Range methods (12 tests)
    VbaDictionaryTest.java          — P3: Scripting.Dictionary (12 tests)
main/sc/JunitTest_sc_vba.mk         — gbuild JUnit test registration
main/sc/Module_sc.mk                — (modified) adds JunitTest_sc_vba to build
```

## 6. Test Summary

### P1 — FormatCondition / FormatConditions (`VbaFormatConditionTest`)
| Test | Status |
|------|--------|
| Add with xlExpression type | Written |
| Add with xlCellValue type | Written |
| Modify condition type and formula | Written |
| Delete all (FormatConditions.Delete) | Written |
| Delete single (FormatCondition.Delete) | Written |
| Item access by index | Written |
| For Each enumeration | Written |
| Formula1/Formula2 properties | Written |

### P2 — FileSearch / FoundFiles (`VbaFileSearchTest`)
| Test | Status |
|------|--------|
| Properties (FileName, LookIn, SearchSubFolders, MatchTextExactly) | Written |
| NewSearch reset | Written |
| Execute on empty result set | Written |
| Wildcard pattern search (\*.xlsx) | Written |
| MatchTextExactly search | Written |
| With SearchSubFolders = True | Written |
| FoundFiles.Item access | Written |

### P2 — Workbook.SaveAs (`VbaSaveAsTest`)
| Test | Status |
|------|--------|
| SaveAs with xlCSV format | Written |
| SaveAs with xlExcel9795 format | Written |
| SaveAs with default format | Written |
| SaveAs with overwrite | Written |

### P2/P4 — Application Properties (`VbaApplicationPropsTest`)
| Test | Status |
|------|--------|
| ScreenUpdating get/set | Written |
| Iteration get/set | Written |
| Calculation get/set | Written |
| CalculationState read | Written |
| DisplayAlerts get/set | Written |
| EnableEvents get/set | Written |
| EnableCancelKey get/set | Written |
| CountA with non-empty cells | Written |
| CountA with empty cells | Written |
| SheetsInNewWorkbook get/set | Written |
| International method | Written |
| Evaluate method | Written |
| Application.Name | Written |
| ActiveWorkbook | Written |
| ThisWorkbook | Written |

### P3 — QueryTable (`VbaQueryTableTest`)
| Test | Status |
|------|--------|
| Range.QueryTable returns non-null | Written |
| Refresh with no link (returns False) | Written |
| Delete with no link (no error) | Written |
| Refresh with background parameter | Written |
| QueryTable on different range | Written |

**Note**: The QueryTable IDL (`XQueryTable.idl`) only defines `Refresh` and `Delete`.
Properties like `Connection`, `CommandText`, and `Destination` are not yet implemented.

### P4 — Range Methods (`VbaRangeTest`)
| Test | Status |
|------|--------|
| Application.Intersect (overlapping) | Written |
| Application.Intersect (non-overlapping) | Written |
| Application.Union | Written |
| Range.Value2 (single cell numeric) | Written |
| Range.Value2 (single cell string) | Written |
| Range.Value2 (multi-cell array) | Written |
| Range.Precedents | Written |
| Range.Dependents | Written |
| Range.Count | Written |
| Range.Rows/Columns count | Written |
| Range.Offset | Written |
| Range.Address | Written |

### P5 — FileDialog (Manual/Interactive)
| Test | Status |
|------|--------|
| FileDialog.Show returns DialogResult | Documented (manual) |

`FileDialog.Show()` requires an interactive desktop session and cannot be tested
headlessly. The test is documented here for manual verification. The VBA FileDialog
object is accessed via `Application.FileDialog(msoFileDialogFilePicker)`.

## 7. Running the Tests

```bash
# Build AOO with VBA support
./configure --enable-vba
./gbuild.sh --type=library sc

# Run the VBA compatibility test suite
cd main/sc
make -r JunitTest_sc_vba
```

The test suite requires:
1. AOO built with `--enable-vba`
2. The `OOO_JUNIT_JAR` environment variable set or available in the build

## 8. Implementation Notes

### BasicCompiler and StarBasic
- The BASIC compiler handles `Option VBASupport 1` via `compiler.cxx`
- VBA objects are resolved through `ScVbaGlobals` (in `vbaglobals.cxx`)
- `Application`, `ActiveSheet`, `ActiveWorkbook`, `ThisWorkbook`, `Workbooks`
  are the main entry points for VBA object model access

### UNO Bridge
- `XScript.invoke()` uses `short[][]` and `Object[][]` as out-parameter holders
- Cell values are read via `XCell.getValue()` (numeric) and `XCell.getString()` (text)
- VBA compatibility mode is set via `XVBACompatibility.setVBACompatibilityMode(true)`
  on `XStorageBasedLibraryContainer` (obtained from `XEmbeddedScripts`)
