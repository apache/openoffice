# MCE Phase 1 Supported Namespace Semantics Validation — Milestone D.4.2

**Project:** Apache OpenOffice  
**Repository:** apache/openoffice  
**Branch:** ooxml  
**Target:** trunk  
**Milestone:** D.4.2  
**Mode:** Source analysis — tests NOT executed (build integration blocked)

---

## 1. Supported Namespace Set

The D.3 implementation uses a conservative supported namespace set containing exactly one namespace:

| Namespace URI | Prefix (typical) | Supported? | Reasoning |
|--------------|------------------|------------|-----------|
| `http://schemas.openxmlformats.org/drawingml/2006/main` | `a:` | **Yes** | AOO implements DrawingML element parsing in `oox/source/drawingml/` |
| `http://schemas.openxmlformats.org/drawingml/2006/diagram` | `dgm:` | No | Diagram support exists but is less mature; excluded from Phase 1 |
| `http://schemas.openxmlformats.org/drawingml/2006/chart` | `c:` | No | Chart support exists but is less mature; excluded from Phase 1 |
| `http://schemas.openxmlformats.org/wordprocessingml/2006/main` | `w:` | No | Document-level markup, not DrawingML content |
| `http://schemas.openxmlformats.org/spreadsheetml/2006/main` | `x:` | No | Document-level markup, not DrawingML content |
| `http://schemas.openxmlformats.org/presentationml/2006/main` | `p:` | No | Document-level markup, not DrawingML content |
| `http://schemas.microsoft.com/office/drawing/2017/model3d` | `msodrawxml:` | No | Not implemented at all |
| `urn:schemas-microsoft-com:vml` | `v:` | No | Legacy format, not DrawingML |

**Source:** `main/oox/source/core/mceprocessor.cxx:40-42`

---

## 2. Namespace URI vs Prefix Spelling Validation

The implementation resolves `Requires` prefixes to namespace URIs via `xmlns:*` attribute declarations, then checks the URI against the supported set. Prefix spelling is NOT compared directly.

### Test Case: Same URI, Different Prefix

```xml
<!-- Choice 1 -->
<mc:Choice Requires="dml">
  <a:pic>...</a:pic>
</mc:Choice>
<!-- xmlns:dml="http://schemas.openxmlformats.org/drawingml/2006/main" -->

<!-- Choice 2 -->
<mc:Choice Requires="drawingml">
  <a:pic>...</a:pic>
</mc:Choice>
<!-- xmlns:drawingml="http://schemas.openxmlformats.org/drawingml/2006/main" -->
```

**Expected:** Both Choices eligible (same URI, different prefix)  
**Implementation behavior:** Both eligible — `resolvePrefix()` maps each prefix to its URI, then `isNamespaceSupported()` checks the URI  
**Status:** NOT RUN — build blocked

### Test Case: Different URI, Same Prefix Spelling

```xml
<!-- Choice 1 -->
<mc:Choice Requires="dml">
  <a:pic>...</a:pic>
</mc:Choice>
<!-- xmlns:dml="http://schemas.openxmlformats.org/drawingml/2006/main" -->

<!-- Choice 2 -->
<mc:Choice Requires="dml">
  <msodrawxml:model3d>...</msodrawxml:model3d>
</mc:Choice>
<!-- xmlns:dml="http://schemas.microsoft.com/office/drawing/2017/model3d" -->
```

**Expected:** Choice 1 eligible, Choice 2 not eligible (same prefix, different URI)  
**Implementation behavior:** Choice 1 eligible (URI matches drawingml/main), Choice 2 not eligible (URI not in supported set)  
**Status:** NOT RUN — build blocked

---

## 3. Namespace Prefix Resolution Scenarios

### Scenario 1: Prefix Declared on Choice Element Itself

```xml
<mc:AlternateContent>
  <mc:Choice Requires="dml" xmlns:dml="http://schemas.openxmlformats.org/drawingml/2006/main">
    <a:pic>...</a:pic>
  </mc:Choice>
  <mc:Fallback>
    <w:r><w:t>Fallback</w:t></w:r>
  </mc:Fallback>
</mc:AlternateContent>
```

**Expected:** Choice eligible, Choice content processed  
**Implementation behavior:** `extractNamespaceDeclarations()` finds `xmlns:dml` in `getUnknownAttributes()`, maps `dml` → drawingml/main URI, `isChoiceEligible()` returns true  
**Status:** NOT RUN — build blocked

**Risk:** `getUnknownAttributes()` only returns attributes whose names cannot be translated to tokens by the fast parser. If `xmlns:dml` is known to the fast parser (likely for well-known namespaces), it will NOT appear in unknown attributes, and the prefix won't be resolved.

### Scenario 2: Prefix Declared on Ancestor Element

```xml
<w:document xmlns:dml="http://schemas.openxmlformats.org/drawingml/2006/main">
  <w:body>
    <w:p>
      <mc:AlternateContent>
        <mc:Choice Requires="dml">
          <a:pic>...</a:pic>
        </mc:Choice>
        <mc:Fallback>
          <w:r><w:t>Fallback</w:t></w:r>
        </mc:Fallback>
      </mc:AlternateContent>
    </w:p>
  </w:body>
</w:document>
```

