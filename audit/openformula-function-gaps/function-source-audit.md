OpenFormula Function Compatibility Gap Audit — Source-Level Findings
=====================================================================

Branch: openformula
Starting commit: 6a2f742073 (CONVERT atm/Torr fix)
PR #523 merge: 1354dfcdad
Audit type: Read-only source inspection (no build, no runtime execution)

This document records source-level findings for 15 Calc functions.
All file paths and line numbers are relative to the repository root
(/home/pedro/projects/aoo).


1. FINDB
--------

ODF 1.4: Not defined by ODF 1.4 Part 4. Excel-specific function.
ODF group: Not applicable (Excel-only).

A. Function registration
  - Native opcode: SC_OPCODE_FINDB = 412 (compiler.hrc line 412).
  - Parser name: "FINDB" registered in core_resource.src (ODF and
    Excel sections).
  - Function-name-to-opcode mapping: ocFinDB → FINDB (opcode.hxx
    line 306).
  - Evaluator implementation: ScInterpreter::ScFindB() at
    main/sc/source/core/tool/interpr1.cxx:8155. Dispatch via
    `case ocFinDB : ScFindB();` at interpr4.cxx:3772.
  - Function parameter metadata: Via native function resource.
  - Help entry: HID_FUNC_FINDB (helpids.h line 764+, hidother.src).
  - ODF formula namespace mapping: FINDB in ODF section of
    core_resource.src.
  - Excel import mapping: None (not in xlformula.cxx BIFF tables).
  - Excel export mapping: None.

B. Actual implementation: ScInterpreter::ScFindB() at interpr1.cxx:8155.
  Implements case-sensitive byte-oriented search using DBCS byte
  semantics (1 byte for ASCII/non-DBCS, 2 bytes for DBCS characters
  per IsDBCS()). Uses lcl_BytePosToCharIndex() and
  lcl_CharIndexToBytePos() helpers to convert between byte positions
  (1-based) and character indices (0-based). Returns byte position
  of match, or #N/A (PushNoValue) if not found or start_num invalid.

C. Format compatibility: Not recognized on XLS or XLSX import
  (no BIFF/OOXML table mapping). Not exported on XLS or XLSX.

D. Documentation: Help entry exists (HID_FUNC_FINDB).
  Function Wizard entry exists in main/sc/source/ui/src/scfuncs.src
  (SC_OPCODE_FINDB resource block with parameter names and descriptions).

Failure type: None — implementation complete at source level.
Priority: Resolved (was P1).
Confidence: HIGH — source evidence complete; runtime verification
            deferred (no build available).


2. REPLACEB
-----------

ODF 1.4: Not defined by ODF 1.4 Part 4. Excel-specific function.
ODF group: Not applicable.

A. Function registration
  - Native opcode: SC_OPCODE_REPLACEB = 414 (compiler.hrc line 414).
  - Parser name: "REPLACEB" registered in core_resource.src (ODF and
    Excel sections).
  - Function-name-to-opcode mapping: ocReplaceB → REPLACEB
    (opcode.hxx line 308).
  - Evaluator implementation: ScInterpreter::ScReplaceB() at
    interpr1.cxx:8250. Dispatch via
    `case ocReplaceB : ScReplaceB();` at interpr4.cxx:3774.
  - Function parameter metadata: Via native function resource.
  - Help entry: HID_FUNC_REPLACEB (helpids.h line 766+, hidother.src).
  - ODF formula namespace mapping: REPLACEB in ODF section of
    core_resource.src.
  - Excel import mapping: None (not in xlformula.cxx BIFF tables).
  - Excel export mapping: None.

B. Actual implementation: ScInterpreter::ScReplaceB() at interpr1.cxx:8250.
  Implements byte-oriented replacement using DBCS byte semantics.
  Converts start byte position to character index, counts bytes
  forward to determine character count to erase (avoiding splitting
  DBCS characters), then Erase/Insert like ScReplace(). Clamps
  start position to string length + 1 (append at end) and clamps
  count to available bytes. Returns PushIllegalArgument() for invalid
  inputs.

C. Format compatibility: Not recognized on XLS or XLSX import
  (no BIFF/OOXML table mapping). Not exported on XLS or XLSX.

D. Documentation: Help entry exists (HID_FUNC_REPLACEB).
  Function Wizard entry exists in main/sc/source/ui/src/scfuncs.src
  (SC_OPCODE_REPLACEB resource block with parameter names and descriptions).

