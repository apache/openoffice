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

# SAP IBP VBA Samples — Audit Summary

## Repository audited

- **Repository:**
  `SAP-samples/integrated-business-planning-excel-addin-vba-samples`
- **Commit SHA:** `904213f33ea292793a245f3bca3c1c3d36fdc141`
- **Commit date:** 2026-06-17
- **License:** Apache-2.0 (SPDX: `Apache-2.0`)
- **Cloned to:** `/tmp/sap-ibp-vba-samples` (not part of AOO source tree)

______________________________________________________________________

## Files inspected

| Category                                             | Count                                              |
| ---------------------------------------------------- | -------------------------------------------------- |
| `.xlsm` workbooks                                    | 15                                                 |
| `.xlam` add-ins                                      | 6 (5 unique add-in templates + 1 duplicate in 1.6) |
| `.bas`/`.cls`/`.frm` source files (standalone)       | 0 (all embedded in workbooks/add-ins)              |
| PDF documentation                                    | 15                                                 |
| `README.md`                                          | 1                                                  |
| `LICENSE` / `LICENSES/Apache-2.0.txt` / `REUSE.toml` | 3                                                  |
| `.mp4` video tutorials                               | 3                                                  |
| `.xml` Ribbon XML files                              | 4                                                  |
| **Total tracked files**                              | 47 (excluding `.git/`)                             |

______________________________________________________________________

## VBA projects extracted

- **VBA projects attempted:** 21 (all `.xlsm` and `.xlam` files)
- **VBA projects successfully extracted:** 21 (100%)
- **Projects with zero VBA modules:** 0
- **Total VBA modules extracted:** 84
- **Total VBA source lines (approximate):** ~195,000 characters across all
  modules

Extraction was performed using `oletools.olevba` (Python package `oletools`
0.60.2). Each workbook's `xl/vbaProject.bin` was parsed and all VBA modules
(Module, Class, UserForm) were extracted to `/tmp/sap_vba_extracted/` for source
analysis. No extraction errors were encountered.

**Limitation:** VBA project references (COM library GUIDs) were not extracted by
`olevba.extract_all_macros()` in this version. This does not affect the feature
analysis since all SAP dependencies are accessed via late-bound `Object`
variables (`Application.COMAddIns(...).Object`) and `CreateObject`.

______________________________________________________________________

## Most useful samples (by priority)

### P1 — Highest value for AOO compatibility testing

1. **WB-03: Key figure grouping** — Exercises `WorksheetFunction.CountIf`,
   `WorksheetFunction.Index`, `Rows.Group`, `Rows.Ungroup`, `Columns.Hidden`,
   `Columns.Cut`, `Columns.Insert Shift:=xlToRight`, `Range.Offset`,
   `Range.EntireRow.Hidden`, `ReDim`, `Range.UsedRange`, `Outline.ShowLevels`,
   `On Error Resume Next`. The SAP portions (`getDataTopLeftCell`,
   `GetActiveReportName`) can be stubbed; the rest is pure Excel OM.

1. **WB-17: SAP IBP Hooks** — Exercises `Application.Match`,
   `Range.Validation.Add` with named args, `Font.ColorIndex`,
   `Interior.ColorIndex`, `Columns(col).Cut`,
   `Columns(col).Insert Shift:=xlToRight`, `Like` operator, 2D array access
   (`arr(i, 0)`), `UBound(array, 1)`, `vbYesNo + vbQuestion` MsgBox constants,
   `vbNewLine`, `Join(array, ",")`. The hook functions themselves are
   SAP-specific, but the body of `IBPMDAfterRefresh` contains rich Excel OM
   usage.

1. **WB-02: Automatically reducing column width** — Exercises
   `For Each col In ActiveSheet.Columns`, `col.Column`, `col.ColumnWidth = 1`,
   `Range.Column`. Fully self-contained except for the initial SAP
   `getDataTopLeftCell` call.

1. **WB-12: Show Master Data Details** — Exercises `Range.Formula2`,
   `Range.Find(LookIn:=xlValues)`, `Cells(row, col).Text`, `Range.Value`, string
   functions (`Mid`, `InStr`, `Replace`, `Left`, `Len`),
   `Join(array, vbNewLine)`, `Worksheets("name")`, `Worksheet_BeforeDoubleClick`
   event. The string-parsing helper functions (`GetID`, `GetValue`, `GetName`)
   are fully self-contained.

