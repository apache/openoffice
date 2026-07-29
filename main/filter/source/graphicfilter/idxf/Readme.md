# idxf — DXF import filter (developer notes)

Imports AutoCAD DXF drawings into AOO as a GDI metafile (vector graphic). This
document is the developer entry point for the filter: the pipeline, the
coordinate systems that cause most of its bugs, and the root cause of every
defect fixed so far.

> **Status.** The fixes described below are source-only C++ and landed upstream on
> **trunk** via [PR #491](https://github.com/apache/openoffice/pull/491)
> (merge `c082241a9b`, 2026-07-28). They were back-ported onto `bazel-migration`
> with `cherry-pick -x` so the two stay patch-id identical. Not yet ported to
> AOO42X / AOO41X.
>
> **Regression corpus.** The DXF samples, reference renders and per-case analyses
> (`cases/`, ~13.5 MB of Bugzilla attachments) are deliberately **not in this
> tree** — they live on the local branch `bugfix-dxf-filter-open`, together with
> the cases that are still unsolved. The comment in `idxf.cxx` that points at
> `idxf/cases/case-99892` refers to that branch.

## Pipeline / architecture

DXF is a tagged ASCII (or binary) format: a flat stream of (group-code, value)
pairs. Import runs in three stages:

1. **Tokenize** — `DXFGroupReader` (dxfgrprd.cxx) reads the raw group/value
   pairs and exposes typed accessors (`GetG/GetS/GetI/GetF`).
2. **Model** — `DXFRepresentation` (dxfreprd.cxx) parses the SECTIONs into an
   in-memory model:
   - HEADER vars (`$EXTMIN/$EXTMAX`, `$DWGCODEPAGE`, `$LTSCALE`) — dxfreprd.cxx.
   - TABLES (layers, linetypes, VPORTs) — dxftblrd.cxx.
   - BLOCKS (reusable geometry for INSERT) — dxfblkrd.cxx.
   - ENTITIES (the drawing itself) — dxfentrd.cxx. Each entity subclass has an
     `EvaluateGroup()`; unknown groups fall through to `DXFBasicEntity`, which
     handles the shared ones (layer 8, color 62, thickness 39, extrusion
     210/220/230, …).
3. **Render** — `DXF2GDIMetaFile` (dxf2mtf.cxx) walks the entities and emits
   VCL primitives into a `VirtualDevice`, producing the metafile.

`idxf.cxx` is the thin UNO/GraphicFilter entry point; `dxfvec.cxx/.hxx` provides
the `DXFVector` (3D double vector) and `DXFTransform` (the transform matrix)
math used throughout rendering.

Note that the filter is **not** self-contained at link time: `idxf.cxx` reads the
caller's `FilterConfigItem`, so the library links svtools (`$(SVTOOLLIB)` in
makefile.mk; under Bazel the `graphic_filter` macro already supplies it, so
`//main/filter:idx` needed no change).

## Coordinate systems (the source of most bugs)

- **OCS / extrusion (ECS→WCS):** each entity may carry an extrusion direction
  (groups 210/220/230, default (0,0,1)). Non-default extrusion means the entity
  is defined in its own object plane; `DXFTransform(rExtrusion)` builds the
  ECS→WCS matrix via Autodesk's *Arbitrary Axis Algorithm* (dxfvec.cxx:89-102).
  A negative-Z extrusion yields a **reflection** — `DXFTransform::Mirror()` is
  the left-handedness test (dxfvec.cxx:246); it must test the **2D projected**
  determinant, not the 3D one, or negative-Z arcs come out as their complement
  (BZ 16564). Equally important: the transform must only be applied to entities
  whose coordinates are actually in OCS — `DXFCoordsAreWCS` gates the WCS types
  (LINE/POINT/3DFACE/3D polyline), which otherwise scatter (BZ 99893).
- **World → page (scaling):** the drawing is fit to the page using
  `aBoundingBox`. This used to be a trap: if the HEADER supplied
  `$EXTMIN/$EXTMAX` those were used directly and the geometry was never
  measured — but AutoCAD's header extents are stale/zoom-affected, which broke
  scaling (BZ 58347). `Read` now always measures the geometry with
  `CalcBoundingBox` and prefers it, keeping the header only as an empty-box
  fallback. Rules that fell out of the follow-up cases:
  - measure what the renderer **draws**: every entity type `DrawEntities`
    handles (HATCH included), and *only* those (SHAPE/ATTDEF are deliberately
    excluded because the renderer never draws them);
  - measure it the way the renderer draws it: through the entity's ECS→WCS
    transform (`UnionOCS`), else negative-Z geometry inflates the box; and by an
    ARC's **swept** extent, not its full circle, else a large-radius arc with an
    off-sheet centre collapses the drawing (BZ 122565);
  - page-fit leaves a 1% symmetric margin, so geometry sitting exactly on the
    fit boundary isn't edge-clipped (BZ 58347 / 70274).
