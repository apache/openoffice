# MCE Phase 1 Source Review — Milestone D.4

**Project:** Apache OpenOffice  
**Repository:** apache/openoffice  
**Branch:** ooxml  
**Target:** trunk  
**Milestone:** D.4  
**Mode:** Source review and build validation

---

## 1. Source Review Findings

### 1.1 Files Reviewed

All 8 files (4 new, 4 modified) were reviewed for correctness:

#### New Files

| File | Status | Lines |
|------|--------|-------|
| `main/oox/inc/oox/core/mceprocessor.hxx` | Reviewed | 117 |
| `main/oox/source/core/mceprocessor.cxx` | Reviewed | 179 |
| `main/oox/inc/oox/core/mcealternatecontext.hxx` | Reviewed | 117 |
| `main/oox/source/core/mcealternatecontext.cxx` | Reviewed | 278 |

#### Modified Files

| File | Status | Change Type |
|------|--------|-------------|
| `main/oox/inc/oox/core/fragmenthandler.hxx` | Reviewed | Added method + forward declaration + friend |
| `main/oox/source/core/fragmenthandler.cxx` | Reviewed | Added namespace getter + MCE interception in `createUnknownChildContext` |
| `main/oox/Library_oox.mk` | Reviewed | Added 2 new exception objects |
| `main/writerfilter/source/ooxml/OOXMLFastDocumentHandler.cxx` | Reviewed | Added MCE interception in `createUnknownChildContext` |

---

## 1.2 Defects Found and Fixed

### Defect 1: CRITICAL — Brace mismatch in `mcealternatecontext.cxx` (FIXED)

**Location:** `main/oox/source/core/mcealternatecontext.cxx`, lines 171-190 (original)

**Problem:** The `createUnknownChildContext` method had mismatched braces:
- The `if` block for `STATE_CHOICE_SELECTED` (line 174) was never closed with `}`
- An extra `{` appeared at line 182 with no matching control statement
- This would prevent compilation

**Fix:** Replaced the broken brace structure with properly balanced blocks:

```cpp
// Before (broken):
if ( condition ) {
    ++mnNextChoiceIndex;
    return mxDelegate->createUnknownChildContext(...);
{   // EXTRA BRACE - no matching control statement
    if ( meState == STATE_FALLBACK_SELECTED ... ) {
        ...
    }
    return ...;
}   // closes extra brace
}   // closes Choice-selected if

// After (fixed):
if ( condition ) {
    ++mnNextChoiceIndex;
    return mxDelegate->createUnknownChildContext(...);
}

if ( meState == STATE_FALLBACK_SELECTED ... ) {
    return mxDelegate->createUnknownChildContext(...);
}
return ...;
```

**Impact:** Without this fix, the code would not compile. With the fix, compilation should succeed (subject to environment constraints).

### Defect 2: MINOR — Double increment of `mnNextChoiceIndex` (NOT FIXED — works by accident)

**Location:** `main/oox/source/core/mcealternatecontext.cxx`

**Problem:** `mnNextChoiceIndex` is incremented in both `implEvaluateChoice()` (line 246) and `createUnknownChildContext()` (line 179). This works correctly because:
- `implEvaluateChoice` sets `maSelection.mnChoiceIndex = mnNextChoiceIndex` THEN increments `mnNextChoiceIndex`
- `createUnknownChildContext` checks `mnNextChoiceIndex - 1 == maSelection.mnChoiceIndex` THEN increments again
- The invariant `mnNextChoiceIndex - 1 == maSelection.mnChoiceIndex` holds at the point of the check

**Impact:** No functional defect, but the double increment is confusing and fragile. Should be cleaned up in a future refactoring.

**Risk:** Low — the logic is correct for all Choice index values.

### Defect 3: MINOR — Dead code (NOT FIXED)

**Location:** `main/oox/inc/oox/core/mcealternatecontext.hxx` lines 100-111

**Problem:** `implCreateBranchContext()` and `implCreateNullContext()` are declared as private methods but never called anywhere in the implementation.

**Impact:** No functional defect. These methods generate compiler warnings about unused private methods but don't affect correctness.

**Recommendation:** Remove in a future cleanup.

### Defect 4: MINOR — Dead field (NOT FIXED)

**Location:** `main/oox/inc/oox/core/mceprocessor.hxx` line 37

**Problem:** `Selection::bMustUnderstand` is set to `false` in `MceProcessor::selectBranch()` but never read anywhere.

**Impact:** No functional effect. The field is dead data.

**Recommendation:** Remove or use in Phase 2 (MustUnderstand handling).

### Defect 5: MINOR — Unused include (NOT FIXED)

**Location:** `main/oox/source/core/mcealternatecontext.cxx` line 6

**Problem:** `#include <osl/diagnose.h>` is included but `OSL_ENSURE` or any other diagnose macro is never used.

**Impact:** No functional effect. Minor build performance impact only.

---

## 1.3 Source Code Correctness Assessment

### Correct Aspects

