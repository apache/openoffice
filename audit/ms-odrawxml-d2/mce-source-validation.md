# MCE Source Validation — Milestone D.2

**Project:** Apache OpenOffice  
**Repository:** apache/openoffice  
**Branch:** OOXML  
**Target:** trunk  
**Milestone:** D.2  
**Mode:** Source audit only — no production code changes

---

## 1. Independent Validation of D.1 Findings

### 1.1 D.1 Finding: "No general mc:AlternateContent/MCE handling"

**Status: CONFIRMED with nuance.**

Zero references to `mc:AlternateContent`, `mc:Choice`, `mc:Fallback`, `mc:Ignorable`, `mc:MustUnderstand`, `mc:ProcessContent`, `mc:PreserveElements`, or `mc:PreserveAttributes` exist as *implemented logic* in any OOXML source code.

**Searched paths (all zero matches for MCE element handling):**
- `main/oox/` — no MCE element handler code
- `main/writerfilter/` — no MCE element handler code
- `main/sw/` — no MCE element handler code
- `main/sc/` — no MCE element handler code
- `main/sd/` — no MCE element handler code
- `main/svx/` — no MCE element handler code
- `main/drawinglayer/` — no MCE element handler code
- `main/package/` — no MCE element handler code

### 1.2 Nuance: Namespace Registration EXISTS (D.1 Under-reported)

D.1 stated "Zero references to any MS-ODRAWXML namespace URI" — this is correct for MS-ODRAWXML namespaces, but D.1 did not check the **Markup Compatibility namespace** specifically.

**Finding: `NMSP_markupCompat` IS registered at `main/oox/source/core/xmlfilterbase.cxx:122`:**

```cpp
maFastParser.registerNamespace( NMSP_markupCompat );    // i123528
```

**Namespace URI:** `http://schemas.openxmlformats.org/markup-compatibility/2006`  
**Defined at:** `main/oox/source/token/namespaces.txt:78`

**Significance:** The MCE namespace URI is known to the fast parser, so `mc:`-prefixed elements will not cause namespace resolution errors. However, registration alone does NOT provide MCE semantics — there is no code that processes `mc:AlternateContent`, `mc:Choice`, `mc:Fallback`, etc.

### 1.3 Nuance: MCE Tokens Exist but Are Unused

Tokens `AlternateContent` (tokens.txt:40), `Choice` (tokens.txt:83), and `Fallback` (tokens.txt:147) ARE defined in the token system. However:

- **Zero source code references** to `TOKEN(AlternateContent)`, `TOKEN(Choice)`, or `TOKEN(Fallback)` for MCE processing
- These tokens exist in the global token table but are never dispatched to any handler
- No `NMSP_markupCompat`-scoped token handlers exist

### 1.4 Unknown Element Handling — The Actual Behavior

When the fast parser encounters an element in a registered namespace for which no token handler exists, it calls:

| Method | File | Line | Behavior |
|--------|------|------|----------|
| `startUnknownElement` | `main/oox/source/core/fragmenthandler.cxx:85-87` | Empty no-op |
| `createUnknownChildContext` | `main/oox/source/core/fragmenthandler.cxx:102-105` | Returns `0` (no child context) |
| `endUnknownElement` | `main/oox/source/core/fragmenthandler.cxx:93-95` | Empty no-op |

**Writerfilter equivalent:**
| Method | File | Line | Behavior |
|--------|------|------|----------|
| `startUnknownElement` | `main/writerfilter/source/ooxml/OOXMLFastContextHandler.cxx:202-215` | Logs namespace/name in debug mode, otherwise `(void)` cast — no action |
| `createUnknownChildContext` | `main/writerfilter/source/ooxml/OOXMLFastContextHandler.cxx:287-296` | Returns null context |

**Result:** Elements in the `mc:` namespace (including `mc:AlternateContent`, `mc:Choice`, `mc:Fallback`) are treated as unknown elements. Their content is skipped — no child context is created, no attributes are processed, no content is captured.

---

## 2. Validation by Application

### 2.1 Writer (DOCX)

