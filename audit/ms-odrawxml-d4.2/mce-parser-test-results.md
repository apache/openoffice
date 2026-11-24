# MCE Phase 1 Parser Test Results — Milestone D.4.2

**Project:** Apache OpenOffice  
**Repository:** apache/openoffice  
**Branch:** ooxml  
**Target:** trunk  
**Milestone:** D.4.2  
**Mode:** Test planning — tests NOT executed (build integration blocked as documented in D.4/D.4.1)

---

## 1. Test Execution Status

| Test Category | Planned | Executed | Passed | Failed | Blocked | Reason |
|--------------|---------|----------|--------|--------|---------|--------|
| MceProcessor unit tests | 12 | 0 | 0 | 0 | 12 | Build blocked — `liboox.so` doesn't include MCE symbols |
| MceAlternateContext unit tests | 8 | 0 | 0 | 0 | 8 | Build blocked |
| Import fixture tests (DOCX) | 14 | 0 | 0 | 0 | 14 | Build blocked |
| Import fixture tests (XLSX) | 4 | 0 | 0 | 0 | 4 | Build blocked |
| Import fixture tests (PPTX) | 4 | 0 | 0 | 0 | 4 | Build blocked |

**All tests blocked** because `liboox.so` does not include `mcealternatecontext.o` (as documented in D.4.1). The `libwriterfilter.so` also fails to link. No runtime tests can be executed until the build integration issue from D.4.1 is resolved.

---

## 2. MceProcessor Unit Test Plan

These tests verify the `MceProcessor` selection logic independently of `MceAlternateContext`. Tests cannot be executed due to missing UNO headers in the source tree (generated during build).

### UT-1: Eligible Choice with Single Requires

| Property | Value |
|----------|-------|
| Input `Requires` | `"dml"` |
| Prefix map | `[("dml", "http://schemas.openxmlformats.org/drawingml/2006/main")]` |
| Supported namespaces | `[drawingml/main]` |
| Expected result | `true` (eligible) |
| Status | NOT RUN — build blocked |

### UT-2: Ineligible Choice with Unsupported Namespace

| Property | Value |
|----------|-------|
| Input `Requires` | `"msodrawxml"` |
| Prefix map | `[("msodrawxml", "http://schemas.microsoft.com/office/drawing/2017/model3d")]` |
| Supported namespaces | `[drawingml/main]` |
| Expected result | `false` (not eligible) |
| Status | NOT RUN — build blocked |

### UT-3: Multiple Requires Prefixes — All Supported

| Property | Value |
|----------|-------|
| Input `Requires` | `"dml dmlChart"` |
| Prefix map | `[("dml", "drawingml/main"), ("dmlChart", "drawingml/chart")]` |
| Supported namespaces | `[drawingml/main]` |
| Expected result | `false` (not eligible — dmlChart not in supported set) |
| Status | NOT RUN — build blocked |

### UT-4: Multiple Requires Prefixes — One Unsupported

| Property | Value |
|----------|-------|
| Input `Requires` | `"dml msodrawxml"` |
| Prefix map | `[("dml", "drawingml/main"), ("msodrawxml", "model3d")]` |
| Supported namespaces | `[drawingml/main]` |
| Expected result | `false` (not eligible) |
| Status | NOT RUN — build blocked |

### UT-5: Unresolved Prefix

| Property | Value |
|----------|-------|
| Input `Requires` | `"undeclaredPrefix"` |
| Prefix map | `[]` (empty) |
| Supported namespaces | `[drawingml/main]` |
| Expected result | `false` (not eligible) |
| Status | NOT RUN — build blocked |

### UT-6: Empty Requires

| Property | Value |
|----------|-------|
| Input `Requires` | `""` |
| Prefix map | `[]` |
| Supported namespaces | `[drawingml/main]` |
| Expected result | `false` (not eligible) |
| Status | NOT RUN — build blocked |

### UT-7: Missing Requires Attribute

