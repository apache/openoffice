# MCE Phase 1 Calc (XLSX) Test Results — Milestone D.4.3

**Project:** Apache OpenOffice  
**Repository:** apache/openoffice  
**Branch:** ooxml  
**Target:** trunk  
**Milestone:** D.4.3  
**Mode:** Test planning — tests NOT executed (build integration blocked)

---

## 1. Test Execution Status

| Test ID | Description | Status | Reason |
|---------|-------------|--------|--------|
| X1 | XLSX with AlternateContent in worksheet drawing, eligible Choice | NOT RUN | Build blocked |
| X2 | XLSX with AlternateContent, ineligible Choice, fallback present | NOT RUN | Build blocked |
| X3 | XLSX with multiple eligible Choices | NOT RUN | Build blocked |
| X4 | XLSX regression (no MCE) | NOT RUN | Build blocked |

**All Calc tests blocked** because `liboox.so` doesn't include `mcealternatecontext.o` (as documented in D.4.1).

---

## 2. Calc Test Fixtures

### X1: XLSX with AlternateContent in Worksheet Drawing

**Package:** `xl/worksheets/sheet1.xml`

```xml
<worksheet xmlns="http://schemas.openxmlformats.org/spreadsheetml/2006/main"
           xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
           xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main">
  <sheetData>
    <row r="1">
      <c r="A1"><v>42</v></c>
    </row>
  </sheetData>
  <drawing>
    <mc:AlternateContent>
      <mc:Choice Requires="dml">
        <a:graphicFrame>
          <a:graphic>
            <a:graphicData uri="http://schemas.openxmlformats.org/drawingml/2006/picture">
              <pic:pic xmlns:pic="http://schemas.openxmlformats.org/drawingml/2006/picture">
                <pic:blipFill>
                  <a:blip r:embed="rId1"/>
                </a:blipFill>
              </pic:pic>
            </a:graphicData>
          </a:graphicFrame>
        </mc:Choice>
        <mc:Fallback>
          <a:pic><a:blipFill><a:blip r:embed="rId2"/></a:blipFill></a:pic>
        </mc:Fallback>
      </mc:AlternateContent>
    </drawing>
  </worksheet>
</worksheet>
```

**Expected:** Picture from Choice imported into worksheet. Cell A1 with value 42 present. Fallback picture NOT present.

### X2: XLSX with AlternateContent, Ineligible Choice, Fallback Present

**Package:** `xl/worksheets/sheet1.xml`

```xml
<worksheet xmlns="http://schemas.openxmlformats.org/spreadsheetml/2006/main"
           xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
           xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main"
           xmlns:msodrawxml="http://schemas.microsoft.com/office/drawing/2017/model3d">
  <sheetData>
    <row r="1">
      <c r="A1"><v>42</v></c>
    </row>
  </sheetData>
  <drawing>
    <mc:AlternateContent>
      <mc:Choice Requires="msodrawxml">
        <msodrawxml:model3d>...</msodrawxml:model3d>
      </mc:Choice>
      <mc:Fallback>
        <a:pic><a:blipFill><a:blip r:embed="rId2"/></a:blipFill></a:pic>
      </mc:Fallback>
    </mc:AlternateContent>
  </drawing>
</worksheet>
```

**Expected:** Fallback picture imported. Cell A1 with value 42 present. 3D model NOT present.

### X3: XLSX with Multiple Eligible Choices

**Package:** `xl/worksheets/sheet1.xml`

```xml
<worksheet xmlns="http://schemas.openxmlformats.org/spreadsheetml/2006/main"
           xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
           xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main">
  <sheetData>
    <row r="1">
      <c r="A1"><v>42</v></c>
    </row>
  </sheetData>
  <drawing>
    <mc:AlternateContent>
      <mc:Choice Requires="dml">
        <a:pic><a:blipFill><a:blip r:embed="rId1"/></a:blipFill></a:pic>
      </mc:Choice>
      <mc:Choice Requires="dml">
        <a:pic><a:blipFill><a:blip r:embed="rId2"/></a:blipFill></a:pic>
      </mc:Choice>
      <mc:Fallback>
        <a:pic><a:blipFill><a:blip r:embed="rId3"/></a:blipFill></a:pic>
      </mc:Fallback>
    </mc:AlternateContent>
  </drawing>
</worksheet>
```

**Expected:** First Choice picture (rId1) imported. Cell A1 with value 42 present. Second Choice and Fallback NOT present.

---

## 3. Calc Pipeline Integration

| Component | File | Role |
|-----------|------|------|
| Fragment handler | `main/oox/source/core/fragmenthandler.cxx:109` | Intercepts `XML_AlternateContent`, creates `MceAlternateContext` |
| Worksheet fragment | `main/oox/source/xls/worksheetfragment.cxx` | Subclass of `FragmentHandler`, overrides `createFastChildContext` for DrawingML |
| Drawing manager | `main/oox/source/xls/drawingmanager.cxx` | Manages drawing import for Calc |

### 3.1 Calc Pipeline Integration Points

| Component | File | Role |
|-----------|------|------|
| Fragment handler | `main/oox/source/core/fragmenthandler.cxx:109` | Intercepts `mc:AlternateContent`, creates `MceAlternateContext` |
| Worksheet fragment | `main/oox/source/xls/worksheetfragment.cxx` | Subclass of `FragmentHandler`, overrides `createFastChildContext` for DrawingML |
| Drawing manager | `main/oox/source/xls/drawingmanager.cxx` | Manages drawing import for Calc |

### 3.2 Calc-Specific Concerns

| Concern | Assessment |
|-----------|------------|
| `WorksheetFragment::createFastChildContext()` | `MceAlternateContext` delegates to `WorksheetFragment`. DrawingML elements inside Choice branch should be processed correctly. |
| Shape table processing | `pSpTree` contains both regular shapes and `mc:AlternateContent`. The `MceAlternateContext` is created by `FragmentHandler::createFastChildContext()` when the parser encounters `mc:AlternateContent` as a child of `pSpTree`. |

---

## 4. Calc Test Results (NOT RUN)

| Test | Expected | Actual | Status |
|------|----------|--------|--------|
| X1 | Picture from Choice present, cell A1=42 | N/A | NOT RUN |
| X2 | Fallback picture present, 3D model absent | N/A | NOT RUN |
| X3 | First choice picture present, second absent | N/A | NOT RUN |
| X4 | No regression for non-MCE XLSX | N/A | NOT RUN |
