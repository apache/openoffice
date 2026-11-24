# MCE Phase 1 Implementation — Milestone D.3

**Project:** Apache OpenOffice  
**Repository:** apache/openoffice  
**Branch:** OOXML  
**Target:** trunk  
**Milestone:** D.3  
**Mode:** Implementation and documentation only

---

## 1. Implementation Summary

Milestone D.3 implements a minimal, reusable Markup Compatibility and Extensibility (MCE) selection engine for `mc:AlternateContent`, `mc:Choice`, and `mc:Fallback` processing. This is Phase 1 of the MCE support plan defined in Milestone D.2.

The implementation addresses the compatibility failure confirmed in Milestones D.1 and D.2: unsupported `mc:AlternateContent` structures were previously skipped entirely, including fallback content that AOO could render. With this implementation, AOO evaluates `mc:Choice` branches in document order, selects the first eligible Choice (based on `mc:Requires` namespace resolution), and falls back to `mc:Fallback` when no Choice is eligible.

---

## 2. Source Files

### 2.1 New Files

| File | Path | Purpose |
|------|------|---------|
| Header | `main/oox/inc/oox/core/mceprocessor.hxx` | MceProcessor class declaration — shared selection logic |
| Implementation | `main/oox/source/core/mceprocessor.cxx` | MceProcessor implementation — namespace resolution, eligibility checking, branch selection |
| Header | `main/oox/inc/oox/core/mcealternatecontext.hxx` | MceAlternateContext class declaration — context handler for mc:AlternateContent |
| Implementation | `main/oox/source/core/mcealternatecontext.cxx` | MceAlternateContext implementation — Choice/Fallback evaluation, event forwarding |

### 2.2 Modified Files

| File | Path | Change |
|------|------|--------|
| Header | `main/oox/inc/oox/core/fragmenthandler.hxx` | Added `getMarkupCompatNamespace()` static method, forward declaration of `MceAlternateContext`, friend declaration |
| Implementation | `main/oox/source/core/fragmenthandler.cxx` | Added `getMarkupCompatNamespace()` implementation; modified `createUnknownChildContext()` to intercept `mc:AlternateContent` and return `MceAlternateContext` |
| Build | `main/oox/Library_oox.mk` | Added `oox/source/core/mcealternatecontext` and `oox/source/core/mceprocessor` to exception objects |
| Implementation | `main/writerfilter/source/ooxml/OOXMLFastDocumentHandler.cxx` | Added `#include <oox/core/mcealternatecontext.hxx>`; modified `createUnknownChildContext()` to intercept `mc:AlternateContent` and return `MceAlternateContext` |

---

## 3. Architecture

### 3.1 Component Diagram

```
XML Input Stream
     │
     ▼
Fast Parser (oox/core/fastparser.cxx)
     │
     ▼
Document Handler (FragmentHandler / OOXMLFastDocumentHandler)
     │
     ├─ createUnknownChildContext(mc-ns, "AlternateContent", ...)
     │   └─► MceAlternateContext (NEW)
     │       ├─ Evaluates mc:Choice Requires via MceProcessor (NEW)
     │       ├─ Selects first eligible Choice or mc:Fallback
     │       ├─ Forwards selected branch events to delegate context
     │       └─ Discards unselected branch events
     │
     ▼
Application Context Handlers
     ├─ oox/source/xls/ (Calc)
     ├─ oox/source/ppt/ (Impress)
     └─ writerfilter/source/ooxml/ (Writer)
```

### 3.2 Integration Points

#### oox/ Pipeline (Calc XLSX, Impress PPTX)

`FragmentHandler::createUnknownChildContext()` (`main/oox/source/core/fragmenthandler.cxx:112`) is modified to check if the unknown element is `mc:AlternateContent` in the Markup Compatibility namespace. If so, it returns a new `MceAlternateContext` instead of 0.

