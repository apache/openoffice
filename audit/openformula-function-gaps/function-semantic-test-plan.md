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

OpenFormula Function Compatibility Gap Audit — Semantic Test Plan
=================================================================

Branch: openformula
Starting commit: 6a2f742073
Audit type: Read-only source inspection (tests prepared, NOT executed)

Updated 2026-11-18 to match the code after the post-merge review. The SG.*
rows were replaced by 99 rows at the end of
test/odf-validate/calcval-cases14.tsv, checked against Excel 16 where Excel
has the function and run on macOS and Linux builds (all match). Expected
values below that were wrong are corrected; where AOO and Excel differ on
purpose, the row note says so.

This document provides a test matrix for all 15 audited functions.
Tests are designed but not run. Each test distinguishes:

  1. Formula parser rejection
  2. Unknown function
  3. Wrong argument count
  4. Evaluation failure
  5. Incorrect numerical result
  6. Incorrect error result
  7. Import failure
  8. Export failure
  9. Missing platform service

Expected results are derived from ODF 1.4 Part 4 (where applicable)
or independently calculated mathematical examples. No test result is
claimed as verified.

1\. FINDB
--------

Defined by ODF 1.2 and 1.4 (§6.7.2: "The same as FIND, but using byte
positions"). Byte model: DBCS (CJK characters count 2 bytes).

Test cases (rows `FINDB.*` in `test/odf-validate/calcval-cases14.tsv`):
  a) =FINDB("b";"abc") — basic byte position (2)
  b) =FINDB("b";"abcb";3) — start position (4)
  c) =FINDB("x";"abc") — not found (#VALUE!)
  d) =FINDB("B";"abc") — case-sensitive (#VALUE!)
  e) =FINDB("a";"abc";0) — start < 1 (#VALUE!)
  f) =FINDB("a";"abc";4) — start past the text (#VALUE!)
  g) =FINDB("国";"中国") — DBCS second char (3)
  h) =FINDB("b";"a中b") — mixed ASCII/DBCS (4)

Implemented: ScFindB() (interpr1.cxx) uses the AOO DBCS byte model
  (DBCS=2 bytes, ASCII=1) via getLengthB, lcl_BytePosToCharIndex,
  lcl_CharIndexToBytePos. Case-sensitive, no wildcards. The two text
  arguments were popped in the wrong order until 2026-11-18.
  Excel 16 counts a CJK character as 1 byte unless a DBCS language is set,
  so g) and h) give 2 and 3 there.

Purpose: Confirm FINDB byte-position semantics and DBCS handling.

2\. REPLACEB
-----------

Defined by ODF 1.2 and 1.4 (§6.7.6: "As REPLACE, but using byte
positions").

Test cases (rows `REPLACEB.*` in `test/odf-validate/calcval-cases14.tsv`):
  a) =REPLACEB("abc";2;1;"X") — basic replace (aXc)
  b) =REPLACEB("abc";2;0;"X") — zero count inserts (aXbc)
  c) =REPLACEB("abc";4;1;"X") — start after the end appends (abcX)
  d) =REPLACEB("abc";2;10;"X") — count past the end (aX)
  e) =REPLACEB("abc";0;1;"X") — start < 1 (error)
  f) =REPLACEB("abc";2;-1;"X") — negative count (error)
  g) =REPLACEB("中国";3;2;"日") — second DBCS character (中日)

Implemented: ScReplaceB() (interpr1.cxx) uses the AOO DBCS byte model
  via getLengthB, lcl_BytePosToCharIndex, lcl_CharIndexToBytePos.
  Clamps position/count, handles DBCS boundaries.

Purpose: Confirm REPLACEB byte-position replace and boundary handling.

3\. SEARCHB
----------

