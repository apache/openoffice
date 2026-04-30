# chart2 — Bazel migration

Four DLLs in dependency order: charttools → chartmodel / chartview → chartcontroller.

## Targets

| Target | Output | Notes |
|---|---|---|
| `//main/chart2:chart2_headers` | (headers) | Public `source/inc/` API headers + chartview/ sub-namespace |
| `//main/chart2:charttools` | `charttools.dll` | Core utilities: regression, scaling, data helpers, property wrappers |
| `//main/chart2:charttools_implib` | `charttools.lib` | Import library |
| `//main/chart2:chartmodel` | `chartmodel.dll` | Data model: ChartModel document, chart type objects, XML filter |
| `//main/chart2:chartmodel_implib` | `chartmodel.lib` | Import library |
| `//main/chart2:chartview` | `chartview.dll` | Rendering: axes, chart type plotters, view layer |
| `//main/chart2:chartview_implib` | `chartview.lib` | Import library |
| `//main/chart2:chartcontroller` | `chartcontroller.dll` | Controller/UI: dialogs, API wrappers, accessibility, drag |
| `//main/chart2:chartcontroller_implib` | `chartcontroller.lib` | Import library |

## Key notes

- **charttools** and **chartview** export their full C++ API via `OOO_DLLIMPLEMENTATION_CHARTTOOLS` /
  `OOO_DLLIMPLEMENTATION_CHARTVIEW` defines which expand `OOO_DLLPUBLIC_*` to `SAL_DLLPUBLIC_EXPORT`
  (`__declspec(dllexport)`). No DEF file needed.
- **chartmodel** and **chartcontroller** export only two UNO component entry points
  (`component_getImplementationEnvironment`, `component_getFactory`) via `util/chartmodel.def` and
  `util/chartcontroller.def`. This matches `solenv/src/component.map` — `component_canUnload` is
  not implemented by these modules.
- Three source files disabled in charttools (listed as `DISABLED_SLOFILES` in the old build):
  `NamedFillProperties.cxx`, `NamedLineProperties.cxx`, `NamedProperties.cxx`.
- `.src` resource files in `source/controller/dialogs/` are runtime resources loaded by
  `ResMgr::CreateResMgr("chartcontroller")`; not compiled by Bazel.
- `precompiled_chart2.hxx` is guarded by `#ifdef PRECOMPILED_HEADERS` — no-op in Bazel builds.
- `source/view/axes/TickmarkHelper.cxx` is excluded and should be deleted: its header
  `TickmarkHelper.hxx` no longer exists. The file is a dead leftover from a refactoring that split
  the functionality into `Tickmarks.cxx`, `Tickmarks_Equidistant.cxx`, and `Tickmarks_Dates.cxx`.
  Nothing else in the codebase references `TickmarkHelper.hxx`.
- `source/model/main/LayoutContainer.cxx` is excluded and should be deleted: it depends on
  `com/sun/star/layout/XLayoutContainer.hpp`, which requires an IDL file in the
  `com.sun.star.layout` offapi namespace that was never written. The entire `layout` namespace is
  absent from offapi. Dead prototype code, intentionally omitted from the old SLOFILES.
- The `source/inc/chartview/` sub-directory contains view API headers exposed to both chartview and
  chartcontroller (e.g., `ExplicitValueProvider.hxx`, `DrawModelWrapper.hxx`, `ChartSfxItemIds.hxx`).
- Include path notes:
  - `/Imain/chart2/source/inc` — resolves unqualified common headers (`AxisHelper.hxx`, `macros.hxx`, etc.)
  - `/Imain/chart2/source/view/inc` — resolves view-private headers (`ShapeFactory.hxx`, `VDiagram.hxx`, etc.)
  - `/Imain/chart2/source/controller/inc` — resolves controller-private headers (`DrawViewWrapper.hxx`, `ItemConverter.hxx`, etc.)
- **`.hrc` files must use `hdrs + strip_include_prefix`, not `textual_hdrs`**: controller `.hrc` files
  (e.g. `HelpIds.hrc`, `ResourceIds.hrc`) are `#include`d by `.cxx` files using unqualified names.
  Using `textual_hdrs` only registers them as tracked inputs but does NOT create output-tree symlinks,
  so MSVC's include search through source-tree junctions can silently miss them (C2065, not C1083).
  Using `hdrs + strip_include_prefix` makes Bazel create explicit symlinks in `_virtual_includes/`
  which MSVC reliably finds. See `chartcontroller_inc_hrc`, `chartcontroller_dialogs_hrc`,
  `chartcontroller_main_hrc` targets.
