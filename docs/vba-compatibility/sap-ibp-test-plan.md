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

# SAP IBP VBA Samples — AOO Native Test Plan

This document proposes independent Apache OpenOffice (AOO) native tests derived
from the SAP IBP VBA samples inventory and feature matrix. Each test is
self-contained, deterministic, and does not require the SAP IBP add-in, an SAP
tenant, or any external COM dependency.

Tests follow the existing AOO VBA test infrastructure conventions:

- Java JUnit classes extending `VbaCompatTest` (in `main/sc/qa/vba/src/`)
- BASIC source embedded as string constants with `'@TestModule` / `'@TestMethod`
  markers
- Results written to cell columns Z (index 25, numeric) and AA (index 26,
  string)
- Assertions verified in Java via UNO `XCell.getValue()` / `XCell.getString()`

______________________________________________________________________

## Test Group A: Basic worksheet automation (column width)

**Source:** WB-02 "Automatically reducing the width of empty columns"

The SAP sample sets `ColumnWidth = 1` on empty columns. This tests the pure
Excel OM portion: iterating columns, reading `Column` property, and setting
`ColumnWidth`.

### Test A-1: ColumnWidth getter/setter on a Range

| Item                | Value                                                                                                                     |
| ------------------- | ------------------------------------------------------------------------------------------------------------------------- |
| **Setup**           | New Calc doc, VBA mode. Set `ActiveSheet.Range("A1").ColumnWidth = 15`, then `Range("B1").EntireColumn.ColumnWidth = 25`. |
| **VBA code**        | `Range("A1").ColumnWidth = 15 : Range("A1").Value = Range("A1").ColumnWidth` (write result to Z1)                         |
| **Expected result** | Z1 = 15.0                                                                                                                 |
| **AOO mechanism**   | `addBasicModule` + `runMacro` + `getNumericResult(0)`                                                                     |
| **Unattended?**     | Yes                                                                                                                       |

### Test A-2: ColumnWidth default and reset

| Item                | Value                                                                                                  |
| ------------------- | ------------------------------------------------------------------------------------------------------ |
| **Setup**           | New doc. Read default `ColumnWidth` of a range, then set 0, then set 15.                               |
| **VBA code**        | `Range("B1").Value = CDbl(Range("A1").ColumnWidth)`; set `ColumnWidth = 0`; read again                 |
| **Expected result** | Z1 = default width (≈ 10.0 or similar). Then `Range("C1").Value = CDbl(Range("A1").ColumnWidth)` = 0.0 |
| **AOO mechanism**   | Same as A-1                                                                                            |
| **Unattended?**     | Yes                                                                                                    |

### Test A-3: Iterating ActiveSheet.Columns and setting ColumnWidth

| Item                | Value                                                                                                                                                                                                                                 |
| ------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Setup**           | Populate columns A–D with values. Iterate `For Each col In ActiveSheet.Columns`, set `ColumnWidth = 1` where col has no data.                                                                                                         |
| **VBA code**        | `Option VBASupport 1\nSub TestColumnIteration()\n  Dim c As Object\n  Dim count As Integer: count = 0\n  For Each c In ActiveSheet.Columns\n    If c.Column > 3 Then count = count + 1\n  Next\n  Range("Z1").Value = count\nEnd Sub` |
| **Expected result** | Z1 = 1 (only column 4 exceeds threshold)                                                                                                                                                                                              |
| **AOO mechanism**   | Same                                                                                                                                                                                                                                  |
| **Unattended?**     | Yes                                                                                                                                                                                                                                   |

### Test A-4: EntireColumn.Hidden

| Item                | Value                                                                                         |
| ------------------- | --------------------------------------------------------------------------------------------- |
| **Setup**           | Set `Columns("B").Hidden = True`. Check `Columns("B").Hidden` returns True. Then set `False`. |
| **VBA code**        | `Columns("B").Hidden = True : Range("Z1").Value = CInt(Columns("B").Hidden)`                  |
| **Expected result** | Z1 = -1 (True in VBA is -1)                                                                   |
| **AOO mechanism**   | Same                                                                                          |
| **Unattended?**     | Yes                                                                                           |

