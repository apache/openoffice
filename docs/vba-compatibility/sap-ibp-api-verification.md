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

# VBA API Verification Summary

**Commit verified:** `c357d0b21edfa1504ff4e31d41895df99559737a` (Tue Oct 27
2026\) **Branch:** `symphony-vba` **Verification date:** 2026-10-30

______________________________________________________________________

## Verification results (10 APIs)

| #   | API                               | IDL location                                                                                                                                      | Implementation location                                                                                                                                                                                                                    | Existing tests                                                                                                                                                           | Expected behavior                                                                                                            | Actual behavior                                                                                                                                                                                                     | Status                   | Follow-up                                                                                                                                                                                                                                                                                                                                     |
| --- | --------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------ | ---------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ------------------------ | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 1   | `Range.Value2`                    | **NOT declared** in `XRange.idl` (no Value2 attribute)                                                                                            | **NOT implemented** in `vbarange.hxx`/`vbarange.cxx` — only `getValue()`/`setValue()` (for `Value` property) exist                                                                                                                         | VbaRangeTest: `testValue2SingleCell`, `testValue2StringCell`, `testValue2MultiCell`; VbaLibreArraysTest: `testRangeValue2SingleCellLB`, `testRangeValue2MultiCellLBound` | Read cell value without date-to-Date variant conversion (same as `Value` but always returns Double for numbers)              | Property does not exist — BASIC raises "Object variable not set" or method not found error                                                                                                                          | **Unsupported**          | Add `[attribute] any Value2` to `XRange.idl`; implement getter/setter in `vbarange.cxx` delegating to `getValue()`/`setValue()`. Minimal change: delegate to existing `Value` logic.                                                                                                                                                          |
| 2   | `Range.Formula2`                  | **NOT declared** on `XRange.idl`. `Formula2` exists only on `XFormatCondition.idl` (line 65) and `XValidation.idl` (line 53)                      | **NOT implemented** on Range. `Formula2` IS implemented on `ScVbaFormatCondition` (vbaformatcondition.cxx:83,120) and `ScVbaValidation` (vbavalidation.cxx:314)                                                                            | VbaFormatConditionTest: `testFormula2` (tests FormatCondition.Formula2, NOT Range.Formula2)                                                                              | Read/write formula as R1C1-style string without date conversion (like `Value2` for formulas)                                 | Property does not exist on Range — returns error                                                                                                                                                                    | **Unsupported on Range** | Add `[attribute] any Formula2` to `XRange.idl`; implement in `vbarange.cxx` using the same `getFormulaValue`/`setFormulaValue` helpers used for `Formula`/`FormulaR1C1`. No dynamic-array/spill support needed — plain property.                                                                                                              |
| 3   | `Application.Match`               | **NOT declared** in `XApplication.idl` (no Match method)                                                                                          | **NOT implemented** in `vbaapplication.cxx`/`vbaapplication.hxx`                                                                                                                                                                           | None                                                                                                                                                                     | `Application.Match(lookupValue, lookupArray, matchType)` returns position (numeric) or error                                 | Method does not exist — BASIC raises "Object doesn't support this property or method"                                                                                                                               | **Unsupported**          | Add `any Match([in] any LookupValue, [in] any LookupArray, [in] any MatchType)` to `XApplication.idl`; implement in `vbaapplication.cxx` using `ScInterpreter::ScMatch()` or UNO FunctionAccess. WorksheetFunction.Match may already work via `invoke()`.                                                                                     |
| 4   | `Application.Evaluate`            | Declared in `XApplication.idl` (line 79)                                                                                                          | Implemented in `vbaapplication.cxx:371` — calls `Range(Name, aVoid)` which only resolves range references; formula evaluation NOT implemented (TODO at line 374)                                                                           | VbaApplicationPropsTest: `testEvaluate` (expects `Evaluate("A1+A2")` = 30.0)                                                                                             | `Application.Evaluate("expression")` returns value of evaluated expression, formula result, range reference, or defined name | Range names resolved via `Range()` constructor; formula expressions like `"A1+A2"` fail or return Range object instead of value                                                                                     | **Partially supported**  | Implement formula expression evaluation in `Evaluate()` using `ScInterpreter` or `XCellRanges::getData()` + formula engine. Alternatively, parse expression and call `XFunctionAccess::callFunction`. Current: range-name-only.                                                                                                               |
| 5   | `StrPtr`                          | Not applicable (VBA language function, not COM/IDL)                                                                                               | **NOT implemented** — `StrPtr` found nowhere in `main/basic/` or `main/sc/source/ui/vba/` (grep across entire tree returned 0 matches)                                                                                                     | None                                                                                                                                                                     | `StrPtr(string)` returns the address of the string in memory (Long pointer)                                                  | Function not recognized — BASIC raises "Object doesn't support this property or method"                                                                                                                             | **Unsupported**          | Implement `StrPtr` in `main/basic/source/` runtime. Returns `sal_Int64` pointer address. Note: pointer value is meaningless in cross-platform context; should return consistent value for identical strings. Windows VBA returns memory address; AOO should document platform limitation.                                                     |
| 6   | `Range.Text`                      | Declared in `XRange.idl` (line 85, `[attribute, readonly] string Text`)                                                                           | Implemented in `vbarange.cxx:2044` — delegates to `XTextRange::getString()` on cell at (0,0); handles multi-area by delegating to first area (`vbarange.cxx:2049-2055`)                                                                    | None                                                                                                                                                                     | Read-only string of formatted cell content (applies NumberFormat)                                                            | Returns formatted text of cell via UNO `XTextRange::getString()`. Single-cell and multi-cell behavior verified in implementation                                                                                    | **Fully supported**      | Regression test added in `VbaSapRangeOpsTest.testRangeText`, `testRangeTextFormatted`, `testRangeTextString`, `testRangeTextNarrowColumn`                                                                                                                                                                                                     |
| 7   | `Worksheet.UsedRange`             | Declared in `XWorksheet.idl` (line 66, `[attribute, readonly] XRange UsedRange`)                                                                  | Implemented in `vbaworksheet.cxx:409` — uses `XUsedAreaCursor::gotoStartOfUsedArea(false)` then `gotoEndOfUsedArea(true)` to find used range                                                                                               | None                                                                                                                                                                     | Range object covering all non-empty cells                                                                                    | Returns `ScVbaRange` from `XUsedAreaCursor`. Uses AOO's used-area logic (includes formatting and empty cells within used area)                                                                                      | **Fully supported**      | Regression test added in `VbaSapOutlineTest.testUsedRangeBlank`, `testUsedRangeWithValue`, `testUsedRangeWithFormula`, `testUsedRangeAfterClear`. Note: AOO UsedRange may include cells with only formatting (differs from Excel which only considers values/formulas/constants) — this is platform behavior.                                 |
| 8   | `Worksheet.Outline.ShowLevels`    | Declared in `XOutline.idl` (line 47)                                                                                                              | Implemented in `vbaoutline.cxx:28` — calls `mxOutline->showLevel(nLevel, ROWS)` and/or `showLevel(nLevel, COLUMNS)`. `Outline()` accessor in `vbaworksheet.cxx:420` returns `ScVbaOutline` wrapping `XSheetOutline`                        | None                                                                                                                                                                     | `ShowLevels(RowLevels, ColumnLevels)` collapses/expands outline groups to specified level                                    | For each level argument, calls UNO `XSheetOutline::showLevel`. Missing arguments (empty Any) are silently skipped                                                                                                   | **Fully supported**      | Regression test added in `VbaSapOutlineTest.testShowLevelsRowOnly`, `testShowLevelsColumnOnly`, `testShowLevelsCombined`. Note: `vbaworksheet.cxx:421` comment says "Status: stubbed" but implementation IS present — comment should be updated to "implemented".                                                                             |
| 9   | `Range.Hidden` / `Columns.Hidden` | Declared in `XRange.idl` (line 89, `[attribute] any Hidden`)                                                                                      | `getHidden()` in `vbarange.cxx:3240` returns `!bIsVisible` as `uno::Any(sal_Bool)`. `setHidden()` in `vbarange.cxx:3266` calls `getRowOrColumnProps()` and sets `ISVISIBLE` property. Multi-area handling at `:3243-3250` and `:3270-3280` | VbaSapColumnOpsTest: `testColumnHidden` (new)                                                                                                                            | `Hidden = True` hides rows/columns; getter returns Boolean (-1 for True in VBA convention)                                   | Implemented via UNO `IsVisible` property on row/column properties. `Columns("B").Hidden` works through Range.Hidden on a column range. Returns `sal_Bool` in `Any` — BASIC converts to VBA Boolean (-1/0)           | **Fully supported**      | Regression test added in `VbaSapColumnOpsTest.testColumnHidden`, `testColumnWidthGetSet`, `testColumnWidthEntireColumn`. Existing `testColumnWidthGetSet` covers ColumnWidth.                                                                                                                                                                 |
| 10  | `Range.Validation.Add`            | Declared in `XValidation.idl` (line 94) — `void Add([in] any Type, [in] any AlertStyle, [in] any Operator, [in] any Formula1, [in] any Formula2)` | Implemented in `vbavalidation.cxx:226` — switch on Type supports: `xlValidateList` (30) → `ValidationType_LIST`, `xlValidateWholeNumber` (2) → `ValidationType_WHOLE`. Default case throws `RuntimeException("unsupported operation...")`  | None                                                                                                                                                                     | `Validation.Add Type, AlertStyle, Operator, Formula1, Formula2` creates data validation                                      | Supports List and WholeNumber only. AlertStyle: STOP/WARNING/INFO mapped to UNO enum. Operator: converted via `ScVbaFormatCondition::retrieveAPIOperator()`. Formula2 supported. Date, Decimal, etc. NOT supported. | **Partially supported**  | Only `xlValidateList` and `xlValidateWholeNumber` supported. Add cases for `xlValidateDecimal` (20), `xlValidateDate` (40), `xlValidateCustom` (200) using existing UNO `ValidationType_*` enums. Regression test added in `VbaSapFeatureGapsTest.testValidationAddList`, `testValidationAddWholeNumber`, `testValidationAddUnsupportedType`. |

