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
 * Programs/_bootstrap_python.c, told where the stdlib is.
 *
 * The bootstrap interpreter starts from PyConfig_InitIsolatedConfig, which
 * ignores every PYTHON* environment variable.  On Unix it finds its stdlib
 * anyway, because it runs inside the build tree (Modules/Setup.local).  Here
 * the executable sits in bazel-out, far from the extracted sources, so it
 * finds nothing and dies with "No module named 'encodings'".
 *
 * The wrapper replaces that one call: it keeps the isolated config and then
 * sets only what the build action decides --
 *   - home from PYTHONHOME (made absolute; the action passes a path relative
 *     to the execroot), so getpath takes the stdlib from <home>\Lib;
 *   - write_bytecode off, which PYTHONDONTWRITEBYTECODE would have done, so no
 *     __pycache__ lands in the source repository;
 *   - site_import off (as -S would): site.py builds the user-site path from
 *     sys.winver, which Python/sysmodule.c sets only for the DLL build
 *     (MS_COREDLL), and deepfreeze.py needs nothing site adds.
 * No other environment variable is read.
 */

#include "Python.h"
#include <windows.h>
#include <stdlib.h>

static void
aoo_init_bootstrap_config(PyConfig *config)
{
    const wchar_t *home;
    wchar_t abs_home[MAX_PATH];

    PyConfig_InitIsolatedConfig(config);
    config->write_bytecode = 0;
    config->site_import = 0;

    home = _wgetenv(L"PYTHONHOME");
    if (home != NULL && home[0] != L'\0') {
        if (_wfullpath(abs_home, home, MAX_PATH) != NULL) {
            home = abs_home;
        }
        /* On failure home stays NULL and getpath reports the missing stdlib. */
        (void)PyConfig_SetString(config, &config->home, home);
    }
}

#define PyConfig_InitIsolatedConfig aoo_init_bootstrap_config
#include "../Programs/_bootstrap_python.c"