______________________________________________________________________

## Test Group B: VBA callback behavior (inter-procedure calls)

**Source:** WB-17 "SAP IBP Hooks", WB-18–20 Ribbon extensions, WB-04
"MergeArray"

Tests that public functions can be called from within other procedures, argument
passing works, and return values propagate correctly.

### Test B-1: Public function call from within another procedure

| Item                | Value                                                                                                     |
| ------------------- | --------------------------------------------------------------------------------------------------------- |
| **Setup**           | Define `Function MyHelper(x As Integer) As Integer: MyHelper = x * 2 : End Function`. Call it from a Sub. |
| **VBA code**        | `Sub TestCall()\n  Dim result As Integer\n  result = MyHelper(21)\n  Range("Z1").Value = result\nEnd Sub` |
| **Expected result** | Z1 = 42                                                                                                   |
| **AOO mechanism**   | Same                                                                                                      |
| **Unattended?**     | Yes                                                                                                       |

### Test B-2: Public function returning Boolean

| Item                | Value                                                                                                                                                                                                                                    |
| ------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Setup**           | Define `Function IsNotEmpty(r As Object) As Boolean`. Check `Range("A1").Value`.                                                                                                                                                         |
| **VBA code**        | `Function IsNotEmpty(r As Object) As Boolean\n  If IsEmpty(r.Value) Then IsNotEmpty = False Else IsNotEmpty = True\nEnd Function\nSub TestBool()\n  Range("A1").Value = 0\n  Range("Z1").Value = CInt(IsNotEmpty(Range("A1")))\nEnd Sub` |
| **Expected result** | Z1 = 0 (IsNotEmpty returns False for empty/zero cell → CInt(False) = 0). **Note:** `IsEmpty` behavior on `0` may differ — this is the hypothesis.                                                                                        |
| **AOO mechanism**   | Same                                                                                                                                                                                                                                     |
| **Unattended?**     | Yes                                                                                                                                                                                                                                      |

### Test B-3: Module-level variable persistence between calls

| Item                | Value                                                                                                                                                |
| ------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Setup**           | Module-level `Private counter As Integer`. Sub A increments it, Sub B reads it. Call A then B.                                                       |
| **VBA code**        | `Option Explicit\nDim counter As Integer\nSub IncCounter(): counter = counter + 1: End Sub\nSub ReadCounter(): Range("Z1").Value = counter: End Sub` |
| **Expected result** | After calling `IncCounter` then `ReadCounter`: Z1 = 1                                                                                                |
| **AOO mechanism**   | `runMacro(module, "IncCounter")` then `runMacro(module, "ReadCounter")`                                                                              |
| **Unattended?**     | Yes                                                                                                                                                  |

### Test B-4: ReDim Preserve with array return from function

| Item                       | Value                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 |
| -------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Setup**                  | `Function MergeArrays(a As Variant, b As Variant) As Variant` — concatenates two arrays using `ReDim Preserve`.                                                                                                                                                                                                                                                                                                                                                                                                                                                       |
| **VBA code**               | `Function MergeArrays(a As Variant, b As Variant) As Variant\n  Dim result As Variant\n  Dim s1 As Integer, s2 As Integer\n  s1 = UBound(a) - LBound(a) + 1\n  s2 = UBound(b) - LBound(b) + 1\n  result = a\n  ReDim Preserve result(s1 + s2 - 1)\n  Dim i As Integer\n  For i = 0 To s2 - 1\n    result(s1 + i) = b(i)\n  Next i\n  MergeArrays = result\nEnd Function\nSub TestMerge()\n  Dim a As Variant, b As Variant, r As Variant\n  a = Array("x", "y")\n  b = Array("z")\n  r = MergeArrays(a, b)\n  Range("Z1").Value = UBound(r) - LBound(r) + 1\nEnd Sub` |
| **Expected result**        | Z1 = 3 (merged array has 3 elements)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  |
| **AOO mechanism**          | Same                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  |
| **Unattended?**            | Yes                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   |
| **Failure classification** | Excel behavior — `ReDim Preserve` of a Variant array passed `ByVal` may fail in AOO if array passing semantics differ                                                                                                                                                                                                                                                                                                                                                                                                                                                 |

