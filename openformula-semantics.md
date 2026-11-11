# OpenFormula Calculation Semantics Audit — Apache OpenOffice Calc

Source-of-truth: Apache OpenOffice implementation (main/sc). ODF 1.4 Part 4 sections cited where the AOO implementation-defined/known-limitations docs reference them. Findings marked **[CONFIRMED]** have source file:line evidence; **[UNVERIFIED]** are hypotheses requiring spec text and/or runtime testing.

## 1. Conversion to scalar

**[CONFIRMED]** ScInterpreter provides `GetStackType()` (interpre.hxx:364) with default replacement (svMissing/svEmptyCell -> svDouble) and `GetRawStackType()` (interpre.hxx:362). Scalar conversion is used by functions taking a single value from a reference/array. `PopSingleRef`/`PopDoubleRef` extract a single value from a reference.

**[UNVERIFIED]** Exact scalar-conversion rules for 1x1 arrays vs references vs scalars per ODF 1.4 §4.2 not traced to a single implementation point.

## 2. Conversion to number

**[CONFIRMED]** `ConvertStringToValue(const String&)` declared at interpre.hxx:228. `GetDouble()` / `GetDoubleWithDefault()` at interpre.hxx:370-371. Implicit string-to-number conversion per odf_docs/ODF14-IMPLEMENTATION-DEFINED-PUBLIC.md §19.646: "locale independent (integers and ISO 8601 dates and times only)".

**[ODF 1.4 impl-defined]** §6.3.5 conversion of Text to Number — AOO: locale independent, integers and ISO 8601 only (§19.646).

**[UNVERIFIED]** Whether leading/trailing whitespace, thousands separators, or locale decimal symbols are accepted in implicit conversion.

## 3. Conversion to integer

**[CONFIRMED]** `ScInt()` in interpr2.cxx. `INT` truncates toward zero (standard). Uses `::rtl::math::approxFloor` in several places (e.g. EASTERSUNDAY interpr2.cxx:271, bit ops interpr1.cxx:1186).

**[UNVERIFIED]** INT vs TRUNC exact behavior for negative values and huge doubles per ODF 1.4 §4.4.

## 4. Conversion to logical

**[CONFIRMED]** `GetBool()` returns `GetDouble() != 0.0` (interpre.hxx:373). `ScAnd`/`ScOr`/`ScXor`/`ScNot` in interpr1.cxx.

**[ODF 1.4 impl-defined]** §6.3.6 conversion of non-integer Number to Logical; ISEVEN/ISODD of a Logical impl-defined (§19.646 list item 6.13.8).

## 5. Conversion to text

**[CONFIRMED]** `GetString()` at interpre.hxx:374. `PushString`/`PushStringBuffer` at interpre.hxx:354-355. `ScText()` (TEXT function) interpr1.cxx. Locale-sensitive conversion via `SvNumberFormatter* pFormatter` (interpre.hxx:187).

**[ODF 1.4 impl-defined]** §6.3.7 conversion to text; TEXT format-code meaning impl-defined (odf_docs §19.646 item 6.20.23). HOST-LOCALE used for number-to-string (`&`, CONCATENATE), VALUE, DATEVALUE, TIMEVALUE, FIXED, DOLLAR, UPPER, LOWER, PROPER, SEARCH (§19.646).

## 6. Empty cell handling

**[CONFIRMED]** `HasCellEmptyData()` (interpre.hxx:240) "Really empty or inherited emptiness". `HasCellValueData()` (interpre.hxx:243). `HasCellStringData()` (interpre.hxx:244). `PushCellResultToken` with `bDisplayEmptyAsString` (interpre.hxx:308). Empty cells normally treated as 0 in numeric contexts (GetCellValueOrZero interpre.hxx:231).

**[ODF 1.4 migration]** COUNTA/COUNTBLANK empty-cell semantics changed in ODF 1.4 vs 1.3. AOO's `ScCount2` (COUNTA) and `ScCountEmptyCells` (COUNTBLANK) need re-check against ODF 1.4. **[UNVERIFIED]** COUNTBLANK of "" impl-defined (odf_docs item 6.13.8).

## 7. String versus numeric values

**[CONFIRMED]** `IsString()` (interpre.hxx:464), `ScIsString`, `ScIsNonString` (interpr1.cxx). `ISTEXT`/`ISNONTEXT` map to ocIsString/ocIsNonString (core_resource.src:108-109).

**[ODF 1.4 impl-defined]** ISNUMBER/ISTEXT of a complex number impl-defined (odf_docs item 4.3.3 list).

## 8. Error propagation

**[CONFIRMED]** `nGlobalError` (interpre.hxx:196). `SetError()` (interpre.hxx:854). `Push()` substitutes `FormulaErrorToken` when `nGlobalError` set (interpre.hxx:263). `TreatDoubleError()` (interpre.hxx:394) sets error on non-finite double. `div()` returns `errDivisionByZero` coded double (interpre.hxx:955-959). Error tokens propagate through the stack.

