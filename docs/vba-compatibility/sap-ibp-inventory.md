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

# SAP IBP VBA Samples — Repository Inventory

**Audit target repository:**
`SAP-samples/integrated-business-planning-excel-addin-vba-samples` **Default
branch audited:** `main` **Commit SHA:**
`904213f33ea292793a245f3bca3c1c3d36fdc141` **Commit date:** 2026-06-17
**License:** Apache-2.0 **Cloned to:** `/tmp/sap-ibp-vba-samples`

______________________________________________________________________

## 1. Tracked tree summary

| Path                      | Type     | VBA? | Documentation                           |
| ------------------------- | -------- | ---- | --------------------------------------- |
| `README.md`               | Markdown | —    | Project overview, requirements, license |
| `LICENSE`                 | Text     | —    | Apache-2.0                              |
| `LICENSES/Apache-2.0.txt` | Text     | —    | Full license text                       |
| `REUSE.toml`              | TOML     | —    | SPDX metadata                           |

### VBA sample workbooks (.xlsm)

| #   | Repository-relative path                                                                                                                                                   | Purpose                                      | Modules | Code chars |
| --- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | -------------------------------------------- | ------- | ---------- |
| 1   | `VBA samples/01 Getting started - Test VBA functions.xlsm`                                                                                                                 | Exposes all SAP IBP VBA methods              | 6       | 5,395      |
| 2   | `VBA samples/Automatically reducing the width of empty columns/Automatically reducing the width of empty columns.xlsm`                                                     | Auto-resize empty columns after refresh      | 5       | 3,412      |
| 3   | `VBA samples/Keyfigure grouping/Key figure grouping.xlsm`                                                                                                                  | Group/ungroup key figure rows                | 4       | 4,782      |
| 4   | `VBA samples/Samples for SAP IBP APIs released with 2202/1.1_Create Customized Navigation Patterns/Example1_Main and Details View.xlsm`                                    | Navigate main→details on double-click        | 4       | 6,135      |
| 5   | `VBA samples/Samples for SAP IBP APIs released with 2202/1.1_Create Customized Navigation Patterns/Example2_Key figure dependent navigation.xlsm`                          | Navigate to key-figure-specific sheets       | 5       | 7,935      |
| 6   | `VBA samples/Samples for SAP IBP APIs released with 2202/1.2_Combination-Based Planning/Combination-Based Planning.xlsm`                                                   | Next/Prev/Save-and-next combination workflow | 4       | 8,190      |
| 7   | `VBA samples/Samples for SAP IBP APIs released with 2202/1.3_Trigger SAP IBP Actions Using Shortcut Keys/Trigger SAP IBP actions via shortcut keys.xlsm`                   | Assign IBP Simulate to a shortcut key        | 3       | 3,104      |
| 8   | `VBA samples/Samples for SAP IBP APIs released with 2202/1.4_Run Simulations Using VBA Buttons/Run Simulations Using VBA Buttons.xlsm`                                     | Button-triggered forecast simulation         | 4       | 3,408      |
| 9   | `VBA samples/Samples for SAP IBP APIs released with 2202/1.5_Measure the Performance of SAP IBP Actions/Measure the Performance of SAP IBP Actions.xlsm`                   | Upgrade-test performance harness (2202)      | 4       | 4,944      |
| 10  | `VBA samples/Samples for SAP IBP APIs released with 2202/1.6_Prefill reason codes and comments when saving data/Prefill Reason Codes and Comments when Saving Data.xlsm`   | Pre-fill reason codes/comments on SaveData   | 3       | 3,646      |
| 11  | `VBA samples/Samples for SAP IBP APIs released with 2205/1.1_Measure the Performance of SAP IBP Actions/Measure the Performance of SAP IBP Actions.xlsm`                   | Upgrade-test performance harness (2205)      | 3       | 4,118      |
| 12  | `VBA samples/Samples for SAP IBP APIs released with 2205/1.2_Show Master Data Details/Show Master Data Details.xlsm`                                                       | Show master-data attributes on double-click  | 4       | 5,007      |
| 13  | `VBA samples/Samples for SAP IBP APIs released with 2205/1.2_Show Master Data Details/Show Master Data Details_WithNames.xlsm`                                             | Variant of #12 with named ranges             | 5       | 5,622      |
| 14  | `VBA samples/Samples for SAP IBP APIs released with 2205/1.3_Change Master Data Attribute Value/Change ABC Code.xlsm`                                                      | Edit ABC code via InputBox                   | 3       | 4,282      |
| 15  | `VBA samples/Samples for SAP IBP APIs released with 2205/1.4_Combination-Based Planning with Changing the Status/Combination-Based Planning with Changing the Status.xlsm` | Combination planning + status change         | 4       | 8,464      |
| 16  | `VBA samples/Scroll to current period/Scroll to current period.xlsm`                                                                                                       | Scroll window to current time period         | 5       | 4,278      |

### VBA add-in workbooks (.xlam)

| #   | Repository-relative path                                                                                                           | Purpose                                                                         | Modules | Code chars |
| --- | ---------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------- | ------- | ---------- |
| 17  | `VBA samples/SAP IBP Hooks/SAP_IBP_VBA_Hook_2005_and_Later_versions.xlam`                                                          | VBA hooks: IBPBeforeSend, IBPAfterRefresh, IBPMDAfterRefresh, IBPMDBeforeUpdate | 3       | 6,532      |
| 18  | `VBA samples/SAP IBP Ribbon Extension/1.1_Sample to explain the content of the .xml and .xlam file/IBPExtension.xlam`              | All Ribbon callback function stubs                                              | 6       | 12,544     |
| 19  | `VBA samples/SAP IBP Ribbon Extension/1.2_Sample to add one basic function/IBPExtension.xlam`                                      | Login + basic Ribbon callbacks                                                  | 6       | 12,531     |
| 20  | `VBA samples/SAP IBP Ribbon Extension/1.3_Sample for 2302 and newer/IBPExtension.xlam`                                             | Login, OpenFavorite, Ribbon enable/disable callbacks                            | 6       | 14,876     |
| 21  | `VBA samples/Samples for SAP IBP APIs released with 2202/1.6_Prefill reason codes and comments when saving data/IBPExtension.xlam` | Ribbon callback for prefill save                                                | 3       | 1,673      |

### Documentation files

