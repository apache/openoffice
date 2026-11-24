# MCE Implementation Design — Milestone D.2

**Project:** Apache OpenOffice  
**Repository:** apache/openoffice  
**Branch:** OOXML  
**Target:** trunk  
**Milestone:** D.2  
**Mode:** Design only — no production code changes

---

## 1. Design Principles

1. **Minimal and shared** — Single MCE processor in `oox/source/core/` serves all three applications (Writer, Calc, Impress)
2. **Non-invasive** — No changes to existing parser logic; MCE processing wraps around existing unknown-element handling
3. **Namespace-aware** — Uses the already-registered `NMSP_markupCompat` namespace (`xmlfilterbase.cxx:122`)
4. **Safe failure** — Malformed MCE markup falls back to current behavior (skip unknown elements) without crashing
5. **Phased** — Phase 1 (selection/fallback), Phase 2 (ignorable/processing directives), Phase 3 (preservation)

---

## 2. Architecture Overview

```
XML Input Stream
     │
     ▼
Fast Parser (oox/source/core/fastparser.cxx)
     │
     ▼
MCE Processor (NEW: oox/source/core/mceprocessor.cxx)
     │  ├─ Evaluates mc:AlternateContent Choice/Fallback
     │  ├─ Resolves mc:Ignorable namespace list
     │  ├─ Checks mc:MustUnderstand
     │  ├─ Processes mc:Requires prefix declarations
     │  └─ Selects content branch to deliver
     │
     ▼
FragmentHandler (existing)
     │  ├─ startFastElement / startUnknownElement
     │  └─ createFastChildContext / createUnknownChildContext
     │
     ▼
Application-specific context handlers
     │  ├─ oox/source/xls/ (Calc)
     │  ├─ oox/source/ppt/ (Impress)
     │  └─ writerfilter/source/ooxml/ (Writer)
```

---

## 3. Integration Points

### 3.1 Primary Integration: `XmlFilterBase` (oox)

The `XmlFilterBase` class (`main/oox/source/core/xmlfilterbase.cxx`) is the base class for all OOXML filters in `oox/`. It already:
- Registers `NMSP_markupCompat` namespace (line 122)
- Creates `FastParser` instance
- Sets document handler for fragment parsing

**Integration point:** Add MCE processing in `XmlFilterBase::setDocumentHandler()` or as a wrapper around `FastParser::parseStream()`. The MCE processor intercepts `mc:AlternateContent` elements before they reach the application-specific handler.

**Files to modify (design only):**
- `main/oox/source/core/xmlfilterbase.cxx` — Add MCE processor instantiation and integration
- `main/oox/source/core/mceprocessor.cxx` (NEW) — MCE processor implementation
- `main/oox/inc/oox/core/mceprocessor.hxx` (NEW) — MCE processor header

### 3.2 Writerfilter Integration

Writerfilter (`main/writerfilter/source/ooxml/`) uses its own handler chain (`OOXMLFastContextHandler`, `OOXMLFastDocumentHandler`). It does NOT use `oox::core::XmlFilterBase`.

**Integration point:** Add MCE processing in `OOXMLFastDocumentHandler::startUnknownElement()` or as a pre-processing wrapper. When `mc:AlternateContent` is encountered as an unknown element, delegate to MCE processor for branch selection.

**Files to modify (design only):**
- `main/writerfilter/source/ooxml/OOXMLFastDocumentHandler.cxx` — MCE delegation in unknown element handling
- `main/writerfilter/source/ooxml/OOXMLFastContextHandler.cxx` — MCE context handler integration

### 3.3 Calc Integration

Calc uses `oox/source/xls/excelfilter.cxx` which extends `XmlFilterBase`. MCE processing in `XmlFilterBase` automatically applies to Calc import.

### 3.4 Impress Integration

Impress uses `oox/source/ppt/pptimport.cxx` which extends `XmlFilterBase`. MCE processing in `XmlFilterBase` automatically applies to Impress import.

---

## 4. MCE Processor Design

### 4.1 Class Interface (Proposed)

