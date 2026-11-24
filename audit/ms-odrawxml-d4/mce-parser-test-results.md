# MCE Phase 1 Parser Test Results — Milestone D.4

**Project:** Apache OpenOffice  
**Repository:** apache/openoffice  
**Branch:** ooxml  
**Target:** trunk  
**Milestone:** D.4  
**Mode:** Test planning (tests not executed — build blocked)

---

## 1. Test Execution Status

| Test Category | Planned | Executed | Passed | Failed | Blocked |
|--------------|---------|----------|--------|--------|---------|
| MceProcessor unit tests | 6 | 0 | 0 | 0 | 6 |
| MceAlternateContext unit tests | 6 | 0 | 0 | 0 | 6 |
| Import fixture tests (DOCX) | 12 | 0 | 0 | 0 | 12 |
| Import fixture tests (XLSX) | 3 | 0 | 0 | 0 | 3 |
| Import fixture tests (PPTX) | 3 | 0 | 0 | 0 | 3 |
| Regression tests | Multiple | 0 | 0 | 0 | Multiple |

**All tests blocked** due to missing Boost development headers preventing full build (see `mce-build-results.md`).

---

## 2. MceProcessor Unit Test Plan

These tests verify the `MceProcessor` selection logic independently of `MceAlternateContext`.

### UT-1: Eligible Choice with Single Requires

| Property | Value |
|----------|-------|
| Input `Requires` | `"dml"` |
| Prefix map | `[("dml", "http://schemas.openxmlformats.org/drawingml/2006/main")]` |
| Supported namespaces | `[drawingml/main]` |
| Expected result | `true` (eligible) |
| Rationale | `dml` resolves to DrawingML main URI which is in supported set |

### UT-2: Ineligible Choice with Unsupported Namespace

| Property | Value |
|----------|-------|
| Input `Requires` | `"msodrawxml"` |
| Prefix map | `[("msodrawxml", "http://schemas.microsoft.com/office/drawing/2017/model3d")]` |
| Supported namespaces | `[drawingml/main]` |
| Expected result | `false` (not eligible) |
| Rationale | `msodrawxml` resolves to model3d URI which is NOT in supported set |

### UT-3: Multiple Requires Prefixes — All Supported

| Property | Value |
|----------|-------|
| Input `Requires` | `"dml dmlChart"` |
| Prefix map | `[("dml", "drawingml/main"), ("dmlChart", "drawingml/chart")]` |
| Supported namespaces | `[drawingml/main]` |
| Expected result | `false` (not eligible) |
| Rationale | `dmlChart` resolves to DrawingML chart URI which is NOT in supported set |

### UT-4: Multiple Requires Prefixes — One Unsupported

| Property | Value |
|----------|-------|
| Input `Requires` | `"dml msodrawxml"` |
| Prefix map | `[("dml", "drawingml/main"), ("msodrawxml", "model3d")]` |
| Supported namespaces | `[drawingml/main]` |
| Expected result | `false` (not eligible) |
| Rationale | `msodrawxml` not supported, so entire Choice is ineligible |

### UT-5: Unresolved Prefix

| Property | Value |
|----------|-------|
| Input `Requires` | `"undeclaredPrefix"` |
| Prefix map | `[]` (empty) |
| Supported namespaces | `[drawingml/main]` |
| Expected result | `false` (not eligible) |
| Rationale | `undeclaredPrefix` cannot be resolved to a namespace URI |

### UT-6: Empty Requires

| Property | Value |
|----------|-------|
| Input `Requires` | `""` |
| Prefix map | `[]` |
| Supported namespaces | `[drawingml/main]` |
| Expected result | `false` (not eligible) |
| Rationale | Empty Requires treated as not eligible (conservative Phase 1 policy) |

### UT-7: Missing Requires Attribute

| Property | Value |
|----------|-------|
| Input `Requires` | (attribute absent) |
| Prefix map | `[]` |
| Supported namespaces | `[drawingml/main]` |
| Expected result | `false` (not eligible) |
| Rationale | No Requires attribute → not eligible |

### UT-8: Branch Selection — First Eligible Choice

| Property | Value |
|----------|-------|
| Choice attribs list | `[Choice1(Requires="msodrawxml"), Choice2(Requires="dml")]` |
| Has Fallback | `true` |
| Supported namespaces | `[drawingml/main]` |
| Expected result | `BRANCH_CHOICE`, index 1 |
| Rationale | First Choice ineligible, second Choice eligible |

### UT-9: Branch Selection — Fallback