1. **WB-14: Change ABC Code** — Exercises `Range.Value2`, `Range.Formula2`,
   `Range.Offset(0, 4)`, `Filter(array, substring)`,
   `InputBox(prompt, title, default)`, `StrPtr` function, `InStr`, `Replace`,
   `Array()` construction, `Like` comparison, `Or` operator.

1. **WB-16: Scroll to current period** — Exercises `For Each col In collection`,
   `InStr(1, str, substr) > 0`, `Range.Find(LookIn:=xlFormulas)`,
   `ActiveWindow.ScrollColumn`, `Range(Cells(...), Cells(...))` constructor,
   `Not c Is Nothing`.

### P2 — Secondary value

- **WB-09/WB-11: Performance measurement** — `ResetStatus_Click()` is
  self-contained: tests `Interior.ColorIndex = 0`, `Range.Value = ""`. Also
  tests `Timer` and `Worksheet.Activate`.
- **WB-10: Prefill reason codes** — Tests `ActiveWorkbook.Names("name")`,
  `Right(Len-n)`, `Left(Len-n)`, string `&` concatenation, 3-arg `Call` with
  array parameter.
- **WB-04/WB-05: Navigation** — Tests `ReDim Preserve` with Variant arrays,
  `On Error GoTo` with multiple named handlers, `GoTo` label.
- **WB-06/WB-15: Combination planning** — Tests module-level `Private`
  variables, `position Is Nothing` check, calling one procedure from another,
  `Attribute VB_ProcData.VB_Invoke_Func` shortcut metadata.

### P3 / EXCLUDE

- **WB-01: Getting started** — Pure SAP API demonstration. Only
  `CreateObject("System.Collections.ArrayList")` and
  `Application.Evaluate("=IBPUser()")` are non-SAP features, both of which are
  external/COM.
- **WB-07: Shortcut keys** — Only SAP `Simulate` call.
- **WB-08: VBA buttons** — Only SAP `Simulate(model, type)` call.
- **WB-13: Master data WithNames** — Near-duplicate of WB-12.
- **WB-18–21: Ribbon extensions** — Ribbon XML callbacks are SAP-specific.
  Standalone callback functions (`ButtonEnabled`, etc.) are trivial.

______________________________________________________________________

## Most promising AOO-native test cases

1. **`TestColumnWidthGetSet`** — A-1: Set `Range.ColumnWidth = 15`, read back.
   Tests a supported property.

1. **`TestValue2`** — C-1: Read `Range.Value2` after setting `Range.Value`.
   **Expected to FAIL** — `Value2` is not implemented in AOO. This is a
   confirmed compatibility gap.

1. **`TestFormula2`** — C-2: Read `Range.Formula2` after setting a formula.
   **Expected to FAIL** — `Formula2` is not implemented. Confirmed gap.

1. **`TestApplicationMatch`** — D-6: Call `Application.Match(value, range, 0)`.
   **Expected to FAIL** — `Application.Match` is not implemented. Confirmed gap.

1. **`TestFilter`** — D-2: `Filter(array, substring)` then check `UBound`. Tests
   standard VBA `Filter` function.

1. **`TestLikeWildcard`** — D-3: `"string" Like "*pattern*"`. Tests standard VBA
   `Like` operator — supported.

1. **`TestRangeFind`** — C-3: `Range.Find("target", LookIn:=xlValues)`. Tests
   Find with named argument — supported.

1. **`TestValidationAdd`** — C-8: `Range.Validation.Add` with named arguments.
   Tests validation API — supported (partial).

1. **`TestStrPtr`** — F-2: `StrPtr("test")`. **Expected to FAIL** — `StrPtr` not
   implemented. Confirmed gap.

1. **`TestErrorHandling`** — D-5: `On Error GoTo` with `Err.Description`. Tests
   error handling — supported.

1. **`TestReDimPreserve`** — B-4: `ReDim Preserve` with Variant array
   concatenation. Tests array handling — supported.

1. **`TestColumnsHidden`** — A-4: `Columns.Hidden = True` then read back. Tests
   column visibility — supported.

______________________________________________________________________

## Confirmed compatibility gaps

