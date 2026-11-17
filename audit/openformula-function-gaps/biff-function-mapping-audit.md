BIFF Formula Function Mapping Audit
====================================

Branch: openformula
HEAD: a330cbd51b (DATEDIF BIFF import mapping fix)
Audit type: Read-only source inspection (no build, no runtime execution)

Methodology
-----------
Inspected `main/sc/source/filter/excel/xlformula.cxx` which contains all
BIFF formula function mapping tables:

- saFuncTable_2 (BIFF2, IDs 0-211)
- saFuncTable_3 (BIFF3, IDs 219-244)
- saFuncTable_4 (BIFF4, IDs 214-332)
- saFuncTable_5 (BIFF5/7, IDs 70-354)
- saFuncTable_8 (BIFF8, IDs 358-367)
- saFuncTable_Oox (OOXML future functions, _xlfn. prefix, NOID/255)
- saFuncTable_Odf (OpenFormula namespace, _xlfnodf. prefix, NOID/255)

Import path: `FillXclFuncMap()` skips EXC_FUNCFLAG_EXPORTONLY entries,
adds entries with non-NOID BIFF function IDs to maXclFuncMap.
Export path: `FillScFuncMap()` skips EXC_FUNCFLAG_IMPORTONLY entries,
adds all others to maScFuncMap keyed by opcode.

BIFF function IDs and Calc opcode IDs are distinct namespaces. A BIFF
function ID is the number Excel uses in its binary file format; a Calc
opcode is AOO's internal representation. The BIFF tables map between them.

Inventory
---------

27 functions inventoried. All functions recently implemented or reviewed
in the OpenFormula compatibility work (H.1–H.3) plus existing function-gap
inventory entries and the B-function family (LENB/RIGHTB/MIDB/LEFTB).

| # | Function | Opcode | Native Evaluator | BIFF ID | Import | Export | Classification |
|---|----------|--------|-----------------|---------|--------|--------|----------------|
| 1 | NUMBERVALUE | ocNumberValue (392) | Yes | None (_xlfnodf) | NOID import-only | 255 export-only | Correct |
| 2 | FINDB | ocFinDB (412) | Yes | None | none | none | Unclear |
| 3 | SEARCHB | ocSearchB (413) | Yes | None | none | none | Unclear |
| 4 | REPLACEB | ocReplaceB (414) | Yes | None | none | none | Unclear |
| 5 | DATEDIF | ocDateDif (415) | Yes | BIFF5 ID 351 | ocDateDif import-only | none | Correct (was ocNoName defect, fixed) |
| 6 | IFNA | ocIfna | Yes | None (_xlfn) | NOID import-only | 255 export-only | Correct |
| 7 | SKEWP | ocSkewP | Yes | None (_xlfn) | NOID import-only | 255 export-only | Correct |
| 8 | IFERROR | ocIfError | Yes | None (_xlfn) | NOID import-only | 255 export-only | Correct |
| 9 | BITAND | ocBitAnd | Yes | None (_xlfn) | NOID import-only | 255 export-only | Correct |
| 10 | BITOR | ocBitOr | Yes | None (_xlfn) | NOID import-only | 255 export-only | Correct |
| 11 | BITXOR | ocBitXor | Yes | None (_xlfn) | NOID import-only | 255 export-only | Correct |
| 12 | BITLSHIFT | ocBitLShift | Yes | None (_xlfn) | NOID import-only | 255 export-only | Correct |
| 13 | BITRSHIFT | ocBitRShift | Yes | None (_xlfn) | NOID import-only | 255 export-only | Correct |
| 14 | ISOWEEKNUM | ocWeek | Yes | None (_xlfnodf) | NOID import-only | 255 export-only | Correct |
| 15 | DDE | ocDde (369) | Yes | None | none | none | Intentional limitation |
| 16 | ODDFPRICE | None (AddIn) | No | None | none | none | Intentional limitation |
| 17 | ODDFYIELD | None (AddIn) | No | None | none | none | Intentional limitation |
| 18 | RRI | ocZGZ | Yes | None (_xlfnodf) | NOID import-only | 255 export-only | Correct |
| 19 | ERROR.TYPE | ocErrorType (151) | Yes | BIFF4 ID 261 | ocErrorType both | ocErrorType both | Correct |
| 20 | GETPIVOTDATA | ocGetPivotData (390) | Yes | BIFF8 ID 358 | ocGetPivotData both | ocGetPivotData both | Correct |
| 21 | FDIST | ocFDist (315) | Yes | BIFF4 ID 281 | ocFDist both | ocFDist both | Correct |
| 22 | FINV | ocFInv (359) | Yes | BIFF4 ID 282 | ocFInv both | ocFInv both | Correct |
| 23 | HARMEAN | ocHarmMean (321) | Yes | BIFF4 ID 320 | ocHarmMean both | ocHarmMean both | Correct |
| 24 | LEFTB | ocLeftB (402) | Yes | BIFF2 ID 208 | ocLeftB both | ocLeftB both | Correct |
| 25 | RIGHTB | ocRightB (401) | Yes | BIFF2 ID 209 | ocRightB both | ocRightB both | Correct |
| 26 | MIDB | ocMidB (403) | Yes | BIFF2 ID 210 | ocMidB both | ocMidB both | Correct |
| 27 | LENB | ocLenB (400) | Yes | BIFF2 ID 211 | ocLenB both | ocLenB both | Correct |

Import Findings
---------------

DATEDIF (BIFF5 ID 351): Was mapped to ocNoName → #NAME! on import. Fixed
in commit a330cbd51b to map to ocDateDif. Import-only flag preserved.
BIFF8 table (saFuncTable_8) does not include DATEDIF.

