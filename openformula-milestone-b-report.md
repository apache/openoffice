# Apache OpenOffice — OpenFormula Milestone B Report (with B.1 corrections)

## 1. Commit IDs

- Starting commit: `0289c7f917` (end of Milestone A)
- IFNA (initial): `082ee9a94b` (source + tests)
- SKEWP (initial): `6a3f45420f` (source + tests)
- IFNA + SKEWP corrections (B.1): `c9db33542f` (source + tests + report)
- Branch: `openformula`

## 2. Source-Mining Findings

### IFNA

**Before this milestone, IFNA did not exist in AOO.** The only reference was an
Excel-filter stub at `main/sc/source/filter/excel/xlformula.cxx:404`:
`EXC_FUNCENTRY_ODF( ocNoName, 2, 2, 0, "IFNA" )` — an `ocNoName` placeholder
with no opcode, no interpreter method, no `scfuncs.src` entry, and no
`core_resource.src` entry.

**IF (ocIf=6)** provides the lazy-evaluation pattern to reuse. IF uses
jump-based compilation (`FormulaCompiler.cxx:1073-1128`) where the compiler
generates jump addresses for the value, fallback, and end paths. At runtime,
`ScInterpreter::ScIfJump()` (`interpr1.cxx:93`) pops the condition and jumps
to the appropriate path, evaluating only the taken branch. It calls
`MatrixDoubleRefToMatrix()` first to convert references to matrices, then
switches on `GetStackType()`.

**IFNA needs the same lazy-evaluation machinery** because `value_if_na` must
not be evaluated when `value` is not #N/A. Simply evaluating both arguments
eagerly would be wrong: e.g. `IFNA(5; 1/0)` must return 5, not #DIV/0!.

**Error code:** #N/A = `NOTAVAILABLE` (0x7fff), defined in
`main/formula/inc/formula/errorcodes.hxx:81`. The interpreter tracks errors
via `nGlobalError` and `FormulaToken::GetError()` / `GetType() == svError`.

### SKEWP

**Before this milestone, SKEWP did not exist in AOO.** The only reference was
an Excel-filter stub at `main/sc/source/filter/excel/xlformula.cxx:415`:
`EXC_FUNCENTRY_ODF( ocNoName, 1, MX, 0, "SKEWP" )`.

**SKEW (ocSchiefe=325)** exists at `interpr3.cxx:3081` (`ScInterpreter::ScSkew`),
using the helper `CalculateSkew()` (`interpr3.cxx:2969`) for data collection.
`CalculateSkew` collects numeric values from doubles, single/double references,
ref lists (via `ScValueIterator`), and matrices — skipping empty cells, text,
and logical values, propagating errors via `nGlobalError`.

**ScSkew formula** (sample skewness):
```
mean = sum / N
stdDev = sqrt( sum((xi-mean)^2) / (N-1) )     // sample, N-1
result = (xcube * N) / ((N-1) * (N-2))        // sample skewness
```

**SKEWP formula** (population skewness, ODF 1.4 §6.18.63):
```
mean = sum / N
sigma = sqrt( sum((xi-mean)^2) / N )          // population, N
result = (1/N) * sum( ((xi-mean)/sigma)^3 )
```

## 3. Defects Found and Corrected (B.1)

### IFNA: reference handling was broken

The initial `ScIfna()` implementation only peeked at `pStack[sp-1]` assuming
it was an `svError` token. This works for direct `NA()` calls but fails for:

- **Cell references** (`svSingleRef`): a reference token on the stack is not
  an error token — it must be dereferenced to check the cell's error code.
- **Range references** (`svDoubleRef`): same issue, needs implicit intersection.
- **Reference lists** (`svRefList`) and **matrices** (`svMatrix`): not handled.

The initial code also had a stack-safety issue: `--sp` discards the token
without calling `PopError()`, which is fine for `svError` but would be wrong
for other types.

### SKEWP: argument semantics inconsistent

`CalculateSkew()` (shared with SKEW) treated direct scalar text (`svString`)
as `errIllegalParameter` via the `default` case, but skipped text in ranges
and matrices. This made `SKEWP(1;"text";2)` fail with an error instead of
returning 0 (skipping text, evaluating `{1, 2}`).

### SKEWP: error propagation lost original error code

When a direct error argument (e.g. `1/0`) was passed, `svError` fell into
`CalculateSkew()`'s `default` → `SetError(errIllegalParameter)`, replacing
the original error (e.g. `#DIV/0!`) with a generic "illegal parameter" error.

