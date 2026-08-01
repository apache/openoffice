/**************************************************************
 *
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
 *
 *************************************************************/

// Bazel migration test support — NOT part of the AOO product source.
//
// Every real AOO binary enters through SAL_IMPLEMENT_MAIN (sal/main.h), whose
// generated main() calls sal_detail_initialize() first.  On Windows that is
// where WSAStartup() happens (sal/osl/w32/salinit.cxx) — sal3.dll's DllMain
// only ever calls the matching WSACleanup(), never WSAStartup().
//
// Several sal/qa GoogleTest suites (qa/osl/socket/*) declare a plain
//     int main(int argc, char ** argv) { InitGoogleTest(...); RUN_ALL_TESTS(); }
// which skips that boilerplate, so Winsock is never initialised and every osl
// socket call fails with WSANOTINITIALISED — SocketAddr constructors silently
// produce unusable addresses, getLocalHostname() fails, and the suite reports
// dozens of unrelated-looking assertion failures.  They date from the testshl2
// era, when the harness supplied main() (and the init) for them; testshl2 was
// retired, and nothing took over the job.
//
// Fixing the suites would mean editing product-adjacent source, which is out of
// scope for this migration.  Instead this TU is linked into those test exes and
// does the initialisation from a dynamic initialiser, which the CRT runs BEFORE
// main().  __argc/__argv are already populated at that point (the MSVC CRT sets
// them during startup, ahead of dynamic initialisers), so sal sees the real
// command line exactly as SAL_IMPLEMENT_MAIN would pass it — osl_setCommandArgs
// asserts argc > 0, so passing 0/NULL is not an option.
//
// Deliberately NOT calling sal_detail_deinitialize(): sal3.dll's DllMain already
// issues WSACleanup() on DLL_PROCESS_DETACH, and a second one would unbalance
// the Winsock refcount.

#include <stdlib.h>  // __argc / __argv

#include "sal/main.h"

namespace {

struct SalProcessInit {
    SalProcessInit() { sal_detail_initialize(__argc, __argv); }
};

// Dynamic initialiser: constructed before main(), like the CRT startup that
// SAL_IMPLEMENT_MAIN would otherwise front.
SalProcessInit theSalProcessInit;

}
