# accessibility — Bazel migration notes

## Outputs
- `acc.dll` — accessibility factory library exposing two entry points:
  - `getStandardAccessibleFactory` — creates VCL widget a11y contexts (buttons, menus, toolbars, …)
  - `getSvtAccessibilityComponentFactory` — creates svt browse-box / grid / tab-bar a11y contexts

## Bazel targets
| Target | Description |
|---|---|
| `//main/accessibility:acc` | `acc.dll` (cc_binary, linkshared) |
| `//main/accessibility:acc_implib` | Import library for downstream linkers |
| `//main/accessibility:accessibility_headers` | Public headers under `inc/accessibility/` |

## Source layout
```
source/helper/   — acc_factory.cxx (factory impl), accresmgr.cxx, characterattributeshelper.cxx
source/standard/ — VCL control wrappers (32 files): menus, buttons, listboxes, toolboxes, …
source/extended/ — BrowseBox, GridControl, TabBar, ListBox, IconChoiceCtrl, ToolPanelDeck (32 files)
source/inc/      — floatingwindowaccessible.hxx (private)
inc/accessibility/ — public headers (helper/, standard/, extended/)
inc/pch/         — precompiled_accessibility.hxx (guarded by PRECOMPILED_HEADERS; not active)
```

## Key decisions
- **PCH**: `/Imain/accessibility/inc/pch` added so `#include "precompiled_accessibility.hxx"` resolves; the header is guarded with `#ifdef PRECOMPILED_HEADERS` so it compiles as a no-op.
- **stlport dep**: `textwindowaccessibility.cxx` uses `<hash_map>`; `//main/stlport:stlport` added.
- **DEF file**: `util/acc.def` created from `util/acc.map` — two global exports only (no `component_getFactory`; this is a support library, not a UNO component).
- **No svx**: build.lst and Library_acc.mk both confirm no svx dependency despite being in the same layer.
- **Java bridge** (`java_uno_accessbridge.dll`, `.jar`): Windows Java Access Bridge — deferred; requires Ant/Maven Java rules not yet wired up.