______________________________________________________________________

## Test Group C: Range and worksheet operations

**Source:** WB-12 (Find/Text), WB-14 (Value2, Offset), WB-17 (Validation,
ColorIndex, Insert, Cut)

### Test C-1: Range.Value2 (gap investigation)

| Item                       | Value                                                                                           |
| -------------------------- | ----------------------------------------------------------------------------------------------- |
| **Setup**                  | Set `Range("A1").Value = 42`. Read via `Range("A1").Value2`.                                    |
| **VBA code**               | `Sub TestValue2()\n  Range("A1").Value = 42\n  Range("Z1").Value = Range("A1").Value2\nEnd Sub` |
| **Expected result**        | Z1 = 42.0                                                                                       |
| **AOO status**             | Value2 NOT implemented (vbavalidation.cxx gap)                                                  |
| **AOO mechanism**          | Same                                                                                            |
| **Unattended?**            | Yes                                                                                             |
| **Failure classification** | Confirmed defect — `Value2` property missing                                                    |

### Test C-2: Range.Formula2 (gap investigation)

| Item                       | Value                                                                                                                                                                                |
| -------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| **Setup**                  | Set `Range("A1").Value = 10`. Set `Range("B1").Formula = "=A1*2"`. Read `Range("B1").Formula2`.                                                                                      |
| **VBA code**               | `Sub TestFormula2()\n  Range("A1").Value = 10\n  Range("B1").Formula = "=A1*2"\n  Range("Z1").Value = CDbl(Range("B1").Value)\n  Range("AA1").Value = Range("B1").Formula2\nEnd Sub` |
| **Expected result**        | Z1 = 20, AA1 = "=A1\*2"                                                                                                                                                              |
| **AOO status**             | Formula2 NOT implemented                                                                                                                                                             |
| **AOO mechanism**          | Same                                                                                                                                                                                 |
| **Unattended?**            | Yes                                                                                                                                                                                  |
| **Failure classification** | Confirmed defect — `Formula2` property missing                                                                                                                                       |

### Test C-3: Range.Find with LookIn:=xlValues

| Item                       | Value                                                                                                                                                                                                                                                                                                   |
| -------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Setup**                  | Populate A1:A10 with strings, one of them "Target". Use `Range("A1:A10").Find("Target", LookIn:=xlValues)`.                                                                                                                                                                                             |
| **VBA code**               | `Sub TestFind()\n  Dim i As Integer\n  For i = 1 To 10: Range("A" & i).Value = "item" & i: Next i\n  Range("A5").Value = "Target"\n  Dim c As Object\n  Set c = Range("A1:A10").Find("Target", LookIn:=xlValues)\n  If c Is Nothing Then Range("Z1").Value = 0 Else Range("Z1").Value = c.Row\nEnd Sub` |
| **Expected result**        | Z1 = 5.0                                                                                                                                                                                                                                                                                                |
| **AOO mechanism**          | Same                                                                                                                                                                                                                                                                                                    |
| **Unattended?**            | Yes                                                                                                                                                                                                                                                                                                     |
| **Failure classification** | Excel OM behavior — may fail if `LookIn` named arg not supported                                                                                                                                                                                                                                        |

### Test C-4: Range.Offset(row, col)

| Item                | Value                                                                                                                                                             |
| ------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Setup**           | Set A1=1, B1=2. `Set r = Range("A1")`. `Range("Z1").Value = r.Offset(0, 1).Value`.                                                                                |
| **VBA code**        | `Sub TestOffset()\n  Range("A1").Value = 1\n  Range("B1").Value = 2\n  Dim r As Object: Set r = Range("A1")\n  Range("Z1").Value = r.Offset(0, 1).Value\nEnd Sub` |
| **Expected result** | Z1 = 2.0                                                                                                                                                          |
| **AOO mechanism**   | Same                                                                                                                                                              |
| **Unattended?**     | Yes                                                                                                                                                               |