The `MceAlternateContext` receives the parent `FragmentHandler`'s `XFastContextHandler` reference as its delegate. When processing the selected branch, it delegates `createFastChildContext`/`createUnknownChildContext` calls to this delegate, allowing application-specific `FragmentHandler` subclasses (e.g., WorksheetFragment, SlideFragmentHandler) to create contexts for DrawingML elements like `a:pic`, `a:blipFill`, etc.

#### writerfilter Pipeline (Writer DOCX)

`OOXMLFastDocumentHandler::createUnknownChildContext()` (`main/writerfilter/source/ooxml/OOXMLFastDocumentHandler.cxx:117`) is modified similarly. It creates an `OOXMLFastContextHandler` copy (via `getContextHandler()` and copy constructor) and passes it as the delegate to `MceAlternateContext`.

The `MceAlternateContext` forwards `startFastElement`/`createFastChildContext`/`createUnknownChildContext` calls to this delegate, which processes them through `OOXMLFactory`.

---

## 4. MceProcessor — Selection Logic

### 4.1 Class Overview

`MceProcessor` is a stateless logic class with no UNO dependencies beyond `XFastAttributeList`. It provides:

| Method | Purpose |
|--------|---------|
| `getMarkupCompatNamespace()` | Returns the `mc:` namespace URI |
| `getSupportedNamespaceSet()` | Returns the Phase 1 supported namespace URIs |
| `isNamespaceSupported(uri)` | Checks if a namespace URI is supported |
| `isChoiceEligible(attribs, prefixMap)` | Evaluates `mc:Requires` for a Choice element |
| `extractNamespaceDeclarations(attribs)` | Extracts `xmlns:prefix="uri"` from attributes |
| `selectBranch(choiceAttribsList, hasFallback, prefixMap)` | Selects the first eligible Choice or Fallback |

### 4.2 Choice Eligibility Algorithm

For each `mc:Choice` element:

1. Read the `Requires` attribute value
2. If `Requires` is missing or empty → Choice is NOT eligible (conservative)
3. Split `Requires` by whitespace into prefix tokens
4. For each prefix token:
   a. Resolve prefix to namespace URI using the prefix map (built from `xmlns:*` attribute declarations on the Choice element)
   b. If prefix cannot be resolved → Choice is NOT eligible
   c. If namespace URI is not in the supported set → Choice is NOT eligible
5. If all prefixes resolve to supported namespaces → Choice IS eligible

### 4.3 Branch Selection Algorithm

1. Evaluate Choice branches in document order using `isChoiceEligible()`
2. Return the first eligible Choice as `BRANCH_CHOICE`
3. If no Choice is eligible and `mc:Fallback` exists → return `BRANCH_FALLBACK`
4. If neither → return `BRANCH_NONE`

---

## 5. MceAlternateContext — Context Handler

### 5.1 State Machine

```
STATE_INITIAL ──(Choice seen)──► STATE_CHOICE_SCAN ──(eligible Choice)──► STATE_CHOICE_SELECTED
                                                     │
                                                     └─(no eligible Choice + Fallback)──► STATE_FALLBACK_SELECTED
                                                                                           │
                                                                                           └─(end of AlternateContent)──► STATE_DONE

STATE_CHOICE_SELECTED ──(end of AlternateContent)──► STATE_DONE
STATE_FALLBACK_SELECTED ──(end of AlternateContent)──► STATE_DONE
```

### 5.2 Event Handling

