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

DATEDIF Semantic Design — ODF 1.4 §6.10.3
==========================================

Branch: openformula
HEAD: 0224e3dc6c (H.1/H.2 complete)

Updated 2026-11-18 to match the code after the post-merge review. Jim chose
Excel-compatible behavior for the cases ODF leaves open; decisions 3, 4 and 8
and the "md"/"yd" algorithms below were changed to what the code does now.
The section "2026-11-18: as implemented" at the end is the current reference.

This document specifies the semantics for the DATEDIF function implementation
in Apache OpenOffice Calc, conforming to ODF 1.4 Part 4, Section 6.10.3.

Normative Reference
-------------------

ODF 1.4 OpenFormula, Section 6.10.3 — DATEDIF: Difference between two dates.

Function Signature
------------------

DATEDIF( StartDate ; EndDate ; Format )

- StartDate: Date — the start of the interval.
- EndDate: Date — the end of the interval.
- Format: Text — one of "y", "m", "d", "md", "ym", "yd".

Result: Number — the difference between the two dates in the units specified
by Format.

ODF 1.4 §6.10.3 Defined Formats
-------------------------------

| Format | Meaning                    |
|--------|----------------------------|
| y      | Number of complete years   |
| m      | Number of complete months  |
| d      | Number of days             |
| md     | Days ignoring months/years |
| ym     | Months ignoring years      |
| yd     | Days ignoring years        |

Semantic Decisions
------------------

1. Start date earlier than end date: Normal forward interval. Compute the
   difference normally for all formats.

2. Start date equal to end date: Zero-length interval. All formats return 0.

3. Start date later than end date (reverse interval): The ODF specification
   does not define this case. Excel 16 returns #NUM! for every format
   (checked). Policy (2026-11-18): an error (Err:502), as Excel. The earlier
   policy of negating "y", "m", "d" and returning absolute components was
   dropped.

4. End-of-month dates: When computing complete months ("m"), a month is
   complete only when the end date's day is >= the start date's day within the
   same month context. If the end date's day is less than the start date's day
   (e.g., Jan 31 to Feb 28), the last partial month is not counted. Policy
   (2026-11-18): a month is complete only when the end day is >= the start
   day. ODF 1.2 and 1.4 say "If there is not a complete month between the
   dates, 0 will be returned", and Excel gives 0 for Jan 31 to Feb 29, 2024.
   The end-of-month substitution was dropped.

5. February 28/29 and leap years: Leap-day anniversaries are handled by
   comparing month/day pairs. Policy: compare month/day pairs directly, so
   Feb 29, 2024 to Feb 28, 2025 is 0 complete years (Excel also gives 0).

6. Fractional time components: Date serials may contain fractional parts
   representing time of day. DATEDIF operates on calendar dates by discarding
   time fractions (floor to midnight). Time components do not affect the
   result. Policy: Floor both dates to integer serials before computation.

7. Null date: The document's configured null date is used via
   pFormatter->GetNullDate(). No assumption is made about the serial epoch.

8. Invalid format strings: Any format string other than "y", "m", "d", "md",
   "ym", "yd" produces an error result. Upper and lower case are equivalent
   (2026-11-18), as in Excel; ODF does not say.

9. Missing arguments / invalid argument types: Fewer than 3 arguments, or
   non-date arguments, produce an error result.

10. Date serials outside supported range: Serials that produce invalid Date
    objects produce an error result.

Algorithm Specification
-----------------------