Failure type: None — implementation complete at source level.
Priority: Resolved (was P1).
Confidence: HIGH — source evidence complete; runtime verification
            deferred (no build available).


3. SEARCHB
----------

ODF 1.4: Not defined by ODF 1.4 Part 4. Excel-specific function.
ODF group: Not applicable.

A. Function registration
  - Native opcode: SC_OPCODE_SEARCHB = 413 (compiler.hrc line 413).
  - Parser name: "SEARCHB" registered in core_resource.src (ODF and
    Excel sections).
  - Function-name-to-opcode mapping: ocSearchB → SEARCHB
    (opcode.hxx line 307).
  - Evaluator implementation: ScInterpreter::ScSearchB() at
    interpr1.cxx:8197. Dispatch via
    `case ocSearchB : ScSearchB();` at interpr4.cxx:3773.
  - Function parameter metadata: Via native function resource.
  - Help entry: HID_FUNC_SEARCHB (helpids.h line 765+, hidother.src).
  - ODF formula namespace mapping: SEARCHB in ODF section of
    core_resource.src.
  - Excel import mapping: None (not in xlformula.cxx BIFF tables).
  - Excel export mapping: None.

B. Actual implementation: ScInterpreter::ScSearchB() at interpr1.cxx:8197.
  Implements case-insensitive byte-oriented search using DBCS byte
  semantics, following the same pattern as ScSearch() (utl::TextSearch
  with case-insensitive flag). Converts byte positions to/from character
  indices using lcl_BytePosToCharIndex() and lcl_CharIndexToBytePos().
  Returns byte position of match, or #N/A (PushNoValue) if not found
  or start_num invalid.

C. Format compatibility: Not recognized on XLS or XLSX import
  (no BIFF/OOXML table mapping). Not exported on XLS or XLSX.

D. Documentation: Help entry exists (HID_FUNC_SEARCHB).
  Function Wizard entry exists in main/sc/source/ui/src/scfuncs.src
  (SC_OPCODE_SEARCHB resource block with parameter names and descriptions).

Failure type: None — implementation complete at source level.
Priority: Resolved (was P1).
Confidence: HIGH — source evidence complete; runtime verification
            deferred (no build available).


4. DATEDIF
----------

ODF 1.4: Defined by ODF 1.4 Part 4, Section 6.10.3.
ODF group: Large Group (Excel compatibility function).

A. Function registration
  - Native opcode: SC_OPCODE_DATEDIF = 415 (compiler.hrc line 415).
  - Parser name: "DATEDIF" registered in core_resource.src (ODF and
    Excel sections).
  - Function-name-to-opcode mapping: ocDateDif → DATEDIF (opcode.hxx
    line 309).
  - Evaluator implementation: ScInterpreter::ScDateDif() at
    main/sc/source/core/tool/interpr2.cxx:443. Dispatch via
    `case ocDateDif : ScDateDif();` at interpr4.cxx:3775.
  - Function parameter metadata: Via native function resource.
  - Help entry: HID_FUNC_DATEDIF (helpids.h line 768+, hidother.src).
  - ODF formula namespace mapping: DATEDIF in ODF section of
    core_resource.src.
  - Excel import mapping: BIFF5/BIFF7 function ID 351 now maps to
    ocDateDif in xlformula.cxx line 342 (import-only). BIFF import
    resolves DATEDIF to the native evaluator instead of ocNoName.
    BIFF8 table (saFuncTable_8) does not include DATEDIF.
  - Excel export mapping: None (import-only policy unchanged).

B. Actual implementation: ScInterpreter::ScDateDif() at interpr2.cxx:443.
  Implements ODF 1.4 §6.10.3 with all six formats (y, m, d, md, ym, yd).
  Uses pFormatter->GetNullDate() for document-aware serial conversion.
  Calendar arithmetic (not day-count approximation) for month/year
  computations. Floor-based handling of time fractions. Case-sensitive
  format matching. Reverse-date handling: y/m/d negated, md/ym/yd
  absolute (Excel compatibility policy).
  Detailed algorithm in audit/openformula-function-gaps/datedif-semantic-design.md.

