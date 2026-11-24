# MCE Risk Analysis — Milestone D.2

**Project:** Apache OpenOffice  
**Repository:** apache/openoffice  
**Branch:** OOXML  
**Target:** trunk  
**Milestone:** D.2  
**Mode:** Design only — no production code changes

---

## 1. Compatibility Risks

### 1.1 Incorrect Branch Selection

**Risk:** The MCE processor may select the wrong Choice branch or Fallback, causing content to be displayed incorrectly or not at all.

**Causes:**
- `Requires` prefix resolution errors (prefix-to-namespace mapping)
- Namespace support set incomplete or incorrect
- Choice branch order misinterpreted

**Impact:**
- DOCX: Pictures, text boxes, or other drawing content missing or wrong
- XLSX: Charts, drawings, or images missing or wrong
- PPTX: Slides, shapes, or 3D models missing or wrong

**Likelihood:** Medium  
**Severity:** High  
**Mitigation:**
- Test all fixture combinations (F1-F14)
- Maintain comprehensive supported-namespace set
- Log branch selection decisions for debugging

### 1.2 Namespace-Prefix Resolution Errors

**Risk:** The `Requires` attribute uses namespace prefixes, not URIs. Resolving prefixes requires access to the current XML namespace context (all `xmlns:*` declarations in scope).

**Causes:**
- Prefix declared on a parent element, not on `mc:AlternateContent` itself
- Prefix redeclared in nested scope
- Default namespace (`xmlns=""`) interactions

**Impact:** Choice branch selection fails silently; Fallback used when Choice should match, or vice versa.

**Likelihood:** Medium  
**Severity:** High  
**Mitigation:**
- Implement proper namespace context stack (track all `xmlns:*` declarations)
- Resolve prefixes against the in-scope namespace declarations
- Unit test prefix resolution with nested and redeclared namespaces

### 1.3 Unsupported Content Accidentally Discarded

**Risk:** Content that AOO could support is discarded because the MCE processor incorrectly determines the Choice branch doesn't match.

**Causes:**
- Incomplete supported-namespace set
- `Requires` prefix not resolved correctly
- MCE processor not invoked for certain code paths

**Impact:** Document content lost without user notification.

**Likelihood:** Medium  
**Severity:** High  
**Mitigation:**
- Comprehensive fixture testing (F1-F14)
- Log all MCE decisions for audit
- Conservative default: when in doubt, deliver Fallback rather than nothing

### 1.4 Content Interpreted in Wrong Namespace

**Risk:** Content from one namespace is processed as if it were from another, causing incorrect rendering or crashes.

**Causes:**
- Namespace URI confusion in branch selection
- Attribute namespace confusion (e.g., `r:embed` vs `mc:Requires`)

**Impact:** Corrupted document state, potential crashes.

**Likelihood:** Low  
**Severity:** High  
**Mitigation:**
- Strict namespace URI comparison (string equality)
- Unit test namespace resolution with multiple namespaces

### 1.5 Parser Behavior Changes for Legacy Documents

**Risk:** Adding MCE processing changes how legacy documents (without MCE) are parsed, causing regressions.

**Causes:**
- MCE processor inserted in document handler chain affects all documents
- Unknown element handling path modified

**Impact:** Existing documents that previously worked may break.

**Likelihood:** Low  
**Severity:** Medium  
**Mitigation:**
- MCE processor only activates when `mc:AlternateContent` element is encountered
- Documents without `mc:AlternateContent` unaffected
- Regression test existing document corpus

---

## 2. Security Risks

### 2.1 Malformed or Hostile XML

**Risk:** Maliciously crafted MCE markup causes parser crashes, infinite loops, or information disclosure.

**Attack vectors:**
- Deeply nested `mc:AlternateContent` → stack overflow
- Circular `Requires` references → infinite loop
- Oversized `Requires` attribute → memory exhaustion
- Invalid UTF-8 in namespace prefixes → parser errors

**Impact:** Denial of service, potential code execution.

**Likelihood:** Low  
**Severity:** High  
**Mitigation:**
- Limit nesting depth of `mc:AlternateContent`
- Validate `Requires` attribute length and content
- Use existing XML parser's security features (entity limits, depth limits)
- Fuzz testing with malformed MCE markup

### 2.2 Entity Expansion in `Requires`

**Risk:** XML entities in `Requires` attribute cause entity expansion attacks.

**Impact:** Memory exhaustion, denial of service.

**Likelihood:** Low  
**Severity:** Medium  
**Mitigation:**
- Disable external entity expansion (already default in most parsers)
- Limit `Requires` attribute length

---

## 3. Regression Risks

### 3.1 DOCX Regression Risk

**Area:** `main/writerfilter/source/ooxml/`  
**Risk:** Adding MCE handling to `OOXMLFastDocumentHandler` changes unknown element processing for all DOCX files.

