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

# vba-test Compatibility Analysis for AOO VBA

## Overview

Analysis of <https://github.com/VBA-tools/vba-test> against AOO's VBA
compatibility mode (Option VBASupport 1).

## Summary

**vba-test is incompatible with AOO's VBA engine.** The framework relies on
class modules with events, which AOO does not support. Specifically,
`Public Event` and `RaiseEvent` are unrecognized keywords in AOO's BASIC
compiler.

## Unsupported Features (AOO lacks)

### 1. Class Events (`Public Event` + `RaiseEvent`)

- **Status: NOT SUPPORTED**
- `Event` is not a keyword in AOO's tokenizer
  (`main/basic/source/inc/token.hxx`)
- `RaiseEvent` is not recognized anywhere in AOO's BASIC source
- vba-test uses 4 event declarations in `TestSuite.cls` and 4 `RaiseEvent` calls
- Without events, the entire test framework's event-driven architecture
  collapses

### 2. `VBA.` Namespace Prefix

- **Status: NOT SUPPORTED**
- `VBA.IsArray`, `VBA.VarType`, `VBA.Collection`, `VBA.String$`, etc. not
  resolvable
- Count: 46 occurrences across `TestSuite.cls` (8), `TestCase.cls` (38),
  `FileReporter.cls` (1)

### 3. `Scripting.Dictionary`

- **Status: NOT SUPPORTED**
- No Dictionary implementation in AOO's VBA runtime
- `TestCase.Context As Dictionary` and `New Dictionary` calls fail

### 4. `Debug.Print`

- **Status: NOT SUPPORTED**
- No `Debug` object in AOO's BASIC runtime
- Used in `ImmediateReporter.cls` (4 calls) for test output

### 5. `vbObjectError`

- **Status: NOT SUPPORTED**
- Constant not defined in AOO's VBA runtime
- Used 3 times in `TestCase.cls` for custom error raising

### 6. `Scripting.Dictionary`

- **Status: NOT IMPLEMENTED**
- No `XDictionary.idl` or `vbadictionary.cxx` exists in AOO source
- Used 13 times in `TestCase.cls` (`Context As Dictionary`, `New Dictionary`)
- A leftover `vbadictionary.o` object file exists in build output
  (`main/solver/510/unxlngx6.pro/workdir/CxxObject/sc/source/ui/vba/vbadictionary.o`)
  but no source
- `VbaDictionaryTest.java` (existing test, commits `1bcfef4d35`) references a
  non-existent `vbadictionary.cxx` and `XDictionary.idl`
- `CreateObject("Scripting.Dictionary")` will return Nothing at runtime

## Supported Features (AOO provides)

| Feature                              | vba-test Usage                   | Status                                                   |
| ------------------------------------ | -------------------------------- | -------------------------------------------------------- |
| `Option VBASupport 1`                | All modules                      | Supported                                                |
| `IIf`                                | Multiple                         | Supported (`stdobj.cxx:310`)                             |
| `vbNewLine`                          | Output formatting                | Supported (`parser.cxx:848`)                             |
| `WithEvents` keyword                 | ImmediateReporter, FileReporter  | Token recognized (`token.cxx:185`) but events won't fire |
| `Class_Initialize`/`Class_Terminate` | TestCase, TestSuite              | Supported                                                |
| `Open`/`Print #` file I/O            | FileReporter                     | Supported (`token.cxx:138`)                              |
| `On Error Resume Next`               | TestCase assertions              | Supported                                                |
| Collections                          | `VBA.Collection` (won't resolve) | Partial                                                  |
| `IsEqual`/`IsArray`                  | TestCase                         | Supported, but `VBA.` prefix needed                      |
| `Format$`                            | TestCase                         | Check                                                    |

## Test Files That Would Fail

| File                        | Critical Failures                                             |
| --------------------------- | ------------------------------------------------------------- |
| `src/TestSuite.cls`         | `Public Event` (4x), `RaiseEvent` (4x), `VBA.Collection` (8x) |
| `src/TestCase.cls`          | `VBA.` prefix (38x), `Dictionary` (13x), `vbObjectError` (3x) |
| `src/ImmediateReporter.cls` | `Debug.Print` (4x), `WithEvents` (useless without events)     |
| `src/FileReporter.cls`      | `VBA.IIf` (1x), `WithEvents` (useless)                        |
| `tests/Tests_TestCase.bas`  | `Dictionary` (4x), `vbaObjectError` usage                     |

## Conclusion

vba-test cannot run on AOO without significant modifications. The fundamental
architectural dependency on VBA class events makes it Excel-specific.