| Path                                                                                                                                                          | Description                                                                                              |
| ------------------------------------------------------------------------------------------------------------------------------------------------------------- | -------------------------------------------------------------------------------------------------------- |
| `README.md`                                                                                                                                                   | Project description, requirements (SAP IBP tenant needed), installation, known issues (ActiveX controls) |
| `Samples for SAP IBP APIs released with 2202/1.0_Overview of the Use Cases Provided.pdf`                                                                      | Overview of 2202 use cases                                                                               |
| `Samples for SAP IBP APIs released with 2205/1.0_Overview of the Use Cases Provided.pdf`                                                                      | Overview of 2205 use cases                                                                               |
| `Samples for SAP IBP APIs released with 2202/1.1_Create Customized Navigation Patterns/Create Customized Navigation Patterns.pdf`                             | Navigation pattern documentation                                                                         |
| `Samples for SAP IBP APIs released with 2202/1.2_Combination-Based Planning/Combination-Based Planning.pdf`                                                   | Combination-based planning documentation                                                                 |
| `Samples for SAP IBP APIs released with 2202/1.3_Trigger SAP IBP Actions Using Shortcut Keys/Trigger SAP IBP Actions Using Shortcut Keys.pdf`                 | Shortcut key documentation                                                                               |
| `Samples for SAP IBP APIs released with 2202/1.4_Run Simulations Using VBA Buttons/Run Simulations Using VBA Buttons.pdf`                                     | VBA button simulation documentation                                                                      |
| `Samples for SAP IBP APIs released with 2202/1.5_Measure the Performance of SAP IBP Actions/Measure the Performance of SAP IBP Actions.pdf`                   | Performance measurement documentation (2202)                                                             |
| `Samples for SAP IBP APIs released with 2202/1.6_Prefill reason codes and comments when saving data/Prefill Reason Codes and Comments when Saving Data.pdf`   | Prefill reason codes documentation                                                                       |
| `Samples for SAP IBP APIs released with 2205/1.1_Measure the Performance of SAP IBP Actions/Measure the Performance of SAP IBP Actions.pdf`                   | Performance measurement documentation (2205)                                                             |
| `Samples for SAP IBP APIs released with 2205/1.2_Show Master Data Details/Show Master Data Details.pdf`                                                       | Master data details documentation                                                                        |
| `Samples for SAP IBP APIs released with 2205/1.3_Change Master Data Attribute Value/Change Master Data Attribute Value.pdf`                                   | Master data change documentation                                                                         |
| `Samples for SAP IBP APIs released with 2205/1.4_Combination-Based Planning with Changing the Status/Combination-Based Planning with Changing the Status.pdf` | Combination planning with status change                                                                  |
| `SAP IBP Hooks/Sample code for SAP IBP Hooks.pdf`                                                                                                             | Hooks documentation                                                                                      |
| `SAP IBP Ribbon Extension/Samples for the SAP IBP Ribbon Extension.pdf`                                                                                       | Ribbon extension documentation                                                                           |

### Other files

| Path                                          | Description                                                      |
| --------------------------------------------- | ---------------------------------------------------------------- |
| `VBA samples/01 Getting started … .mp4`       | Video tutorial for the getting-started sample                    |
| `VBA samples/Automatically reducing … .mp4`   | Video tutorial for the column-width sample                       |
| `VBA samples/Scroll to current period … .mp4` | Video tutorial for the scroll sample                             |
| `SAP IBP Ribbon Extension/1.1…/RibbonXML.xml` | Ribbon XML with Button, ToggleButton, Menu, Separator components |
| `SAP IBP Ribbon Extension/1.2…/RibbonXML.xml` | Ribbon XML with single Button + IsOffline callback               |
| `SAP IBP Ribbon Extension/1.3…/RibbonXML.xml` | Ribbon XML with Menu + onActionParameters (2302+ features)       |
| `…/1.6_Prefill…/RibbonXML.xml`                | Ribbon XML for prefill save                                      |
| `…/1.6_Prefill…/IBPExtension.xlam`            | Add-in for prefill save                                          |

______________________________________________________________________

## 2. Per-workbook inventory

### WB-01: `01 Getting started - Test VBA functions.xlsm`

**Purpose (repo):** Expose all SAP IBP VBA methods. Intended as a comprehensive
reference of available API calls.

**Modules extracted:** Sheet1.cls, Sheet2.cls, ThisWorkbook.cls, UserForm1.frm,
Module1.bas, Module2.bas

**Procedures in Module1.bas:**

- `testIBPVBA()` — Demonstrates: `COMAddIns`, `GetActiveReportName`,
  `setSheetOption`, `getSheetOption`, `getActiveConnection`,
  `GetRowAxisDimensionCount`, `GetRowAxisMembers`,
  `GetColumnAxisDimensionCount`, `GetColumnAxisMembers`,
  `getDataBottomRightCell`, `getDataTopLeftCell`,
  `CreateObject("System.Collections.ArrayList")`, `getMemberDimension`,
  `On Error Resume Next`, `ArrayList.Contains`, `ArrayList.Add`, `getShift`,
  `GetDimensionList`, `EnableLocalMember`, `SwitchToIbpTab`,
  `Application.Evaluate("=IBPUser()")`

**Procedures in Module2.bas:**

- `AFTER_REFRESH() As Boolean` — Calls `testIBPVBA`

**References / dependencies:** SAP IBP COM add-in (`IBPXLClient.Connect`),
`System.Collections.ArrayList`

**Potentially useful VBA features:**

- `CreateObject("System.Collections.ArrayList")` — late-bound COM object
  creation
- `For Each ... In` loop over Variant array returned by SAP API
- `Application.Evaluate` with custom formula `=IBPUser()`
- `On Error Resume Next` error handling
- `ArrayList.Contains`, `ArrayList.Add` methods

**Potentially useful Excel object-model features:** None beyond
`Application.COMAddIns` and `Application.Evaluate`. The code is almost entirely
SAP-specific API calls.

**Self-contained?** No — requires an active SAP IBP planning view.

**Priority:** EXCLUDE — every call depends on the SAP IBP COM add-in. No
independent VBA behavior to test.

______________________________________________________________________

### WB-02: `Automatically reducing the width of empty columns/Automatically reducing the width of empty columns.xlsm`

**Purpose (repo/PDF):** After data refresh, automatically set the `ColumnWidth`
of empty columns to a fixed value (1).

**Modules extracted:** Sheet1.cls, ThisWorkbook.cls, UserForm1.frm,
VBA_subs.bas, SAPIBP_AFTERREFRESH.bas

**Procedures:**

- `VBA_subs.hideEmptyColumn()` — Key procedure. Uses:
  - `Application.COMAddIns("IBPXLClient.Connect").Object`
  - `IBPAutomationObject.GetActiveReportName(ActiveSheet)`
  - `IBPAutomationObject.getDataTopLeftCell(ActiveSheet, rptID)`
  - `Range(topLeftCell).Column` — get column number of a Range
  - `Range.ColumnWidth = 1` — set column width
  - `For Each col In ActiveSheet.Columns` — iterate columns collection
  - `col.Column` — access Column property of a column range
  - `Exit For` — early loop exit
  - `On Error Resume Next` (in the SAP hook variant)
- `SAPIBP_AFTERREFRESH.AFTER_REFRESH() As Boolean` — Calls `hideEmptyColumn`

**References / dependencies:** SAP IBP COM add-in

**Potentially useful VBA features:**

- `For Each col In ActiveSheet.Columns` — iterating worksheet columns collection
- `col.Column` — Column property access
- `col.ColumnWidth = 1` — Range.ColumnWidth property setter
- `Range(topLeftCell).Column` — resolving a string address to a Range and
  reading Column
- Numeric `Integer` variable types
- `Option Base 0` default for `Integer`

**Potentially useful Excel object-model features:**

- `ActiveSheet.Columns` enumeration
- `Range.ColumnWidth` (setter)
- `Range.Column` (getter)
- `ActiveSheet.Range(...)` (string-address resolution)

**Self-contained?** Partially — the column-width manipulation can be reproduced
with a synthetic range; the SAP `getDataTopLeftCell` call is not needed.

**Priority:** P1 — the column-width logic is purely Excel object model. High
value, low effort.

______________________________________________________________________

### WB-03: `Keyfigure grouping/Key figure grouping.xlsm`

**Purpose (repo):** Group and ungroup key figure rows in a planning view.

**Modules extracted:** Sheet1.cls, Sheet14.cls, ThisWorkbook.cls, VBA_subs.bas

**Procedures in VBA_subs.bas:**

