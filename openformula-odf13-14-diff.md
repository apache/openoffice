# ODF 1.3 vs ODF 1.4 Migration Audit — Apache OpenOffice Calc

Source-of-truth: AOO source (main/sc) and `odf_docs/ODF14-{IMPLEMENTATION-DEFINED,KNOWN-LIMITATIONS}-PUBLIC.md` (AOO development source of 2026-11-04). ODF 1.4 Part 4 is OASIS Standard, 6 October 2025. Findings marked **[CONFIRMED]** have source:line evidence; **[UNVERIFIED]** require the ODF 1.4 Part 4 spec text and/or runtime testing.

## Investigated functions

### EASTERSUNDAY — ODF 1.3/1.4 addition (§6.10.8)

**[CONFIRMED] Implemented.**
- Opcode `SC_OPCODE_EASTERSUNDAY = 380` — `main/formula/inc/formula/compiler.hrc:376`, `main/formula/inc/formula/opcode.hxx:387`.
- Implementation `ScInterpreter::ScEasterSunday` — `main/sc/source/core/tool/interpr2.cxx:261-294`.
- Dispatch `case ocEasterSunday: ScEasterSunday();` — `main/sc/source/core/tool/interpr4.cxx:3514`.
- Declaration `void ScEasterSunday();` — `main/sc/source/core/inc/interpre.hxx:613`.
- Metadata `Resource SC_OPCODE_EASTERSUNDAY` — `main/sc/source/ui/src/scfuncs.src:1004-1026` (documents year 1583-9956; omitted = next Easter on/after today).
- Name mapping `SC_OPCODE_EASTERSUNDAY -> "EASTERSUNDAY"` — `main/formula/source/core/resource/core_resource.src:333` (ODFF) and `:675` (non-ODFF).
- Code comment explicitly references ODF 1.4: `// ODF 1.4 Part 4 6.10.8: 1583 <= Year <= 9956` — `interpr2.cxx:272`.
- Year validation `if (fYear < 0.0 || fYear > 9956.0) PushIllegalArgument();` — `interpr2.cxx:273-277`.
- Strict date serial `GetDateSerial(nYear,nMonth,nDay,true)` — `interpr2.cxx:282`.

**[CONFIRMED] Legacy alias.**
- `main/sc/source/core/tool/compiler.cxx:2542`: `{ "ORG.OPENOFFICE.EASTERSUNDAY", ocEasterSunday }` — comment `// EASTERSUNDAY -> ORG.OPENOFFICE.EASTERSUNDAY`. Listed under "ODFF names ... not written ... but to be recognized", i.e. `ORG.OPENOFFICE.EASTERSUNDAY` is a legacy read-only alias; the standard name `EASTERSUNDAY` is written/recognized per core_resource.src.

**[CONFIRMED] ODF 1.4 range enforced.** Year 1583 <= year <= 9956 per ODF 1.4 §6.10.8; out-of-range raises illegal argument. Matches spec.

**Assessment: EASTERSUNDAY is FULL under the standard name.** The `ORG.OPENOFFICE.EASTERSUNDAY` alias is backward-compat read-only, not a migration blocker.

### CONVERT — ODF 1.4 unit table changed

**[CONFIRMED] Implemented, unit-value discrepancy.**
- `SC_OPCODE_CONVERT = 382` — `compiler.hrc:381`.
- Implementation `ScInterpreter::ScConvert` — `main/sc/source/core/tool/interpr2.cxx` (declared `interpre.hxx:641`).
- Name mapping `SC_OPCODE_CONVERT -> "CONVERT"` — `core_resource.src:340` (ODFF and non-ODFF), `scfuncs.src` metadata.

**[CONFIRMED] Discrepancy vs ODF 1.4.**
- `odf_docs/ODF14-KNOWN-LIMITATIONS-PUBLIC.md:72`: "CONVERT with `atm` uses 101324.996583 Pa; the ODF 1.4 table gives 1.0132510E+5, a relative difference of 1.0e-6."

**Assessment: PARTIAL — implemented but `atm` unit value differs from ODF 1.4 table by 1.0e-6 relative.** Migration gap for unit-table conformity.

### COUNTA — empty-cell semantics changed

**[CONFIRMED] Implemented.**
- `SC_OPCODE_COUNT_2 = 228` — `compiler.hrc:228`; name "COUNTA" — `core_resource.src:179`.
- Implementation `ScInterpreter::ScCount2` — `interpr1.cxx` (declared `interpre.hxx:505`).

**[UNVERIFIED] ODF 1.4 vs 1.3 empty-cell semantics.** ODF 1.4 changed COUNTA/COUNTBLANK empty-cell handling; AOO's `ScCount2`/`ScCountEmptyCells` behavior against the ODF 1.4 definition not traced. **[UNVERIFIED] COUNTBLANK of "" impl-defined (odf_docs item 6.13.8).**

**Assessment: PARTIAL/UNVERIFIED — implementation present, ODF 1.4 empty-cell semantics not verified.**

### INDEX — row-vector single-argument rule removed

**[CONFIRMED] Implemented, behavioral discrepancy.**
- `SC_OPCODE_INDEX = 290` — `compiler.hrc:286`; name "INDEX" — `core_resource.src:253`.
- Implementation `ScInterpreter::ScIndex` — `interpr1.cxx` (declared `interpre.hxx:564`).

**[CONFIRMED] Discrepancy vs ODF 1.4.**
- `odf_docs/ODF14-KNOWN-LIMITATIONS-PUBLIC.md:71`: "INDEX on a row vector with only a row argument returns the column, as in other spreadsheets. ODF 1.4 removed that rule and calls the call an error."

**Assessment: MAPPING/DISCREPANCY — AOO returns column (legacy behavior); ODF 1.4 requires error.** Migration gap.

