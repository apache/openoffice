<!--
 Licensed to the Apache Software Foundation (ASF) under one
 or more contributor license agreements.  See the NOTICE file
 distributed with this work for additional information
 regarding copyright ownership.  The ASF licenses this file
 to you under the Apache License, Version 2.0 (the
 "License"); you may not use this file except in compliance
 with the License.  You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing,
 software distributed under the License is distributed on an
 "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 KIND, either express or implied.  See the License for the
 specific language governing permissions and limitations
 under the License.
-->

# VBA Compatibility Test Suite for Symphony VBA Branch

## Status: Phases 1-4 Complete; Phase 5 Documented as Manual Test

## 1. Overview

This directory (`main/sc/qa/vba/`) houses a compatibility regression test suite
for the VBA-compatible APIs added by the `symphony-vba` branch of Apache
OpenOffice.

The suite verifies that VBA object model implementations in
`main/sc/source/ui/vba/` behave compatibly with Microsoft Excel VBA, using
recorded Excel results as the reference point of truth.

**Primary test mechanism**: Java JUnit tests using UNO to drive a headless AOO
instance. Test macros are BASIC programs that use VBA-compatible syntax
(`Option VBASupport 1`) and exercise VBA object model APIs. Results are written
to cell values and verified by Java assertions through UNO.

## 2. Repository State

- Branch: `symphony-vba`
- Git author: Pedro Giffuni <pfg@apache.org>
- License: Apache 2.0

## 3. APIs Under Test

| API                                                                   | IDL                                  | Implementation                                      | Priority         | Status                    |
| --------------------------------------------------------------------- | ------------------------------------ | --------------------------------------------------- | ---------------- | ------------------------- |
| FormatCondition / FormatConditions                                    | `XFormatCondition.idl`               | `vbaformatcondition.cxx`, `vbaformatconditions.cxx` | P1               | 8 tests written           |
| FileSearch / FoundFiles                                               | `XFileSearch.idl`, `XFoundFiles.idl` | `vbafilesearch.cxx`, `vbafoundfiles.cxx`            | P2               | 6 tests written           |
| Workbook.SaveAs                                                       | `XWorkbook.idl`                      | `vbaworkbook.cxx`                                   | P2               | 4 tests written           |
| Application props (ScreenUpdating, Iteration, CalculationState, etc.) | `XApplication.idl`                   | `vbaapplication.cxx`                                | P2/P4            | 15 tests written          |
| QueryTable                                                            | `XQueryTable.idl`                    | `vbaquerytable.cxx`                                 | P3               | 5 tests written           |
| Range methods (Intersect, Union, Value2, Precedents, Dependents)      | `XRange.idl`                         | `vbarange.cxx`                                      | P4               | 12 tests written          |
| Scripting.Dictionary                                                  | `XDictionary.idl`                    | `vbadictionary.cxx`                                 | P3               | 12 tests written          |
| FileDialog (Show)                                                     | `XFileDialog.idl`                    | `vbafiledialog.cxx`                                 | P5 (interactive) | Documented as manual test |

**Total: 62 test methods across 7 test classes.**

## 4. Test Infrastructure

### 4.1 Build Registration

- `main/sc/JunitTest_sc_vba.mk` — gbuild JUnit test registration
- `main/sc/Module_sc.mk` — adds `JunitTest_sc_vba` under `OOO_JUNIT_JAR` guard
- Platform makefiles (`main/solenv/gbuild/platform/linux.mk:444-446`)
  automatically set `org.openoffice.test.arg.soffice`,
  `org.openoffice.test.arg.env`, and `org.openoffice.test.arg.user` DEFS for all
  JUnit tests

### 4.2 Test Connection

`OfficeConnection` (in `main/test/java/test/`) starts AOO with
`--headless --accept=pipe,...` and connects via UNO URL resolver. The `soffice`
path points to the installed AOO binary.

### 4.3 VBA Macro Execution

1. `VbaCompatTest` base class creates a Calc document with
   `VBACompatibilityMode=true`
