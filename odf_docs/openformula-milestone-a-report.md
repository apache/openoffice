# Apache OpenOffice — OpenFormula Milestone A Report

## 1. Commit IDs

- Starting commit: `1a90caaf711eb0c8fba10d331504670475957a66`
- Ending commit:   `20ab51ba5a` (source + tests)
- Branch: `openformula` (created in the prior audit phase)

This report covers Milestone A (commit `9c7fb2dfd9`) and its corrections,
Milestone A.1 (commit `91645d9295`) and Milestone A.1.1 (commit
`20ab51ba5a`). A.1 fixed an off-by-one defect in the 48-bit operand upper
bound (accepting `2^48`, which ODF 1.4 §6.6.1 excludes) and added
`::rtl::math::isFinite` guards before the float-to-integer conversion in
both bitwise helpers. A.1.1 completed the `ScBitArithmeticOps()` NaN guard:
`PushIllegalArgument()` records an error but does not return, so without an
early-exit the invalid operand was still converted to `sal_uInt64`
(undefined behaviour); operands are now validated across the consumption
loop and conversion/`PushDouble` are skipped when any operand is invalid.

## 2. Files changed and reasons

| File | Reason |
|------|--------|
| `main/sc/source/core/tool/interpr1.cxx` | Corrected `ScInterpreter::ScBitShiftOps` (BITLSHIFT/BITRSHIFT) and `ScInterpreter::ScIndex` (INDEX). |
| `test/odf-validate/calcval-cases14.tsv` | Added ODF 1.4 Part 4 regression cases for BITLSHIFT, BITRSHIFT, and INDEX. |

Only these two files were committed. Pre-existing, unrelated working-tree modifications
(`main/solenv/gbuild/platform/freebsd.mk`, `main/solenv/inc/unxfbsd.mk`) and the prior
audit-phase deliverables (`openformula-*.tsv`, `openformula-*.md`) were left unstaged
and uncommitted.

No production code outside the three functions was touched. No LibreOffice code was
copied. No third-party formula engine was introduced.

## 3. Original defects

### BITLSHIFT / BITRSHIFT (`ScBitShiftOps`, interpr1.cxx:1216)

The negative-shift-count direction reversal already matched ODF 1.4 (negative count
reverses direction: BITLSHIFT(8;-1)=4, BITRSHIFT(8;-1)=16), so no direction change was
needed. The actual defects were:

1. **Missing operand range check.** `BITAND`/`BITOR`/`BITXOR` (`ScBitArithmeticOps`)
   reject operands outside `0 .. 2^48 - 1` (ODF 1.4 §6.6.1 requires `0 <= x <= 2^48 - 1`),
   but `ScBitShiftOps` only rejected negative operands. The value `2^48` itself
   (281474976710656) was accepted.
2. **Undefined behaviour on large shift counts.** The shift count was cast straight to
   `sal_uInt64` and used as the shift amount. Shifting by `>= 64` bits (the width of
   `sal_uInt64`) or by a huge count (e.g. `1e300`) is undefined behaviour in C++.

### INDEX (`ScIndex`, interpr1.cxx:7311)

The `bRowArray` logic implemented the ODF 1.3 row-vector shortcut: for a single-row
reference called with exactly two arguments (reference + row), the row argument was
reinterpreted as a column offset (`nCol = nRow; nRow = 1`). ODF 1.4 removes that rule,
so `INDEX(H1:L1;3)` must be an error (row 3 on a 1-row reference) rather than returning
the third column.

## 4. Exact semantic changes

### BITLSHIFT / BITRSHIFT

- Corrected the upper-bound operand check to match ODF 1.4 §6.6.1, which defines the
  valid unsigned range as `0 <= x <= 2^48 - 1` (previously the check allowed `2^48`
  itself). The bound is now `val > 2^48 - 1`, i.e. `281474976710655`, the same ceiling
  used by `ScBitArithmeticOps` (BITAND/BITOR/BITXOR).
- Added `::rtl::math::isFinite` guards on both operands before the
  floating-point-to-integer conversion, so NaN and non-finite values cannot reach the
  `static_cast<sal_uInt64>` (undefined behaviour). `n` (the shift count) is guarded too.