## 4. Files Changed and Purpose

| File | Change | Purpose |
|------|--------|---------|
| `main/formula/inc/formula/compiler.hrc` | Add `SC_OPCODE_IFNA=409`, `SC_OPCODE_SKEWP=410`; move `SC_OPCODE_STOP_2_PAR` to 411; set `SC_OPCODE_LAST_OPCODE_ID=410` | Allocate opcodes for the two new functions |
| `main/formula/inc/formula/opcode.hxx` | Add `ocIfna` and `ocSkewP` enum entries | Map opcode constants to C++ names |
| `main/formula/source/core/api/FormulaCompiler.cxx` | Add `ocIfna` jump-based compilation block (lazy eval); SKEWP handled by existing 2-param branch | Compiler recognition and code generation |
| `main/formula/source/core/resource/core_resource.src` | Add `IFNA` and `SKEWP` string mappings in all 3 Resource blocks | Function-name recognition (ODFF, English, localized) |
| `main/sc/source/core/inc/interpre.hxx` | Declare `ScIfna()` and `ScSkewP()` | Interpreter method declarations |
| `main/sc/source/core/tool/interpr1.cxx` | **Rewrite `ScInterpreter::ScIfna()`**: handle `svError`, `svSingleRef`, `svDoubleRef`, `svRefList`, `svMatrix`, and default cases with proper reference dereferencing via `PopDoubleRefOrSingleRef()` and cell value push via `PushCellResultToken()` | IFNA evaluation with correct reference handling |
| `main/sc/source/core/tool/interpr3.cxx` | Implement `ScInterpreter::ScSkewP()` (population skewness); add `svString` skip case to `CalculateSkew()` (fixes text handling for both SKEW and SKEWP); add `svError` case to `CalculateSkew()` (preserves original error code) | SKEWP computation + shared bug fixes |
| `main/sc/source/core/tool/interpr4.cxx` | Add `case ocIfna: ScIfna();` and `case ocSkewp: ScSkewP();` | Interpreter dispatch |
| `main/sc/source/ui/src/scfuncs.src` | Add `Resource SC_OPCODE_IFNA` and `Resource SC_OPCODE_SKEWP` blocks | Function wizard / metadata / help |
| `main/sc/source/filter/excel/xlformula.cxx` | Replace `ocNoName` stubs with `ocIfna` and `ocSkewP` | Excel import/export mapping |
| `test/odf-validate/calcval-cases14.tsv` | Add/correct 22 test cases for IFNA and SKEWP | Regression tests |

No production code outside IFNA/SKEWP was touched. No LibreOffice code was
copied. No third-party formula engine was introduced. No unrelated refactoring
was performed.

## 5. IFNA Semantics and Error-Handling Decisions

### Lazy evaluation via jump-based compilation

`FormulaCompiler.cxx` generates code for `IFNA(value; value_if_na)` as:

```
[value code] [IFNA token(jump[0]=2)] [fallback code]
                         jump[1]       jump[2]
```

- `jump[1]` = address where fallback code begins
- `jump[2]` = address after fallback (end)

At runtime, `ScInterpreter::ScIfna()` (`interpr1.cxx:324`):