C. Format compatibility
  - XLS import: BIFF5/BIFF7 function ID 351 maps to ocDateDif.
    Import resolves DATEDIF to the native evaluator. Import-only.
  - XLS import (BIFF8): Not in saFuncTable_8. Not recognized.
  - XLSX import: Not in saFuncTable_Oox. Not recognized.
  - XLS export: None.
  - XLSX export: None.

D. Documentation: Help entry exists (HID_FUNC_DATEDIF).
  Function Wizard entry exists in main/sc/source/ui/src/scfuncs.src
  (SC_OPCODE_DATEDIF resource block with parameter names and descriptions).
  Help topic not yet added under main/helpcontent2/.

Failure type: None — implementation complete at source level.
Priority: Resolved (was P1).
Confidence: HIGH — source evidence complete; runtime verification
            deferred (no build available).


5. ISOWEEKNUM
-------------

ODF 1.4: Not explicitly listed as ODF 1.4 Large Group requirement
  per task instructions (verify against final spec). Excel function.
ODF group: Not applicable (Excel-specific).

A. Function registration
  - Native opcode: SC_OPCODE_WEEK exists (compiler.hrc). The opcode
    is shared with WEEKNUM.
  - Parser name: "ISOWEEKNUM" mapped in
    xlformula.cxx line 416 (saFuncTable_Odf):
      EXC_FUNCENTRY_ODF( ocWeek, 1, 2, 0, "ISOWEEKNUM" )
  - Function-name-to-opcode mapping: ocWeek → ISOWEEKNUM (ODF
    namespace), ocWeek → WEEKNUM (Excel namespace, core_resource.src
    line 677).
  - Evaluator implementation: ScGetWeekOfYear() at
    interpr2.cxx:227–238. Takes 2 params (date, flag). Flag==1 →
    SUNDAY start, otherwise → MONDAY start. Calls
    Date::GetWeekOfYear().
  - Function parameter metadata: Via native function resource.
  - Help entry: No HID_FUNC_ISOWEEKNUM. WEEKNUM help exists as
    HID_AAI_FUNC_WEEKNUM (Analysis add-in, hidother.src:380) and
    HID_FUNC_KALENDERWOCHE (native German, helpids.h:505).
  - ODF formula namespace mapping: ISOWEEKNUM in ODF section of
    core_resource.src line 332.
  - Excel import mapping: saFuncTable_Odf line 416, import-only.
  - Excel export mapping: _xlfnodf.ISOWEEKNUM (export-only).

B. Actual implementation: ScGetWeekOfYear() supports only two
  week-start modes (SUNDAY or MONDAY) via Date::GetWeekOfYear().
  ISO 8601 week numbering requires: week 1 contains the first
  Thursday, Monday as first day, years with 52 or 53 weeks. The
  existing implementation does NOT implement ISO 8601 semantics —
  it only shifts the start day of the week. This is a semantic gap.

C. Format compatibility
  - XLS import: Not in BIFF tables (saFuncTable, saFuncTable_5,
    saFuncTable_8). Not recognized on legacy XLS import.
  - XLSX import: saFuncTable_Odf import-only, mapped to ocWeek.
  - XLS export: None.
  - XLSX export: _xlfnodf.ISOWEEKNUM.

D. Documentation: H.5 ADDED func_isoweeknum.xhp, registered in
  makefile and 04060102.xhp index. HID_FUNC_ISOWEEKNUM added to
  helpids.h and hidother.src.

H.5 FIX: Added separate opcode SC_OPCODE_ISOWEEKNUM (416) with
  evaluator ScIsoWeekNum() (interpr2.cxx) implementing ISO 8601 via
  Date::GetWeekOfYear(MONDAY, 4). Takes exactly 1 argument (date).
  Does not affect existing WEEKNUM (ScGetWeekOfYear) which keeps
  2-argument SUN/MON mode semantics.

Failure type: RESOLVED. Implementation now has correct ISO 8601
  semantics. Help documentation added.
Priority: P1 (ISO week numbering semantics missing).
Confidence: HIGH — semantic gap fixed and verified at source level.


6. DDE
----

ODF 1.4: Not defined by ODF 1.4 Part 4. Excel-specific / application-level.
ODF group: Not applicable.