### Test C-5: Range.Offset + EntireRow.Hidden

| Item                       | Value                                                                                                                                           |
| -------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------- |
| **Setup**                  | Create data in rows 1–5. Set `Range("A1").Offset(3, 0).EntireRow.Hidden = True`. Check hidden.                                                  |
| **VBA code**               | `Sub TestOffsetHidden()\n  Range("A1").Offset(3, 0).EntireRow.Hidden = True\n  Range("Z1").Value = CInt(Range("A4").EntireRow.Hidden)\nEnd Sub` |
| **Expected result**        | Z1 = -1 (True)                                                                                                                                  |
| **AOO mechanism**          | Same                                                                                                                                            |
| **Unattended?**            | Yes                                                                                                                                             |
| **Failure classification** | Excel OM — chaining Offset→EntireRow→Hidden may fail if one link is unsupported                                                                 |

### Test C-6: Columns(col).Cut and Columns(col).Insert

| Item                       | Value                                                                                                                                                                                |
| -------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| **Setup**                  | Put value in A1. Cut column A. Insert at column C with `Shift:=xlToRight`. Verify content moved.                                                                                     |
| **VBA code**               | `Sub TestCutInsert()\n  Range("A1").Value = "original"\n  Columns("A").Cut\n  Columns("C").Insert Shift:=-4162\n  Range("Z1").Value = CDbl(Columns("C").Cells(1, 1).Value)\nEnd Sub` |
| **Expected result**        | Z1 = "original" (as string — may need string test)                                                                                                                                   |
| **AOO mechanism**          | Same, read as string: `Range("C1").Value`                                                                                                                                            |
| **Unattended?**            | Yes                                                                                                                                                                                  |
| **Failure classification** | Excel OM — `Columns(col)` and `Insert` with Shift parameter                                                                                                                          |

### Test C-7: Range.Rows.Group / Ungroup

| Item                       | Value                                                                                                                                                                                    |
| -------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Setup**                  | Put data in rows 3–8. `Range("A3:A8").Rows.Group`. Check outline level. `Range("A3:A8").Rows.Ungroup`.                                                                                   |
| **VBA code**               | `Sub TestGroup()\n  Range("A3:A8").Rows.Group\n  Range("Z1").Value = CDbl(ActiveSheet.Range("A3:A8").Rows.Count)\n  Range("A3:A8").Rows.Ungroup\n  Range("AA1").Value = "done"\nEnd Sub` |
| **Expected result**        | Z1 = 6 (rows in group), AA1 = "done"                                                                                                                                                     |
| **AOO mechanism**          | Same                                                                                                                                                                                     |
| **Unattended?**            | Yes                                                                                                                                                                                      |
| **Failure classification** | Excel OM — Group/Ungroup on rows                                                                                                                                                         |

### Test C-8: Range.Validation.Add (list validation)

| Item                       | Value                                                                                                                                                                                                                                                                                      |
| -------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| **Setup**                  | `Range("A1").Validation.Delete`. `Range("A1").Validation.Add Type:=30, AlertStyle:=1, Operator:=3, Formula1:="A,B,C"`. Check validation exists.                                                                                                                                            |
| **VBA code**               | `Sub TestValidation()\n  Range("A1").Validation.Delete\n  Range("A1").Validation.Add Type:=30, AlertStyle:=1, Operator:=3, Formula1:="A,B,C"\n  Range("Z1").Value = CInt(Range("A1").Validation.Type)\n  Range("A1").Value = "B"\n  Range("AA1").Value = CStr(Range("A1").Value)\nEnd Sub` |
| **Expected result**        | Z1 = 30 (xlValidateList), AA1 = "B"                                                                                                                                                                                                                                                        |
| **AOO mechanism**          | Same                                                                                                                                                                                                                                                                                       |
| **Unattended?**            | Yes                                                                                                                                                                                                                                                                                        |
| **Failure classification** | Excel OM — Validation.Add with named arguments and constants                                                                                                                                                                                                                               |