Defined by ODF 1.2 and 1.4 (§6.7.8: "As SEARCH, but using byte
positions").

Test cases (rows `SEARCHB.*` in `test/odf-validate/calcval-cases14.tsv`):
  a) =SEARCHB("B";"abc") — case-insensitive (2)
  b) =SEARCHB("b";"abcb";3) — start position (4)
  c) =SEARCHB("x";"abc") — not found (#VALUE!)
  d) =SEARCHB("国";"中国") — DBCS second char (3)

Implemented: ScSearchB() (interpr1.cxx) uses the AOO DBCS byte model
  via getLengthB, lcl_BytePosToCharIndex, lcl_CharIndexToBytePos.
  Case-insensitive like SEARCH; regular expressions when the Calc option
  for them is on. AOO 4 has no ? and * wildcards, so the earlier wildcard
  cases (=SEARCHB("a?c";"abc") expected 1) were wrong. The two text
  arguments were popped in the wrong order until 2026-11-18.

Purpose: Confirm SEARCHB byte-position search.

4\. DATEDIF
----------

ODF 1.4 Part 4 §6.10.3. Excel-compatible date difference function.

Test cases (rows `DATEDIF.*` in `test/odf-validate/calcval-cases14.tsv`):
  a) =DATEDIF(DATE(2024;1;1);DATE(2024;12;31);"d") — days difference (365)
  b) =DATEDIF(DATE(2024;1;15);DATE(2024;6;14);"m") — complete months (4)
  c) =DATEDIF(DATE(2024;1;31);DATE(2024;2;29);"m") — no complete month (0)
  d) =DATEDIF(DATE(2020;6;15);DATE(2024;6;14);"y") — complete years (3)
  e) =DATEDIF(DATE(2024;2;29);DATE(2025;2;28);"y") — leap-day anniversary (0)
  f) =DATEDIF(DATE(2024;1;15);DATE(2024;3;10);"md") — Excel borrow from the
     month before the end month (24; the old expectation 26 was wrong)
  g) =DATEDIF(DATE(2024;1;31);DATE(2024;3;1);"md") — Excel gives -1, so does AOO
  h) =DATEDIF(DATE(1995;5;3);DATE(2009;4;28);"yd") — 361, as Excel
  i) =DATEDIF(DATE(2024;12;31);DATE(2024;1;1);"d") — start after end (error, as Excel)
  j) =DATEDIF(DATE(2024;1;1);DATE(2024;12;31);"D") — upper case accepted (365)
  k) "x" and "" formats (error); time fractions ignored
  l) xls: written and read (BIFF 351); xlsx: read

Purpose: Validate DATEDIF semantics; see datedif-semantic-design.md.
Run 2026-11-18 on macOS and Linux builds and against Excel 16 (630 cases,
one difference, documented there).

5\. ISOWEEKNUM
-------------

ODF 1.4: Defined by ODF 1.4 Part 4. ISO 8601 week numbering.

Test cases (rows `ISOWEEKNUM.*` in `test/odf-validate/calcval-cases14.tsv`):
  a) =ISOWEEKNUM(DATE(2024;12;29)) — Sunday, week 52
  b) =ISOWEEKNUM(DATE(2024;12;30)) — ISO week 1 of 2025 (1)
  c) =ISOWEEKNUM(DATE(2020;12;31)) — ISO week 53
  d) =ISOWEEKNUM(DATE(2021;1;3)) — still week 53 of 2020
  e) =ISOWEEKNUM(DATE(2026;1;1)) — Thursday, week 1 (an earlier row expected 53)
  f) =ISOWEEKNUM() — wrong arg count (error)
  g) =WEEKNUM(DATE(2024;12;30)) — WEEKNUM still needs two arguments (error)

Fixed: Added separate opcode SC_OPCODE_ISOWEEKNUM (416) and
  ScIsoWeekNum() (interpr2.cxx) implementing ISO 8601 via
  Date::GetWeekOfYear(MONDAY, 4). Takes exactly 1 argument (date).
  Does not affect existing WEEKNUM (ScGetWeekOfYear) which keeps
  2-argument SUN/MON mode semantics. On ODF import, ISOWEEKNUM with one
  argument becomes ocIsoWeekNum (FormulaCompiler::Factor); with two it
  stays WEEKNUM (checked with an .ods holding both forms).

