# unoxml — Bazel Migration

## Deliverables

| Target | Output | Status |
|--------|--------|--------|
| `//main/unoxml:unoxml` | `unoxml.dll` | ✅ |
| `//main/unoxml:unoxml_implib` | `unoxml.lib` (interface library) | ✅ |
| `unordf.dll` | RDF repository UNO component | ⬜ deferred |

## What builds

**unoxml.dll** — XML DOM, XPath, and Events UNO component. Provides:
- `CDocumentBuilder` — builds a DOM tree from a stream or SAX events
- `CSAXDocumentBuilder` — builds a DOM tree from a SAX content handler
- `CXPathAPI` — XPath query service over a DOM tree
- `CTestListener` — event listener utility

Sources: `source/dom/` (22 files), `source/events/` (6 files), `source/xpath/` (3 files), `source/service/services.cxx`.

External dependency: `@libxml2` (static, provides `<libxml/...>` headers).

## What is deferred

**unordf.dll** — RDF repository UNO component (`source/rdf/`). Requires Redland (`@redland`), which depends on raptor2 and rasqal. These libraries have no Windows/MSVC `configure_make` BUILD setup yet. Defer until redland is ported.

## Key decisions

- **No DEF file** — both components export via `SAL_DLLPUBLIC_EXPORT` (`__declspec(dllexport)`), same pattern as `binaryurp`.
- **Three `/I` copts** — source files use angle-bracket local includes (`<cdatasection.hxx>`, `<xpathapi.hxx>`, `<event.hxx>`), so `/Imain/unoxml/source/dom`, `.../events`, `.../xpath` are required.
- **`/Zc:wchar_t-`** — `element.cxx` uses `sal_Unicode` cast.
