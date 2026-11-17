OpenFormula Function Compatibility Gap Audit — Implementation Roadmap
=====================================================================

Branch: openformula
Starting commit: 6a2f742073
Audit type: Read-only source inspection (roadmap is a proposal, not committed work)

This document proposes an implementation sequence for the 15 audited
functions, grouping related work where appropriate. No production
code changes have been made.


Group 1 — Byte-Oriented Text Functions (FINDB, REPLACEB, SEARCHB) — COMPLETED
--------------------------------------------------------------------------------

Status: Implemented in H.2.

Changes:
  1. Added SC_OPCODE_FINDB=412, SC_OPCODE_SEARCHB=413, SC_OPCODE_REPLACEB=414
     to compiler.hrc. Existing opcodes (BITAND=404 through IFERROR=411) were
     NOT renumbered — new opcodes were added at the end of the function range
     to preserve XLS binary compatibility (tokstack.cxx stores
     8192 + opcode_value in XLS files).
  2. Added ocFinDB, ocSearchB, ocReplaceB to opcode.hxx enum.
  3. Registered function names in core_resource.src (ODF and Excel sections,
     plus localized en-US text).
  4. Added HID_FUNC_FINDB, HID_FUNC_SEARCHB, HID_FUNC_REPLACEB to helpids.h
     and hidother.src.
  5. Added dispatch cases in interpr4.cxx (lines 3772-3774).
  6. Implemented ScFindB(), ScSearchB(), ScReplaceB() in interpr1.cxx
     using DBCS byte semantics (consistent with existing LENB/LEFTB/RIGHTB/
     MIDB). Added lcl_BytePosToBytePos() and lcl_CharIndexToBytePos()
     helpers for byte↔character position conversion.
   7. Added 14 regression test cases to calcval-cases14.tsv covering
      basic search, case sensitivity, start position, not found, empty
      find text, basic replace, append at end, truncate, zero count.
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
  1. Added SC_OPCODE_DATEDIF = 415 to compiler.hrc. Moved STOP_2_PAR to 416
     and LAST_OPCODE_ID to 415. Existing opcodes unchanged.
  2. Added ocDateDif to opcode.hxx enum.
  3. Registered "DATEDIF" name in core_resource.src (ODF and Excel sections,
     plus localized en-US text).
  4. Added HID_FUNC_DATEDIF to helpids.h and hidother.src.
  5. Added dispatch case in interpr4.cxx (line 3775).
  6. Implemented ScInterpreter::ScDateDif() in interpr2.cxx (line 443)
     implementing ODF 1.4 §6.10.3 with all six formats (y, m, d, md, ym, yd).
  7. Added Function Wizard entry in scfuncs.src (Start_date, End_date, Format
     parameters, ID_FUNCTION_GRP_DATETIME group).
  8. Added 15 regression test cases to calcval-cases14.tsv covering
     basic days/months/years, same date, month boundaries, leap year dates,
     component formats (md/ym/yd), reverse dates, and invalid formats.
  9. Fixed BIFF5/BIFF7 import mapping in xlformula.cxx line 342:
     changed `ocNoName` → `ocDateDif` for function ID 351 (DATEDIF),
     preserving EXC_FUNCFLAG_IMPORTONLY flag. BIFF import now resolves
     DATEDIF to the native evaluator instead of producing #NAME!.

Semantic design: See datedif-semantic-design.md for detailed algorithm
specification including reverse-date policy, time fraction handling, and
month/year boundary semantics.

ISOWEEKNUM: Still pending (see below).

Recommended sequence for ISOWEEKNUM:
  1. Verify ScGetWeekOfYear() (interpr2.cxx:227) does NOT implement
     ISO 8601 (only SUN/MON start via Date::GetWeekOfYear()).
  2. Implement ISO 8601 week numbering (week 1 = first Thursday,
     Monday start, 52/53 week years) — either in ScGetWeekOfYear()
     with mode=21 or as a separate code path.
  3. Add English help entry (currently only German KALENDERWOCHE).
  4. Verify Excel BIFF import mapping (currently not in BIFF tables).
  5. Add test cases.

Rationale: DATEDIF was entirely missing from the date/time compatibility
  group. Now implemented following ODF 1.4 §6.10.3 with documented
  Excel-compatibility edge cases. ISOWEEKNUM remains pending.


Group 3 — Financial Odd-Period Functions (ODDFPRICE, ODDFYIELD)
---------------------------------------------------------------

Recommended sequence:
  1. Verify Analysis AddIn implementations (getOddfprice, getOddfyield
     in financial.cxx:404–426) are complete and correct.
  2. If native implementation desired: add SC_OPCODE_ODDFPRICE,
     SC_OPCODE_ODDFYIELD to compiler.hrc, implement ScOddfprice(),
     ScOddfyield() in interpr*.cxx.
  3. Add English help entries (currently German-only or AddIn-only).
  4. Add Excel import/export mappings.
  5. Add test cases.
  6. Consider ODDLPRICE/ODDLYIELD together (same code path).

Rationale: Currently Analysis AddIn functions only. Not native opcodes.
  Share code with ODDLPRICE/ODDLYIELD (already implemented as AddIn).


Group 4 — RRI (Rate of Return)
------------------------------