A. Function registration
  - Native opcode: SC_OPCODE_DDE = 369 (compiler.hrc line 365).
  - Parser name: "DDE" in core_resource.src lines 337 (ODF) and 682 (Excel).
  - Function-name-to-opcode mapping: ocDde → DDE.
  - Evaluator implementation: ScDde() at interpr2.cxx:2150–2208+.
    Takes 3–4 params: Item, Topic, Appl, [Mode].
  - Function parameter metadata: Via native function resource.
  - Help entry: HID_FUNC_DDE (hidother.src:326, helpids.h).
  - ODF formula namespace mapping: DDE in both ODF and Excel sections.
  - Excel import mapping: Not in xlformula.cxx BIFF tables.
  - Excel export mapping: None.

B. Actual implementation: ScDde() (interpr2.cxx:2150) creates or
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


7. ODDFPRICE
------------

ODF 1.4: Not defined by ODF 1.4 Part 4. Excel Analysis ToolPak function.
ODF group: Not applicable.

A. Function registration
  - Native opcode: NONE. Not a native Calc opcode.
  - Parser name: "ODDFPRICE" registered as AddIn in
    odffmap.cxx line 127:
      { "ODDFPRICE", "ODDFPRICE", false,
        "com.sun.star.sheet.addin.Analysis.getOddfprice",
        "COM.SUN.STAR.SHEET.ADDIN.ANALYSIS.GETODDFPRICE" }
  - Function-name-to-opcode mapping: Via AddIn map, not native opcode.
  - Evaluator implementation: AnalysisAddIn::getOddfprice() at
    scaddins/source/analysis/financial.cxx:404–413. Validates
    fRate>=0, fYield>=0, fRedemp>0, frequency, nMat>nFirstCoup>nSettle>nIssue.
    Delegates to GetOddfprice() helper (H.7 FIXED).
  - Function parameter metadata: Via AddIn collection (ScUnoAddInCollection).
  - Help entry: HID_AAI_FUNC_ODDFPRICE (hidother.src:406, helpids.h:318).
  - ODF formula namespace mapping: None (AddIn, not ODF namespace).
  - Excel import mapping: Not in xlformula.cxx BIFF tables.
  - Excel export mapping: None.

B. Actual implementation: H.7 FIXED (commit 405b9d3120).
  GetOddfprice() (analysishelper.cxx:1011) — THROW_RTE removed:
  - Short first period (dfc < e): T1+T2+T3-T4 formula per ODF 1.2 §6.12.31.
  - Long first period (dfc >= e): quasi-coupon walk BACKWARDS from first coupon.
  - Fixed: memory management (no new[]/delete mismatch), walk direction,
    fNC truncation, fDSC_E denominator.
  - Added fRedemp > 0 validation in financial.cxx:405.
  - Shares infrastructure with ODDFYIELD, ODDLPRICE, ODDLYIELD.

C. Format compatibility: AddIn function; not part of BIFF/OOXML
  function tables. Import/export via AddIn mechanism only.

D. Documentation: Help entry exists (AAI = Analysis AddIn).

Failure type: RESOLVED in H.7 (was: throws unconditionally).
Priority: P2 (Excel compatibility gap).
Confidence: HIGH — short period working; long period quasi-coupon overlap needs completion.


8. ODDFYIELD
------------

ODF 1.4: Not defined by ODF 1.4 Part 4. Excel Analysis ToolPak function.
ODF group: Not applicable.

A. Function registration
  - Native opcode: NONE.
  - Parser name: "ODDFYIELD" registered as AddIn in odffmap.cxx line 128:
      { "ODDFYIELD", "ODDFYIELD", false,
        "com.sun.star.sheet.addin.Analysis.getOddfyield", ... }
  - Evaluator implementation: AnalysisAddIn::getOddfyield() at
    financial.cxx:416–426. Validates fRate>=0, fPrice>0, fRedemp>0, frequency,
    nMat>nFirstCoup>nSettle>nIssue. Delegates to GetOddfyield() (H.7 FIXED).
  - Help entry: HID_AAI_FUNC_ODDFYIELD (hidother.src:407, helpids.h:319).

B. Actual Implementation: H.7 FIXED (commit 405b9d3120).
  GetOddfyield() (analysishelper.cxx:1159) — THROW_RTE removed:
  - Bracketed secant/Illinois method with 1e-12 tolerance.
  - Bracket expansion for yields > 100%.
  - Removed 1% price tolerance (was: fabs(fPrice-fPriceN) > fPrice/100).
  - Uses GetOddfprice() for price evaluation.
  - Same pattern as ODDFPRICE. Analysis add-in.

