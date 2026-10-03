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

# Debugging the built office

- TRIAGE RULE, learned the hard way 2026-08-14: when runtime symptoms are
  INCOHERENT — a feature works then stops, some tables in one database are
  editable and others are not, a capability is missing that the driver plainly
  supports — suspect a STALE / MIXED staging tree BEFORE analysing source.  A
  whole session went into an embedded-database "bug" (greyed-out field editing,
  then a hard exit creating a database) that a plain full rebuild made vanish,
  and afterwards the driver also exposed sdbcx VIEWS, i.e. what was staged had
  been a partial build all along.  The cheap check is timestamps:
  `ls -la bazel-<cfg>-bin/main/staging/program/{soffice.exe,services.rdb,*.dll}`
  — a multi-HOUR spread across services.rdb and the DLLs is the tell (it was
  02:24 vs 09:11 here).  Bazel does preserve timestamps of unchanged outputs, so
  a spread is a signal and not proof; treat it as "rebuild before theorising",
  which costs one build against hours of chasing a phantom.
- `--config=timelog` turns on AOO's OWN tracing: sal/inc/rtl/logfile.hxx gates
  every RTL_LOGFILE_CONTEXT/_TRACE behind TIMELOG, so untraced they are all
  ((void)0), and desktop's Desktop::Main is densely instrumented with exactly
  those.  rtl_logfile itself is NOT gated (it reads the RTL_LOGFILE bootstrap
  variable at runtime), so only the call sites need enabling and that is a pure
  compiler define — no source change.  Run the result with
  `-env:RTL_LOGFILE=C:/temp/oo`, which writes `C:\temp\oo_<pid>.log` (logfile.cxx
  appends `_<pid>.log` unless the value ends in `.nopid`).  Without the config the
  same env var still yields the few RTL_LOGFILE_PRODUCT_* points, enough to see
  how far startup got.  The config carries its own --platform_suffix so a traced
  build does not invalidate the normal output tree.
- CRASH-vs-CLEAN-EXIT TEST, free with any RTL_LOGFILE run and far more reliable
  than reading a cdb stack: the final line `closing log file at <n>` is written
  by `LoggerGuard::~LoggerGuard()`, a STATIC DESTRUCTOR.  Static destructors run
  on normal CRT teardown and are SKIPPED by `_exit()` — which is exactly what
  desktop's FatalError() calls.  So a log ending in that line exited cleanly; a
  log that simply stops was killed.  A healthy x64 session reads:
  `enter Main()` (1 ms) → `enter Application::Execute()` (3.8 s) →
  `DesktopOpenClients_Impl()` → … → `closing log file`.
- capturing a GUI process's stderr works fine: `soffice.exe … 2> file`.  The
  handle is inherited whether or not a console is attached, so this needs no
  debugger — useful because UNO exceptions do not derive from std::exception and
  their message often reaches only stderr, flushed at process exit.
- HEADLESS WITH A FRESH PROFILE needs `-nofirststartwizard`, learned
  2026-10-03: `soffice -headless -env:UserInstallation=file:///C:/temp/x
  -accept=...` exits with code 0 within ~5 s (the acceptor loads, then the
  office quits) because the first-start wizard cannot run headless.  The
  default profile finished its first start long ago, so the same command
  without `-env:UserInstallation` stays up — which makes it look like whatever
  you just put into the test profile is to blame.  Exit 0 and a log ending in
  `closing log file` mean a deliberate shutdown, not a crash.
