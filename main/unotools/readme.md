# unotools — Bazel build notes

## Output
- `utl.dll` — UNO utility library (accessibility helpers, config wrappers, i18n wrappers, UCB helpers, streaming, property helpers)

## Key decisions

### Private source headers
Two directories contain private headers that are `#include`d with angle brackets by sibling `.cxx` files:
- `source/config/itemholder1.hxx` — referenced by all config option sources
- `source/ucbhelper/XTempFile.hxx` — referenced by `xtempfile.cxx`

Both paths added to `copts` via `/I` flags.

### Deps
Links against: sal, cppu, cppuhelper, salhelper, comphelper, ucbhelper, tools (tl), i18npool (i18nisolang1), vos3.
Uses stlport + boost.legacy (boost::unordered_map patterns in config sources).

### /Zc:wchar_t-
Required — sources use `sal_Unicode` / `Sequence<sal_Unicode>`.

### No DEF file
Exports use `UNOTOOLS_DLLPUBLIC` (SAL_DLLPUBLIC_EXPORT macro) — no explicit `.def` needed.
