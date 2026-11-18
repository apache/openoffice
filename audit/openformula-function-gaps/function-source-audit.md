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

OpenFormula Function Compatibility Gap Audit — Source-Level Findings
=====================================================================

Branch: openformula
Starting commit: 6a2f742073 (CONVERT atm/Torr fix)
PR #523 merge: 1354dfcdad
Audit type: Read-only source inspection (no build, no runtime execution)

This document records source-level findings for 15 Calc functions.
All file paths and line numbers are relative to the repository root
(/home/pedro/projects/aoo).

Updated 2026-11-18 to match the code after the post-merge review. Line
numbers are those of the 2026-11-18 working tree. "Run" means the working
tree was built on macOS and Linux and the case was run; "Excel 16" means it
was also compared with Excel. Sections 6, 12 and 15 were only re-read.

1\. FINDB
--------

ODF 1.4: Defined (§6.7.2 in ODF 1.2 and 1.4: "The same as FIND, but using
  byte positions"). Byte positions are implementation-dependent.
ODF group: not checked.

A. Function registration

- Native opcode: SC_OPCODE_FINDB = 412 (compiler.hrc line 408).
- Parser name: "FINDB" registered in core_resource.src (ODF and
    Excel sections).
- Function-name-to-opcode mapping: ocFindB → FINDB (opcode.hxx
    line 307).
- Evaluator implementation: ScInterpreter::ScFindB() at
    main/sc/source/core/tool/interpr1.cxx:8260. Dispatch via
    `case ocFindB : ScFindB();` at interpr4.cxx:3774.
- Function parameter metadata: Via native function resource.
- Help entry: HID_FUNC_FINDB (helpids.h line 769, hidother.src);
    topic func_findb.xhp.
- ODF formula namespace mapping: FINDB in ODF section of
    core_resource.src.
- Excel import/export mapping: BIFF2 ID 205 (xlformula.cxx line 211),
    added 2026-11-18. xlsx: ODF name FINDB (formulabase.cxx line 442).

B. Actual implementation: ScInterpreter::ScFindB() at interpr1.cxx:8260.
  Implements case-sensitive byte-oriented search using DBCS byte
  semantics (1 byte for ASCII/non-DBCS, 2 bytes for DBCS characters
  per IsDBCS()). Uses lcl_BytePosToCharIndex() and
  lcl_CharIndexToBytePos() helpers to convert between byte positions
  (1-based) and character indices (0-based). Returns byte position
  of match, or #VALUE! (PushNoValue) if not found or start invalid.
  Until 2026-11-18 the two text arguments were popped in the wrong
  order, so FINDB("b";"abc") searched "b" for "abc".

C. Format compatibility: XLS read and written as BIFF 205; an AOO xls
  opens in Excel 16 with the value (run). XLSX read (run, hand-made
  file). AOO has no xlsx export.

D. Documentation: Help topic func_findb.xhp, reachable through its help
  ID since helpcontent2/util/scalc/makefile.mk lists it (2026-11-18).
  Function Wizard entry exists in main/sc/source/ui/src/scfuncs.src
  (SC_OPCODE_FINDB resource block with parameter names and descriptions).

Failure type: None.
Priority: Resolved (was P1).
Confidence: HIGH — run; ASCII cases match Excel 16 (Excel counts CJK as
            1 byte unless a DBCS language is set; AOO always 2).

2\. REPLACEB
-----------

ODF 1.4: Defined (§6.7.6: "As REPLACE, but using byte positions").
ODF group: not checked.

A. Function registration

- Native opcode: SC_OPCODE_REPLACEB = 414 (compiler.hrc line 410).
- Parser name: "REPLACEB" registered in core_resource.src (ODF and
    Excel sections).
- Function-name-to-opcode mapping: ocReplaceB → REPLACEB
    (opcode.hxx line 309).
- Evaluator implementation: ScInterpreter::ScReplaceB() at
    interpr1.cxx:8355. Dispatch via
    `case ocReplaceB : ScReplaceB();` at interpr4.cxx:3776.
- Function parameter metadata: Via native function resource.
- Help entry: HID_FUNC_REPLACEB (helpids.h line 771, hidother.src);
    topic func_replaceb.xhp.
- ODF formula namespace mapping: REPLACEB in ODF section of
    core_resource.src.
- Excel import/export mapping: BIFF2 ID 207 (xlformula.cxx line 213),
    added 2026-11-18. xlsx: formulabase.cxx line 444.

B. Actual implementation: ScInterpreter::ScReplaceB() at interpr1.cxx:8355.
  Implements byte-oriented replacement using DBCS byte semantics.
  Converts start byte position to character index, counts bytes
  forward to determine character count to erase (avoiding splitting
  DBCS characters), then Erase/Insert like ScReplace(). Clamps
  start position to string length + 1 (append at end) and clamps
  count to available bytes. Returns PushIllegalArgument() for invalid
  inputs.

C. Format compatibility: XLS read and written as BIFF 207 (source).
  XLSX read (run).

D. Documentation: Help topic func_replaceb.xhp (help ID resolves since
  2026-11-18). Function Wizard entry exists in scfuncs.src.

Failure type: None.
Priority: Resolved (was P1).
Confidence: HIGH — run; ASCII cases match Excel 16.

3\. SEARCHB
----------

ODF 1.4: Defined (§6.7.8: "As SEARCH, but using byte positions").
ODF group: not checked.

A. Function registration

- Native opcode: SC_OPCODE_SEARCHB = 413 (compiler.hrc line 409).
- Parser name: "SEARCHB" registered in core_resource.src (ODF and
    Excel sections).
- Function-name-to-opcode mapping: ocSearchB → SEARCHB
    (opcode.hxx line 308).
- Evaluator implementation: ScInterpreter::ScSearchB() at
    interpr1.cxx:8302. Dispatch via
    `case ocSearchB : ScSearchB();` at interpr4.cxx:3775.
- Function parameter metadata: Via native function resource.
- Help entry: HID_FUNC_SEARCHB (helpids.h line 770, hidother.src);
    topic func_searchb.xhp.
- ODF formula namespace mapping: SEARCHB in ODF section of
    core_resource.src.
- Excel import/export mapping: BIFF2 ID 206 (xlformula.cxx line 212),
    added 2026-11-18. xlsx: formulabase.cxx line 443.

B. Actual implementation: ScInterpreter::ScSearchB() at interpr1.cxx:8302.
  Implements case-insensitive byte-oriented search using DBCS byte
  semantics, following the same pattern as ScSearch() (utl::TextSearch
  with case-insensitive flag; regular expressions when enabled, no ? or *
  wildcards). Converts byte positions to/from character
  indices using lcl_BytePosToCharIndex() and lcl_CharIndexToBytePos().
  Returns byte position of match, or #VALUE! (PushNoValue) if not found
  or start invalid. The text arguments were popped in the wrong order
  until 2026-11-18, as in FINDB.

C. Format compatibility: XLS read and written as BIFF 206 (source).
  XLSX read (run).

D. Documentation: Help topic func_searchb.xhp (the wildcard claim and its
  example were removed 2026-11-18). Function Wizard entry exists in
  scfuncs.src.

Failure type: None.
Priority: Resolved (was P1).
Confidence: HIGH — run; ASCII cases match Excel 16.

4\. DATEDIF
----------

ODF 1.4: Defined by ODF 1.4 Part 4, Section 6.10.3.
ODF group: Large Group (Excel compatibility function; not rechecked).

A. Function registration

- Native opcode: SC_OPCODE_DATEDIF = 415 (compiler.hrc line 411).
- Parser name: "DATEDIF" registered in core_resource.src (ODF and
    Excel sections).
- Function-name-to-opcode mapping: ocDateDif → DATEDIF (opcode.hxx
    line 310).
- Evaluator implementation: ScInterpreter::ScDateDif() at
    main/sc/source/core/tool/interpr2.cxx:461. Dispatch via
    `case ocDateDif : ScDateDif();` at interpr4.cxx:3777.
- Function parameter metadata: Via native function resource.
- Help entry: HID_FUNC_DATEDIF (helpids.h line 772, hidother.src);
    topic func_datedif.xhp.
- ODF formula namespace mapping: DATEDIF in ODF section of
    core_resource.src.
- Excel import/export mapping: BIFF5/BIFF7 function ID 351 maps to
    ocDateDif both ways (xlformula.cxx line 346; the import-only flag
    was dropped 2026-11-18). xlsx: formulabase.cxx line 671.

B. Actual implementation: ScInterpreter::ScDateDif() at interpr2.cxx:461.
  Implements ODF 1.4 §6.10.3 with all six formats (y, m, d, md, ym, yd).
  Since 2026-11-18 (Jim's decision) it follows Excel where ODF is
  silent: start after end is an error, the format ignores case, "md"
  borrows the month before the end month and "yd" moves the end date into
  the start year. The H.3 version negated reverse intervals, was
  case-sensitive and computed "yd" wrongly.
  Detailed algorithm in audit/openformula-function-gaps/datedif-semantic-design.md.

C. Format compatibility

- XLS import and export: BIFF ID 351 (run; Excel 16 calculates an AOO
    xls).
- XLSX import: read (run).
- XLSX export: AOO has none.

D. Documentation: Help topic func_datedif.xhp (rewritten 2026-11-18).
  Function Wizard entry exists in main/sc/source/ui/src/scfuncs.src.

Failure type: None. One Excel difference ("yd" from Feb 29, 2020 to
  Mar 1, 2023).
Priority: Resolved (was P1).
Confidence: HIGH — run; 630 cases compared with Excel 16.

5\. ISOWEEKNUM
-------------

ODF 1.4: Defined (ODF 1.2 §6.10.11, ODF 1.4 §6.10.12): one argument,
  ISO 8601 week. No Mode parameter in either version.
ODF group: not checked.

A. Function registration

- Native opcode: SC_OPCODE_ISOWEEKNUM = 416 (compiler.hrc line 412),
    added in H.5. WEEKNUM is SC_OPCODE_WEEK.
- Function-name-to-opcode mapping: ocIsoWeekNum → ISOWEEKNUM
    (opcode.hxx line 311; core_resource.src lines 340, 693 and 1635).
    The ODF list also maps ocWeek to "ISOWEEKNUM" (line 339), which is
    how AOO has always written WEEKNUM(d;mode) to ODF; when reading, the
    first entry wins, so FormulaCompiler::Factor (FormulaCompiler.cxx
    line 1083) turns an ODF ISOWEEKNUM with one argument into
    ocIsoWeekNum (2026-11-18). With two arguments it stays WEEKNUM.
- Evaluator implementation: ScIsoWeekNum() at interpr2.cxx:240,
    Date::GetWeekOfYear(MONDAY, 4). WEEKNUM is ScGetWeekOfYear() at
    interpr2.cxx:227 and still takes exactly 2 arguments.
- Help entry: HID_FUNC_ISOWEEKNUM (helpids.h line 506, hidother.src);
    topic func_isoweeknum.xhp.
- Excel mapping: `_xlfn.ISOWEEKNUM` in saFuncTable_Oox (xlformula.cxx
    lines 385-386), added 2026-11-18. WEEKNUM keeps the
    `_xlfnodf.ISOWEEKNUM` entry (line 422).

B. Actual implementation: ISO 8601 (Monday start, week 1 holds the
  first Thursday). The Function Wizard resource had one ExtraData number
  too many; fixed 2026-11-18.

C. Format compatibility

- ODF: one-argument form read and written as ISOWEEKNUM; old files'
    two-argument form read as WEEKNUM (run, .ods round trip).
- XLS: written as `_xlfn.ISOWEEKNUM` and read back; Excel 16 calculates
    it (run).
- XLSX import: `_xlfn.ISOWEEKNUM` gives #NAME? (run).

D. Documentation: func_isoweeknum.xhp, embedded in 04060102.xhp; help
  ID resolves since 2026-11-18.

Failure type: None (xlsx import gap above).
Priority: Resolved (was P1).
Confidence: HIGH — run; 300 dates compared with Excel 16.

6\. DDE
------

ODF 1.4: Not defined by ODF 1.4 Part 4 (not rechecked). Excel-specific /
  application-level.
ODF group: Not applicable.

A. Function registration

- Native opcode: SC_OPCODE_DDE = 369 (compiler.hrc line 365).
- Parser name: "DDE" in core_resource.src lines 345 (ODF) and 698 (Excel).
- Function-name-to-opcode mapping: ocDde → DDE.
- Evaluator implementation: ScDde() at interpr2.cxx:2247.
    Takes 3–4 params: Item, Topic, Appl, [Mode].
- Function parameter metadata: Via native function resource.
- Help entry: HID_FUNC_DDE (hidother.src:330, helpids.h:738).
- ODF formula namespace mapping: DDE in both ODF and Excel sections.
- Excel import mapping: Not in xlformula.cxx BIFF tables.
- Excel export mapping: None.

B. Actual implementation: ScDde() (interpr2.cxx:2247) creates or
  finds an ScDdeLink via sfx2::LinkManager. Requires a document shell
  with a LinkManager (fails with PushNoValue() for temporary documents
  without DocShell). Sets recalc mode on load. Disables idle during
  evaluation. DDE is platform-dependent (implemented via
  sfx2/source/appl/impldde.cxx, appdde.cxx). DDE links can be created
  new (not just opening existing links). Updates are driven by the
  DDE link mechanism, not automatic per-recalc. External application
  failures are handled via error codes. Security: no user confirmation
  visible in source; DDE is disabled in server mode
  (appdde.cxx:494, 526: "Im Serverbetrieb ist DDE abgeklemmt!").

C. Format compatibility: DDE is not a file-format function per se;
  no XLS/XLSX import/export mapping for the DDE function itself.
  DDE links (external references) are handled separately in Excel
  filters (xilink.cxx, xelink.cxx, xehelper.cxx).

D. Documentation: HID_FUNC_DDE help entry exists.

Failure type: Platform-dependent application-level feature.
  Not a pure formula function; requires DDE subsystem availability.
Priority: DEFER — requires DDE service platform support; security
          and platform-appropriateness questions unresolved.
Confidence: HIGH — source evidence clear; implementation exists but
            is platform-dependent.

7\. ODDFPRICE
------------

ODF 1.4: Defined (§6.12.31 in ODF 1.2 and 1.4). The spec gives no formula.
ODF group: not checked.

A. Function registration

- Native opcode: NONE. Not a native Calc opcode (H.6.1 added one and
    f4f3de4f92 reverted it).
- Parser name: "ODDFPRICE" registered as AddIn in
    odffmap.cxx line 127:
      { "ODDFPRICE", "ODDFPRICE", false,
        "com.sun.star.sheet.addin.Analysis.getOddfprice",
        "COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETODDFPRICE" }
- Function-name-to-opcode mapping: Via AddIn map, not native opcode.
- Evaluator implementation: AnalysisAddIn::getOddfprice() at
    scaddins/source/analysis/financial.cxx:404. Validates
    fRate>=0, fYield>=0, fRedemp>0, frequency, nMat>nFirstCoup>nSettle>nIssue.
    Delegates to GetOddfprice() (analysishelper.cxx:1038).
- Function parameter metadata: Via AddIn collection (ScUnoAddInCollection).
- Help entry: HID_AAI_FUNC_ODDFPRICE (hidother.src:414, helpids.h:318);
    English topic in 04060118.xhp.
- ODF formula namespace mapping: None (AddIn, not ODF namespace).
- Excel import mapping: Not in xlformula.cxx BIFF tables.
- Excel export mapping: None.

B. Actual implementation: on trunk GetOddfprice() threw (THROW_RTE,
  #87380#). 8ce50c3485 (message "No production code changes") brought in
  an unfinished version: the long-period terms were multiplied by 0 and a
  placeholder line did not compile. Jim chose to finish it; the
  2026-11-18 version:

- Requires the first coupon to be on the maturity's coupon schedule
    (lcl_GetCouppcd), as Excel does (#NUM! otherwise).
- Short first period (DFC < E): Excel's short formula, DSC, A and DFC
    counted with ScaDate::getDiff().
- Long first period: Excel's long formula, summed per quasi-coupon
    period; a whole quasi-coupon period inside the odd period counts as 1,
    the quasi-coupon dates are stepped back by plain months, and Nq follows
    Excel's count for month-end first coupons
    (lcl_GetOddfQuasiPeriods()).
  These rules were found by comparing with Excel 16, not taken from a
  spec.

C. Format compatibility: AddIn function; not part of BIFF/OOXML
  function tables. Import/export via AddIn mechanism only.

D. Documentation: Help entry exists (AAI = Analysis AddIn).

Failure type: RESOLVED 2026-11-18 (was: throws unconditionally on trunk).
  4524 random ODDFPRICE/ODDFYIELD cases compared with Excel 16: 14
  differences in 4 classes (DFC equal to E where Excel gives #NUM!: 4;
  annual 30/360 US bonds with Feb-end coupons, 2/360 of a coupon: 5;
  Excel's yield less exact: 4; Excel's yield wrong: 1).
Priority: P2 (native opcode deferred).
Confidence: HIGH — run, Excel 16.

8\. ODDFYIELD
------------

ODF 1.4: Defined (§6.12.32 in ODF 1.2 and 1.4).
ODF group: not checked.

A. Function registration

- Native opcode: NONE.
- Parser name: "ODDFYIELD" registered as AddIn in odffmap.cxx line 128:
      { "ODDFYIELD", "ODDFYIELD", false,
        "com.sun.star.sheet.addin.Analysis.getOddfyield", ... }
- Evaluator implementation: AnalysisAddIn::getOddfyield() at
    financial.cxx:416. Validates fRate>=0, fPrice>0, fRedemp>0 (added
    2026-11-18), frequency, nMat>nFirstCoup>nSettle>nIssue. Delegates to
    GetOddfyield() (analysishelper.cxx:1177).
- Help entry: HID_AAI_FUNC_ODDFYIELD (hidother.src:415, helpids.h:319).

B. Actual Implementation: 2026-11-18. The price falls as the yield
  rises, so GetOddfyield() brackets the yield (above -frequency) and
  narrows it with the Illinois variant of regula falsi, stopping when the
  price is within 1e-12 relative. No bracket or no convergence: #NUM!.

C. Format compatibility: AddIn only; no BIFF/OOXML table mapping.

D. Documentation: Help entry exists.

Failure type: RESOLVED 2026-11-18 (was: throws unconditionally on trunk).
  See ODDFPRICE for the Excel comparison; where the yields differ,
  AOO's reproduces the price within 1e-10 in Excel's own ODDFPRICE,
  Excel's within 1.8e-7.
Priority: P2 (native opcode deferred).
Confidence: HIGH — run, Excel 16.

9\. RRI
------

ODF 1.4: Defined (§6.12.44 in ODF 1.2 and 1.4; constraint N > 0).
ODF group: not checked.

A. Function registration

- Native opcode: SC_OPCODE_ZGZ = 256 (compiler.hrc line 252).
- Parser names: "RRI" in the ODF and English lists since 2026-11-18
    (core_resource.src lines 216 and 569; trunk had "ZGZ" there, so
    older ODF files hold of:ZGZ). "ZGZ" is still read through
    OpCodeMap::putOpCodeAlias() (FormulaCompiler.cxx line 695), which adds
    the name for input only. The UI name is RRI in en-US and ZGZ in
    German (translation keyed on SC_OPCODE_ZGZ).
- The RRI commit (6fefc3f19c) had renamed ocZGZ, SC_OPCODE_ZGZ, ScZGZ()
    and HID_FUNC_ZGZ and added the alias with putOpCode(), which also
    replaced the name AOO writes; both were undone 2026-11-18.
- Excel mapping: saFuncTable_Odf line 428, `_xlfnodf.RRI`.
- Evaluator implementation: ScZGZ() at interpr2.cxx:1295.
    Takes exactly 3 params: periods, present value, future value.
    Computes pow(Fv/Pv, 1/N) - 1. Format: percent.
- Help entry: HID_FUNC_ZGZ (hidother.src:136, helpids.h:528); English
    RRI topic in 04060118.xhp.

B. Actual implementation: ScZGZ() computes pow(Fv/Pv, 1/N) - 1 and
  checks nothing; ODF requires N > 0. 8ce50c3485 reclassified the earlier
  validation concern as a test-data error. The formula itself is right:
  RRI(10;100;200) = 0.0717734625362931, as Excel 16.

C. Format compatibility

- ODF: of:RRI written; of:ZGZ and of:RRI read (run).
- XLS: `_xlfnodf.RRI`; AOO reads it back, Excel shows #NAME? (run).
- XLSX import: `_xlfn.RRI` is read (run, 2026-11-18).

D. Documentation: English help (04060118.xhp, unchanged from trunk).

Failure type: No N > 0 check; xls/xlsx naming.
Priority: P3.
Confidence: HIGH — run.

10\. ERROR.TYPE
--------------

ODF 1.4: Defined (§6.13.11 in ODF 1.2 and 1.4). Not propagating; a
  non-error gives an error; #NULL!..#N/A are 1..7.
ODF group: Verify against final spec (likely Large Group).

A. Function registration

- Native opcode: SC_OPCODE_ERROR_TYPE_ODF = 419 (compiler.hrc line
    415), ocErrorTypeODF (opcode.hxx line 177), since 2026-11-18. The
    legacy SC_OPCODE_ERROR_TYPE = 151 (line 182) stays ERRORTYPE.
- Parser names:
  - ERROR.TYPE: "ERROR.TYPE" in ODF, English and UI lists
      (core_resource.src lines 147, 500, 1129)
  - ERRORTYPE: "ORG.OPENOFFICE.ERRORTYPE" (ODF, line 146), "ERRORTYPE"
      (English, line 499; UI, line 1125)
- Evaluator implementation: ScErrorType( bool bODF ) at
    interpr1.cxx:8703. Both opcodes are in CASE_OCERRFUNC
    (interpr4.cxx:3863-3864), so an error argument reaches them.
- Help entry: HID_FUNC_ERROR_TYPE_ODF (helpids.h:736, hidother.src:328),
    topic func_errortype.xhp embedded in 04060109.xhp next to ERRORTYPE
    (HID_FUNC_FEHLERTYP). H.4's topic had reused HID_FUNC_FEHLERTYP and
    was embedded in the date functions page.
- Excel mapping: BIFF4 ID 261 (xlformula.cxx lines 260-261): import to
    ocErrorTypeODF, export from both. xlsx: ERROR.TYPE (formulabase.cxx
    line 486).

B. Actual implementation: ScErrorType() examines the stack type:

- svRefList: gets error code from first cell in ref list;
    errNoRef if empty, errNoValue if >1 ref.
- svDoubleRef/svSingleRef: gets cell error code via pDok->GetErrCode().
- default: PopError(), pushes error number.
  If no error (nErr == 0), pushes NA (PushNA()).
  For ERROR.TYPE, lcl_ErrorTypeODF() (interpr1.cxx:8680) maps the code as
  XclTools::GetXclErrorCode does: errNoCode 1; errDivisionByZero 2;
  errNoValue, errIllegalArgument, errIllegalParameter, errPairExpected,
  errOperatorExpected, errVariableExpected, errParameterExpected,
  errCircularReference 3; errNoRef 4; errNoName, errNoAddin, errNoMacro 5;
  errIllegalFPOperation 6; everything else 7.
  ERROR.TYPE is in the multi-argument opcode range, so it checks for exactly
  one argument itself (MustHaveParamCount); `=ERROR.TYPE()` is an error
  (calcval row ERROR.TYPE.args, run).
  H.4 had changed ScErrorType() itself, so ERRORTYPE returned 1-7 and
  lost the internal codes its help documents (Err:518 gives 518); Jim's
  decision: separate function, ERRORTYPE as on trunk.

C. Format compatibility: xls ERROR.TYPE reads as ERROR.TYPE; ERRORTYPE
  is written as ERROR.TYPE and comes back as ERROR.TYPE (run). xlsx
  ERROR.TYPE read (run).

D. Documentation: English topics for both functions.

Failure type: None. ERROR.TYPE(SQRT(-1)) is 3 in AOO (Err:502) and 6 in
  Excel.
Priority: Resolved (was P1).
Confidence: HIGH — run, Excel 16.

11\. NUMBERVALUE
---------------

ODF 1.4: Defined (§6.13.28 in ODF 1.2 and 1.4).
ODF group: Verify against final spec (likely Large Group).

A. Function registration

- Native opcode: SC_OPCODE_NUMBERVALUE = 392 (compiler.hrc line 388).
- Parser name: "NUMBERVALUE" in core_resource.src lines 356 (ODF)
    and 709 (Excel).
- Function-name-to-opcode mapping: ocNumberValue → NUMBERVALUE.
- Evaluator implementation: ScInterpreter::ScNumberValue() at
    main/sc/source/core/tool/interpr1.cxx:3102 (moved from interpr4.cxx
    2026-11-18). Dispatch via `case ocNumberValue : ScNumberValue();` at
    interpr4.cxx:3558.
- Function parameter metadata: Function Wizard resource with 3
    parameters, 2 and 3 optional (the H.1 block had one flag too few).
- Help entry: HID_FUNC_NUMBERVALUE (hidother.src:378, helpids.h:784);
    topic func_numbervalue.xhp.
- Excel mapping: saFuncTable_Odf line 424:
      EXC_FUNCENTRY_ODF( ocNumberValue, 1, 3, 0, "NUMBERVALUE" )
    (was 2, 2).

B. Actual implementation: the ODF steps in order: group separators
  removed only before the first decimal separator, that separator turned
  into '.', whitespace removed, trailing percent signs stripped (each
  divides by 100), and the rest must be a plain xsd:float without INF/NaN
  (lcl_IsPlainFloat(), interpr1.cxx:3072). LEN(DecimalSeparator) must be 1
  and it must not occur in GroupSeparator; omitted separators come from
  the locale, and the locale group separator is dropped when it equals
  the given decimal separator. The H.1 version removed group separators
  everywhere and replaced every decimal separator.

C. Format compatibility

- XLS: `_xlfnodf.NUMBERVALUE`; AOO reads it back, Excel shows #NAME? (run).
- XLSX import: `_xlfn.NUMBERVALUE` is read (run, 2026-11-18).

D. Documentation: func_numbervalue.xhp (rules added 2026-11-18).

Failure type: None. Differences from Excel 16 on purpose: empty text is
  an error (Excel 0), an empty GroupSeparator is accepted (Excel error),
  a two-character DecimalSeparator is an error (Excel uses the first).
Priority: Resolved (was P1).
Confidence: HIGH — run, Excel 16.

12\. GETPIVOTDATA
----------------

ODF 1.4: Not defined by ODF 1.4 Part 4. Excel-specific function.
ODF group: Not applicable.

A. Function registration

- Native opcode: SC_OPCODE_GET_PIVOT_DATA = 390 (compiler.hrc line 386).
- Parser name: "GETPIVOTDATA" in core_resource.src lines 354 (ODF)
    and 707 (Excel).
- Function-name-to-opcode mapping: ocGetPivotData → GETPIVOTDATA.
- Evaluator implementation: ScGetPivotData() at interpr2.cxx:3069.
    Takes 2–30 params (must be even count). Supports old syntax
    (constraint string) and new syntax (field/item pairs). Uses
    pDok->GetDPAtBlock() to find the DataPilot object.
- Function parameter metadata: Via native function resource.
- Help entry: HID_FUNC_GETPIVOTDATA (hidother.src:334, helpids.h:742).
- ODF formula namespace mapping: GETPIVOTDATA in both sections.
- Excel import mapping: saFuncTable_8 line 355 (BIFF8):
      { ocGetPivotData, 358, 2, MX, V, { RR, RR, VR }, 0, 0 }
- Excel export mapping: BIFF8 export (same table entry, no importonly/exportonly flag).

B. Actual implementation: ScGetPivotData() parses target field name
  and field/item filter pairs, locates the DataPilot table via
  ScDPObject, calls GetPivotData(). Comment at interpr2.cxx:3035–3036
  notes: "MS Excel docs claim to use the 'most recent' which is not
  exactly the same as what we do in ScDocument::GetDPAtBlock." (not
  re-read 2026-11-18) The
  function depends entirely on the DataPilot subsystem
  (dptablecache.cxx:517 mentions GETPIVOTDATA disabling nested
  formula interpretation).

C. Format compatibility

- XLS import: BIFF8 (function ID 358), recognized and mapped.
- XLSX import: oox/source/xls/formulabase.cxx line 692 has an
    import-only GETPIVOTDATA row (xlsx import does not use
    saFuncTable_Oox, which is xls-only). Not run.
- XLS export: BIFF8 export supported.
- XLSX export: AOO has no xlsx export.

D. Documentation: Help entry exists.

Failure type: Implementation present. DataPilot dependency makes it
  application-level.
Priority: P3.
Confidence: HIGH — source evidence clear; runtime unverified.

13\. FDIST
---------

ODF 1.4: ODF FDIST (§6.18.22) is the left tail with optional Cumulative
  (default TRUE; 0 for x < 0; density error only at x = 0, r1 = 1).
  LEGACY.FDIST (§6.18.23) is the right tail, three arguments.
ODF group: Not applicable.

A. Function registration

- AOO FDIST = ODF LEGACY.FDIST: SC_OPCODE_F_DIST = 315 (compiler.hrc
    line 311), ocFDist, ScFDist() at interpr3.cxx:1588, three arguments,
    unchanged from trunk. Names: "LEGACY.FDIST" (ODF, core_resource.src
    line 287), "FDIST" (English line 640, UI line 1695). xls BIFF4 281.
    Help HID_FUNC_FVERT.
- AOO F.DIST = ODF FDIST, since 2026-11-18: SC_OPCODE_F_DIST_LEFT = 417
    (compiler.hrc line 413), ocFDistLeft (opcode.hxx line 372),
    ScFDistLeft() at interpr3.cxx:1604. Names: "FDIST" (ODF, line 288),
    "F.DIST" (English line 641, UI line 1699). xls: saFuncTable_Oox
    lines 385-386, `_xlfn.F.DIST`; `_xlfnodf.FDIST` imports as FDIST
    (line 421). Help HID_FUNC_F_DIST_LEFT (helpids.h:689,
    hidother.src:285), F.DIST section in 04060182.xhp.

B. History: H.8/H.8.1 changed ocFDist itself into the left tail (with a
  4th argument popped after the others) and added ocLegacyFDist. That
  changed FDIST typed in every UI language, in macros and in .sxc files,
  and f4f3de4f92 then dropped the SC_OPCODE_LEGACY_F_* defines, so the
  branch did not build. Jim's decision: keep FDIST, add F.DIST.
  ScFDistLeft() computes the left tail with GetBetaDist() at
  r1*x/(r1*x+r2) and the density with GetBetaDistPDF().

C. Format compatibility

- ODF: of:FDIST ↔ F.DIST, of:LEGACY.FDIST ↔ FDIST (run, round trip).
- XLS: FDIST as 281 (run); F.DIST as `_xlfn.F.DIST`, Excel calculates the 4-argument form (run); `_xlfnodf.FDIST` from an older xls reads as FDIST (run).
- XLSX: FDIST (281) read (run); `_xlfn.F.DIST` is read (run, 2026-11-18).

D. Documentation: FDIST help as on trunk; F.DIST section added.

Failure type: None.
Priority: Resolved.
Confidence: HIGH — run; F.DIST and FDIST match Excel 16 on 300 and 263
            random cases.

14\. FINV
--------

ODF 1.4: ODF FINV (§6.18.24) inverts FDIST (left tail); LEGACY.FINV
  (§6.18.25) inverts LEGACY.FDIST.
ODF group: Not applicable.

A. Function registration

- AOO FINV = ODF LEGACY.FINV: SC_OPCODE_F_INV = 359 (compiler.hrc line
    355), ScFInv() at interpr3.cxx:2052, unchanged. Names "LEGACY.FINV"
    (ODF, line 332), "FINV" (English line 685, UI line 1879). xls BIFF4
    282.
- AOO F.INV = ODF FINV, since 2026-11-18: SC_OPCODE_F_INV_LEFT = 418
    (compiler.hrc line 414), ocFInvLeft, ScFInvLeft() at
    interpr3.cxx:2074, using ScFDistLeftFunction + lcl_IterateInverse.
    Names "FINV" (ODF, line 333), "F.INV" (English line 686, UI line
    1883). xls `_xlfnodf.FINV` (saFuncTable_Odf line 417). Help
    HID_FUNC_F_INV_LEFT, F.INV section in 04060182.xhp.

B. Actual implementation: F.INV accepts 0 <= p < 1 (0 gives 0).

C. Format compatibility: as FDIST/F.DIST.

D. Documentation: FINV help as on trunk; F.INV section added.

Failure type: None.
Priority: Resolved.
Confidence: HIGH — run; FINV matches Excel 16 on 300 random cases, F.INV
            on 298 (the other two within 3e-10 at p = 0.999, r2 = 1).

15\. HARMEAN
-----------

ODF 1.4: Defined by ODF 1.4 Part 4 (statistical function).
ODF group: Verify against final spec (likely Large Group).

A. Function registration

- Native opcode: SC_OPCODE_HAR_MEAN = 321 (compiler.hrc line 317).
- Parser name: "HARMEAN" in core_resource.src lines 294 (ODF)
    and 647 (Excel).
- Function-name-to-opcode mapping: ocHarMean → HARMEAN.
- Evaluator implementation: ScHarMean() at interpr3.cxx:2637.
    Iterates over params, accumulates 1/x for x>0, counts values.
    Sets errIllegalArgument for x<=0. Skips empty cells.
- Function parameter metadata: Via native function resource.
- Help entry: HID_FUNC_HARMITTEL (hidother.src:248, helpids.h:652).
    The help ID name is German; the help text (04060182.xhp) is English.
- ODF formula namespace mapping: HARMEAN in both sections.
- Excel import/export mapping: BIFF4 ID 320 both ways
    (xlformula.cxx line 313, saFuncTable_4); xlsx 320
    (formulabase.cxx line 538).

B. Actual implementation: ScHarMean() implements H = n / Σ(1/xᵢ)
  counting only positive values. Zero or negative values trigger
  errIllegalArgument. Text values and empty cells are skipped
  (handled by the switch's default case / HasCellValueData check).
  Error values propagate via nGlobalError.

C. Format compatibility: BIFF 320 and xlsx (source; not run).

D. Documentation: English help in 04060182.xhp.

Failure type: None found.
Priority: P3.
Confidence: HIGH — source.

Cross-Cutting Observation: LENB/RIGHTB/LEFTB/MIDB Opcodes
----------------------------------------------------------

compiler.hrc defines SC_OPCODE_LENB=400, SC_OPCODE_RIGHTB=401,
SC_OPCODE_LEFTB=402, SC_OPCODE_MIDB=403, and since H.2
SC_OPCODE_FINDB=412, SC_OPCODE_SEARCHB=413, SC_OPCODE_REPLACEB=414.
All seven map to Excel's BIFF IDs 205-211.