- **3D:** viewport projection IS wired in — `Convert()` (dxf2mtf.cxx:1069-1134)
  honors the `*ACTIVE` VPORT and, for a non-top view direction, projects via
  `DXFTransform(rViewDir,rViewTarget)` (dxfvec.cxx:105, used at 1126). Only when
  there is no usable (non-top) viewport does it fall back to the flat drop-Z
  projection (`Transform(...,Point)` drops Z, dxfvec.cxx:158). The VPORT lookup
  is case-insensitive — files naming their viewport `*Active` used to miss
  `strcmp("*ACTIVE",…)` and silently render flat (BZ 70273). TEXT in a 3D plane
  goes through `Draw3DTextEntity`, which transforms glyph outlines like ordinary
  geometry.
- **Text encoding:** `$DWGCODEPAGE` → rtl encoding via `DXFCodePageToTextEncoding`
  (dxfreprd.cxx: extracts the code-page number from `ANSI_<cp>`/`DOS<cp>` and
  resolves through `rtl_getTextEncodingFromWindowsCodePage`). Precedence is
  **file > caller > default**: no codepage in the file → the caller's
  `"CharacterSet"` `FilterConfigItem` option (read in idxf.cxx before `Read`, so
  a codepage in the file still wins) → else the system encoding
  (`gsl_getSystemTextEncoding`), but if that is UTF-8/UTF-7/UCS2/UCS4/unknown it
  falls back to MS_1252, because a UTF-8 host would otherwise mis-decode legacy
  single-byte DXF bytes into tofu.

## Defects fixed

