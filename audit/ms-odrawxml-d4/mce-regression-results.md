# MCE Phase 1 Regression Test Results — Milestone D.4

**Project:** Apache OpenOffice  
**Repository:** apache/openoffice  
**Branch:** ooxml  
**Target:** trunk  
**Milestone:** D.4  
**Mode:** Regression planning (not executed — build blocked)

---

## 1. Regression Test Status

| Test Category | Planned | Executed | Passed | Failed | Blocked |
|--------------|---------|----------|--------|--------|---------|
| DOCX import regression | Multiple | 0 | 0 | 0 | Multiple |
| XLSX import regression | Multiple | 0 | 0 | 0 | Multiple |
| PPTX import regression | Multiple | 0 | 0 | 0 | Multiple |
| DrawingML shape import | Multiple | 0 | 0 | 0 | Multiple |
| Non-MCE document processing | Multiple | 0 | 0 | 0 | Multiple |

**All regression tests blocked** due to missing Boost development headers preventing full build (see `mce-build-results.md`).

---

## 2. Regression Test Plan

### 2.1 Rationale for Regression Testing

The D.3 implementation modifies two critical interception points:

1. `FragmentHandler::createUnknownChildContext()` — called for ALL unknown elements in oox/ filters
2. `OOXMLFastDocumentHandler::createUnknownChildContext()` — called for ALL unknown elements in writerfilter

The modification adds a check: if the unknown element is `mc:AlternateContent` in the Markup Compatibility namespace, return a `MceAlternateContext` instead of 0 (or a new `OOXMLFastDocumentHandler`). For all other unknown elements, behavior is unchanged.

Regression testing must verify that:
- Documents without `mc:AlternateContent` are processed identically to before D.3
- Documents with `mc:AlternateContent` but no eligible Choice/Fallback produce the same result as before (content skipped)
- DrawingML elements inside non-MCE contexts are unaffected

### 2.2 DOCX Regression Tests

| Test ID | Test Document | Expected Behavior |
|---------|---------------|-------------------|
| REG-D1 | Simple DOCX with paragraph text | Text imported correctly |
| REG-D2 | DOCX with DrawingML image | Image imported correctly |
| REG-D3 | DOCX with table | Table structure preserved |
| REG-D4 | DOCX with fields | Fields resolved correctly |
| REG-D5 | DOCX with headers/footers | Headers/footers present |
| REG-D6 | DOCX with bookmarks | Bookmarks preserved |
| REG-D7 | DOCX with `mc:AlternateContent` (no eligible Choice, no Fallback) | No content from AlternateContent (same as before D.3) |
| REG-D8 | DOCX with `mc:AlternateContent` (eligible Choice) | Choice content processed (new behavior) |

**Existing test documents:** `main/writerfilter/qa/documents/*.docx`, `main/writerfilter/qa/complex/ooxml/testdocuments/*.docx`

### 2.3 XLSX Regression Tests

| Test ID | Test Document | Expected Behavior |
|---------|---------------|-------------------|
| REG-X1 | Simple XLSX with cell values | Cells imported correctly |
| REG-X2 | XLSX with formulas | Formulas preserved |
| REG-X3 | XLSX with chart | Chart imported correctly |
| REG-X4 | XLSX with styles | Styles applied correctly |
| REG-X5 | XLSX with merged cells | Merged cells preserved |
| REG-X6 | XLSX with `mc:AlternateContent` (no eligible Choice, no Fallback) | No content from AlternateContent (same as before D.3) |

**Existing test documents:** `test/testgui/data/pvt/*.xlsx`

### 2.4 PPTX Regression Tests

| Test ID | Test Document | Expected Behavior |
|---------|---------------|-------------------|
| REG-P1 | Simple PPTX with slide text | Text imported correctly |
| REG-P2 | PPTX with shapes | Shapes imported correctly |
| REG-P3 | PPTX with images | Images imported correctly |
| REG-P4 | PPTX with tables | Table structure preserved |
| REG-P5 | PPTX with transitions | Transitions preserved |
| REG-P6 | PPTX with `mc:AlternateContent` (no eligible Choice, no Fallback) | No content from AlternateContent (same as before D.3) |

**Existing test documents:** `test/testgui/data/pvt/*.pptx`

### 2.5 DrawingML Regression Tests

| Test ID | Test Document | Expected Behavior |
|---------|---------------|-------------------|
| REG-DM1 | DOCX with `a:pic` image | Image rendered correctly |
| REG-DM2 | XLSX with `a:pic` in drawing | Picture in worksheet rendered |
| REG-DM3 | PPTX with `a:pic` on slide | Picture on slide rendered |
| REG-DM4 | DOCX with `a:graphicFrame` | Graphic frame rendered |
| REG-DM5 | DOCX with `a:txBody` rich text | Rich text rendered |

---

## 3. Specific Regression Risk Areas

### 3.1 `createUnknownChildContext` Interception

**Risk:** The modification to `FragmentHandler::createUnknownChildContext()` adds a namespace/name check for every unknown element. For non-MCE elements, the check fails immediately and the method returns 0 (same as before).