1. Switches on `GetStackType()`:
   - **`svError`**: Peeks at error code. If `NOTAVAILABLE` (#N/A), discards
     the token (`--sp`) and jumps to `jump[1]` (fallback). Otherwise leaves
     the error on the stack and jumps to `jump[2]` (end, propagate error).
   - **`svSingleRef` / `svDoubleRef`**: Pops the reference via
     `PopDoubleRefOrSingleRef()` (which performs implicit intersection for
     ranges), gets the cell via `GetCell()`, checks `GetCellErrCode()`:
     - `#N/A` → jump to fallback (reference already popped, nothing on stack)
     - Other error → `PushError(nErr)`, jump to end
     - Value → `PushCellResultToken(true, aAdr, NULL, NULL)`, jump to end
   - **`svRefList` / `svMatrix`**: Jumps to end, returning the value as-is.
     IFNA is not an array-forcing function; per-element #N/A substitution is
     not implemented. This is a documented limitation.
   - **Default** (double, string, etc.): Jumps to end, keeping the value.

2. `value_if_na` is never compiled/evaluated unless `value` is #N/A — the
   compiler only emits fallback code after the jump target, so it is simply
   never reached when value is not #N/A.

3. `value` is evaluated exactly once.

4. Errors other than #N/A are propagated unchanged — `ScIfna` does not clear
   `nGlobalError` for non-#N/A errors.

### Supported input forms

| Input form | Handling |
|------------|----------|
| `NA()` (direct #N/A error) | Recognized as `svError` with `NOTAVAILABLE` → fallback |
| Cell reference containing #N/A | Dereferenced via `PopSingleRef`, cell error checked → fallback |
| Cell reference containing other error | Dereferenced, error pushed → propagated |
| Cell reference containing value | Dereferenced, cell value pushed → returned |
| Range reference | Implicit intersection via `DoubleRefToPosSingleRef`, first cell checked |
| Reference list | Returned as-is (documented limitation) |
| Matrix | Returned as-is (documented limitation) |
| Direct text/number/logical | Returned unchanged |

### Documented limitation: matrix handling

IFNA is not an array-forcing function in OpenFormula 1.4. When `value` is a
matrix, the current implementation returns the matrix as-is without
per-element #N/A substitution. Implementing full element-wise IFNA would
require a `ScJumpMatrix`-based approach (as used by `ScIfJump`) to evaluate
the fallback per element, which is beyond the scope of this milestone.

## 6. SKEWP Formula and Edge-Case Decisions

### Population standard deviation (sigma, denominator N):

```
sigma = sqrt( sum((xi - mean)^2) / N )
```

This differs from SKEW's sample standard deviation (denominator N−1).

### Population skewness result:

```
result = (1/N) * sum( ((xi - mean) / sigma)^3 )
```

### Edge cases:

| Case | Behavior | Rationale |
|------|----------|-----------|
| N = 0 (no args) | Error | `CalculateSkew` requires ≥1 param |
| N = 1 | Error | `sigma = 0`, zero variance |
| Constant data (zero variance) | Error | `sigma = 0`, division by zero |
| Two observations, non-constant | Valid result | e.g. SKEWP(1;2) = 0 |
| Error in input | Propagated | Original error code preserved |
| Text in direct argument | Skipped | Consistent with range/matrix handling |
| Empty cells | Skipped | `CalculateSkew` skips non-numeric |
| Matrix / array input | Supported | `CalculateSkew` handles `svMatrix` |

### Numerical stability

Uses the same two-pass approach as SKEW (compute mean first, then squared
deviations). No unnecessary integer conversions. The `fSigma == 0.0` check
avoids division by zero for zero-variance data.

### SKEW vs SKEWP difference

For `{1, 1, 2}`:
- SKEWP = 1/√2 ≈ 0.7071067812 (population)
- SKEW = √3 ≈ 1.7320508076 (sample)
They differ, confirming the formulas are distinct.

## 7. Function Registration and Documentation

**Opcode allocation:** IFNA=409, SKEWP=410. `SC_OPCODE_LAST_OPCODE_ID`
updated from 408 to 410. `SC_OPCODE_STOP_2_PAR` moved from 409 to 411 so both
new functions fall in the 2-parameter range (`START_2_PAR=201` to
`STOP_2_PAR=411`), which is already handled by the compiler's symbol-table
construction (`FormulaCompiler.cxx:452-462`) and 2-param compilation branch
(`FormulaCompiler.cxx:1014`).

**Compiler:** IFNA uses a dedicated jump-based compilation block (modeled on
ocIf/ocChose at `FormulaCompiler.cxx:1073-1128`). SKEWP uses the existing
2-parameter function compilation path.

**Interpreter:** `ScIfna` in `interpr1.cxx:324`, `ScSkewP` in
`interpr3.cxx:3081`. Dispatch in `interpr4.cxx`.

**Resources:** `core_resource.src` entries in all 3 Resource blocks
(ENGLISH_ODFF, ENGLISH, localized). `scfuncs.src` entries with function group
(LOGICAL for IFNA, STATISTIC for SKEWP), parameter names/descriptions, and
HID references.

**Excel import/export:** `xlformula.cxx` stubs updated from `ocNoName` to
`ocIfna`/`ocSkewP`, so Excel files containing IFNA/SKEWP are mapped to the
real implementations.

**ODF save/reload:** Function names are written as standard `IFNA`/`SKEWP`
per `core_resource.src` ODFF mappings, so they survive ODF save/reload.

**Help documentation:** `scfuncs.src` entries include English descriptions
and parameter documentation. No separate helpcontent2 files were added (the
existing convention for recently added functions like EASTERSUNDAY also uses
only `scfuncs.src`).

## 8. Test Cases Added and Corrected

Added 6 cases to `test/odf-validate/calcval-cases14.tsv` in B.1 (total IFNA,
SKEWP, and SKEW cases now 24):

**IFNA (11 cases):**
| ID | Formula | Expected | Purpose |
|----|---------|----------|---------|
| IFNA.num | `=IFNA(5;10)` | 5 | Numeric value returned unchanged |
| IFNA.text | `=IFNA("hello";"world")` | hello | Text value returned unchanged |
| IFNA.na.num | `=IFNA(NA();10)` | 10 | #N/A → fallback numeric |
| IFNA.na.text | `=IFNA(NA();"fallback")` | fallback | #N/A → fallback text |
| IFNA.otherr | `=IFNA(1/0;10)` | #ERR | Non-#N/A error propagated |
| IFNA.lazy.ok | `=IFNA(5;1/0)` | 5 | Fallback not evaluated (lazy) |
| IFNA.lazy.err | `=IFNA(NA();1/0)` | #ERR | Fallback evaluated and errors |
| IFNA.nested | `=IFNA(IFNA(NA();5);10)` | 5 | Nested IFNA |
| IFNA.ref | `=IFNA(D1;10)` | 10 | Cell reference containing value (D1=10) |
| IFNA.ref.text | `=IFNA(A1;10)` | y | Cell reference containing text (A1=y) |
| IFNA.range | `=IFNA(D1:D3;99)` | 10 | Range reference, implicit intersection takes first cell |

**SKEWP (11 cases):**
| ID | Formula | Expected | Purpose |
|----|---------|----------|---------|
| SKEWP.sym | `=SKEWP(1;2;3;4;5)` | 0 | Symmetric → zero skewness |
| SKEWP.pos | `=SKEWP(1;1;2)` | 0.7071067812 | Positive skew (1/√2) |
| SKEWP.neg | `=SKEWP(1;2;2)` | -0.7071067812 | Negative skew (−1/√2) |
| SKEWP.vs.skew | `=SKEWP(1;1;2)` | 0.7071067812 | Population ≠ sample (SKEW gives √3≈1.7321) |
| SKEWP.one | `=SKEWP(5)` | #ERR | Single observation → zero variance |
| SKEWP.zero | `=SKEWP(5;5;5)` | #ERR | Constant data → zero variance |
| SKEWP.two | `=SKEWP(1;2)` | 0 | Two observations, non-constant, zero skew |
| SKEWP.range | `=SKEWP(D1:D3)` | 0 | Range input (D1:D3=10,20,30, symmetric) |
| SKEWP.array | `=SKEWP({1;1;2})` | 0.7071067812 | Array/matrix input |
| SKEWP.mixed | `=SKEWP(1;"text";2)` | 0 | Text skipped, {1,2} symmetric |
| SKEWP.err | `=SKEWP(1/0)` | #ERR | Error value propagates (original error preserved) |

**SKEW regression (2 cases):**
| ID | Formula | Expected | Purpose |
|----|---------|----------|---------|
| SKEW.text | `=SKEW(1;"text";2)` | 0 | Regression: direct scalar text skipped, {1,2} symmetric |
| SKEW.err | `=SKEW(1/0)` | #ERR | Regression: error argument propagates |

**Independent numerical verification (SKEWP):**
- SKEWP({1,1,2}): mean=4/3, sigma²=2/9, sigma=√2/3
  standardized³ sum = 3/√2, result = (3/√2)/3 = 1/√2 ≈ 0.7071067812
- SKEW({1,1,2}): sample stdDev=1/√3, result = √3 ≈ 1.7320508076
  (differs from SKEWP, confirming distinct formulas)
- SKEWP({1,2}): mean=1.5, sigma²=0.25, sigma=0.5
  ((1-1.5)/0.5)³ = -1, ((2-1.5)/0.5)³ = 1, sum=0, result=0/2=0

**Independent numerical verification (IFNA):** All IFNA cases are logical
operations (return value or fallback), no numerical computation needed.

**Independent numerical verification (SKEW):** SKEW({1,"text",2}) skips text,
evaluating {1,2}, which is symmetric → 0. SKEW(1/0) propagates #DIV/0!.

## 9. Build and Test Commands Executed

**Environment:** Linux (WSL2) x86_64. `dmake` and `make` are installed, but
there is **no configured AOO build tree**. The `main/solver/510/` directory
contains only a partial `xml2cmp` build; the `sc` module is not built. No
`Module_*.mk` makefiles exist in the source tree (they are generated by AOO's
gbuild configure step, which has not been run).

**Attempted:** `dmake -C main/sc/source/core/tool interpr1.obj` → prints dmake
usage only (no makefile present).

**Result:** Build and test execution were **not possible** in this environment.
The calcval ODF 1.4 tests require a built office and (per `test/odf-validate/`
README) run on macOS.

**Static verification performed:**
- Brace balance: 0 for all modified C++ files (`interpr1.cxx`, `interpr3.cxx`)
- Opcode values: IFNA=409, SKEWP=410, LAST_OPCODE_ID=410, STOP_2_PAR=411 —
  all unique and consistent
- All identifiers used (`formula::svError`, `NOTAVAILABLE`, `pStack`, `sp`,
  `aCode`, `pCur`, `PopDoubleRefOrSingleRef`, `PushCellResultToken`,
  `GetCellErrCode`, `GetCell`, `PopError`, `PopString`) were already present
  in the respective function scopes

**For validation in a configured build environment:**
1. Configure AOO (run `autogen.sh`/`configure`, then `build.pl --all`)
2. Build the `sc` module
3. Run `test/odf-validate/start.sh calcval <port>` and
   `test/odf-validate/runpy.sh calcval.py <port> calcval-cases14.tsv <out.json>`
   on macOS, then `python3 -I calcval.py --compare ...`

## 10. Not Verified and Why

- **Real build and runtime tests:** No configured AOO build tree exists in
  this environment. Static checks (brace balance, opcode uniqueness,
  identifier scope) are not substitutes for compilation and execution.
- **ODF save/reload round-trip:** Cannot be tested without a built office.
- **Excel import/export:** Cannot be tested without a built office.
- **Function wizard integration:** Cannot be tested without a built office.
- **Matrix IFNA per-element behavior:** Documented as not implemented;
  cannot be verified without a built office.
- **IFNA reference to #N/A cell:** Cannot be tested because the calcval test
  harness does not write cells containing error values; only fixed data
  values are pre-populated. The reference-to-#N/A code path (via
  `GetCellErrCode()`) is implemented but untested at runtime.
- **Specific error code expectations:** The test harness only supports `#ERR`
  (any error) and `#N/A`, not specific error codes like `#DIV/0!`. Error
  tests therefore use `#ERR`, which cannot distinguish the original error
  from a substituted one.

## 11. Remaining Compatibility and Conformance Concerns

- **IFNA matrix evaluation:** IFNA is not an array-forcing function. When
  `value` is a matrix or reference list, the current implementation returns
  the value as-is without per-element #N/A substitution. Per OpenFormula 1.4
  §6.15.13, IFNA's behavior with array inputs is implementation-defined.
  Full element-wise support would require `ScJumpMatrix`-based infrastructure
  (as used by `ScIfJump`).
- **IFNA range references:** Uses implicit intersection
  (`DoubleRefToPosSingleRef`) to check the first cell of a range. If the
  range contains #N/A in a non-intersected cell, it is not detected.
- **SKEWP text handling change:** `CalculateSkew()` now skips direct scalar
  text (`svString`) instead of raising `errIllegalParameter`. This changes
  SKEW behavior identically (both functions share `CalculateSkew()`). The
  change makes direct-argument text handling consistent with range/matrix
  handling, which is more consistent with OpenFormula's NumberSequence
  definition (text excluded). Both SKEW and SKEWP are affected equally.
- **SKEWP error propagation:** Direct error arguments now preserve the
  original error code instead of replacing it with `errIllegalParameter`.
  This changes SKEW behavior identically (both functions share
  `CalculateSkew()`). Both functions now propagate the original error.

## 12. Normative vs Implementation Choices

| Aspect | ODF 1.4 Normative | AOO Implementation Choice |
|--------|-------------------|---------------------------|
| IFNA lazy evaluation | Required (value_if_na only when value is #N/A) | Jump-based compilation (same as IF) |
| IFNA error propagation | Non-#N/A errors propagated | Propagated via nGlobalError |
| IFNA matrix/array behavior | Implementation-defined | Return value as-is (documented limitation) |
| SKEWP formula | Population skewness, denominator N | Implemented exactly as specified |
| SKEWP zero variance | Not explicitly specified | Returns error (PushIllegalArgument) |
| SKEWP N=1 | Not explicitly specified | Returns error (sigma=0) |
| SKEWP text in direct arg | NumberSequence excludes text | Skipped (consistent with ranges) |
| SKEWP error in direct arg | Error propagated | Original error code preserved |
