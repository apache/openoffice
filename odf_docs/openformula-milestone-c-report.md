# Apache OpenOffice — OpenFormula Milestone C Report

## 1. Commit Information

- Branch: `openformula` (rebased onto `trunk`)
- Baseline commit: `fd13af33ac` (end of Milestone B.2)
- Final commit: Milestone C changes (this commit)
- Repository: https://github.com/pgiffuni/openoffice

## 2. Source-Mining Findings

### CONVERT Implementation Architecture

The CONVERT function is implemented in the Analysis Add-in at
`main/scaddins/source/analysis/analysishelper.cxx`. The architecture consists of:

- **ConvertData** class: Represents a single unit with a name, conversion factor
  (`fConst`, units per base unit), class (e.g. CDC_Pressure, CDC_Length), and
  prefix support flag (`bPrefixSupport`).
- **ConvertDataLinear** subclass: Handles temperature units with offset
  (Celsius, Fahrenheit, etc.).
- **ConvertDataList**: Maintains the registered unit table and performs
  conversions via `ConvertDataList::Convert()`.
- **GetMatchingLevel()**: Resolves unit names including prefix matching,
  squared/cubed suffixes (`^2`, `^3`), and the `da`/`e` deca legacy forms.
- **AnalysisAddIn::getConvert()**: Exposes CONVERT to the formula engine.

### Key Design Observations

- Conversion factors are stored as **units per base unit** (reciprocal of
  base-units-per-unit). For pressure, base = Pascal; for length, base = Meter;
  for area, base = m²; for volume, base = Liter.
- Prefix exponents on squared/cubed units are multiplied by 2 or 3
  (lines 2266-2273: "cm3" is 10⁻⁶ m³, not 10⁻² m³).
- Binary prefixes (Table 26) are only accepted for CDC_Information class
  (bit, byte).
- `da` (two-char) is deca (+1), `d` (one-char) is deci (-1), `e` is legacy deca (+1).

## 3. Unit-Table Comparison Against ODF 1.4 Table 24

All units in ODF 1.4 Tables 24-26 were audited against the registered table.
Categories:

| Category | Count | Details |
|----------|-------|---------|
| Required symbols implemented correctly | Most | Pa, m, in, ft, yd, g, N, J, W, T, C, F, K, bit, byte, etc. |
| Required symbols with corrected constants | 2 | `atm`, `Torr` (pressure constants were off) |
| Required symbols with missing aliases | 4 | `picapt`, `pica`, `pica2`, `pica3` (Pica family) |
| Required symbols missing entirely | 0 | All Table 24 required symbols present after fixes |
| Prefix handling correct | Yes | All decimal prefixes (Table 25), all binary prefixes (Table 26) |
| AOO/Excel extensions retained | Yes | All `***` marked extensions preserved |

### Specific Unit Comparisons

**Pressure (Table 24):**
| Unit | ODF 1.4 Value | Before Fix | After Fix | Status |
|------|--------------|------------|-----------|--------|
| Pa | 1 Pa | 1.0 | 1.0 | Correct |
| atm | 101325.1 Pa | 101324.996... Pa | 101325.1 Pa | Corrected |
| at | deprecated | 101324.996... Pa | unchanged | Legacy retained |
| SI_at | 98066.5 Pa | 98066.5 Pa | 98066.5 Pa | Already correct |
| Torr | 101325/760 Pa | 133.32237... Pa | 133.322368... Pa | Corrected |

**Pica family (Table 24):**
| Unit | Definition | Before | After |
|------|-----------|--------|-------|
| Pica / picapt | 1/72 inch | Present (Pica only) | Present + picapt alias |
| pica | 1/6 inch | Missing | Added |
| Pica² / picapt² / Pica^2 / picapt^2 | (1/72 in)² | Present (Pica2) | Present + picapt2 alias |
| pica² / pica^2 | (1/6 in)² | Missing | Added |
| Pica³ / picapt³ / Pica^3 / picapt^3 | (1/72 in)³ | Present (Pica3) | Present + picapt3 alias |
| pica³ / pica^3 | (1/6 in)³ | Missing | Added |

## 4. Constants Changed and Their Derivation

### Pressure (factors are units per Pascal):

**atm:** ODF 1.4 defines 1 atm = 101325.1 Pa.
- Factor = 1/101325.1 = 9.86922292699439618e-06
- Before: 9.8692329999819300E-06 (corresponds to 101324.996... Pa)
- Relative error before fix: ~1.0e-06