Recommended sequence:
  1. Review ScZGZ() (interpr2.cxx:1198) for missing validation:
     zero/negative present value, zero periods, overflow/underflow.
  2. Add input validation (PushIllegalArgument for invalid inputs).
  3. Add English help entry (currently German ZGZ only).
  4. Verify Excel import/export mapping (_xlfnodf.RRI may cause
     Excel compatibility issues — Excel has RRI natively).
  5. Add test cases.

Rationale: Implementation exists but lacks validation. Help only in
  German. Export naming may cause Excel compatibility issues.


Group 5 — ERROR.TYPE
--------------------

Status: COMPLETED in H.4 (commit on openformula branch).

Changes:
  1. Added MapErrorType() helper in interpr1.cxx mapping internal error codes
     to ODF ERROR.TYPE return values (1–7).
  2. Modified ScErrorType() to use MapErrorType() before pushing result.
  3. Added English help topic func_errortype.xhp and registered in makefile
     and 04060102.xhp index.
  4. Added calcval test cases SG.ERRTYPE-01 through SG.ERRTYPE-12.

Rationale: Implementation existed but error-code-to-ODF-number mapping
  was missing. Now resolved at source level.


Group 6 — NUMBERVALUE (Critical Gap) — COMPLETED
------------------------------------------------

Status: Implemented in H.1.1 (commit on openformula branch).

Changes:
  1. Added case ocNumberValue : ScNumberValue(); to interpr4.cxx:3559
     dispatch switch.
  2. Implemented ScInterpreter::ScNumberValue() at interpr4.cxx:4066+,
     following ODF 1.4 §6.13.28 transformation steps:
     - Whitespace removal
     - Trailing percent sign handling (divide by 100 each)
     - Group separator removal (lenient, no 3-digit restriction)
     - Decimal separator replacement with ASCII period
     - Numeric parsing via ::rtl::math::stringToDouble
     - Full string consumption verification
     - Locale defaults for omitted/empty separators
   3. Added 12 regression test cases to calcval-cases14.tsv covering
      basic, European, space-grouping, empty, whitespace, percent,
      double-percent, decimal-percent, repeated-decimal, negative,
      scientific, and omitted-separator cases.
   4. Added Function Wizard documentation in main/sc/source/ui/src/scfuncs.src
      (SC_OPCODE_NUMBERVALUE resource block with text, decimal_separator,
      and group_separator parameters).

Rationale: CRITICAL gap resolved — opcode (392) and name registered
  but no evaluator existed. Function now compiles and evaluates
  correctly. P1 priority resolved.



Group 7 — GETPIVOTDATA
----------------------

Recommended sequence:
  1. Review ScGetPivotData() (interpr2.cxx:2972) for correctness.
  2. Add XLSX import/export mapping (currently only BIFF8/XLS).
  3. Verify DataPilot dependency handling (refresh, multiple tables).
  4. Add test cases.

Rationale: Implementation present but XLSX mapping missing.
  DataPilot-dependent application-level function.


Group 8 — FDIST, FINV (Legacy Statistical)
------------------------------------------

Recommended sequence:
  1. Review ScFDist() (interpr3.cxx:1588) — enforces exactly 3
     params; ODF allows 3–4 with cumulative flag.
  2. Review ScFInv() (interpr3.cxx:2154) — numerical inversion via
     iteration.
  3. Consider adding optional 4th cumulative parameter to FDIST.
  4. Review export naming: _xlfnodf.FDIST/_xlfnodf.FINV — Excel has
     these natively without _xlfn prefix; may cause compatibility
     issues.
  5. Add English help entries (currently German FVERT for FDIST;
     FINV has English help).
  6. Add test cases.

Rationale: Implementations present but export naming and parameter
  count are gaps. P2 priority.


Group 9 — HARMEAN
-----------------

Recommended sequence:
  1. Review ScHarMean() (interpr3.cxx:2712) — counts only x>0,
     errIllegalArgument for x<=0. Verify against ODF definition.
  2. Add English help entry (currently German HARMITTEL only).
  3. Verify BIFF main-table import/export mapping.
  4. Add test cases.

Rationale: Implementation present and appears correct. Minor
  documentation gap. P3 priority.


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


Prioritization Summary
----------------------

P1 (Critical — ODF-required behavior missing):
  - ERROR.TYPE: Error-code mapping to ODF return values unverified
  - NUMBERVALUE: RESOLVED — evaluator implemented in H.1.1
  - FINDB/REPLACEB/SEARCHB: RESOLVED — evaluators implemented in H.2
  - DATEDIF: RESOLVED — evaluator implemented in H.3

P1 (High — semantic gaps):
  - ISOWEEKNUM: ISO 8601 week numbering not implemented

P2 (Excel compatibility):
  - ODDFPRICE/ODDFYIELD: Analysis AddIn only, not native
  - RRI: Validation gaps, German help only
  - GETPIVOTDATA: XLSX mapping missing
  - FDIST/FINV: Export naming (_xlfnodf), param count (FDIST)

P3 (Minor):
  - HARMEAN: German help only

DEFER (Platform/application-level):
  - DDE: Requires DDE subsystem; security questions

Confidence Levels
-----------------
HIGH: FINDB, REPLACEB, SEARCHB, DATEDIF, NUMBERVALUE, ODDFPRICE,
      ODDFYIELD, GETPIVOTDATA, FDIST, FINV, HARMEAN, DDE
MEDIUM: ISOWEEKNUM, RRI, ERROR.TYPE (implementation present but
        semantic correctness unverified at runtime)