```cpp
class MceProcessor
{
public:
    // Processes an mc:AlternateContent element and returns the selected branch
    enum class Branch { CHOICE, FALLBACK, NONE };

    struct BranchResult {
        Branch eBranch;           // Which branch was selected
        OUString aFragmentPath;   // Path to the selected branch content
        bool bMustUnderstand;     // Whether mc:MustUnderstand was set
        bool bError;              // Whether an error occurred
    };

    // Evaluates an mc:AlternateContent element
    BranchResult evaluateAlternateContent(
        const OUString& rElementNamespace,
        const OUString& rElementName,
        const Reference< XFastAttributeList >& rAttribs,
        const Sequence< Reference< XFastContextHandler > >& rChildContexts
    );

    // Resolves mc:Ignorable namespace list
    bool isNamespaceIgnorable(const OUString& rNamespaceUri);

    // Checks mc:MustUnderstand
    bool isMustUnderstand(const Reference< XFastAttributeList >& rAttribs);

    // Processes mc:ProcessContent
    enum class ProcessContent { CHOICE, FALLBACK, ALL };
    ProcessContent getProcessContent(const Reference< XFastAttributeList >& rAttribs);
};
```

### 4.2 Namespace Resolution for `Requires`

The `Requires` attribute contains whitespace-separated namespace prefixes (e.g., `Requires="dml ppt"`). Resolution steps:

1. Parse `Requires` attribute value into prefix list
2. For each prefix, look up the namespace URI from the current XML namespace context
3. Check if the namespace URI is in the supported namespace set
4. If all prefixes resolve to supported namespaces → Choice matches
5. If any prefix is unresolved or unsupported → Choice does not match

**Supported namespace set** (initial): All namespaces registered in `XmlFilterBaseImpl` constructor (`xmlfilterbase.cxx:101-122`).

### 4.3 Phase 1: Minimal Selection and Fallback

**Goal:** Consume supported OOXML safely — when `mc:AlternateContent` is encountered, evaluate Choice branches and select Fallback if no Choice matches.

**Components:**
- `MceProcessor` class in `oox/source/core/`
- Integration in `XmlFilterBase::setDocumentHandler()` to wrap document handler
- Integration in `OOXMLFastDocumentHandler::startUnknownElement()` for writerfilter

**Behavior:**
- `mc:AlternateContent` → evaluate Choice branches in order
- First Choice with all `Requires` namespaces supported → deliver that branch's content
- No Choice matches → deliver Fallback content
- No Fallback → deliver nothing (empty content)
- Malformed MCE → fall back to current behavior (skip unknown element)

**Integration flow:**
```
FastParser encounters mc:AlternateContent
    → startUnknownElement called (namespace=mc, name=AlternateContent)
    → MceProcessor::evaluateAlternateContent() called
    → Selects branch (Choice or Fallback)
    → Creates context handler for selected branch only
    → Unselected branches' content is skipped
```

### 4.4 Phase 2: Ignorable Content and Processing Directives

**Goal:** Handle ignorable namespaces, processing directives, and unknown extension content gracefully.

**Additional components:**
- `mc:Ignorable` namespace list tracking
- `mc:ProcessContent` handling
- Unknown extension element skipping (already works via `createUnknownChildContext` returning 0)

**Behavior:**
- Elements in ignorable namespaces are skipped without error
- `mc:ProcessContent="fallback"` → process Fallback content even in ignorable region
- `mc:ProcessContent="choice"` → process Choice content
- `mc:ProcessContent="all"` → process all content

### 4.5 Phase 3: Preservation of Unsupported Extension Markup

**Goal:** Preserve unsupported extension markup during round trips, if feasible.

**Approach:** Raw XML retention of unsupported extension content in a sidecar storage, keyed by relationship ID. On re-export, the sidecar content is re-injected into the output package.

**Risks (see mce-risk-analysis.md):**
- Relationship ID changes can break preserved references
- Drawing object modifications can invalidate preserved markup
- Package structure changes can invalidate sidecar references

**Recommendation:** Phase 3 should NOT recommend blind XML copying. Instead, preserve at the object level where possible (convert extension to AOO internal representation), and only fall back to raw XML retention for truly unsupported content with clear relationship tracking.

---

## 5. Preservation Feasibility Analysis

### 5.1 Raw XML Retention

**Approach:** Store raw XML of unsupported extension elements in a sidecar part.

**Feasibility:** LOW-MEDIUM

| Factor | Assessment |
|--------|------------|
| Technical feasibility | Possible — store raw XML strings keyed by element path |
| Relationship integrity | HIGH RISK — relationship IDs in preserved XML may reference parts that changed |
| Drawing object consistency | HIGH RISK — preserved markup may reference objects that were modified or deleted |
| Round-trip fidelity | PARTIAL — extension content preserved but may be inconsistent with modified document |
| Implementation complexity | Medium — requires sidecar storage and re-injection logic |

**Recommendation:** Do NOT recommend blind XML copying. Raw XML retention should only be used as a last resort with clear relationship tracking.

### 5.2 Package-Part Preservation

**Approach:** Preserve entire package parts containing extension content.

**Feasibility:** LOW