```text
Helper: date_from_serial(serial) -> Date
  null = pFormatter->GetNullDate()
  return null + floor(serial)

Helper: is_last_day_of_month(date) -> bool
  return date.GetDay() == date.GetDaysInMonth()

Format "d" — Calendar day difference:
  d1 = date_from_serial(StartDate)
  d2 = date_from_serial(EndDate)
  return (d2 - d1) in days (serial difference, floored)
  For reverse: compute forward, negate.

Format "m" — Complete calendar months:
  d1 = date_from_serial(StartDate)
  d2 = date_from_serial(EndDate)
  months = (d2.GetYear() - d1.GetYear()) * 12 + (d2.GetMonth() - d1.GetMonth())
  if d2.GetDay() < d1.GetDay():
    months -= 1
  elif d2.GetDay() >= d1.GetDay() and is_last_day_of_month(d1) and is_last_day_of_month(d2):
    # both end-of-month: count the full month
    pass
  # clamp to non-negative for forward intervals
  For reverse: compute forward, negate.

Format "y" — Complete calendar years:
  d1 = date_from_serial(StartDate)
  d2 = date_from_serial(EndDate)
  years = d2.GetYear() - d1.GetYear()
  if (d2.GetMonth(), d2.GetDay()) < (d1.GetMonth(), d1.GetDay()):
    years -= 1
  # Feb 29 anniversary in non-leap year: month/day (2,28) < (2,29) handled
  # naturally by tuple comparison above
  For reverse: compute forward, negate.

The algorithms below are the original design. Where they differ from the
code, the code (see "2026-11-18: as implemented") wins; "md" and "yd" and the
reverse-interval handling differ.

Format "md" — Days ignoring months/years:
  d1 = date_from_serial(StartDate)
  d2 = date_from_serial(EndDate)
  # Compare day-of-month, adjusting for month boundaries
  day1 = d1.GetDay()
  day2 = d2.GetDay()
  # If end day < start day, borrow from previous month
  if day2 < day1:
    # Compute the last day of the month before d2's month
    prev_month = d2.GetMonth() - 1
    prev_year = d2.GetYear()
    if prev_month < 1:
      prev_month = 12
      prev_year -= 1
    Date prev_last(prev_month's last day, prev_month, prev_year)
    day_diff = day2 + prev_last.GetDaysInMonth() - day1
  else:
    day_diff = day2 - day1
  return abs(day_diff) for reverse-date compatibility

Format "ym" — Months ignoring years:
  d1 = date_from_serial(StartDate)
  d2 = date_from_serial(EndDate)
  months = d2.GetMonth() - d1.GetMonth()
  # If end month < start month, this is a partial year
  # The month component ignoring years: if we've passed the start month
  # in the end year, count it; otherwise count from previous year
  if months < 0:
    months += 12
  return abs(months) for reverse-date compatibility

Format "yd" — Days ignoring years:
  d1 = date_from_serial(StartDate)
  d2 = date_from_serial(EndDate)
  # Compute day-of-year difference, wrapping at year boundary
  doy1 = d1.GetDayOfYear()
  doy2 = d2.GetDayOfYear()
  day_diff = doy2 - doy1
  return abs(day_diff) for reverse-date compatibility
```

Implementation Notes
--------------------

- Use ::rtl::math::approxFloor() for flooring serials (consistent with
  ScGetDiffDate360 in interpr2.cxx).
- Use pFormatter->GetNullDate() for the document's null date.
- Use Date class methods: GetDay(), GetMonth(), GetYear(), GetDaysInMonth(),
  IsLeapYear(), GetDayOfYear(), operator+=, comparison operators.
- Do not use fabs() to conceal negative results; handle sign explicitly.
- Do not approximate months as 30 days or years as 365 days; use calendar
  arithmetic.
- Validate exactly 3 arguments via MustHaveParamCount.
- (Superseded 2026-11-18: the format is case-insensitive.)

Error Handling
--------------

