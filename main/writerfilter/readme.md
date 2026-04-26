# writerfilter — Bazel Migration

## Outputs
- `writerfilter.dll` — Document format import/export DLL (.doc, .docx, .rtf)
- `writerfilter_debug.dll` — Debug/test UNO services (DocTok/OOXML analysis)

## Code Generation Pipeline

The library depends on a multi-stage XSL/Perl generation pipeline:

### doctok (resources.xmi → C++ headers + impl)
- `doctok/sprmids.hxx` — SPRM ID constants (sprmids.xsl)
- `doctok/resourceids.hxx` — Resource ID constants (resourceids.xsl)
- `doctok/resources.hxx` — Resource type declarations (resources.xsl)
- `resources.cxx` — Resource implementations (resourcesimpl.xsl)

### OOXML model preprocessing
- `namespace_preprocess.pl namespaces.txt → namespacesmap.xsl` (imports modelpreprocess.xsl)
- `xsltproc namespacesmap.xsl model.xml → model_preprocessed.xml`
- Both XSL files must be in the same directory because of the relative `<xsl:include>`

### OOXML factory generation (from model_preprocessed.xml)
- `ooxml/resourceids.hxx` — OOXML resource ID constants
- `OOXMLFactory_generated.hxx/.cxx` — Base factory class
- `OOXMLFactory_values.hxx/.cxx` — Value factory
- `OOXMLFactory_<ns>.hxx/.cxx` — Per-namespace factory (24 namespaces, see `_OOXML_NS`)
- `ooxml/OOXMLFastTokens.hxx` — Fast token enum (fasttokens.xsl + token.xml)
- `ooxml/OOXMLnamespaceids.hxx` — Namespace ID constants
- `gperffasttoken.hxx` — gperf perfect hash for fast token lookup

### String tables
- `qnametostr.cxx` — QName-to-string map (ooxml + doctok xsltproc output, concatenated)
- `sprmcodetostr.cxx` — SPRM code-to-string map

## Include path layout

Generated headers are in three search roots exposed via `writerfilter_gen_headers`
(includes = [".", "ooxml", "doctok"]):

| Search root | Provides |
|---|---|
| `bazel-bin/main/writerfilter/` | `<ooxml/...>`, `<doctok/...>`, `"OOXMLFactory_generated.hxx"`, `"gperffasttoken.hxx"` |
| `bazel-bin/main/writerfilter/ooxml/` | `"OOXMLnamespaceids.hxx"`, `"OOXMLFastTokens.hxx"`, `"resourceids.hxx"` in generated .cxx |
| `bazel-bin/main/writerfilter/doctok/` | `"resources.hxx"` in doctok source files |

## Dependencies
build.lst: `sal BOOST:boost cppu cppuhelper ucbhelper tools svx LIBXSLT:libxslt oox`

Link deps (Library_writerfilter.mk): oox, i18nisolang1, i18npaper, sot, tl, utl, comphelper, cppuhelper, cppu, sal

Note: libxslt and svx are build-time dependencies (code generation and headers
pulled transitively via oox) — not linked into writerfilter.dll at runtime.