| Property | Value |
|----------|-------|
| Input `Requires` | (attribute absent) |
| Prefix map | `[]` |
| Supported namespaces | `[drawingml/main]` |
| Expected result | `false` (not eligible) |
| Status | NOT RUN — build blocked |

### UT-8: Branch Selection — First Eligible Choice

| Property | Value |
|----------|-------|
| Choice attribs list | `[Choice1(Requires="msodrawxml"), Choice2(Requires="dml")]` |
| Has Fallback | `true` |
| Supported namespaces | `[drawingml/main]` |
| Expected result | `BRANCH_CHOICE`, index 1 |
| Status | NOT RUN — build blocked |

### UT-9: Branch Selection — Fallback

| Property | Value |
|----------|-------|
| Choice attribs list | `[Choice1(Requires="msodrawxml")]` |
| Has Fallback | `true` |
| Supported namespaces | `[drawingml/main]` |
| Expected result | `BRANCH_FALLBACK`, index -1 |
| Status | NOT RUN — build blocked |

### UT-10: Branch Selection — No Fallback

| Property | Value |
|----------|-------|
| Choice attribs list | `[Choice1(Requires="msodrawxml")]` |
| Has Fallback | `false` |
| Supported namespaces | `[drawingml/main]` |
| Expected result | `BRANCH_NONE`, index -1 |
| Status | NOT RUN — build blocked |

### UT-11: Namespace URI vs Prefix Spelling

| Property | Value |
|----------|-------|
| Input `Requires` | `"dml"` |
| Prefix map | `[("dml", "http://schemas.openxmlformats.org/drawingml/2006/main")]` |
| Different prefix, same URI | `[("drawingml", "http://schemas.openxmlformats.org/drawingml/2006/main")]` with `Requires="drawingml"` |
| Expected result | Both `true` (namespace URI match, not prefix string match) |
| Status | NOT RUN — build blocked |

### UT-12: Equivalent Namespace, Different Prefix

| Property | Value |
|----------|-------|
| Choice 1 `Requires` | `"dml"` with `xmlns:dml="drawingml/main"` |
| Choice 2 `Requires` | `"draw"` with `xmlns:draw="drawingml/main"` |
| Supported namespaces | `[drawingml/main]` |
| Expected result | Both eligible (same URI, different prefix) |
| Status | NOT RUN — build blocked |

---

## 3. MceAlternateContext Unit Test Plan

These tests verify the context handler's branch selection and event forwarding. Tests cannot be executed due to missing UNO headers.

### UT-13: Choice Selected — Events Forwarded to Delegate

**Setup:** `MceAlternateContext` with delegate `D`. Sequence:
1. `startFastElement(XML_AlternateContent, {})` → creates MceAlternateContext
2. `startFastElement(XML_Choice, {Requires="dml"})` → MceAlternateContext evaluates, sets `mbInSelectedBranch=true`
3. `createFastChildContext(XML_Choice, {})` → returns delegate context
4. `startFastElement(a:pic, attribs)` → forwarded to delegate
5. `characters("text")` → forwarded to delegate
6. `endFastElement(XML_Choice)` → MceAlternateContext sets `mbInSelectedBranch=false`
7. `endFastElement(XML_AlternateContent)` → MceAlternateContext sets state to DONE

**Expected:** Delegate receives all selected branch events; unselected branch events discarded  
**Status:** NOT RUN — build blocked

### UT-14: Fallback Selected — Fallback Events Forwarded

**Setup:** Same as UT-13 but Choice requires `msodrawxml` (unsupported), Fallback contains `a:pic`.

**Expected:** Delegate receives Fallback events; Choice events discarded  
**Status:** NOT RUN — build blocked

### UT-15: Multiple Choices — First Eligible Wins

**Setup:** Choice 1 requires `msodrawxml` (unsupported), Choice 2 requires `dml` (supported), Fallback present.

**Expected:** Choice 2's content forwarded to delegate  
**Status:** NOT RUN — build blocked

