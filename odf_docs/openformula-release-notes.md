<!--
 Licensed to the Apache Software Foundation (ASF) under one
 or more contributor license agreements.  See the NOTICE file
 distributed with this work for additional information
 regarding copyright ownership.  The ASF licenses this file
 to you under the Apache License, Version 2.0 (the
 "License"); you may not use this file except in compliance
 with the License.  You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing,
 software distributed under the License is distributed on an
 "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 KIND, either express or implied.  See the License for the
 specific language governing permissions and limitations
 under the License.
-->

# OpenFormula release notes (draft)

As-of 2026-11-18.

## What's new

- **IFNA(Value; Value_if_na)** gives you Value, unless it's `#N/A`, in which case you get `Value_if_na`. Other errors pass straight through, and Value_if_na only gets evaluated when it's needed. In an array formula, each `#N/A` in a range or array is replaced on its own.
- **SKEWP** is the population version of SKEW. You'll get `Err:502` if there's only one value, or if all the values are the same.
- **IFERROR(Value; Alternative)** gives you Value, unless it's an error value of any kind, in which case you get Alternative. Like IFNA, Alternative only gets evaluated when it's needed, an empty cell counts as 0, and in an array formula each error is replaced on its own.
- **DATEDIF(StartDate; EndDate; Format)** works like Excel's: "y", "m", "d", "ym", "md" and "yd", in upper or lower case. A start date after the end date gives `Err:502` (Excel: #NUM!). "md" borrows the days of the month before the end month the way Excel does, so it can be negative: January 31 to March 1, 2024 gives -1.
- **ISOWEEKNUM(Date)** gives the ISO 8601 week number.
- **NUMBERVALUE(Text; DecimalSeparator; GroupSeparator)** turns text into a number following the ODF rules. The separators are optional and default to the locale's.
- **FINDB**, **SEARCHB** and **REPLACEB** are FIND, SEARCH and REPLACE counted in bytes, like LENB, LEFTB, RIGHTB and MIDB. A CJK character counts 2 bytes.
- **F.DIST** and **F.INV** are the left-tailed F distribution and its inverse; F.DIST also gives the density. FDIST and FINV haven't changed and still give the right tail.
- **ERROR.TYPE** gives 1 to 7 for #NULL!, #DIV/0!, #VALUE!, #REF!, #NAME?, #NUM! and #N/A. ERRORTYPE still gives the internal error number (532 for a division by zero).
- **HYPGEOMDIST** takes an optional fifth argument, Cumulative. Without it you get the probability of exactly X results, as before.
- **ODDFPRICE** and **ODDFYIELD** work now; they used to give an error every time. For short and long odd first periods they give Excel's results.
- There are help topics now for DATEDIF, ISOWEEKNUM, NUMBERVALUE, FINDB, SEARCHB, REPLACEB, ERROR.TYPE, F.DIST and F.INV, and the help for HYPGEOMDIST covers Cumulative.
- There are help topics now for IFERROR, IFNA, SKEWP, BITAND, BITOR, BITXOR, BITLSHIFT and BITRSHIFT, linked from the function lists.

## What changed

- Formulas can hold 2048 tokens now, up from 512. A 1026-character `=1+1+...` formula works (it gave Err:512 before). A function takes up to 255 arguments; with 256 or more you get `Err:512`. This was checked with `SUM`.
- If you type a formula that's too long, it's refused: you see "`Error: Formula overflow`" and the cell keeps what it had. If a file contains one, it keeps its text and shows `Err:512`.
- A string literal in a formula can be 32767 characters long. It used to stop at 1021 (1022 or more gave `Err:513`). A literal of 32768 or more still gives `Err:513`.
- BITAND, BITOR, BITXOR, BITLSHIFT and BITRSHIFT give `Err:502` when an operand is 2^48 or more. They used to hand back a number.
- COUNTBLANK counts a cell whose formula returns `""` as blank. It didn't before.
- DCOUNT and DCOUNTA without a field argument now count every matching record. Before, they could miss a record that had empty cells (the criterion `<>5` skipped a record with an empty Score), and an inline array as the database always counted 0. A row with nothing in the database columns isn't a record.

- HYPGEOMDIST gave wrong results for some large arguments: `HYPGEOMDIST(900;1828;2033;4167)` gave 3.2E+181 instead of 0.0219.

## File formats

