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
 * PathCch* for a python311.dll that must load on Windows 7 SP1.  See
 * PC/pathcch.h for why this exists.
 *
 * Each function first asks KernelBase.dll for the real implementation.
 * KernelBase is mapped into every process from Windows 7 on, so this loads
 * nothing and searches no directory; from Windows 8 on it exports the PathCch
 * family and the system's own code runs.  Only when the export is missing --
 * i.e. on Windows 7 -- does the fallback below run.
 *
 * The fallbacks are written from the documented contract of each function,
 * and cover what Python 3.11 asks of them:
 *   PathCchSkipRoot      os._path_splitroot (posixmodule.c), _Py_isabs
 *                        (fileutils.c)
 *   PathCchFindExtension the ".exe" test in getpath.c's isxfile
 *   PathCchCombineEx     _Py_join_relfile (fileutils.c), used by getpath
 * PathCchCombineEx falls back to shlwapi's PathCombineW and so is limited to
 * MAX_PATH there -- which is the limit Windows 7 itself imposes on the Win32
 * file API without a \\?\ prefix.
 */

#include "pathcch.h"

#include <shlwapi.h>
#include <string.h>
#include <wchar.h>

typedef HRESULT (WINAPI *skip_root_fn)(PCWSTR, PCWSTR *);
typedef HRESULT (WINAPI *find_extension_fn)(PCWSTR, size_t, PCWSTR *);
typedef HRESULT (WINAPI *combine_ex_fn)(PWSTR, size_t, PCWSTR, PCWSTR, ULONG);

/* Looks NAME up in KernelBase.dll once and remembers the answer in *SLOT:
   0 = not looked up yet, -1 = not exported, otherwise the address.  Two
   threads racing here store the same value, and a pointer-sized aligned store
   is atomic on every Windows target, so no lock is needed. */
static FARPROC
kernelbase_export(volatile LONG_PTR *slot, const char *name)
{
    LONG_PTR value = *slot;
    if (value == 0) {
        HMODULE kernelbase = GetModuleHandleW(L"kernelbase.dll");
        FARPROC proc = kernelbase ? GetProcAddress(kernelbase, name) : NULL;
        value = proc ? (LONG_PTR)proc : (LONG_PTR)-1;
        *slot = value;
    }
    return value == (LONG_PTR)-1 ? NULL : (FARPROC)value;
}

/* Advances past one path segment and the backslash ending it, if any. */
static PCWSTR
skip_segment(PCWSTR p)
{
    while (*p != L'\0' && *p != L'\\') {
        ++p;
    }
    if (*p == L'\\') {
        ++p;
    }
    return p;
}

static int
is_drive_letter(WCHAR c)
{
    return (c >= L'A' && c <= L'Z') || (c >= L'a' && c <= L'z');
}

/* "X:" or "X:\" -> the character after it. */
static PCWSTR
skip_drive(PCWSTR p)
{
    p += 2;
    if (*p == L'\\') {
        ++p;
    }
    return p;
}

/* Root forms, as documented for PathCchSkipRoot:
     C:\path        drive-absolute       root "C:\"
     C:path         drive-relative       root "C:"
     \path          rooted               root "\"
     \\server\share\path                 root "\\server\share\"
     \\?\C:\path                         root "\\?\C:\"
     \\?\UNC\server\share\path           root "\\?\UNC\server\share\"
     \\?\Volume{guid}\path               root "\\?\Volume{guid}\"
   Anything else -- a relative path -- has no root and is E_INVALIDARG. */
