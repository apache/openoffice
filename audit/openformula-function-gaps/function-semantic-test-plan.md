OpenFormula Function Compatibility Gap Audit — Semantic Test Plan
=================================================================

Branch: openformula
Starting commit: 6a2f742073
Audit type: Read-only source inspection (tests prepared, NOT executed)

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


1. FINDB
--------

Not defined by ODF 1.4. Excel-specific byte-oriented find.

Test cases:
  a) =FINDB("abc";"a") — parser rejection expected (unknown function)
  b) =FINDB("abc";"b";1) — unknown function
  c) XLS import of workbook containing FINDB — import failure expected
  d) XLSX export of workbook containing FINDB — export failure expected

Purpose: Confirm function is entirely absent (no opcode, no parser,
         no evaluator).


2. REPLACEB
-----------

Not defined by ODF 1.4. Excel-specific byte-oriented replace.

Test cases:
  a) =REPLACEB("abc";2;1;"x") — parser rejection expected
  b) XLS import of workbook containing REPLACEB — import failure expected

Purpose: Confirm function is entirely absent.


3. SEARCHB
----------

Not defined by ODF 1.4. Excel-specific byte-oriented search.

Test cases:
  a) =SEARCHB("b";"abc") — parser rejection expected
  b) XLS import of workbook containing SEARCHB — import failure expected

Purpose: Confirm function is entirely absent.


4. DATEDIF
----------

ODF 1.4 Part 4 §6.10.3. Excel-compatible date difference function.

Test cases (implemented as `SG.DATEDIF-*` in `test/odf-validate/calcval-cases14.tsv`):
  a) =DATEDIF(DATE(2024;1;1);DATE(2024;12;31);"d") — days difference (365)
  b) =DATEDIF(DATE(2024;1;1);DATE(2025;1;1);"m") — months difference (12)
  c) =DATEDIF(DATE(2024;1;1);DATE(2025;1;1);"y") — years difference (1)
  d) =DATEDIF(DATE(2024;2;29);DATE(2024;3;1);"d") — leap-day interval
  e) =DATEDIF(DATE(2024;12;31);DATE(2024;1;1);"d") — reverse interval → -365 (y/m/d negated)
  f) =DATEDIF(DATE(2024;1;1);DATE(2024;1;1);"d"/"m"/"y"/"md"/"ym"/"yd") — same date (0)
  g) =DATEDIF(DATE(2024;1;15);DATE(2024;3;10);"md") — days ignoring months/years with borrow (26)
  h) =DATEDIF(DATE(2024;1;15);DATE(2024;3;10);"ym") — months ignoring years after complete months (1)
  i) =DATEDIF(DATE(2024;1;15);DATE(2024;3;10);"yd") — days ignoring years from anniversary (55)
  j) =DATEDIF(DATE(2024;1;1);DATE(2024;12;31);"x") — invalid unit string (error)
  k) =DATEDIF(DATE(2024;1;1);DATE(2024;12;31);"") — empty format (error)
  l) =DATEDIF(DATE(2024;1;1);DATE(2024;12;31);"D") — case-sensitive format (error)
  m) end-of-month / leap-anniversary / multi-year / reverse-component / time-fraction edge cases
  n) BIFF5/7 import of DATEDIF — maps to `ocDateDif` (import-only, ID 351); BIFF8/OOXML not mapped
  o) wrong arg count / invalid date serials — error (reviewed in code)

Purpose: Validate implemented DATEDIF semantics and reverse-interval policy.
Source-level review completed in H.3.1; runtime execution pending build
availability (`sc`/`scaddins` not built in this environment).


5. ISOWEEKNUM
-------------

ODF 1.4: Not confirmed as ODF-defined (verify against final spec).
ISO 8601 week numbering: Monday first, week 1 = first Thursday.

Test cases:
  a) =ISOWEEKNUM(DATE(2024;1;1)) — Monday 2024-01-01 is ISO week 1 (expected: 1)
  b) =ISOWEEKNUM(DATE(2024;12;30)) — ISO week 53 (expected: 53)
  c) =ISOWEEKNUM(DATE(2024;12;31)) — ISO week 1 of 2025 (expected: 1)
  d) =ISOWEEKNUM(DATE(2025;12;29)) — ISO week 1 of 2026 (expected: 1)
  e) =ISOWEEKNUM(DATE(2024;1;1);1) — 2 args with mode flag (current impl supports SUN/MON only)
  f) =ISOWEEKNUM("not a date") — invalid input (error)
  g) =ISOWEEKNUM() — wrong arg count (error)
  h) =ISOWEEKNUM(DATE(2024;6;15);21) — ISO mode (21) if supported; current impl ignores mode value beyond SUN/MON