- ODF: IFNA is saved as `of:=IFNA(...)`, and IFERROR as `of:=IFERROR(...)`.
- xls: IFNA and SKEWP are saved as `_xlfn.IFNA` and `_xlfn.SKEW.P` and read back fine. Before, they were lost on save and came back as `NA()`.
- xlsx: we read `IFNA`, `_xlfn.IFNA`, `SKEW.P` and `_xlfn.SKEW.P`, and also `_xlfn.COUNTIFS` and `_xlfn.BAHTTEXT`. All of those gave #NAME? before.
- xls: BITAND, BITOR, BITXOR, BITLSHIFT and BITRSHIFT are saved as `_xlfn.` names, and Excel calculates them. Excel used to show #NAME? for them.
- xls: IFERROR is saved as `_xlfn.IFERROR` and Excel calculates it; `_xlfn.IFERROR` written by other programs is read too.
- xlsx: we read `IFERROR`. It gave #NAME? before.
- xls: IFNA with a cell reference as its first argument now works in Excel: `IFNA(A2;99)` with A2 = `NA()` gives 99. Excel used to give #N/A.
- ODF: F.DIST and F.INV are saved as `of:FDIST` and `of:FINV`, and FDIST and FINV stay `of:LEGACY.FDIST` and `of:LEGACY.FINV`. ERROR.TYPE is `of:ERROR.TYPE`, and ERRORTYPE stays `of:ORG.OPENOFFICE.ERRORTYPE`.
- ODF: ISOWEEKNUM with one argument is read as ISOWEEKNUM. With two arguments it's still read as WEEKNUM, which is how files from older versions store WEEKNUM.
- ODF: RRI is saved as `of:RRI`, and as `RRI` in ODF 1.0/1.1 files too. Files that have `ZGZ` still load. Older versions don't know `RRI`.
- xls: DATEDIF is read and saved. FINDB, SEARCHB and REPLACEB are saved as Excel's own functions, ISOWEEKNUM as `_xlfn.ISOWEEKNUM`, and F.DIST and F.INV as `_xlfn.F.DIST` and `_xlfn.F.INV`; Excel calculates them. `_xlfnodf.FDIST` and `_xlfnodf.FINV`, which older versions wrote for FDIST and FINV, are still read as FDIST and FINV.
- xls and xlsx: ERROR.TYPE in an Excel file is read as ERROR.TYPE. It used to be read as ERRORTYPE and gave the internal error number. ERRORTYPE is still saved as ERROR.TYPE, so in an xls it comes back as ERROR.TYPE.
- xlsx: we read DATEDIF, FINDB, SEARCHB and REPLACEB.
- xlsx: we read `_xlfn.BITAND` and plain `BITAND`, and the same for the other four bit functions. They gave #NAME? before.
- xlsx: we read `_xlfn.F.DIST`, `_xlfn.F.INV`, `_xlfn.NUMBERVALUE` and `_xlfn.RRI`. They gave #NAME? before.

## Known limitations

- INDEX with one index on a one-row range returns that column, so `INDEX(H1:L1;3)` is the third cell. ODF 1.4 calls that an error. We're keeping the old behavior on purpose.
- IFNA only replaces `#N/A` element by element in an array formula. In an ordinary formula the range is cut down to the cell in the formula's own row, and you get #VALUE! if there isn't one. `SUMPRODUCT(IFNA(A1:A3;0))` in C1 returns the value of A1. Entered as array formulas, `SUM(IFNA(A1:A3;0))` and `SUMPRODUCT(IFNA(A1:A3;0))` both add up all three cells. IF behaves the same way: `SUMPRODUCT(IF(B1:B3>1;1;0))` outside an array formula gives #VALUE! in a cell below the data.
- A text argument typed straight into SKEW or SKEWP gives `Err:504`. ODF allows either 0 or an error here.
- SKEWP of symmetric data can return a tiny leftover like 3.7E-17 instead of 0.
- DCOUNT and DCOUNTA with field 0 count all the matching records instead of giving an error. An empty criteria cell is ignored, so `DCOUNT(R1:T6;"Age";W1:W2)` counts all 5 records.
- xls: HYPGEOMDIST with five arguments is saved as `NA()`; the xls function only takes four. NUMBERVALUE and RRI are saved under ODF names that we read back but Excel shows as #NAME?. F.DIST with three arguments is saved as `_xlfn.F.DIST`, which Excel shows as #N/A because it wants four.
- xlsx: `_xlfn.ISOWEEKNUM` gives #NAME?.
- ERROR.TYPE gives an `Err:` error the number Excel gets when the file is saved as xls, so `ERROR.TYPE(SQRT(-1))` is 3 (`Err:502` becomes #VALUE!). Excel gives 6.
- NUMBERVALUE of empty text is an error, as ODF says; Excel gives 0. An empty GroupSeparator is fine here and an error in Excel.
- DATEDIF "yd" from February 29, 2020 to March 1, 2023 gives 1; Excel gives 0.
- ODDFPRICE and ODDFYIELD were checked against Excel on 4524 random cases and differ in 14. In 4 of them DFC equals E exactly and Excel gives #NUM!. In 5, annual bonds on 30/360 US with coupons at the end of February, the price is off by 2/360 of a coupon. The other 5 are Excel's ODDFYIELD: four times it stops short of the exact yield, once it finds a wrong one.
- FINDB, SEARCHB and REPLACEB count a CJK character as 2 bytes whatever the language settings are. Excel counts 2 only when a double-byte language is set up.
- You can't save xlsx: the xlsx filters only import. That was already the case before these changes.

## What we haven't tested yet

- Files saved by Excel itself, and xlsb.
