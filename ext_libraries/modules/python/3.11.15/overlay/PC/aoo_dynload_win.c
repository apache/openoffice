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

/* Python/dynload_win.c with PY3_DLLNAME, which PCbuild/pyproject.props gives
   every project: the stable-ABI forwarder DLL that abi3 extensions link.  We
   ship no python3.dll, so such an extension fails to load with an ordinary
   ImportError; the name still has to be defined for the file to compile.
   See PC/aoo_getpath.c for why the string is spelled here. */

#define PY3_DLLNAME L"python3"

#include "../Python/dynload_win.c"