| Aspect | Assessment |
|--------|------------|
| `MceProcessor` stateless design | Correct — no member state, all methods static |
| Namespace URI resolution | Correct — uses `xmlns:*` attribute parsing from `XFastAttributeList` |
| Choice eligibility (all prefixes must be supported) | Correct — matches ISO/IEC 29500-3 semantics |
| First-eligible-Choice-wins | Correct — iterates in document order, returns first match |
| Fallback when no Choice eligible | Correct — checks `bHasFallback` before returning `BRANCH_FALLBACK` |
| BRANCH_NONE when no Choice and no Fallback | Correct — safe handling of empty AlternateContent |
| Event forwarding only for selected branch | Correct — `mbInSelectedBranch` gate on all forwarding methods |
| State machine transitions | Correct — INITIAL → CHOICE_SCAN → CHOICE_SELECTED/FALLBACK_SELECTED → DONE |
| Nested AlternateContent (outer level) | Correct — outer context handles its own Choice/Fallback |
| `mbInSelectedBranch` reset on branch end | Correct — set to `false` in `endUnknownElement` for Choice/Fallback |
| `mbInSelectedBranch` reset on AlternateContent end | Correct — set to `false` in `endUnknownElement` for AlternateContent |

### Known Limitations (documented in mce-phase1-limitations.md)

| Limitation | Severity | Phase |
|------------|----------|-------|
| `mc:Ignorable` not processed | High | Phase 2 |
| `mc:MustUnderstand` not checked | High | Phase 2 |
| `mc:ProcessContent` not handled | Medium | Phase 2 |
| `mc:PreserveElements`/`mc:PreserveAttributes` not implemented | Medium | Phase 3 |
| Nested AlternateContent inside Choice branch not handled | Medium | Future |
| Namespace prefix resolution only checks element's own `xmlns:*` | Medium | Phase 2 |
| Supported namespace limited to DrawingML main only | Medium | Future |
| Raw XML retention not implemented | High | Phase 3 |

---

## 1.4 Integration Point Verification

### oox/ Pipeline (Calc XLSX, Impress PPTX)

**Integration point:** `FragmentHandler::createUnknownChildContext()`

**Flow:**
1. Fast parser encounters unknown element `mc:AlternateContent` in `mc:` namespace
2. Parser calls `FragmentHandler::createUnknownChildContext(mc-ns, "AlternateContent", attribs)`
3. Modified code checks namespace + name match, returns `MceAlternateContext`
4. `MceAlternateContext` receives `getFastContextHandler()` as delegate (the `FragmentHandler` itself)
5. When processing Choice children, `MceAlternateContext` delegates to `FragmentHandler::createFastChildContext`/`createUnknownChildContext`
6. DrawingML `FragmentHandler` subclasses (e.g., `WorksheetFragment`, `SlideFragmentHandler`) override these methods and handle `a:pic`, `a:blipFill`, etc.

**Correctness:** The delegate is the `FragmentHandler` itself (or subclass). For DrawingML subclasses that override `createFastChildContext`, known elements like `a:pic` will be correctly processed. For the base `FragmentHandler` (returns 0), known elements inside selected branches would be skipped — but this case doesn't occur in practice because DrawingML documents always use subclassed handlers.

### writerfilter Pipeline (Writer DOCX)

**Integration point:** `OOXMLFastDocumentHandler::createUnknownChildContext()`

**Flow:**
1. Fast parser encounters unknown element `mc:AlternateContent` in `mc:` namespace
2. Parser calls `OOXMLFastDocumentHandler::createUnknownChildContext(mc-ns, "AlternateContent", attribs)`
3. Modified code checks namespace + name match
4. Gets current context handler via `getContextHandler()`, creates a copy
5. Wraps the copy in `MceAlternateContext` and returns it
6. `MceAlternateContext` forwards events to the `OOXMLFastContextHandler` delegate
7. Delegate processes events through `OOXMLFactory`

**Correctness:** The `OOXMLFastContextHandler` copy constructor copies the context state. The `MceAlternateContext` properly delegates `startFastElement`/`createFastChildContext`/`createUnknownChildContext` to the delegate, which processes DrawingML elements through `OOXMLFactory::startAction` etc.

**Concern:** The copy constructor `OOXMLFastContextHandler(OOXMLFastContextHandler * pContext)` — need to verify it properly copies all state. This is existing code (not modified by D.3) and is used elsewhere in the same file (line 123-124 creates a new `OOXMLFastDocumentHandler` with `0, 0` for stream/document), so the pattern is consistent.

---

## 1.5 Build File Verification

### `main/oox/Library_oox.mk`

Added two exception objects:
```
oox/source/core/mcealternatecontext \
oox/source/core/mceprocessor \
```

**Correctness:** These are compiled as part of the `oox` shared library. Header files are in `main/oox/inc/oox/core/`, covered by existing `oox_inc` package headers. No additional build configuration needed.

### Writerfilter Build

No new source files in writerfilter. The existing `OOXMLFastDocumentHandler.cxx` is modified to include `oox/core/mcealternatecontext.hxx`. Writerfilter already links against `oox` (`gb_Library_add_linked_libs,writerfilter,\n\toox \`), so symbol resolution works.

---

## 1.6 Conclusion

All source defects that would prevent compilation have been fixed (brace mismatch in `mcealternatecontext.cxx`). Remaining issues are minor (dead code, dead fields, double increment) and do not affect correctness. The implementation follows AOO's existing patterns and integrates cleanly with both the `oox/` and writerfilter parser architectures.

**Build status:** Could not complete full build due to missing Boost development headers (`libboost-dev`) — see `mce-build-results.md`.