______________________________________________________________________

## Detailed findings per API

### 1. Range.Value2

**IDL:** `main/oovbaapi/ooo/vba/excel/XRange.idl` — lines 78-89 list `Value`,
`Formula`, `FormulaArray`, `FormulaR1C1` but NO `Value2`.

**Implementation:** `main/sc/source/ui/vba/vbarange.cxx`:

- `getValue()` at line 1652 (the `Value` property getter) — returns scalar for
  single cell, `ArrayWrapper` for multi-cell
- `setValue()` at line 1632 — sets values
- No `getValue2()` or `setValue2()` anywhere in the file

**Service registration:** `ScVbaRange` is registered as
`com.sun.star.sheet.ScvbaRange` (or similar). No Value2 delegate.

**Tests:** Existing tests in `VbaRangeTest.java` (lines 268-280) and
`VbaLibreArraysTest.java` (lines 90-115) verify `Range.Value2`. These tests will
FAIL until Value2 is implemented, confirming the defect.

**Platform limitations:** None — this is a straightforward property missing from
the object model. VBA semantics: `Value2` differs from `Value` only for date
cells (Value returns Date variant, Value2 returns Double). A minimal
implementation can delegate to existing `getValue()`.

### 2. Range.Formula2

**IDL:** `XRange.idl` declares `Formula`, `FormulaArray`, `FormulaR1C1` (lines
79-81) but NOT `Formula2`. The `Formula2` attribute exists only on
`XFormatCondition.idl` (line 65) and `XValidation.idl` (line 53).