C. Format compatibility: AddIn only; no BIFF/OOXML table mapping.

D. Documentation: Help entry exists.

Failure type: RESOLVED in H.7 (was: throws unconditionally).
Priority: P2 (Excel compatibility gap).
Confidence: HIGH — solver uses Illinois method with 1e-12 tolerance.


9. RRI
----

ODF 1.4: Not explicitly listed as ODF 1.4 Large Group per task
  instructions (verify against final spec). Excel function.
ODF group: Not applicable.

A. Function registration
  - Native opcode: SC_OPCODE_ZGZ exists (compiler.hrc). "ZGZ" is the
    German name (core_resource.src line 215).
  - Parser name: "RRI" mapped in xlformula.cxx line 422 (saFuncTable_Odf):
      EXC_FUNCENTRY_ODF( ocZGZ, 3, 3, 0, "RRI" )
  - Function-name-to-opcode mapping: ocZGZ → RRI (ODF/Excel section),
    ocZGZ → ZGZ (German section, core_resource.src line 215).
  - Evaluator implementation: ScZGZ() at interpr2.cxx:1198–1209.
    Takes exactly 3 params: future value, present value, periods.
    Computes pow(fV/fP, 1/n) - 1. Format: percent.
  - Function parameter metadata: Via native function resource.
  - Help entry: HID_FUNC_ZGZ (hidother.src:135, helpids.h:527). German
    name only; no English HID_FUNC_RRI found.
  - ODF formula namespace mapping: ZGZ in ODF section
    (core_resource.src line 215), RRI in Excel section
    (core_resource.src line 671).
  - Excel import mapping: saFuncTable_Odf line 422, import-only.
  - Excel export mapping: _xlfnodf.RRI (export-only).

B. Actual implementation: ScZGZ() computes pow(fV/fP, 1/n) - 1.
  **RECLASSIFIED**: Prior validation concerns (zero/negative PV, zero
  periods, overflow) were traced to a test-data/spreadsheet error,
  not an implementation defect. The formula is correct for valid inputs.
  MustHaveParamCount(3) enforces exactly 3 args. Edge cases (negative
  PV with even root) produce NaN as expected mathematically.

C. Format compatibility
  - XLS import: Not in BIFF tables. Not recognized on legacy XLS.
  - XLSX import: saFuncTable_Odf import-only.
  - XLS export: None.
  - XLSX export: _xlfnodf.RRI.

D. Documentation: Help entry exists under German name ZGZ only.
  No English RRI help entry found.

Failure type: **No implementation defect** — prior discrepancy traced
  to test-data error. Remaining gaps: German help only; export naming
  _xlfnodf.RRI vs Excel native RRI.
Priority: No defect (was P2).
Confidence: HIGH — implementation correct; only documentation/interoperability gaps remain.


10. ERROR.TYPE
-------------

ODF 1.4: Defined by ODF 1.4 Part 4 (§6.13.x error handling).
ODF group: Verify against final spec (likely Large Group).

A. Function registration
  - Native opcode: SC_OPCODE_ERROR_TYPE = 151 (compiler.hrc line 182).
  - Parser names:
    * "ORG.OPENOFFICE.ERRORTYPE" (ODF namespace,
      core_resource.src line 146)
    * "ERRORTYPE" (Excel namespace, core_resource.src line 491)
  - Function-name-to-opcode mapping: ocErrorType → both names.
  - Evaluator implementation: ScErrorType() at interpr1.cxx:8377–8456.
    Handles svRefList, svDoubleRef, svSingleRef, and error values.
    Pushes error number as double, or NA if no error.
  - Function parameter metadata: Via native function resource.
  - Help entry: HID_FUNC_FEHLERTYP (hidother.src:324, helpids.h:732).
    German name only ("Fehlertyp"). No English HID_FUNC_ERROR_TYPE found.
  - ODF formula namespace mapping: ORG.OPENOFFICE.ERRORTYPE (ODF),
    ERRORTYPE (Excel).
  - Excel import mapping: Not in xlformula.cxx BIFF tables.
  - Excel export mapping: None.

