/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

/*
 * Stand-in for the Windows SDK's <pathcch.h>, for a Python that must load on
 * Windows 7 SP1.
 *
 * The SDK header declares the PathCch* functions as imports from
 * api-ms-win-core-path-l1-1-0.dll, an API set that exists from Windows 8 on.
 * A python311.dll importing it statically cannot be loaded on Windows 7 at
 * all.  This file lives in PC/, which is on the include path ahead of the SDK,
 * so `#include <pathcch.h>` in getpath.c, posixmodule.c, fileutils.c and
 * pathconfig.c resolves here instead and the SDK header is never seen.
 *
 * Only what those four files use is declared.  The definitions in
 * PC/pathcch_win7.c call the real functions from KernelBase.dll when it
 * exports them (Windows 8 and later), so behaviour there is the system's own;
 * the fallbacks only ever run on Windows 7.
 */

#ifndef AOO_PYTHON_PATHCCH_H
#define AOO_PYTHON_PATHCCH_H

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PATHCCH_NONE             0x00000000
#define PATHCCH_ALLOW_LONG_PATHS 0x00000001

/* Largest buffer, in characters, the PathCch functions accept. */
#define PATHCCH_MAX_CCH          0x8000

HRESULT WINAPI PathCchSkipRoot(PCWSTR pszPath, PCWSTR *ppszRootEnd);
HRESULT WINAPI PathCchFindExtension(PCWSTR pszPath, size_t cchPath,
                                    PCWSTR *ppszExt);
HRESULT WINAPI PathCchCombineEx(PWSTR pszPathOut, size_t cchPathOut,
                                PCWSTR pszPathIn, PCWSTR pszMore,
                                ULONG dwFlags);

#ifdef __cplusplus
}
#endif

#endif /* AOO_PYTHON_PATHCCH_H */
