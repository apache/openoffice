OpenFormula Function Compatibility Gap Audit — Inventory
=========================================================

Branch: openformula
Starting commit: 6a2f742073
Audit type: Read-only source inspection (no build, no runtime execution)

Summary table of all 15 audited functions.

| Function | Support Status | Native Opcode | Evaluator | Help | ODF 1.4 Defined | ODF Group | Excel XLS Import | Excel XLSX Import | Excel XLS Export | Excel XLSX Export | Failure Type | Priority | Confidence |
|----------|---------------|---------------|-----------|------|-----------------|-----------|------------------|-------------------|------------------|-------------------|--------------|----------|------------|
| FINDB | Present | ocFinDB (412) | ScFindB() | Yes (HID_FUNC_FINDB) | No | N/A | No | No | No | No | None — implementation complete at source level | Resolved (was P1) | HIGH |
| REPLACEB | Present | ocReplaceB (414) | ScReplaceB() | Yes (HID_FUNC_REPLACEB) | No | N/A | No | No | No | No | None — implementation complete at source level | Resolved (was P1) | HIGH |
| SEARCHB | Present | ocSearchB (413) | ScSearchB() | Yes (HID_FUNC_SEARCHB) | No | N/A | No | No | No | No | None — implementation complete at source level | Resolved (was P1) | HIGH |
| DATEDIF | Missing (import→#NAME!) | ocNoName | None | None | No | N/A | Yes (BIFF5/7, ID 351, →#NAME!) | No | No | No | Import maps to ocNoName → #NAME! | P1 | HIGH |
| ISOWEEKNUM | Partial (wrong semantics) | ocWeek | ScGetWeekOfYear() | German only (KALENDERWOCHE) | No | N/A | No | Yes (ODF table, import-only) | No | _xlfnodf.ISOWEEKNUM | ISO 8601 semantics not implemented (only SUN/MON start) | P1 | MEDIUM |
| DDE | Present (platform-dependent) | ocDde (369) | ScDde() | Yes (HID_FUNC_DDE) | No | N/A | No | No | No | No | Platform-dependent; no file-format mapping | DEFER | HIGH |
| ODDFPRICE | Present (AddIn) | None | getOddfprice() (AddIn) | Yes (HID_AAI_FUNC_ODDFPRICE) | No | N/A | No | No | No | No | Analysis AddIn, not native | P2 | HIGH |
| ODDFYIELD | Present (AddIn) | None | getOddfyield() (AddIn) | Yes (HID_AAI_FUNC_ODDFYIELD) | No | N/A | No | No | No | No | Analysis AddIn, not native | P2 | HIGH |
| RRI | Present (validation gaps) | ocZGZ | ScZGZ() | German only (ZGZ) | No | N/A | No | Yes (ODF table, import-only) | No | _xlfnodf.RRI | No input validation (zero/neg PV, zero periods, overflow); German help only | P2 | HIGH |
| ERROR.TYPE | Present (mapping gap) | ocErrorType (151) | ScErrorType() | German only (FEHLERTYP) | Yes | Verify (likely Large Group) | No | No | No | No | Internal error codes not mapped to ODF return values (1–7); German help only | P1 | MEDIUM |
| NUMBERVALUE | Present | ocNumberValue (392) | ScNumberValue() | Yes (HID_FUNC_NUMBERVALUE) | Yes | Verify (likely Large Group) | No | Yes (ODF table, import-only) | No | _xlfnodf.NUMBERVALUE | None — implementation complete at source level | Resolved (was P1) | HIGH |
| GETPIVOTDATA | Present | ocGetPivotData (390) | ScGetPivotData() | Yes (HID_FUNC_GETPIVOTDATA) | No | N/A | Yes (BIFF8, ID 358) | No | Yes (BIFF8) | No | DataPilot-dependent; XLSX import/export mapping missing | P2 | HIGH |
| FDIST | Present | ocFDist (315) | ScFDist() | German only (FVERT) | No | N/A | No | Yes (ODF table, import-only) | No | _xlfnodf.FDIST | Param count (3 vs ODF 3–4); German help only; _xlfnodf export naming | P2 | HIGH |
| FINV | Present | ocFInv (359) | ScFInv() | Yes (HID_FUNC_FINV) | No | N/A | No | Yes (ODF table, import-only) | No | _xlfnodf.FINV | _xlfnodf export naming (Excel has FINV natively) | P2 | HIGH |
| HARMEAN | Present | ocHarmMean (321) | ScHarMean() | German only (HARMITTEL) | Yes | Verify (likely Large Group) | Not in reviewed tables | Not in reviewed tables | Not in reviewed tables | Not in reviewed tables | German help only; BIFF main-table mapping unverified | P3 | HIGH |

Notes:
- "Native Opcode" column lists SC_OPCODE_* constants from
  main/formula/inc/formula/compiler.hrc.
- "Evaluator" column lists the ScInterpreter::Sc*() implementation
  or Analysis AddIn entry point.
- "Help" column indicates whether a HID_FUNC_* help ID exists and
  under what language name.
- "ODF Group" classification requires verification against the final
  ODF 1.4 Part 4 specification (Small/Medium/Large Group).
- Priority: P1 = ODF-required behavior missing/incorrect; P2 = Excel
  compatibility gap; P3 = documentation/alias/minor edge case;
  DEFER = substantial platform/application-level work or unresolved
  spec/security questions.
- Confidence: HIGH = direct source evidence; MEDIUM = source evidence
  exists but runtime behavior unverified.
