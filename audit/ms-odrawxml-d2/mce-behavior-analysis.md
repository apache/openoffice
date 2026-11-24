# MCE Behavior Analysis — Milestone D.2

**Project:** Apache OpenOffice  
**Repository:** apache/openoffice  
**Branch:** OOXML  
**Target:** trunk  
**Milestone:** D.2  
**Mode:** Source audit only — no production code changes  
**Specification basis:** ISO/IEC 29500-3:2015 (Markup Compatibility and Extensibility)

---

## 1. MCE Constructs and Expected Behavior

| Construct | Expected Behavior (ISO 29500-3) | AOO Actual Behavior | Source Evidence |
|-----------|-------------------------------|---------------------|-----------------|
| `mc:AlternateContent` | Wrapper containing Choice/Fallback pairs; evaluate Choice branches in order, select first matching, use Fallback if none match | Treated as unknown element; entire wrapper skipped including all Choice and Fallback content | `fragmenthandler.cxx:102-105` (`createUnknownChildContext` returns 0); `OOXMLFastContextHandler.cxx:287-296` (returns null) |
| `mc:Choice` | Contains preferred content with `Requires` attribute listing required namespaces | Treated as unknown element; skipped | Same as above |
| `mc:Fallback` | Contains fallback content used when no Choice matches | Treated as unknown element; skipped | Same as above |
| `mc:Ignorable` | Declares namespaces that consumers may ignore | Not processed; attribute value ignored | No code references `mc:Ignorable` |
| `mc:MustUnderstand` | If set on an element, consumer must understand it or fail | Not processed | No code references `mc:MustUnderstand` |
| `mc:ProcessContent` | Controls whether child content is processed in ignorable regions | Not processed | No code references `mc:ProcessContent` |
| `mc:PreserveElements` | Preserves specific elements within ignorable regions | Not processed | No code references `mc:PreserveElements` |
| `mc:PreserveAttributes` | Preserves specific attributes within ignorable regions | Not processed | No code references `mc:PreserveAttributes` |

---

## 2. Scenario-by-Scenario Behavior Analysis

### 2.1 `mc:AlternateContent` with Multiple Choice Branches

**Input:**
```xml
<mc:AlternateContent xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006">
  <mc:Choice Requires="dml1">
    <!-- Extension drawing -->
  </mc:Choice>
  <mc:Choice Requires="dml2">
    <!-- Alternative drawing -->
  </mc:Choice>
  <mc:Fallback>
    <!-- Standard drawing -->
  </mc:Fallback>
</mc:AlternateContent>
```

**Expected:** Evaluate Choice branches in order; select first where all `Requires` namespaces are supported; if none match, use Fallback.

**AOO Actual:** `mc:AlternateContent` is treated as an unknown element. `FragmentHandler::createUnknownChildContext()` returns 0 (fragmenthandler.cxx:102-105), so no child context is created. The entire wrapper — including all Choice branches and the Fallback — is skipped.

**Result:** All content within `mc:AlternateContent` is lost, including the Fallback that AOO could have rendered.

---

### 2.2 Choice Branches Containing Unknown Namespaces

**Input:**
```xml
<mc:Choice Requires="msodrawxml">
  <msodrawxml:model3d>...</msodrawxml:model3d>
</mc:Choice>
<mc:Fallback>
  <a:pic>...</a:pic>
</mc:Fallback>
```

**Expected:** `msodrawxml` namespace is not supported by AOO, so skip this Choice and use Fallback.

**AOO Actual:** The `mc:Choice` element itself is unknown. No namespace evaluation occurs. The entire `mc:AlternateContent` wrapper is skipped.

**Result:** Both the Choice content AND the Fallback content are lost.

---

### 2.3 Choice Branches Containing Known Namespaces but Unsupported Elements

**Input:**
```xml
<mc:Choice Requires="dml">
  <a:pic><a:blipFill><a:effectLst><a:gamma></a:gamma></a:effectLst></a:blipFill></a:pic>
</mc:Choice>
<mc:Fallback>
  <a:pic><a:blipFill><a:blip r:embed="rId1"/></a:blipFill></a:pic>
</mc:Fallback>
```

**Expected:** `dml` namespace is supported, but `a:gamma` effect may not be. Depending on implementation, either use Choice (if gamma supported) or Fallback.

**AOO Actual:** `mc:Choice` is unknown. No namespace evaluation. Entire wrapper skipped.

