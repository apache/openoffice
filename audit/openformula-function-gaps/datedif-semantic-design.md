DATEDIF Semantic Design — ODF 1.4 §6.10.3
==========================================

Branch: openformula
HEAD: 0224e3dc6c (H.1/H.2 complete)

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
   does not explicitly define this case. Excel returns a negative value for
   forward-calculated formats ("d", "m", "y") computed as if the dates were
   swapped then negated. For component formats ("md", "ym", "yd"), Excel
   computes the component difference without sign consideration, which can
   produce unexpected results. Policy: For forward formats ("y", "m", "d"),
   swap dates, compute forward result, negate. For component formats ("md",
   "ym", "yd"), compute the absolute component difference (consistent with
   Excel's observed behavior). This is an Excel-compatibility choice, not an
   ODF normative requirement.

4. End-of-month dates: When computing complete months ("m"), a month is
   complete only when the end date's day is >= the start date's day within the
   same month context. If the end date's day is less than the start date's day
   (e.g., Jan 31 to Feb 28), the last partial month is not counted. Policy:
   Follow Excel's documented behavior — a month is complete when the end day
   is >= the start day, OR when the end date is the last day of its month and
   the start date was the last day of its month.

5. February 28/29 and leap years: Leap-day anniversaries are handled by
   comparing month/day pairs. Feb 29 to Feb 28 in a non-leap year counts as
   one complete year if the anniversary date (Feb 29) would fall on Feb 28 in
   the target year. Policy: For year computations, compare month/day pairs
   directly; Feb 29 matches Feb 28 in non-leap years for anniversary purposes.

6. Fractional time components: Date serials may contain fractional parts
   representing time of day. DATEDIF operates on calendar dates by discarding
   time fractions (floor to midnight). Time components do not affect the
   result. Policy: Floor both dates to integer serials before computation.

7. Null date: The document's configured null date is used via
   pFormatter->GetNullDate(). No assumption is made about the serial epoch.

8. Invalid format strings: Any format string other than "y", "m", "d", "md",
   "ym", "yd" (case-sensitive) produces an error result.

9. Missing arguments / invalid argument types: Fewer than 3 arguments, or
   non-date arguments, produce an error result.

10. Date serials outside supported range: Serials that produce invalid Date
    objects produce an error result.

Algorithm Specification
-----------------------

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
- Format argument is case-sensitive per ODF; "Y", "M", "D" etc. are invalid.

Error Handling
--------------
- Wrong argument count: error via MustHaveParamCount.
- Non-date arguments: error via GetDouble() validation + PushIllegalArgument.
- Invalid format string: PushIllegalArgument().
- Reverse dates (start > end): Not an error; handled per policy above.

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
