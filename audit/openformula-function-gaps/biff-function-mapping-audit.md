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

BIFF Formula Function Mapping Audit
====================================

Branch: openformula
HEAD: a330cbd51b (DATEDIF BIFF import mapping fix)
Audit type: Read-only source inspection (no build, no runtime execution)

Updated 2026-11-18 to match the code after the post-merge review. Rows marked
"run" were written to xls by AOO, read back by AOO and opened in Excel 16.

Methodology
-----------

Inspected `main/sc/source/filter/excel/xlformula.cxx` which contains all
BIFF formula function mapping tables:

- saFuncTable_2 (BIFF2, IDs 0-211)
- saFuncTable_3 (BIFF3, IDs 219-244)
- saFuncTable_4 (BIFF4, IDs 214-332)
- saFuncTable_5 (BIFF5/7, IDs 70-354)
- saFuncTable_8 (BIFF8, IDs 358-367)
- saFuncTable_Oox (OOXML future functions, `_xlfn.` prefix, NOID/255)
- saFuncTable_Odf (OpenFormula namespace, `_xlfnodf.` prefix, NOID/255)

Import path: `FillXclFuncMap()` skips EXC_FUNCFLAG_EXPORTONLY entries,
adds entries with non-NOID BIFF function IDs to maXclFuncMap.
Export path: `FillScFuncMap()` skips EXC_FUNCFLAG_IMPORTONLY entries,
adds all others to maScFuncMap keyed by opcode.

BIFF function IDs and Calc opcode IDs are distinct namespaces. A BIFF
function ID is the number Excel uses in its binary file format; a Calc
opcode is AOO's internal representation. The BIFF tables map between them.

Inventory
---------

32 rows. The functions of the OpenFormula compatibility work (H.1-H.9), the
other function-gap entries and the B-function family (LENB/RIGHTB/MIDB/LEFTB).

| # | Function | Opcode | Native Evaluator | BIFF ID | Import | Export | Classification |
|---|----------|--------|-----------------|---------|--------|--------|----------------|
| 1 | NUMBERVALUE | ocNumberValue (392) | Yes | None (_xlfnodf) | NOID import-only | 255 export-only | Correct; Excel shows #NAME? (run) |
| 2 | FINDB | ocFindB (412) | Yes | BIFF2 ID 205 | both | both | Correct (added 2026-11-18; run) |
| 3 | SEARCHB | ocSearchB (413) | Yes | BIFF2 ID 206 | both | both | Correct (added 2026-11-18) |
| 4 | REPLACEB | ocReplaceB (414) | Yes | BIFF2 ID 207 | both | both | Correct (added 2026-11-18) |
| 5 | DATEDIF | ocDateDif (415) | Yes | BIFF5 ID 351 | both | both | Correct (export added 2026-11-18; run) |
| 6 | IFNA | ocIfna | Yes | None (_xlfn) | NOID import-only | 255 export-only | Correct |
| 7 | SKEWP | ocSkewP | Yes | None (_xlfn) | NOID import-only | 255 export-only | Correct |
| 8 | IFERROR | ocIfError | Yes | None (_xlfn) | NOID import-only | 255 export-only | Correct |
| 9 | BITAND | ocBitAnd | Yes | None (_xlfn) | NOID import-only | 255 export-only | Correct |
| 10 | BITOR | ocBitOr | Yes | None (_xlfn) | NOID import-only | 255 export-only | Correct |
| 11 | BITXOR | ocBitXor | Yes | None (_xlfn) | NOID import-only | 255 export-only | Correct |
| 12 | BITLSHIFT | ocBitLShift | Yes | None (_xlfn) | NOID import-only | 255 export-only | Correct |
| 13 | BITRSHIFT | ocBitRShift | Yes | None (_xlfn) | NOID import-only | 255 export-only | Correct |
| 14 | ISOWEEKNUM | ocIsoWeekNum (416) | Yes | None (_xlfn) | NOID import-only | 255 export-only | Correct (added 2026-11-18; run) |
| 15 | WEEKNUM as ODF ISOWEEKNUM | ocWeek (365) | Yes | None (_xlfnodf) | NOID import-only | 255 export-only | Correct |
| 16 | DDE | ocDde (369) | Yes | None | none | none | Intentional limitation |
| 17 | ODDFPRICE | None (AddIn) | No | None | none | none | Intentional limitation |
| 18 | ODDFYIELD | None (AddIn) | No | None | none | none | Intentional limitation |
| 19 | RRI | ocZGZ (256) | Yes | None (_xlfnodf) | NOID import-only | 255 export-only | Correct; Excel shows #NAME? (run) |
| 20 | ERROR.TYPE | ocErrorTypeODF (419) | Yes | BIFF4 ID 261 | both | both | Correct (run) |
| 21 | ERRORTYPE | ocErrorType (151) | Yes | BIFF4 ID 261 | none | export-only | Correct; comes back as ERROR.TYPE (run) |
| 22 | GETPIVOTDATA | ocGetPivotData (390) | Yes | BIFF8 ID 358 | both | both | Correct |
| 23 | FDIST | ocFDist (315) | Yes | BIFF4 ID 281 | both | both | Correct (run) |
| 24 | FINV | ocFInv (359) | Yes | BIFF4 ID 282 | both | both | Correct (run) |
| 25 | F.DIST | ocFDistLeft (417) | Yes | None (`_xlfn.F.DIST`) | NOID import-only | 255 export-only | Correct; Excel calculates 4 arguments, #N/A for 3 (run) |
| 26 | F.INV | ocFInvLeft (418) | Yes | None (`_xlfn.F.INV`) | NOID import-only | 255 export-only | Correct; Excel calculates it (run) |
| 27 | HYPGEOMDIST | ocHypGeomDist (312) | Yes | BIFF4 ID 289 | both, 4 args | both, 4 args | Limitation: 5 args written as NA() (run) |
| 28 | HARMEAN | ocHarMean (321) | Yes | BIFF4 ID 320 | both | both | Correct |
| 29 | LEFTB | ocLeftB (402) | Yes | BIFF2 ID 208 | both | both | Correct |
| 30 | RIGHTB | ocRightB (401) | Yes | BIFF2 ID 209 | both | both | Correct |
| 31 | MIDB | ocMidB (403) | Yes | BIFF2 ID 210 | both | both | Correct |
| 32 | LENB | ocLenB (400) | Yes | BIFF2 ID 211 | both | both | Correct |