**Result:** Both Choice and Fallback content lost.

---

### 2.4 `mc:Choice` with Multiple `Requires` Prefixes

**Input:**
```xml
<mc:Choice Requires="dml ppt">
  <!-- Content requiring both dml and ppt -->
</mc:Choice>
```

**Expected:** All listed namespaces must be supported for this Choice to match.

**AOO Actual:** `Requires` attribute is never read. `mc:Choice` is unknown element, skipped entirely.

**Result:** Choice content lost regardless of namespace support.

---

### 2.5 `mc:Choice` with Invalid/Unresolved `Requires` Prefix

**Input:**
```xml
<mc:Choice Requires="unknownNs">
  <!-- Content -->
</mc:Choice>
<mc:Fallback>
  <!-- Standard content -->
</mc:Fallback>
```

**Expected:** `unknownNs` prefix not declared → Choice does not match → use Fallback.

**AOO Actual:** `Requires` attribute never read. Entire wrapper skipped.

**Result:** Fallback content also lost.

---

### 2.6 Valid `mc:Fallback`

**Input:**
```xml
<mc:AlternateContent>
  <mc:Choice Requires="unsupportedNs">
    <!-- Extension -->
  </mc:Choice>
  <mc:Fallback>
    <a:pic>...</a:pic>
  </mc:Fallback>
</mc:AlternateContent>
```

**Expected:** Use Fallback content since Choice requires unsupported namespace.

**AOO Actual:** Entire `mc:AlternateContent` wrapper skipped. Fallback not used.

**Result:** Standard fallback content lost.

---

### 2.7 No Fallback

**Input:**
```xml
<mc:AlternateContent>
  <mc:Choice Requires="unsupportedNs">
    <!-- Extension -->
  </mc:Choice>
</mc:AlternateContent>
```

**Expected:** No content to display if no Choice matches.

**AOO Actual:** Entire wrapper skipped. Same result (no content), but for wrong reason — even if Choice had matched, content would still be skipped.

**Result:** No content (correct by accident, wrong mechanism).

---

### 2.8 Nested `mc:AlternateContent`

**Input:**
```xml
<mc:AlternateContent>
  <mc:Choice Requires="ns1">
    <mc:AlternateContent>
      <mc:Choice Requires="ns2">
        <!-- Deep extension -->
      </mc:Choice>
      <mc:Fallback>
        <!-- Mid fallback -->
      </mc:Fallback>
    </mc:AlternateContent>
  </mc:Choice>
  <mc:Fallback>
    <!-- Top fallback -->
  </mc:Fallback>
</mc:AlternateContent>
```

**Expected:** Evaluate outer Choice; if ns1 supported, evaluate inner AlternateContent; otherwise use top Fallback.

**AOO Actual:** Outer `mc:AlternateContent` is unknown, skipped entirely. All nested content lost.

**Result:** All nested and fallback content lost.

---

### 2.9 `mc:Ignorable` Containing Extension Namespaces

**Input:**
```xml
<root mc:Ignorable="msodrawxml dml1" xmlns:mc="...">
  <msodrawxml:model3d>...</msodrawxml:model3d>
  <dml1:ext>...</dml1:ext>
  <a:pic>...</a:pic>
</root>
```

**Expected:** Consumer may ignore `msodrawxml` and `dml1` namespaces; process `a:pic` normally.

**AOO Actual:** `mc:Ignorable` attribute is never read. Elements in unknown namespaces are skipped via `createUnknownChildContext` returning 0. Known elements in known namespaces are processed normally.

**Result:** Extension elements in unknown namespaces are lost (correct by accident). Standard elements processed normally.

---

### 2.10 Ignorable Namespaces Containing Elements Unknown to AOO

**Input:**
```xml
<root mc:Ignorable="dml">
  <dml:unknownElement>...</dml:unknownElement>
  <dml:pic>...</dml:pic>
</root>
```

**Expected:** `dml` is ignorable, so both elements may be skipped without error.

**AOO Actual:** `mc:Ignorable` not read. `dml:unknownElement` skipped (unknown token). `dml:pic` processed if token is known.

**Result:** Unknown elements in known namespaces skipped correctly (by accident). Known elements processed correctly.

---

### 2.11 `mc:ProcessContent`