**Expected:** Choice eligible (dml namespace is in scope from ancestor declaration)  
**Implementation behavior:** `extractNamespaceDeclarations()` only checks attributes on the `mc:Choice` element itself. If `xmlns:dml` is on `w:document` (ancestor), it won't be found. `resolvePrefix("dml", {})` returns empty URI. Choice not eligible. Fallback used instead.  
**Severity:** Medium — Choice incorrectly treated as ineligible when namespace is declared on ancestor  
**Status:** NOT RUN — build blocked

### Scenario 3: Multiple Prefixes in Requires

```xml
<mc:Choice Requires="dml dmlChart" xmlns:dml="drawingml/main" xmlns:dmlChart="drawingml/chart">
```

**Expected:** Choice NOT eligible (dmlChart namespace not in supported set)  
**Implementation behavior:** `resolvePrefix("dml")` → drawingml/main (supported), `resolvePrefix("dmlChart")` → drawingml/chart (NOT supported), `isChoiceEligible()` returns false  
**Status:** NOT RUN — build blocked

### Scenario 4: Unresolved Prefix

```xml
<mc:Choice Requires="unknownPrefix">
```

**Expected:** Choice NOT eligible  
**Implementation behavior:** `resolvePrefix("unknownPrefix", {})` returns empty URI, `isChoiceEligible()` returns false  
**Status:** NOT RUN — build blocked

---

## 4. Edge Case Analysis

### Edge Case 1: Requires with Mixed Whitespace

```xml
<mc:Choice Requires="  dml   msodrawxml  ">
```

**Expected:** Choice NOT eligible (msodrawxml not supported)  
**Implementation behavior:** Whitespace trimmed between tokens, `dml` resolved (supported), `msodrawxml` resolved (not supported), `isChoiceEligible()` returns false  
**Status:** NOT RUN — build blocked

### Edge Case 2: Requires with Tab/Newline Whitespace

```xml
<mc:Choice Requires="dml&#x9;msodrawxml">
```

**Expected:** Choice NOT eligible  
**Implementation behavior:** Tab treated as whitespace separator, `dml` resolved (supported), `msodrawxml` resolved (not supported), `isChoiceEligible()` returns false  
**Status:** NOT RUN — build blocked

### Edge Case 3: Requires with Only Whitespace

```xml
<mc:Choice Requires="   ">
```

**Expected:** Choice NOT eligible  
**Implementation behavior:** All whitespace skipped, `nPos >= nLen` reached, loop exits, `isChoiceEligible()` returns true (no prefixes to check)  
**Severity:** Low — whitespace-only Requires treated as eligible (no namespaces required). This is debatable but unlikely in practice.  
**Status:** NOT RUN — build blocked

### Edge Case 4: Empty AlternateContent (No Choice, No Fallback)

```xml
<mc:AlternateContent>
</mc:AlternateContent>
```

**Expected:** BRANCH_NONE, no content processed  
**Implementation behavior:** No Choice elements encountered, `mnNextChoiceIndex` stays 0, `maSelection.meResult` stays BRANCH_NONE, `mbHasFallback` stays false  
**Status:** NOT RUN — build blocked

### Edge Case 5: Multiple Fallback Elements

```xml
<mc:AlternateContent>
  <mc:Choice Requires="unsupported">
    <a:pic>...</a:pic>
  </mc:Choice>
  <mc:Fallback>
    <w:r><w:t>First fallback</w:t></w:r>
  </mc:Fallback>
  <mc:Fallback>
    <w:r><w:t>Second fallback</w:t></w:r>
  </mc:Fallback>
</mc:AlternateContent>
```

**Expected:** First Fallback used (ISO/IEC 29500-3 doesn't specify multiple fallbacks)  
**Implementation behavior:** `mbHasFallback` set to true on first Fallback. Second Fallback also sets `mbHasFallback = true` (already true). Selection returns BRANCH_FALLBACK. Both Fallback elements' `startFastElement` are called, but only the first one's content is forwarded (since `mbInSelectedBranch` is already true).  
**Severity:** Low — both Fallback elements' `startFastElement` calls will forward content if `mbInSelectedBranch` is true, potentially duplicating fallback content  
**Status:** NOT RUN — build blocked

---

## 5. Conclusion

The namespace semantics implementation is correct for the common case where `xmlns:*` declarations are on the `mc:Choice` element itself and appear in `getUnknownAttributes()`. Known limitations:

1. **Ancestor namespace declarations not resolved** — If `xmlns:*` is on an ancestor element, the prefix won't be resolved and Choice will be incorrectly ineligible.
2. **`getUnknownAttributes()` may not return `xmlns:*` declarations** — If the fast parser knows the namespace prefix, it won't be in unknown attributes.
3. **Whitespace-only Requires treated as eligible** — Minor edge case, unlikely in practice.

These limitations are documented in the source review (mce-source-review.md) and should be addressed in Phase 2.
