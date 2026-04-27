# vbahelper — Bazel Migration

## Outputs
- `vbahelper.dll` — core VBA helper infrastructure (base classes for VBA implementations)
- `msforms.dll` — MS Forms UNO component (form controls: button, checkbox, combobox, etc.)

## Targets
- `//main/vbahelper:vbahelper_headers` — public headers from `inc/vbahelper/`
- `//main/vbahelper:vbahelper` — shared library (DLL)
- `//main/vbahelper:vbahelper_implib` — import library for downstream modules
- `//main/vbahelper:msforms` — UNO component DLL (ControlProviderImpl, ScVbaUserForm)

## Dependencies
`oovbaapi`, `offapi`, `basic` (sb.dll), `sfx2`, `svx` (svxcore), `filter` (msfilter),
`vcl`, `svtools`, `tools`, `unotools`, `comphelper`, `svl`, `toolkit`

## Notes
- `VBAHELPER_DLLIMPLEMENTATION` define controls `VBAHELPER_DLLPUBLIC` export/import decoration
- `msforms.dll` registers two UNO services via `comphelper::service_decl`:
  `ControlProviderImpl` and `ScVbaUserForm` (from `util/msforms.component`)
- No DEF files needed — all public symbols use `SAL_DLLPUBLIC_EXPORT` (`__declspec(dllexport)`)