| Event | Behavior |
|-------|----------|
| `startUnknownElement(mc-ns, "Choice", attribs)` | Evaluate Requires via `MceProcessor::isChoiceEligible()`. If first eligible Choice, transition to `STATE_CHOICE_SELECTED` and set `mbInSelectedBranch = true`. |
| `startUnknownElement(mc-ns, "Fallback", attribs)` | If no eligible Choice found, transition to `STATE_FALLBACK_SELECTED` and set `mbInSelectedBranch = true`. |
| `startFastElement` / `startUnknownElement` (non-MCE) | If `mbInSelectedBranch`, forward to delegate context handler. Otherwise, discard. |
| `createFastChildContext` / `createUnknownChildContext` | If `mbInSelectedBranch`, delegate to delegate context handler. Otherwise, return null. |
| `endUnknownElement(mc-ns, "AlternateContent")` | Transition to `STATE_DONE`, set `mbInSelectedBranch = false`. |
| `characters` / `ignorableWhitespace` / `processingInstruction` | If `mbInSelectedBranch`, forward to delegate. Otherwise, discard. |

---

## 6. Build Integration

### 6.1 oox Library (`main/oox/Library_oox.mk`)

Added two new exception objects:
```
oox/source/core/mcealternatecontext \
oox/source/core/mceprocessor \
```

These are compiled as part of the `oox` shared library. The header files are in `main/oox/inc/oox/core/`, which is covered by the existing `oox_inc` package headers.

### 6.2 Writerfilter Library

No new source files in writerfilter. The existing `OOXMLFastDocumentHandler.cxx` is modified to include `oox/core/mcealternatecontext.hxx` and use `MceAlternateContext`. Writerfilter already links against `oox` (`gb_Library_add_linked_libs,writerfilter,\n\toox \`), so symbol resolution works.

---

## 7. Phase 1 Supported Namespace Set

The implementation uses a conservative supported namespace set (defined in `MceProcessor::getSupportedNamespaceSet()`):

| Namespace URI | Reasoning |
|--------------|-----------|
| `http://schemas.openxmlformats.org/drawingml/2006/main` | AOO implements DrawingML element parsing in `oox/source/drawingml/` (shape.cxx, fillproperties.cxx, lineproperties.cxx, textparagraph.cxx, textrun.cxx, theme.cxx, color.cxx, etc.) |

Namespaces explicitly NOT in the Phase 1 set:
- SpreadsheetML, WordprocessingML, PresentationML main namespaces (document-level markup)
- All MS-ODRAWXML extension namespaces (not implemented)
- All VML namespaces (legacy format)
- DrawingML diagram, chart sub-namespaces (less mature; excluded from Phase 1)

The supported set can be expanded by modifying the `maSupportedNamespaces` vector initialization in `main/oox/source/core/mceprocessor.cxx`.

---

## 8. Verification Checklist

| Item | Status | Evidence |
|------|--------|----------|
| MceProcessor class created | Implemented | `mceprocessor.hxx`, `mceprocessor.cxx` |
| MceAlternateContext class created | Implemented | `mcealternatecontext.hxx`, `mcealternatecontext.cxx` |
| FragmentHandler integration | Modified | `fragmenthandler.cxx:112` — createUnknownChildContext checks for mc:AlternateContent |
| Writerfilter integration | Modified | `OOXMLFastDocumentHandler.cxx:117` — createUnknownChildContext checks for mc:AlternateContent |
| Build files updated | Modified | `Library_oox.mk` — two new exception objects added |
| No build performed | Confirmed | No compilation or linking attempted |
| No runtime testing performed | Confirmed | No test execution |

---

## 9. Relationship to Milestones D.1 and D.2

| Milestone | Contribution to D.3 |
|-----------|---------------------|
| D.1 (Inventory) | Identified all MS-ODRAWXML extension namespaces (43 namespaces, 42 schema sections) |
| D.2 (Validation & Design) | Confirmed zero MCE handling exists; proposed phased architecture; defined fixture matrix |
| D.3 (Implementation) | Implements Phase 1 of D.2's architecture: mc:AlternateContent/Choice/Fallback selection engine |

D.3 implements only Phase 1 of D.2's three-phase plan. Phase 2 (mc:Ignorable, mc:MustUnderstand, mc:ProcessContent) and Phase 3 (preservation) are deferred to future milestones.
