# linguistic — Bazel Migration

## Output
- `lng.dll` — core linguistic services (spell check, hyphenation, thesaurus, text conversion, proofreading iterator)

## Build
```
bazel build //main/linguistic:lng
```

## Notes
- `/Imain/linguistic/inc/pch` required so source files can `#include "precompiled_linguistic.hxx"` without a path prefix
- Links against `xo.dll` (xmloff) for XML dictionary serialization (`convdicxml.cxx`)
- ICU `icuuc` linked directly for `<unicode/uscript.h>` usage in `gciterator.cxx`
- DEF file exports only the two entry points present in `lngreg.cxx`: `component_getImplementationEnvironment`, `component_getFactory`

## Dependencies
`svl`, `xmloff`, `ucbhelper`, `vos`, `comphelper`, `tools`, `unotools`, `i18npool`, `i18nutil`, `@icu//:icuuc`
