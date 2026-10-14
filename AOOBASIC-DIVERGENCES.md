# AOO Basic: observed divergences from VBA

Observed behaviour of AOO Basic in non-VBA mode (no `Option VBASupport`), each row checked by running the named case in a headless office built from this tree. The cases live in the (not yet committed) Basic test suite under `main/basic/test/`.

| Behaviour | AOO result | VBA result | Case |
|---|---|---|---|
| `Err` after `On Error Resume Next` and a failing statement | 0 (cleared immediately) | error number stays until cleared | `TResumeNextClearsErr` |
| `Err.Raise` / `Err.Number` | runtime error 91 (`Err` is a plain integer function) | works | `TErrRaiseNonVba` |
| `For i = 1 To n` where the body changes `n` | bound re-read every iteration (1 pass) | evaluated once (3 passes) | `TForBoundReeval` |
| `Not 1 = 2` | `False` (`Not` binds tighter than `=`) | `True` (comparison binds tighter) | `TNotPrecedence` |
| `7.5 \ 2` | 3 (operand truncated) | 4 (operand rounded) | `TIntDivFloat` |
| `Dim a As Long: a = 2.5` / `3.5` | 3 / 4 (half up) | 2 / 4 (banker's rounding) | `TRoundHalfToLong` |

## Causes found in the source

- **`Err` cleared:** `SbiRuntime::StepRESUME` sets `pInst->nErr = 0` (`runtime/step1.cxx:385`) and the `Resume Next` path calls it before the next statement runs (`runtime/runtime.cxx:791`).
- **`Err.Raise`:** the `Err` object exists only under VBA mode (`runtime/props.cxx:50-53`); otherwise `Err` returns the integer code (`props.cxx:63`).
- The other four rows are behaviour only; I did not trace them to code.

## Checked and matching VBA

Integer `\` and `Mod` on negatives, non-short-circuit `And`, `For ... Step`, `Select Case` (`To`, `Is`, case-sensitive strings), `GoSub`, `ByRef`/`ByVal`, `Static`, `True = -1`, array bounds (default 0, explicit `1 To 3`), `Collection.Item` index base (1-based, index 0 is error 5), `VarType` of Empty/Null, `IsMissing`, runtime error numbers for divide by zero (11) and overflow (6) under `On Error Goto`.

## Not covered

- VBA mode (`Option VBASupport 1`): the table is non-VBA only; `Err` and error handling are known to differ there.
- String comparison, `Like`, `UCase`, number-to-string and string-to-number coercion: not yet tested in a real office, so none are claimed.
- `Error 1234` as a statement did not raise under `On Error Goto`; I did not investigate.
- `For Each`, class modules, `Declare`, file I/O, dates.
