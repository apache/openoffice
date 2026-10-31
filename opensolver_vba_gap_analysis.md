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

# OpenSolver VBA Gap Analysis for Apache OpenOffice VBA Port

## Summary

This report documents all missing VBA API capabilities required to run
OpenSolver v2.9.2 in AOO's VBA-compatible BASIC interpreter on the
`symphony-vba` branch. Critical blockers (items #1-#4) will prevent OpenSolver
from compiling or running at all.

______________________________________________________________________

## Critical Blockers (Must Resolve Before Any OpenSolver Code Runs)

### 1. VBA `Interface` Keyword Not Supported

**Status:** BLOCKER\
**Location:** `basic/source/comp/parser.cxx:77`\
**Detail:** AOO BASIC supports `Implements` (token exists in `token.cxx:114`,
handler in `parser.cxx:707`) but the `Interface` keyword to *declare* an
interface is **completely absent** from the parser. OpenSolver defines 6
interface modules that rely on the VBA compiler recognizing them as interfaces:

- `ISolver.cls` (13 members: 5 properties, 1 sub)
- `ISolverFile.cls` (3 members: 1 property, 2 subs)
- `ISolverLocalExec.cls` (3 members: 1 function, 1 property)
- `ISolverLocalLib.cls` (4 members)
- `ISolverLinear.cls` (3 members)
- `ISolverNeos.cls` (1 member)

Without `Interface` keyword support, these `.cls` files are treated as regular
CLASS modules (with empty method bodies), not interfaces. While `Implements`
name-prefix wiring exists in codegen, the `TypeOf ... Is ISolverLocalLib` type
checks throughout `SolverCommon.bas` (lines 74, 212, 478) cannot perform proper
interface type discrimination.

### 2. Conditional Compilation (`#If`) Not Supported

**Status:** BLOCKER\
**Location:** `basic/source/comp/parser.cxx` (no match found)\
**Detail:** AOO BASIC compiler has **zero** support for
`#If`/`#ElseIf`/`#Else`/`#End If` conditional compilation directives. OpenSolver
uses these pervasively for platform-specific code paths (Mac/Win32/Win64), with
numbering blocks 1-7 in `SolverCommon.bas` alone. Files affected:

- `SolverCommon.bas`: `#If Mac And MAC_OFFICE_VERSION >= 15` (line 16)
- `SolverCommon.bas`: `#If Mac / # ElseIf Win64 / #Else / #End If` in `LibDir()`
  (lines 345-354)
- `OpenSolverIO.bas`: Platform-specific temp path logic
- `OpenSolverExternalCommand.bas`: Windows API declarations

### 3. `Scripting.Dictionary` Not Available

**Status:** BLOCKER\
**Location:** Not present in VBA object model\
**Detail:** `Scripting.Dictionary` is used as a core data structure throughout
OpenSolver. Both instantiation patterns are used:

- `New Dictionary` in `OpenSolverUtils.bas:298` (via late binding)
- Return type of `GetSolverParametersDict()` in `OpenSolverUtils.bas:292`
- Parameter type for `ParametersToKwargs()`, `ParametersToFlags()`,
  `ParametersToOptionsFileString()`, `ParametersToOptionsFile()` in
  `OpenSolverUtils.bas` (lines 342, 364, 385, 407)
- Used in `JsonConverter.bas:474` as `json_ParseObject` return type
- `.Item()` accessor used extensively in solver parameter handling

AOO has no built-in `Dictionary` type and `CreateObject("Scripting.Dictionary")`
will fail.

### 4. `Scripting.FileSystemObject` Not Available

**Status:** BLOCKER\
**Location:** I am implementing this (vbafilesystem.cxx/hxx)\
**Detail:** `CreateObject("Scripting.FileSystemObject")` is used in multiple
files:

- `CodeImporter.bas:38`
- `OpenSolverIO.bas:341`
- `OpenSolverUtils.bas:54` (FileOrDirExists function)

My `FileSystemObject` VBA implementation (332 lines in vbafilesystem.cxx) uses
`osl::File`/`osl::Directory` and covers `GetSpecialFolder`, `GetDrive`,
`CopyFile`, `DeleteFile`, `FileExists`, `FolderExists`, `GetFile`, `GetFolder`,
`CreateTextFile`, `GetTempName`, `BuildPath`, `GetFileName`,
`GetParentFolderName`, `GetAbsolutePathName`. However, `CreateObject` dispatch
for COM progids like "Scripting.FileSystemObject" needs to be wired in
`ScVbaApplication::CreateObject` or a VBA `CreateObject` function.

______________________________________________________________________

## Major Missing APIs (Will Cause Runtime Errors)

### 5. `Application.ScreenUpdating` Not Implemented

**Status:** MISSING\
**Location:** `oovbaapi/ooo/vba/excel/XApplication.idl` (no property)\
**Detail:** Used in `SolverCommon.bas:51` to suppress screen updates during
solving. Would need `[attribute] boolean ScreenUpdating` added to
XApplication.idl and implemented in `vbaapplication.hxx/.cxx`.

### 6. `Application.Iteration` Not Implemented

**Status:** MISSING\
**Location:** `oovbaapi/ooo/vba/excel/XApplication.idl` (no property)\
**Detail:** Used in `SolverCommon.bas:46` to get/set iteration settings. Would
need `[attribute] boolean Iteration` added to XApplication.idl and
implementation in `vbaapplication.hxx/.cxx`.

### 7. `Shell` Method Not Implemented

**Status:** MISSING\
**Location:** `oovbaapi/ooo/vba/excel/XApplication.idl` (no method)\
**Detail:** `OpenSolverExternalCommand.bas` uses `Shell()` to launch external
solver executables (`ExecCapture` at line 462, `Exec` at line 511). Also used by
`WScript.Shell`-style operations elsewhere. AOO BASIC has a global `Shell()`
function but the VBA `Application.Shell` method is different.

### 8. `ExecCapture` Function

**Status:** Custom VBA function (not VBA standard)\
**Location:** `OpenSolverExternalCommand.bas:462`\
**Detail:** OpenSolver's own utility that wraps Shell to capture stdout/stderr.
Requires file I/O redirection capabilities. On AOO/Linux this would need to map
to `osl_execute` or `system()` with popen-style pipes.

### 9. `MSXML2.ServerXMLHTTP`

**Status:** Not available\
**Location:** `OpenSolverUpdateCheck.bas:66`\
**Detail:** Used for update checking (HTTP requests). Would need a UNO HTTP
client wrapper. Lower priority since update checking is not core solver
functionality.

### 10. `Scriptlet.TypeLib` / `WScript.Shell`

**Status:** Not available\
**Location:** `OpenSolverUpdateCheck.bas:356`, elsewhere\
**Detail:** Used for GUID generation and shell operations. Non-critical for
solver functionality.

______________________________________________________________________

## Implemented (Already Completed)

### Completed

- `Range.Value2` — `XRange.idl`, `vbarange.hxx`, `CellValue2Getter` in
  `vbarange.cxx`
- `Range.Precedents` / `Range.Dependents` — Added to interface and
  implementation
- `Application.EnableCancelKey` — Added to XApplication.idl, implemented get/set
- `Application.CalculationState` — Added, implemented (returns `xlDone`)
- `Application.Calculation` — Already existed
- `Application.ScreenUpdating` — **IMPLEMENTED** — Added to XApplication.idl,
  implemented get/set with shared settings
- `Application.Iteration` — **IMPLEMENTED** — Added to XApplication.idl,
  implemented get/set with shared settings
- `FileSystemObject` VBA service — Implemented in `vbafilesystem.cxx` (332
  lines), registered in `service.cxx` and `sc/util/vbaobj.component`
- `CreateObject("Scripting.FileSystemObject")` — Wired in
  `ScVbaApplication::CreateObject` (`sb.cxx:440-451`)
- `CreateObject("Scripting.Dictionary")` — Wired in
  `ScVbaApplication::CreateObject` (`sb.cxx:452-464`)
- `Scripting.Dictionary` class implementation — `vbadictionary.cxx`/`.hxx` with
  `XDictionary.idl` interface
- Enum constants for `EnableCancelKey`, `CalculationState`, `Calculation` —
  Added to `vbaapplication.cxx`
- `#If` conditional compilation — Fully implemented in `parser.cxx`,
  `token.cxx`, `parser.hxx`
- VBA `Interface` keyword — Added `INTERFACE` token, keyword mapping, and parser
  handling (treats Interface modules as CLASS type like VBA)

______________________________________________________________________

## Recommended Implementation Priority

| Priority | Task                                                                      | Effort                        |
| -------- | ------------------------------------------------------------------------- | ----------------------------- |
| **P0**   | Add `ScreenUpdating` and `Iteration` to XApplication                      | ~20 lines IDL + 20 lines impl |
| **P0**   | Wire `CreateObject("Scripting.Dictionary")` to a VBA Dictionary service   | ~50 lines                     |
| **P0**   | Wire `CreateObject("Scripting.FileSystemObject")` to existing FSO service | ~10 lines                     |
| **P0**   | Port OpenSolver removing `#If` blocks (create Mac/Win64/Win32 variants)   | Medium                        |
| **P1**   | Implement `Shell()` in VBA Application                                    | ~30 lines                     |
| **P1**   | Implement `ExecCapture` equivalent                                        | ~40 lines                     |
| **P1**   | Add `Interface` keyword support to BASIC compiler                         | High (parser change)          |
| **P2**   | Wire `TypeOf` for interface discrimination                                | Medium                        |
| **P2**   | `MSXML2.ServerXMLHTTP` wrapper (for update checks)                        | Low                           |
| **P2**   | `WScript.Shell` wrapper (env vars, GUID)                                  | Low                           |

______________________________________________________________________

## Key Files in OpenSolver Execution Path

1. **Entry**: `OpenSolverAPI.bas:RunOpenSolver()` — top-level solve entry
1. **Model**: `COpenSolver.cls` — `BuildModelFromSolverData`, `SolveModel`,
   `LoadResultsToSheet`
1. **Coordination**: `SolverCommon.bas` — `SolveModel`, `CreateSolver`,
   `WriteModelFile`, `RunLocalSolver`
1. **LP Writing**: `SolverCBC.bas` or `ReaderFileLP.bas` — model file generation
1. **Execution**: `OpenSolverExternalCommand.bas` — `ExecCapture`, `Exec`,
   `ExecAsync`
1. **I/O**: `OpenSolverIO.bas` — temp files, paths, `FileSystemObject`
1. **Params**: `OpenSolverUtils.bas` — `GetSolverParametersDict`, `Dictionary`
   usage

## Interfaces Used in SolverCommon.bas (require Interface support)

| Interface          | TypeOf Check Line | Members Required                |
| ------------------ | ----------------- | ------------------------------- |
| `ISolverLocalLib`  | 74, 212           | `.Solve`, `.LibBinary`          |
| `ISolverFile`      | 78, 39            | `.ReadResults`, `.FileType`     |
| `ISolverLinear`    | 33, 89, 116, 133  | `.SensitivityAnalysisAvailable` |
| `ISolverLocalExec` | 478               | `.CreateSolveCommand`           |
| `ISolverNeos`      | 483, 517          | NEOS server communication       |
