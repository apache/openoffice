# framework — Bazel migration

## Outputs

| Target | DLL | Description |
|--------|-----|-------------|
| `:fwi` | `fwi.dll` | Framework interfaces (thread helpers, item containers, protocol cache) |
| `:fwe` | `fwe.dll` | Framework widgets engine (menus, toolbox config, action triggers) |
| `:fwk` | `fwk.dll` | Framework core (Desktop, Frame, LayoutManager, accelerators, UNO services) |
| `:fwl` | `fwl.dll` | Framework UI element controllers (font menus, dispatch helpers) |
| `:fwm` | `fwm.dll` | Framework module misc (tab window, shell job, help-on-startup) |

Import libraries: `:fwi_implib`, `:fwe_implib`, `:fwk_implib`, `:fwl_implib`, `:fwm_implib`

Public headers: `:framework_headers` (strip_include_prefix = "inc")

## Dependency graph

```
fwi  (sal, cppu, cppuhelper, comphelper, svl, svt, ootk, utl, tl, i18nisolang1, vcl, vos3, advapi32)
fwe  → fwi
fwk  → fwi, fwe  (+ sot, ucbhelper)
fwl  → fwi, fwe
fwm  → fwi        (+ vos3)
```

## Notes

- Several source directories (`source/classes/`, `source/dispatch/`, `source/jobs/`,
  `source/register/`, `source/services/`, `source/uielement/`) are split across multiple
  DLLs — sources are listed explicitly rather than globbed.
- `source/services/uriabbreviation.cxx` is compiled into both fwk and fwl (separate
  translation units in separate DLLs).
- `fwk` includes the precompiled header stub `inc/pch/precompiled_framework.cxx`.
- All DLLs define `snprintf=_snprintf`, `snwprintf=_snwprintf`, and `/Zc:wchar_t-` per project-wide conventions.
  `snwprintf` is needed by `spinfieldtoolbarcontroller.cxx` which calls the wide-char variant directly.
- `fwi` links `advapi32.lib` (Windows registry access in `networkdomain.cxx`).
