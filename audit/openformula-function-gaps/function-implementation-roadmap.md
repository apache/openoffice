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

OpenFormula Function Compatibility Gap Audit — Implementation Roadmap
=====================================================================

Branch: openformula
Starting commit: 6a2f742073
Audit type: Read-only source inspection (roadmap is a proposal, not committed work)

This document proposes an implementation sequence for the 15 audited
functions, grouping related work where appropriate. No production
code changes have been made.

Updated 2026-11-18 to match the code after the post-merge review: the branch
tip did not compile and several H.1-H.9 changes were wrong; the fixes are
noted per group. The H.x regression rows were replaced by 99 rows checked
against Excel 16 (end of test/odf-validate/calcval-cases14.tsv), which match
on macOS and Linux builds.

Group 1 — Byte-Oriented Text Functions (FINDB, REPLACEB, SEARCHB) — COMPLETED
--------------------------------------------------------------------------------

Status: Implemented in H.2.

Changes:

  1. Added SC_OPCODE_FINDB=412, SC_OPCODE_SEARCHB=413, SC_OPCODE_REPLACEB=414
     to compiler.hrc. Existing opcodes (BITAND=404 through IFERROR=411) were
     NOT renumbered — new opcodes were added at the end of the function range.
     (The earlier note that XLS files store 8192 + opcode was wrong:
     TokenPool::nScTokenOff in tokstack.cxx is an in-memory offset.)
  2. Added ocFindB, ocSearchB, ocReplaceB to opcode.hxx enum.
  3. Registered function names in core_resource.src (ODF and Excel sections,
     plus localized en-US text).
  4. Added HID_FUNC_FINDB, HID_FUNC_SEARCHB, HID_FUNC_REPLACEB to helpids.h
     and hidother.src.
  5. Added dispatch cases in interpr4.cxx.
  6. Implemented ScFindB(), ScSearchB(), ScReplaceB() in interpr1.cxx
     using DBCS byte semantics (consistent with existing LENB/LEFTB/RIGHTB/
     MIDB). Added lcl_BytePosToCharIndex() and lcl_CharIndexToBytePos()
     helpers for byte↔character position conversion.
     2026-11-18: ScFindB() and ScSearchB() popped the two text arguments in
     the wrong order (FINDB("b";"abc") searched "b" for "abc"); fixed.
     xls: now BIFF 205-207 both ways (xlformula.cxx saFuncTable_2), and
     Excel 16 calculates FINDB from an AOO xls; xlsx: read
     (formulabase.cxx, ODF names FINDB/SEARCHB/REPLACEB).
  7. Regression rows in calcval-cases14.tsv (FINDB.*, SEARCHB.*,
      REPLACEB.*; rewritten 2026-11-18).
  8. Added Function Wizard documentation in main/sc/source/ui/src/scfuncs.src
      (SC_OPCODE_FINDB, SC_OPCODE_SEARCHB, SC_OPCODE_REPLACEB resource
      blocks with parameter names and descriptions).

Byte semantics: DBCS model (1 byte for ASCII/non-DBCS chars, 2 bytes for
  DBCS chars per IsDBCS()). Consistent with existing LENB/LEFTB/RIGHTB/
  MIDB implementations. Byte positions are 1-based. Partial DBCS character
  boundaries are avoided in REPLACEB by scanning forward byte-by-byte.

Rationale: These three functions were entirely missing from the byte-
  oriented text function family. Now implemented following the existing
  B-function patterns.

Group 2 — Date/Time Compatibility (DATEDIF, ISOWEEKNUM) — DATEDIF COMPLETED
--------------------------------------------------------------------------------

DATEDIF Status: Implemented in H.3.

Changes:

  1. Added SC_OPCODE_DATEDIF = 415 to compiler.hrc. Existing opcodes
     unchanged. (STOP_2_PAR is now 420 and LAST_OPCODE_ID 419.)
  2. Added ocDateDif to opcode.hxx enum.
  3. Registered "DATEDIF" name in core_resource.src (ODF and Excel sections,
     plus localized en-US text).
  4. Added HID_FUNC_DATEDIF to helpids.h and hidother.src.
  5. Added dispatch case in interpr4.cxx.
  6. Implemented ScInterpreter::ScDateDif() in interpr2.cxx
     implementing ODF 1.4 §6.10.3 with all six formats (y, m, d, md, ym, yd).
     2026-11-18: rewritten to Excel's behavior where ODF is silent (Jim's
     decision): start after end is an error, the format ignores case, "md"
     and "yd" follow Excel; "yd" was wrong before. See
     datedif-semantic-design.md.
  7. Added Function Wizard entry in scfuncs.src (Start_date, End_date, Format
     parameters, ID_FUNCTION_GRP_DATETIME group).
  8. Regression rows DATEDIF.* in calcval-cases14.tsv (rewritten
     2026-11-18).
  9. Fixed BIFF5/BIFF7 import mapping in xlformula.cxx:
     changed `ocNoName` → `ocDateDif` for function ID 351 (DATEDIF).
     2026-11-18: the import-only flag was dropped, so DATEDIF is written
     to xls too (Excel 16 calculates it); xlsx reads DATEDIF.