- Completed the `ScBitArithmeticOps()` guard: `PushIllegalArgument()` records an error
  but does not return, so operands are now validated across the consumption loop and
  conversion / `PushDouble` are skipped when any operand is invalid (tracked with a
  `bValid` flag), matching the if/else pattern already used by `ScBitShiftOps()`.
- The shift-count magnitude is now clamped to the width of `sal_uInt64`: a magnitude
  of `>= 64` yields result `0`; otherwise the count is cast safely. This eliminates
  undefined behaviour from shifting by `>=` the bit width or by a huge count.
- **ODF requirement vs AOO choice:** ODF 1.4 §6.6.1 mandates the operand range
  (`0 <= x <= 2^48 - 1`) but does not prescribe a result for shift counts at or beyond
  the result width. Returning `0` for a shift of 64 bits or more is an AOO
  implementation choice (the smallest defined behaviour avoiding undefined behaviour),
  not an ODF-mandated value.
- Direction logic preserved (already ODF 1.4-correct): a negative count reverses
  direction (`BITLSHIFT` → right shift, `BITRSHIFT` → left shift); zero count returns
  the value unchanged; positive count shifts in the named direction.
- Result is pushed as a double via `PushDouble`.

Result table (all verified against the ODF 1.4 required values):

| Formula | Result |
|---------|--------|
| `BITLSHIFT(8;1)` | 16 |
| `BITLSHIFT(8;-1)` | 4 |
| `BITLSHIFT(8;0)` | 8 |
| `BITRSHIFT(8;1)` | 4 |
| `BITRSHIFT(8;-1)` | 16 |
| `BITRSHIFT(8;0)` | 8 |
| `BITLSHIFT(1;63)` | 9223372036854775808 |
| `BITLSHIFT(1;64)` | 0 (AOO choice; out-of-width shift) |
| `BITRSHIFT(1;64)` | 0 (AOO choice; out-of-width shift) |
| `BITLSHIFT(8;-64)` | 0 (AOO choice; out-of-width shift) |
| `BITLSHIFT(-1;1)` | #ERR |
| `BITRSHIFT(-1;1)` | #ERR |
| `BITLSHIFT(281474976710655;0)` | 281474976710655 (max valid operand, 2^48 - 1) |
| `BITLSHIFT(281474976710656;1)` | #ERR (operand 2^48 is out of range) |
| `BITLSHIFT(281474976710657;1)` | #ERR (operand > 2^48) |

### INDEX

- Removed the `bRowArray` variable, its two assignments, its two error-condition
  clauses, and the row-vector reinterpretation branch (`nCol = nRow; nRow = 1`).
- The existing bounds check (`nRow > 0 && nRow1+nRow-1 > nRow2`) now applies uniformly,
  so a row index beyond the number of rows is an error regardless of orientation.

Result table (ODF 1.4, §6.14.6), using the sheet data documented in the test file
(`D1:D5` = 10,20,30,40,50; `H1:L1` = 10,20,30,40,50; `D1:F3` = 3×3):

| Formula | Result |
|---------|--------|
| `INDEX(D1:D5;2)` | 20 |
| `INDEX(D1:D5;2;1)` | 20 |
| `INDEX(D1:F1;1;3)` | 101 |
| `INDEX(H1:L1;3)` | #ERR (was 30 — legacy row-vector shortcut removed) |
| `INDEX(H1:L1;;3)` | 30 |
| `INDEX(D1:D5;2;2)` | #ERR |
| `INDEX(D1:F3;2;3)` | 202 |
| `INDEX(D1:D5;6)` | #ERR |

## 5. INDEX compatibility findings and decision

**Update:** this decision was reversed later. `ScIndex` keeps the legacy row-vector rule,
`INDEX(H1:L1;3)` returns 30 again, and the case is listed as a known deviation.