| BZ | Title | Area | Root cause / fix |
|----|-------|------|------------------|
| [58347](https://bz.apache.org/ooo/show_bug.cgi?id=58347) | Wrong scaling after import | scaling / extents | Stale header `$EXTMIN/$EXTMAX` used as the bounding box. dxfreprd.cxx: always measure geometry (`CalcBoundingBox`, WCS/extrusion-aware) and prefer it; HATCH measurement gap closed. dxf2mtf.cxx: 1% page-fit margin so a tight box doesn't edge-clip the frame. |
| [16564](https://bz.apache.org/ooo/show_bug.cgi?id=16564) | Arcs imported incorrectly | geometry / arcs | **(a)** Negative-Z extrusion arcs drawn as their complement: `Mirror()` tested the 3D determinant, but the Arbitrary-Axis result for ext (0,0,-1) is a proper rotation that flips only the *in-plane* orientation → now tests the 2D projected determinant, plus an extrusion-aware bbox. **(b)** The R13 file's "arcs" are 14 ELLIPSE + 8 SPLINE that idxf did not support at all → added ELLIPSE and SPLINE (parse + de-Boor/parametric render + bbox). |
| [70274](https://bz.apache.org/ooo/show_bug.cgi?id=70274) | Cutting edges on import | scaling / clip | Duplicate of 58347: width 180 > height 90 put the far edge exactly on the fit boundary → right-edge clip. Resolved by the page-fit margin. |
| [14993](https://bz.apache.org/ooo/show_bug.cgi?id=14993) | Objects lost / incorrect | import (old) | No fix needed. Plain 2D LINE/ARC/POINT/CIRCLE, no extrusion, all supported; an old Linux-only 1.0.2/1.1beta defect, reporter confirms fixed since 3.0.0. Verified by spot-check. |
| [122565](https://bz.apache.org/ooo/show_bug.cgi?id=122565) | Import fails: "unknown graphics format" | detection + reader + scaling | Pro/ENGINEER file, **three** independent bugs: (1) it opens with `999` comment groups but the detector (`ImpPeekGraphicFormat`, svtools/source/filter/filter.cxx) demanded first char `0` + `SECTION` → now skips leading `999`; (2) DIMSTYLE flags `71`/`72` written as floats (`"1.0"`) made `DXFGroupReader::ReadI` reject the value and abort the import → now tolerates a decimal fraction; (3) it then rendered near-empty because DXF_ARC unioned the full circle → swept extent. Also fixed a nearby OOB write, `I170_175[nG-175]` → `nG-170`. |
| [112320](https://bz.apache.org/ooo/show_bug.cgi?id=112320) | Circle rendered as a bar | geometry / LWPOLYLINE bulge | Eagle stores the circle as a *bulged* LWPOLYLINE, not a CIRCLE; the per-vertex bulge (group 42) was dropped, so straight chords were drawn = a bar. Read the bulge and tessellate the arcs in `DrawLWPolyLineEntity` (dxfentrd.hxx/.cxx, dxf2mtf.cxx). |
| [70273](https://bz.apache.org/ooo/show_bug.cgi?id=70273) | Partial import (misaligned) | viewport lookup / 3D | Not missing entities — all imported. The file names its viewport `*Active`, which the case-sensitive `SearchVPort` missed → flat drop-Z projection. Fixed in dxftblrd.cxx. (This file's saved view genuinely *is* top (0,0,1), so top-view rendering is correct by design.) |
| [70275](https://bz.apache.org/ooo/show_bug.cgi?id=70275) | Wrong line type | linetype | A dotted line (`DOT2` = `[0.0,-3.175]`) drew solid/invisible: a `0.0` pattern element is a *dot* (zero-length pen-down), so `fDotLen=0` clamped only to 1/100mm. `LTypeToDXFLineInfo` (dxf2mtf.cxx) now gives a zero-length dot a model-unit length of 0.05·gap so it scales and reads as a dot; real dashes untouched. VCL has no round-dot line style (native Draw is dashed too), so true round dots remain future work. |
| _(no BZ)_ | HATCH pattern fills render as solid blobs | area fill / hatch | Found via 122565: `DrawHatchEntity` ignored `nFlags`, so pattern fills (group 70 = 0) were flood-filled in the entity colour. Now gates solid fill on `nFlags & 1`; for a pattern fill it parses the pattern-definition line (angle 53, offset 45/46 — new fields in dxfentrd.hxx/.cxx) and draws the section lines via VCL `DrawHatch` (angle+spacing mapped through `rTransform`, style from group 78); the boundary tessellates circular-arc edges (type 2) for a correct clip. Every fallback path degrades to no-blob. |
| [99893](https://bz.apache.org/ooo/show_bug.cgi?id=99893) | 3D DXF imported flat / scattered | 3D / extrusion + projection | Viewport projection was already correct; the real cause was applying the OCS/extrusion transform to WCS entities (LINE/POINT/3DFACE/3D polyline) → the pyramid scattered. `DXFCoordsAreWCS` skips extrusion for those types; VPORT lookup made case-insensitive; new `Draw3DTextEntity` puts TEXT in the 3D plane via transformed glyph outlines. |
| [99892](https://bz.apache.org/ooo/show_bug.cgi?id=99892) | Cyrillic / accented text garbled | text / encoding | **Partial — the filter half only.** `$DWGCODEPAGE` now resolves through `DXFCodePageToTextEncoding`; the default is the system encoding with a UTF-8→MS_1252 guard; and a `"CharacterSet"` `FilterConfigItem` override seam lets a higher layer supply the encoding. Nothing populates that option yet — see *Open* below. |
| _(no BZ)_ | Oversized LWPOLYLINE truncated | reader robustness | A vertex count exceeding the reader's limit truncated the polyline; it is now chunked instead. |

## Open / not fixed

Carried on the local branch `bugfix-dxf-filter-open` with the full sample corpus
and per-case analyses:

- **99892 Phase B** — the UI half of the encoding fix. The filter seam exists;
  what is missing is detect-and-prompt (an `SvxTextEncodingBox` chooser) to fill
  the `"CharacterSet"` option. That lives above idxf, in the import/dialog layer.
- **Thumbnail of a text-dense DXF scrambles** — full-size import is correct; only
  the small slide/graphic-selector preview overlaps, because at thumbnail scale
  geometry shrinks while text hits VCL's minimum font size. Fix, if any, belongs
  to the app/VCL thumbnail renderer, not this filter.
- **Confirmed outside idxf** (no work planned here): 75928 and 75922 (metafile
  break/paste behaviour in Draw), 60138 (graphic-link persistence), 97993
  (metafile→PDF export).
- Of 22 collected Bugzilla reports, 14 were analysed; the remainder are
  unexported on that branch.