| ID   | Feature                                     | Source file            | AOO status                           | Evidence                                                                                                                                             |
| ---- | ------------------------------------------- | ---------------------- | ------------------------------------ | ---------------------------------------------------------------------------------------------------------------------------------------------------- |
| G-01 | `Range.Value2`                              | vbarange.cxx           | NOT implemented                      | `grep "Value2" main/sc/source/ui/vba/vbarange.cxx` → 0 matches; `getValue2`/`setValue2` not present in header                                        |
| G-02 | `Range.Formula2`                            | vbarange.cxx           | NOT implemented                      | `grep "Formula2" main/sc/source/ui/vba/vbarange.cxx` → 0 matches                                                                                     |
| G-03 | `Application.Match`                         | vbaapplication.cxx     | NOT implemented                      | `grep -n "Match" main/sc/source/ui/vba/vbaapplication.cxx` → 0 matches                                                                               |
| G-04 | `Application.Evaluate` (formula evaluation) | vbaapplication.cxx     | Partial                              | Implementation (line 368) only handles range-name lookup; `// #TODO Evaluate allows other things to be evaluated, e.g. functions`                    |
| G-05 | `StrPtr` function                           | sbcomp.cxx, stdobj.cxx | NOT implemented                      | `grep -rn "StrPtr" main/basic/` → 0 matches in VBA-relevant files                                                                                    |
| G-06 | `Range.Text` property                       | vbarange.cxx           | Partial                              | No explicit `getText`/`Text` property on XRange interface in AOO                                                                                     |
| G-07 | Ribbon XML support                          | Entire codebase        | NOT implemented                      | No Ribbon XML parsing in AOO VBA; Ribbon is handled via UNO command infrastructure                                                                   |
| G-08 | `COMAddIns`                                 | vbaapplication.cxx     | NOT implemented                      | `grep -rn "COMAddIns" main/sc/source/ui/vba/` → 0 matches                                                                                            |
| G-09 | ActiveX controls                            | msforms/               | Partially supported (UserForms only) | Samples use `MSForms.TextBox` etc. via `VB_Control` attributes; AOO has MSForms replacement but may not fully support embedded ActiveX in worksheets |
| G-10 | `WorksheetFunction.CountIf`                 | vbawsfunction.cxx      | Partial                              | Delegates to `FunctionAccess.invoke` — CountIf may or may not work depending on FunctionAccess support                                               |
| G-11 | `Range.UsedRange`                           | vbarange.cxx           | Need to verify                       | Not found in grep; may not be implemented                                                                                                            |
| G-12 | `Outline.ShowLevels`                        | vbaworksheet.cxx       | Need to verify                       | Not found in grep                                                                                                                                    |

______________________________________________________________________

## SAP-specific features to exclude

All of the following features are specific to the SAP IBP Excel add-in and
**cannot** be tested or supported in AOO without introducing an SAP dependency:

1. `Application.COMAddIns("IBPXLClient.Connect").Object` — SAP IBP COM add-in
   access
1. All IBP automation object methods: `GetActiveReportName`,
   `getDataTopLeftCell`, `getDataBottomRightCell`, `getRowAxisDimensionCount`,
   `getRowAxisMembers`, `getColumnAxisDimensionCount`, `getColumnAxisMembers`,
   `setSheetOption`, `getSheetOption`, `getActiveConnection`,
   `getMemberDimension`, `getShift`, `GetDimensionList`, `EnableLocalMember`,
   `SwitchToIbpTab`
1. `GetAttributeValues`, `GetFilterValues`, `SetFilterValues`,
   `GetSingleMasterData`, `UpdateSingleMasterData`,
   `GetMasterDataTypeKeyAttributes`
1. `Refresh`, `Simulate`, `Simulate(model, type)`, `SaveData`,
   `SaveMasterDataSheet`
1. `OpenFavorite`, `OpenMasterDataFavorite`, `Connect`, `Logoff`,
   `GetConnections`
1. Custom IBP formulas: `=IBPUser()`, `=IBPMemberProperty(...)`,
   `=SOP_Connection_Name`, `=SOP_Template_Name`
1. EPM formulas: `=EPMOlapMemberO(...)` (used in `Target.Formula2` string
   parsing)
1. SAP IBP Hooks: `IBPBeforeSend`, `IBPAfterRefresh`, `IBPMDAfterRefresh`,
   `IBPMDBeforeUpdate`
1. Ribbon XML callback dispatch (`IBPExtension.xlam!FunctionName`)
1. Named ranges `SOP_Template_Name`, `KFGroups`, `KFHeader`, `Auto_ReGroup`
   (workbook-specific data)
1. ActiveX control attributes (`Attribute VB_Control = "..."`)
1. `CreateObject("System.Collections.ArrayList")` — .NET COM object

______________________________________________________________________

## Licensing and provenance notes

- **Repository license:** Apache License 2.0. No separate license file per file;
  REUSE.toml applies `SPDX-License-Identifier = "Apache-2.0"` to all files.