- `Set_KF_Grouping()` — Key procedure. Uses:
  - `Application.COMAddIns("IBPXLClient.Connect").Object` (SAP)
  - `IBPAutomationObject.GetActiveReportName(ActiveSheet)` (SAP)
  - `MsgBox(...)`, `Exit Sub`
  - `Application.ActiveSheet.Name`
  - `Range("KFGroups").Rows.Count` — Named range row count
  - `Range("KFHeader").Value` — Named range value
  - `WorksheetFunction.CountIf(Range("KFGroups"), "X")` — Excel worksheet
    function
  - `ReDim KFArray(KFCount)` — dynamic array sizing
  - `For tablerow = 1 To KFGroupRows` loop
  - `WorksheetFunction.Index(Range("KFGroups"), tablerow, 1)` — 2D worksheet
    function
  - `Exit For`
  - `Range(topLeftCell).Row`, `Range(topLeftCell).Column`
  - `Range(Cells(KFRow, KFCol), Cells(KFRow, KFCol))` — Cells(row, col)
    constructor
  - `WorksheetFunction.CountA(Columns(KFCol))` — Column via Columns() function
  - `On Error Resume Next`
  - `Range("planningViewRange").Rows.Ungroup` — Range.Rows.Ungroup
  - `Range(...).Offset(3, 0).EntireRow.Hidden = False` — Offset + EntireRow +
    Hidden
  - `KFCell.Offset(groupRow, 0).Rows.Group` — Range.Rows.Group
  - `Range(groupRow, 0)` — Offset with two args
  - `For Each arrayItem In KFArray` — iterate array
  - `ActiveSheet.Outline.ShowLevels RowLevels:=Range("Auto_ReGroup").Value` —
    Outline.ShowLevels with named range

**Procedures in VBA_subs.bas (continued):**

- `Set_Ungroup()` — Uses:
  - `Application.ActiveSheet.Name`
  - `On Error Resume Next`
  - `ActiveWorkbook.Sheets(sheetName).UsedRange` — UsedRange property
  - `Range.Rows.Ungroup`
  - `Range.Offset(3, 0).EntireRow.Hidden = False`

**References / dependencies:** SAP IBP COM add-in, named ranges `KFGroups`,
`KFHeader`, `Auto_ReGroup`

**Potentially useful VBA features:**

- `WorksheetFunction` object: `CountIf`, `Index`, `CountA`
- `ReDim` of a fixed-size 1D array
- `On Error Resume Next` with `UBound` check for error suppression
- `Exit For` in nested loops
- `Like` operator (not used here but in hooks)

**Potentially useful Excel object-model features:**

- `Range("name").Rows.Count`
- `Range("name").Value`
- `Cells(row, col)` constructor
- `Columns(col)` function
- `Range.Rows.Ungroup` / `Range.Rows.Group`
- `Range.Offset(row, col).EntireRow.Hidden = Boolean`
- `Range.Rows.Group`
- `ActiveSheet.Outline.ShowLevels RowLevels:=Variant`
- `ActiveWorkbook.Sheets(name).UsedRange`
- `ActiveSheet.Name`

**Self-contained?** Partially — requires SAP `getDataTopLeftCell`, but the
grouping/ungrouping/column-width logic can be reproduced with synthetic ranges.

**Priority:** P1 — exercises `Rows.Group`, `Rows.Ungroup`, `EntireRow.Hidden`,
`Cells(row, col)`, `Columns(col)`, `WorksheetFunction`, `ReDim`, `UsedRange`,
`Outline.ShowLevels`. Many of these are either unsupported or untested in AOO.

______________________________________________________________________

### WB-04: `1.1_Create Customized Navigation Patterns/Example1_Main and Details View.xlsm`

**Purpose (PDF):** Navigate from a main planning view to a details planning view
by double-clicking a cell.

**Modules extracted:** Sheet2.cls, Sheet5.cls, Sheet6.cls, Sheet7.cls,
ThisWorkbook.cls

**Procedures in Sheet2.cls:**

- `Worksheet_BeforeDoubleClick(ByVal Target As Range, Cancel As Boolean)` — Key
  event handler. Uses:
  - `Target.Formula2` — Excel 2010+ Formula2 property (reads cell formula)
  - `InStr(1, formula, "=@ EPMOlapMemberO(""[KEY_FIGURES].[].[")` — InStr with
    embedded double-quotes in string literal
  - `Replace(formula, ...)` — string replacement
  - `Left(keyFigure, InStr(1, keyFigure, "]""") - 1)` — Left + InStr parsing
  - `On Error GoTo ErrorHandling:` — structured error handling
  - `Application.COMAddIns(...)` (SAP)
  - `IBPAutomationObject.GetAttributeValues(Target.Offset(0, 1))` (SAP)
  - `IBPAutomationObject.GetFilterValues` (SAP)
  - `MergeArray(attributes, filterValues)` — calling a custom function
  - `IBPAutomationObject.SetFilterValues(filterToPass, targetSheet)` (SAP)
  - `targetSheet.Activate` — Worksheet.Activate
  - `MsgBox Err.Description, vbOKOnly, "title"`
  - `Exit Sub`
  - `GoTo ErrorHandling:` label with error handler

**Procedures in Sheet2.cls (continued):**

- `MergeArray(list1 As Variant, list2 As Variant) As Variant` — Custom function:
  - `UBound(list1)`, `UBound(list2)` — array upper bound
  - `result = list1` — Variant assignment of entire array
  - `ReDim Preserve result(size1 + size2 + 1)` — ReDim Preserve
  - `For i = 0 To size2` loop
  - `result(size1 + 1 + i) = list2(i)` — array element assignment
  - `MergeArray = result` — return array via function

**References / dependencies:** SAP IBP COM add-in, EPMOlapMemberO formulas in
cells

**Potentially useful VBA features:**

- `Target.Formula2` — Range.Formula2 property (Excel 2010+)
- `Worksheet_BeforeDoubleClick` event with
  `ByVal Target As Range, Cancel As Boolean`
- `InStr` with multi-argument form `InStr(start, string, substring)`
- `Replace`, `Left` string functions
- `ReDim Preserve` for array resizing
- `Variant` parameters for arrays
- `On Error GoTo` structured error handling
- `GoTo` label with error handler section

**Potentially useful Excel object-model features:**

- `Range.Formula2` (property)
- `Worksheet.Activate`
- `Worksheet_BeforeDoubleClick` event
- `Range.Offset(0, 1)` (two-argument form)
- `MsgBox` with `vbOKOnly` and custom title

**Self-contained?** No — but many features are standard VBA/Excel.

**Priority:** P2 — `Formula2` is likely unsupported. `ReDim Preserve` with
Variant arrays is interesting. The event handler pattern is standard.

______________________________________________________________________

### WB-05: `1.1_Create Customized Navigation Patterns/Example2_Key figure dependent navigation.xlsm`

**Purpose (PDF):** Navigate to a key-figure-specific worksheet on double-click.

**Modules extracted:** Sheet5.cls, Sheet2.cls, Sheet6.cls, ThisWorkbook.cls

**Procedures in Sheet5.cls:**

- `Worksheet_BeforeDoubleClick(ByVal Target As Range, Cancel As Boolean)` — Key
  procedure. Uses:
  - `Target.Formula2` — Excel 2010+ Formula2 property
  - `InStr(1, formula, "=@ EPMOlapMemberO(""[KEY_FIGURES].[].[")` — InStr with
    embedded double quotes
  - `Replace(formula, "=@ EPMOlapMemberO(""[KEY_FIGURES].[].[", "")` — Replace
    with multi-double-quote argument
  - `Left(keyFigure, InStr(1, keyFigure, "]""") - 1)` — Left + InStr
  - `On Error GoTo NWS:`, `On Error GoTo ErrorHandling:` — multiple error
    handlers
  - `Set targetSheet = ActiveWorkbook.Sheets(keyFigure)` — Sheets(name) by name
  - `On Error GoTo ErrorHandling:` after the first handler (error handler
    chaining)
  - `IBPAutomationObject.GetAttributeValues(Target.Offset(0, 1))` (SAP)
  - `IBPAutomationObject.GetFilterValues` (SAP)
  - `MergeArray(attributes, filterValues)` — custom function
  - `IBPAutomationObject.SetFilterValues(filterToPass, targetSheet)` (SAP)
  - `targetSheet.Activate`
  - `MsgBox ...`
  - `Exit Sub`
