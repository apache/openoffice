# reportdesign — Bazel migration

Produces three UNO component DLLs:

| Target | Output | Description |
|--------|--------|-------------|
| `//main/reportdesign:rpt` | `rpt.dll` | Core report engine (model, API, SDraw layer) |
| `//main/reportdesign:rptui` | `rptui.dll` | Report Designer UI (dialogs, inspection, controllers) |
| `//main/reportdesign:rptxml` | `rptxml.dll` | XML import/export filters |

## Dependency graph

```
rptui ─┐
       ├─ rpt (core)
rptxml ┘
```

`rpt` → editeng, basic (sb), tools (tl), svtools (svt), svl, cppuhelper, cppu,
         comphelper, connectivity (dbtools), dbaccess (dbu), unotools (utl), vcl,
         sal, stlport, i18nisolang1, ucbhelper, sfx2, salhelper, framework (fwe),
         svx, svxcore, toolkit (ootk), vos3, sot

`rptui` → rpt + formula (for, forui), dbaccess (dba), sot, vos3

`rptxml` → rpt + xmloff (xo), sot, dbaccess (dba)

## Key notes

- `stringconstants.cxx` is `#include`'d inside `corestrings.cxx`, `uistrings.cxx`,
  and `xmlstrings.cxx` — exposed as `_stringconstants` textual_hdrs.
- `source/ui/report/ReportRuler.cxx` exists on disk but is not in Library_rptui.mk;
  excluded from the build.
- `source/filter/xml/xmlRow.cxx` exists on disk but is not in Library_rptxml.mk;
  excluded from the build.
- DEF files created at `util/rpt.def`, `util/rptui.def`, `util/rptxml.def` with
  the standard two UNO component exports.