**Mitigation:** The check is a simple string comparison:
```cpp
if ( rNamespace.equals( getMarkupCompatNamespace() ) &&
     rName.equalsAsciiL( "AlternateContent", 14 ) )
```
This adds minimal overhead (two string comparisons) for non-MCE elements.

**Regression test:** REG-D1 through REG-D7, REG-X1 through REG-X6, REG-P1 through REG-P6

### 3.2 `OOXMLFastDocumentHandler::createUnknownChildContext` Interception

**Risk:** Same as above — adds namespace/name check for every unknown element in writerfilter.

**Mitigation:** Same simple string comparison. For non-MCE elements, falls through to original behavior (creates new `OOXMLFastDocumentHandler`).

**Regression test:** REG-D1 through REG-D8

### 3.3 Event Forwarding in `MceAlternateContext`

**Risk:** If `mbInSelectedBranch` is incorrectly set or reset, events could leak to the wrong delegate or be lost.

**Mitigation:** State machine carefully manages `mbInSelectedBranch`:
- Set to `true` when eligible Choice found (`implEvaluateChoice`)
- Set to `true` when Fallback selected (`startUnknownElement` for Fallback)
- Set to `false` when Choice ends (`endUnknownElement` for Choice)
- Set to `false` when Fallback ends (`endUnknownElement` for Fallback)
- Set to `false` when AlternateContent ends (`endUnknownElement` for AlternateContent)

**Regression test:** All MCE fixture tests (F1-F14) + regression tests

### 3.4 Context Handler Delegation

**Risk:** `MceAlternateContext` delegates `createFastChildContext`/`createUnknownChildContext` to the parent context handler. If the parent doesn't properly handle DrawingML elements, content could be lost.

**Mitigation:** DrawingML `FragmentHandler` subclasses (`WorksheetFragment`, `SlideFragmentHandler`) override `createFastChildContext` for known DrawingML tokens. The `MceAlternateContext` delegates to these subclasses, so DrawingML elements are processed correctly.

**Regression test:** REG-DM1 through REG-DM5

---

## 4. Regression Test Infrastructure

| Framework | Location | How to Run |
|-----------|----------|------------|
| C++ unit tests | `main/*/qa/cppunittests/` | Build with dmake, run test binaries |
| Java unit tests | `main/package/qa/ofopxmlstorages/` | Compile with javac, run with java |
| Import test documents | `main/writerfilter/qa/documents/`, `test/testgui/data/pvt/` | Load in AOO, verify content |

### 4.1 Test Documents Already Available

| Location | Format | Count | Use |
|----------|--------|-------|-----|
| `main/writerfilter/qa/documents/` | DOCX | Multiple | DOCX import regression |
| `main/writerfilter/qa/complex/ooxml/testdocuments/` | DOCX | Multiple | Complex DOCX regression |
| `test/testgui/data/pvt/*.xlsx` | XLSX | Multiple | XLSX import regression |
| `test/testgui/data/pvt/*.pptx` | PPTX | Multiple | PPTX import regression |
| `test/odf-validate/fixtures/*.docx` | DOCX | 3 | ODF validation with DOCX fixtures |

---

## 5. Regression Test Execution Procedure

Once the build environment is fixed:

1. **Build all modules:**
   ```bash
   cd /home/pedro/projects/aoo/main/instsetoo_native
   source LinuxX86-64Env.Set.sh
   perl ../solenv/bin/build.pl --all
   ```

2. **Run C++ unit tests:**
   ```bash
   # Test binaries are in solver/510/unxlngx6.pro/bin/
   ./solver/510/unxlngx6.pro/bin/<test_binary>
   ```

3. **Run import regression tests:**
   - Open each test document in AOO headless mode
   - Verify document content matches expected
   - Compare with pre-D.3 behavior

4. **Compare results:**
   - Document any differences between pre-D.3 and post-D.3 behavior
   - Investigate and fix any regressions

---

## 6. Expected Regression Results

| Test Category | Expected Result | Confidence |
|---------------|----------------|------------|
| Non-MCE documents | No change (identical to pre-D.3) | High — modification only affects `mc:AlternateContent` |
| Documents with `mc:AlternateContent` (no eligible Choice/Fallback) | No change (content skipped) | High — same behavior as before D.3 |
| Documents with `mc:AlternateContent` (eligible Choice) | New behavior (Choice content processed) | Expected — this is the feature |
| DrawingML elements outside MCE | No change | High — `MceAlternateContext` only active for `mc:AlternateContent` |

---

## 7. Conclusion

No regressions are expected from the D.3 implementation because:
- The modification only affects `mc:AlternateContent` elements in the Markup Compatibility namespace
- All other unknown elements continue with existing behavior (return 0 or create new handler)
- The `MceAlternateContext` only forwards events for the selected branch
- Event forwarding is gated by `mbInSelectedBranch` which is carefully managed

**However, this cannot be verified until the build environment is fixed and tests are executed.**