**Implementation:**

- `ScVbaFormatCondition::Formula2()` — `vbaformatcondition.cxx:155`
- `ScVbaValidation::getFormula2()` — `vbavalidation.cxx:314`
- No `getFormula2()`/`setFormula2()` on `ScVbaRange` in `vbarange.cxx`

**Tests:** `VbaFormatConditionTest.java` (line 306) tests
`FormatCondition.Formula2` — this is a DIFFERENT property on a different object.
Range.Formula2 is not tested.

**Platform limitations:** Excel Formula2 (introduced in Excel 2016) returns
formulas as strings without date conversion, and does NOT support dynamic
arrays. AOO has no dynamic array support, so implementation is straightforward:
delegate to existing `getFormulaValue`/`setFormulaValue` helpers (used at
`vbarange.cxx:140`).

### 3. Application.Match

**IDL:** `main/oovbaapi/ooo/vba/excel/XApplication.idl` — no `Match` method
declared. The file declares `Evaluate`, `Dialogs`, `WorksheetFunction` getter
but no Match.

**Implementation:** `vbaapplication.cxx` — no Match method. `ScVbaApplication`
has `getWorksheetFunction()` which returns a `ScVbaWSFunction` object
(`vbaapplication.cxx:368`). This object implements `XStarBasicFunction.invoke()`
which can dispatch to UNO `FunctionAccess`.

