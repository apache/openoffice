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
 * PyWin_DLLhModule for _bootstrap_python, which links the core statically.
 * (_freeze_module gets it from PC/config_minimal.c, which _bootstrap_python
 * cannot use: that file is the minimal builtin-module table.)
 *
 * Modules/getpath.c and Python/dynload_win.c reference it on every Windows
 * build, but PC/dl_nt.c defines it only under Py_ENABLE_SHARED, where DllMain
 * sets it to python3XX.dll's handle.  A static core has no such DLL.  NULL is
 * the value both users already handle: getpath.c then takes the executable's
 * path as the library's, and dynload_win.c skips looking for a python3.dll
 * beside it.
 */

#include "Python.h"
#include "windows.h"

#ifndef Py_ENABLE_SHARED
HMODULE PyWin_DLLhModule = NULL;
#endif
