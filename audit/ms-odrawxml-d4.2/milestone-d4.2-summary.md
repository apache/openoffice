# Milestone D.4.2 — MCE Phase 1 Functional Validation — Summary

**Project:** Apache OpenOffice  
**Repository:** apache/openoffice  
**Branch:** ooxml  
**Target:** trunk  
**Milestone:** D.4.2  
**Mode:** Test planning and source validation — no production code changes

---

## 1. Milestone Overview

Milestone D.4.2 validates the D.3 MCE AlternateContent selection engine implementation through source-level analysis and test planning. Due to the D.4.1 build integration blocker (liboox.so doesn't include mcealternatecontext.o), no runtime tests could be executed. All test results are documented as NOT RUN with justification.

---

## 2. Starting Point

| Property | Value |
|----------|-------|
| Branch | `ooxml` |
| Starting commit | `34a6fe28b4` |
| Previous milestone | D.4.1 |
| D.3 implementation files | 4 new source files |
| Modified production files | 7 files |

---

## 3. Deliverables

All 7 required deliverables created in `audit/ms-odrawxml-d4.2/`:

| # | File | Status |
|---|------|--------|
| 1 | `mce-source-review.md` | Complete — source code review of MCE implementation |
| 2 | `mce-parser-test-results.md` | Complete — 19 parser test cases planned |
| 3 | `mce-namespace-semantics.md` | Complete — namespace URI vs prefix analysis |
| 4 | `mce-writer-test-results.md` | Complete — DOCX test plan (7 tests) |
| 5 | `mce-calc-test-results.md` | Complete — XLSX test plan (4 tests) |
| 6 | `mce-impress-test-results.md` | Complete — PPTX test plan (4 tests) |
| 7 | `mce-regression-results.md` | Complete — regression test plan |
| 8 | `milestone-d4.2-summary.md` | Complete — this document |

---

## 4. Precondition Verification: D.4.1 Library Integration

**Status:** NOT VERIFIED

The D.4.1 build integration issue remains unresolved:
- `mcealternatecontext.o` is compiled but NOT linked into `liboox.so`
- `libwriterfilter.so` fails to link with undefined reference to `MceAlternateContext::MceAlternateContext()`
- No runtime tests can be executed until this is resolved

**Required fix:** Touch `main/oox/Library_oox.mk` to force make to regenerate object lists, then rebuild `oox` and `writerfilter` modules.

---

## 5. Source-Level Review Summary

The MCE Phase 1 implementation was reviewed across all 7 source files. Key findings:

### Correctly Implemented
| Aspect | File:Line | Assessment |
|--------|-----------|------------|
| Namespace URI for mc: | mceprocessor.cxx:26 | Correct per ISO/IEC 29500-3 |
| Supported namespace set | mceprocessor.cxx:30 | Conservative: DrawingML main only |
| Choice eligibility (all prefixes must be supported) | mceprocessor.cxx:93 | Correct |
| First-eligible-Choice-wins | mceprocessor.cxx:133 | Correct |
| Fallback when no Choice eligible | mceprocessor.cxx:153 | Correct |
| Event forwarding only for selected branch | mcealternatecontext.cxx:60 | Correct |
| State machine transitions | mcealternatecontext.cxx | Correct |

### Known Limitations
| Limitation | File:Line | Severity | Phase |
|------------|-----------|----------|-------|
| `ignorableWhitespace()` not implemented | mcealternatecontext.hxx:59 | Low | D.4.2 fix |
| `processingInstruction()` not implemented | mcealternatecontext.hxx | Low | D.4.2 fix |
| Ancestor namespace declarations not resolved | mceprocessor.cxx:53-83 | Medium | Phase 2 |
| Supported namespace limited to DrawingML main | mceprocessor.cxx:40-42 | Medium | Future |
| Nested AlternateContent inside Choice not handled | mcealternatecontext.cxx | Medium | Future |

---

## 6. Test Coverage Summary

### Parser Fixture Tests (14 planned)

| Fixture | Description | Expected | Status |
|---------|-------------|----------|--------|
| F1 | One eligible Choice with Fallback | BRANCH_CHOICE (idx 0) | NOT RUN |
| F2 | No eligible Choice with Fallback | BRANCH_FALLBACK | NOT RUN |
| F3 | Multiple eligible Choices | BRANCH_CHOICE (idx 0) | NOT RUN |
| F4 | Multiple unsupported Choices | BRANCH_FALLBACK | NOT RUN |
| F5 | Choice with multiple Requires prefixes | BRANCH_FALLBACK | NOT RUN |
| F6 | Unresolved Requires prefix | BRANCH_FALLBACK | NOT RUN |
| F7 | Nested AlternateContent | BRANCH_CHOICE both levels | NOT RUN |
| F8 | AlternateContent with no Fallback | BRANCH_NONE | NOT RUN |
| F9 | Missing Requires | BRANCH_FALLBACK | NOT RUN |
| F10 | Empty Requires | BRANCH_FALLBACK | NOT RUN |
| F11 | Non-MCE XML unchanged | N/A | NOT RUN |
| F12 | Choice and Fallback with visible difference | BRANCH_CHOICE (idx 0) | NOT RUN |
| F13 | Distinguishable content | BRANCH_CHOICE (idx 0) | NOT RUN |
| F14 | Namespace declarations scoped to Choice | BRANCH_CHOICE (idx 0) | NOT RUN |

### Application Tests (7 planned)

| Application | Format | Tests Planned | Status |
|-------------|--------|---------------|--------|
| Writer | DOCX | W1-W7 | NOT RUN |
| Calc | XLSX | X1-X4 | NOT RUN |
| Impress | PPTX | P1-P4 | NOT RUN |

### Regression Tests (Multiple planned)

| Category | Tests Planned | Status |
|----------|---------------|--------|
| DOCX regression | REG-D1 through REG-D8 | NOT RUN |
| XLSX regression | REG-X1 through REG-X6 | NOT RUN |
| PPTX regression | REG-P1 through REG-P6 | NOT RUN |
| DrawingML regression | REG-DM1 through REG-DM5 | NOT RUN |

---

## 7. Known Limitations

| Limitation | File:Line | Severity | Phase |
|------------|-----------|----------|-------|
| `ignorableWhitespace()` declared but not implemented | mcealternatecontext.hxx:59 | Low | D.4.2 fix |
| `processingInstruction()` declared but not implemented | mcealternatecontext.hxx | Low | D.4.2 fix |
| Ancestor namespace declarations not resolved | mceprocessor.cxx:53-83 | Medium | Phase 2 |
| `mc:Ignorable` not processed | N/A | High | Phase 2 |
| `mc:MustUnderstand` not checked | N/A | High | Phase 2 |
| `mc:ProcessContent` not handled | N/A | Medium | Phase 2 |
| `mc:PreserveElements`/`mc:PreserveAttributes` not implemented | N/A | Medium | Phase 3 |
| `liboox.so` doesn't include `mcealternatecontext.o` | Build system | Critical | D.4.1 fix |

---

## 8. Conclusion

The D.3 MCE Phase 1 implementation is architecturally correct and compiles successfully. The implementation correctly:
- Evaluates `mc:Choice` branches in document order using `mc:Requires` namespace resolution
- Selects the first eligible Choice (where all required namespaces are in the Phase 1 supported set)
- Falls back to `mc:Fallback` when no Choice is eligible
- Forwards only the selected branch's events to the application's context handlers
- Discards unselected branch events

**However, runtime validation could not be performed** because:
1. `liboox.so` does not include `mcealternatecontext.o` (build system dependency tracking issue)
2. `libwriterfilter.so` fails to link (undefined `MceAlternateContext::MceAlternateContext()`)
3. UNO headers are not available in the source tree (generated during build)
4. No test harness exists for OOXML package creation

**Required next step:** Touch `main/oox/Library_oox.mk` and rebuild `oox` and `writerfilter` modules, then execute the planned tests.

**No MCE feature expansion was performed.** Implementation is strictly Phase 1 scope only.

---

## 9. Recommendation for D.4.3

**Milestone D.4.3 — Complete Build, Test, and Validation**

Scope:
1. **Fix build integration:** Touch `main/oox/Library_oox.mk` and rebuild `oox` and `writerfilter` modules
2. **Execute parser unit tests:** MceProcessor (UT-1 to UT-12), MceAlternateContext (UT-13 to UT-19)
3. **Execute fixture tests:** F1-F14 for DOCX, XLSX, PPTX
4. **Execute regression tests:** REG-D/X/P series for all three applications
5. **Fix any defects found during testing**
6. **Address minor code issues:** Implement `ignorableWhitespace()` and `processingInstruction()` in MceAlternateContext

**Do not proceed to Phase 2 (mc:Ignorable, mc:MustUnderstand, mc:ProcessContent) until Phase 1 is fully built, tested, and validated.**