1. BASIC test modules are inserted via `XStarBasicAccess.addModule()`
1. Macros are executed via
   `XScriptProviderSupplier.getScriptProvider().getScript().invoke()`
1. Test results are written to cells Z1, Z2, etc. (column 25 for numeric, column
   26 for string)
1. Java test reads values via `XCell.getValue()` / `XCell.getString()`

### 4.4 BASIC Module Convention

VBA compatibility is enabled via `Option VBASupport 1` at the top of each BASIC
module. VBA object model methods (`ActiveSheet.Range`, `Application`,
`ThisWorkbook`, etc.) are available within these modules.

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

The base class `VbaCompatTest` provides `createTempFile()` and
`createTempSubdir()` methods to create deterministic test fixtures. These are
used to:

- Provide known files for FileSearch tests (instead of relying on
  `Environ("TEMP")`)
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
    VbaLibreNamedRangesTest.java    — LO-style: cross-sheet names, RefersToRange, string indexer (6 tests)
    VbaLibreOptionalArgsTest.java   — LO-style: optional arg handling for Offset/Address/Cells/etc (8 tests)
    VbaLibreArraysTest.java         — LO-style: Dim arr(n) with Option Base 0/1, explicit To bounds, Array(), Split (10 tests)
    VbaLibreVariantTest.java        — LO-style: VarType/TypeName/TypeConversion for Variant (10 tests)
    VbaLibreRangeOpsTest.java       — LO-style: Intersect/Union/MergeArea edge cases (11 tests)
    VbaLibreLocaleTest.java         — LO-style: locale number parsing, NBSP thousands sep (10 tests)
    VbaLibreErrorTest.java          — LO-style: On Error, Err, EnableCancelKey, state cleanup (9 tests)