**Torr:** ODF 1.4 defines 1 Torr = 101325/760 Pa.
- Factor = 760/101325 = 7.50061682704169716e-03
- Before: 7.5006380000000000E-03
- Relative error before fix: ~2.8e-06

**SI_at:** Already correct: 1/98066.5 = 1.01971621297792819e-05
**at:** Preserved at legacy value (9.8692329999819300E-06, matches old atm).
ODF 1.4 marks `at` as deprecated and historically ambiguous; AOO retains the
standard-atmosphere interpretation for compatibility.

### Pica family (factors are units per base unit):

**Pica / picapt:** 1/72 inch = 0.0254/72 m.
- Factor = 72/0.0254 = 2.83464566929133889e+03 per meter
- Minor precision improvement from 2.8346456692913386E03.

**Pica² / picapt²:** (1/72 in)² per m² = 8.03521607043214142e+06.
**Pica³ / picapt³:** (1/72 in)³ per Liter = 2.27769904358706400e+07.

**pica:** 1/6 inch = 0.0254/6 m.
- Factor = 6/0.0254 = 2.36220472440944917e+02 per meter.

**pica²:** (1/6 in)² per m² = 5.58001116002232156e+04.
**pica³:** (1/6 in)³ per Liter = 1.31811287244621799e+04.

## 5. Prefix Behavior Audited (Tables 25-26)

**Decimal prefixes (Table 25):** All 20 prefixes (yocto through yotta)
recognized with correct exponents. `da` = deca (+1), `d` = deci (-1),
`e` = legacy deca (+1). No changes needed.

**Binary prefixes (Table 26):** All 8 prefixes (kibi through yobi)
recognized for CDC_Information class only (bit, byte). Rejected for other
classes. No changes needed.

**Squared/cubed prefix exponent multiplication:** Verified at lines 2266-2273.
`cm3` → exponent ×3 (10⁻⁶ m³), `cm2` → exponent ×2 (10⁻⁴ m²). Correct.

**Case sensitivity:** Unit name matching uses `equals()` which is case-sensitive.
`ATM` ≠ `atm`. No changes needed.

## 6. Tests Added to calcval-cases14.tsv

21 new test cases added:

**Pressure (1 case):**
| ID | Formula | Expected | Purpose |
|----|---------|----------|---------|
| CONVERT.Torr | =CONVERT(1;"Torr";"Pa") | 133.32236842105263 | Torr = 101325/760 Pa exactly |

**Pica family (15 cases):**
| ID | Formula | Expected | Purpose |
|----|---------|----------|---------|
| CONVERT.Pica.in | =CONVERT(1;"Pica";"in") | 0.013888888888888888 | Pica point = 1/72 in |
| CONVERT.picapt.in | =CONVERT(1;"picapt";"in") | 0.013888888888888888 | picapt alias = 1/72 in |
| CONVERT.pica.in | =CONVERT(1;"pica";"in") | 0.16666666666666666 | Pica = 1/6 in |
| CONVERT.Pica2.in2 | =CONVERT(1;"Pica2";"in2") | 0.00019290123456790123 | Square Pica point |
| CONVERT.pica2.in2 | =CONVERT(1;"pica2";"in2") | 0.027777777777777776 | Square Pica |
| CONVERT.Pica3.in3 | =CONVERT(1;"Pica3";"in3") | 0.0000026791838134430725 | Cubic Pica point |
| CONVERT.pica3.in3 | =CONVERT(1;"pica3";"in3") | 0.004629629629629629 | Cubic Pica |
| CONVERT.picapt2.in2 | =CONVERT(1;"picapt2";"in2") | 0.00019290123456790123 | picapt2 alias = square Pica point |
| CONVERT.picapt3.in3 | =CONVERT(1;"picapt3";"in3") | 0.0000026791838134430725 | picapt3 alias = cubic Pica point |
| CONVERT.Pica^2.in2 | =CONVERT(1;"Pica^2";"in2") | 0.00019290123456790123 | caret exponent spelling Pica^2 |
| CONVERT.picapt^2.in2 | =CONVERT(1;"picapt^2";"in2") | 0.00019290123456790123 | caret exponent spelling picapt^2 |
| CONVERT.pica^2.in2 | =CONVERT(1;"pica^2";"in2") | 0.027777777777777776 | caret exponent spelling pica^2 |
| CONVERT.Pica^3.in3 | =CONVERT(1;"Pica^3";"in3") | 0.0000026791838134430725 | caret exponent spelling Pica^3 |
| CONVERT.picapt^3.in3 | =CONVERT(1;"picapt^3";"in3") | 0.0000026791838134430725 | caret exponent spelling picapt^3 |
| CONVERT.pica^3.in3 | =CONVERT(1;"pica^3";"in3") | 0.004629629629629629 | caret exponent spelling pica^3 |

