# readlicense_oo — Bazel Migration

Generates localized readme and license files from XML source + XSLT stylesheet.

## Source layout

```
docs/
  readme.dtd          — DTD for readme.xrm validation
  readme.xsl          — XSLT stylesheet (HTML and plain-text output modes)
  readme/
    readme.xrm        — English XML source with localizable string nodes
    makefile.mk       — dmake build rules (uses xsltproc)
```

## Build process (dmake)

1. **Merge** — `l10ntools` merges translated strings into a combined `readme.xrm`
   (only when `WITH_LANG` is set; otherwise the English source is used directly).
2. **Transform** — `xsltproc` applies `readme.xsl` to `readme.xrm` twice:
   - `type=html` → `readme_en-US.html`
   - `type=text` → `readme_en-US.txt`
3. **Install** — files land in the `readmes/` subdirectory of the install root.

## Status: Deferred

**Blockers:**
- **xsltproc**: not a standard Windows tool. Strawberry Perl includes
  `XML::LibXSLT` which could substitute, but requires a wrapper genrule.
- **l10ntools merge**: the localization merge pipeline is not yet migrated to
  Bazel; without it only English output is possible.

**Resolution path:** once a Perl-based XSLT genrule is in place (or `xsltproc`
is added as an `http_file` dependency), the English readme can be generated
in a single `genrule` call. Full localization waits on the l10ntools pipeline.
