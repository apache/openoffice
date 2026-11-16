# OpenFormula release notes (draft)

As-of 2026-11-16.

## What's new
- **IFNA(Value; Value_if_na)** gives you Value, unless it's `#N/A`, in which case you get `Value_if_na`. Other errors pass straight through, and Value_if_na only gets evaluated when it's needed. In an array formula, each `#N/A` in a range or array is replaced on its own.
- **SKEWP** is the population version of SKEW. You'll get `Err:502` if there's only one value, or if all the values are the same.
- **IFERROR(Value; Alternative)** gives you Value, unless it's an error value of any kind, in which case you get Alternative. Like IFNA, Alternative only gets evaluated when it's needed, an empty cell counts as 0, and in an array formula each error is replaced on its own.
- There are help topics now for IFERROR, IFNA, SKEWP, BITAND, BITOR, BITXOR, BITLSHIFT and BITRSHIFT, linked from the function lists.

## What changed
- Formulas can hold 2048 tokens now, up from 512. A 1026-character `=1+1+...` formula works (it gave Err:512 before). A function takes up to 255 arguments; with 256 or more you get `Err:512`. This was checked with `SUM`.
- If you type a formula that's too long, it's refused: you see "`Error: Formula overflow`" and the cell keeps what it had. If a file contains one, it keeps its text and shows `Err:512`.
- A string literal in a formula can be 32767 characters long. It used to stop at 1021 (1022 or more gave `Err:513`). A literal of 32768 or more still gives `Err:513`.
- BITAND, BITOR, BITXOR, BITLSHIFT and BITRSHIFT give `Err:502` when an operand is 2^48 or more. They used to hand back a number.
- COUNTBLANK counts a cell whose formula returns `""` as blank. It didn't before.
- DCOUNT and DCOUNTA without a field argument now count every matching record. Before, they could miss a record that had empty cells (the criterion `<>5` skipped a record with an empty Score), and an inline array as the database always counted 0. A row with nothing in the database columns isn't a record.

## File formats
- ODF: IFNA is saved as `of:=IFNA(...)`, and IFERROR as `of:=IFERROR(...)`.
- xls: IFNA and SKEWP are saved as `_xlfn.IFNA` and `_xlfn.SKEW.P` and read back fine. Before, they were lost on save and came back as `NA()`.
- xlsx: we read `IFNA`, `_xlfn.IFNA`, `SKEW.P` and `_xlfn.SKEW.P`, and also `_xlfn.COUNTIFS` and `_xlfn.BAHTTEXT`. All of those gave #NAME? before.
- xls: BITAND, BITOR, BITXOR, BITLSHIFT and BITRSHIFT are saved as `_xlfn.` names, and Excel calculates them. Excel used to show #NAME? for them.
- xls: IFERROR is saved as `_xlfn.IFERROR` and Excel calculates it; `_xlfn.IFERROR` written by other programs is read too.
- xlsx: we read `IFERROR`. It gave #NAME? before.
- xls: IFNA with a cell reference as its first argument now works in Excel: `IFNA(A2;99)` with A2 = `NA()` gives 99. Excel used to give #N/A.
- xlsx: we read `_xlfn.BITAND` and plain `BITAND`, and the same for the other four bit functions. They gave #NAME? before.

## Known limitations
- INDEX with one index on a one-row range returns that column, so `INDEX(H1:L1;3)` is the third cell. ODF 1.4 calls that an error. We're keeping the old behavior on purpose.
- IFNA only replaces `#N/A` element by element in an array formula. In an ordinary formula the range is cut down to the cell in the formula's own row, and you get #VALUE! if there isn't one. `SUMPRODUCT(IFNA(A1:A3;0))` in C1 returns the value of A1. Entered as array formulas, `SUM(IFNA(A1:A3;0))` and `SUMPRODUCT(IFNA(A1:A3;0))` both add up all three cells. IF behaves the same way: `SUMPRODUCT(IF(B1:B3>1;1;0))` outside an array formula gives #VALUE! in a cell below the data.
- A text argument typed straight into SKEW or SKEWP gives `Err:504`. ODF allows either 0 or an error here.
- SKEWP of symmetric data can return a tiny leftover like 3.7E-17 instead of 0.
- DCOUNT and DCOUNTA with field 0 count all the matching records instead of giving an error. An empty criteria cell is ignored, so `DCOUNT(R1:T6;"Age";W1:W2)` counts all 5 records.
- You can't save xlsx: the xlsx filters only import. That was already the case before these changes.

## What we haven't tested yet
- Files saved by Excel itself, and xlsb.