Expected: Current implementation ScGetWeekOfYear() (interpr2.cxx:227)
  only supports SUNDAY (flag=1) or MONDAY (flag!=1) via
  Date::GetWeekOfYear(). ISO 8601 week numbering (week 1 = first
  Thursday) is NOT implemented. Tests (a)-(d) will produce incorrect
  results for dates near year boundaries.

Purpose: Confirm ISO 8601 semantic gap.


6. DDE
----

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


7. ODDFPRICE
------------

Analysis AddIn function. Odd-period bond price.

Test cases:
  a) =ODDFPRICE(DATE(2024;3;15);DATE(2024;6;30);DATE(2023;11;15);DATE(2028;12;31);0.05;0.06;100;2) — price calculation
  b) =ODDFPRICE(DATE(2024;3;15);DATE(2024;6;30);DATE(2023;11;15);DATE(2028;12;31);-0.05;0.06;100;2) — negative rate (error)
  c) =ODDFPRICE(DATE(2024;3;15);DATE(2024;6;30);DATE(2023;11;15);DATE(2028;12;31);0.05;0.06;100;0) — invalid frequency (error)
  d) =ODDFPRICE() — wrong arg count (error)
  e) XLS import of ODDFPRICE — import failure (not in BIFF tables)
  f) XLSX import of ODDFPRICE — import failure (not in OOXML table)

Expected: AnalysisAddIn::getOddfprice() (financial.cxx:404) validates
  inputs and delegates to GetOddfprice(). Implementation present as
  AddIn but not as native function.

Purpose: Confirm AddIn implementation works; confirm not native.


8. ODDFYIELD
------------

Analysis AddIn function. Odd-period bond yield.

Test cases:
  a) =ODDFYIELD(DATE(2024;3;15);DATE(2024;6;30);DATE(2023;11;15);DATE(2028;12;31);0.05;95;100;2) — yield calculation
  b) =ODDFYIELD(...;0.05;0;100;2) — zero price (error)
  c) =ODDFYIELD() — wrong arg count (error)

Expected: AnalysisAddIn::getOddfyield() (financial.cxx:416).

Purpose: Confirm AddIn implementation works.


9. RRI
----

Rate of return per period: RRI = (fV/pV)^(1/n) - 1.

Test cases:
  a) =RRI(100;50;5) — (100/50)^(1/5) - 1 = 2^(0.2) - 1 ≈ 0.1487
  b) =RRI(100;50;0) — zero periods (division by zero → error)
  c) =RRI(100;-50;5) — negative present value (invalid sign → error expected)
  d) =RRI(0;50;5) — zero future value (−1 = -100%)
  e) =RRI(100;50;-5) — negative periods (error)
  f) =RRI(100;50) — wrong arg count (error)
  g) =RRI(1e300;1;5) — overflow test
  h) XLSX import of RRI — import-only via ODF table (_xlfnodf.RRI)
  i) XLSX export of RRI — _xlfnodf.RRI

Expected: ScZGZ() (interpr2.cxx:1198) computes pow(fV/fP, 1/n) - 1
  with NO validation for zero/negative PV, zero periods, or overflow.
  Tests (b), (c), (e), (g) may produce undefined results instead of
  proper errors.

Purpose: Confirm validation gaps in RRI implementation.


10. ERROR.TYPE
--------------

ODF 1.4: Returns error number (1=#NULL!, 2=#DIV/0!, ..., 7=#N/A).

Test cases:
  a) =ERROR.TYPE(#DIV/0!) — expected: 2
  b) =ERROR.TYPE(#VALUE!) — expected: 3
  c) =ERROR.TYPE(#REF!) — expected: 4
  d) =ERROR.TYPE(#NAME!) — expected: 5
  e) =ERROR.TYPE(#NUM!) — expected: 6
  f) =ERROR.TYPE(#N/A) — expected: 7
  g) =ERROR.TYPE(#NULL!) — expected: 1
  h) =ERROR.TYPE(5) — no error → NA error
  i) =ERROR.TYPE(A1) where A1 contains #DIV/0! — reference to error cell (2)
  j) =ERROR.TYPE(A1:B2) — multiple refs → errNoValue
  k) =ERROR.TYPE() — wrong arg count (error)
  l) =ERROR.TYPE(ORG.OPENOFFICE.ERRORTYPE(#DIV/0!)) — ODF namespace name

Expected: ScErrorType() (interpr1.cxx:8377) pushes internal error code
  as double. The mapping from internal error codes to ODF ERROR.TYPE
  return values (1–7) is NOT evident in source. Tests (a)–(g) verify
  whether the mapping is correct. Test (j) verifies multi-ref handling.

Purpose: Confirm error-code mapping to ODF return values. This is the
  primary semantic gap for ERROR.TYPE.


11. NUMBERVALUE
---------------

ODF 1.4: Converts text to number using explicit separators.

Test cases:
  a) =NUMBERVALUE("1.234";".";",") — expected: 1.234
  b) =NUMBERVALUE("1.234,56";",";";") — European format: expected: 1234.56
  c) =NUMBERVALUE("1 234.56";".";" ") — space grouping sep: expected: 1234.56
  d) =NUMBERVALUE("";".";",") — empty string (error expected)
  e) =NUMBERVALUE("  123  ";".";",") — leading/trailing whitespace
  f) =NUMBERVALUE("123%";".";",") — percent sign handling
  g) =NUMBERVALUE("1.2.3";".";",") — repeated decimal sep (error)
  h) =NUMBERVALUE("1,,2";",";",") — repeated grouping sep (error)
  i) =NUMBERVALUE("1.5e2";".";",") — scientific notation
  j) =NUMBERVALUE("123";".";",") — no grouping sep needed
  k) =NUMBERVALUE("123";".") — omitted optional grouping sep (2 args)
  l) =NUMBERVALUE("123") — wrong arg count (needs at least 2)