Purpose: Validate ISO 8601 week numbering at year boundaries. 300 random
  dates matched Excel 16.

6\. DDE
------

Application-level DDE function. Platform-dependent.

Test cases:
  a) =DDE("soffice";"table";"A1") — creates DDE link (platform-dependent)
  b) =DDE("nonexistent";"topic";"item") — unavailable application (error expected)
  c) =DDE("app";"topic") — 2 args (wrong arg count, needs 3)
  d) DDE on Linux without DDE support — missing platform service
  e) DDE in headless/server mode — DDE disabled in server mode
     (appdde.cxx:494, 526)

Purpose: Confirm DDE works where platform supports it, fails gracefully
  otherwise. Not a pure formula test — requires DDE subsystem.

7\. ODDFPRICE
------------

Analysis AddIn function. Odd-period bond price. ODF 1.2/1.4 §6.12.31.

Test cases (rows `ODDFPRICE.*` in `test/odf-validate/calcval-cases14.tsv`):
  a) Excel's short odd first period example: 113.597717474079
  b) long odd first periods, semiannual and annual
  c) first coupon not on the maturity's coupon schedule (error, as Excel)
  d) redemption 0 (error)
  e) XLS/XLSX: no mapping (add-in function)

Implemented 2026-11-18 in analysishelper.cxx GetOddfprice() (trunk threw
  #87380#). Checked against Excel 16 on random cases with all bases and
  frequencies, short and long periods, many month ends: see
  function-implementation-roadmap.md, Group 3, for the 14 differences in
  4524 ODDFPRICE/ODDFYIELD cases.

Purpose: Confirm the add-in matches Excel.

8\. ODDFYIELD
------------

Analysis AddIn function. Odd-period bond yield. ODF 1.2/1.4 §6.12.32.

Test cases (rows `ODDFYIELD.*`):
  a) Excel's example: 0.0772455415978 (Excel 0.077245541597)
  b) ODDFYIELD of an ODDFPRICE result gives the yield back (0.1492)
  c) redemption 0 (error)

Implemented 2026-11-18: bracketed Illinois iteration on GetOddfprice().
  In the 222 cases where the two yields differed, feeding both to Excel's
  ODDFPRICE gave back the price within 1e-10 for AOO's yield and within
  1.8e-7 for Excel's.

Purpose: Confirm the add-in matches Excel.

9\. RRI
------

Rate of return per period: RRI(N; Pv; Fv) = (Fv/Pv)^(1/N) - 1.
ODF 1.2/1.4 §6.12.44, constraint N > 0.

Test cases:
  a) =RRI(10;100;200) — 0.0717734625362931 (row RRI.basic; Excel the same)
  b) =ZGZ(10;100;200) — old name still read (row RRI.zgz)
  c) =RRI(0;100;200) — N = 0 (ODF: error; not run)
  d) =RRI(-5;100;200) — N < 0 (ODF: error; not run)
  e) =RRI(10;-100;200) — negative Pv (not run)
  f) =RRI(10;100) — wrong arg count (error)
  g) xls: `_xlfnodf.RRI` (AOO reads it back, Excel #NAME?); xlsx
     `_xlfn.RRI` is read

Expected: ScZGZ() (interpr2.cxx) computes pow(Fv/Pv, 1/N) - 1
  with NO validation. c) to e) are not checked by the code.

Purpose: Confirm validation gaps in RRI implementation.

10\. ERROR.TYPE
--------------

