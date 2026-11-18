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

OpenFormula Function Compatibility Gap Audit — Inventory
=========================================================

Branch: openformula
Starting commit: 6a2f742073
Audit type: Read-only source inspection (no build, no runtime execution)

Updated 2026-11-18 to match the code after the post-merge review. The
working tree was built on macOS and Linux and the functions were run and,
where Excel has them, compared with Excel 16; "run" in the Confidence column
means that. The ODF Group column and DDE/GETPIVOTDATA ODF status are from
the original audit and were not rechecked.

Summary table of the 15 audited functions plus HYPGEOMDIST (H.9).

| Function | Support Status | Native Opcode | Evaluator | Help | ODF 1.4 Defined | ODF Group | Excel XLS Import | Excel XLSX Import | Excel XLS Export | Excel XLSX Export | Failure Type | Priority | Confidence |
|----------|---------------|---------------|-----------|------|-----------------|-----------|------------------|-------------------|------------------|-------------------|--------------|----------|------------|
| FINDB | Present | ocFindB(412) | ScFindB() | Yes (HID_FUNC_FINDB) | Yes (§6.7.2) | not checked | Yes(BIFF2,ID205) | Yes | Yes(BIFF2,ID205) | n/a (no xlsx export) | None; argument order fixed 2026-11-18 | Resolved | HIGH (run, Excel 16) |
| REPLACEB | Present | ocReplaceB(414) | ScReplaceB() | Yes (HID_FUNC_REPLACEB) | Yes (§6.7.6) | not checked | Yes(BIFF2,ID207) | Yes | Yes(BIFF2,ID207) | n/a (no xlsx export) | None | Resolved | HIGH (run, Excel 16) |
| SEARCHB | Present | ocSearchB(413) | ScSearchB() | Yes (HID_FUNC_SEARCHB) | Yes (§6.7.8) | not checked | Yes(BIFF2,ID206) | Yes | Yes(BIFF2,ID206) | n/a (no xlsx export) | None; argument order fixed 2026-11-18; regular expressions, no wildcards | Resolved | HIGH (run, Excel 16) |
| DATEDIF | Present | ocDateDif(415) | ScDateDif() | Yes (HID_FUNC_DATEDIF) | Yes (§6.10.3) | Large Group (not rechecked) | Yes(BIFF5,ID351) | Yes | Yes(BIFF5,ID351) | n/a (no xlsx export) | Excel-compatible where ODF is silent (Jim): start>end error, case-insensitive, Excel md/yd; one Feb 29 yd case differs from Excel | Resolved | HIGH (run, Excel 16) |
| ISOWEEKNUM | Present | ocIsoWeekNum(416) | ScIsoWeekNum() | Yes (HID_FUNC_ISOWEEKNUM) | Yes | Large Group (not rechecked) | Yes(_xlfn.ISOWEEKNUM) | No (_xlfn.ISOWEEKNUM gives #NAME?) | Yes(_xlfn.ISOWEEKNUM) | n/a (no xlsx export) | ODF one-argument ISOWEEKNUM read as ocIsoWeekNum (FormulaCompiler::Factor); two arguments stay WEEKNUM | Resolved | HIGH (run, Excel 16) |
| DDE | Present (platform-dependent) | ocDde(369) | ScDde() | Yes (HID_FUNC_DDE) | not checked | N/A | No | No | No | n/a (no xlsx export) | Platform-dependent; no file-format mapping | DEFER | HIGH (source) |
| ODDFPRICE | Present (AddIn) | None | getOddfprice() (AddIn) | Yes (HID_AAI_FUNC_ODDFPRICE) | Yes (§6.12.31) | not checked | No | No | No | n/a (no xlsx export) | Implemented 2026-11-18 to Excel formulas; 14 of 4524 cases differ from Excel (classified) | Resolved (native opcode deferred) | HIGH (run, Excel 16) |
| ODDFYIELD | Present (AddIn) | None | getOddfyield() (AddIn) | Yes (HID_AAI_FUNC_ODDFYIELD) | Yes (§6.12.32) | not checked | No | No | No | n/a (no xlsx export) | Implemented 2026-11-18; bracketed Illinois solver | Resolved (native opcode deferred) | HIGH (run, Excel 16) |
| RRI | Present | ocZGZ(256) | ScZGZ() | Yes (HID_FUNC_ZGZ, 04060118.xhp) | Yes (§6.12.44) | not checked | Yes(_xlfnodf.RRI) | Yes (_xlfn.RRI) | _xlfnodf.RRI (Excel #NAME?) | n/a (no xlsx export) | No validation of N > 0; ODF/English name RRI since 2026-11-18, ZGZ alias | P3 | HIGH (run, Excel 16) |
| ERROR.TYPE | Present | ocErrorTypeODF(419) | ScErrorType(true) | Yes (HID_FUNC_ERROR_TYPE_ODF) | Yes (§6.13.11) | Large Group (not rechecked) | Yes(BIFF4,ID261) | Yes | Yes(BIFF4,ID261) | n/a (no xlsx export) | Own opcode since 2026-11-18 (Jim); mapping like XclTools::GetXclErrorCode; ERRORTYPE unchanged | Resolved | HIGH (run, Excel 16) |
| NUMBERVALUE | Present | ocNumberValue(392) | ScNumberValue() | Yes (HID_FUNC_NUMBERVALUE) | Yes (§6.13.28) | not checked | Yes(_xlfnodf.NUMBERVALUE) | Yes (_xlfn.NUMBERVALUE) | _xlfnodf.NUMBERVALUE (Excel #NAME?) | n/a (no xlsx export) | ODF steps since 2026-11-18; empty text and empty GroupSeparator differ from Excel on purpose | Resolved | HIGH (run, Excel 16) |
| GETPIVOTDATA | Present | ocGetPivotData(390) | ScGetPivotData() | Yes (HID_FUNC_GETPIVOTDATA) | not checked | N/A | Yes(BIFF8,ID358) | Yes (import-only row, not run) | Yes(BIFF8) | n/a (no xlsx export) | DataPilot-dependent | P3 | HIGH (source) |
| FDIST | Present | ocFDistLeft(417) for ODF FDIST; ocFDist(315) is LEGACY.FDIST | ScFDistLeft() | Yes (HID_FUNC_F_DIST_LEFT) | Yes (§6.18.22) | N/A | Yes(`_xlfn.F.DIST`; `_xlfnodf.FDIST` reads as legacy FDIST) | Yes (`_xlfn.F.DIST`, 4 args) | `_xlfn.F.DIST` (Excel calculates 4 args, #N/A for 3) | n/a (no xlsx export) | New opcode 2026-11-18 (Jim); legacy FDIST unchanged | Resolved | HIGH (run, Excel 16) |
| FINV | Present | ocFInvLeft(418) for ODF FINV; ocFInv(359) is LEGACY.FINV | ScFInvLeft() | Yes (HID_FUNC_F_INV_LEFT) | Yes (§6.18.24) | N/A | Yes(`_xlfn.F.INV`; `_xlfnodf.FINV` reads as legacy FINV) | Yes (`_xlfn.F.INV`) | `_xlfn.F.INV` (Excel calculates it) | n/a (no xlsx export) | New opcode 2026-11-18 (Jim); legacy FINV unchanged | Resolved | HIGH (run, Excel 16) |
| HYPGEOMDIST | Present | ocHypGeomDist(312) | ScHypGeomDist() | Yes (HID_FUNC_HYPGEOMVERT) | Yes (§6.18.37) | not checked | Yes(BIFF4,ID289) | Yes | Yes(BIFF4,ID289; 5 args written as NA()) | n/a (no xlsx export) | Cumulative added (H.9); PMF replaced 2026-11-18 (trunk gave 3.2E+181 for large args) | Resolved | HIGH (run, Excel 16) |
| HARMEAN | Present | ocHarMean(321) | ScHarMean() | Yes (HID_FUNC_HARMITTEL, 04060182.xhp) | Yes (not rechecked) | not checked | Yes(BIFF4,ID320) | Yes | Yes(BIFF4,ID320) | n/a (no xlsx export) | None found | P3 | HIGH (source) |

Notes:

- "Native Opcode" column lists SC_OPCODE_* constants from
  main/formula/inc/formula/compiler.hrc.
- "Evaluator" column lists the ScInterpreter::Sc*() implementation
  or Analysis AddIn entry point.
- "Help" column indicates whether a HID_FUNC_* help ID exists. Help ID
  names are German (HID_FUNC_ZGZ, HID_FUNC_HARMITTEL); the help text is
  English.
- FDIST and FINV rows are the ODF functions (left tail), which AOO calls
  F.DIST and F.INV since 2026-11-18. AOO's FDIST and FINV are ODF
  LEGACY.FDIST and LEGACY.FINV and did not change.
- "ODF Group" classification requires verification against the final
  ODF 1.4 Part 4 specification (Small/Medium/Large Group).
- Priority: P1 = ODF-required behavior missing/incorrect; P2 = Excel
  compatibility gap; P3 = documentation/alias/minor edge case;
  DEFER = substantial platform/application-level work or unresolved
  spec/security questions.
- Confidence: HIGH = direct source evidence; "run" = also run on builds,
  "Excel 16" = also compared with Excel; "source" = read only.

BIFF Compatibility
------------------

See audit/openformula-function-gaps/biff-function-mapping-audit.md for the
detailed BIFF import/export mapping audit and
audit/openformula-function-gaps/biff-function-mapping.tsv for the
machine-readable inventory.

Key findings:

- DATEDIF BIFF5 ID 351 was mapped to ocNoName (→ #NAME! on import) and
  was fixed to ocDateDif in commit a330cbd51b. Since 2026-11-18 it is
  also written to xls.
- FINDB, SEARCHB, REPLACEB had no BIFF mapping; since 2026-11-18 they are
  BIFF2 IDs 205-207, next to LEFTB/RIGHTB/MIDB/LENB (208-211), and xlsx
  reads them.
- ISOWEEKNUM (ocIsoWeekNum) is written as `_xlfn.ISOWEEKNUM` since
  2026-11-18; WEEKNUM keeps its `_xlfnodf.ISOWEEKNUM` entry.
- ERROR.TYPE (BIFF4 ID 261) reads as the new ocErrorTypeODF; ERRORTYPE
  is still written as 261.
- HYPGEOMDIST with the fifth argument cannot be written to xls (NA()).
- DDE has a native opcode and evaluator but no BIFF mapping (intentional
  limitation — DDE is not a BIFF function).
- ODDFPRICE and ODDFYIELD are Analysis AddIn functions with no native
  opcode and no BIFF mapping (intentional limitation).
- OOXML future functions (IFNA, IFERROR, SKEWP, BITAND, BITOR, BITXOR,
  BITLSHIFT, BITRSHIFT, ISOWEEKNUM, F.DIST, F.INV) use `_xlfn.` names and
  ODF namespace functions (WEEKNUM as ISOWEEKNUM, NUMBERVALUE, RRI) use
  `_xlfnodf.` names, with NOID/255 BIFF IDs. Excel shows #NAME? for the
  `_xlfnodf.` ones. `_xlfnodf.FDIST` and `_xlfnodf.FINV`, which older
  versions wrote for FDIST and FINV, import as FDIST and FINV.

Classification summary (32 rows in biff-function-mapping.tsv):

- Correct: 28
- Limitations: 4 (DDE, ODDFPRICE, ODDFYIELD intentional; HYPGEOMDIST with
  5 arguments)
- Import defects: 0
- Export defects: 0