### Test C-9: Font.ColorIndex and Interior.ColorIndex

| Item                       | Value                                                                                                                                                                                                                                                          |
| -------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Setup**                  | Set `Range("A1").Value = "text"`. Set `Range("A1").Font.ColorIndex = 5`. Set `Range("A1").Interior.ColorIndex = 3`. Read back.                                                                                                                                 |
| **VBA code**               | `Sub TestColorIndex()\n  Range("A1").Value = "text"\n  Range("A1").Font.ColorIndex = 5\n  Range("A1").Interior.ColorIndex = 3\n  Range("Z1").Value = CDbl(Range("A1").Font.ColorIndex)\n  Range("AA1").Value = CDbl(Range("A1").Interior.ColorIndex)\nEnd Sub` |
| **Expected result**        | Z1 = 5, AA1 = 3                                                                                                                                                                                                                                                |
| **AOO mechanism**          | Same                                                                                                                                                                                                                                                           |
| **Unattended?**            | Yes                                                                                                                                                                                                                                                            |
| **Failure classification** | Excel OM — ColorIndex get/set                                                                                                                                                                                                                                  |

### Test C-10: Range.Text

| Item                       | Value                                                                                       |
| -------------------------- | ------------------------------------------------------------------------------------------- |
| **Setup**                  | Set `Range("A1").Value = 42`. Read `Range("A1").Text`.                                      |
| **VBA code**               | `Sub TestText()\n  Range("A1").Value = 42\n  Range("Z1").Value = Range("A1").Text\nEnd Sub` |
| **Expected result**        | Z1 = "42" (as string — read via AA1)                                                        |
| **AOO mechanism**          | Same, read as string                                                                        |
| **Unattended?**            | Yes                                                                                         |
| **Failure classification** | Excel OM — `.Text` property may not be implemented                                          |

______________________________________________________________________

## Test Group D: Navigation and automation

**Source:** WB-12 (GetID/GetValue/GetName string functions), WB-14 (Filter,
StrPtr, InputBox)

### Test D-1: Mid, InStr, Left string functions

| Item                | Value                                                                                                                                                                                                                                                                                          |
| ------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Setup**           | Test string `"=[PRDID].[].[PRD_001]"`. Use `InStr`, `Mid`, `Left` to extract `PRD_001`.                                                                                                                                                                                                        |
| **VBA code**        | `Sub TestStringParse()\n  Dim s As String: s = "=[PRDID].[].[PRD_001]"\n  Dim pos As Integer: pos = InStr(s, "].[].[")\n  Dim result As String: result = Mid(s, pos + 7)\n  result = Left(result, Len(result) - 1)\n  Range("Z1").Value = Len(result)\n  Range("AA1").Value = result\nEnd Sub` |
| **Expected result** | Z1 = 7 (length of "PRD_001"), AA1 = "PRD_001"                                                                                                                                                                                                                                                  |
| **AOO mechanism**   | Same                                                                                                                                                                                                                                                                                           |
| **Unattended?**     | Yes                                                                                                                                                                                                                                                                                            |

### Test D-2: Filter function

| Item                | Value                                                                                                                                                                                                                                                                                            |
| ------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| **Setup**           | Create `Variant arr = Array("[PRDID].[].[PRD_001]", "[LOCREGION].[].[EUROPE]")`. `Filter(arr, "[PRDID].[].")`.                                                                                                                                                                                   |
| **VBA code**        | `Sub TestFilter()\n  Dim arr As Variant\n  arr = Array("[PRDID].[].[PRD_001]", "[LOCREGION].[].[EUROPE]")\n  Dim filtered As Variant\n  filtered = Filter(arr, "[PRDID].[].")\n  Range("Z1").Value = UBound(filtered) - LBound(filtered) + 1\n  Range("AA1").Value = CStr(filtered(0))\nEnd Sub` |
| **Expected result** | Z1 = 1, AA1 = "[PRDID].[].[PRD_001]"                                                                                                                                                                                                                                                             |
| **AOO mechanism**   | Same                                                                                                                                                                                                                                                                                             |
| **Unattended?**     | Yes                                                                                                                                                                                                                                                                                              |