- `MergeArray(list1 As Variant, list2 As Variant) As Variant` — Same as WB-04
  - `UBound`, `ReDim Preserve`, `For i = 0 To ...`, array element assignment,
    function return

**Potentially useful Excel object-model features:** See WB-04. Same pattern.

**Self-contained?** No — SAP-dependent.

**Priority:** P2 — same as WB-04. `Formula2` and `Replace` with multi-quote
strings.

______________________________________________________________________

### WB-06: `1.2_Combination-Based Planning/Combination-Based Planning.xlsm`

**Purpose (PDF):** Navigate through planning combinations one by one. Buttons:
Refresh Worklist, Previous, Next, Save and Next.

**Modules extracted:** Sheet2.cls, Sheet5.cls, Sheet6.cls, ThisWorkbook.cls

**Procedures in Sheet2.cls:**

- `Worksheet_BeforeDoubleClick` — Empty (just event signature)
- `RefreshList_Click()` — Key procedure:
  - `IBPAutomationObject` (SAP)
  - `rptID = IBPAutomationObject.GetActiveReportName(ActiveSheet)` (SAP)
  - `If rptID = "" Then ... Exit Sub` — empty-string check
  - `ActiveWorkbook.Sheets("List").Activate` — Sheets by name + Activate
  - `IBPAutomationObject.Refresh` (SAP)
  - `topLeftCell = IBPAutomationObject.getDataTopLeftCell(ActiveSheet, rptID)`
    (SAP)
  - `Set position = ActiveSheet.Range(topLeftCell)` — Range(string) constructor
  - `combination = IBPAutomationObject.GetAttributeValues(position)` (SAP)
  - `IBPAutomationObject.SetFilterValues(combination, ActiveWorkbook.Sheets("Planning"))`
    (SAP)
  - `ActiveWorkbook.Sheets("Planning").Activate`
  - `On Error GoTo ErrorHandling:`
  - `MsgBox Err.Description, vbOKOnly, "title"`
- `NextItem_Click()` — Uses `position.Offset(1, 0)`, `Range.Is Nothing` check,
  SAP calls, error handling
- `PreviousItem_Click()` — Same pattern with `position.Offset(-1, 0)`
- `SaveAndNext_Click()` — Uses `position Is Nothing` check,
  `IBPAutomationObject.SaveData` (SAP), calls `NextItem_Click` (callback-style),
  `Attribute SaveAndNext_Click.VB_ProcData.VB_Invoke_Func = "S\n14"` (shortcut
  key assignment)

**Potentially useful VBA features:**

- Module-level `Private position As Range` — module variable
- `position Is Nothing` — Nothing check on Range object
- `position.Offset(1, 0)` — Offset property
- Calling one procedure from another (`Call NextItem_Click`)
- `Attribute ...VB_ProcData.VB_Invoke_Func = "S\n14"` — shortcut key metadata
- `Empty` — `If rptID = ""` treats unassigned Variant as empty string

**Potentially useful Excel object-model features:**

- `ActiveWorkbook.Sheets("name")` — Sheets collection by name
- `ActiveSheet.Range(string)` — string-to-Range resolution
- `Worksheet.Activate`
- `Application.COMAddIns` (SAP-specific, exclude)

**Self-contained?** No — SAP-dependent.

**Priority:** P2 — tests `On Error GoTo` with `Err.Description`,
`position Is Nothing`, calling procedures from procedures.

______________________________________________________________________

### WB-07: `1.3_Trigger SAP IBP Actions Using Shortcut Keys/Trigger SAP IBP actions via shortcut keys.xlsm`

**Purpose (PDF):** Assign SAP IBP Simulate to a shortcut key.

**Modules extracted:** Sheet2.cls, Sheet6.cls, ThisWorkbook.cls

**Procedures in Sheet6.cls:**

- `IBPSimulate()` — `Application.COMAddIns(...)`,
  `IBPAutomationObject.Simulate`, error handling. Marked `Public` with
  `Attribute IBPSimulate.VB_Description = "Run Simulate"` and
  `Attribute IBPSimulate.VB_ProcData.VB_Invoke_Func = "S\n14"`

**Potentially useful:** None — purely SAP API calls.

**Priority:** EXCLUDE

______________________________________________________________________

### WB-08: `1.4_Run Simulations Using VBA Buttons/Run Simulations Using VBA Buttons.xlsm`

**Purpose (PDF):** Run a statistical forecast simulation via a button click.

**Modules extracted:** Sheet2.cls, Sheet3.cls, Sheet6.cls, ThisWorkbook.cls

**Procedures in Sheet2.cls:**

- `SimulateForecast_Click()` — `Application.COMAddIns`,
  `IBPAutomationObject.Simulate("My forecast model", "IBPFORECAST")`, error
  handling.

**Potentially useful:** None beyond error handling patterns already seen.

**Priority:** EXCLUDE (SAP API call `Simulate` with string args — could test
argument passing, but minimal value)

______________________________________________________________________

### WB-09: `1.5_Measure the Performance of SAP IBP Actions (2202)/Measure the Performance of SAP IBP Actions.xlsm`

**Purpose (PDF):** Upgrade-test performance harness. Measures Refresh, Simulate,
Simulate(model), SaveData with `Timer`.

**Modules extracted:** Sheet1.cls, Sheet2.cls, Sheet6.cls, ThisWorkbook.cls

**Procedures in Sheet1.cls:**

- `ResetStatus_Click()` — Key procedure (non-SAP portion):
  - `Range("B8:E16").Interior.ColorIndex = 0` — Interior.ColorIndex property
  - `Range("E8:G16").Value = ""` — Range.Value setter with empty string
- `RunUpgradeTest_Click()` — Uses `Timer` (VBA built-in),
  `Dim StartTime As Double`, `SecondsElapsed = Timer - StartTime`,
  `tester.Range("E8").Interior.ColorIndex = 35`,
  `tester.Range("E8").Value = SecondsElapsed`, plus SAP calls

**Potentially useful VBA features:**

- `Timer` — VBA built-in for elapsed time (Double returned)
- `Dim StartTime As Double` — Double type declaration
- `Interior.ColorIndex = 0` (reset to none) / `= 35` (green?)
- `Range.Value = ""` — clearing values
- `ActiveSheet.Activate`

**Potentially useful Excel object-model features:**

- `Range.Interior.ColorIndex` (get/set)
- `Range.Value` (set with string)
- `Range("E8:G16")` — string address
- `Worksheet.Activate`

**Self-contained?** Yes for `ResetStatus_Click` — no SAP dependencies.
`RunUpgradeTest_Click` requires SAP.

**Priority:** P1 — `ResetStatus_Click` is fully self-contained and tests
`Interior.ColorIndex`, `Range.Value`, and `Range` address resolution.

______________________________________________________________________

### WB-10: `1.6_Prefill reason codes and comments when saving data/Prefill Reason Codes and Comments when Saving Data.xlsm`

**Purpose (PDF):** Pre-fill reason codes and comments on SaveData using arrays
and `Right`/`Left`/`Len`.

**Modules extracted:** Module1.bas, Sheet1.cls, Sheet2.cls, Sheet6.cls,
ThisWorkbook.cls

**Procedures in Module1.bas AND Sheet2.cls** (duplicate — same code in both):