**Input:**
```xml
<mc:AlternateContent mc:ProcessContent="fallback">
  <mc:Choice Requires="unsupported">
    <!-- Extension -->
  </mc:Choice>
  <mc:Fallback>
    <a:pic>...</a:pic>
  </mc:Fallback>
</mc:AlternateContent>
```

**Expected:** `mc:ProcessContent="fallback"` means process Fallback content as normal even though it's in an ignorable context.

**AOO Actual:** `mc:ProcessContent` not read. Entire wrapper skipped.

**Result:** Fallback content lost.

---

### 2.12 `mc:MustUnderstand`

**Input:**
```xml
<mc:AlternateContent mc:MustUnderstand="true">
  <mc:Choice Requires="unsupported">
    <!-- Critical extension -->
  </mc:Choice>
  <mc:Fallback>
    <!-- Standard content -->
  </mc:Fallback>
</mc:AlternateContent>
```

**Expected:** Consumer must understand the extension or fail to load the document.

**AOO Actual:** `mc:MustUnderstand` not read. No error raised. Entire wrapper skipped.

**Result:** Critical extension silently lost. No error or warning to user.

---

### 2.13 Preservation Directives

**Input:**
```xml
<mc:AlternateContent mc:PreserveElements="dml:effectLst" mc:PreserveAttributes="dml:gamma">
  <mc:Choice Requires="unsupported">
    <!-- Extension with effectLst -->
  </mc:Choice>
  <mc:Fallback>
    <!-- Standard -->
  </mc:Fallback>
</mc:AlternateContent>
```

**Expected:** Preserve specified elements/attributes even in ignorable contexts.

**AOO Actual:** `mc:PreserveElements`/`mc:PreserveAttributes` not read. No preservation mechanism exists.

**Result:** No preservation of any kind.

---

### 2.14 Extension Content Interleaved with Ordinary OOXML Content

**Input:**
```xml
<w:document>
  <w:body>
    <w:p>
      <w:r>
        <w:t>Normal text</w:t>
      </w:r>
      <mc:AlternateContent xmlns:mc="...">
        <mc:Choice Requires="msodrawxml">
          <msodrawxml:model3d>...</msodrawxml:model3d>
        </mc:Choice>
        <mc:Fallback>
          <a:pic>...</a:pic>
        </mc:Fallback>
      </mc:AlternateContent>
      <w:r>
        <w:t>More text</w:t>
      </w:r>
    </w:p>
  </w:body>
</w:document>
```

**Expected:** Normal text preserved; AlternateContent evaluated; Fallback used if Choice unsupported.

**AOO Actual:** Normal text processed normally. `mc:AlternateContent` wrapper skipped entirely. Text after the wrapper processed normally.

**Result:** Text before and after preserved. Extension content and Fallback both lost.

---

## 3. Summary of Deficiencies

| Deficiency | Severity | Impact |
|------------|----------|--------|
| No `mc:AlternateContent` evaluation | Critical | All extension content AND fallback content lost |
| No `mc:Ignorable` processing | High | Extension elements skipped silently (correct by accident) |
| No `mc:MustUnderstand` checking | High | Critical extensions silently lost with no error |
| No `mc:ProcessContent` handling | Medium | Fallback content in ignorable regions not processed |
| No `mc:PreserveElements`/`mc:PreserveAttributes` | Medium | No preservation of extension markup |
| No `Requires` prefix resolution | High | Cannot determine which Choice branch to select |
| Extension content not preserved | High | Round-trip fidelity impossible for extension documents |

---

## 4. Confidence Summary

| Finding | Confidence | Basis |
|---------|-----------|-------|
| No `mc:AlternateContent` evaluation logic exists | VERIFIED | Zero MCE element handler code; `createUnknownChildContext` returns 0 |
| No `mc:Ignorable` processing exists | VERIFIED | Zero references to `mc:Ignorable` in any source file |
| No `mc:MustUnderstand` checking exists | VERIFIED | Zero references to `MustUnderstand` in OOXML code |
| No `Requires` prefix resolution exists | VERIFIED | Zero references to `Requires` attribute processing |
| Unknown elements are skipped (content lost) | VERIFIED | `FragmentHandler::createUnknownChildContext` returns 0; `OOXMLFastContextHandler::createUnknownChildContext` returns null |
| `NMSP_markupCompat` namespace IS registered | VERIFIED | `xmlfilterbase.cxx:122` |
| MCE tokens exist but unused | VERIFIED | `tokens.txt:40,83,147` defined; zero `TOKEN()` references in source |
