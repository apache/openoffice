# MCE Phase 1 Regression Test Results — Milestone D.4.3

**Project:** Apache OpenOffice  
**Repository:** apache/openoffice  
**Branch:** ooxml  
**Target:** trunk  
**Milestone:** D.4.3  
**Mode:** Test planning — tests NOT executed (build integration blocked)

---

## 1. Regression Test Status

| Test Category | Planned | Executed | Passed | Failed | Blocked | Reason |
|--------------|---------|----------|--------|--------|---------|--------|
| DOCX import regression | Multiple | 0 | 0 | 0 | Multiple | Build blocked |
| XLSX import regression | Multiple | 0 | 0 | 0 | Multiple | Build blocked |
| PPTX import regression | Multiple | 0 | 0 | 0 | Multiple | Build blocked |
| DrawingML shape import | Multiple | 0 | 0 | 0 | Multiple | Build blocked |
| Non-MCE document processing | Multiple | 0 | 0 | 0 | Multiple | Build blocked |

**All regression tests blocked** because `liboox.so` doesn't include `mcealternatecontext.o` (as documented in D.4.1).

---

## 2. Regression Test Plan

### 2.1 Rationale for Regression Testing

The D.3 implementation modifies two critical interception points:

1. `FragmentHandler::createFastChildContext()` — called for ALL known elements in `oox/` filters
2. `OOXMLFastDocumentHandler::createFastChildContext()` — called for ALL known elements in `writerfilter`

The modification adds a check: if the element is `XML_AlternateContent`, return a `MceAlternateContext` instead of delegating to `OOXMLFactory`. For all other elements, behavior is unchanged.

Regression testing must verify that:
- Documents without `mc:AlternateContent` are processed identically to before D.3
- Documents with `mc:AlternateContent` but no eligible Choice/Fallback produce the same result as before D.3 (content skipped)
- DrawingML elements inside `mc:AlternateContent` are unaffected

### 2.2 DOCX Regression Tests

| Test ID | Test Document | Expected Behavior |
|---------|---------------|-------------------|
| REG-D1 | Simple DOCX with paragraph text | Text imported correctly |
| REG-D2 | DOCX with DrawingML image | Image imported correctly |
| REG-D3 | DOCX with table | Table structure preserved |
| REG-D4 | DOCX with fields | Fields resolved correctly |
| REG-D5 | DOCX with headers/footers | Headers/footers present |
| REG-D6 | DOCX with bookmarks | Bookmarks preserved |
| REG-D7 | DOCX with `mc:AlternateContent` (no eligible Choice, no Fallback) | No content from `mc:AlternateContent` (same as before D.3) |
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
| REG-X6 | XLSX with `mc:AlternateContent` (no eligible Choice, no Fallback) | No content from `mc:AlternateContent` (same as before D.3) |

**Existing test documents:** `test/testgui/data/pvt/*.xlsx`

### 2.4 PPTX Regression Tests

| Test ID | Test Document | Expected Behavior |
|---------|---------------|-------------------|
| REG-P1 | Simple PPTX with slide text | Text imported correctly |
| REG-P2 | PPTX with shapes | Shapes imported correctly |
| REG-P3 | PPTX with images | Images imported correctly |
| REG-P4 | PPTX with table | Table structure preserved |
| REG-P5 | PPTX with transitions | Transitions preserved |
| REG-P6 | PPTX with `mc:AlternateContent` (no eligible Choice, no Fallback) | No content from `mc:AlternateContent` (same as before D.3) |

**Existing test documents:** `test/testgui/data/pvt/*.pptx`

### 2.5 DrawingML Regression Tests

| Test ID | Test Document | Expected Behavior |
|---------|---------------|-------------------|
| REG-DM1 | DOCX with `a:pic` image | Image rendered correctly |
| REG-DM2 | XLSX with `a:pic` in drawing | Picture in worksheet rendered correctly |
| REG-DM3 | PPTX with `a:pic` on slide | Picture on slide rendered correctly |
| REG-DM4 | DOCX with `a:graphicFrame` | Graphic frame rendered correctly |
| REG-DM5 | DOCX with `a:txBody` rich text | Rich text rendered correctly |

---

## 3. Specific Regression Risk Areas

### 3.1 `createFastChildContext` Interception (oox/ pipeline)

**Risk:** The modification to `FragmentHandler::createFastChildContext()` adds a check: if the element is `XML_AlternateContent`, return a `MceAlternateContext` instead of 0. For all other elements, behavior is unchanged.

**Mitigation:** The check is a simple `if (Element == XML_AlternateContent)` comparison. For non-MCE elements, the check fails immediately and the method returns 0 (same as before D.3).

**Regression test:** REG-D1 through REG-D7, REG-X1 through REG-X6, REG-P1 through REG-P6

### 3.2 `createFastChildContext` Interception (writerfilter pipeline)

**Risk:** Same as above — adds integer check for every element in WriterFilter.

**Mitigation:** Same simple `if (Element == oox::XML_AlternateContent)` check. For non-MCE elements, falls through to original behavior (delegates to `OOXMLFactory`).

**Regression test:** REG-D1 through REG-D8

### 3.3 Event Forwarding in `MceAlternateContext`

**Risk:** If `mbInSelectedBranch` is incorrectly set or reset, events could leak to the wrong delegate or be lost.

**Mitigation:** State machine carefully manages `mbInSelectedBranch`:
- Set to `true` when eligible Choice found (`implEvaluateChoice`)
- Set to `true` when Fallback selected (`startFastElement` for Fallback)
- Set to `false` when Choice ends (`endFastElement` for Choice)
- Set to `false` when Fallback ends (`endFastElement` for Fallback)
- Set to `false` when AlternateContent ends (`endFastElement` for AlternateContent)

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
- All other elements continue with existing behavior (return 0 or delegate to `OOXMLFactory`)
- The `MceAlternateContext` only forwards events for the selected branch
- Event forwarding is gated by `mbInSelectedBranch` which is carefully managed

**However, this cannot be verified until the build environment is fixed and tests are executed.**
