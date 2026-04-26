# filter — Bazel Migration

## What is built

| DLL | Source directory | Export |
|-----|-----------------|--------|
| `msfilter.dll` | `source/msfilter/` | MSO binary codec (EscherEx, BIFF, VBA, DFF) |
| `xmlfa.dll` | `source/xmlfilteradaptor/` | UNO XML filter adaptor component |
| `xfld.dll` | `source/xmlfilterdetect/` | UNO XML filter detector component |
| `svgfilter.dll` | `source/svg/` | SVG import/export filter |
| `pdffilter.dll` | `source/pdf/` | PDF export filter |
| `flash.dll` | `source/flash/` | SWF/Flash export filter |
| `placeware.dll` | `source/placeware/` | Placeware presentation export |
| `t602filter.dll` | `source/t602/` | T602 document import filter |
| `filterconfig1.dll` | `source/config/cache/` | Filter type detection and configuration |
| `xsltfilter.dll` | `source/xsltfilter/` | XSLT-based filter bridge (C++ part) |
| `xsltdlg.dll` | `source/xsltdialog/` | XSLT filter UI dialogs |
| `filtertracer.dll` | `source/filtertracer/` | Filter tracing/debugging utility |
| `egi.dll` | `source/graphicfilter/egif/` | GIF export |
| `eme.dll` | `source/graphicfilter/eos2met/` | OS/2 metafile export |
| `epb.dll` | `source/graphicfilter/epbm/` | PBM export |
| `epg.dll` | `source/graphicfilter/epgm/` | PGM export |
| `epp.dll` | `source/graphicfilter/eppm/` | PPM export |
| `era.dll` | `source/graphicfilter/eras/` | RAS export |
| `eti.dll` | `source/graphicfilter/etiff/` | TIFF export |
| `exp.dll` | `source/graphicfilter/expm/` | XPM export |
| `ept.dll` | `source/graphicfilter/epict/` | PICT export |
| `eps.dll` | `source/graphicfilter/eps/` | EPS export |
| `ime.dll` | `source/graphicfilter/ios2met/` | OS/2 metafile import |
| `ipb.dll` | `source/graphicfilter/ipbm/` | PBM import |
| `icd.dll` | `source/graphicfilter/ipcd/` | PCD import |
| `ipx.dll` | `source/graphicfilter/ipcx/` | PCX import |
| `ipt.dll` | `source/graphicfilter/ipict/` | PICT import |
| `idx.dll` | `source/graphicfilter/idxf/` | DXF import |
| `iti.dll` | `source/graphicfilter/itiff/` | TIFF import |
| `ipd.dll` | `source/graphicfilter/ipsd/` | PSD import |
| `itg.dll` | `source/graphicfilter/itga/` | TGA import |
| `ira.dll` | `source/graphicfilter/iras/` | RAS import |
| `ips.dll` | `source/graphicfilter/ieps/` | EPS import |
| `icg.dll` | `source/graphicfilter/icgm/` | CGM import |

## Key migration decisions

**DEF files** — Component DLLs use `util/component.def` (standard UNO exports:
`component_getImplementationEnvironment` + `component_getFactory`).  Graphic
filters share `util/GraphicExport.def`, `util/GraphicImport.def`, or
`util/ImportCGM.def` since all filters in each category export the same symbol.

**Graphic filter macro** — The `graphic_filter()` Starlark macro reduces
boilerplate for the 22 graphic filter DLLs.  `icgm` passes `extra_copts`
because it uses angle-bracket local includes (`<cgm.hxx>`, `<main.hxx>`).
`idxf` uses relative includes so no extra `/I` is needed.

**Resource files** — `.src` resource files (impdialog.src, pdf.src,
impswfdialog.src, t602filter.src, xsltdialog/*.src, eps/epsstr.src) are
**skipped** — no rsc compilation pipeline exists in the Bazel build yet.
UI strings/dialogs from those targets will be absent at runtime.

**Java components skipped** — `xsltvalidate` (pure Java) and the Java half of
`xsltfilter` (XSLTFilter.jar) require a Java pipeline not yet implemented.
`svgimport.cxx` (JVM-based SVG import) is also omitted.

**COMPMOD_NAMESPACE** — `xmlfa` and `xfld` define `COMPMOD_NAMESPACE=framework`
as in their makefiles (used for internal namespace routing).

**Z_PREFIX / SYSTEM_ZLIB** — `flash.dll` defines both to match the zlib build
convention used throughout this project.