| Factor | Assessment |
|--------|------------|
| Technical feasibility | Possible — copy unknown parts to output package |
| Relationship integrity | HIGH RISK — part relationships may reference modified parts |
| Content consistency | HIGH RISK — preserved parts may reference objects that no longer exist |
| Implementation complexity | Low-Medium |

**Recommendation:** Not recommended as primary approach. Too high risk of producing inconsistent packages.

### 5.3 Object-Level Preservation

**Approach:** Convert extension content to AOO's internal drawing model where possible; skip what cannot be converted.

**Feasibility:** HIGH

| Factor | Assessment |
|--------|------------|
| Technical feasibility | Uses existing drawing layer model (`main/drawinglayer/`) |
| Relationship integrity | LOW RISK — AOO manages its own relationships |
| Content consistency | LOW RISK — AOO's internal model is self-consistent |
| Implementation complexity | High — requires implementing each extension type |
| Round-trip fidelity | PARTIAL — only supported extensions round-trip perfectly |

**Recommendation:** PRIMARY APPROACH. Convert what can be converted, skip the rest. This is consistent with how AOO already handles unsupported features.

### 5.4 Existing Round-Trip Mechanisms

| Mechanism | File | Relevance to MCE preservation |
|-----------|------|------------------------------|
| ZipStorage | `oox/source/helper/zipstorage.cxx` | Package read/write — could store sidecar parts |
| Relations | `oox/source/core/relations.cxx` | Relationship tracking — could track preserved part references |
| xstorage | `main/package/source/xstor/xstorage.cxx` | OLE storage over ZIP — not directly relevant |

**Assessment:** Existing infrastructure supports sidecar storage and relationship tracking, but no mechanism exists for preserving unknown extension markup across round trips.

---

## 6. Phased Implementation Plan

### Phase 1 — Minimal Selection and Fallback

| Item | Description | File | Effort |
|------|-------------|------|--------|
| MceProcessor class | Branch selection logic | `oox/source/core/mceprocessor.cxx` (NEW) | Low-Medium |
| MceProcessor header | Class declaration | `oox/inc/oox/core/mceprocessor.hxx` (NEW) | Low |
| XmlFilterBase integration | Wrap document handler with MCE processing | `oox/source/core/xmlfilterbase.cxx` | Low |
| Writerfilter integration | Delegate unknown mc:AlternateContent to MCE processor | `writerfilter/source/ooxml/OOXMLFastDocumentHandler.cxx` | Low |
| Unit tests | Fixture-based tests F1-F3, F9, F13, F14 | New test files | Medium |

**Deliverable:** `mc:AlternateContent` with Choice/Fallback selection works for all three applications.

### Phase 2 — Ignorable Content and Processing Directives

| Item | Description | File | Effort |
|------|-------------|------|--------|
| mc:Ignorable tracking | Track ignorable namespace list | `mceprocessor.cxx` | Low |
| mc:ProcessContent | Handle choice/fallback/all processing | `mceprocessor.cxx` | Low |
| mc:MustUnderstand | Check and report unsupported required extensions | `mceprocessor.cxx` | Low |
| Writerfilter Phase 2 | Integrate ignorable processing in writerfilter chain | `OOXMLFastContextHandler.cxx` | Medium |
| Unit tests | Fixture-based tests F4-F8, F10, F11, F12 | New test files | Medium |

**Deliverable:** `mc:Ignorable`, `mc:ProcessContent`, `mc:MustUnderstand` handled correctly.

### Phase 3 — Preservation (Optional)

| Item | Description | File | Effort |
|------|-------------|------|--------|
| Sidecar storage | Store unsupported extension XML | `oox/source/helper/zipstorage.cxx` | Medium |
| Relationship tracking | Track preserved part references | `oox/source/core/relations.cxx` | Medium |
| Re-injection logic | Re-inject preserved content on export | Filter-specific export code | High |
| Unit tests | Round-trip preservation tests | New test files | High |

**Deliverable:** Unsupported extension markup preserved across round trips (where safe).

---

## 7. Files to Create (New)

| File | Purpose |
|------|---------|
| `main/oox/inc/oox/core/mceprocessor.hxx` | MCE processor class declaration |
| `main/oox/source/core/mceprocessor.cxx` | MCE processor implementation |

---

## 8. Files to Modify (Design Only)

| File | Change |
|------|--------|
| `main/oox/source/core/xmlfilterbase.cxx` | Integrate MCE processor in document handler chain |
| `main/writerfilter/source/ooxml/OOXMLFastDocumentHandler.cxx` | Delegate mc:AlternateContent to MCE processor |

**No production code will be modified in this milestone.** This is a design document only.
