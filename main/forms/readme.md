# forms — Bazel migration notes

## Output
`frm.dll` — UNO form controls component (101 .cxx files)

## Structure
All sources compiled as a single `cc_binary`:
- `source/component/` — form control components (Button, CheckBox, ComboBox, Grid, etc.)
- `source/helper/` — navigation and feature helpers
- `source/misc/` — module init, services registry, string IDs
- `source/resource/` — runtime resource manager (`SimpleResMgr`)
- `source/richtext/` — rich text control implementation
- `source/runtime/` — form operations
- `source/solar/component/` — navigation bar control (navbarcontrol)
- `source/solar/control/` — navigation toolbar (navtoolbar)
- `source/xforms/` — XForms model, bindings, datatypes, path expressions
- `source/xforms/submission/` — XForms submission (GET/POST/PUT)
- `source/xforms/xpathlib/` — XPath extension functions for XForms

Private headers live in `source/inc/` (shared across all subdirs) and in each subdir.
`source/solar/inc/navtoolbar.hxx` is the only header under `solar/inc/`.

## Key deps
- ICU: both `icuuc` and `icui18n` — `xforms/datatypes.cxx` uses `unicode/regex.h`
  (RegexMatcher is in icui18n, not icuuc)
- libxml2: XForms XML handling
- editeng, svxcore, dbtools, vos3, i18nisolang1 per original makefile

## Deferred
`.src` resource files (`strings.src`, `xforms.src`) — no rsc pipeline yet.
`frm_resource.cxx` loads strings at runtime via `SimpleResMgr`; silently returns
empty strings until the rsc pipeline is wired up.