- `SaveData_Click()` — Key procedure. Uses:
  - `Dim reasonCodes(1 To 1) As String` — fixed-size 1-based string array
  - `reasonCodes(1) = "Customer Input"` — 1-based array assignment
  - `sheetName = ActiveSheet.Name`
  - `templateName = ActiveWorkbook.Names("SOP_Template_Name")` — NamedRange
    lookup
  - `Right(templateName, Len(templateName) - 2)` — Right + Len with arithmetic
  - `Left(templateName, Len(templateName) - 1)` — Left + Len
  - `text = "..." & templateName & "..." & sheetName & "..."` — String
    concatenation
  - `Call IBPAutomationObject.SaveData(False, reasonCodes, text)` (SAP)
  - Error handling pattern

**Potentially useful VBA features:**

- `Dim arr(1 To 1) As String` — explicit 1-based array
- `ActiveWorkbook.Names("name")` — workbook Names collection
- `Right(string, Len(string) - n)` pattern
- `Left(string, Len(string) - n)` pattern
- `Call subName(arg1, arg2, arg3)` — multi-argument call with `Call` keyword

**Potentially useful Excel object-model features:**

- `ActiveWorkbook.Names("namedrange")` — workbook-level named range

**Self-contained?** Partially — the string manipulation (Right/Left/Len/Names)
can be tested without SAP. The `SaveData` call is SAP-specific.

**Priority:** P2 — `ActiveWorkbook.Names` is interesting; `Right`/`Left`/`Len`
are well-tested but good coverage.

______________________________________________________________________

### WB-11: `1.1_Measure the Performance of SAP IBP Actions (2205)/Measure the Performance of SAP IBP Actions.xlsm`

**Purpose (PDF):** Same as WB-09 but with 2205 APIs (OpenFavorite,
SaveMasterDataSheet).

**Modules extracted:** Sheet1.cls, Sheet6.cls, ThisWorkbook.cls

**Procedures in Sheet1.cls:**

- `ResetStatus_Click()` — Same as WB-09
- `RunUpgradeTest_Click()` — Additional API calls:
  `IBPAutomationObject.OpenFavorite`,
  `IBPAutomationObject.OpenMasterDataFavorite`,
  `IBPAutomationObject.SaveMasterDataSheet(True, True)`

**Potentially useful:** Same as WB-09 for self-contained parts.
`ResetStatus_Click` is identical.

**Priority:** P3 — duplicate of WB-09 `ResetStatus_Click`. Lower priority.

______________________________________________________________________

### WB-12: `1.2_Show Master Data Details/Show Master Data Details.xlsm`

**Purpose (PDF):** Show master data details for a Product ID by double-clicking.

**Modules extracted:** Sheet1.cls, Sheet2.cls, Sheet6.cls, ThisWorkbook.cls,
UserForm1.frm

**Procedures in Sheet2.cls:**

- `Worksheet_BeforeDoubleClick(ByVal Target As Range, Cancel As Boolean)` — Key
  procedure. Uses:
  - `Target.Formula2` — Excel 2010+ Formula2 property
  - `InStr(1, formula, "=@ EPMOlapMemberO(""[PRDID].[].[")` — InStr
  - `Replace(formula, "=@ EPMOlapMemberO(""", "")` — Replace with embedded
    quotes
  - `Left(productID, InStr(2, productID, "]"""))` — Left + InStr (two-argument
    start)
  - `IBPAutomationObject.GetSingleMasterData("Product", Array(productID))` (SAP)
  - `length = UBound(attributes)` — UBound of returned array
  - `ReDim details(length)`
  - `For i = 0 To length`
  - `attributeID = GetID(attributes(i))` — calling custom function
  - `attributeName = GetName(attributeID)` — calling custom function
  - `attributeValue = GetValue(attributes(i))` — calling custom function
  - `details(i) = attributeID + ": " + attributeValue` — String concatenation
    with `+`
  - `MsgBox Join(details, vbNewLine)` — Join with vbNewLine constant
  - Error handling pattern

**Procedures (continued):**

- `GetID(attributeDef As String) As String` —
  `Mid(attributeDef, 2, InStr(attributeDef, "].[].[") - 2)` — Mid with 3 args,
  InStr with 2 args
- `GetValue(attributeDef As String) As String` — `Mid(...)`, `InStr(...)`,
  `Len(...)`, `Left(...)`
- `GetName(attributeID As String) As String` — Key procedure:
  - `Dim position As range` — late-bound type (lowercase `range`)
  - `Set position = Worksheets("IDName").range("A2:A21").Find(attributeID, LookIn:=xlValues)`
    — Find with named arg
  - `Worksheets("IDName").Cells(position.Row, 2).Text` — Cells(row, col) + .Text
    property

**References / dependencies:** SAP IBP COM add-in, `IDName` worksheet

**Potentially useful VBA features:**

- `Target.Formula2` — Excel 2010+ property
- `Mid(string, start, length)` — 3-argument Mid
- `InStr(string, substring)` — 2-argument InStr (default start=1)
- `Worksheets("name").range("A2:A21").Find(...)` — Find with `LookIn:=xlValues`
  named argument
- `Cells(row, col).Text` — Text property of cells
- `Join(array, vbNewLine)` — Join with constant
- String concatenation with `+` vs `&`
- Calling functions from within event handler

**Potentially useful Excel object-model features:**

- `Target.Formula2` property
- `Worksheets("name")` — Worksheets collection by name
- `Range.Find` with `LookIn:=xlValues`
- `Cells(row, col)` constructor
- `.Text` property
- `Worksheets("name").Range("A1:B2")`

**Self-contained?** Partially — the string parsing functions (`GetID`,
`GetValue`, `GetName`) and `Find` usage can be tested independently. The
`Formula2` and `GetSingleMasterData` are SAP-dependent.

**Priority:** P1 — tests `Formula2`, `Find` with `LookIn:=xlValues`, `Mid`,
`Cells(row,col)`, `Worksheets("name")`, `Join` with `vbNewLine`.

______________________________________________________________________

### WB-13: `1.2_Show Master Data Details/Show Master Data Details_WithNames.xlsm`

**Purpose:** Variant of WB-12 with named ranges.

**Modules extracted:** Sheet1.cls, Sheet2.cls, Sheet6.cls, ThisWorkbook.cls,
UserForm1.frm

**Code is identical to WB-12** for the Sheet2.cls procedures. Sheet1.cls is
empty (attributes only).

**Priority:** P3 — duplicate of WB-12.

______________________________________________________________________

### WB-14: `1.3_Change Master Data Attribute Value/Change ABC Code.xlsm`

**Purpose (PDF):** Change ABC code for a Product ID via InputBox on
double-click.

**Modules extracted:** Sheet2.cls, Sheet6.cls, ThisWorkbook.cls

**Procedures in Sheet2.cls:**

- `Worksheet_BeforeDoubleClick(ByVal Target As Range, Cancel As Boolean)` — Key
  procedure. Uses:
  - `Target.Formula2` — Excel 2010+
  - `InStr(1, formula, "=@ EPMOlapMemberO(""[ABCID].[].[")` — InStr with
    embedded quotes
  - `IBPAutomationObject.GetAttributeValues(Target.Offset(0, 4))` (SAP)
  - `Filter(attributes, "[PRDID].[].")` — VBA `Filter` function
  - `UBound(attributes)` — UBound
  - `InputBox("...", "...", ABCcode)` — VBA InputBox function with 3 args
  - `Target.Value2` — Range.Value2 property (Excel 2010+)
  - `StrPtr(newABC) = 0` — StrPtr for detecting empty/cancelled InputBox
  - `If newABC = "A" Or newABC = "B" Or newABC = "C"` — Or operator, string
    comparison
  - `Array("[ABCID].[].[" + newABC + "]")` — Array() with concatenation
  - Error handling pattern

