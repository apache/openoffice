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

# VBA-Dictionary Compatibility Analysis for AOO

## Overview

Analysis of <https://github.com/VBA-tools/VBA-Dictionary> (pure VBA
Scripting.Dictionary replacement) against AOO's VBA compatibility mode.

## Repository Details

- URL: <https://github.com/VBA-tools/VBA-Dictionary>
- License: MIT
- Main artifact: `Dictionary.cls` (456 lines, single class module)
- Purpose: Drop-in replacement for Scripting.Dictionary, originally for Mac
  (where native Dictionary is unreliable)

## Key Finding

**VBA-Dictionary requires modifications to work in AOO.** While it's a pure VBA
implementation (no native dependencies), it uses several features AOO doesn't
support.

## Unsupported Features

### 1. `VBA.` Namespace Prefix (~46 occurrences)

- `VBA.IsObject`, `VBA.vbBoolean`, `VBA.vbString`, `VBA.vbBinaryCompare`, etc.
- AOO's VBA mode does NOT resolve the `VBA.` prefix
- **Workaround:** Strip all `VBA.` prefixes (use direct function calls)

### 2. `Attribute` Lines (8 occurrences)

```vb
Attribute VB_Name = "Dictionary"
Attribute CompareMode.VB_Description = "..."
Attribute Item.VB_UserMemId = 0
Attribute Add.VB_Description = "..."
```

- These are Excel VBA class module attributes
- May cause parsing issues in AOO's BASIC compiler

### 3. `#If` with Compound Expressions

```vb
#Const UseScriptingDictionaryIfAvailable = True
#If Mac Or Not UseScriptingDictionaryIfAvailable Then
```

- Current AOO `#If` implementation (`main/basic/source/comp/parser.cxx:PPIf()`)
  only handles simple expressions
- Does NOT support `Or`/`And` operators in conditions
- After reading `Mac` (unknown constant → false), parser expects `Then` but
  finds `Or`
- **Effect:** `#Else` branch would be taken (native Dictionary), not the custom
  implementation
- **Workaround:** Simplify condition to
  `#If Not UseScriptingDictionaryIfAvailable Then`

### 4. Missing VBA Constants

- `vbBinaryCompare` (=0), `vbTextCompare` (=1), `vbDatabaseCompare` (=2) - NOT
  defined
- `vbBoolean` (=2), `vbString` (=8), `vbVariant` (=12) - NOT defined
- Used in `CompareMethod` enum and `dict_GetFormattedKey`
- **Workaround:** Define these constants with `#Const`

## Supported Features

| Feature                                 | Status    | Notes                                      |
| --------------------------------------- | --------- | ------------------------------------------ |
| Class modules with Property Get/Let/Set | Supported | `Class_Module` type set by VBA compat mode |
| `New Collection`                        | Supported | AOO's Collection supports this             |
| `Collection.Add ... Before:`            | Supported | `sb.cxx:2116` shows Before param support   |
| `Class_Initialize`/`Class_Terminate`    | Supported | `sb.cxx:806/824`                           |
| `On Error Resume Next`                  | Supported | Standard VBA error handling                |
| Dynamic arrays (`Dim arr() As Variant`) | Supported |                                            |
| `ReDim`/`Erase`                         | Supported |                                            |
| `vbNullString`, `vbCr`, `vbCrLf`        | Supported | Defined in `parser.cxx:843-853`            |

## Required Modifications for AOO

1. Remove all `VBA.` prefixes (e.g., `VBA.IsObject` → `IsObject`)
1. Remove or comment out `Attribute` lines
1. Simplify `#If` condition to a single constant test
1. Define missing constants: `vbBinaryCompare`, `vbTextCompare`,
   `vbDatabaseCompare`, etc.
1. Set `UseScriptingDictionaryIfAvailable = False` to force custom
   implementation

## Conclusion

With the modifications above, VBA-Dictionary could provide Dictionary
functionality in AOO. The core implementation is sound - it uses Collection and
arrays which AOO supports. The main blockers are the `VBA.` namespace, missing
constants, and `#If` expression limitations.