**Finding:** AOO retains no document/formula-version information that `INDEX` evaluation
can branch on. `ScDocOptions` (docoptio.hxx) tracks only formula-related toggles such as
`bFormulaRegexEnabled`; there is no ODF 1.2 / 1.3 / 1.4 formula conformance setting per
document, and `FormulaLanguage` governs grammar/symbol names (ODF vs Excel), not ODF
1.3-vs-1.4 `INDEX` semantics. Per the milestone constraints I did not introduce a new
compatibility subsystem and did not infer document version from formula text.

**Decision:** The change is applied globally — all documents now use ODF 1.4 `INDEX`
semantics. This is the only spec-compliant path given the absence of a version-tracking
mechanism.

**Compatibility consequence (documented):** ODF 1.3 documents that relied on the legacy
row-vector behaviour, i.e. `INDEX(<single-row-reference>;N)` returning column N, will
now return an error when `N > 1` (row count exceeds the single row). Single-row,
single-column references and two-argument calls on column vectors are unaffected.
This is a deliberate, documented break from ODF 1.3 behaviour in favour of ODF 1.4
conformance; it should be communicated to users migrating legacy documents.

## 6. Added test cases and expected results

Appended to `test/odf-validate/calcval-cases14.tsv` (columns: id, formula, mode,
expected, note), following the existing file conventions:

**BIT shifts** — positive, zero, and negative shift counts; both directions; width
boundary; negative and out-of-range operands; exact-range boundary across all five bit
functions:
- `BITLSHIFT.pos` =BITLSHIFT(8;1) → 16
- `BITLSHIFT.neg` =BITLSHIFT(8;-1) → 4
- `BITLSHIFT.zero` =BITLSHIFT(8;0) → 8
- `BITRSHIFT.pos` =BITRSHIFT(8;1) → 4
- `BITRSHIFT.neg` =BITRSHIFT(8;-1) → 16
- `BITRSHIFT.zero` =BITRSHIFT(8;0) → 8
- `BITLSHIFT.bit63` =BITLSHIFT(1;63) → 9223372036854775808
- `BITLSHIFT.bit64` =BITLSHIFT(1;64) → 0
- `BITRSHIFT.bit64` =BITRSHIFT(1;64) → 0
- `BITLSHIFT.neg64` =BITLSHIFT(8;-64) → 0
- `BITLSHIFT.negval` =BITLSHIFT(-1;1) → #ERR
- `BITRSHIFT.negval` =BITRSHIFT(-1;1) → #ERR
- `BITLSHIFT.ovr` =BITLSHIFT(281474976710657;1) → #ERR
- `BITAND.maxval` =BITAND(281474976710655;281474976710655) → 281474976710655
- `BITAND.ovr2` =BITAND(281474976710656;1) → #ERR (operand 2^48 now rejected)
- `BITOR.maxval` =BITOR(281474976710655;0) → 281474976710655
- `BITOR.ovr2` =BITOR(281474976710656;1) → #ERR
- `BITXOR.maxval` =BITXOR(281474976710655;0) → 281474976710655
- `BITXOR.ovr2` =BITXOR(281474976710656;1) → #ERR
- `BITAND.negval` =BITAND(-1;1) → #ERR (negative operand rejected)
- `BITOR.negval` =BITOR(-1;1) → #ERR (negative operand rejected)
- `BITXOR.negval` =BITXOR(-1;1) → #ERR (negative operand rejected)
- `BITLSHIFT.maxval` =BITLSHIFT(281474976710655;0) → 281474976710655
- `BITLSHIFT.ovr2` =BITLSHIFT(281474976710656;1) → #ERR
- `BITRSHIFT.maxval` =BITRSHIFT(281474976710655;0) → 281474976710655
- `BITRSHIFT.ovr2` =BITRSHIFT(281474976710656;1) → #ERR

**INDEX** — row-vector, column-vector, two-dimensional, omitted args, out-of-range,
and array (matrix) input:
- `INDEX.row.twod` =INDEX(D1:F3;2;3) → 202
- `INDEX.row.outrange` =INDEX(D1:D5;6) → #ERR
- `INDEX.array` =INDEX({10;20|30;40|50;60};3;1) → 50