main/sc/JunitTest_sc_vba.mk         — gbuild JUnit test registration
main/sc/Module_sc.mk                — (modified) adds JunitTest_sc_vba to build
```

**Phase 1-4 tests**: 8 registered test classes, 62 test methods (existing).
**Phase 5 tests**: 7 LibreOffice-style failure hypothesis classes (64 test
methods total) as untracked source files (not registered in build per user
preference).

## 6. Test Summary

### P1 — FormatCondition / FormatConditions (`VbaFormatConditionTest`)

| Test                                   | Status  |
| -------------------------------------- | ------- |
| Add with xlExpression type             | Written |
| Add with xlCellValue type              | Written |
| Modify condition type and formula      | Written |
| Delete all (FormatConditions.Delete)   | Written |
| Delete single (FormatCondition.Delete) | Written |
| Item access by index                   | Written |
| For Each enumeration                   | Written |
| Formula1/Formula2 properties           | Written |

### P2 — FileSearch / FoundFiles (`VbaFileSearchTest`)

| Test                                                              | Status  |
| ----------------------------------------------------------------- | ------- |
| Properties (FileName, LookIn, SearchSubFolders, MatchTextExactly) | Written |
| NewSearch reset                                                   | Written |
| Execute on empty result set                                       | Written |
| Wildcard pattern search (\*.xlsx)                                 | Written |
| MatchTextExactly search                                           | Written |
| With SearchSubFolders = True                                      | Written |
| FoundFiles.Item access                                            | Written |

### P2 — Workbook.SaveAs (`VbaSaveAsTest`)

| Test                           | Status  |
| ------------------------------ | ------- |
| SaveAs with xlCSV format       | Written |
| SaveAs with xlExcel9795 format | Written |
| SaveAs with default format     | Written |
| SaveAs with overwrite          | Written |

### P2/P4 — Application Properties (`VbaApplicationPropsTest`)

| Test                        | Status  |
| --------------------------- | ------- |
| ScreenUpdating get/set      | Written |
| Iteration get/set           | Written |
| Calculation get/set         | Written |
| CalculationState read       | Written |
| DisplayAlerts get/set       | Written |
| EnableEvents get/set        | Written |
| EnableCancelKey get/set     | Written |
| CountA with non-empty cells | Written |
| CountA with empty cells     | Written |
| SheetsInNewWorkbook get/set | Written |
| International method        | Written |
| Evaluate method             | Written |
| Application.Name            | Written |
| ActiveWorkbook              | Written |
| ThisWorkbook                | Written |

### P3 — QueryTable (`VbaQueryTableTest`)

| Test                                 | Status  |
| ------------------------------------ | ------- |
| Range.QueryTable returns non-null    | Written |
| Refresh with no link (returns False) | Written |
| Delete with no link (no error)       | Written |
| Refresh with background parameter    | Written |
| QueryTable on different range        | Written |

**Note**: The QueryTable IDL (`XQueryTable.idl`) only defines `Refresh` and
`Delete`. Properties like `Connection`, `CommandText`, and `Destination` are not
yet implemented.

### P4 — Range Methods (`VbaRangeTest`)

| Test                                    | Status  |
| --------------------------------------- | ------- |
| Application.Intersect (overlapping)     | Written |
| Application.Intersect (non-overlapping) | Written |
| Application.Union                       | Written |
| Range.Value2 (single cell numeric)      | Written |
| Range.Value2 (single cell string)       | Written |
| Range.Value2 (multi-cell array)         | Written |
| Range.Precedents                        | Written |
| Range.Dependents                        | Written |
| Range.Count                             | Written |
| Range.Rows/Columns count                | Written |
| Range.Offset                            | Written |
| Range.Address                           | Written |

### P5 — FileDialog (Manual/Interactive)

| Test                                 | Status              |
| ------------------------------------ | ------------------- |
| FileDialog.Show returns DialogResult | Documented (manual) |

`FileDialog.Show()` requires an interactive desktop session and cannot be tested
headlessly. The test is documented here for manual verification. The VBA
FileDialog object is accessed via
`Application.FileDialog(msoFileDialogFilePicker)`.

## 6.2. LibreOffice-Style Failure Hypothesis Tests

The following test classes probe known trouble areas from the LibreOffice VBA
port. They exist as **untracked source files** in `src/` and are **not**
registered in `JunitTest_sc_vba.mk` (per user preference). They exist solely to
surface potential failure hypotheses informed by the old third-party LibreOffice
VBA port's documented trouble areas. Each test is designed to probe a specific
failure mode rather than verify a known-good behavior.

### Code Fixes Applied (in response to failure hypotheses)

| Fix                                  | File:Line                                         | Description                                                                                                                     |
| ------------------------------------ | ------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------- |
| TypeName for null objects            | `main/basic/source/runtime/methods.cxx:3623-3637` | Returns "Nothing" instead of "Object" for null in VBA mode                                                                      |
| Array() 0-based in VBA               | `main/basic/source/runtime/methods1.cxx:771`      | `Array()` is always 0-based per VBA spec (ignores `Option Base 1` in VBA mode)                                                  |
| NamedRanges.Add string RefersTo      | `main/sc/source/ui/vba/vbanames.cxx:122-151`      | String addresses like `"=Sheet1!A1:A3"` are now parsed via `getRangeObjectForName` after stripping `=` prefix                   |
| NamedRange.Name without sheet prefix | `main/sc/source/ui/vba/vbaname.cxx:64-72`         | `getName()` returns raw name (was prepending `Sheet1!`)                                                                         |
| NamedRange.RefersToRange fix         | `main/sc/source/ui/vba/vbaname.cxx:228-234`       | Uses `getValue()` (formatted VBA address) instead of `getName()` to resolve the range                                           |
| NBSP as thousands separator          | `main/basic/source/sbx/sbxscan.cxx:71-80`         | Replaces U+00A0 with space before ASCII-US conversion; also normalizes locale `cThousandSep`                                    |
| Array base respects Option Base      | `main/basic/source/runtime/step1.cxx:551-564`     | `StepBASED` in VBA mode uses the compiled `Option Base` value (0 or 1), not forced 1                                            |
| DefType respects Option Base         | `main/basic/source/comp/dim.cxx:615-632`          | Same as above for type-member arrays (DefType); `Dim arr(n)` with `Option Base 0` -> `0 To n`, with `Option Base 1` -> `1 To n` |
| JunitTest registration               | `main/sc/JunitTest_sc_vba.mk`                     | Phase 1-4 tests already registered; 7 LibreOffice-style files left unassigned per user preference                               |

### Failure Hypothesis Summary

| Test File                  | Test Count | Primary Hypotheses                                                                                                                        | Status      |
| -------------------------- | ---------- | ----------------------------------------------------------------------------------------------------------------------------------------- | ----------- |
| `VbaLibreNamedRangesTest`  | 6          | Workbook-level vs worksheet-level `Names()` scoping; `RefersToRange` resolution; string indexer access                                    | Exploratory |
| `VbaLibreOptionalArgsTest` | 8          | Omitted VBA optional arguments may not convert to `Missing`; `Offset()`/`Resize()`/`Address()` default params; `Cells(row, col)` indexing | Exploratory |
| `VbaLibreArraysTest`       | 10         | `Dim arr(n)` with `Option Base 0` vs `1` vs explicit `To`; `Array()` always 0-based; `Split()`; Range.Value 1-based                       | Exploratory |
| `VbaLibreVariantTest`      | 10         | Variant subtypes (Empty, Null, Error) preserved through UNO Any bridge; `VarType`/`TypeName` codes; `CVErr` support; type conversion      | Exploratory |
| `VbaLibreRangeOpsTest`     | 11         | `Intersect`/`Union` overlap removal; non-contiguous ranges; `MergeArea` on merged cells; `Count` semantics                                | Exploratory |
| `VbaLibreLocaleTest`       | 10         | `CDbl` with comma decimal separator; NBSP as thousands separator; `Format`/`Str`/`Val`/`CSng` locale sensitivity                          | Exploratory |
| `VbaLibreErrorTest`        | 9          | `On Error Resume Next` suppression; `Err.Number`/`Description`; `EnableCancelKey`; error state cleanup after exception                    | Exploratory |

### Test Status Distinctions

| Status          | Definition                                                                                    |
| --------------- | --------------------------------------------------------------------------------------------- |
| **Exploratory** | Written but not executed or verified. Hypotheses need validation against AOO runtime.         |
| **Validated**   | Run against AOO and checked against documented expected behavior.                             |
| **Regression**  | Integrated into the test build (`JunitTest_sc_vba.mk`) and suitable for repeatable execution. |

All 7 LibreOffice-style test classes are currently **Exploratory**. They are not
integrated into the build and require a full AOO build with `--enable-vba` to
run.

### Known Missing Functionality

| API                                   | Status                                                         | File                                                    |
| ------------------------------------- | -------------------------------------------------------------- | ------------------------------------------------------- |
| `Application.Run` method              | Missing from XApplication.idl                                  | `main/oovbaapi/ooo/vba/excel/XApplication.idl:95`       |
| `Application.EnableCancelKey` get/set | Implemented in interop but may not support xlErrorHandler mode | `main/sc/source/ui/vba/vbaapplication.cxx`              |
| `Range.Worksheet` property            | Not in XRange.idl                                              | `main/oovbaapi/ooo/vba/excel/XRange.idl`                |
| `XWorksheetFunction` interface        | IDL is empty — no actual Excel functions exposed               | `main/oovbaapi/ooo/vba/excel/XWorksheetFunction.idl:48` |

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
1. The `OOO_JUNIT_JAR` environment variable set or available in the build

## 8. Implementation Notes

### BasicCompiler and StarBasic

- The BASIC compiler handles `Option VBASupport 1` via `compiler.cxx`
- VBA objects are resolved through `ScVbaGlobals` (in `vbaglobals.cxx`)
- `Application`, `ActiveSheet`, `ActiveWorkbook`, `ThisWorkbook`, `Workbooks`
  are the main entry points for VBA object model access

### UNO Bridge

- `XScript.invoke()` uses `short[][]` and `Object[][]` as out-parameter holders
- Cell values are read via `XCell.getValue()` (numeric) and `XCell.getString()`
  (text)
- VBA compatibility mode is set via
  `XVBACompatibility.setVBACompatibilityMode(true)` on
  `XStorageBasedLibraryContainer` (obtained from `XEmbeddedScripts`)