| Property | Value |
|----------|-------|
| Choice attribs list | `[Choice1(Requires="msodrawxml")]` |
| Has Fallback | `true` |
| Supported namespaces | `[drawingml/main]` |
| Expected result | `BRANCH_FALLBACK`, index -1 |
| Rationale | No eligible Choice, Fallback available |

### UT-10: Branch Selection — No Fallback

| Property | Value |
|----------|-------|
| Choice attribs list | `[Choice1(Requires="msodrawxml")]` |
| Has Fallback | `false` |
| Supported namespaces | `[drawingml/main]` |
| Expected result | `BRANCH_NONE`, index -1 |
| Rationale | No eligible Choice, no Fallback |

---

## 3. MceAlternateContext Unit Test Plan

These tests verify the context handler's branch selection and event forwarding.

### UT-11: Choice Selected — Events Forwarded to Delegate

**Setup:** `MceAlternateContext` with delegate `D`. Sequence:
1. `startUnknownElement(mc-ns, "AlternateContent", {})` → returns MceAlternateContext
2. `createUnknownChildContext(mc-ns, "AlternateContent", {})` → returns MceAlternateContext
3. Parser sets MceAlternateContext as current context
4. `startUnknownElement(mc-ns, "Choice", {Requires="dml"})` → MceAlternateContext evaluates, sets `mbInSelectedBranch=true`
5. `createUnknownChildContext(mc-ns, "Choice", {})` → returns delegate context
6. `startFastElement(a:pic, attribs)` → forwarded to delegate
7. `characters("text")` → forwarded to delegate
8. `endUnknownElement(mc-ns, "Choice")` → MceAlternateContext sets `mbInSelectedBranch=false`
9. `endUnknownElement(mc-ns, "AlternateContent")` → MceAlternateContext sets state to DONE

**Expected:** Delegate receives all selected branch events; unselected branch events discarded

### UT-12: Fallback Selected — Fallback Events Forwarded

**Setup:** Same as UT-11 but Choice requires `msodrawxml` (unsupported), Fallback contains `a:pic`.

**Expected:** Delegate receives Fallback events; Choice events discarded

### UT-13: Nested AlternateContent — Outer Selection

**Setup:** Outer AlternateContent with Choice requiring `dml`, inner AlternateContent inside Choice.

**Expected:** Outer Choice selected; inner AlternateContent processed through delegate (note: nested MCE inside Choice branch is a known limitation — see `mce-phase1-limitations.md`)

### UT-14: Event Balance — Start/End Matching

**Setup:** Complete AlternateContent with Choice and Fallback.

**Expected:** Every `startFastElement`/`startUnknownElement` matched by corresponding `endFastElement`/`endUnknownElement`; no event leaks to delegate from unselected branch

---

## 4. Import Fixture Test Plan (from mce-phase1-test-plan.md)

All 12 fixtures (F1-F14) are defined but not executed. Expected results summarized:

| Fixture | Expected Selection | Expected Document Outcome |
|---------|-------------------|---------------------------|
| F1 | BRANCH_CHOICE (idx 0) | Choice picture processed |
| F2 | BRANCH_FALLBACK | Fallback text processed |
| F3 | BRANCH_CHOICE (idx 0) | First Choice processed |
| F4 | BRANCH_FALLBACK | Fallback processed |
| F5 | BRANCH_FALLBACK | Fallback processed (multi-prefix, one unsupported) |
| F6 | BRANCH_FALLBACK | Fallback processed (unresolved prefix) |
| F7 | BRANCH_CHOICE (idx 0) both levels | Nested choice processed |
| F8 | BRANCH_NONE | No content from AlternateContent |
| F9 | BRANCH_FALLBACK | Fallback processed (missing Requires) |
| F10 | BRANCH_FALLBACK | Fallback processed (empty Requires) |
| F11 | N/A | Normal processing (no AlternateContent) |
| F12 | BRANCH_CHOICE (idx 0) | Styled title processed |

---

## 5. Test Blockers

| Blocker | Impact | Resolution |
|---------|--------|------------|
| Missing `libboost-dev` | Full build blocked | Install `libboost-dev` (requires root) |
| No runtime environment | Tests cannot execute | Build and run tests after build fix |
| No OOXML test documents | Fixtures need to be created | Create minimal OOXML packages for each fixture |

---

## 6. Recommendation

Once the build environment is fixed (install `libboost-dev`), the following should be executed in order:

1. Build `oox` and `writerfilter` modules
2. Compile and run MceProcessor unit tests (UT-1 to UT-10)
3. Compile and run MceAlternateContext unit tests (UT-11 to UT-14)
4. Create and run import fixture tests (F1-F14) for DOCX, XLSX, PPTX
5. Run regression tests on existing OOXML test corpus