ODF 1.4: Returns error number (1=#NULL!, 2=#DIV/0!, ..., 7=#N/A).

Test cases (rows `ERROR.TYPE.*` and `ERRORTYPE.*`):
  a) =ERROR.TYPE(1/0) — 2
  b) =ERROR.TYPE(NA()) — 7
  c) =ERROR.TYPE(AM1), AM1 = NA() — 7; =ERROR.TYPE(AM2), AM2 = 1/0 — 2
  d) =ERROR.TYPE(SQRT(-1)) — 3 (AOO gives Err:502, which the xls export
     writes as #VALUE!; Excel gives 6)
  e) =ERROR.TYPE(5), =ERROR.TYPE("t") — non-error → #N/A
  f) =ERROR.TYPE() — wrong arg count (error)
  g) =ERRORTYPE(1/0) — 532 and =ERRORTYPE(NA()) — 32767 (legacy, unchanged)

  The old cases with error literals (=ERROR.TYPE(#REF!) etc.) do not parse
  in AOO's English API grammar (Err:511/520), so they were dropped.

Fixed 2026-11-18: ERROR.TYPE is its own opcode (ocErrorTypeODF, 419);
  ScErrorType( true ) maps with lcl_ErrorTypeODF() like
  XclTools::GetXclErrorCode (unlisted errors give 7). H.4 had changed
  ERRORTYPE instead; that is undone.

Purpose: Validate error-code mapping to ODF return values.

11\. NUMBERVALUE
---------------

ODF 1.4: Converts text to number using explicit separators.

Test cases (rows `NUMBERVALUE.*`):
  a) =NUMBERVALUE("1.234";".";",") — 1.234
  b) =NUMBERVALUE("1.234,56";",";".") — 1234.56
  c) =NUMBERVALUE("1 234.56";".";" ") — space grouping sep: 1234.56
  d) =NUMBERVALUE("1.234,5";".";",") — group sep after the decimal sep (error)
  e) =NUMBERVALUE("  1 2 3  ";".";",") — whitespace removed: 123
  f) =NUMBERVALUE("12%";".";",") — 0.12; "1.5%%" — 0.00015
  g) =NUMBERVALUE(".5";".";",") — 0.5; "-1.5e3" — -1500; "1e" (error)
  h) =NUMBERVALUE("1.2.3";".";",") — second decimal sep (error)
  i) =NUMBERVALUE("";".";",") — error (Excel 0)
  j) =NUMBERVALUE("1,5";",,";".") — DecimalSeparator longer than 1 (error;
     Excel uses the first character)
  k) =NUMBERVALUE("1,5";",";",.") — DecimalSeparator in GroupSeparator (error)
  l) =NUMBERVALUE("1234.5";".";"") — no group separator (1234.5; Excel error)
  m) =NUMBERVALUE("1,5";",") — locale group sep dropped when it equals the
     decimal sep (1.5)

Expected: ScNumberValue() (interpr1.cxx) implements the ODF steps (see
  function-implementation-roadmap.md, Group 6). Before H.1 there was no
  evaluator; the H.1 version removed group separators everywhere and
  replaced every decimal separator.

Purpose: Confirm the ODF transformation steps.

12\. GETPIVOTDATA
----------------

Extracts data from a DataPilot (pivot) table.

Test cases:
  a) =GETPIVOTDATA("Sales";$A$1:C$10;"Region";"West") — field/item pair
  b) =GETPIVOTDATA("Sales";$A$1) — old syntax with constraint string
  c) =GETPIVOTDATA("Sales";$A$1:C$10;"Region";"West";"Year";2024) — multiple filters
  d) =GETPIVOTDATA("Nonexistent";$A$1:C$10) — missing field (error)
  e) =GETPIVOTDATA("Sales";$A$1:C$10;"Region";"Nonexistent") — missing item (error)
  f) =GETPIVOTDATA() — wrong arg count (error)
  g) =GETPIVOTDATA("Sales";A1:C10;"Region";"West") — odd arg count (error)
  h) XLS import (BIFF8) of GETPIVOTDATA — recognized (function ID 358)
  i) XLSX import of GETPIVOTDATA — formulabase.cxx has an import-only row
     (not run)
  j) XLSX export — there is no xlsx export
  k) Refresh pivot table then recalc — DataPilot dependency test

Expected: ScGetPivotData() (interpr2.cxx) uses
  pDok->GetDPAtBlock() to locate DataPilot object.