static HRESULT
skip_root_fallback(PCWSTR path, PCWSTR *root_end)
{
    PCWSTR p;

    if (path == NULL || root_end == NULL || path[0] == L'\0') {
        return E_INVALIDARG;
    }
    *root_end = NULL;

    if (path[0] == L'\\' && path[1] == L'\\') {
        if (path[2] == L'?') {
            if (path[3] != L'\\') {
                return E_INVALIDARG;
            }
            p = path + 4;
            if (_wcsnicmp(p, L"UNC\\", 4) == 0) {
                p = skip_segment(p + 4);        /* server */
                p = skip_segment(p);            /* share */
            }
            else if (is_drive_letter(p[0]) && p[1] == L':') {
                p = skip_drive(p);
            }
            else if (_wcsnicmp(p, L"Volume{", 7) == 0) {
                p = skip_segment(p);
            }
            else {
                return E_INVALIDARG;
            }
        }
        else {
            p = skip_segment(path + 2);         /* server */
            p = skip_segment(p);                /* share */
        }
    }
    else if (is_drive_letter(path[0]) && path[1] == L':') {
        p = skip_drive(path);
    }
    else if (path[0] == L'\\') {
        p = path + 1;
    }
    else {
        return E_INVALIDARG;
    }

    *root_end = p;
    return S_OK;
}

/* The extension is the last '.' in the final path segment; with none,
   *ext is the terminating NUL.  The string must end within cch characters. */
static HRESULT
find_extension_fallback(PCWSTR path, size_t cch, PCWSTR *ext)
{
    PCWSTR p;
    PCWSTR dot = NULL;

    if (ext == NULL) {
        return E_INVALIDARG;
    }
    *ext = NULL;
    if (path == NULL || cch == 0 || cch > PATHCCH_MAX_CCH) {
        return E_INVALIDARG;
    }
    for (p = path; p < path + cch && *p != L'\0'; ++p) {
        if (*p == L'.') {
            dot = p;
        }
        else if (*p == L'\\') {
            dot = NULL;
        }
    }
    if (p == path + cch) {
        return E_INVALIDARG;                    /* not terminated */
    }
    *ext = dot != NULL ? dot : p;
    return S_OK;
}

static HRESULT
combine_ex_fallback(PWSTR out, size_t cch, PCWSTR in, PCWSTR more,
                    ULONG flags)
{
    WCHAR combined[MAX_PATH];
    size_t len;

    (void)flags;        /* long paths do not exist on Windows 7 */
    if (out == NULL || cch == 0 || cch > PATHCCH_MAX_CCH) {
        return E_INVALIDARG;
    }
    out[0] = L'\0';
    if (PathCombineW(combined, in, more) == NULL) {
        return HRESULT_FROM_WIN32(ERROR_FILENAME_EXCED_RANGE);
    }
    len = wcslen(combined);
    if (len >= cch) {
        return HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
    }
    memcpy(out, combined, (len + 1) * sizeof(WCHAR));
    return S_OK;
}

static volatile LONG_PTR skip_root_slot;
static volatile LONG_PTR find_extension_slot;
static volatile LONG_PTR combine_ex_slot;

HRESULT WINAPI
PathCchSkipRoot(PCWSTR pszPath, PCWSTR *ppszRootEnd)
{
    skip_root_fn real = (skip_root_fn)kernelbase_export(
        &skip_root_slot, "PathCchSkipRoot");
    return real != NULL ? real(pszPath, ppszRootEnd)
                        : skip_root_fallback(pszPath, ppszRootEnd);
}

HRESULT WINAPI
PathCchFindExtension(PCWSTR pszPath, size_t cchPath, PCWSTR *ppszExt)
{
    find_extension_fn real = (find_extension_fn)kernelbase_export(
        &find_extension_slot, "PathCchFindExtension");
    return real != NULL ? real(pszPath, cchPath, ppszExt)
                        : find_extension_fallback(pszPath, cchPath, ppszExt);
}

HRESULT WINAPI
PathCchCombineEx(PWSTR pszPathOut, size_t cchPathOut, PCWSTR pszPathIn,
                 PCWSTR pszMore, ULONG dwFlags)
{
    combine_ex_fn real = (combine_ex_fn)kernelbase_export(
        &combine_ex_slot, "PathCchCombineEx");
    return real != NULL
        ? real(pszPathOut, cchPathOut, pszPathIn, pszMore, dwFlags)
        : combine_ex_fallback(pszPathOut, cchPathOut, pszPathIn, pszMore,
                              dwFlags);
}