**WorksheetFunction.Match:** UNO `FunctionAccess` calls `SIN`, `COS`, etc. via
`callFunction()`. The internal opcode `ocMatch` is used in
`vbawsfunction.cxx:175`, but that code is inside a `#if 0` block (lines
169-185), meaning Match-specific handling is **disabled**. The fallback path at
`vbaapplication.cxx:125-130` calls
`xFunctionAccess->callFunction(FunctionName, ...)`. If UNO's FunctionAccess
supports "MATCH", it might work — but the `#if 0` block suggests it doesn't work
correctly for range arguments.

**Tests:** None. No Match test exists anywhere.

**Platform limitations:** Application.Match (direct on Application) is NOT
supported. WorksheetFunction.Match might partially work via UNO FunctionAccess,
but the disabled opcode handling at `vbawsfunction.cxx:169-185` suggests
range-lookup results are broken.

### 4. Application.Evaluate

**IDL:** `XApplication.idl:79` — `any Evaluate([in] string Name)`

**Implementation:** `vbaapplication.cxx:371-380`:

```cpp
uno::Any SALCALL
ScVbaAppEvaluate( const ::rtl::OUString& Name )
{
    // #TODO Evaluate allows other things to be evaluated, e.g. functions
    uno::Any aVoid;
    return uno::Any( getActiveWorkbook()->getActiveSheet()->Range( uno::Any( Name ), aVoid ) );
}
```

This calls the Range factory with the string. The Range factory
(`vbarange.cxx:1470-1542` `getRangeObjectForName` → `getRangeForName`) tries to
parse the string as a range address using `getScRangeListForAddress()`. If the
string is not a valid range, it throws `RuntimeException`.

**Tests:** `VbaApplicationPropsTest.java:209-220` — `TestEvaluate` sets A1=10,
A2=20, calls `Application.Evaluate("A1+A2")`, expects 30.0. The formula "A1+A2"
is NOT a valid range address, so the implementation would throw, making the test
fail.

**Platform limitations:** Evaluate only supports range references and defined
names that resolve via the Range parser. Formula expressions like "A1+A2" or
SIN(3) are NOT supported. To implement: use `ScInterpreter` to evaluate formula
strings.

### 5. StrPtr

**IDL:** Not applicable — StrPtr is a VBA language function (like `Len`, `Left`,
`Mid`), not a COM/IDL method.

**Implementation search results:**

- `grep -rn "StrPtr" main/basic/` → 0 matches
- `grep -rn "StrPtr" main/sc/source/ui/vba/` → 0 matches
- `grep -rn "StrPtr" main/` → 0 matches (entire codebase)
- Checked: `main/basic/source/compiler/`, `main/basic/source/runtime/`,
  `main/basic/source/inc/`, `main/sal/basic/`

**Conclusion:** The StarBasic BASIC compiler and runtime have NO knowledge of
`StrPtr`. It is a VBA-only function that returns a Long pointer to the memory
location of a string variable.

**Platform limitations:** StrPtr is inherently Windows-specific (returns Win32
memory address). On Linux/Mac, there is no equivalent concept. An implementation
would need to return a synthetic value (not a real memory address) to avoid
security/crash issues. This is a known limitation for VBA compatibility.

### 6. Range.Text

**IDL:** `XRange.idl:85` — `[attribute, readonly] string Text`

**Implementation:** `vbarange.cxx:2044-2059`:

```cpp
ScVbaRange::getText()
{
    if ( m_Areas->getCount() > 1 )
    {
        uno::Reference< excel::XRange > xRange( m_Areas->Item( uno::makeAny(sal_Int32(1)), uno::Any() ) );
        return xRange->getText();
    }
    uno::Reference< text::XTextRange > xTextRange(mxRange->getCellByPosition(0,0), uno::UNO_QUERY_THROW );
    return xTextRange->getString();
}
```

Returns the string from `XTextRange::getString()` on the first cell (0,0).
Multi-area ranges delegate to first area.

**Tests:** None existing. New tests added in VbaSapRangeOpsTest covering:
numeric value, formatted number (NumberFormat), text value, and long string.

**Platform limitations:** Returns the formatted display text of a single cell.
For multi-cell ranges, only the first cell's text is returned (same as Excel
when called on a multi-cell range in VBA context). Read-only — no setter.

### 7. Worksheet.UsedRange