### Test D-3: Like operator with wildcards

| Item                       | Value                                                                                                                                                                                                                        |
| -------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Setup**                  | Test `"PRD_001" Like "*PRD*"` and `"ABC_001" Like "*IBP*"`.                                                                                                                                                                  |
| **VBA code**               | `Sub TestLike()\n  Dim r1 As Integer, r2 As Integer\n  If "PRD_001" Like "*PRD*" Then r1 = 1 Else r1 = 0\n  If "ABC_001" Like "*IBP*" Then r2 = 1 Else r2 = 0\n  Range("Z1").Value = r1\n  Range("AA1").Value = r2\nEnd Sub` |
| **Expected result**        | Z1 = 1, AA1 = 0                                                                                                                                                                                                              |
| **AOO mechanism**          | Same                                                                                                                                                                                                                         |
| **Unattended?**            | Yes                                                                                                                                                                                                                          |
| **Failure classification** | VBA language — Like with `*` wildcards                                                                                                                                                                                       |

### Test D-4: VBA string escape `""` in literals

| Item                       | Value                                                                                                                                               |
| -------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Setup**                  | Test that `InStr(1, formula, "=@ EPMOlapMemberO(""[ABCID]")` correctly produces the literal string `=@ EPMOlapMemberO("[ABCID]`.                    |
| **VBA code**               | `Sub TestQuoteEscape()\n  Dim s As String\n  s = "=@ EPMOlapMemberO(""[ABCID]"")"\n  Range("Z1").Value = Len(s)\n  Range("AA1").Value = s\nEnd Sub` |
| **Expected result**        | Z1 = 22, AA1 = `=@ EPMOlapMemberO("[ABCID]")`                                                                                                       |
| **AOO mechanism**          | Same                                                                                                                                                |
| **Unattended?**            | Yes                                                                                                                                                 |
| **Failure classification** | VBA language — double-quote escape in string literals                                                                                               |

### Test D-5: On Error GoTo with Err.Description

| Item                       | Value                                                                                                                                                                                                                                            |
| -------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| **Setup**                  | Force a type-mismatch error by dividing a string, catch in error handler, read `Err.Description`.                                                                                                                                                |
| **VBA code**               | `Sub TestErrorHandling()\n  On Error GoTo ErrHandler\n  Dim x As Integer\n  x = CInt("not a number")\n  Range("Z1").Value = 0\n  Exit Sub\nErrHandler:\n  Range("Z1").Value = 1\n  Range("AA1").Value = Err.Description\n  Resume Next\nEnd Sub` |
| **Expected result**        | Z1 = 1, AA1 = (error description string)                                                                                                                                                                                                         |
| **AOO mechanism**          | Same                                                                                                                                                                                                                                             |
| **Unattended?**            | Yes                                                                                                                                                                                                                                              |
| **Failure classification** | VBA language — error handling and Err object                                                                                                                                                                                                     |

### Test D-6: Application.Match (gap investigation)

| Item                       | Value                                                                                                                                                                                                                                                                |
| -------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Setup**                  | Create range `Range("A1:A3")` with values `Array(10, 20, 30)`. `Application.Match(20, Range("A1:A3"), 0)`.                                                                                                                                                           |
| **VBA code**               | `Sub TestAppMatch()\n  Range("A1").Value = 10: Range("A2").Value = 20: Range("A3").Value = 30\n  Dim result As Variant\n  On Error Resume Next\n  result = Application.Match(20, Range("A1:A3"), 0)\n  On Error GoTo 0\n  Range("Z1").Value = CDbl(result)\nEnd Sub` |
| **Expected result**        | Z1 = 2.0 (position of 20)                                                                                                                                                                                                                                            |
| **AOO status**             | Application.Match NOT implemented                                                                                                                                                                                                                                    |
| **AOO mechanism**          | Same                                                                                                                                                                                                                                                                 |
| **Unattended?**            | Yes                                                                                                                                                                                                                                                                  |
| **Failure classification** | Confirmed defect — `Application.Match` method missing                                                                                                                                                                                                                |