Purpose: Confirm DataPilot dependency.

13\. FDIST
---------

ODF FDIST (§6.18.22) is the left tail with an optional Cumulative; AOO's
FDIST is ODF LEGACY.FDIST (§6.18.23, right tail). Since 2026-11-18 the ODF
function is F.DIST in AOO (ocFDistLeft, 417); FDIST is unchanged.

Test cases (rows `F.DIST.*`, `FDIST.*`):
  a) =F.DIST(2.5;5;10) — 0.8979977233557302 (Excel F.DIST the same)
  b) =F.DIST(2.5;5;10;0) — density 0.0935947544496855
  c) =F.DIST(-1;5;10) — 0 (ODF; Excel #NUM!)
  d) =F.DIST(0;1;5;0) — pole (error); =F.DIST(0;2;5;0) — 1
  e) =F.DIST(2.5;0;10) — r1 < 1 (error)
  f) =F.DIST(2.5;5;10)+FDIST(2.5;5;10) — 1
  g) =FDIST(2.5;5;10) — 0.10200227664427 (legacy right tail)
  h) =FDIST(2.5;5;10;1) — legacy takes three arguments (error)

Expected: ScFDistLeft() and ScFDist() (interpr3.cxx). xls: F.DIST as
  `_xlfn.F.DIST` (Excel calculates 4 arguments), FDIST as BIFF 281, and
  `_xlfnodf.FDIST` from an older xls reads as FDIST.

Purpose: Confirm left and right tail.

14\. FINV
--------

ODF FINV (§6.18.24) is the inverse of the left tail; AOO's FINV is ODF
LEGACY.FINV (§6.18.25). Since 2026-11-18 the ODF function is F.INV in AOO
(ocFInvLeft, 418); FINV is unchanged.

Test cases (rows `F.INV.*`, `FINV.*`):
  a) =F.INV(0.5;5;10) — 0.931933160851048
  b) =F.INV(0;5;10) — 0; =F.INV(1;5;10) — error
  c) =F.INV(F.DIST(2.5;5;10);5;10) — 2.5
  d) =FINV(0.1;5;10) — 2.52164068620962 (legacy)

Expected: ScFInvLeft() uses ScFDistLeftFunction + lcl_IterateInverse;
  ScFInv() unchanged.

Purpose: Confirm the inverses.

15\. HARMEAN
-----------

Harmonic mean: H = n / Σ(1/xᵢ), counting only positive values.

Test cases:
  a) =HARMEAN(1;2;4) — 3 / (1/1 + 1/2 + 1/4) = 3/1.75 ≈ 1.714
  b) =HARMEAN(1;0;2) — zero value → errIllegalArgument
  c) =HARMEAN(1;-2;4) — negative value → errIllegalArgument
  d) =HARMEAN(5) — single value → 5
  e) =HARMEAN() — wrong arg count (error)
  f) =HARMEAN(A1:A5) where A1:A5 contains text — text cells skipped
  g) =HARMEAN(A1:A5) where A1:A5 contains error — error propagates

Expected: ScHarMean() (interpr3.cxx) counts only x > 0 values,
  sets errIllegalArgument for x <= 0, skips empty/text cells.

Purpose: Confirm implementation is correct. English help exists
  (04060182.xhp; HARMITTEL is only the help ID name). Not run.

Cross-Function Test: Byte-Oriented Text Family
----------------------------------------------

FINDB, SEARCHB, REPLACEB (412–414) join LENB, RIGHTB, LEFTB, MIDB
(400–403). All seven are written to xls as Excel's own functions (BIFF
205–211).

Purpose: Confirm the whole byte-oriented text family works.

Test Execution Note
-------------------

Originally designed but not executed. Since 2026-11-18 the rows named
above are run by test/odf-validate/calcval.py on macOS and Linux builds;
cases marked "not run" are still hypotheses. DDE, GETPIVOTDATA and HARMEAN
were not run.