- **Extracted VBA source:** Stored in `/tmp/sap_vba_extracted/` (outside the AOO
  source tree). Not committed to AOO.
- **No SAP code copied:** No SAP VBA, PDF text, or workbook files have been
  placed in the AOO repository. All analysis was performed in `/tmp/` from a
  cloned copy.
- **Test code origin:** Proposed tests are independently authored, following
  AOO's existing test conventions (`VbaCompatTest` base class, `'@TestMethod`
  markers). They reference SAP samples only as *inspiration* for standard
  VBA/Excel behavior, not as copied code.
- **Test data:** Proposed tests use synthetic Calc documents with
  `Option VBASupport 1`. No SAP workbooks or templates are used as test data.

______________________________________________________________________

## Recommended next steps, ranked by value and implementation effort

### 1. Implement `Range.Value2` property (Gap G-01)

- **Value:** High — `Value2` is used in WB-14 and is a commonly needed Excel OM
  property.
- **Effort:** Low — `Value2` behaves identically to `Value` for non-date cells.
  Implementation likely involves adding `getValue2`/`setValue2` to `ScVbaRange`
  that delegates to existing value getters/setters without date coercion.
- **Files to modify:** `main/sc/source/ui/vba/vbarange.cxx`,
  `main/sc/source/ui/vba/vbarange.hxx`, `main/sc/source/ui/vba/vbarange.idl` (if
  IDL exists)
- **Validation:** Test C-1 should pass after implementation.

### 2. Implement `Range.Formula2` property (Gap G-02)

- **Value:** Medium — `Formula2` is used in WB-04, WB-05, WB-12, WB-14. It is
  the Excel 2010+ formula representation (returns a `Formula` object rather than
  string, but in VBA context it behaves like `Value`/`Formula` for simple
  cases).
- **Effort:** Medium — need to add `getFormula2`/`setFormula2` to the range
  implementation. For simple cases, it can delegate to
  `getFormula`/`setFormula`.
- **Files to modify:** `main/sc/source/ui/vba/vbarange.cxx`, `vbarange.hxx`
- **Validation:** Test C-2 should pass after implementation.

### 3. Implement `Application.Match` function (Gap G-03)

- **Value:** Medium — `Application.Match` is used in WB-17. It's a standard
  Excel worksheet function.
- **Effort:** Medium — AOO already has a `WorksheetFunction` dispatch mechanism
  via `FunctionAccess` (`vbawsfunction.cxx`). `Match` may be addable to the
  `invoke` method or as a dedicated method on `ScVbaApplication`.
- **Files to modify:** `main/sc/source/ui/vba/vbaapplication.cxx`, possibly
  `vbawsfunction.cxx`
- **Validation:** Test D-6 should pass after implementation.

### 4. Write and integrate the core test suite (Tests A-1, A-3, A-4, C-3, C-4, C-5, C-7, C-9, D-1, D-2, D-3, D-5)

- **Value:** High — establishes baseline coverage for supported features.
- **Effort:** Low-Medium — follow existing `VbaLibre*Test` pattern, extend
  `JunitTest_sc_vba.mk` to include new test classes.
- **Files to create:** New Java test files in `main/sc/qa/vba/src/`
- **Files to modify:** `main/sc/JunitTest_sc_vba.mk` (add new test classes to
  `_add_sourcefiles` and `_add_classes`)

### 5. Investigate and fix `Range.UsedRange` and `Outline.ShowLevels` (Gaps G-11, G-12)

- **Value:** Medium — used in WB-03.
- **Effort:** Medium-High — may require new UNO API calls or VBA implementation
  additions.
- **Files to modify:** `main/sc/source/ui/vba/vbarange.cxx`, `vbaworksheet.cxx`

### 6. Investigate `StrPtr` function (Gap G-05)

- **Value:** Low — only used in WB-14 for InputBox cancel detection. Alternative
  patterns (empty-string check) exist.
- **Effort:** Medium — requires BASIC runtime addition.
- **Files to modify:** `main/basic/source/runtime/`

### 7. Add exploratory tests for interactive/environment-dependent features (F-1, G-1)

- **Value:** Low — useful for documentation but cannot run in CI.
- **Effort:** Low — add as `@Ignore` tests or separate exploratory class.

### 8. Do NOT implement: COMAddIns, Ribbon XML, SAP APIs, ActiveX controls

- **Rationale:** These are external dependencies or integration features outside
  AOO's scope. Adding them would introduce SAP/Excel add-in dependencies,
  violating constraints.