______________________________________________________________________

## Test Group E: Performance and repeated operations

**Source:** WB-09, WB-11 (Timer usage pattern)

### Test E-1: Timer delta measurement

| Item                | Value                                                                                                                                                                                                           |
| ------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Setup**           | Record `Timer` before and after a small computation loop.                                                                                                                                                       |
| **VBA code**        | `Sub TestTimer()\n  Dim t1 As Double, t2 As Double\n  t1 = Timer\n  Dim i As Integer\n  For i = 1 To 1000: Range("A1").Value = i: Next i\n  t2 = Timer\n  Range("Z1").Value = IIf(t2 - t1 >= 0, 1, 0)\nEnd Sub` |
| **Expected result** | Z1 = 1.0 (Timer is monotonic)                                                                                                                                                                                   |
| **AOO mechanism**   | Same                                                                                                                                                                                                            |
| **Unattended?**     | Yes                                                                                                                                                                                                             |

______________________________________________________________________

## Test Group F: Master-data workflow substitutes

**Source:** WB-14 (InputBox, StrPtr, Filter), WB-10 (Right/Left/Len parsing)

### Test F-1: InputBox with 3 arguments

| Item                       | Value                                                                                                                                                       |
| -------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Setup**                  | Call `InputBox("prompt", "title", "default")`. Since InputBox is interactive, this test should be marked exploratory unless headless input can be injected. |
| **VBA code**               | `Sub TestInputBox()\n  Dim r As String\n  r = InputBox("Enter text", "Title", "default")\n  Range("Z1").Value = CInt(r = "default")\nEnd Sub`               |
| **Expected result**        | Z1 = -1 (True if user enters "default")                                                                                                                     |
| **AOO mechanism**          | `runMacro` — may hang waiting for input                                                                                                                     |
| **Unattended?**            | No — interactive                                                                                                                                            |
| **Failure classification** | VBA language — InputBox 3-arg form; needs manual run or env injection                                                                                       |
| **Priority**               | P3 — exploratory only                                                                                                                                       |

### Test F-2: StrPtr function (gap investigation)

| Item                       | Value                                                                                                                                                                    |
| -------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| **Setup**                  | `StrPtr("")` — test if StrPtr function exists in AOO VBA.                                                                                                                |
| **VBA code**               | `Sub TestStrPtr()\n  On Error Resume Next\n  Dim p As Long\n  p = StrPtr(\"test\")\n  Range("Z1").Value = CInt(Err.Number = 0)\n  Range("AA1").Value = CStr(p)\nEnd Sub` |
| **Expected result**        | If StrPtr exists: Z1=1, AA1=<number>. If not: Z1=0, AA1="0".                                                                                                             |
| **AOO status**             | StrPtr NOT found in source                                                                                                                                               |
| **AOO mechanism**          | Same                                                                                                                                                                     |
| **Unattended?**            | Yes                                                                                                                                                                      |
| **Failure classification** | Confirmed defect — `StrPtr` function not implemented                                                                                                                     |

### Test F-3: Names collection (from WB-10)

| Item                       | Value                                                                                                                                                                             |
| -------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Setup**                  | `ThisWorkbook.Names.Add "TestName", "=$A$1"`. Set `Range("A1").Value = "hello"`. Read `Range("Z1").Value = ThisWorkbook.Names("TestName").RefersToRange.Value`.                   |
| **VBA code**               | `Sub TestNames()\n  ThisWorkbook.Names.Add "TestName", "=$A$1"\n  Range("A1").Value = "hello"\n  Range("Z1").Value = ThisWorkbook.Names("TestName").RefersToRange.Value\nEnd Sub` |
| **Expected result**        | Z1 = "hello"                                                                                                                                                                      |
| **AOO mechanism**          | Same, read as string                                                                                                                                                              |
| **Unattended?**            | Yes                                                                                                                                                                               |
| **Failure classification** | Excel OM — Names collection and RefersToRange                                                                                                                                     |

______________________________________________________________________

## Test Group G: Event handlers

