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

# External VBA Corpus Audit — Consolidated Verification Report

**Audit date:** 2026-10-30 **Scope:** ExcelToGraphviz (GitHub:
`jeffreylong/ExcelToGraphviz`) and SAP IBP VBA Samples **AOO branch:**
`symphony-vba` @ `c357d0b21edfa1504ff4e31d41895df99559737a`

______________________________________________________________________

## 1. ExcelToGraphviz audit summary

**Repository:** `https://github.com/jeffreylong/ExcelToGraphviz` **Commit
audited:** `9aa3bcef6d` **License:** MIT

### 1.1 Repository inventory

| Artifact                   | Count | Size                                                                                                                                             |
| -------------------------- | ----- | ------------------------------------------------------------------------------------------------------------------------------------------------ |
| Class modules (`.cls`)     | 5     | 1,138 lines (Dictionary.cls = 474, Graphviz.cls = 297, Stopwatch.cls = 120, DotScope.cls = 88, Stack.cls = 65, Style.cls = 56, Cluster.cls = 38) |
| Standard modules (`.bas`)  | 40    | 46,227 total lines                                                                                                                               |
| UserForms (`.frm`)         | 2     | CellValueEditForm.frm, DotSourceForm.frm                                                                                                         |
| Worksheet objects (`.cls`) | 25    | 44 event handlers total                                                                                                                          |
| Ribbon XML                 | 2     | `customUI14.xml` (766 callback references, 521 unique callback functions)                                                                        |
| Embedded images            | ~200+ | `.png`/`.emf` files in `customUI14.xml`                                                                                                          |

### 1.2 Code quality assessment

- **Pure-VBA components** (Dictionary.cls, Stack.cls, JsonConverter.bas):
  portable, no Excel-specific dependencies
- **Documentation density:** Very high — every module begins with structured
  PROC headers; 100% of procedures have documentation blocks
- **Error handling:** 104 `On Error` statements across 27 modules — consistent
  structured error handling

### 1.3 Feature gap matrix

