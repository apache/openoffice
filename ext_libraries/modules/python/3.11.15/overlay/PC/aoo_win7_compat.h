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
 * Force-included (/FI) into every CPython translation unit.
 *
 * CPython 3.11 is compiled here at the Windows 7 API level (_WIN32_WINNT
 * 0x0601) instead of PC/pyconfig.h's Windows 8 one, so the SDK hides what it
 * gates on Windows 8.  Most of that would be a real dependency and must stay a
 * compile error.  What is defined here is only what CPython uses WITHOUT
 * depending on Windows 8 -- plain constants, never functions -- each with the
 * use that makes it safe.  The values are the SDK's.
 *
 * Found by matching every name the 10.0.19041.0 SDK declares under a Windows 8
 * or later guard against the sources this module compiles.
 */

#ifndef AOO_PYTHON_WIN7_COMPAT_H
#define AOO_PYTHON_WIN7_COMPAT_H

/* WinNls.h, WINVER >= _WIN32_WINNT_WIN8.  Modules/_winapi.c
   (_winapi.LCMapStringEx) only uses them to reject these flags with
   ValueError before calling LCMapStringEx, which itself exists from Vista. */
#ifndef LCMAP_SORTHANDLE
#define LCMAP_SORTHANDLE 0x20000000
#endif
#ifndef LCMAP_HASH
#define LCMAP_HASH       0x00040000
#endif

#endif /* AOO_PYTHON_WIN7_COMPAT_H */