**Potentially useful VBA features:**

- `Filter(array, substring)` — VBA Filter function
- `InputBox(prompt, title, default)` — 3-argument InputBox
- `StrPtr(variable) = 0` — StrPtr for detecting Cancel in InputBox
- `Target.Value2` — Range.Value2 property
- `Target.Offset(0, 4)` — two-arg Offset
- String concatenation with `+`
- `Array(string)` — Array() with runtime-constructed string

**Potentially useful Excel object-model features:**

- `Target.Value2` — Excel 2010+ property
- `Target.Offset(row, col)`
- `Target.Formula2`

**Self-contained?** Partially — `Filter`, `InputBox`, `StrPtr`, `UBound`,
`Array()`, string concatenation can be tested independently. `Formula2`,
`Value2`, `Offset` and SAP calls require context.

**Priority:** P1 — `Value2`, `Formula2`, `Filter`, `InputBox`, `StrPtr`,
`Offset` are key features to test.

______________________________________________________________________

### WB-15: `1.4_Combination-Based Planning with Changing the Status/Combination-Based Planning with Changing the Status.xlsm`

**Purpose (PDF):** Combination-based planning, extended with status change via
UpdateSingleMasterData.

**Modules extracted:** Sheet2.cls, Sheet5.cls, Sheet6.cls, ThisWorkbook.cls

**Procedures in Sheet2.cls:**

- `RefreshList_Click()` — Similar to WB-06
- `NextItem_Click()` — Similar to WB-06
- `PreviousItem_Click()` — Similar to WB-06
- `SaveAndNext_Click()` — Extended: also calls
  `IBPAutomationObject.UpdateSingleMasterData("Location Product", combination, Array("[PLANSTATUS].[].[For Review]"))`

**Module-level variables in Sheet2.cls:**

- `Private IBPAutomationObject As Object`
- `Private position As Range`
- `Private combination() As String` — module-level array

**Potentially useful:** Same as WB-06. Additional: module-level
`Dim combination() As String` (array as module variable).

**Priority:** P3 — near-duplicate of WB-06.

______________________________________________________________________

### WB-16: `Scroll to current period/Scroll to current period.xlsm`

**Purpose (repo):** Scroll the active window to the current time period column
after refresh.

**Modules extracted:** Sheet1.cls, ThisWorkbook.cls, UserForm1.frm,
VBA_subs.bas, SAPIBP_AFTERREFRESH.bas

**Procedures in VBA_subs.bas:**

- `scrollToCurrentPeriod()` — Key procedure. Uses:
  - `Application.COMAddIns(...)` (SAP)
  - `IBPAutomationObject.GetActiveReportName(ActiveSheet)` (SAP)
  - `MsgBox(...)`, `Exit Sub`
  - `IBPAutomationObject.getColumnAxisMembers(ActiveSheet, rptID)` (SAP)
  - `For Each col In colAxisMembers`
  - `InStr(1, col, "PERIOD") > 0` — InStr
  - `Application.Evaluate("=IBPMemberProperty(""" & col & """, ""RELATIVE"")")`
    (SAP custom function)
  - `If relativeProperty = 0 Then ... Exit For`
  - `ActiveSheet.Range(topLeftCell).Column` — Range.Column
  - `ActiveSheet.Range(topLeftCell).Row` — Range.Row
  - `ActiveSheet.Range(bottomRightCell).Column`
  - `ActiveSheet.Range(Cells(topLeftRow, topLeftCol), Cells(topLeftRow, bottomRightCol))`
    — Range(Cells(...), Cells(...)) constructor with variables
  - `.Find(currentPeriod, LookIn:=xlFormulas)` — Find with named argument
  - `If Not c Is Nothing Then` — range object Nothing check
  - `ActiveWindow.ScrollColumn = c.Column` — ActiveWindow.ScrollColumn

**Potentially useful VBA features:**

- `InStr(start, string, substring) > 0` — InStr in conditional
- `For Each col In collection` — iterating over Variant array
- `Exit For` in For Each loop
- `Range.Find(lookin:=xlFormulas)` with named argument
- `Not c Is Nothing` — negated Nothing check
- `ActiveWindow.ScrollColumn = col` — ActiveWindow.ScrollColumn property setter
- `Range(Cells(r1,c1), Cells(r2,c2))` — 2-arg Range constructor with variables

**Potentially useful Excel object-model features:**

- `Range.Find` with `LookIn:=xlFormulas` named argument
- `ActiveWindow.ScrollColumn` setter
- `Range(Cells(...), Cells(...))` constructor
- `Cells(row, col)` with variable arguments

**Self-contained?** No — requires SAP APIs and EPMOlapMemberO formulas. But the
`.Find`, `ScrollColumn`, and `Range(Cells,Cells)` patterns are standard.

**Priority:** P2 — `Range.Find` with named arg, `ActiveWindow.ScrollColumn`,
`Range(Cells(...), Cells(...))` constructor.

______________________________________________________________________

### WB-17: `SAP IBP Hooks/SAP_IBP_VBA_Hook_2005_and_Later_versions.xlam`

**Purpose (PDF):** Demonstrate SAP IBP VBA hooks: IBPBeforeSend,
IBPAfterRefresh, IBPMDAfterRefresh, IBPMDBeforeUpdate.

**Modules extracted:** ThisWorkbook.cls, Sheet1.cls, IbpVbaHookManager.bas

**Procedures in IbpVbaHookManager.bas:**

- `IBPBeforeSend(callMode As String) As Boolean`:

  - `If callMode = "SAVE" Then` — string comparison
  - `Dim answer As Integer`
  - `MsgBox(callMode & vbNewLine & "Some cells..." & vbNewLine & "...", vbYesNo + vbQuestion, "title")`
    — MsgBox with `vbYesNo + vbQuestion` (addition of constants), `vbNewLine`,
    string `&` concatenation, 3-arg MsgBox
  - `If answer = vbYes Then` — constant comparison
  - `IBPBeforeSend = True` / `= False` — function return via assignment
  - `ElseIf callMode = "SIMULATE"` / `"CREATE_SIMULATION"`

- `IBPAfterRefresh()`:

  - `MsgBox("...")` — simple MsgBox

- `IBPMDAfterRefresh(name As String, editable As Boolean, rows As Long, headers() As String)`:

  - `If editable Then addColumn = 1 Else addColumn = 0` — If/Else with boolean
  - `If name = "Product" Then` — string comparison
  - `Application.Match("ABCID", headers, False)` — Application.Match
  - `UBound` not used; `headers(i)` — array indexing
  - `Dim MyList(3) As String` — 0-based fixed-size array
  - `MyList(0) = "A"`, `MyList(1) = "B"`, `MyList(2) = "C"` — 0-based array
  - `Range(Cells(3, idxWhat), Cells(3 + rows + 30, idxWhat)).Validation` —
    Range.Validation
  - `.Delete` — Validation.Delete
  - `.add Type:=xlValidateList, AlertStyle:=xlValidAlertStop, Operator:=xlBetween, Formula1:=Join(MyList, ",")`
    — Validation.Add with named args
  - `work.Font.ColorIndex = 2` — Font.ColorIndex
  - `work.Interior.ColorIndex = 5` — Interior.ColorIndex
  - `Columns(idxWhat).Cut` — Columns.Cut
  - `Columns(idxWhere).Insert Shift:=xlToRight` — Columns.Insert with named arg
  - `If name = "Location" Then` — another branch
  - `Columns(idxWhat).Hidden = True` — Columns.Hidden