**Mitigation:**
- MCE processor only activates for `mc:AlternateContent` elements
- All other unknown elements continue with existing behavior
- Test with existing DOCX test corpus (`main/writerfilter/qa/documents/`)

### 3.2 XLSX Regression Risk

**Area:** `main/oox/source/core/xmlfilterbase.cxx`  
**Risk:** Adding MCE processing to `XmlFilterBase` affects all OOXML filters (XLSX, PPTX, etc.).

**Mitigation:**
- MCE processor only activates for `mc:AlternateContent` elements
- All other elements continue with existing behavior
- Test with existing XLSX test corpus (`test/testgui/data/pvt/*.xlsx`)

### 3.3 PPTX Regression Risk

**Area:** `main/oox/source/ppt/pptimport.cxx`  
**Risk:** Same as XLSX — `pptimport` extends `XmlFilterBase`.

**Mitigation:**
- Same as XLSX
- Test with existing PPTX test corpus (`test/testgui/data/pvt/*.pptx`)

---

## 4. Architectural Risks

### 4.1 Tight Coupling to `XmlFilterBase`

**Risk:** MCE processor integrated into `XmlFilterBase` creates a dependency that writerfilter cannot use without refactoring.

**Impact:** Writerfilter needs separate MCE integration, duplicating logic.

**Mitigation:**
- Design `MceProcessor` as a standalone class with no `XmlFilterBase` dependency
- Writerfilter integration uses `MceProcessor` directly
- Document the integration contract clearly

### 4.2 Performance Impact

**Risk:** MCE processing adds overhead to every XML parse operation.

**Assessment:** MINIMAL — MCE processor only activates when `mc:AlternateContent` element is encountered. Documents without MCE (the vast majority) are unaffected.

**Mitigation:**
- Lazy initialization of MCE processor (only create when needed)
- No per-element overhead for non-MCE content

### 4.3 Interaction with Existing Fallback Behavior

**Risk:** Some code paths already have fallback behavior (e.g., unknown elements skipped). MCE processing may interact unexpectedly with these.

**Assessment:** LOW — MCE processing operates at a higher level (branch selection) than unknown element handling (content skipping). They are complementary, not conflicting.

---

## 5. Safeguards and Recommendations

### 5.1 Safeguards

| Safeguard | Description | Implementation |
|-----------|-------------|----------------|
| Nesting depth limit | Limit `mc:AlternateContent` nesting to prevent stack overflow | Configurable max depth (default: 10) |
| `Requires` length limit | Limit `Requires` attribute length to prevent memory exhaustion | Max 4096 characters |
| Prefix validation | Validate that all `Requires` prefixes are declared | Skip Choice if any prefix unresolved |
| Error logging | Log all MCE decisions for debugging | `OSL_TRACE` or debug logger |
| Conservative default | When in doubt, deliver Fallback rather than nothing | Fail-safe branch selection |

### 5.2 Recommended Regression Tests

| Test | Fixture | Purpose |
|------|---------|---------|
| MCE branch selection | F1, F2, F3 | Verify Choice/Fallback selection |
| Nested AlternateContent | F4 | Verify nesting handling |
| Ignorable namespaces | F9 | Verify ignorable processing |
| ProcessContent | F10 | Verify processing directives |
| MustUnderstand | F11 | Verify error reporting |
| No fallback | F13 | Verify graceful degradation |
| Invalid Requires | F14 | Verify prefix resolution |
| Legacy DOCX regression | Existing test corpus | Verify no regressions |
| Legacy XLSX regression | Existing test corpus | Verify no regressions |
| Legacy PPTX regression | Existing test corpus | Verify no regressions |

---

## 6. Risk Summary

| Risk Category | Risk | Likelihood | Severity | Overall |
|---------------|------|------------|----------|---------|
| Compatibility | Incorrect branch selection | Medium | High | High |
| Compatibility | Namespace-prefix resolution errors | Medium | High | High |
| Compatibility | Unsupported content discarded | Medium | High | High |
| Compatibility | Wrong namespace interpretation | Low | High | Medium |
| Compatibility | Legacy document regression | Low | Medium | Low |
| Security | Malformed/hostile XML | Low | High | Medium |
| Security | Entity expansion | Low | Medium | Low |
| Architecture | Tight coupling to XmlFilterBase | Medium | Medium | Medium |
| Architecture | Performance impact | Negligible | Low | Negligible |
| Architecture | Existing fallback interaction | Low | Medium | Low |

**Top 3 risks to address first:**
1. Incorrect branch selection (High/High)
2. Namespace-prefix resolution errors (High/High)
3. Unsupported content accidentally discarded (High/High)

All three are mitigated by comprehensive fixture testing (F1-F14) and conservative fail-safe defaults.