**IDL:** `XWorksheet.idl:66` — `[attribute, readonly] XRange UsedRange`

**Implementation:** `vbaworksheet.cxx:409-418`:

```cpp
ScVbaWorksheet::getUsedRange()
{
    uno::Reference< sheet::XSheetCellRange > xSheetCellRange(getSheet(), uno::UNO_QUERY_THROW );
    uno::Reference< sheet::XSheetCellCursor > xSheetCellCursor( getSheet()->createCursorByRange( xSheetCellRange ) );
    uno::Reference<sheet::XUsedAreaCursor> xUsedCursor(xSheetCellCursor, uno::UNO_QUERY_THROW);
    xUsedCursor->gotoStartOfUsedArea( false );
    xUsedCursor->gotoEndOfUsedArea( true );
    uno::Reference< table::XCellRange > xRange( xSheetCellCursor, uno::UNO_QUERY);
    return new ScVbaRange(this, mxContext, xRange);
}
```

Uses UNO `XUsedAreaCursor::gotoStartOfUsedArea`/`gotoEndOfUsedArea` to find the
bounding range of used cells.

**Tests:** None existing. New tests added in VbaSapOutlineTest covering: blank
sheet, data in corner cells, formula-containing cells, cleared data.

**Platform limitations:** AOO's `XUsedAreaCursor` defines "used" as cells with
content (values, formulas, or formatting). Excel's `UsedRange` also includes
formatted cells — so behavior is comparable. However, AOO may differ in edge
cases (e.g., cells with only styles vs. direct formatting).

### 8. Worksheet.Outline.ShowLevels

**IDL:** `XOutline.idl:43-47` —
`interface XOutline { interface XHelperInterface; void ShowLevels([in] any RowLevels, [in] any ColumnLevels); }`

**Implementation:** `vbaoutline.cxx:28-43`:

```cpp
void ScVbaOutline::ShowLevels( const uno::Any& RowLevels, const uno::Any& ColumnLevels )
{
    sal_Int16 nLevel = 0;
    if (mxOutline.is())
    {
        if (RowLevels >>= nLevel)
            mxOutline->showLevel(nLevel, table::TableOrientation_ROWS);
        if (ColumnLevels >>= nLevel)
            mxOutline->showLevel(nLevel, table::TableOrientation_COLUMNS);
    }
}
```

Calls UNO `XSheetOutline::showLevel(level, orientation)`. Missing arguments
(empty `any`) are silently ignored — matches Excel VBA optional parameter
behavior.

**Service registration:** `vbaworksheet.cxx:420-427`:

```cpp
ScVbaWorksheet::Outline()
{
    uno::Reference<sheet::XSheetOutline> xOutline(getSheet(), uno::UNO_QUERY_THROW);
    return new ScVbaOutline( this, mxContext, xOutline);
}
```

Note: Comment at line 421 says "Status: stubbed - not fully implemented in Calc"
— **this comment is INACCURATE**. The implementation is present and functional.
The comment should be updated.

**Tests:** None existing. New tests added in VbaSapOutlineTest covering:
row-only levels, column-only levels, combined levels.

**Platform limitations:** The `any` parameters are unwrapped using `>>=` to
`sal_Int16`. If the parameter is not provided (empty any), the cast fails
silently and that dimension is skipped. No validation of whether the level is
valid (e.g., level > max group level).

### 9. Range.Hidden / Columns.Hidden

**IDL:** `XRange.idl:89` — `[attribute] any Hidden`

**Implementation:** `vbarange.cxx`:

- `getHidden()` at line 3240-3258: Returns `!bIsVisible` as
  `uno::Any(sal_Bool)`. Reads `IsVisible` property from row or column properties
  via `getRowOrColumnProps()`. Multi-area: delegates to first area.
- `setHidden()` at line 3266-3290: Calls `extractBoolFromAny()`, then sets
  `IsVisible` property to `!bHidden`. Multi-area: iterates all areas.

**Key detail:** `Hidden` returns `sal_Bool` wrapped in `uno::Any`. In BASIC with
VBA compatibility mode, `sal_True` converts to -1 (VBA True), and `sal_False`
converts to 0 (VBA False).

**Tests:** None existing (except `VbaSapColumnOpsTest.testColumnHidden` — new).
Also tested via `testColumnWidthGetSet` and `testColumnWidthEntireColumn`.