### UT-16: No Eligible Choice, No Fallback

**Setup:** Choice requires `msodrawxml` (unsupported), no Fallback.

**Expected:** `BRANCH_NONE`, no events forwarded  
**Status:** NOT RUN — build blocked

### UT-17: Event Forwarding Suppression — Unselected Branch

**Setup:** Choice requires `dml` (supported), Fallback contains `a:pic`. Both branches present.

**Expected:** Delegate receives Choice's `a:pic` events; Fallback's events NOT forwarded  
**Status:** NOT RUN — build blocked

### UT-18: Character Data in Selected Branch

**Setup:** Choice requires `dml`, contains text content.

**Expected:** Character data forwarded to delegate  
**Status:** NOT RUN — build blocked

### UT-19: Nested AlternateContent Inside Choice

**Setup:** Outer Choice requires `dml` (supported), inner AlternateContent inside Choice branch.

**Expected:** Outer Choice selected; inner AlternateContent processed through delegate (note: nested MCE inside Choice branch is a known limitation — inner AlternateContent will be treated as unknown element and its `createUnknownChildContext` will return null, dropping all nested content)

**Status:** NOT RUN — build blocked (known limitation)

---

## 4. Import Fixture Test Plan (from D.2/fixture-matrix.md)

All 14 fixtures (F1-F14) are defined but not executed. Expected results summarized:

| Fixture | Description | Expected Selection | Status |
|---------|-------------|-------------------|--------|
| F1 | One eligible Choice with Fallback | BRANCH_CHOICE (idx 0) | NOT RUN |
| F2 | No eligible Choice with Fallback | BRANCH_FALLBACK | NOT RUN |
| F3 | Multiple eligible Choices | BRANCH_CHOICE (idx 0) | NOT RUN |
| F4 | Multiple unsupported Choices | BRANCH_FALLBACK | NOT RUN |
| F5 | Choice with multiple Requires prefixes | BRANCH_FALLBACK | NOT RUN |
| F6 | Unresolved Requires prefix | BRANCH_FALLBACK | NOT RUN |
| F7 | Nested AlternateContent | BRANCH_CHOICE (idx 0) both levels | NOT RUN |
| F8 | AlternateContent with no Fallback | BRANCH_NONE | NOT RUN |
| F9 | Missing Requires | BRANCH_FALLBACK | NOT RUN |
| F10 | Empty Requires | BRANCH_FALLBACK | NOT RUN |
| F11 | Non-MCE XML unchanged | N/A | NOT RUN |
| F12 | Choice and Fallback with visible difference | BRANCH_CHOICE (idx 0) | NOT RUN |
| F13 | Distinguishable content | BRANCH_CHOICE (idx 0) | NOT RUN |
| F14 | Namespace declarations scoped to Choice | BRANCH_CHOICE (idx 0) | NOT RUN |

---

## 5. Test Blocker Analysis

| Blocker | Impact | Resolution Required |
|---------|--------|---------------------|
| `liboox.so` doesn't include `mcealternatecontext.o` | All XLSX/PPTX tests blocked | Touch `Library_oox.mk` and rebuild `oox` module (D.4.1) |
| `libwriterfilter.so` link fails | All DOCX tests blocked | Fix `liboox.so` first, then rebuild `writerfilter` |
| UNO headers not in source tree | All unit tests blocked | Full build required to generate headers |
| No test harness for OOXML package creation | All application tests blocked | Create minimal OOXML packages manually |

---

## 6. Recommendation

Once the D.4.1 build integration issue is resolved (touch `Library_oox.mk` and rebuild `oox` and `writerfilter`), the following should be executed in order:

1. Build `oox` and `writerfilter` modules
2. Compile and run MceProcessor unit tests (UT-1 to UT-12)
3. Compile and run MceAlternateContext unit tests (UT-13 to UT-19)
4. Create and run import fixture tests (F1-F14) for DOCX, XLSX, PPTX
5. Run regression tests on existing OOXML test corpus