**[UNVERIFIED]** Whether all functions propagate errors per ODF 1.4 (some functions like `CHOOSE`/`OFFSET` may suppress). Needs per-function audit.

## 9. Short-circuit evaluation

**[CONFIRMED]** `ScIfJump()` (interpr1.cxx) and `ScChoseJump()` (interpr1.cxx:401) implement jump-based short-circuit for IF and CHOOSE. `JumpMatrix()` (interpr1.cxx:406).

**[UNVERIFIED]** AND/OR short-circuit (ODF 1.4 requires AND/OR to not necessarily evaluate all args if result determined) — AOO `ScAnd`/`ScOr` iterate all parameters (IterateParameters pattern); **[UNVERIFIED]** whether short-circuit is implemented.

## 10. Array evaluation

**[CONFIRMED]** `ScMatrixRef` / `ScMatrix` (scmatrix.hxx/.cxx). `PopMatrix()` (interpre.hxx:350). `CreateMatrixFromDoubleRef` (interpre.hxx:377). `ConvertMatrixParameters()` / `MatrixParameterConversion()` (interpre.hxx:346-349). `ScTableOp()` for multiple operations (interpre.hxx:383). `ScSumProduct` (interpr5.cxx), `ScMatDet/Inv/Mult/Trans` (interpr5.cxx). `bMatrixFormula` flag (interpre.hxx:209). `ScJumpMatrix` for array conditions (interpre.hxx:184).

**[UNVERIFIED]** Array evaluation rules (force-array, broadcasting, dimension matching) per ODF 1.4 not fully traced.

## 11. Date and time serial values

**[CONFIRMED]** `GetDateSerial(nYear,nMonth,nDay,bStrict)` (interpr2.cxx:604). `GetDateValue`/`GetTimeValue`. `DATE` returns `#VALUE!` for dates before 15 October 1582 (odf_docs/ODF14-KNOWN-LIMITATIONS-PUBLIC.md line 74). EASTERSUNDAY uses `GetDateSerial(..., true)` strict (interpr2.cxx:282).

**[ODF 1.4 impl-defined]** §7.4 dates before 1583: AOO `DATE` returns #VALUE! before 15 Oct 1582; YEAR/MONTH/DAY of pre-1583 serial computed proleptic Gregorian with no validity check (odf_docs/ODF14-IMPLEMENTATION-DEFINED-PUBLIC.md line 155). ODF 1.4 requires support only from 1583.

## 12. Complex number behavior

**[CONFIRMED]** COMPLEX/IMABS/IMREAL/IMAGINARY and IM* functions in interpr3.cxx (ScComplex via COMPLEX opcode, plus IMABS etc.). `COMPLEX` maps to... need opcode check. Complex functions present.

**[ODF 1.4 impl-defined]** ISNUMBER/ISTEXT of a complex number impl-defined (odf_docs item 4.3.3). IMARGUMENT(0), IMSUM with no parameters impl-defined (odf_docs item 6.6.1 list).

## 13. Numerical precision

**[CONFIRMED]** `bCalcAsShown` flag (interpre.hxx:208) — "precision as shown" option. `TreatDoubleError` uses `::rtl::math::isFinite`. `div` guards zero.

**[UNVERIFIED]** Beyond-48-bit integer handling impl-defined (odf_docs item 6.6.1). Numerical precision beyond ODF's 15-digit guarantee not audited.

## 14. Locale-sensitive conversion

**[CONFIRMED]** `SvNumberFormatter* pFormatter` (interpre.hxx:187) used for TEXT/VALUE/DATEVALUE/etc. HOST-LOCALE is the configured office locale (§19.646). UPPER/LOWER/PROPER/SEARCH locale-sensitive.

**[ODF 1.4 impl-defined]** §6.3.12/6.3.15/6.3.16 conversions impl-defined. Locale-sensitive conversion of VALUE etc.

## 15. Semantic discrepancies summary

| Area | AOO behavior | ODF 1.4 normative | Status | Source |
|------|-------------|-------------------|--------|--------|
| DATE pre-1583 | #VALUE! before 15 Oct 1582 | support from 1583; earlier impl-defined | Documented impl-defined | KNOWN-LIMITATIONS:74; IMPLEMENTATION-DEFINED:155 |
| CONVERT atm | 101324.996583 Pa | 1.0132510E+5 Pa (rel diff 1e-6) | Discrepancy | KNOWN-LIMITATIONS:72 |
| INDEX row-vector single-arg | returns column | ODF 1.4: error | Discrepancy | KNOWN-LIMITATIONS:71 |
| Trailing empty param `NPER(r;p;pv;;)` | error (except IF) | ODF 1.4 rule | Documented | KNOWN-LIMITATIONS:73 |
| BITAND/BITOR/BITXOR | exactly 2 params | ODF 1.2: 2 params; ODF 1.4: verify variadic | UNVERIFIED | interpr1.cxx:1180 |
| BITLSHIFT/BITRSHIFT negative shift | inverted direction (cxx:1231-1234) | ODF 1.4: shift>=0 or error | Potential bug | interpr1.cxx:1229-1243 |