Import Findings
---------------

DATEDIF (BIFF5 ID 351): Was mapped to ocNoName → #NAME! on import. Fixed
in commit a330cbd51b to map to ocDateDif.
BIFF8 table (saFuncTable_8) does not include DATEDIF.

ERROR.TYPE (BIFF4 ID 261): reads as ocErrorTypeODF since 2026-11-18. Before
that it read as ocErrorType, so Excel files got AOO's internal error numbers.

All functions in saFuncTable_Oox (`_xlfn.` prefix) have NOID as BIFF
function ID for import, meaning they cannot be imported via BIFF ID
lookup. These are Excel future functions (Excel 2007/2010/2013+) that
don't exist in older BIFF versions. This is intentional.

All functions in saFuncTable_Odf (`_xlfnodf.` prefix) have NOID as BIFF
function ID for import, meaning they cannot be imported via BIFF ID
lookup. These are ODF namespace functions not defined in BIFF. This is
intentional.

DDE has a native evaluator but no BIFF function ID in any AOO table.

Functions ODDFPRICE, ODDFYIELD are Analysis AddIn functions with no native
opcode. Classification: intentional limitation.

Function DDE has a native opcode and evaluator but is not a BIFF function.
Classification: intentional limitation.

Export Findings
---------------

The tables are filled in the order 2, 3, 4, 5, 8, Oox, Odf; for export a
later entry for the same opcode replaces an earlier one.

All functions in saFuncTable_Oox have EXC_FUNCFLAG_EXPORTONLY entries with
BIFF ID 255 (future function marker). These are exported to BIFF as future
functions using the `_xlfn.` name mechanism. Correct for OOXML future
functions.

All functions in saFuncTable_Odf have EXC_FUNCFLAG_EXPORTONLY entries with
BIFF ID 255 (future function marker). These are exported to BIFF using the
`_xlfnodf.` name mechanism. AOO reads them back; Excel shows #NAME? for them.
Until 2026-11-18 the ODF table listed ocFDist for "FDIST" and ocFInv for
"FINV"; it now lists the left-tail ocFDistLeft/ocFInvLeft, which is what ODF
FDIST/FINV mean.

DATEDIF has been written to BIFF ID 351 since 2026-11-18 (it was
import-only), and Excel 16 calculates it. ERRORTYPE has an export-only 261
entry, so it is written as ERROR.TYPE, as before; on reading it becomes
ERROR.TYPE.

HYPGEOMDIST is written as BIFF4 ID 289, which takes four arguments; a call
with the fifth (Cumulative) argument is written as NA().