- `IBPMDBeforeUpdate(name As String, headers() As String, changedItems() As String) As Boolean`:

  - `UBound(changedItems, 1)` — UBound with dimension argument (2D array)
  - `For i = 0 To changeCount`
  - `productId = changedItems(i, 0)` — 2D array access: `array(row, col)`
  - `If productId Like "*IBP*"` — Like operator with wildcards
  - `Exit Function`
  - `MsgBox(...)`

**References / dependencies:** SAP IBP hooks (called by add-in), named ranges in
cells, `IBPXLClient.Connect` (indirect)

**Potentially useful VBA features:**

- `Like "*IBP*"` — Like operator with `*` wildcards
- `MsgBox(..., vbYesNo + vbQuestion, "title")` — MsgBox with multiple constants
- `vbNewLine` constant
- String concatenation with `&`
- Function return via named assignment (`IBPBeforeSend = True`)
- `If/ElseIf` chains
- `UBound(array, 1)` — UBound with dimension arg
- `changedItems(i, 0)` — 2D Variant array access
- `Exit Function`
- `Dim arr(3) As String` — default 0-based (no Option Base)
- `Application.Match(value, array, matchType)` — Application.Match function
- `Join(MyList, ",")` — Join with comma
- `Range.Validation.Delete` and `.Add` — Validation object
- `xlValidateList`, `xlValidAlertStop`, `xlBetween` — validation constants
- `Font.ColorIndex`, `Interior.ColorIndex` — color properties
- `Columns(col).Cut`, `Columns(col).Insert Shift:=xlToRight` — column operations
- `Columns(col).Hidden = True` — column visibility
- `Integer` type usage

**Potentially useful Excel object-model features:**

- `Application.Match` — NOT found in AOO VBA application implementation
- `Range.Validation.Add` — found in `vbavalidation.cxx`
- `Range.Validation.Delete` — need to check
- `Font.ColorIndex` — found in `vbafont.cxx`
- `Interior.ColorIndex` — found in `vbainterior.cxx`
- `Columns(col).Cut` — Range.Cut found
- `Columns(col).Insert Shift:=xlToRight` — Range.Insert found, need to check
  Shift param
- `Range.Font`, `Range.Interior` — need to check
- `Cells(row, col)` — found
- `Columns.Hidden` — Range.Hidden found
- `Range.Rows.Ungroup` — need to check (Rows.Ungroup)

**Self-contained?** The validation/formatting code in `IBPMDAfterRefresh` can be
reproduced with a synthetic worksheet. `Application.Match` is NOT available in
AOO (confirmed by source search).

**Priority:** P1 — `Application.Match`, `Range.Validation.Add`,
`Font.ColorIndex`, `Interior.ColorIndex`, `Columns().Cut`,
`Columns().Insert Shift:=`, `Like`, 2D arrays, `vbNewLine`,
`vbYesNo + vbQuestion`.

______________________________________________________________________

### WB-18: `SAP IBP Ribbon Extension/1.1/IBPExtension.xlam`

**Purpose (PDF):** Demonstrate all Ribbon XML component types and their VBA
callbacks.

**Modules extracted:** Sheet1.cls, Sheet2.cls, Sheet3.cls, Sheet4.cls,
ThisWorkbook.cls, Module1.bas

**Procedures in Module1.bas (Ribbon callbacks):**

- `ButtonAction()` — Sub that shows MsgBox
- `ButtonEnabled() As Boolean` — Returns True
- `ToggleButtonAction()` — Sub
- `ToggleButtonEnabled() As Boolean` — Returns True
- `ToggleButtonPressed() As Boolean` — Returns False
- `MenuEnabled() As Boolean` — Returns True
- `AnotherButtonAction(buttonName As String)` — Sub with parameter, shows MsgBox
- `AnotherToggleButtonAction()` — Sub
- `AnotherToggleButtonPressed() As Boolean` — Returns False

**Procedures in Sheet1.cls (SAP-dependent button handlers):** All 10 button
click handlers: `GetApi`, `CommandButton1_Click` through `CommandButton9_Click`,
`Connect_Click`, `FilterOnSelectedRange_Click`,
`GetFilterValuesFromThisSheet_Click`, `GetMDTKeyAttributes_Click`,
`ReadCust0001_Click`

**References / dependencies:** SAP IBP COM add-in, RibbonXML.xml

**Potentially useful:** Ribbon callbacks are SAP-specific
(IBPExtension.xlam!FunctionName). The standalone functions (`ButtonAction`,
`ButtonEnabled`, etc.) test basic function signatures and return values.

**Self-contained?** Module1.bas functions are self-contained but trivial (just
return True/False/MsgBox).

**Priority:** P3 — Ribbon callbacks not supported in AOO; standalone functions
are trivial.

______________________________________________________________________

### WB-19: `SAP IBP Ribbon Extension/1.2/IBPExtension.xlam`

**Purpose (PDF):** Add one basic Ribbon function (LogOn).

**Modules extracted:** Sheet1.cls, Sheet2.cls, Sheet3.cls, Sheet4.cls,
ThisWorkbook.cls, Module1.bas

**Procedures in Module1.bas:**

- `LogOn()` — Sub with error handling,
  `IBPAutomationObject.Connect("Productive System")`
- `IsOffline() As Boolean` — `Dim activeConnection As Variant`,
  `UBound(activeConnection) = -1`, returns Boolean

**Potentially useful VBA features:**