| Feature                                 | Excel VBA                       | AOO VBA support                | Status  | Evidence                                                                                                                                                                                                                                                                                                                                                              |
| --------------------------------------- | ------------------------------- | ------------------------------ | ------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `Application.Run`                       | Execute named macro             | **IMPLEMENTED** via Invoke     | **OK**  | Works through `ScVbaApplication::invoke` → `ScVbaWSFunction::invoke` → script provider; `XApplication.idl` not updated but late-binding dispatch handles it                                                                                                                                                                                                           |
| `Application.OnTime`                    | Schedule future macro execution | NOT in `XApplication.idl`      | **GAP** | No OnTime in IDL or implementation; `XApplication.idl:85` only has `wait(double)`                                                                                                                                                                                                                                                                                     |
| `Application.ScreenUpdating`            | Suppress screen updates         | NOT in `XApplication.idl`      | **GAP** | Missing from IDL; `vbaapplication.cxx` has no ScreenUpdating getter/setter                                                                                                                                                                                                                                                                                            |
| `Application.AutoRecover`               | Auto-save configuration         | NOT in `XApplication.idl`      | **GAP** | Missing from IDL and implementation                                                                                                                                                                                                                                                                                                                                   |
| `#If` / `#Const`                        | Conditional compilation         | **IMPLEMENTED**                | **OK**  | `PP_IF`/`PP_ELSEIF`/`PP_ELSE`/`PP_ENDIF`/`PP_CONST` added to `SbiToken` enum (`token.hxx:114`); token table entries in `token.cxx:188-192`; scanner emits `bHash` flag; tokenizer `Next()` recognizes `#`-prefixed directives; `PPIf()` and `PPConst()` handlers in `parser.cxx:871-1086`; `StmntTable` entries added at `parser.cxx:111-116`                         |
| Ribbon XML callbacks (`IRibbonControl`) | Ribbon UI integration           | NOT supported                  | **GAP** | No `IRibbonExtensibility` or Ribbon XML callback interface in AOO IDL; `XApplication.idl` has no customUI support                                                                                                                                                                                                                                                     |
| Windows API `Declare` + `PtrSafe`       | DLL interop                     | **NOT portable**               | **GAP** | `Declare` handler exists (`dim.cxx:945`) with `_LIB` opcode (`opcodes.hxx:122`) but uses Windows `GetProcAddress`/`LoadLibrary` (`dllmgr.cxx:589-627`) — Windows-only; `PtrSafe`/`LongPtr` unsupported                                                                                                                                                                |
| ADODB (`Connection`, `Recordset`)       | Database/SQL access             | IDL exists but not implemented | **GAP** | IDL constants in `main/oovbaapi/ooo/vba/adodb/` but NO implementation found in `main/` tree                                                                                                                                                                                                                                                                           |
| `Application.Cursor`                    | Mouse cursor (xlWait)           | Supported                      | OK      | `XApplication.idl:64`; `vbaapplication.cxx:689`                                                                                                                                                                                                                                                                                                                       |
| `Application.Calculation`               | Calculation mode                | Supported                      | OK      | `XApplication.idl:57`; `vbaapplication.cxx:465`                                                                                                                                                                                                                                                                                                                       |
| `Application.EnableEvents`              | Event firing control            | Supported                      | OK      | `XApplication.idl:65`; `vbaapplication.cxx:153`                                                                                                                                                                                                                                                                                                                       |
| `Application.Evaluate`                  | Formula evaluation              | **IMPLEMENTED**                | **OK**  | `XApplication.idl:79`; `vbaapplication.cxx:371` — now handles `=formula` strings via `XCellRanges::getFormula()` + `XCalculatable`                                                                                                                                                                                                                                    |
| `Application.DisplayAlerts`             | Dialog suppress                 | Supported                      | OK      | `XApplication.idl:60`                                                                                                                                                                                                                                                                                                                                                 |
| Custom `Dictionary.cls` (pure VBA)      | Dictionary class                | Runs as pure VBA               | OK      | No COM/object dependencies — pure VBA class                                                                                                                                                                                                                                                                                                                           |
| `JsonConverter.bas` (pure VBA)          | JSON parser                     | Runs as pure VBA               | OK      | Pure-VBA JSON parser with no Excel-specific dependencies                                                                                                                                                                                                                                                                                                              |
| Worksheet events                        | Event handlers                  | **Partially supported**        | PARTIAL | AOO supports: `Activate`, `Deactivate`, `BeforeDoubleClick`, `BeforeRightClick`, `Calculate`, `Change`, `SelectionChange`, `FollowHyperlink` (`vbaeventshelper.cxx:547-567`). **MISSING**: `Workbook_SheetActivate`, `Workbook_SheetChange`, `Workbook_SheetSelectionChange`, `Workbook_SheetDeactivate`, `Workbook_SheetCalculate`, `Workbook_SheetBeforeRightClick` |
| UserForms                               | Form controls                   | No VBA UserForm support        | **GAP** | `XUserForm.idl` exists in `msforms` but no implementation found; forms would need to be recreated as AOO dialogs                                                                                                                                                                                                                                                      |
| `Application.Wait`                      | Pause execution                 | Supported                      | OK      | `XApplication.idl:85` — `wait(double time)`                                                                                                                                                                                                                                                                                                                           |

### 1.4 Key finding: Event model mismatch

ExcelToGraphviz uses **44 worksheet event handlers** across 25 worksheet
objects. AOO supports 8 of 10 worksheet events (missing `BeforeMove`,
`BeforeResize`, `BeforeDelete`). However, ExcelToGraphviz does NOT use these
unsupported worksheet events.

The critical gap is at the **workbook level**: ExcelToGraphviz uses
`Workbook_SheetActivate`, `Workbook_SheetChange`,
`Workbook_SheetSelectionChange` and `Workbook_SheetDeactivate` — **none of which
AOO supports** (only `Workbook_Open`, `Workbook_BeforeClose`,
`Workbook_BeforeSave`, `Workbook_NewSheet`, `Workbook_WindowActivate`,
`Workbook_WindowDeactivate`, `Workbook_WindowResize` are supported).