**Source:** WB-04, WB-05, WB-12, WB-14 (Worksheet_BeforeDoubleClick)

### Test G-1: Worksheet_BeforeDoubleClick fires

| Item                       | Value                                                                                                                                                                         |
| -------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Setup**                  | Document has a `Worksheet_BeforeDoubleClick` handler that writes a marker value to Z1.                                                                                        |
| **VBA code**               | `Sub TestEventSetup()\n  Range("Z1").Value = 0\nEnd Sub\nPrivate Sub Worksheet_BeforeDoubleClick(ByVal Target As Range, Cancel As Boolean)\n  Range("Z1").Value = 1\nEnd Sub` |
| **Expected result**        | After programmatically triggering double-click on the sheet: Z1 = 1                                                                                                           |
| **AOO mechanism**          | UNO dispatch — simulate `XCell::onDoubleClick` or similar; **may not be automatable**                                                                                         |
| **Unattended?**            | No — requires UI event simulation                                                                                                                                             |
| **Failure classification** | Excel OM event — exploratory; verify event registration exists                                                                                                                |

______________________________________________________________________

## Integration recommendations

| Test                            | Priority | Ready for build integration? | Rationale                                                |
| ------------------------------- | -------- | ---------------------------- | -------------------------------------------------------- |
| A-1 (ColumnWidth get/set)       | P1       | Yes                          | Simple, deterministic, tests supported feature           |
| A-3 (Columns iteration)         | P1       | Yes                          | Tests column enumeration + Column property               |
| A-4 (EntireColumn.Hidden)       | P1       | Yes                          | Tests Hidden property on columns                         |
| B-1 (Public function call)      | P1       | Yes                          | Core language feature, trivial                           |
| B-2 (Boolean function return)   | P1       | Yes                          | Tests function return + IsEmpty                          |
| B-3 (Module-level variable)     | P1       | Yes                          | Tests scoping, multi-call                                |
| B-4 (ReDim Preserve array)      | P2       | Yes                          | Tests dynamic array handling                             |
| C-1 (Value2)                    | P1       | Yes                          | **Confirms defect** — test should fail until implemented |
| C-2 (Formula2)                  | P1       | Yes                          | **Confirms defect** — test should fail until implemented |
| C-3 (Find with LookIn)          | P1       | Yes                          | Tests Find + named argument                              |
| C-4 (Offset)                    | P1       | Yes                          | Tests supported feature                                  |
| C-5 (Offset + EntireRow.Hidden) | P1       | Yes                          | Tests chained property access                            |
| C-6 (Cut + Insert)              | P2       | Yes                          | Tests clipboard operations                               |
| C-7 (Rows.Group/Ungroup)        | P1       | Yes                          | Tests supported feature                                  |
| C-8 (Validation.Add)            | P1       | Yes                          | Tests validation API                                     |
| C-9 (ColorIndex)                | P1       | Yes                          | Tests supported feature                                  |
| C-10 (Range.Text)               | P2       | Yes                          | Tests Text property                                      |
| D-1 (Mid/InStr/Left)            | P1       | Yes                          | Tests string functions                                   |
| D-2 (Filter)                    | P1       | Yes                          | Tests Filter function                                    |
| D-3 (Like)                      | P1       | Yes                          | Tests Like operator                                      |
| D-4 (Quote escape)              | P2       | Yes                          | Tests string literal parsing                             |
| D-5 (Error handling)            | P1       | Yes                          | Tests On Error GoTo + Err                                |
| D-6 (Application.Match)         | P1       | Yes                          | **Confirms defect** — test should fail until implemented |
| E-1 (Timer)                     | P3       | Yes                          | Simple Timer test                                        |
| F-1 (InputBox 3-arg)            | P3       | No                           | Interactive — exploratory                                |
| F-2 (StrPtr)                    | P2       | Yes                          | **Confirms defect** — test should fail                   |
| F-3 (Names collection)          | P2       | Yes                          | Tests Names API                                          |
| G-1 (BeforeDoubleClick)         | P3       | No                           | Requires UI event simulation                             |