All functions in saFuncTable_Oox (_xlfn. prefix) have NOID as BIFF
function ID for import, meaning they cannot be imported via BIFF ID
lookup. These are Excel future functions (Excel 2007/2010/2013+) that
don't exist in older BIFF versions. This is intentional.

All functions in saFuncTable_Odf (_xlfnodf. prefix) have NOID as BIFF
function ID for import, meaning they cannot be imported via BIFF ID
lookup. These are ODF namespace functions not defined in BIFF. This is
intentional.

Functions FINDB, SEARCHB, REPLACEB, DDE have native evaluators but no
BIFF function ID in any AOO table. BIFF may define IDs for FINDB/SEARCHB/
REPLACEB in later versions (Excel does have these functions), but AOO does
not implement those mappings. Classification: unclear.

Functions ODDFPRICE, ODDFYIELD are Analysis AddIn functions with no native
opcode. Classification: intentional limitation.

Function DDE has a native opcode and evaluator but is not a BIFF function.
Classification: intentional limitation.

Export Findings
---------------

All functions in saFuncTable_Oox have EXC_FUNCFLAG_EXPORTONLY entries with
BIFF ID 255 (future function marker). These are exported to BIFF as future
functions using the _xlfn. name mechanism. Correct for OOXML future
functions.

All functions in saFuncTable_Odf have EXC_FUNCFLAG_EXPORTONLY entries with
BIFF ID 255 (future function marker). These are exported to BIFF using the
_xlfnodf. name mechanism. Correct for ODF namespace functions.

DATEDIF is import-only in saFuncTable_5 (BIFF ID 351). No export mapping
exists. DATEDIF is imported from BIFF but not exported back to BIFF — this
is an intentional import-only policy, not an export defect. The function's
BIFF import mapping was corrected from ocNoName to ocDateDif in commit
a330cbd51b.

Functions in saFuncTable_2 (BIFF2 IDs 208-211 for B-functions) and
saFuncTable_4 (BIFF4 IDs 261-320) and saFuncTable_8 (BIFF8 ID 358) have
both import and export mappings. Correct.

Remaining ocNoName entries in BIFF tables:
- DATESTRING (BIFF5 ID 352): No native opcode. Intentional.
- NUMBERSTRING (BIFF5 ID 353): No native opcode. Intentional.
- PHONETIC (BIFF8 ID 360): No native opcode. Intentional.

Adjacent Compatibility Layers
-----------------------------

OOXML/XLSX mappings (saFuncTable_Oox) are separate from BIFF mappings and
use _xlfn. prefix names with NOID/255 BIFF IDs. These are not confused with
BIFF support - NOID means no BIFF function ID exists.

ODF namespace mappings (saFuncTable_Odf) use _xlfnodf. prefix names with
NOID/255 BIFF IDs. These are distinct from native BIFF function IDs.

BIFF function IDs and Calc opcode IDs are distinct namespaces. BIFF ID 255
is used as a sentinel for "future function" in OOXML/OXDF namespace
mappings, not as a real BIFF function ID for import purposes.

Defects Found
-------------

1. DATEDIF: BIFF5 ID 351 mapped to ocNoName → #NAME! on import. FIXED
   in commit a330cbd51b (ocNoName → ocDateDif). No remaining defect.

No remaining BIFF import or export defects were found for the inventoried
functions.

Functions Requiring Follow-up
-----------------------------

- FINDB, SEARCHB, REPLACEB: No BIFF function ID in any AOO table
  (saFuncTable_2/3/4/5/8, saFuncTable_Oox, saFuncTable_Odf). The related
  extract functions LEFTB/RIGHTB/MIDB/LENB DO have BIFF2 IDs (208-211),
  but the find/replace/search variants are absent from all tables.
  Classification: unclear — AOO has native evaluators and opcode
  dispatch, but no BIFF import/export mapping exists. XLS import of
  FINDB/SEARCHB/REPLACEB formulas would fail in AOO.

- DATEDIF import: Supported via BIFF5 ID 351 → ocDateDif (import-only).
  No BIFF export mapping exists by design. Not a defect — import-only
  policy is intentional.

Statistics
----------
- Total functions inventoried: 27
- Correct mappings: 21
- Import defects: 0 (DATEDIF ocNoName defect was fixed before this audit)
- Export defects: 0
- Intentional limitations: 3 (DDE, ODDFPRICE, ODDFYIELD)
- Unclear: 3 (FINDB, SEARCHB, REPLACEB - BIFF function IDs unknown in AOO tables)
- Functions with native evaluators but no BIFF mapping: 4 (FINDB, SEARCHB, REPLACEB, DDE)
- Functions implemented via AddIn (no native opcode): 2 (ODDFPRICE, ODDFYIELD)

Source References
-----------------
- BIFF function tables: main/sc/source/filter/excel/xlformula.cxx
  - saFuncTable_2: lines 74-218
  - saFuncTable_3: lines 220-246
  - saFuncTable_4: lines 248-322
  - saFuncTable_5: lines 324-346
  - saFuncTable_8: lines 348-364
  - saFuncTable_Oox: lines 366-393
  - saFuncTable_Odf: lines 395-430
- Import path: FillXclFuncMap(), lines 481-493
- Export path: FillScFuncMap(), lines 495-500
- Opcode definitions: main/formula/inc/formula/compiler.hrc
- Opcode enum: main/formula/inc/formula/opcode.hxx
- Function name registration: main/formula/source/core/resource/core_resource.src
- Evaluator dispatch: main/sc/source/core/tool/interpr4.cxx