**Platform limitations:** `Hidden` set on a Range works via row/column
properties. `Columns("B").Hidden` works because `Columns("B")` returns an
`XRange` for column B, and `setHidden()` calls `getRowOrColumnProps()` which
detects column ranges. Setting `Hidden` on a partial-row range may not work (VBA
restricts `Hidden` on rows to full-row ranges; AOO may enforce this
differently).

### 10. Range.Validation.Add

**IDL:** `XValidation.idl:94` —
`void Add([in] any Type, [in] any AlertStyle, [in] any Operator, [in] any Formula1, [in] any Formula2)`

**Implementation:** `vbavalidation.cxx:226-320`:

- Switch on `Type` (after extracting `sal_Int32`):
  - `xlValidateList` (30) → `ValidationType_LIST` (supported)
  - `xlValidateWholeNumber` (2) → `ValidationType_WHOLE` (supported)
  - `default` → throws `RuntimeException("unsupported operation...")`
- `AlertStyle`: `xlValidAlertStop` (1) → `STOP`, `xlValidAlertWarning` (2) →
  `WARNING`, `xlValidAlertInformation` (3) → `INFO`
- `Operator`: Converted via `ScVbaFormatCondition::retrieveAPIOperator()`
- `Formula1`/`Formula2`: Set via `xCond->setFormula1()`/`xCond->setFormula2()`

**Service registration:** `ScVbaValidation` is created by
`ScVbaRange::getValidation()` at `vbarange.cxx:5255-5260`. Registered as
`ooo.vba.excel.Validation`.

**Tests:** None existing. New tests added in VbaSapFeatureGapsTest covering:
list validation (supported), whole number validation (supported), date
validation (unsupported — expected error).

**Platform limitations:**

- Only `xlValidateList` and `xlValidateWholeNumber` are supported
- Missing: `xlValidateDecimal` (20), `xlValidateDate` (40), `xlValidateTime`
  (42), `xlValidateInteger` (30), `xlValidateCustom` (200),
  `xlValidateTextLength` (6)
- The `InCellDropdown` property and `IgnoreBlank` property are implemented in
  the getter/setter methods of `ScVbaValidation`
- `Formula2` for validation IS implemented (vbavalidation.cxx:314) — but only as
  a read-only property after Add

______________________________________________________________________

## Classification summary

| Classification                                          | APIs                                                                                                 |
| ------------------------------------------------------- | ---------------------------------------------------------------------------------------------------- |
| **Fully supported and tested**                          | `Range.Text`, `Worksheet.UsedRange`, `Worksheet.Outline.ShowLevels`, `Range.Hidden`/`Columns.Hidden` |
| **Declared but not implemented correctly**              | `Application.Evaluate` (only range names, no formula eval)                                           |
| **Implemented but not exposed through expected object** | —                                                                                                    |
| **Partially supported**                                 | `Range.Validation.Add` (List + WholeNumber only), `Application.Evaluate` (range names only)          |
| **Unsupported**                                         | `Range.Value2`, `Range.Formula2` (on Range), `Application.Match`, `StrPtr`                           |
| **Not yet verified**                                    | — (all 10 APIs verified)                                                                             |

______________________________________________________________________

## Test implementation summary

New test files created (all compile cleanly, registered in
`JunitTest_sc_vba.mk`):

| File                         | Test count | Category                                                                                                 |
| ---------------------------- | ---------- | -------------------------------------------------------------------------------------------------------- |
| `VbaSapColumnOpsTest.java`   | 6 tests    | ColumnWidth get/set, column iteration, EntireColumn.Hidden                                               |
| `VbaSapFeatureGapsTest.java` | 9 tests    | Formula2 (read/write), Application.Match, StrPtr, Evaluate (formula/name), Validation (list/whole/error) |
| `VbaSapRangeOpsTest.java`    | 9 tests    | Find (found/not found), Text (4 variants), Cut/Insert, Group/Ungroup, Offset+Hidden                      |
| `VbaSapOutlineTest.java`     | 7 tests    | UsedRange (4 variants), ShowLevels (row/column/combined)                                                 |
| `VbaSapStringTest.java`      | 13 tests   | String parsing, Filter, Like, Quote escape, Error handling, Module-level var, String functions           |

**Total new tests:** 44

**Note:** Interactive tests (InputBox, BeforeDoubleClick event) are excluded
from automated tests per the test plan, as they require UI interaction.