Functions in saFuncTable_2 (BIFF2 IDs 205-211 for B-functions) and
saFuncTable_4 (BIFF4 IDs 261-320) and saFuncTable_8 (BIFF8 ID 358) have
both import and export mappings. Correct.

Remaining ocNoName entries in BIFF tables:

- DATESTRING (BIFF5 ID 352): No native opcode. Intentional.
- NUMBERSTRING (BIFF5 ID 353): No native opcode. Intentional.
- PHONETIC (BIFF8 ID 360): No native opcode. Intentional.

Adjacent Compatibility Layers
-----------------------------

OOXML/XLSX mappings (saFuncTable_Oox) are separate from BIFF mappings and
use `_xlfn.` prefix names with NOID/255 BIFF IDs. These are not confused with
BIFF support - NOID means no BIFF function ID exists.

ODF namespace mappings (saFuncTable_Odf) use `_xlfnodf.` prefix names with
NOID/255 BIFF IDs. These are distinct from native BIFF function IDs.

BIFF function IDs and Calc opcode IDs are distinct namespaces. BIFF ID 255
is used as a sentinel for "future function" in OOXML/OXDF namespace
mappings, not as a real BIFF function ID for import purposes.

The xlsx import is a separate table in oox/source/xls/formulabase.cxx. Since
2026-11-18 it reads FINDB, SEARCHB, REPLACEB, DATEDIF and ERROR.TYPE (run
with a hand-made xlsx); `_xlfn.F.DIST`, `_xlfn.F.INV`, `_xlfn.NUMBERVALUE` and `_xlfn.RRI` are read too
(run, hand-made xlsx); `_xlfn.ISOWEEKNUM` still gives #NAME? (run).

Defects Found
-------------

1. DATEDIF: BIFF5 ID 351 mapped to ocNoName → #NAME! on import. FIXED
   in commit a330cbd51b (ocNoName → ocDateDif).
2. FINDB, SEARCHB, REPLACEB had no BIFF mapping; FINDB came back from xls as
   #MACRO? in AOO and #NAME? in Excel (run). FIXED 2026-11-18 (205-207).
3. ISOWEEKNUM (ocIsoWeekNum) had no mapping; same symptom. FIXED 2026-11-18
   (`_xlfn.ISOWEEKNUM`).
4. ERROR.TYPE from Excel read as ERRORTYPE. FIXED 2026-11-18.
5. Legacy FDIST and FINV have been written to xls as `_xlfnodf.FDIST` and
   `_xlfnodf.FINV` since the 2011 OOo import (the ODF table is filled last and
   overrode BIFF 281/282). Those names now import as FDIST and FINV, not as
   F.DIST and F.INV (an xls written by the 11-16 build, run). FDIST and FINV
   are now written as BIFF 281/282.

Functions Requiring Follow-up
-----------------------------

- HYPGEOMDIST with five arguments: no xls form (Excel 2010's HYPGEOM.DIST
  would need its own export entry; one opcode has one export entry).
- NUMBERVALUE, RRI: written under `_xlfnodf.` names, which Excel does not
  know; Excel 2013 has both.
- F.DIST with three arguments: written as `_xlfn.F.DIST`, which Excel shows as
  #N/A because it needs four.

Statistics
----------

- Total rows: 32
- Correct mappings: 28
- Limitations: 4 (DDE, ODDFPRICE, ODDFYIELD intentional; HYPGEOMDIST with 5 args)
- Functions with native evaluators but no BIFF mapping: 1 (DDE)
- Functions implemented via AddIn (no native opcode): 2 (ODDFPRICE, ODDFYIELD)

Source References
-----------------

- BIFF function tables: main/sc/source/filter/excel/xlformula.cxx
  - saFuncTable_2: lines 74-221
  - saFuncTable_3: lines 224-249
  - saFuncTable_4: lines 252-326
  - saFuncTable_5: lines 329-350
  - saFuncTable_8: lines 353-368
  - saFuncTable_Oox: lines 371-403
  - saFuncTable_Odf: lines 410-439
- Import path: FillXclFuncMap(), line 492
- Export path: FillScFuncMap(), line 506
- Opcode definitions: main/formula/inc/formula/compiler.hrc
- Opcode enum: main/formula/inc/formula/opcode.hxx
- Function name registration: main/formula/source/core/resource/core_resource.src
- Evaluator dispatch: main/sc/source/core/tool/interpr4.cxx
- xlsx import table: main/oox/source/xls/formulabase.cxx