Semantic design: See datedif-semantic-design.md for detailed algorithm
specification including reverse-date policy, time fraction handling, and
month/year boundary semantics.

ISOWEEKNUM: COMPLETED in H.5 (commit on openformula branch).

Changes:

  1. Added SC_OPCODE_ISOWEEKNUM = 416 (compiler.hrc, opcode.hxx).
  2. Added ScIsoWeekNum() in interpr2.cxx using Date::GetWeekOfYear(MONDAY, 4).
  3. Added dispatch case ocIsoWeekNum in interpr4.cxx.
  4. Registered ISOWEEKNUM name in core_resource.src (ODF and Excel namespaces).
     The ODF list also has ISOWEEKNUM for ocWeek, which is how AOO writes
     WEEKNUM(d;mode); on reading, the first entry wins. 2026-11-18:
     FormulaCompiler::Factor turns an ODF ISOWEEKNUM with one argument into
     ocIsoWeekNum, so files from other applications load; two arguments
     stay WEEKNUM.
  5. Added function wizard resource in scfuncs.src (1 argument, no mode;
     its ExtraData had one number too many, fixed 2026-11-18).
  6. Added HID_FUNC_ISOWEEKNUM in helpids.h and hidother.src.
  7. Added help topic func_isoweeknum.xhp, registered in makefile and 04060102.xhp.
  8. Calcval rows ISOWEEKNUM.* (rewritten 2026-11-18; several SG.ISO
     expectations were wrong, e.g. week 1 for Sunday 2024-12-29).
     xls: written as `_xlfn.ISOWEEKNUM` (2026-11-18). xlsx `_xlfn.ISOWEEKNUM`
     still gives #NAME?.
  9. WEEKNUM (ScGetWeekOfYear) unchanged: retains 2-arg SUN/MON mode.

Rationale: ISOWEEKNUM needed separate opcode and evaluator for ISO 8601
  compliance (1 arg, Monday start, week 1 = first Thursday). Now resolved.

Group 3 — Financial Odd-Period Functions (ODDFPRICE, ODDFYIELD)
---------------------------------------------------------------

Status: native opcodes DEFERRED — pending financial function registration
policy. The Analysis add-in functions were implemented on 2026-11-18 (Jim's
decision).

Rationale: H.6.1 opcode registration was reverted (f4f3de4f92).
  A broader financial function registration policy must be established
  before allocating native opcodes to ODDFPRICE/ODDFYIELD. This includes:

  1. Inventory of native vs AddIn-only financial functions.
  2. Consistent criteria for opcode allocation, import/export,
     Function Wizard, and help registration.
  3. Decision on whether ODDFPRICE/ODDFYIELD should receive native
     implementations or remain AddIn-only.

