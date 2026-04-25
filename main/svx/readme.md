# svx — Bazel Migration

## Outputs
- `svxcore.dll` — core drawing framework: SDR objects, UNO draw, gallery, form model, 3D engine, xoutdev
- `svx.dll` — UI layer: accessibility, dialogs, sidebar panels, toolbar/statusbar controls
- `textconversiondlgs.dll` — Chinese text conversion UNO dialog component

## Key notes
- Split into two DLLs: `svxcore` (model/engine) and `svx` (UI); `svx` links against `svxcore_implib`
- SDI slot generation via `sdi_target` rule (`sdi/svxslots.sdi` → generated headers in `svxslots_sdi_inc/`)
- `svxslots_headers` exposes the generated slot headers; included transitively via `svx_headers`
- Private source headers (`source/inc/`) exposed via `svx_source_inc` (textual_hdrs, used by both DLLs)
- All `source/<subdir>/` directories added to `_COPTS` — each subdir has private headers included without path prefix
- `sidebar/` has 10 sub-panel directories that also need individual include paths
- `accessibletableshape.cxx` lives in `source/table/` (not `source/accessibility/`)
- `BOOST_SPIRIT_USE_OLD_NAMESPACE` required by `EnhancedCustomShapeFunctionParser.cxx` (boost >= 1.36 puts Spirit classic in `boost::spirit::classic` by default)
- `connectivity_headers` needed `textual_hdrs = ["source/parse/sqlbison.hxx"]` + `includes = ["source/parse"]` so `connectivity/sqlparse.hxx` resolves `#include "sqlbison.hxx"` in external consumers

## svidl / sdi_target fixes
- `features = ["-opt"]` on `//main/idl:svidl` — use-after-free in `SvMetaSlot::Insert` triggers under `/O2`; `/Od` in copts alone does not reliably override in MSVC 2008
- svidl links CRT statically (`/MT`); no CRT DLLs or vc90 manifest staged
- Runtime DLLs declared as `data = [...]` on the svidl `cc_binary`; `sdi_target.bzl` stages via `DefaultInfo.data_runfiles` — no hardcoded DLL list in the rule
- `//main/sal:sal` renamed to `//main/sal:sal3` (output is now `sal3.dll`, matching `LIBRARY sal3` in the DEF file); alias `//main/sal:sal` preserved for backward compat
