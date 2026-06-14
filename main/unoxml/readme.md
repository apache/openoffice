# unoxml — Bazel Migration

## Deliverables

| Target | Output | Status |
|--------|--------|--------|
| `//main/unoxml:unoxml` | `unoxml.dll` | ✅ |
| `//main/unoxml:unoxml_implib` | `unoxml.lib` (interface library) | ✅ |
| `//main/unoxml:unordf` | `unordf.dll` (RDF repository UNO component) | ✅ |
| `//main/unoxml:unordf_implib` | `unordf.lib` (interface library) | ✅ |

## What builds

**unoxml.dll** — XML DOM, XPath, and Events UNO component. Provides:
- `CDocumentBuilder` — builds a DOM tree from a stream or SAX events
- `CSAXDocumentBuilder` — builds a DOM tree from a SAX content handler
- `CXPathAPI` — XPath query service over a DOM tree
- `CTestListener` — event listener utility

Sources: `source/dom/` (22 files), `source/events/` (6 files), `source/xpath/` (3 files), `source/service/services.cxx`.

External dependency: `@libxml2` (static, provides `<libxml/...>` headers).

**unordf.dll** — RDF/metadata UNO component (`source/rdf/`): the `librdf_Repository`
service used by ODF document metadata (e.g. `manifest.rdf` on save). Backed by the
static Redland stack (`@redland` → `@raptor2` + `@rasqal`), plus `@libxml2` and
`@libxslt`. Own `component_getFactory`, so it is a **separate** cc_binary (cannot
merge into `unoxml.dll`).

The Redland stack was migrated to **native cc_library** (not `configure_make`) and
is documented in detail in the top-level **`rdf-readme.md`** — including the
`*_INTERNAL`/`HAVE_CONFIG_H` defines, `local_defines`-vs-`defines` propagation, the
`*_STATIC` dllimport guard, the win32 config-header reproduction, and the
`RAPTOR_WWW_NONE`/`S_ISREG` link fixes. Build-side complete; the save *runtime* path
is the remaining open item (see `rdf-readme.md` → Open items).

## Key decisions

- **No DEF file** — both components export via `SAL_DLLPUBLIC_EXPORT` (`__declspec(dllexport)`), same pattern as `binaryurp`.
- **Three `/I` copts** — source files use angle-bracket local includes (`<cdatasection.hxx>`, `<xpathapi.hxx>`, `<event.hxx>`), so `/Imain/unoxml/source/dom`, `.../events`, `.../xpath` are required.
- **`/Zc:wchar_t-`** — `element.cxx` uses `sal_Unicode` cast.
