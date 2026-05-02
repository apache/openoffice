# uui — Bazel migration

Produces `uui.dll` — UNO component providing user interaction handler services:
- `com.sun.star.comp.uui.UUIInteractionHandler`
- `com.sun.star.comp.uui.UUIInteractionRequestStringResolver`
- `com.sun.star.comp.uui.PasswordContainerInteractionHandler`

## Outputs
- `uui.dll` / `uui_implib`

## Notes
- `stlport` dep required: `iahndl.hxx` uses `std::hash_map` via `<hash_map>`
- DEF exports: `component_getImplementationEnvironment`, `component_getFactory` only (no `component_canUnload`)
- `.src` resource files (18 files in `source/`) deferred — no rsc pipeline yet
- No public include directory; headers are co-located with sources in `source/`
- `build.lst` also lists `LIBXSLT:libxslt` but only for the l10n resource pipeline — not needed for the DLL itself