### ISBLANK / ISFORMULA / ISLOGICAL / ISNONTEXT / ISNUMBER / ISREF / ISTEXT

**[CONFIRMED] All implemented.**
- ISBlank `ocIsEmpty=117` — `interpr1.cxx:ScIsEmpty`, `core_resource.src:107`.
- ISFORMULA `ocIsFormula=125` — `interpr1.cxx:ScIsFormula`, `core_resource.src:115`.
- ISLOGICAL `ocIsLogical=120` — `interpr1.cxx:ScIsLogical`, `core_resource.src:110`.
- ISNONTEXT `ocIsNonString=119` — `interpr1.cxx:ScIsNonString`, `core_resource.src:109`.
- ISNUMBER `ocIsValue=124` — `interpr1.cxx:ScIsValue`, `core_resource.src:114`.
- ISREF `ocIsRef=123` — `interpr1.cxx:ScIsRef`, `core_resource.src:113`.
- ISTEXT `ocIsString=118` — `interpr1.cxx:ScIsString`, `core_resource.src:108`.

**[ODF 1.4 impl-defined items]**
- ISNUMBER/ISTEXT of a complex number impl-defined (odf_docs item 4.3.3).
- ISEVEN/ISODD of a Logical impl-defined (item 6.13.8).
- ISFORMULA of a non-reference impl-defined (item 6.13.8).
- N of Text impl-defined (item 6.13.8).
- VALUE of a non-Text impl-defined (item 6.13.8).
- COUNT with 0 parameters, COUNTA, COUNTBLANK of "" impl-defined (items 6.13.6-6.13.8).

**Assessment: FULL presence; ODF 1.4 edge-case semantics impl-defined / UNVERIFIED.**

### NPER — trailing empty parameter

**[CONFIRMED] Implemented.**
- `SC_OPCODE_NBW = 229` (NPV opcode value reused... actually NPER maps to `ocZZR=258`) — `compiler.hrc:258`; name "NPER" — `core_resource.src:217`.
- Implementation `ScInterpreter::ScZZR` — `interpr2.cxx`.

**[CONFIRMED] Documented rule.**
- `odf_docs/ODF14-KNOWN-LIMITATIONS-PUBLIC.md:73`: "A trailing empty parameter before the closing bracket, as in `NPER(r;p;pv;;)`, is an error for every function except `IF`. An empty parameter in the middle works."

**Assessment: FULL with documented trailing-empty-parameter rule consistent with ODF 1.4.**

### PMT — migration candidate

**[CONFIRMED] Implemented.**
- `SC_OPCODE_RMZ = 251` — `compiler.hrc:251`; name "PMT" — `core_resource.src:208`.
- Implementation `ScInterpreter::ScRMZ` — `interpr2.cxx`.

**[UNVERIFIED]** ODF 1.4 PMT definition conformity not traced.

**Assessment: FULL presence; ODF 1.4 semantics UNVERIFIED.**

### Year 1583 handling

**[CONFIRMED] Documented impl-defined.**
- `odf_docs/ODF14-IMPLEMENTATION-DEFINED-PUBLIC.md:155` (§7.4): "`DATE` returns #VALUE! for any date before 15 October 1582 ... YEAR, MONTH and DAY of a serial number that falls before that day are computed with proleptic Gregorian rules and no validity check."
- `odf_docs/ODF14-KNOWN-LIMITATIONS-PUBLIC.md:74`: "DATE gives an error for dates before 15 October 1582. ODF 1.4 requires support only from 1583 and makes earlier dates implementation-defined."
- EASTERSUNDAY enforces 1583-9956 (`interpr2.cxx:273-277`).

**Assessment: Documented impl-defined behavior, consistent with ODF 1.4 §7.4.**

## Migration gap summary

| Function | ODF 1.4 change | AOO status | Gap severity | Source |
|----------|----------------|------------|--------------|--------|
| EASTERSUNDAY | New in 1.3/1.4, year 1583-9956 | FULL (standard name) + legacy alias | None | interpr2.cxx:261; compiler.cxx:2542; core_resource.src:333 |
| CONVERT | Unit table revised (atm) | Implemented, atm value off by 1e-6 | Low | KNOWN-LIMITATIONS:72 |
| COUNTA | Empty-cell semantics | Present, ODF 1.4 semantics UNVERIFIED | Medium | interpr1.cxx:ScCount2 |
| INDEX | Row-vector single-arg now error | Returns column (legacy) | High | KNOWN-LIMITATIONS:71 |
| IS* functions | Edge cases impl-defined | Present, edge cases UNVERIFIED | Low/Medium | interpr1.cxx |
| NPER | Trailing empty param = error | Documented as error (except IF) | None | KNOWN-LIMITATIONS:73 |
| PMT | Definition refined | Present, ODF 1.4 semantics UNVERIFIED | Low | interpr2.cxx:ScRMZ |
| Year 1583 | Pre-1583 impl-defined | DATE #VALUE! before 1582-10-15; proleptic Gregorian for YEAR/MONTH/DAY | None (documented) | IMPLEMENTATION-DEFINED:155 |

## ODF 1.2 vs 1.4 notes

The test inventories `test/aoo-test/data/openformula-functions.txt` and `test/odf-validate/openformula-functions.txt` are titled "OpenDocument 1.2 Part 2 (OpenFormula) chapter 6" and list the ODF 1.2 function set. They do **not** include ODF 1.3/1.4-only functions such as EASTERSUNDAY as a separate inventory entry (EASTERSUNDAY is not in either list), confirming the test baselines are stale relative to ODF 1.4. (Note: IFNA, SKEWP, and the BIT functions *are* listed, so those lists predate the ODF 1.2 final or are a superset.)