- Wrong argument count: error via MustHaveParamCount.
- Non-date arguments: error via GetDouble() validation + PushIllegalArgument.
- Invalid format string: PushIllegalArgument().
- Reverse dates (start > end): Err:502 (2026-11-18; Excel gives #NUM!).

Files to Modify
---------------

1. main/formula/inc/formula/compiler.hrc — Add SC_OPCODE_DATEDIF = 415
2. main/formula/inc/formula/opcode.hxx — Add ocDateDif = SC_OPCODE_DATEDIF
3. main/formula/source/core/resource/core_resource.src — Register "DATEDIF" name
4. main/sc/source/core/tool/interpr2.cxx — Add ScInterpreter::ScDateDif()
5. main/sc/source/core/tool/interpr4.cxx — Add dispatch case ocDateDif
6. main/sc/source/ui/src/scfuncs.src — Function Wizard entry
7. main/sc/inc/helpids.h — Add HID_FUNC_DATEDIF
8. main/sc/util/hidother.src — Register Help ID
9. test/odf-validate/calcval-cases14.tsv — Regression tests
10. audit/openformula-function-gaps/ — Update audit records

H.3.1 Implementation/Review Notes (2026-11-17)
----------------------------------------------

Implementation: `ScInterpreter::ScDateDif()` in
`main/sc/source/core/tool/interpr2.cxx`, declared in
`main/sc/source/core/inc/interpre.hxx`, dispatched from
`main/sc/source/core/tool/interpr4.cxx` (`ocDateDif`), with resources/help IDs
registered and BIFF5/7 import mapped import-only (`EXC_FUNCFLAG_IMPORTONLY`) in
`main/sc/source/filter/excel/xlformula.cxx` (ID 351 → `ocDateDif`).

Semantics as implemented (ODF 1.4 §6.10.3):

- Exactly 3 arguments: StartDate, EndDate, Format (popped Format, EndDate,
  StartDate).
- Date serials floored to calendar dates via `::rtl::math::approxFloor()`; time
  fractions discarded. Calendar conversion uses `pFormatter->GetNullDate()`.
- Invalid/unsupported date serials (`Date::IsValid() == false`) → illegal
  argument error.
- Format is case-sensitive: only `y`, `m`, `d`, `md`, `ym`, `yd` accepted;
  any other value (including `Y/M/D` and empty string) → illegal argument error.
- Forward intervals (`StartDate <= EndDate`): `d` = calendar day difference;
  `y` = complete calendar years; `m` = complete calendar months with
  end-of-month substitution when the start day is the last day of its month;
  `ym` = `m - y*12` (months remaining after complete years);
  `md` = day difference ignoring months/years, borrowing from the start date's
  month when end day < start day;
  `yd` = days from the anniversary (start date shifted forward by complete
  years, adjusted for Feb 29 in non-leap anniversary years) to end date.
- Reverse intervals (`StartDate > EndDate`): computed on the swapped forward
  interval. `y`, `m`, `d` return the negated forward result; `md`, `ym`, `yd`
  return a non-negative component value (Excel-compatible policy; ODF 1.4 does
  not define reverse intervals). This is an implementation constraint/policy,
  not a normative ODF requirement.

Validation notes:

- Source-level review/static verification only in this environment; Calc and
  calcval runtime tests cannot be executed because `libsot.so` / `sc`/`scaddins`
  are not built (GCC 15.2 vs AOO 510 ABI mismatch, no older GCC, no sudo).
- `SG.DATEDIF-010-md` expected value corrected to `26` (borrow from start
  month January: `10 + 31 - 15`) and DATEDIF regression coverage expanded in
  `test/odf-validate/calcval-cases14.tsv`.

The H.3.1 notes above describe the code as of 62e7f86e31. Of those, the
end-of-month substitution, the reverse-interval policy, the case-sensitive
format, the "md" borrow from the start month and the `26` expectation for
`SG.DATEDIF-010-md` no longer apply.

2026-11-18: as implemented
--------------------------

`ScInterpreter::ScDateDif()` in `main/sc/source/core/tool/interpr2.cxx`.
xls: BIFF 351 is read and written (`xlformula.cxx`, no import-only flag).
xlsx: read through the `DATEDIF` row of `oox/source/xls/formulabase.cxx`.

- Arguments popped as Format, EndDate, StartDate; both dates floored with
  `::rtl::math::approxFloor()`; an error in an argument is passed on.
- StartDate > EndDate: Err:502. Invalid `Date`: Err:502. Unknown format:
  Err:502. The format is lowered with `ToLowerAscii()` first.
- Complete months: `(y2-y1)*12 + (m2-m1)`, minus 1 when the end day is before
  the start day.
- "d": day difference. "m": complete months. "y": complete months / 12.
  "ym": complete months % 12.
- "md": `d2 - d1`; when negative, add the length of the month before the end
  month. This is Excel's rule and can be negative: Jan 31 to Mar 1, 2024 is -1
  (Excel 16 gives -1 too).
- "yd": the end date is moved into the start year, or into the next year when
  its month/day comes before the start's; Feb 29 moved into a common year
  becomes Mar 1. Result: that date minus StartDate. This is Excel's rule:
  May 3, 1995 to Apr 28, 2009 is 361 (Excel 361).

Checked against Excel 16 on 630 random and hand-picked cases: one difference,
"yd" from Feb 29, 2020 to Mar 1, 2023 (AOO 1, Excel 0). Tests: the `DATEDIF.*`
rows at the end of `test/odf-validate/calcval-cases14.tsv`, all matching on
the Mac and Linux builds.
