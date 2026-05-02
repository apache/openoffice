# scripting — Bazel migration

## Targets

| Target | Output | Description |
|--------|--------|-------------|
| `//main/scripting:scriptframe` | `scriptframe.dll` | MasterScriptProvider framework (provider + browse-node factory) |
| `//main/scripting:basprov.uno` | `basprov.uno.dll` | Basic script provider |
| `//main/scripting:dlgprov.uno` | `dlgprov.uno.dll` | Dialog provider |
| `//main/scripting:vbaevents.uno` | `vbaevents.uno.dll` | VBA events bridge |
| `//main/scripting:stringresource.uno` | `stringresource.uno.dll` | String resource component |
| `//main/scripting:protocolhandler` | `protocolhandler.dll` | Script protocol handler |

## Key notes

- All targets need `//main/stlport:stlport` — `source/stringresource/stringresource.hxx`,
  `source/provider/ProviderCache.hxx`, `source/dlgprov/dlgevtatt.hxx`, and
  `source/vbaevents/eventhelper.cxx` all use `std::hash_map` from `<hash_map>`.
- `scriptframe` sources are listed explicitly (not via glob) because `ScriptImpl.cxx`
  and `ScriptingContext.cxx` exist in `source/provider/` but were not in the original
  nmake SLOFILES list and are not compiled.
- Include paths: `/Imain/scripting/inc/pch` (for `precompiled_scripting.hxx`),
  `/Imain/scripting/source/inc` (for `util/MiscUtils.hxx`, `bcholder.hxx`).
- `basprov.uno` and `dlgprov.uno` depend on `//main/oovbaapi:oovbaapi_idl_headers`
  for `ooo/vba/` UNO-generated headers.
- Java-based scripting providers (`scripting/java/`, BeanShell, JavaScript) are
  deferred — Java rules not yet in scope.