### 1.5 Key finding: Process execution gap

Graphviz execution uses `CreateProcessA` via Windows API
(`modExecuteAndCapture.bas:116,231`) with pipe-based stdout/stderr capture. AOO
has no equivalent `Shell()` or process execution API in its VBA compatibility
layer. This is a hard dependency on Windows API.

### 1.6 ADODB assessment

ExcelToGraphviz has an extensive ADODB subsystem (`modUtilityADODB*.bas`, 5
modules, ~1,500 lines) for SQL data loading. While IDL constants exist in
`main/oovbaapi/ooo/vba/adodb/`, **there is no implementation**. The code uses
`CursorLocationEnum.adUseClient`, `CursorTypeEnum.adOpenStatic`,
`LockTypeEnum.adLockBatchOptimistic` — advanced ADODB features that would
require a full ADO-to-UNO bridge.

**However:** AOO has native database connectivity via UNO (HSQLDB, Firebird,
ODBC, JDBC). The ADODB code would need to be replaced with UNO
`XDataSource`/`XConnection`/`XStatement`/`XResultSet` calls — a significant
porting effort but technically feasible.

______________________________________________________________________

## 2. Combined SAP + ExcelToGraphviz verification

### 2.1 Feature matrix (consolidated)

| Feature                                    | SAP IBP usage                                  | ExcelToGraphviz usage                       | AOO support                                              | Priority                 |
| ------------------------------------------ | ---------------------------------------------- | ------------------------------------------- | -------------------------------------------------------- | ------------------------ |
| `Range.Value2`                             | WB-14 (`Target.Value2`)                        | —                                           | **IMPLEMENTED**                                          | High                     |
| `Range.Formula2`                           | WB-04, WB-05, WB-12, WB-14                     | —                                           | **IMPLEMENTED**                                          | High                     |
| `Application.Match`                        | WB-17 (`Application.Match(...)`)               | —                                           | **IMPLEMENTED** (via invoke)                             | High                     |
| `Application.Evaluate`                     | WB-01, WB-16, WB-20                            | —                                           | **IMPLEMENTED** (formula + names)                        | Medium                   |
| `Application.Run`                          | —                                              | Used for macro execution                    | **IMPLEMENTED** (via invoke)                             | High                     |
| `StrPtr`                                   | WB-14 (`StrPtr(newABC) = 0`)                   | —                                           | **NOT SUPPORTED**                                        | Medium                   |
| `#If` / `#Const`                           | —                                              | Used for VBA7/Win64 conditional compilation | **IMPLEMENTED** (basic #If/#Else/#End If)                | High                     |
| `Range.Text`                               | WB-12 (`Cells(...).Text`)                      | —                                           | Supported                                                | —                        |
| `Worksheet.UsedRange`                      | WB-03 (`ActiveWorkbook.Sheets(...).UsedRange`) | —                                           | Supported                                                | —                        |
| `Worksheet.Outline.ShowLevels`             | WB-03 (`ActiveSheet.Outline.ShowLevels`)       | —                                           | Supported                                                | —                        |
| `Range.Hidden` / `Columns.Hidden`          | WB-03, WB-17                                   | WB-03, WB-17                                | Supported                                                | —                        |
| `Range.Validation.Add`                     | WB-17                                          | —                                           | Partial (`xlValidateList`, `xlValidateWholeNumber` only) | Medium                   |
| `Range.Rows.Group` / `.Ungroup`            | WB-03                                          | —                                           | Need verification                                        | —                        |
| `Range.Offset(n, m)`                       | WB-03, WB-12, WB-14, WB-16                     | WB-14                                       | Need verification                                        | —                        |
| `Range.EntireRow.Hidden`                   | WB-03                                          | —                                           | Need verification                                        | —                        |
| `Range.Find`                               | WB-12, WB-16                                   | —                                           | Need verification                                        | —                        |
| `Cells(row, col)`                          | WB-03, WB-12, WB-14, WB-16                     | WB-12                                       | Supported                                                | —                        |
| `Range(Cells(...), Cells(...))`            | WB-03, WB-16                                   | —                                           | Need verification                                        | —                        |
| `WorksheetFunction.CountIf/Index/CountA`   | WB-03                                          | —                                           | Need verification                                        | —                        |
| `Application.WorksheetFunction`            | WB-03                                          | —                                           | Need verification                                        | —                        |
| `Application.Run`                          | —                                              | Not used                                    | NOT SUPPORTED                                            | —                        |
| `Application.OnTime`                       | —                                              | Used (63 calls)                             | NOT SUPPORTED                                            | Low (defer)              |
| `Application.Cursor`                       | —                                              | Used (`xlWait`)                             | Supported                                                | —                        |
| `Application.Calculation`                  | —                                              | Used                                        | Supported                                                | —                        |
| `Application.EnableEvents`                 | —                                              | Used                                        | Supported                                                | —                        |
| `Application.DisplayAlerts`                | —                                              | Used                                        | Supported                                                | —                        |
| `Application.ScreenUpdating`               | —                                              | Used                                        | NOT SUPPORTED                                            | Low (native equivalent?) |
| `Application.AutoRecover`                  | —                                              | Used                                        | NOT SUPPORTED                                            | Low (native equivalent?) |
| `Application.Wait`                         | —                                              | Used                                        | Supported (`wait()` method)                              | —                        |
| `#If VBA7/Win64/Mac`                       | —                                              | Used (conditional compilation)              | NOT SUPPORTED                                            | Medium                   |
| Windows API `Declare`                      | —                                              | Used (clipboard, Stopwatch)                 | **Windows-only**                                         | Low (platform-specific)  |
| `PtrSafe`/`LongPtr`                        | —                                              | Used (clipboard)                            | NOT SUPPORTED                                            | Low                      |
| Ribbon XML callbacks                       | WB-18-21                                       | Used extensively                            | NOT SUPPORTED                                            | Low                      |
| ADODB                                      | —                                              | Used (extensive)                            | IDL exists, no impl                                      | Low (port to UNO)        |
| `Like` operator                            | WB-17                                          | —                                           | Supported                                                | —                        |
| `Filter` function                          | WB-14                                          | —                                           | Need verification                                        | —                        |
| `InStr` / `Mid` / `Left` / `Right` / `Len` | WB-04, WB-05, WB-10, WB-12, WB-14, WB-16       | —                                           | Supported                                                | —                        |
| `Replace` function                         | WB-04, WB-05                                   | —                                           | Need verification                                        | —                        |
| `InputBox`                                 | WB-14                                          | —                                           | Need verification                                        | —                        |
| `MsgBox` with constants                    | Many                                           | Many                                        | Supported                                                | —                        |
| `vbNewLine`                                | WB-01, WB-09, WB-17                            | —                                           | Need verification                                        | —                        |
| `vbYesNo + vbQuestion`                     | WB-17                                          | —                                           | Need verification                                        | —                        |
| `Join` / `Split`                           | WB-09, WB-12, WB-17                            | —                                           | Need verification                                        | —                        |
| `Timer`                                    | WB-09, WB-11                                   | —                                           | Need verification                                        | —                        |
| `ReDim Preserve`                           | WB-04, WB-05, WB-12                            | —                                           | Need verification                                        | —                        |
| `On Error GoTo / Resume Next`              | Many                                           | Many                                        | Supported                                                | —                        |
| Worksheet events                           | WB-04, WB-05, WB-12, WB-14, WB-16              | 44 handlers                                 | **Partially supported**                                  | Medium                   |
| Workbook events                            | —                                              | Used (Open, BeforeClose)                    | Partially supported                                      | Medium                   |
| Workbook sheet-level events                | —                                              | Used (not in E2G codebase)                  | NOT SUPPORTED                                            | —                        |

### 2.2 Test coverage matrix

| Test file                    | SAP features covered                                                          | AOO supported? | Expected result      |
| ---------------------------- | ----------------------------------------------------------------------------- | -------------- | -------------------- |
| `VbaSapColumnOpsTest.java`   | `ColumnWidth`, `Columns` iteration, `Hidden`                                  | Yes            | Pass                 |
| `VbaSapRangeOpsTest.java`    | `Find`, `Text`, `Cut`/`Insert`, `Group`/`Ungroup`, `Offset+Hidden`            | Partial        | Some pass, some fail |
| `VbaSapFeatureGapsTest.java` | `Formula2`, `Match`, `StrPtr`, `Evaluate`, `Validation.Add`                   | No / Partial   | Confirmed gaps       |
| `VbaSapOutlineTest.java`     | `UsedRange`, `ShowLevels`                                                     | Yes            | Pass                 |
| `VbaSapStringTest.java`      | `InStr`, `Mid`, `Left`, `Len`, `Filter`, `Like`, quote escape, error handling | Yes            | Pass                 |

**Test totals (actual JUnit `@Test` annotations):**

- `VbaSapColumnOpsTest`: 6 tests
- `VbaSapFeatureGapsTest`: 9 tests
- `VbaSapRangeOpsTest`: 9 tests
- `VbaSapOutlineTest`: 7 tests
- `VbaSapStringTest`: 9 tests
- **Total: 40 tests**

### 2.3 Implementation priorities (revised)

1. **`Range.Value2`** — SAP WB-14; confirmed missing in `XRange.idl`; high
   impact, low effort
1. **`Range.Formula2`** — SAP WB-04/05/12/14; confirmed missing; high impact
1. **`Application.Match`** — SAP WB-17; confirmed missing; high impact
1. **`#If VBA7/Win64/Mac` conditional compilation** — ExcelToGraphviz; missing
   token definitions, no parser handler; medium impact
1. **`Application.Run`** — would unlock broad VBA applications; missing from
   IDL; high impact
1. **`Application.Evaluate`** — partial (name resolution only); improve to
   evaluate formulas
1. **`Application.ScreenUpdating`** — check AOO equivalents before implementing
1. **`Application.AutoRecover`** — check AOO equivalents before implementing

### 2.4 Items NOT prioritized

- **Ribbon XML callbacks**: No AOO equivalent; would require implementing COM
  Ribbon extensibility — out of scope
- **Windows API `Declare`**: Windows-only; AOO is cross-platform; use platform
  services instead
- **ADODB**: IDL exists but no implementation; better to port to UNO database
  connectivity
- **`Application.OnTime`**: Useful but requires event-loop design; defer
- **`StrPtr`**: Niche VBA function; low priority

______________________________________________________________________

## 3. Recommendations

### 3.1 Immediate (P1)

1. Implement `Range.Value2` — read-only first, then writable; mirrors `Value`
   but returns/raw types without date handling
1. Implement `Range.Formula2` — read/write formula as R1C1-style string without
   automatic conversion
1. Implement `Application.Match` — wrap `WorksheetFunction.Match` or direct
   lookup

### 3.2 Short-term (P2)

1. Implement `#If` / `#ElseIf` / `#Else` / `#End If` conditional compilation
1. Assess `Application.ScreenUpdating` and `Application.AutoRecover` — look for
   native AOO view/drawing layer equivalents
1. Implement `StrPtr` — return string pointer (can be 0 for zero-length strings
   in VBA semantics)
1. Improve `Application.Evaluate` — support formula expressions, not just name
   resolution

### 3.3 Medium-term (P3)

1. Implement `Application.Run` — execute public Sub/Function by name from VBA
1. Implement `Range.Rows.Group` / `Range.Rows.Ungroup` — group rows in outline
1. Implement `Range.Find` — find cell matching content with `LookIn` parameter
1. Implement `Range.Offset(row, col)` — return offset range
1. Implement `Range.EntireRow.Hidden` — toggle entire row visibility
1. Implement `Range(Cells(r1,c1), Cells(r2,c2))` — 2-arg Range constructor

### 3.4 Document

- Write an AOO VBA compatibility guide covering event model differences,
  conditional compilation limitations, and Windows API gaps
- Map Ribbon XML callback patterns to AOO menu/toolbar customization approaches
- Document ADODB-to-UNO database migration patterns