**Prefixes (3 cases):**
| ID | Formula | Expected | Purpose |
|----|---------|----------|---------|
| CONVERT.km.m | =CONVERT(1;"km";"m") | 1000 | Decimal prefix |
| CONVERT.cm2.m2 | =CONVERT(1;"cm2";"m2") | 0.0001 | Centi on squared unit |
| CONVERT.Kibyte.bit | =CONVERT(1;"Kibyte";"bit") | 8192 | Binary prefix |

**Invalid conversions (2 cases):**
| ID | Formula | Expected | Purpose |
|----|---------|----------|---------|
| CONVERT.km.Pa | =CONVERT(1;"km";"Pa") | #ERR | Different unit groups |
| CONVERT.ATM.Pa | =CONVERT(1;"ATM";"Pa") | #ERR | Case-sensitive unit names |

## 7. Static Checks Performed

- Brace balance: 0 for analysishelper.cxx
- All fConst values verified by independent Python computation
- Pica point factor matches existing value within precision improvement
- Pica²/Pica³ factors match existing values within precision improvement
- atm/Torr corrections use exact ODF 1.4 definitions (101325.1 Pa, 101325/760 Pa)
- Pica/pica factors derived from exact inch definition (0.0254 m, exact)
- Test expected values independently calculated from unit definitions

## 8. Build and Test Status

**No build or runtime tests were executed.** Per milestone instructions:
- The AOO `sc` and `scaddins` modules are not configured for build in this
  environment (no `Module_*.mk` makefiles, no generated build tree).
- `dmake` cannot build without the gbuild configure step.
- The calcval test harness requires a built office and runs on macOS.

**For validation in a configured AOO build environment:**
1. Configure AOO (`autogen.sh`/`configure`, then `build.pl --all`)
2. Build the `scaddins` module (Analysis Add-in)
3. Run `test/odf-validate/start.sh calcval <port>` and
   `test/odf-validate/runpy.sh calcval.py <port> calcval-cases14.tsv <out.json>`
   on macOS, then `python3 -I calcval.py --compare ...`

## 9. Milestone B Report Correction

The B.2 section of the Milestone B report previously stated that B.2 added six
test cases. B.2 actually added four cases relative to B.1:
- IFNA.ref.text, IFNA.range, SKEW.text, SKEW.err (4 cases, not 6).
The B.1 baseline had IFNA.ref and SKEWP.two (2 cases).

## 10. Remaining Known Deviations

- **Build/runtime validation deferred:** No execution of the new CONVERT tests
  against a built office was possible in this environment.
- **`at` deprecated unit:** Retained at legacy standard-atmosphere value
  (9.8692329999819300E-06 units/Pa) for Excel compatibility. ODF 1.4 marks it
  as deprecated/ambiguous; no redefinition was made.
- **Pica precision:** Pica point factor updated from 2.8346456692913386E03 to
  2.83464566929133889E03 (full double precision of 72/0.0254). This is a
  precision improvement, not a semantic change.

## 11. Normative vs Implementation Choices

| Aspect | ODF 1.4 Normative | AOO Implementation |
|--------|-------------------|-------------------|
| atm = 101325.1 Pa | Mandatory | Corrected to match |
| Torr = 101325/760 Pa | Mandatory | Corrected to match |
| SI_at = 98066.5 Pa | Mandatory | Already matched |
| at = deprecated | Optional/legacy | Retained at standard-atmosphere value |
| Pica/picapt = 1/72 in | Mandatory | Implemented |
| pica = 1/6 in | Mandatory | Implemented |
| Pica²/pica² etc. | Mandatory | Implemented |
| Decimal prefixes | Mandatory | Implemented |
| Binary prefixes on bit/byte | Mandatory | Implemented |
| Case-sensitive unit names | Mandatory | Implemented |
| Extension units (`***`) | Not specified | Retained for compatibility |