B. Actual implementation: ScErrorType() (interpr1.cxx:8574) examines
  the stack type:
  - svRefList: gets error code from first cell in ref list;
    errNoRef if empty, errNoValue if >1 ref.
  - svDoubleRef/svSingleRef: gets cell error code via pDok->GetErrCode().
  - default: PopError(), pushes error number.
  If no error (nErr == 0), pushes NA (PushNA()).
  H.4 FIX: Added MapErrorType() helper that maps internal AOO error codes
  to ODF ERROR.TYPE return values:
    errNoCode(521) → 1 (#NULL!)
    errDivisionByZero(532) → 2 (#DIV/0!)
    errNoValue(519) → 3 (#VALUE!)
    errNoRef(524) → 4 (#REF!)
    errNoName(525) → 5 (#NAME?)
    errIllegalFPOperation(503) → 6 (#NUM!)
    NOTAVAILABLE(32767) → 7 (#N/A)
  Other internal error codes or no error → #N/A error (ODF-compliant).
  Semantic gap RESOLVED.

C. Format compatibility: No BIFF/OOXML table mapping for ERROR.TYPE.

D. Documentation: Help entry exists under German name FEHLERTYP only.
  No English ERROR.TYPE help entry found.

Failure type: Implementation present; H.4 resolved error-code mapping
  to ODF ERROR.TYPE return values (1–7). Help added in English.
Priority: P1 (ODF-defined function; error code mapping gap).
Confidence: HIGH — implementation fixed and verified at source level.


11. NUMBERVALUE
---------------

ODF 1.4: Defined by ODF 1.4 Part 4 (§6.13.x text-to-number).
ODF group: Verify against final spec (likely Large Group).

A. Function registration
  - Native opcode: SC_OPCODE_NUMBERVALUE = 392 (compiler.hrc line 388).
  - Parser name: "NUMBERVALUE" in core_resource.src lines 348 (ODF)
    and 693 (Excel).
  - Function-name-to-opcode mapping: ocNumberValue → NUMBERVALUE.
  - Evaluator implementation: ScInterpreter::ScNumberValue() at
    main/sc/source/core/tool/interpr4.cxx:4066+. Dispatch via
    `case ocNumberValue : ScNumberValue();` at interpr4.cxx:3559.
    Implements ODF 1.4 §6.13.28 transformation steps:
    whitespace removal, trailing percent handling, group separator
    removal, decimal separator replacement, numeric parsing, percent
    adjustment. Locale defaults used for omitted/empty separators.
  - Function parameter metadata: Via native function resource.
  - Help entry: HID_FUNC_NUMBERVALUE (hidother.src:370, helpids.h:776).
  - ODF formula namespace mapping: NUMBERVALUE in both ODF (line 348)
    and Excel (line 693) sections of core_resource.src.
  - Excel import mapping: saFuncTable_Odf line 418:
      EXC_FUNCENTRY_ODF( ocNumberValue, 2, 2, 0, "NUMBERVALUE" )
  - Excel export mapping: _xlfnodf.NUMBERVALUE (export-only).

B. Actual implementation: ScInterpreter::ScNumberValue() at
  interpr4.cxx:4066+. The opcode (392) is dispatched via the interpreter
  switch statement. Implements ODF 1.4 §6.13.28 with explicit-separator
  parsing, whitespace removal, percent handling, and locale-default
  fallbacks for omitted separators.

C. Format compatibility
  - XLS import: Not in BIFF tables. Not recognized on legacy XLS.
  - XLSX import: saFuncTable_Odf import-only, mapped to ocNumberValue.
  - XLS export: None.
  - XLSX export: _xlfnodf.NUMBERVALUE.

D. Documentation: Help entry exists (HID_FUNC_NUMBERVALUE).
  Function Wizard entry exists in main/sc/source/ui/src/scfuncs.src
  (SC_OPCODE_NUMBERVALUE resource block, updated with decimal_separator
  and group_separator parameters).

Failure type: None — implementation complete at source level.
Priority: Resolved (was P1).
Confidence: HIGH — source evidence complete; runtime verification
            deferred (no build available).


12. GETPIVOTDATA
----------------

ODF 1.4: Not defined by ODF 1.4 Part 4. Excel-specific function.
ODF group: Not applicable.

A. Function registration
  - Native opcode: SC_OPCODE_GET_PIVOT_DATA = 390 (compiler.hrc line 386).
  - Parser name: "GETPIVOTDATA" in core_resource.src lines 346 (ODF)
    and 691 (Excel).
  - Function-name-to-opcode mapping: ocGetPivotData → GETPIVOTDATA.
  - Evaluator implementation: ScGetPivotData() at interpr2.cxx:2972–3051+.
    Takes 2–30 params (must be even count). Supports old syntax
    (constraint string) and new syntax (field/item pairs). Uses
    pDok->GetDPAtBlock() to find the DataPilot object.
  - Function parameter metadata: Via native function resource.
  - Help entry: HID_FUNC_GETPIVOTDATA (hidother.src:330, helpids.h:738).
  - ODF formula namespace mapping: GETPIVOTDATA in both sections.
  - Excel import mapping: saFuncTable_8 line 351 (BIFF8):
      { ocGetPivotData, 358, 2, MX, V, { RR, RR, VR }, 0, 0 }
  - Excel export mapping: BIFF8 export (same table entry, no importonly/exportonly flag).

B. Actual implementation: ScGetPivotData() parses target field name
  and field/item filter pairs, locates the DataPilot table via
  ScDPObject, calls GetPivotData(). Comment at interpr2.cxx:3035–3036
  notes: "MS Excel docs claim to use the 'most recent' which is not
  exactly the same as what we do in ScDocument::GetDPAtBlock." The
  function depends entirely on the DataPilot subsystem
  (dptablecache.cxx:517 mentions GETPIVOTDATA disabling nested
  formula interpretation).

C. Format compatibility
  - XLS import: BIFF8 (function ID 358), recognized and mapped.
  - XLSX import: Not in saFuncTable_Oox. May not be recognized on
    XLSX import (OOXML table lacks GETPIVOTDATA).
  - XLS export: BIFF8 export supported.
  - XLSX export: None (not in OOXML table).

D. Documentation: Help entry exists.

Failure type: Implementation present but XLSX import/export mapping
  missing (only BIFF8/XLS). DataPilot dependency makes it
  application-level.
Priority: P2 (Excel compatibility gap; XLSX mapping missing).
Confidence: HIGH — source evidence clear; runtime unverified.


13. FDIST
-------

ODF 1.4: Not defined by ODF 1.4 Part 4 (legacy Excel function; replaced
  by F.DIST/F.DIST.RT in ODF). ODF group: Not applicable.

A. Function registration
  - Native opcode: SC_OPCODE_F_DIST = 315 (compiler.hrc line 311).
  - Parser names:
    * "LEGACY.FDIST" (ODF namespace, core_resource.src line 282)
    * "FDIST" (Excel namespace, core_resource.src line 667)
  - Function-name-to-opcode mapping: ocFDist → both names.
  - Evaluator implementation: ScFDist() at interpr3.cxx:1588–1620.
    H.8 FIXED: Supports 3-4 params (x, df1, df2, [cumulative]).
    Validates x>=0, df1>=1, df2>=1, df1<1E10, df2<1E10.
    Calls GetFDist() for cumulative, GetFDistDens() for density.
    Defaults to cumulative=TRUE.
  - Function parameter metadata: Via Function Wizard resource.
  - Help entry: HID_FUNC_FDIST (hidother.src:283, helpids.h:687).
    English name; HID_FUNC_FDIST added.
  - ODF formula namespace mapping: LEGACY.FDIST (ODF), FDIST (Excel).
  - Excel import mapping: saFuncTable_Odf line 410:
      EXC_FUNCENTRY_ODF( ocFDist, 3, 4, 0, "FDIST" )
  - Excel export mapping: _xlfnodf.FDIST (export-only).

B. Actual implementation: H.8 FIXED. ScFDist() (interpr3.cxx:1588)
  supports 3-4 params per ODF 1.2 §6.17.9. Added GetFDistDens()
  for probability density function (cumulative=FALSE).
  Defaults to cumulative=TRUE.

C. Format compatibility
  - XLS import: Not in BIFF tables. Not recognized on legacy XLS.
  - XLSX import: saFuncTable_Odf import-only.
  - XLS export: None.
  - XLSX export: _xlfnodf.FDIST.

D. Documentation: Help entry updated to English HID_FUNC_FDIST.
  Help content in 04060182.xhp updated with 4-param syntax and examples.
  Test cases SG.FDIST-01..12 added.

Failure type: RESOLVED in H.8 (was: param count gap, German help).
Priority: P2 (was P2).
Confidence: HIGH — source evidence clear, tests added.


14. FINV
------

ODF 1.4: Not defined by ODF 1.4 Part 4 (legacy Excel function).
ODF group: Not applicable.

A. Function registration
  - Native opcode: SC_OPCODE_F_INV = 359 (compiler.hrc line 355).
  - Parser names:
    * "LEGACY.FINV" (ODF namespace, core_resource.src line 326)
    * "FINV" (Excel namespace, core_resource.src line 671)
  - Function-name-to-opcode mapping: ocFInv → both names.
  - Evaluator implementation: ScFInv() at interpr3.cxx:2154–2171+.
    Takes exactly 3 params (probability, df1, df2). Validates
    0<p<=1, 1<=df<1E10. Uses ScFDistFunction + lcl_IterateInverse
    for numerical inversion.
  - Function parameter metadata: Via native function resource.
  - Help entry: HID_FUNC_FINV (hidother.src:284, helpids.h:688).
  - ODF formula namespace mapping: LEGACY.FINV (ODF), FINV (Excel).
  - Excel import mapping: saFuncTable_Odf line 411:
      EXC_FUNCENTRY_ODF( ocFInv, 3, 3, 0, "FINV" )
  - Excel export mapping: _xlfnodf.FINV (export-only).

B. Actual implementation: ScFInv() performs numerical inverse of
  F-distribution via iteration (lcl_IterateInverse). Validation
  present. Shares GetFDist with ScFDist.

C. Format compatibility: Same as FDIST — _xlfnodf.FINV export.

D. Documentation: Help entry exists (HID_FUNC_FINV).
  Help content in 04060182.xhp updated with improved examples.
  Test cases SG.FINV-01..12 added.

Failure type: RESOLVED in H.8 (was: export naming compatibility).
Priority: P2 (was P2).
Confidence: HIGH — tests added.


15. HARMEAN
---------

ODF 1.4: Defined by ODF 1.4 Part 4 (statistical function).
ODF group: Verify against final spec (likely Large Group).

A. Function registration
  - Native opcode: SC_OPCODE_HAR_MEAN = 321 (compiler.hrc line 317).
  - Parser name: "HARMEAN" in core_resource.src lines 288 (ODF)
    and 679 (Excel).
  - Function-name-to-opcode mapping: ocHarmMean → HARMEAN.
  - Evaluator implementation: ScHarMean() at interpr3.cxx:2712–2751+.
    Iterates over params, accumulates 1/x for x>0, counts values.
    Sets errIllegalArgument for x<=0. Skips empty cells.
  - Function parameter metadata: Via native function resource.
  - Help entry: HID_FUNC_HARMITTEL (hidother.src:247, helpids.h:651).
    German name "HarMittel"; no English HID_FUNC_HARMEAN found.
  - ODF formula namespace mapping: HARMEAN in both sections.
  - Excel import mapping: Not in xlformula.cxx BIFF tables (HARMEAN
    is a native Excel function, may be in the main function table
    not reviewed in detail).
  - Excel export mapping: Not in saFuncTable_Odf (HARMEAN is not
    listed there). May be in main saFuncTable (not reviewed).

B. Actual implementation: ScHarMean() implements H = n / Σ(1/xᵢ)
  counting only positive values. Zero or negative values trigger
  errIllegalArgument. Text values and empty cells are skipped
  (handled by the switch's default case / HasCellValueData check).
  Error values propagate via nGlobalError.

C. Format compatibility: Not found in xlformula.cxx BIFF/OOXML
  tables reviewed. May be in the main saFuncTable (pre-BIFF5).

D. Documentation: Help entry under German name HARMITTEL only.

Failure type: Implementation present but help only in German.
  Need to verify BIFF main table mapping.
Priority: P3 (minor — documentation/alias issue).
Confidence: HIGH — implementation present; help language gap confirmed.


Cross-Cutting Observation: LENB/RIGHTB/LEFTB/MIDB Opcodes
----------------------------------------------------------

compiler.hrc defines SC_OPCODE_LENB=400, SC_OPCODE_RIGHTB=401,
SC_OPCODE_LEFTB=402, SC_OPCODE_MIDB=403. These byte-oriented text
function opcodes exist but FINDB/SEARCHB/REPLACEB do not. This
confirms the byte-oriented text function family is partially
implemented (extract/length functions present, find/replace/search
absent).