- `Dim activeConnection As Variant` — Variant type
- `UBound(activeConnection) = -1` — checking UBound of empty array
- Duplicate `Dim activeConnection As Variant` in Sheet3.cls (compilation error
  in Excel? — actually it's in different scope)

**Priority:** EXCLUDE — SAP-specific.

______________________________________________________________________

### WB-20: `SAP IBP Ribbon Extension/1.3/IBPExtension.xlam`

**Purpose (PDF):** More complex Ribbon with login/favorite navigation.

**Modules extracted:** Sheet1.cls, Sheet2.cls, Sheet3.cls, Sheet4.cls,
ThisWorkbook.cls, Module1.bas

**Procedures in Module1.bas:**

- `LogOn(connectionName As String)` —
  `IBPAutomationObject.Connect(connectionName)`
- `IsOffline() As Boolean` — `UBound(activeConnection) = -1`
- `IsOnline() As Boolean` — `UBound(activeConnection) <> -1`
- `OpenFavorite(favoriteName As String)` —
  `IBPAutomationObject.OpenFavorite(favoriteName)`
- `LogOnAndOpenFavorite(connectionName As String, favoriteName As String)` —
  Multi-arg:
  - `If (IsOffline()) Then` — calling function with parens
  - `currentConnection = Evaluate("=SOP_Connection_Name")` —
    Application.Evaluate with formula string
  - `If (currentConnection <> connectionName) Then` — String comparison with
    `<>`
  - `IBPAutomationObject.Logoff` (SAP)
  - `LogOn(connectionName)` — calling own function with single arg (no parens)
  - `OpenFavorite(favoriteName)` — calling own function

**Potentially useful VBA features:**

- `Evaluate("=SOP_Connection_Name")` — Application.Evaluate with custom formula
- `If (IsOffline()) Then` — function call in condition with parens
- `LogOn(connectionName)` — function call without `Call` keyword, without parens
- `If (currentConnection <> connectionName) Then` — `<>` string comparison
- Multi-line `If ... Then ... Else ... End If` with function calls
- Calling one public function from another

**Potentially useful Excel object-model features:**

- `Application.Evaluate("=formula")`

**Self-contained?** No — SAP calls. But the function-calling patterns and
`Evaluate` usage are standard.

**Priority:** P3 — `Evaluate` is interesting but marked as TODO in AOO source.

______________________________________________________________________

### WB-21: `1.6_Prefill reason codes and comments when saving data/IBPExtension.xlam`

**Purpose:** Ribbon callback for prefill save.

**Modules extracted:** Module1.bas, Sheet1.cls, ThisWorkbook.cls

**Procedures:** `SaveData_Click` (same as WB-10 Module1.bas), basic
Sheet1/ThisWorkbook attributes.

**Priority:** P3 — near-duplicate of WB-10.

______________________________________________________________________

## 3. Cross-sample observations

### Most common VBA patterns across samples

| Feature                                                                | Occurrences                                     |
| ---------------------------------------------------------------------- | ----------------------------------------------- |
| `Application.COMAddIns("IBPXLClient.Connect").Object`                  | All 15 .xlsm workbooks + 4 .xlam add-ins        |
| `On Error GoTo ErrorHandling:`                                         | 14 workbooks                                    |
| `MsgBox Err.Description, vbOKOnly, "Microsoft Excel: Custom VBA code"` | 14 workbooks                                    |
| `IBPAutomationObject.GetActiveReportName(ActiveSheet)`                 | 8 workbooks                                     |
| `Worksheet_BeforeDoubleClick` event                                    | 4 workbooks                                     |
| `Target.Formula2`                                                      | 4 workbooks (2202 navigation, 2205 master data) |
| `Exit Sub` / `Exit Function` / `Exit For`                              | Many workbooks                                  |
| `Dim x As Object` (late binding)                                       | All SAP-dependent modules                       |
| `CreateObject("System.Collections.ArrayList")`                         | WB-01 only                                      |
| `Range.Columns(col)` iteration with `For Each`                         | WB-02, WB-03                                    |
| `WorksheetFunction.CountIf/Index/CountA`                               | WB-03                                           |
| `Range.Rows.Group` / `.Ungroup`                                        | WB-03                                           |
| `Range.Offset(n, m)`                                                   | WB-03, WB-12, WB-14, WB-15                      |
| `Range.EntireRow.Hidden = Boolean`                                     | WB-03, WB-15                                    |
| `Application.Match`                                                    | WB-17 only                                      |
| `Range.Validation.Add`                                                 | WB-17 only                                      |
| `Font.ColorIndex`                                                      | WB-17 only                                      |
| `Interior.ColorIndex`                                                  | WB-09, WB-17                                    |
| `Columns.Hidden = True`                                                | WB-17 only                                      |
| `Columns(col).Cut` / `.Insert Shift:=xlToRight`                        | WB-17 only                                      |
| `Like` operator                                                        | WB-17 only                                      |
| `UBound(array, dimension)`                                             | WB-13, WB-17                                    |
| `2D array access: arr(i, j)`                                           | WB-17                                           |
| `Filter(array, substring)`                                             | WB-14                                           |
| `InputBox(prompt, title, default)`                                     | WB-14                                           |
| `StrPtr(variable) = 0`                                                 | WB-14                                           |
| `vbNewLine`                                                            | WB-01, WB-09, WB-17                             |
| `vbYesNo + vbQuestion`                                                 | WB-17                                           |
| `Timer` (VBA built-in)                                                 | WB-09, WB-11                                    |
| `Join(array, delimiter)`                                               | WB-09, WB-12, WB-17                             |
| `Range.Value = ""`                                                     | WB-09                                           |
| `ActiveWindow.ScrollColumn`                                            | WB-16                                           |
| `Range.Find(LookIn:=xlValues/xlFormulas)`                              | WB-12, WB-16                                    |
| `Cells(row, col)`                                                      | WB-03, WB-12, WB-14, WB-16                      |
| `Range(Cells(...), Cells(...))`                                        | WB-03, WB-12, WB-14, WB-16                      |
| `ActiveWorkbook.Names("name")`                                         | WB-10, WB-20                                    |
| `Mid(s, start, length)`                                                | WB-12                                           |
| `Right(s, Len(s) - n)` / `Left(s, Len(s) - n)`                         | WB-10                                           |
| `ReDim Preserve array(newSize)`                                        | WB-04, WB-05, WB-12                             |

### SAP-specific (non-standard) APIs referenced

All samples use one or more of these SAP IBP add-in APIs:

- `Application.COMAddIns("IBPXLClient.Connect").Object` — COM add-in access
- `GetActiveReportName`, `getDataTopLeftCell`, `getDataBottomRightCell`
- `GetRowAxisDimensionCount`, `GetRowAxisMembers`
- `GetColumnAxisDimensionCount`, `getColumnAxisMembers`
- `getSheetOption`, `setSheetOption`, `getActiveConnection`
- `GetMemberDimension`, `GetShift`, `GetDimensionList`, `EnableLocalMember`
- `SwitchToIbpTab`, `IBPUser()` (via Evaluate)
- `GetAttributeValues`, `GetFilterValues`, `SetFilterValues`
- `Refresh`, `Simulate`, `SaveData`, `SaveMasterDataSheet`
- `OpenFavorite`, `OpenMasterDataFavorite`, `Connect`, `Logoff`
- `GetConnections`, `GetMasterDataTypeKeyAttributes`
- `GetSingleMasterData`, `UpdateSingleMasterData`
- Custom formulas: `=IBPUser()`, `=IBPMemberProperty(...)`,
  `=SOP_Connection_Name`, `=SOP_Template_Name`

### Ribbon XML callbacks

Samples WB-18 through WB-21 define public functions called from RibbonXML.xml:

- `ButtonAction()`, `ButtonEnabled() As Boolean`
- `ToggleButtonAction()`, `ToggleButtonEnabled() As Boolean`,
  `ToggleButtonPressed() As Boolean`
- `MenuEnabled() As Boolean`
- `AnotherButtonAction(buttonName As String)`
- `AnotherToggleButtonAction()`, `AnotherToggleButtonPressed() As Boolean`
- `LogOn(connectionName As String)`, `IsOffline() As Boolean`,
  `IsOnline() As Boolean`
- `OpenFavorite(favoriteName As String)`
- `LogOnAndOpenFavorite(connectionName As String, favoriteName As String)`

### Files that could not be extracted

All 20 VBA projects were successfully extracted. No extraction failures.

### UserForm files

All UserForm files (`*.frm`) contained only attribute headers and one empty
event handler (`Private Sub Label1_Click()`). No meaningful code in forms.

______________________________________________________________________

## 4. Sample prioritization summary

| Priority    | Samples                                                                                                                                                                       | Rationale                                                                                                     |
| ----------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------- |
| **P1**      | WB-02 (column width), WB-03 (key figure grouping), WB-09 (performance ResetStatus), WB-12 (master data details), WB-14 (change ABC code), WB-17 (IBP hooks MD)                | Standard VBA/Excel features, self-contained or partially reproducible, exercises unsupported/missing features |
| **P2**      | WB-04, WB-05 (navigation), WB-06, WB-15 (combination planning), WB-16 (scroll)                                                                                                | Standard VBA features but deeply intertwined with SAP APIs for full reproduction                              |
| **P3**      | WB-01 (getting started), WB-07 (shortcut keys), WB-08 (buttons), WB-10 (prefill), WB-11 (performance 2205), WB-13 (WithNames), WB-18, WB-19, WB-20, WB-21 (ribbon extensions) | SAP-specific, trivial VBA, or near-duplicates                                                                 |
| **EXCLUDE** | WB-07, WB-08                                                                                                                                                                  | Pure SAP API calls with no independent VBA behavior                                                           |