Expected: NUMBERVALUE has opcode (SC_OPCODE_NUMBERVALUE=392) and
  name registration but NO evaluator implementation. All tests are
  expected to fail at evaluation time (no ScNumberValue function,
  no case ocNumberValue in interpr4.cxx switch).

Purpose: Confirm the opcode-without-evaluator gap. This is a
  critical P1 gap — the function appears to exist but does not work.


12. GETPIVOTDATA
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
  i) XLSX import of GETPIVOTDATA — import failure (not in OOXML table)
  j) XLSX export of GETPIVOTDATA — export failure (not in OOXML table)
  k) Refresh pivot table then recalc — DataPilot dependency test

Expected: ScGetPivotData() (interpr2.cxx:2972) uses
  pDok->GetDPAtBlock() to locate DataPilot object. XLSX import/export
  mapping missing (only BIFF8/XLS).

Purpose: Confirm DataPilot dependency and XLSX mapping gap.


13. FDIST
---------

F-distribution probability (legacy Excel function).

Test cases:
  a) =FDIST(2.5;5;10) — F-distribution probability
  b) =FDIST(-1;5;10) — negative x (error: illegal argument)
  c) =FDIST(2.5;0;10) — zero df1 (error: df1 < 1)
  d) =FDIST(2.5;5;0) — zero df2 (error: df2 < 1)
  e) =FDIST(2.5;5) — wrong arg count (needs exactly 3)
  f) =FDIST(2.5;5;10;1) — 4 args — current impl enforces exactly 3,
     ODF allows 3–4 with cumulative flag (semantic gap)
  g) =FDIST(1e11;5;10) — df too large (>=1E10 → error)

Expected: ScFDist() (interpr3.cxx:1588) enforces exactly 3 params,
  validates ranges, calls GetFDist(). ODF allows optional 4th
  cumulative parameter — not supported.

Purpose: Confirm parameter count limitation (3 vs 3–4) and
  _xlfnodf.FDIST export naming.


14. FINV
------

Inverse F-distribution (legacy Excel function).

Test cases:
  a) =FINV(0.05;5;10) — inverse F probability
  b) =FINV(0;5;10) — zero probability (error: fP <= 0)
  c) =FINV(1.5;5;10) — probability > 1 (error: fP > 1)
  d) =FINV(0.05;0;10) — zero df1 (error: df1 < 1)
  e) =FINV(0.05;5;0) — zero df2 (error: df2 < 1)
  f) =FINV(0.05;5) — wrong arg count (needs exactly 3)

Expected: ScFInv() (interpr3.cxx:2154) validates inputs, uses
  ScFDistFunction + lcl_IterateInverse for numerical inversion.

Purpose: Confirm _xlfnodf.FINV export naming (Excel has FINV natively
  without _xlfn prefix).


15. HARMEAN
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

Expected: ScHarMean() (interpr3.cxx:2712) counts only x > 0 values,
  sets errIllegalArgument for x <= 0, skips empty/text cells.

Purpose: Confirm implementation is correct. Minor gap: German help
  only (HARMITTEL).

Cross-Function Test: Byte-Oriented Text Family
----------------------------------------------

Related missing functions: FINDB, SEARCHB, REPLACEB.
Existing: LENB, RIGHTB, LEFTB, MIDB (opcodes 400–403).

Test cases:
  a) =LENB("abc") — existing function, should work
  b) =FINDB("b";"abc") — missing function (parser rejection)
  c) =SEARCHB("b";"abc") — missing function (parser rejection)
  d) =REPLACEB("abc";2;1;"x") — missing function (parser rejection)

Purpose: Confirm partial implementation of byte-oriented text family.


Test Execution Note
-------------------

All tests above are designed but NOT executed. No build or runtime
verification was performed. Test results marked "expected" are
hypotheses based on source inspection only.