Current state: Implemented as Analysis AddIn only
  (financial.cxx getOddfprice/getOddfyield, analysishelper.cxx
  GetOddfprice/GetOddfyield). On trunk both threw (THROW_RTE, #87380#);
  8ce50c3485 brought in an unfinished version. The 2026-11-18 code follows
  Excel's short and long odd first period formulas, requires the first coupon
  to be on the maturity's coupon schedule (as Excel), and solves the yield
  with a bracketed Illinois iteration. Against Excel 16 on 4524 random cases:
  14 differences, all classified (4 where DFC equals E and Excel gives #NUM!,
  5 annual 30/360 bonds with Feb-end coupons off by 2/360 of a coupon, 5
  where Excel's yield is less exact or wrong). No native opcode; Function
  Wizard and English help (04060118.xhp) come from the add-in.

PRIORITY: P2 (Excel compatibility gap). DEFERRED until policy defined.

Group 4 — RRI (Rate of Return)
------------------------------

Recommended sequence:

  1. Review ScZGZ() (interpr2.cxx:1198) for missing validation:
     zero/negative present value, zero periods, overflow/underflow.
  2. Add input validation (PushIllegalArgument for invalid inputs).
  3. English help exists (04060118.xhp, HID_FUNC_ZGZ).
  4. Verify Excel import/export mapping (`_xlfnodf.RRI` may cause
     Excel compatibility issues — Excel has RRI natively).
  5. Add test cases.

Rationale: Implementation exists but lacks validation (ScZGZ() checks
  nothing; ODF requires N > 0). Export naming may cause Excel compatibility
  issues.

2026-11-18: the ODF and English names are RRI (ZGZ is still read, through
  OpCodeMap::putOpCodeAlias). The RRI commit had also renamed ocZGZ,
  SC_OPCODE_ZGZ, ScZGZ() and HID_FUNC_ZGZ and reworded the help; that was
  undone, because translations are keyed on SC_OPCODE_ZGZ and the German
  function name ZGZ would have been lost.

Group 5 — ERROR.TYPE
--------------------

Status: COMPLETED in H.4, redone 2026-11-18.

H.4 changed ScErrorType() itself, so ERRORTYPE (ODF name
  ORG.OPENOFFICE.ERRORTYPE) stopped returning the internal error numbers its
  help documents (Err:518 gives 518). Jim's decision: ERRORTYPE is back to
  the trunk behavior and ERROR.TYPE is its own opcode.

Changes now:

  1. SC_OPCODE_ERROR_TYPE_ODF = 419, ocErrorTypeODF, names "ERROR.TYPE"
     (ODF, English, UI). ScErrorType( bool bODF ) with lcl_ErrorTypeODF()
     in interpr1.cxx; the mapping is that of XclTools::GetXclErrorCode, so
     Err:502 gives 3, anything not listed 7. Listed in CASE_OCERRFUNC.
  2. xls: BIFF 261 reads as ERROR.TYPE; ERRORTYPE is still written as 261.
     xlsx: ERROR.TYPE reads as ERROR.TYPE.
  3. Help: func_errortype.xhp with its own help ID HID_FUNC_ERROR_TYPE_ODF
     (it had reused ERRORTYPE's), embedded in 04060109.xhp next to
     ERRORTYPE instead of the date functions page.
  4. Calcval rows `ERROR.TYPE.*` and ERRORTYPE.legacy*.

Group 6 — NUMBERVALUE (Critical Gap) — COMPLETED
------------------------------------------------

Status: Implemented in H.1.1 (commit on openformula branch); rewritten
2026-11-18 to the ODF step order and moved to interpr1.cxx next to VALUE.

Changes:

  1. Added case ocNumberValue : ScNumberValue(); to the interpr4.cxx
     dispatch switch.
  2. ScInterpreter::ScNumberValue() in interpr1.cxx, following ODF 1.2/1.4
     §6.13.28: group separators removed only before the decimal separator,
     the first decimal separator becomes '.', whitespace removed, trailing
     percent signs divide by 100, and the rest must be a plain xsd:float
     (lcl_IsPlainFloat()). LEN(DecimalSeparator) must be 1 and it must not
     occur in GroupSeparator. Omitted separators come from the locale; the
     locale group separator is dropped when it equals the given decimal
     separator.
  3. Regression rows NUMBERVALUE.* in calcval-cases14.tsv (rewritten
      2026-11-18).
  4. Function Wizard resource in scfuncs.src: the trunk strings kept,
      unsuppressed, a third parameter added, parameters 2 and 3 optional
      (the H.1 block had one flag too few).
   Differences from Excel 16 (checked): empty text is an error (Excel 0); an
   empty GroupSeparator is accepted (Excel error); a two-character
   DecimalSeparator is an error (Excel uses the first).

Rationale: CRITICAL gap resolved — opcode (392) and name registered
  but no evaluator existed. Function now compiles and evaluates
  correctly. P1 priority resolved.

Group 7 — GETPIVOTDATA
----------------------

Recommended sequence:

  1. Review ScGetPivotData() (interpr2.cxx:2972) for correctness.
  2. XLSX import exists (formulabase.cxx GETPIVOTDATA row, import-only);
     there is no xlsx export at all. Not run.
  3. Verify DataPilot dependency handling (refresh, multiple tables).
  4. Add test cases.

Rationale: Implementation present. DataPilot-dependent application-level
  function.

Group 8 — FDIST, FINV (Legacy Statistical)
------------------------------------------

2026-11-18: H.8/H.8.1 turned ocFDist/ocFInv into the ODF left-tail
functions and added ocLegacyFDist/ocLegacyFInv, which changed the meaning of
FDIST/FINV typed in every UI language, in macros and in .sxc files; the H.6.1
revert also dropped the new opcode defines, so the branch did not build.
Jim's decision: FDIST and FINV stay as on trunk (right tail; ODF
LEGACY.FDIST/LEGACY.FINV; xls 281/282), and the ODF left-tail functions are
new opcodes F.DIST (SC_OPCODE_F_DIST_LEFT 417, ScFDistLeft()) and F.INV
(SC_OPCODE_F_INV_LEFT 418, ScFInvLeft()), with ODF names FDIST and FINV.
F.DIST takes an optional Cumulative (default TRUE), returns 0 for x < 0 as
ODF says, and gives the density through GetBetaDistPDF(). F.DIST matches
Excel 16 on 300 random cases; F.INV on 298 of 300, the other two within
3e-10 where the distribution is almost flat (p = 0.999, r2 = 1). xls: `_xlfn.F.DIST`/`_xlfn.F.INV`
(Excel calculates them; F.DIST needs four arguments there), and `_xlfnodf.FDIST`/`_xlfnodf.FINV` from older files import as FDIST/FINV. Help: F.DIST and F.INV sections in
04060182.xhp.

Original recommended sequence:

  1. Review ScFDist() (interpr3.cxx:1588) — enforces exactly 3
     params; ODF allows 3–4 with cumulative flag.
  2. Review ScFInv() (interpr3.cxx:2154) — numerical inversion via
     iteration.
  3. Consider adding optional 4th cumulative parameter to FDIST.
  4. Review export naming: `_xlfnodf.FDIST`/`_xlfnodf.FINV` — Excel has
     these natively without _xlfn prefix; may cause compatibility
     issues.
  5. English help exists for both (04060182.xhp; FVERT is only the help ID
     name).
  6. Add test cases.

Rationale: Implementations present but export naming and parameter
  count are gaps. P2 priority.

Group 9 — HARMEAN
-----------------

Recommended sequence:

  1. Review ScHarMean() (interpr3.cxx:2712) — counts only x>0,
     errIllegalArgument for x<=0. Verify against ODF definition.
  2. English help exists (04060182.xhp; HARMITTEL is only the help ID name).
  3. BIFF 320 both ways (xlformula.cxx saFuncTable_4) and xlsx 320
     (formulabase.cxx).
  4. Add test cases.

Rationale: Implementation present and appears correct. No gap left in
  this audit's terms.

Group 10 — DDE (Deferred)
-------------------------

Recommended sequence:

  1. Evaluate platform support (Linux, Windows, macOS).
  2. Address security concerns (external application access,
     user confirmation).
  3. Consider whether DDE should be available in headless/server mode
     (currently disabled).
  4. If implemented: add Excel import/export mappings, test cases.

Rationale: DEFER — requires substantial platform/application-level
  work; unresolved security and platform-appropriateness questions.
  Not a pure formula function.

Group 11 — HYPGEOMDIST Cumulative (H.9, added 2026-11-18)
---------------------------------------------------------

H.9/H.9.1 added the optional fifth argument Cumulative (default FALSE). The
optional argument was popped after the others, one line of the trunk PMF
algorithm was changed (wrong in 12240 of 135750 exhaustive small cases), and
the function header was lost in a later merge. 2026-11-18: the trunk
algorithm itself gave wrong results for some large arguments
(HYPGEOMDIST(900;1828;2033;4167) gave 3.2E+181 instead of 0.0219), so
GetHypGeomDist() now multiplies the C(n,x) factors and the probability ratios
in an order that keeps the product near 1 (same size limit as before).
Cumulative sums the smaller tail with term ratios. Checked: all 635375 small
cases against exact values (worst relative error 9e-14), 400 large cases
against exact big-integer values, and Excel 16 HYPGEOM.DIST/HYPGEOMDIST on 895 cases.
xls cannot hold five arguments (written as NA()).

Prioritization Summary
----------------------

P1 (Critical — ODF-required behavior missing):

- ERROR.TYPE: RESOLVED in H.4
- NUMBERVALUE: RESOLVED in H.1
- FINDB/REPLACEB/SEARCHB: RESOLVED in H.2
- DATEDIF: RESOLVED in H.3/H.3.1

P1 (High — semantic gaps):

- ISOWEEKNUM: RESOLVED in H.5

P2 (Excel compatibility):

- ODDFPRICE/ODDFYIELD: add-in implemented 2026-11-18; native opcodes
    DEFERRED — pending financial registration policy
- RRI: no validation of N > 0; xls export as `_xlfnodf.RRI`
- GETPIVOTDATA: xlsx import present, not run
- F.DIST: xls export with three arguments shows #N/A in Excel

P3 (Minor):

- HARMEAN: nothing open

DEFER (Platform/application-level):

- DDE: Requires DDE subsystem; security questions

Confidence Levels
-----------------

Run and checked against Excel 16 (2026-11-18): FINDB, REPLACEB, SEARCHB
(ASCII; CJK counts 2 bytes in AOO always), DATEDIF, ISOWEEKNUM, NUMBERVALUE,
ERROR.TYPE, F.DIST, F.INV, FDIST, FINV, HYPGEOMDIST, ODDFPRICE, ODDFYIELD,
RRI (one case). Source only: GETPIVOTDATA, HARMEAN, DDE.
