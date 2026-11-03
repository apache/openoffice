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

/* Modules/getpath.c with the per-file defines PCbuild/pythoncore.vcxproj
   gives it (release configuration).  They are string literals, which are
   spelled here once instead of being escaped through Bazel, a params file and
   cl.exe's command line. */

#define PREFIX      NULL
#define EXEC_PREFIX NULL
#define VERSION     NULL
#define VPATH       "..\\.."
#define PYDEBUGEXT  ""
#define PLATLIBDIR  "DLLs"
#define MS_DLL_ID   "3.11"

#include "../Modules/getpath.c"