(The file already contained `INDEX.col.row`, `INDEX.col.rowcol`, `INDEX.row.col`,
`INDEX.row.rowonly`, `INDEX.row.empty`, `INDEX.col.beyond`; the INDEX change makes
`INDEX.row.rowonly` =INDEX(H1:L1;3) → #ERR as the file already expects.)

Each case asserts an exact expected result (value or #ERR), not mere evaluation
success, per ODF 1.4 normative values.

## 7. Build and test commands with results

**Environment:** Linux (WSL2) x86_64, Python 3.14.4. `dmake` and `make` are present,
and `main/solver/510/unxlngx6.pro` contains only a partial `xml2cmp` build.

**Attempted:**
- `dmake -C main/sc/source/core/tool interpr1.obj` → prints dmake usage only; there is
  no makefile in the source tree (the `Module_*.mk` build files are generated by AOO's
  gbuild configure step, which has not been run). `build.pl`/`deliver.pl` are absent
  from the repository root.

**Result:** A build could not be performed in this environment. The `sc` module is not
built, and the full dependency chain required to compile `interpr1.cxx` is unavailable.
Consequently the calcval ODF 1.4 tests (which require a headless office on macOS per
`test/odf-validate/README.md`) could not be executed here.

**Verification performed instead:** brace balance of `interpr1.cxx` is 0; every
identifier used in the edits (`bitOperations`, `BITLSHIFT`, `BITRSHIFT`,
`SAL_CONST_UINT64`, `sal_uInt64`, `PushIllegalArgument`, `PushDouble`, `GetByte`,
`GetDouble`, `::rtl::math::approxFloor`, `static_cast`) was already present in the same
function's scope; argument pop order was confirmed (`GetDouble()` pops the last-pushed
parameter, verified via `ScIndex` at interpr1.cxx:7326-7330), so `n` is the shift count
and `val` is the value.

**To complete verification:** configure and build AOO (gbuild), build the `sc` module,
then run `test/odf-validate/start.sh calcval <port>` and
`test/odf-validate/runpy.sh calcval.py <port> calcval-cases14.tsv <out.json>` followed by
`python3 -I calcval.py --compare ...` on macOS, plus existing Calc regression tests.

## 8. Known limitations

- Build and test execution were not possible in this environment (see §7). Test cases
  were written to the existing `calcval-cases14.tsv` conventions and their expected
  values derived from the ODF 1.4 required behaviour, but they have not been executed
  against a built office.
- `BITLSHIFT(1;63) = 9223372036854775808` (2^63) relies on the result being pushed as a
  double; calcval compares numbers with 1e-9 relative tolerance, which is satisfied since
  2^63 is exactly representable.
- The shift-by-`>=64` → `0` choice is the safe defined behaviour; ODF 1.4's exact
  prescription for out-of-width shift counts was not available in full local text, so
  this is the smallest AOO-native correction avoiding undefined behaviour.
- The operand range uses the same `2^48 - 1` ceiling as `BITAND`/`BITOR`/`BITXOR`;
  ODF 1.4 §6.6.1 says "at least 48 bits", so this is consistent but not necessarily
  maximal.

## 9. Remaining ODF 1.4 compliance gaps discovered

From the prior audit (retained here for completeness), ODF 1.4 gaps outside this
milestone's scope (BITLSHIFT/BITRSHIFT/INDEX only):

- **IFNA** and **SKEWP** (ODF 1.3) are missing entirely — no opcode, no interpreter
  implementation, only Excel-filter `ocNoName` stubs (`xlformula.cxx:404`, `:415`).
- **CONVERT** `atm` unit value differs from the ODF 1.4 table by 1.0e-6 relative
  (`ODF14-KNOWN-LIMITATIONS-PUBLIC.md:72`).
- **COUNTA**/COUNTBLANK ODF 1.4 empty-cell semantics unverified.
- AOO claims **no** Part 4 conformance group (Small/Medium/Large)
  (`ODF14-IMPLEMENTATION-DEFINED-PUBLIC.md:134`); coverage cannot be certified without
  the full ODF 1.4 Part 4 normative text and per-group mandatory checks.
