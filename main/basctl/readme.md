# basctl — Basic IDE

Bazel target: `//main/basctl:basctl` → `basctl.dll`

## Outputs

| Target | Output |
|---|---|
| `//main/basctl:basctl` | `basctl.dll` — Basic IDE shell, dialog editor, accessibility |
| `//main/basctl:basctl_implib` | `basctl.if.lib` — import library |

## Dependencies

- **basic** (sb.dll — StarBasic runtime)
- **sfx2** (sfx.dll — framework)
- **svx** (svxcore + svx — drawing/shape layer)
- **editeng** (editeng.dll — text editing)
- **vcl, svtools, svl, tools, unotools, sot** (core UI/util layer)
- **framework** (fwe.dll — frame/window environment)
- **toolkit** (ootk.dll — UNO toolkit)
- **xmlscript** (xcr.dll — dialog XML import/export)
- **ucbhelper** (UCB helper)
- **stlport** — hash_map used in bastypes.hxx

## SDI pipeline

`sdi/basslots.sdi` (includes `baside.sdi`) is processed by `svidl.exe` to generate
`basslots_sdi_inc/basslots.hxx`. The SDI target depends on sfx2 and svx SDI packages
for slot inheritance.

## Notes

- `basctl/inc/helpid.hrc` must shadow `editeng/inc/helpid.hrc` and others.
  Achieved via `:basctl_inc` cc_library (`strip_include_prefix = "inc"`) placed
  first in deps, matching the cui module pattern.
- `source/basicide/` .hrc resource constant files (baside2.hrc, brkdlg.hrc, etc.)
  are included with angle brackets → `/Imain/basctl/source/basicide` in COPTS.
- No `component_canUnload` export (not an unloadable component) — DEF has only
  `component_getImplementationEnvironment` and `component_getFactory`.
- Resource files (.src → .res) are not yet built (rsc pipeline is a cross-cutting
  infra item, deferred).