| Mechanism | Status | Evidence |
|-----------|--------|----------|
| `mc:AlternateContent` recognition | NOT IMPLEMENTED | No handler code in `writerfilter/source/ooxml/` |
| `mc:Choice` evaluation | NOT IMPLEMENTED | No handler code |
| `mc:Fallback` selection | NOT IMPLEMENTED | No handler code |
| `mc:Ignorable` processing | NOT IMPLEMENTED | No handler code |
| `mc:MustUnderstand` checking | NOT IMPLEMENTED | No handler code |
| `mc:ProcessContent` handling | NOT IMPLEMENTED | No handler code |
| `mc:PreserveElements`/`mc:PreserveAttributes` | NOT IMPLEMENTED | No handler code |
| Unknown `mc:*` element skipping | IMPLEMENTED (incidental) | `OOXMLFastContextHandler::startUnknownElement` (cxx:202-215) — logs, no action |
| `NMSP_markupCompat` registration | NOT IN WRITERFILTER | Registration is in `oox/source/core/xmlfilterbase.cxx`, not writerfilter |

### 2.2 Calc (XLSX)

| Mechanism | Status | Evidence |
|-----------|--------|----------|
| `mc:AlternateContent` recognition | NOT IMPLEMENTED | No handler code in `oox/source/xls/` or `sc/source/filter/excel/` |
| `mc:Choice` evaluation | NOT IMPLEMENTED | No handler code |
| `mc:Fallback` selection | NOT IMPLEMENTED | No handler code |
| Unknown `mc:*` element skipping | IMPLEMENTED (incidental) | `FragmentHandler::createUnknownChildContext` returns 0 |

### 2.3 Impress (PPTX)

| Mechanism | Status | Evidence |
|-----------|--------|----------|
| `mc:AlternateContent` recognition | NOT IMPLEMENTED | No handler code in `oox/source/ppt/` |
| `mc:Choice` evaluation | NOT IMPLEMENTED | No handler code |
| `mc:Fallback` selection | NOT IMPLEMENTED | No handler code |
| Unknown `mc:*` element skipping | IMPLEMENTED (incidental) | `FragmentHandler::createUnknownChildContext` returns 0 |

---

## 3. Existing Partial Mechanisms

### 3.1 Namespace Registration (Partial)

| Item | File | Line | Status |
|------|------|------|--------|
| `NMSP_markupCompat` registered in `XmlFilterBaseImpl` | `main/oox/source/core/xmlfilterbase.cxx:122` | Registered but no MCE semantics |

### 3.2 Token Definitions (Partial)

| Token | File | Line | Status |
|-------|------|------|--------|
| `AlternateContent` | `main/oox/source/token/tokens.txt:40` | Defined but never used |
| `Choice` | `main/oox/source/token/tokens.txt:83` | Defined but never used |
| `Fallback` | `main/oox/source/token/tokens.txt:147` | Defined but never used |

### 3.3 Unknown Element Infrastructure (Incidental)

| Item | File | Line | Status |
|------|------|------|--------|
| `FragmentHandler::startUnknownElement` | `main/oox/source/core/fragmenthandler.cxx:85-87` | Empty no-op |
| `FragmentHandler::createUnknownChildContext` | `main/oox/source/core/fragmenthandler.cxx:102-105` | Returns 0 |
| `OOXMLFastContextHandler::startUnknownElement` | `main/writerfilter/source/ooxml/OOXMLFastContextHandler.cxx:202-215` | Debug logging only |

These are NOT MCE mechanisms — they are generic unknown-element handling that happens to skip `mc:*` elements without any MCE semantics.

---

## 4. Confirmation Summary

| D.1 Finding | Validation Status | Evidence |
|-------------|-------------------|----------|
| No general `mc:AlternateContent`/MCE handling | CONFIRMED | Zero MCE element handler code across all source paths |
| No recognized MS-ODRAWXML extension namespaces | CONFIRMED | Zero `schemas.microsoft.com/office/drawing` URI references |
| Multiple import locations skip `extLst` | CONFIRMED | 9 locations in `oox/source/drawingml/` |
| Multiple export locations contain `XML_extLst` OOXTODO | CONFIRMED | 7 locations in `sc/source/filter/excel/` |
| Significant potential document-content loss | CONFIRMED | All extension content silently dropped |

**Revised finding:** D.1 under-reported the namespace registration — `NMSP_markupCompat` IS registered at `xmlfilterbase.cxx:122`. However, this registration alone provides no MCE semantics. The conclusion stands: no actual MCE processing exists.
