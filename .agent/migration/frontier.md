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

# Current frontier

Demo done 2026-06-20 → goal is now FULL migration (see [[project-full-migration-goal]]).
Buckets below labelled "Remaining" are in-scope work, not abandoned; only
"Out of scope" and "Dropped" are excluded.  Ordered by priority — testing of
already-migrated modules is the active front-line task.

── Active: test coverage for already-migrated modules ───────────────────
test          🔨  C++ unit-test infra runnable — NOW THE FRONT-LINE TASK: bring up
                   per-module qa/ tests across every ALREADY-MIGRATED module and keep
                   them green as the migration proceeds (regression net for the rest).
                   @gtest 1.7.0 bzlmod wrap (built /Zc:wchar_t- to match sal_Unicode);
                   build/rules/gtest_test.bzl (svidl_bundle analog: stages /MD exe +
                   DLLs + CRT + external manifest into one dir or R6034); libtest
                   (test.dll) builds.  GREEN: o3tl_test (5), tools_pathutils,
                   //main/sal:sal_tests (22) via sal_qa_test macro, salhelper_test,
                   comphelper_test_string + comphelper_test_weakbag, sax_test_converter,
                   cppu_qa_{any,unotype,reference,recursion} (private types.idl → headers
                   via idl_library reuse), cppuhelper_tests (ifcontainer/unourl/weak),
                   binaryurp_tests (cache/unmarshal).  See main/test/readme.md.
                   LANDMINE (cost a whole session): the C++/UNO bridge DLL
                   (msci_uno/mscx_uno) is a RUNTIME dep nothing links — cppu
                   osl_loadModule()s it from any Mapping (getCaughtException etc.).
                   Unstaged ⇒ either a null mapping AV or R6034 → 0xC0000142 with an
                   EMPTY test.log; and the CRT manifest must be EMBEDDED (RT_MANIFEST
                   id 1, now linked into every gtest_test exe) not just staged as an
                   external <exe>.manifest, or late DLL loads fall outside the
                   activation context.  The qa/ SWEEP IS DONE: no ALREADY-MIGRATED module
                   still has an unwired C++ qa/ suite (verified 2026-08-04 — every other
                   qa/ dir is Java, or Perl/shell tooling as in basegfx+slideshow; the
                   only C++ leftover is xmlsecurity/qa/certext, which needs fixture (b)).
                   DONE since: 8 sal osl/socket suites + shell_qa_zip + the 3 sal
                   child-process suites (osl_process 7/8, rtl_Process 3/3,
                   rtl_Bootstrap 25/30).
                   "Needs a CppUnit external dep" was largely a MYTH — NOTHING checked
                   so far actually uses cppunit.  osl/socket and shell/qa are plain
                   GoogleTest (now wired); writerfilter/qa/cppunittests is misnamed —
                   only doctok is GoogleTest, the other 4 (odiapi/qname/sl/xxml) use
                   the RETIRED testshl harness, as does rtl_strings, which is also
                   superseded by qa/rtl/{ostring,oustring} — treat all of those as dead
                   weight, not blockers, and do not migrate them.  Two reusable hooks came
                   out of it, both in gtest_test: //build/testsupport:sal_process_init
                   (suites with a bare main() skip SAL_IMPLEMENT_MAIN ⇒ no WSAStartup
                   ⇒ every osl socket call is WSANOTINITIALISED) and data_files (a
                   staged fixture is NOT enough — bazel test's CWD is the execroot, so
                   relative opens need the cd-first .bat launcher).  NEXT: what is
                   genuinely left is not standalone — the JAVA/UNO suites (see the
                   uno_junit_test block at the end of this bucket, DONE 2026-08-06,
                   3 green); svl/qa/test_URIHelper,
                   configmgr/qa/unit and cppuhelper/qa/propertysetmixin bootstrap a UNO
                   component context.  KEY DISTINCTION (was conflated under
                   "OfficeConnection", making the work look bigger than it is): those
                   are TWO fixtures.  (a) IN-PROCESS bootstrap —
                   defaultBootstrap_InitialComponentContext(), NO soffice process —
                   is DONE: gtest_test's uno_install=//main/staging:install exports
                   URE_BOOTSTRAP at the staged program/fundamental.ini, whose ${ORIGIN}
                   then supplies UNO_TYPES/UNO_SERVICES/URE_INTERNAL_LIB_DIR
                   transitively (no hand-set env vars, no drift).  First green:
                   //main/svl:svl_qa_test_URIHelper.  Caveat — it depends on the WHOLE
                   install, so it is slow and not a unit test; only use uno_install
                   where UNO is genuinely bootstrapped.  (b) test::OfficeConnection
                   (launch soffice -accept=…;urp, resolve over URP) is DONE 2026-08-05:
                   gtest_test office_connection=True (+ uno_install) makes a throwaway
                   user installation and exports arg-soffice/arg-user.  GREEN:
                   //main/test:test_qa_officeconnection (2/2, ~13s — real office
                   boot → URP resolve → remote Desktop → clean terminate).
                   NOTHING TO PORT from dmake: solenv's C++ APP1TEST rule (_tg_app.mk)
                   runs the bare exe with only --gtest_output and never sets these args
                   at all; the complete recipe is the JAVA one
                   (installationtest.mk::javatest), which is what was mirrored.  Two
                   forced divergences from it: (1) the args go in the ENVIRONMENT, not
                   as -env: command-line args — rtl::Bootstrap tries the command line
                   first, but that half reads osl_getCommandArgCount(), only populated
                   by SAL_IMPLEMENT_MAIN, and AOO's gtest suites all declare a bare
                   main() (same root cause as //build/testsupport:sal_process_init);
                   (2) arg-user is a NATIVE PATH, not Java's file:// URL, because the
                   C++ side feeds it to getFileURLFromSystemPath().
                   LANDMINE: a BACKSLASH IS AN ESCAPE CHARACTER in any rtl::Bootstrap
                   value (every value goes through macro expansion; read() in sal
                   bootstrap.cxx turns \X into X), so a raw Windows path loses EVERY
                   separator — "C:\Users\x" comes back "C:Usersx" and
                   getFileURLFromSystemPath fails with 21.  Double them.
                   LANDMINE (INVERTS the usual runtime_dlls advice): with uno_install,
                   co-locating a core UNO DLL BREAKS the bootstrap.  cppuhelper picks
                   the dir to load bootstrap.uno.dll from via get_this_libpath() =
                   getUrlFromAddress on ITSELF, and the exe's own dir beats PATH — so a
                   co-located cppuhelper3MSC.dll makes it search the TEST dir ("loading
                   component library failed: …/<test>.run/bootstrap.uno.dll").  List
                   only test-only DLLs; the office closure is already on PATH via
                   program/ (svl_qa_test_URIHelper lists none, which is why it passed).
                   SECOND, WIDER MECHANISM found 2026-08-05 via testtools/bridgetest_java
                   — same cause, but it breaks MACRO EXPANSION rather than one DLL load,
                   so the symptom can appear layers away with no mention of a DLL:
                   cppu::get_unorc() opens get_this_libpath()+"/uno.ini", i.e. uno.ini
                   BESIDE cppuhelper3MSC.dll, and uno.ini — NOT fundamental.ini — is where
                   URE_INTERNAL_LIB_DIR is defined.  Every vnd.sun.star.expand: URI in the
                   tree goes through that handle (bootstrap_expandUri →
                   cppuhelper::detail::expandMacros), so a co-located cppuhelper3MSC.dll
                   makes all of them silently expand to NOTHING.  It surfaced as jvmfwk's
                   "The file: vnd.sun.star.expand:$URE_INTERNAL_LIB_DIR/sunjavaplugin.dll
                   does not exist" + the catch-all "JRE could not be recognized".
                   CONSUMER NOTE: upstream's ONLY C++ user of the fixture,
                   xmlsecurity/qa/certext, CANNOT BUILD — it needs neon
                   (ne_ssl_cert_read) and AOO replaced neon with curl (no main/neon, no
                   ext module, no neon in configure.in, ucb webdav is curl-based; only a
                   stale NEON3RDLIB lingers in solenv/inc/libs.mk).  Hence the
                   migration-authored smoke test, without which the fixture would ship
                   unexercised.
                   LANDMINE for any new launcher: bazel test's CWD is NEITHER the exe
                   dir NOR the execroot — locate everything from %~dp0 (see
                   _windows_relpath in gtest_test.bzl).
                   LANDMINE (icu): C:\Windows\System32\icuuc.dll SHADOWS our bundled
                   ICU 49 icuuc.dll, and PATH cannot beat it — System32 is searched
                   before CWD/PATH.  Anything importing it (sw.dll, sfx.dll…) dies at
                   load with STATUS_ENTRYPOINT_NOT_FOUND 0xC0000139 + an EMPTY
                   test.log; soffice.exe is immune only because it sits in program/
                   next to our copy.  gtest_test co-locates the ICU DLLs with the test
                   exe when uno_install is set.  icuuc is the ONLY collision among the
                   248 staged DLLs.  Empty-log triage: 0xC0000139 = wrong DLL won the
                   search; 0xC0000142 = DllMain failed (CRT activation ctx).
                   Wired since: sal qa_rtl_strings, sw_qa_bigpointerarray,
                   sfx2_qa_metadatable, desktop_qa_dp_version.  uno_install doubles as
                   "give me the whole office DLL closure on PATH", which beats
                   enumerating dozens of transitive DLLs for sw/sfx-sized libraries.
                   writerfilter_qa_doctok wired too (RED on its own missing fixture:
                   reads <cwd>/test.doc, never checked in; testInitUno passes).
                   FIXTURE (c) DONE 2026-08-05 — "own mini installation": the test
                   supplies its own registry data and builds its own service manager,
                   no default bootstrap and no soffice.  Four NEW general gtest_test
                   attrs, all reusable: data_tree={label:"rel/path"} (staging at a
                   CHOSEN path, not flat by basename — $ORIGIN resolves against an
                   ini's own dir and a mini installation has TWO files named
                   bootstrap.ini), ure_bootstrap="rel/path" (point URE_BOOTSTRAP at a
                   staged ini instead of the install's program/fundamental.ini; STILL
                   needs uno_install — only the DATA root moves, the DLL closure stays
                   on PATH from program/), env={} and prerun=[] (extra env vars, and
                   launcher command lines run before the exe, each || exit /b 1).  Both
                   expand $(RUNDIR)/$(PROGRAM)/$(SCRATCH)/$(SCRATCH_URL).  $(SCRATCH) is
                   a fresh WRITABLE dir under TEST_TMPDIR wiped before AND after —
                   everything a test writes goes there, since the staged dir is
                   bazel-out and must be read-only; office_connection's user
                   installation is now this same mechanism.  prerun exists for fixtures
                   only buildable AT RUN TIME (a registry a UNO tool must write: as a
                   build action it needs its own DLL closure + CRT manifest, and the
                   location it records is an ABSOLUTE path that must not enter a cached
                   artifact).  BACKSLASH RULE CUTS BOTH WAYS: an env value read back via
                   rtl::Bootstrap::get() is macro-expanded so must be DOUBLED
                   (arg-user); one read with plain getenv() must NOT be
                   (CONFIGMGR_UNIT_FORWARD_STRING).  Prefer a file:/// URL where the
                   consumer takes one.
                   STILL TODO, with what each actually needs:
                    • configmgr/qa/unit — WIRED 2026-08-05 but RED, and NOT fixable
                      without a SOURCE change.  The blocker recorded here before ("needs
                      a custom ure_bootstrap root + extra env") was read off
                      qa/unit/makefile.mk and was WRONG: that fixture is built and works
                      (mini installation stages + resolves).  The real wall is that the
                      test's BOOTSTRAP PREMISE was retired under it — regcomp -register
                      registers via component_writeInfo(), the pre-.component mechanism,
                      and configmgr/source/services.cxx exports only
                      component_getFactory + component_getImplementationEnvironment
                      (tree-wide: 438 .component files vs a handful of
                      component_writeInfo exporters, all odk examples/workbench/legacy
                      tests) ⇒ "cannot get symbol: component_writeInfo".  The escape of
                      feeding it a TEXTUAL (XML) services registry is closed: test.cxx
                      calls the 1-ARG createRegistryServiceFactory overload, which
                      defaults bReadOnly=sal_False, so SimpleRegistry::open() gets
                      (readonly=false, create=true) and the textual branch needs
                      bReadOnly && !bCreate.  UNO_SERVICES belongs to
                      defaultBootstrap_InitialComponentContext, not this path.  UPSTREAM
                      KNOWS: the GoogleTest-migration commit 7231f715d2 (#i125003#) says
                      "All tests fail and on Windows it doesn't start running".  Kept
                      wired (it BUILDS) with the regcomp line deliberately left in — its
                      error names the retired mechanism exactly.  Fix = port test.cxx to
                      defaultBootstrap_InitialComponentContext + services.rdb (i.e.
                      fixture (a), already supported), or pass bReadOnly=sal_True with a
                      textual registry, or re-export component_writeInfo (worst).
                      GENERAL LESSON: a qa/ dir gated behind ENABLE_UNIT_TESTS=NO for a
                      decade may encode a mechanism the product no longer has — check
                      the test's BOOTSTRAP PATH against current source before costing
                      the fixture.
                    • cppuhelper/qa/propertysetmixin — 6/6 GREEN 2026-08-05, and the
                      FIRST Java UNO component this tree has ever loaded (the 3 testJava*
                      cases start a real JVM via jvmfwk, javaloader builds a class loader
                      over qa_propertysetmixin.uno.jar, UNO round-trips into it).
                      //main/cppuhelper:cppuhelper_tests is therefore a GREEN GATE again.
                      DEBUGGING NOTE: UNO exceptions don't derive from std::exception, so
                      gtest reports any failure here as a bare "Unknown C++ exception"
                      with NO message — the real diagnosis is on stderr, flushed at
                      process exit, i.e. at the END of test.log, not in the gtest summary.
                      It is fixture (a) (in-process bootstrap,
                      NO soffice) despite living behind OOO_SUBSEQUENT_TESTS — the old
                      note in cppuhelper/BUILD.bazel calling it an OfficeConnection test
                      was wrong.  Uses the MODERN .component mechanism, so it does NOT
                      hit the configmgr component_writeInfo wall.  Most involved qa/
                      wiring so far — FOUR artifacts, not one: idl_library
                      (qa/propertysetmixin/types.idl → headers) + merge_rdb (re-emits
                      just the registry as psm_types.rdb, since idl_library returns rdb
                      AND a header dir) + the component DLL qa_propertysetmixin.uno
                      (2-export DEF — comp_propertysetmixin.cxx has NO
                      component_canUnload) + services_rdb registering it at
                      vnd.sun.star.expand:$OOO_INBUILD_SHAREDLIB_DIR/… .
                      LANDMINE: rtl::Bootstrap has NO append and resolves the
                      ENVIRONMENT BEFORE the ini, so env UNO_TYPES/UNO_SERVICES REPLACE
                      fundamental.ini's and must REPEAT them (incl. oovbaapi.rdb) before
                      adding the test's own — DRIFT WATCH on main/staging/fundamental.ini.
                      The Java half is now wired too: javamaker_classes on the private IDL
                      + java_library(JavaSupplier.java) merged by uno_jar into
                      qa_propertysetmixin.uno.jar (RegistrationClassName manifest), a 2nd
                      component in the test's services.rdb at
                      vnd.sun.star.expand:$OOO_INBUILD_JAR_DIR/…, and
                      UNO_JAVA_JFW_JREHOME putting jvmfwk in DIRECT mode (its other mode
                      wants a javasettings_<os>_<arch>.xml a human wrote via Tools >
                      Options > Java, which a fixture has no history of ⇒
                      JFW_E_JAVA_DISABLED).  The JRE must match the TARGET arch (32-bit
                      for the default build), so the path is select()ed per arch —
                      machine-specific, like //main/bridges test_any_jni's jvm_path_dirs;
                      both want one build setting for "the JRE for this arch".
                      The 3 test BODIES are shared functions called once per supplier, so
                      the same assertions now pass through BOTH a C++ and a Java
                      implementation of the same interfaces.
                      See main/cppuhelper/readme.md.
                    • xmlsecurity/qa/certext — BLOCKED, and NOT on fixture (b) as
                      recorded here before: it #includes <neon/ne_ssl.h> and calls
                      ne_ssl_cert_read(), but AOO deleted neon in favour of curl.  Would
                      need a neon external wrap (or a source change) — do not treat it as
                      a fixture-(b) consumer.
                   DONE 2026-08-04 — sal child-process suites (osl/process, rtl/process,
                   rtl/bootstrap).  The recorded blocker ("helper exe via
                   getExecutablePath()+/../bin, needs source changes") was a MISREADING:
                   the idiom is dir-of(own module) → strip last component → +"bin", which
                   under dmake's solver/bin is the IDENTITY, not a sibling lookup.  New
                   gtest_test `bin_layout` just names the staging dir "bin" and it
                   resolves; helpers via sal_qa_helper_exe + `companions`.  Second
                   landmine, unrelated: BOTH osl/process TUs use LPTSTR/
                   GetEnvironmentStrings/_tcslen without including <windows.h> (nothing
                   they include reaches it — precompiled_sal.hxx is empty), i.e. bit-rot,
                   since dmake gates qa/ behind ENABLE_UNIT_TESTS=NO.  Fixed with
                   /FIwindows.h, but the two need OPPOSITE flavours and swapping them
                   compiles cleanly while walking an ANSI block as wide chars: parent =
                   ANSI, child = /DUNICODE= /D_UNICODE= (empty, so its own #define is an
                   identical redefinition, not C4005).  Residual reds are test defects,
                   not build: osl_process asserts an env ORDER Windows doesn't use;
                   rtl_Bootstrap expects the default ini to be testshl2.ini because the
                   testshl2-era process was literally testshl2.exe.
                   JAVA/UNO SUITES — FIXTURE DONE 2026-08-06, 3 GREEN.  New rule
                   uno_junit_test (build/rules/junit_test.bzl) = the JAVA HALF of
                   fixture (b), porting installationtest.mk::javatest: java_library +
                   a launcher .bat that runs org.junit.runner.JUnitCore with
                   -Dorg.openoffice.test.arg.soffice=path:<staged soffice> and
                   .arg.user=file:///<scratch>.  SAME fixture as the C++ side, spelled
                   differently, and BOTH differences bite: the args are SYSTEM
                   PROPERTIES not env vars (Java reads System.getProperty, C++ reads
                   rtl::Bootstrap), and arg.user is a file:/// URL not a native path
                   (Java feeds it to -env:UserInstallation=, C++ to
                   getFileURLFromSystemPath).  Transport differs too — Java connects
                   over a NAMED PIPE (pipe,name=oootest<uuid>), so unlike bridgetest_urp
                   there is no port to reserve and no tags=["exclusive"]; the three
                   suites run concurrently.
                   OFFICE SHUTDOWN DEADLOCK, and the general escape from it —
                   uno_junit_test `fixture_starts_office`.  qadevOOo/qa/unoapi ran
                   its assertions green in 3s and then HUNG to the 300s timeout.
                   DIAGNOSED, not guessed: the JVM's main thread sits in
                   Process.waitFor() (OfficeConnection:126), i.e. XDesktop.terminate()
                   ALREADY RETURNED and the office did not exit; the office is alive,
                   idle, has NO visible window and no longer answers a fresh UNO
                   connection; a non-invasive `cdb -pv -p <pid> -c "~*k"` shows its
                   MAIN thread inside a WinProc dispatch that entered a NESTED VCL
                   message wait (Application::Execute → DispatchMessageW → …
                   → GetMessageW) — so it holds the SolarMutex — while TWO URP threads
                   block in vos::OMutex::acquire: an incoming binaryurp request into
                   fwk's LockHelper, and an sw proxy release coming back through
                   msci_uno.  So it is a solar-mutex-vs-in-flight-bridge-calls deadlock
                   in the OFFICE's shutdown, not a fixture defect (the suites holding
                   no stale proxies at teardown terminate cleanly); fixing it properly
                   is a SOURCE change.  fixture_starts_office makes the LAUNCHER own
                   the office (start detached, wait for the pipe, taskkill after) and
                   the test attach with connect: instead of path:, which leaves
                   tearDown a NO-OP (process==null ⇒ it neither terminates nor waits).
                   Verdict = the JUnit result.  OPT-IN: it gives up tearDown's check
                   that the office exits 0, so use it only where THAT is what is broken
                   and say why.  Two mechanics: (1) the readiness wait is ONE PowerShell
                   process — cmd CANNOT list the pipe namespace ("dir \\.\pipe\" =
                   "invalid parameter", it is a device path) and osl pipes appear as
                   \\.\pipe\OSL_PIPE_<SID>_<name> so match the NAME at the end; it is
                   not optional because OfficeConnection's resolve loop has NO sleep
                   when it did not start the process itself (it would spin a core);
                   (2) the kill matches the UNIQUE PIPE NAME on the command line, never
                   taskkill /im soffice.exe — that would kill a developer's session and
                   any concurrent suite.  GREEN: //main/qadevOOo:
                   qa_complex_junitskeleton (3, ~14s — upstream's own worked example,
                   so it exercises the whole path: connect → XMultiServiceFactory →
                   qadevOOo TestParameters → fixture doc by relative path → LOAD it into
                   the office → close → office temp dir), //main/svl:
                   qa_complex_passwordcontainer (3, ~13s — com.sun.star.task.
                   PasswordContainer with/without master password, persistent +
                   session-only, through the test's own XInteractionHandler; no C++
                   equivalent is possible, it is a UNO-only service), //main/svtools:
                   qa_unoapi (~25s — FIRST UNOAPI suite ever run here, 26 interface/
                   property checks on svtools.AccessibleTabBar), //main/qadevOOo:
                   qa_unoapi (~16s — the runner testing ITSELF, i.e. the guard on the
                   framework every other unoapi suite is built out of;
                   fixture_starts_office).
                   JUnit 4.10 via http_file (@junit_jar → //build/third_party/junit) —
                   upstream's OOO_JUNIT_JAR, never in ext_sources.  4.10 DELIBERATELY:
                   it EMBEDS hamcrest-core, so there is no second jar (every upstream
                   recipe forks on HAMCREST_CORE_JAR).
                   THREE PRODUCT GAPS surfaced, all invisible until a FOREIGN process
                   loaded our DLLs:
                    (1) jpipe.dll + jpipx.dll were NEVER STAGED.  Nothing in the office
                        links or loads them — the CLIENT JVM does — so their absence
                        broke nothing visible.  Now in //main/staging.
                    (2) a /MD DLL loaded by a process we do NOT build needs its OWN
                        EMBEDDED MANIFEST.  We link /MANIFEST:NO everywhere, which is
                        invisible inside soffice.exe (its manifest covers the process)
                        and fatal for a DLL a stock java.exe loads: no activation
                        context ⇒ MSVCR90.dll unresolvable ⇒ System.loadLibrary says only
                        "Can't find dependent libraries" (and a loose msvcr90.dll on PATH
                        would just trade that for R6034).  New
                        //main/external/msvcp90:vc90_dll_manifest_res — same manifest at
                        RT_MANIFEST id 2 (the DLL slot; id 1 is the EXE slot) — linked
                        into both pipe DLLs.  Upstream gets this free: solenv embeds a
                        manifest in every DLL.  ANY future DLL a foreign host loads needs
                        this.
                    (3) OOoRunner.jar carried NO object descriptions — see the qadevOOo
                        bucket.  Fixing it is what makes the unoapi CATEGORY runnable.
                   The test JVM is arch-selected like jre_home_env (new jre_java_exe /
                   jre_home_native in //build:jre.bzl): the office is a separate process,
                   but the JVM loads jpipe.dll ITSELF, so a 32-bit build needs a 32-bit
                   JVM.  JAVA_HOME is pinned to the same JDK because OfficeConnection
                   always passes -env:UNO_JAVA_JFW_ENV_JREHOME=true (the OFFICE's jvmfwk
                   then reads it).
                   qa/unoapi — WHOLE CATEGORY WIRED 2026-08-06, all 18, every one
                   //main/<module>:qa_unoapi.  They are not 18 tests but 18 sets of DATA
                   for one runner, so they are ONE MACRO — unoapi_test (junit_test.bzl)
                   over the only four things that differ: module (= the source dir name,
                   which is ALSO the adapter package org.openoffice.<module>.qa.unoapi
                   .Test for all 18 without exception), sce, xcl, tdoc.  Mirrors the
                   JunitTest_<module>_unoapi.mk files, which differ only in their
                   -Dorg.openoffice.test.arg.<name> lines.  ANALYSED CLEAN (bazel build
                   --nobuild, all 18); NOT YET RUN — 16 of them have never executed here.
                   THE RECORDED -tdoc GATE WAS WRONG on the key fact: it is NOT
                   qadevOOo/testdocs.  Every makefile that sets -tdoc points at
                   $(SRCDIR)/<module>/qa/unoapi/testdocuments — 10 small per-module dirs,
                   ~570 KB total (linguistic points at qa/unoapi ITSELF, i.e. passing
                   something non-null; neither of its 2 objects opens a document).
                   qadevOOo/testdocs is only the FALLBACK util.utils getFullTestDocName
                   takes when the arg is absent AND SRC_ROOT is set — a dmake-tree
                   assumption no wired suite reaches.  So staging it is NOT a gate;
                   OOoRunnerLight/testdocs stay ⬜ for other reasons.
                   The gate that WAS real: data_dirs, the directory-preserving companion
                   to data_tree (gtest_test.bzl, shared by both launchers).  data_tree
                   maps one label→one path, right when exact PLACEMENT is under test (a
                   mini installation has two files named bootstrap.ini); wrong for a
                   document ROOT, since the runner joins a name onto -tdoc AT RUN TIME so
                   which documents a scenario opens is not a build-time fact — and
                   dbaccess/forms testdocuments/TestDB/ is a NESTED dir data_tree cannot
                   express at all.  Each entry stages every file of the label under the
                   chosen dir at its path relative to the LONGEST COMMON DIRECTORY of
                   that label's files: the strip prefix is DERIVED, not declared, which
                   is exact for the glob(["<dir>/**"]) filegroup the macro generates and
                   keeps the call site to the one fact it knows.
                   FIRST ONE RUN — //main/toolkit:qa_unoapi, RED on an ORPHANED MUTEX in
                   acc.dll (main/accessibility), left red.  Fixture is FINE: it connects,
                   stages testdocuments/, and the first object
                   (toolkit.AccessibleDropDownComboBox = the Find toolbar combo) passes
                   FOUR whole interfaces; then the office HANGS and takes the other 52
                   with it.  cdb -pv (non-invasive, on the live hang): Windows logs
                   Application Hang event 1002, NOT a crash (Responding=False) — the Java
                   EOFException/DisposedException are only the consequence; the MAIN
                   thread holds the SolarMutex inside a WinProc dispatch
                   (Application::Execute→Yield→DispatchMessageW) delivering a focus event
                   ImplGrabFocus→ImplCallActivateListeners→VclEventListeners::Call→
                   ootk!VCLXAccessibleComponent::WindowEventListener→acc.dll→
                   sal3!osl_acquireMutex and BLOCKS; a binaryurp worker (the test's UNO
                   call) blocks on the SAME acc mutex; and !locks names the contended CS
                   (LockCount 2 = exactly those two waiters) whose OwningThread IS NOT IN
                   THE LIVE THREAD LIST.  So NOT a lock-order inversion and NOT the
                   qadevOOo shutdown deadlock — the lock is ORPHANED: a thread exited
                   holding it, and an osl mutex is a plain CRITICAL_SECTION, so it can
                   never be acquired again and nothing recovers in-process.  A real
                   product defect reachable by ANY assistive technology attaching to a
                   running office, not just this test; fix = source change in
                   main/accessibility, out of scope.  Upstream's knownissues.xcl excludes
                   toolkit.AccessibleComboBox but NOT AccessibleDropDownComboBox, i.e.
                   upstream thinks the object is testable — consistent with a qa/ dir
                   gated behind ENABLE_UNIT_TESTS=NO for a decade that never ran.
                   TWO MORE REDS EXPECTED, both recorded at the call site: dbaccess (the only
                   -ini adapter; dbaccess.props names a MySQL server no dev box has, so
                   ORowSet + OSingleSelectQueryComposer fail and the other 7 objects do
                   not — wired anyway, a partial red naming which objects need a DB beats
                   an unwired suite) and sc (43 active objects vs 67 COMMENTED OUT, each
                   with its issue number — the honest upstream state, left verbatim).
                   Long poles size="large": sw (75 objects), toolkit (53), sc (43),
                   forms (34).
                   STILL ⬜: the 24 qa/complex dirs — unlike unoapi these are hand-written
                   suites with no shared shape, so one at a time.  Gates: per-module
                   fixtures (svl/qa/complex/ConfigItems needs a C++ helper component) and
                   several (writerfilter's among them) have no makefile.mk at all, i.e.
                   were never wired upstream either.
                   See main/test/readme.md.
                   OPEN RED, LEFT RED ON PURPOSE — //main/bridges:test_any_jni.
                   NOT a fixture problem and NOT caused by anything above: it fails in
                   TestSeqSize because jni_uno's seq_allocate() computes
                   SAL_SEQUENCE_HEADER_SIZE + (nElements*nSize) in 32-BIT SIGNED
                   arithmetic (jni_data.cxx:41).  5,000,000 × 1024 = 5.12e9 wraps to
                   825,032,704, which a 32-bit process cannot allocate ⇒
                   BridgeRuntimeError "out of memory!", and the test only accepts a
                   rejection that came from a SIZE GUARD ("out of range").  That
                   distinction is the point: today the oversized sequence is refused BY
                   LUCK.  Pick a count whose wrap lands small — 4,194,304 × 1024 is
                   exactly 2^32 ⇒ wraps to 0 — and seq_allocate returns an 8-byte header
                   while the loop at jni_data.cxx:1115 writes nElements elements into it
                   (nPos*nSize overflowing too, so some writes land BEFORE the buffer):
                   a heap overflow reachable from any in-process Java UNO caller.
                   THE FIX EXISTS ON ANOTHER BRANCH — find it by SUBJECT, not hash:
                   "jni_uno: added guard sequence allocation size against integer
                   overflow", on security-triage and ww8-fixes.  (Cited as 7efd38098e
                   until 2026-08-16; rebasing rewrites that hash on every branch it
                   sits on, so the subject is the only stable handle — it is currently
                   1b2ff27d90 on security-triage and d150d3a4a6 on ww8-fixes, same
                   patch-id.  security-ASVS-Scan also carried it and was deleted as
                   strictly superseded by security-triage.)  The fix widens the size to
                   sal_uInt64, rejects negatives, and returns "sequence size out of
                   range" above SAL_MAX_SIZE.  799ee9fa5e
                   brought the TEST here when it unified the two bridge test sets and
                   deliberately left the security-branch SOURCE edits behind; its own
                   note says "both test targets ANALYSE clean", i.e. analysis, not
                   execution.  So this branch holds the regression pin without the fix.
                   Kept red because it is an accurate report of a real defect and this
                   branch does not change source; cherry-picking 7efd38098e (~15 lines,
                   one file) is the fix if it is ever to carry it.
                   See main/bridges/readme.md.
testtools     🔨  bridgetest GREEN 2026-08-05, ALL THREE halves — //main/testtools:
                   bridgetest (C++ object in-process, ~1.1s), :bridgetest_java (Java
                   object over the java_uno JNI bridge, ~1.4s) and :bridgetest_urp
                   (C++ object in a SECOND PROCESS over socket URP, ~4s);
                   :bridgetest_tests runs all three.  This is
                   the widest type-marshalling check in the tree (every simple type,
                   string, enum, struct, POLYMORPHIC struct, sequence, any, interface,
                   attribute, out/inout param, exception, multiple inheritance, current
                   context, recursive + sequence-of-calls dispatch) and the Java half is
                   the FIRST verification that the java_uno bridge marshals correctly —
                   propertysetmixin only proved a Java component can be LOADED.
                   NOT a gtest and must not become one (that would be a source change):
                   it is 3 UNO components driven by the generic //main/cpputools uno.exe,
                   which instantiates the -s service, queries XMain and calls run() with
                   everything after "--"; the driver takes the NAME of the object to test,
                   which is exactly why ONE driver serves the C++/Java/Python/CLI objects.
                   run() 's return value IS the exit code, so no wrapper is needed.
                   NEW general rule attr `run_args` (gtest_test.bzl, on staged_run_test):
                   a FIXED command line baked into the launcher and token-expanded like
                   `env`, so the "test binary" can be a generic tool the suite configures.
                   FOURTH FIXTURE KIND (the C++ half): uno.exe -ro calls
                   bootstrap_InitialComponentContext(REGISTRY), not defaultBootstrap —
                   no fundamental.ini, no URE_BOOTSTRAP, NO uno_install, ~1s.  Everything
                   else is hardcoded in cppuhelper bootstrapInitialSF() out of
                   bootstrap.uno.dll.  Two reusable facts: (1) the TEXTUAL (XML) registry
                   WORKS on this path — openRegistry passes (bReadOnly=true,
                   bCreate=false), exactly the combination configmgr/qa/unit cannot get
                   from its 1-arg createRegistryServiceFactory; (2) a RELATIVE component
                   uri ("./cppobj.uno.dll") is resolved by textualservices.cxx with
                   rtl::Uri::convertRelToAbs against the RDB FILE's own URL, so "beside
                   this rdb" needs no bootstrap variable and no env at all.
                   LANDMINE — the co-located-UNO-DLL rule has a SECOND, bigger mechanism
                   than the recorded one (bootstrap.uno.dll via get_this_libpath):
                   cppu::get_unorc() opens get_this_libpath()+"/uno.ini", i.e. uno.ini
                   BESIDE cppuhelper3MSC.dll, and uno.ini is where URE_INTERNAL_LIB_DIR
                   is defined (fundamental.ini does NOT define it).  EVERY
                   vnd.sun.star.expand: URI resolves through that one handle
                   (bootstrap_expandUri → cppuhelper::detail::expandMacros).  Co-locate
                   cppuhelper3MSC.dll and the exe's dir wins the loader search ⇒ lookup
                   lands in the test dir ⇒ no uno.ini ⇒ every such macro silently expands
                   to NOTHING.  Surfaced two layers away as jvmfwk's "The file:
                   vnd.sun.star.expand:$URE_INTERNAL_LIB_DIR/sunjavaplugin.dll does not
                   exist" + "could not be recognized".  So with uno_install, `runtime`
                   must list ONLY test-only files: the C++ target lists the whole core
                   stack and the Java one lists NONE — opposite, both correct.
                   Also: the Java run needs the INSTALL's services.rdb as a 3rd -ro
                   (dmake's $(SOLARXMLDIR)/ure/services.rdb).  Registering just the two
                   Java2-loader components instead does NOT work — javaloader resolves
                   the component URL via com.sun.star.uri.UriReferenceFactory, itself a
                   URE component (stoc uriproc), so it only moves the failure one service
                   along.  Passed as a file:/// URL (unoexe convertToFileUrl takes it
                   verbatim).  //build:jre.bzl now centralizes the machine-specific,
                   arch-select()ed test JRE (was duplicated in cppuhelper).
                   SOCKET-URP VARIANT GREEN 2026-08-05 — //main/testtools:bridgetest_urp
                   (~4s), dmake's bridgetest_server + bridgetest_client pair.
                   Same driver, same assertions, same C++ object, but in a SECOND PROCESS,
                   so every call crosses //main/binaryurp over TCP.  Only test in the tree
                   that puts a real object graph across a real connection (binaryurp's own
                   qa/ covers cache+unmarshal in isolation); also WIDER than the Java half,
                   which must pass noCurrentContext — here the current-context check runs,
                   so it is the only proof a UNO current context propagates across a
                   process boundary.  The whole client/server split is ONE argument: "-u
                   <uno url>" AFTER "--" makes it an arg to BridgeTest::run(), which then
                   resolves via UnoUrlResolver instead of instantiating locally.
                   FIRST REAL BITE of the KNOWN .uno COMPONENT-NAMING DIVERGENCE, at
                   exactly the site CLAUDE.md predicted ("remote-UNO/URP bootstrap"):
                   dmake gives the SERVER only the two test registries and lets
                   unoexe.cxx's createInstance() fallback loadSharedLibComponentFactory()
                   the HARDCODED names acceptor.uno.dll / connector.uno.dll /
                   binaryurp.uno.dll — we emit acceptor.dll etc., so that path can never
                   fire.  FIX IS NOT A RENAME: nest the installation's own
                   program/services.rdb into the server too (the Java half already does it
                   for the client) and all three resolve BY SERVICE NAME on the first
                   attempt, so the filename path is never reached.  Naming services is
                   rename-proof; a rename would need services.rdb moved in lockstep.
                   TWO NEW GENERAL gtest_test ATTRS: server_args (stage `binary` a SECOND
                   time as <name>_server.exe and start it detached before the client —
                   the second name is what makes cleanup `taskkill /f /im` precise; a
                   shared image name would kill the client too) and server_ready_port
                   (poll netstat until LISTENING).  The wait is NOT optional: the server
                   needs ~1s to reach accept() and UnoUrlResolver::resolve() does ONE
                   connect() then throws NoConnectException.  And it must be netstat, not
                   a connect probe — a probe that succeeds CONSUMES the single connection
                   --singleaccept will serve.  Verdict = the CLIENT's exit code only;
                   stragglers are killed so a failed run cannot leave a server blocked in
                   accept() holding the port.  tags=["exclusive"] — port 2002 is
                   upstream's hardcoded choice and is machine-global.  NOTE dmake only
                   GENERATES the two .bat files (in ALLTAR but never run; only the
                   in-process `runtest` executes), so there was no recipe to port.
                   See main/testtools/readme.md.  STILL ⬜: cli + cliversioning + qa/cli
                   (cli_ure bucket), source/performance (a benchmark), and
                   bridgetest_javaserver (Java server over URP — needs the background
                   process to be a `java` command line, which server_args does not
                   express; its Java marshalling is already covered in-process).
                   pyuno variant — ⬜ BLOCKED, and the old note "reachable now" was WRONG.
                   It is not the same driver: main.py is a unittest suite whose FIRST
                   statement is unohelper.addComponentsToContext(…,
                   "com.sun.star.loader.SharedLibrary"), i.e.
                   ImplementationRegistration → DllComponentLoader::writeRegistryInfo →
                   cppuhelper writeSharedLibComponentInfo, which resolves
                   component_writeInfo — the SAME RETIRED MECHANISM as configmgr/qa/unit.
                   cppobj/bridgetest export only getImplementationEnvironment+getFactory
                   ⇒ "cannot get symbol: component_writeInfo".  Putting them in
                   UNO_SERVICES does not help (the call is unconditional, before any test),
                   and importer.py's testDynamicComponentRegistration repeats it with
                   acceptor.uno/connector.uno — it is the suite's PREMISE, not one line.
                   Second, independent blocker: main.py never checks the runner result and
                   never sys.exit()s, so a faithful port would be GREEN whatever it
                   reported.  Fixing either is a source change.
qadevOOo      🔨  OOoRunner.jar built (//main/qadevOOo:OOoRunner — qadevOOo QA
                   framework, ~2137 classes; classpath ridl/unoil/jurt/juh_jar/
                   java_uno_jar; manifest omitted).  Unblocks bridges java_uno
                   tests (acquire, java_remote).
                   objdsc/*.csv ARE NOW JARRED (2026-08-06) — a deliberate
                   divergence from the ant jar target (whose <include> list omits
                   .csv) and the thing that makes the whole unoapi CATEGORY
                   runnable: APIDescGetter takes a description either from a
                   -objdsc DIRECTORY or, absent that argument, from the CLASSPATH
                   resource /objdsc/<module> (it has a JarURLConnection branch for
                   exactly that), and the qa/unoapi Test.java adapters pass no
                   -objdsc ⇒ against upstream's csv-less jar every unoapi suite
                   dies at its first object with "couldn't find module".
                   JunitTest_qadevOOo_unoapi GREEN 2026-08-06 (//main/qadevOOo:
                   qa_unoapi, ~16s) via uno_junit_test fixture_starts_office — see
                   the OFFICE SHUTDOWN DEADLOCK note in the test bucket.
                   STILL ⬜: OOoRunnerLight, testdocs.  CORRECTION — testdocs is
                   NOT what the 11 -tdoc adapters need: they each point at their
                   OWN <module>/qa/unoapi/testdocuments (now staged per module via
                   data_dirs).  qadevOOo/testdocs is only utils.getFullTestDocName's
                   fallback when -tdoc is absent AND SRC_ROOT is set, which no
                   wired suite hits.  It is a leftover, not a blocker.
testgraphical ⬜  (graphical/visual regression tests; needs instsetoo_native + qadevOOo)

── Remaining: Java-based ────────────────────────────────────────────────
JAVA2 LOADER — STARTED 2026-08-05.  The bucket's real first blocker was NOT
javamaker (already built): com.sun.star.loader.Java2 had NO IMPLEMENTATION.
javaloader.uno.dll + javavm.uno.dll were never built while BOTH were registered
in services.rdb MAPPED TO bootstrap.uno.dll as a placeholder — unworkable, since
neither impl is in that DLL.  Now built (//main/stoc), staged, and registered at
their real DLLs; x64 //main/staging:install GREEN per user build 2026-08-05.
TWO LANDMINES: (1) SOLAR_JAVA is LOAD-BEARING — jvmaccess/virtualmachine.hxx
pulls the real <jni.h> only under it, else stubs, so every JNIEnv-> call is
C2027 even in javaloader.cxx which #includes "jni.h" ITSELF (the stub is already
in scope); (2) javavm needs /Zc:wchar_t- (passes sal_Unicode ptr into NewString/
GetStringRegion; on Windows sal_Unicode IS wchar_t, distinct from jchar unless
wchar_t is unsigned short), javaloader does NOT (only NewStringUTF).
STAGING DONE 2026-08-05 — the whole bootstrap chain is now wired; what is left is
to EXERCISE it (nothing has yet loaded a real Java UNO component).
OPEN QUESTION ANSWERED — there are TWO channels, and conflating them is what made
URE_INTERNAL_JAVA_CLASSPATH look optional: (1) FIVE jars arrive by HARDCODED NAME
— javavm.cxx opens the literal "$URE_INTERNAL_JAVA_DIR/unoloader.jar", and
UnoClassLoader.createUrls() then appends java_uno.jar/juh.jar/jurt.jar/ridl.jar.
No manifest, no classpath var, no UNO-Type-Path involved: the FILENAME IS THE
LOOKUP KEY.  (2) unoil.jar is NOT one of the five — it reaches the loader only as
the classPath ctor arg, i.e. $URE_INTERNAL_JAVA_CLASSPATH → URE_MORE_JAVA_TYPES
(upstream $ORIGIN/classes/unoil.jar + ScriptFramework.jar + each extension's
UNO_JAVA_CLASSPATH).  So an empty value does not just lose extensions, it loses
EVERY com.sun.star.* office API type.  UNO-Type-Path bites somewhere else
entirely: getClassLoader() reads it when javaloader opens a COMPONENT jar; juh's
empty value suppresses the hoist, an ABSENT one falls back to "<>" = re-add the
jar itself, already there via createUrls() ⇒ harmless duplicate.  Hence only
juh's RegistrationClassName was reproduced (juh.jar IS a registered component).
NEW RULE uno_jar (build/rules/java_pipeline.bzl, singlejar) solves all three
build-side problems at once — exact `out =` filename at the PRODUCING target
(not a staging rename table), N-jar merge (ridl.jar = :ridl + :udkapi_java_jar,
verified 436 classes incl. XInterface/TypeClass/UnoRuntime), and manifest main
attributes.  DIVERGENCE: Sealed: is OMITTED.  --deploy_manifest_lines writes the
MAIN section only, so jurt's per-package un-sealing of com/sun/star/uno/ +
com/sun/star/lib/util/ (those packages are SPLIT across jurt.jar and ridl.jar) is
inexpressible, and a blanket Sealed:true would be STRICTER than upstream →
SecurityException on exactly that split.  Sealing only ever restricts, never
enables ⇒ omitting is the safe direction.
Staged flat to program/classes/ via //main/staging:_install_classes, using a new
`flatten` opt on tree_install (six jars, five packages, no single strip_prefix).
fundamental.ini got URE_INTERNAL_JAVA_DIR + URE_MORE_JAVA_TYPES +
URE_INTERNAL_JAVA_CLASSPATH + URE_OVERRIDE_JAVA_JFW_{SHARED,USER}_DATA — it is
the URE_BOOTSTRAP file theMacroExpander resolves against, uno.ini is NOT.
TWO MORE STUBS FOUND, same family as the javaloader/javavm one (a DLL exists but
the feature is compiled out): (a) jvmfwk.dll was built WITHOUT SOLAR_JAVA, under
which framework.cxx compiles jfw_startVM() to a bare `return JFW_E_ERROR;` — no
configuration could ever have made Java work; ABI is unaffected (JNI types appear
only behind pointers) so consumers including framework.h without it still link;
(b) sunjavaplugin.dll was NEVER BUILT — jvmfwk holds no JRE knowledge at all,
javavendors.xml maps every vendor to
vnd.sun.star.expand:$URE_INTERNAL_LIB_DIR/sunjavaplugin.dll which it
osl_loadModule()s.  Now built (//main/jvmfwk, 8 TUs, 4 exports from
sunjavaplugin.map → util/sunjavaplugin.def — a PRIVATE plugin interface, NOT a
UNO component: no component_getFactory, nothing in services.rdb) and listed
explicitly in staging since NOTHING LINKS IT.  javavendors.xml (from
javavendors_wnt.xml via copy_file) + jvmfwk3.ini staged into program/; the ini
NAME is fixed by fwkutil.hxx's SAL_CONFIGFILE("/jvmfwk3") relative to the
library's own dir, so it stays jvmfwk3 even though ours is jvmfwk.dll.
NOT staged: sunjavapluginrc — its one key is read via the DEFAULT rtl::Bootstrap,
not a plugin-private ini, so it looks inert in an office install.
EXERCISED AND GREEN 2026-08-05 — cppuhelper/qa/propertysetmixin 6/6, the first
Java UNO component ever loaded here (see the test bucket above).  Getting from
"staged" to "green" took FOUR more fixes, none of them in the staging itself:
 (a) JREProperties.class was never built.  sunjavaplugin does not PARSE a JRE, it
     RUNS it — "<jre>/bin/java -classpath <dir of sunjavaplugin.dll>
     JREProperties" — and reads java.vendor/version off stdout (util.cxx
     getJavaProps).  So it must be a LOOSE .class next to the plugin; a jar in
     that dir is not on that -classpath.  New rule javac_classes (java_pipeline
     .bzl), --release 8 since it runs on the CANDIDATE JRE not the build JDK.
     Upstream builds it with Ant (Ant_jreproperties.mk).
 (b) vendor "Temurin" was in NEITHER gate — vendorlist.cxx's compiled-in map nor
     javavendors*.xml.  Adoptium renamed AdoptOpenJDK→Temurin in 2021 (8u302+),
     so stock AOO rejects every current JDK.  SOURCE FIX, separate commit.
 (c) 64-bit JREs ship no client VM, and the WNT runtime-path lists had only
     bin/{client,hotspot,classic,jrockit}/jvm.dll — added bin/server/jvm.dll.
     Upstream already did the equivalent for UNX ("/lib/server/libjvm.so // > 1.8")
     and never for Windows.  Matters for x64 only; x86 finds client first.
 (d) LANDMINE, cost most of the session — a literal % in a gtest_test `env` value
     was EATEN by the launcher.  The launcher is a .bat, where % is a
     metacharacter, and percent-DIGIT is the silent case: cmd reads %2 as the
     script's (empty) 2nd argument and drops it.  A %20-escaped file URL is
     exactly that shape, so file:///C:/Program%20Files%20(x86)/… arrived as
     file:///C:/Program0Files0(x86)/….  `_expand_tokens` in gtest_test.bzl now
     doubles % BEFORE substituting its own %VAR% refs.  No layer reported an
     error; the only symptom was jvmfwk's "could not be recognized".
GENERAL TRIAGE RULE from (a)+(b)+(d): jvmfwk's "The JRE … could not be
recognized" covers EVERY failure mode of jfw_getJavaInfoByPath — missing probe
class, unsupported vendor, unreadable path — so it means "check all three", not
"vendor problem".  And because UNO exceptions don't derive from std::exception,
gtest shows only "Unknown C++ exception": the real message is on stderr at the
END of test.log.
MEASURED NOT LOAD-BEARING (do not cargo-cult): URE_INTERNAL_LIB_DIR in
fundamental.ini (findPlugin resolves the expanded URI relative to the jvmfwk
library's own dir, already program/), and sunjavaplugin.ini (staged anyway for
upstream parity — its noaccessibility key is the probe's only dependency on a
usable display, so it would bite headless).
See main/stoc/readme.md and main/jvmfwk/readme.md.
FIRST PRODUCT FEATURE ON THE JAVA RUNTIME — the EMBEDDED DATABASE, GREEN
2026-08-14 on BOTH arches (user-verified: x64 boots, "create a new Base
database" works, a table can be created).  Everything above proved a Java UNO
component could be LOADED; this is the first thing a USER can do that needs it.
Six new SDBC driver DLLs in //main/connectivity (file, dbase, flat, calc, jdbc,
hsqldb) + hsqldb.jar + sdbc_hsqldb.jar.  The chain is four artifacts, not one:
dbaccess hardcodes sdbc:embedded:hsqldb (dsntypes.cxx) -> hsqldb.dll -> which
does NOT do the SQL but asks the driver manager for "jdbc:hsqldb:db" ->
jdbc.dll -> the hsqldb.jar engine; sdbc_hsqldb.jar is what makes it a SINGLE
FILE (StorageAccess/StorageFileAccess implement org.hsqldb.lib.Storage/
FileAccess over UNO embedded storage, their native methods being the 27 Java_*
exports of hsqldb.dll).  Both jars MUST land in program/classes/ — not a
convention, HDriver.cxx hardcodes
vnd.sun.star.expand:$OOO_BASE_DIR/program/classes/<name>.jar.
ROOT CAUSE IT FIXED, and the general rule: services.rdb had SEVEN connectivity
drivers registered with NO DLL EVER BUILT (calc/dbase/flat + adabas/ado/mysql/
odbc), and their DataAccess XCUs made all seven SELECTABLE in the Base wizard.
A registration with no library behind it is WORSE THAN NONE: the driver manager
resolves the service, osl_loadModule fails on a file that was never produced,
and the UNO exception escapes the VCL message loop to desktop app.cxx:2241,
whose catch calls FatalError() -> _exit().  So picking such a driver KILLED the
office instead of reporting an error — a hard process exit, no dismissible
dialog, losing unsaved work in every other window.  NEVER register a .component
whose cc_binary does not exist; the four still unbuilt are now unregistered.
postprocess's "Java-only components ... omitted, Java build is deferred" comment
had been STALE since 2026-08-05 — that is what hid hsqldb/jdbc.
THREE LANDMINES, all recorded in main/connectivity/readme.md:
 (a) stlport is needed by EVERY consumer of connectivity/dbtools.hxx, not just
     the hash_map users: line 611 declares askForParameters() with a
     ::std::bit_vector default argument, an SGI STL extension.
 (b) dbase/DNoException.cxx must be EXCLUDED from the glob — the one file in
     that dir absent from dmake's SLOFILES and #included by nothing, a stale
     duplicate of bodies in DTable.cxx/dindexnode.cxx => ~25 LNK2005.
 (c) AOO's hsqldb patches CANNOT be used verbatim as bzlmod `patches`.
     i121754.patch was generated with `diff -urbwB` (ignore whitespace AND
     blank lines); GNU patch tolerates it, Bazel's stricter implementation
     SILENTLY SKIPPED THE WHOLE PATCH (applying only script.patch) and never
     created the new file lib/StringComparator.java — no error, no warning.
     Fix = one regenerated byte-exact patch (patch_strip 1); verified the
     fetched tree is diff -r identical to a GNU-patch reference.  This is a
     SECOND face of the recorded bzlmod patch bug, and the check is always
     "diff the fetched repo against a reference", never "the build succeeded".
     Also: a source.json edit needs `bazel mod deps --lockfile_mode=refresh`
     or the stale external repo is silently reused.
DELIBERATE DIVERGENCE — hsqldb.jar is the RELEASE JAR from the archive plus
AOO's eight BEHAVIOURALLY patched files recompiled over it (uno_jar merge,
patched classes FIRST since singlejar keeps the first duplicate), not a full
source build.  Upstream's Ant build runs HSQLDB's own CodeSwitcher preprocessor
over the sources; a plain javac fails with 12 errors where the JDBC wrapper
classes do not implement methods java.sql grew after 2008 (getCharacterStream
(long,long), isWrapperFor, generatedKeyAlwaysReturned, getParentLogger,
getObject(String,Class<T>)), and javac 21 cannot target below --release 8, so a
faithful source build would need NEW source changes upstream does not have.
Every behaviour patch is kept (incl. script.patch's 2023 SCRIPT-replay fix);
only i121754's Java-7 BUILD-compat hunks are dropped, and those are moot when
the classes come from the release jar.  Reversible: a CodeSwitcher rule (a small
Bazel action, like throwspec.py) would allow the full source build.
WATCH AT RUNTIME: sdbc_hsqldb's NativeLibraries.java calls
System.loadLibrary("hsqldb") and NOTHING in javavm/jvmfwk sets
java.library.path — it resolves today because the JVM is in-process in
soffice.exe and hsqldb.dll is already loaded from program/.  Its other preloads
(msvcr71, uwinapi, dbtoolsmi) are STALE names absent from this build; those
failures are swallowed by design, so they are noise, not the bug.
ODBC + MYSQL DONE 2026-10-02 (x86 install + x64 DLLs green; not yet run against
a live DSN).  odbcbase.dll (declspec, unregistered — the file.dll of the ODBC
side) + odbc.dll + mysql.dll, all registered, staged, DataAccess XCUs packed.
NO ODBC IMPORT LIB anywhere: OFunctions.cxx osl_loadModule()s ODBC32.DLL on the
first connect, so unixODBC is headers only.  mysql delegates BY URL through the
driver manager (links neither odbc nor jdbc).  Its XCU's third, native arm
(sdbc:mysql:mysqlc:) needs the MySQL Connector extension and is SAFE without it
— getDriverByURL returns null, no library load is attempted — unlike an
unbuilt-but-registered driver.  FOUND ALONG THE WAY: jdbc.xcu had never been
packed although jdbc.dll was registered with the embedded database, so the
generic JDBC type was missing from the Base wizard; now packed.
STILL UNMIGRATED:
 • ado — 32 srcs of COM/OLE-DB.  adoint.h/adoctint.h/oledb.h/oaidl.h/ocidl.h
   are ALL in the pinned SDK v7.0 this build already uses, so there is NO
   external module to add — just the largest file count, lowest value.
 • adabas — 22 srcs + odbcbase (now built), and needs an installed Adabas D
   server+client.  Discontinued commercial product; nothing can exercise it.
   Recommend leaving it unregistered permanently rather than migrating.
RESOURCE-LIBRARY SPLIT, fixed 2026-08-14 — GENERAL rsc_res LESSON: one module's
source/**/*.src is NOT necessarily one .res.  connectivity/source/resource builds
THREE (cnr, sdbcl, sdberr) and the old glob merged them into cnren-US.res alone,
so two bundles simply did not exist and IDs could collide across what were meant
to be separate namespaces.  They are looked up BY NAME at runtime: dbtools is
compiled with CONN_SHARED_RESOURCE_FILE=cnr, and JDriver.cxx constructs
comphelper::ResourceBasedEventLogger("sdbcl", "org.openoffice.sdbc.jdbcBridge")
where the FIRST argument is the bundle name, so the logger resolves sdbcl<lang>.res.
Latent until now because no driver was built; and sdbcl is inert while logging is
off (the level check returns before any resource lookup), while sdberr is the SQL
ERROR MESSAGES — missing, it empties the text at exactly the moment something
fails, which is part of why driver problems here are so quiet.  CHECK ANY OTHER
MODULE WHOSE rsc_res GLOBS `source/**/*.src` against its makefile's RESLIB<N>NAME
count.  Upstream's fourth reslib here (hsqldb, from hsqlui.src) is deliberately
NOT built: its own comment says the .res is never installed and it exists only to
force two images into images.zip, a dmake necessity that
//main/default_images:images already covers by globbing `database/**/*.png`.
See main/connectivity/readme.md.
NOTE: rules_java 8.11.0 IS now wired (MODULE.bazel) and the core Java UNO
runtime is migrated & green — ridljar/jurt/jvmaccess/javaunohelper/jvmfwk/
bridges (incl. the java_uno JNI bridge: java_uno.dll + java_uno.jar, done
2026-06-26)/stoc/io/remotebridges/unotools/unoil/pyuno.  The items below are
the higher-level Java apps/extensions still to do (no longer gated on rules_java
itself; per-module UNO-Java component/packaging rules may still be needed).
reportbuilder ⬜  (pure Java .oxt extension; blockers: JFreeReport suite not on Maven, SourceForge ZIPs have token-based URLs; wizards dep also deferred)
bean          ✅  DONE 2026-10-02 — officebean.jar (program/classes) + officebean.dll
                   (program/, found one dir up by jurt NativeLibraryLoader).  Jar
                   entry-identical to dmake's.  DLL: undecorated Java_* DEF + RT_MANIFEST
                   id 2 (a foreign java.exe loads it — jpipe rule).  jawt is an import
                   lib cut from a STUB compiled against the JDK's jawt.h
                   (//build/third_party/jawt), not a machine JDK's jawt.lib: a
                   `lib /DEF:` one is WRONG on x86 (name type "no prefix" ⇒ loader wants
                   JAWT_GetAWT@8, jawt.dll exports _JAWT_GetAWT@8; MSVC lib has no `==`).
                   LANDMINE: the staging aspect follows additional_linker_inputs to the
                   DLL — right for every real DLL, fatal for a stub (beside
                   officebean.dll it wins the import) — so the stub is exposed via
                   import_lib_only, whose attr the aspect does not traverse.
                   GENERAL: //main/staging never PRUNES — a file that stops being
                   produced stays in bazel-*-bin/main/staging until a clean.
                   See main/bean/readme.md.
saxon         ✅  DONE 2026-10-02 — saxon9.jar via a bzlmod module over the
                   ext_sources zip; entry-identical (1046) to the dmake jar, Latin-1
                   string constants byte-identical.  No stax (JDK javax.xml.stream).
                   LANDMINE: JavaBuilder IGNORES -encoding in javacopts (forces UTF-8);
                   ten Latin-1 sources (Numberer_* STRING LITERALS) are iconv'd in the
                   overlay.  The jar's META-INF/services TransformerFactory entry makes
                   JAXP return Saxon wherever it is on the class path.
                   UNBLOCKED A LIVE DEFECT: XSLTFilter.jar (filter/xsltfilter Java half)
                   had been REGISTERED with no jar while xsltfilter.dll only ever asks for
                   its JAXTHelper ⇒ XHTML export, Word/Excel 2003 XML, DocBook, UOF all
                   offered with nothing behind them.  Now built (entry-identical to dmake)
                   and staged beside saxon9.jar (relative Class-Path).  Then a THIRD
                   layer: share/xslt (the .xsl files) was never staged either — fixed, 55
                   files identical to scp2.  EXERCISED LIVE (x86): XHTML, Word 2003 XML,
                   DocBook export all correct; Saxon's "XSLT 1.0 stylesheet with an XSLT
                   2.0 processor" warning proves JAXP resolves to Saxon.  xsltvalidate still ⬜ (crimson/xalan, and
                   unregistered).  See main/saxon/readme.md, main/filter/readme.md.
wizards       ✅  DONE 2026-08-16 — File > Wizards is LIVE (officecfg had
                   dispatched service:com.sun.star.wizards.letter.CallWizard?start
                   at nothing since the demo baseline).  Three unrelated halves:
                   10 Java uno_jars (wizard_jar macro in main/wizards/wizards.bzl)
                   → program/classes, 9 BASIC libraries staged as DIRECTORIES to
                   share/basic/<DosName>/ (no zip step to port — upstream's .zip
                   only exists for the MSI to unpack), 7 rsc_res bundles.  First
                   module that is all Java/data, no C++.
                   LANDMINE — UNO-Type-Path must be PRESENT and EMPTY, and absent
                   is NOT the same: UnoClassLoader.getClassLoader() substitutes
                   "<>" for a missing header, which resolves to the jar's own URL
                   and gets addURL()ed into the SHARED UnoClassLoader, permanently
                   hoisting a component jar into the global UNO type path.  Needs
                   a trailing space in Starlark ("UNO-Type-Path: ") — singlejar
                   splits on ": ".  Emitted manifests verified byte-equivalent to
                   upstream's checked-in MANIFEST.MF.
                   The bundle NAME is the runtime lookup key (2nd arg of BASIC
                   InitResources(), 3rd of Java new Resource()), so the seven
                   cannot merge; dbw serves ALL the Java wizards, not just the
                   database ones.  A "com" bundle is referenced by Tools/Misc.xba
                   and exists NOWHERE in the tree, upstream included — dead
                   branch, not a migration gap.
                   NOT STAGED, on purpose: reportbuilderwizard.jar (ships inside
                   the deferred reportbuilder .oxt), basicsrvweb + basicsrvlauncher
                   (file_ooo.scp has no File entry for either and script.xlc does
                   not list them — upstream builds and then discards them).
                   DEFECT FOUND, LEFT UNFIXED — the wizard thumbnails are blank,
                   and it is NOT the usual images_root mistake.  rsc2 rewrites a
                   File= line into a path-qualified one ONLY when that line holds
                   a SINGLE '=' (rsc.cxx PreprocessSrsFile, GetTokenCount('=')==2).
                   The one-line form `ImageBitmap = Bitmap { File = "x.png"; };`
                   has two, so the rewrite is skipped, the .res keeps the BARE
                   basename and nothing reaches the .ilst; ImplImageTree::loadImage
                   then does an EXACT hasByName() against an images.zip keyed by
                   path with ZERO top-level entries.  TREE-WIDE and upstream's own:
                   1033 such references in 19 modules (svx 307, sfx2 113, sd 105,
                   cui 91, sw 87, sc 85 …), wizards contributing 30.  Modules that
                   put `File =` on its own line (vcl, framework) are unaffected —
                   which is why the Start Center images.zip fix worked.  Repair =
                   1033 source edits or re-keying images.zip for every module, i.e.
                   a product change; deliberately not slipped into a module port.
                   See main/wizards/readme.md.
xmerge        ✅  DONE 2026-10-02 — 5 jars to program/classes, all entry+manifest
                   identical to dmake's; XMergeBridge registered.  The palm/pocketword/
                   pocketexcel .xcd were packed long before, i.e. those filters were
                   offered with NOTHING behind them (same class as connectivity +
                   XSLTFilter).  EXERCISED LIVE (x86): Pocket Word export writes a valid
                   .psw.  Plugins sit on no class path — XMergeBridge opens
                   "$(progurl)/" + the filter config's "classes/<plugin>.jar".
                   xmergesync.dll (prebuilt 32-bit ActiveSync) and htmlsoff not shipped,
                   as upstream.  See main/xmerge/readme.md.
                   LIVE-TEST RECIPE (AOO has NO -convert-to; that is LibreOffice):
                   soffice -headless -accept=pipe,… then pyuno storeToURL per filter.
                   FOUND DOING IT, FIXED SAME DAY — program/python.exe could not start
                   (0xC0000135): it was the raw @python CPython exe, no CRT manifest, and
                   none of the environment AOO's launcher sets.  Now program/python.exe
                   IS upstream's launcher (pyuno/zipcore/python.cxx, embedded manifest),
                   and python-core-2.7.18/bin/python.exe is CPython with the VC90
                   manifest embedded by mt.exe (@python cannot name a main-repo label).
                   Verified x86+x64.  NOTE upstream's launcher does NOT put program/ on
                   PYTHONPATH: `import uno` resolves via sys.path[0] (script dir / CWD)
                   or a user PYTHONPATH — same as upstream, not a migration gap.
javainstaller2 ⬜ (Java installer UI)
swext         ⬜  (Writer Java extensions e.g. mediawiki)
unodevtools   ✅  MIGRATED 2026-08-18 and MOVED OUT of this bucket — it is not
                   Java and never was.  `uno-skeletonmaker.exe`, 9 C++ sources, no
                   Java source at all: Java is only one of the OUTPUT languages of
                   the skeletons it emits, which is why it links codemaker's
                   `commonjava`.  So it was never gated on the Java bucket and it
                   built on the first try.  Has a qa/ now (7 gtest cases,
                   //main/unodevtools:skeletonmaker_test) that upstream does NOT
                   have: the generators are UNO-free and take codemaker's
                   TypeManager base, so the suite runs off udkapi's rdb via
                   RegistryTypeManager with 3 DLLs and NO staged install.
                   See main/unodevtools/readme.md.

── Remaining: .NET interop ──────────────────────────────────────────────
cli_ure       ⬜  (cppu, cppuhelper, sal, codemaker, stoc, udkapi, bridges)
                   Blockers: C# rules (csc.exe), C++/CLI (/clr toolchain),
                   AL.exe policy assemblies, sn.exe strong-name signing.
unoil         ⬜  update climaker

── Remaining: Installer/packaging ───────────────────────────────────────
instsetoo_native ⬜ (native Windows installer build)
setup_native  ⬜  (Windows setup UI)
packimages    ⬜  (image packaging for install)
sysui         ⬜  (system UI integration — mime types, desktop entries)
solenv        ⬜  (legacy build environment, mostly migrated away)

── Remaining: Docs/dev tooling ──────────────────────────────────────────
autodoc       ⬜  (API documentation generator)
odk           ⬜  (OpenDocument/Developer Kit)
helpauthoring ⬜  (help authoring tools)
helpcontent2  ⬜  (help content sources)
xml2cmp       ⬜  (XML component comparison tool)

── Remaining: Standalone/misc ───────────────────────────────────────────
automation    ⬜  (test automation/macro recorder framework)
migrationanalysis ⬜ (migration analysis tool)
more_fonts    ⬜  bundled fonts; blocked: SourceForge token URLs prevent http_archive;
                   OpenSymbol staged via extras; other fonts (DejaVu, Carlito…) pending
readlicense_oo ⬜  readme.html/txt; blocked: needs xsltproc + l10ntools merge

── Out of scope: Linux/macOS — not applicable to the Windows build ──────
padmin        —   Linux printer administration
sane          —   Linux scanner interface (SANE)
unixODBC      —   Linux ODBC bridge
x11_extensions —  X11/Linux platform extensions
psprint_config —  Linux PostScript/font config
apple_remote  —   macOS Apple Remote control
macOS         —   macOS-specific platform UI

── Dropped — do NOT migrate ─────────────────────────────────────────────
stax          ❌  StAX (JSR-173 / javax.xml.stream) has been part of the JDK since
                   Java SE 6; bundling/building stax-1.2.0.jar is dead weight.  Only
                   saxon ever depended on it (ooxml already uses the JDK's
                   javax.xml.stream directly).  Since Bazel replaces the dmake/configure
                   layer entirely (where the stax module lived), there is nothing to
                   migrate — saxon must just rely on the toolchain JDK's
                   javax.xml.stream.  Supersedes upstream PR apache/openoffice#87
                   (stax removal against dmake tree, now moot).

── Innovation (NOT a migration — no dmake equivalent to port) ───────────
release-identity 💡 Replaces the old configure --with-build-version="$(date) -
                   uname" + --with-vendor.  A wall-clock build string defeats
                   reproducible builds, so do NOT port it.  Build instead a
                   deterministic release identity:
                     • //build:channel string_flag (dev|beta|release, default dev)
                       — successor to --with-vendor; gates branding via
                       product.bzl → Setup.xcu (Apache OpenOffice vs …Dev/Snapshot).
                     • version = `git describe --tags` captured as a STABLE
                       workspace-status key (STABLE_*, rebuild-triggering); ties the
                       binary to an exact commit and only changes with source.
                     • keep volatile keys (BUILD_TIMESTAMP) OUT of release artifacts;
                       if a date is needed, derive SOURCE_DATE_EPOCH from the commit
                       date, never `date +%s`.
                     • the actual "valid release" proof = a deterministic buildinfo
                       provenance manifest (commit, channel, MSVC/SDK/Bazel versions),
                       optionally SLSA attestation — verifiable, unlike a timestamp.
                   Post-migration: needs the Installer/packaging bucket landed first
                   to have artifacts to stamp.  (The old --enable-win-x64-shellext is
                   NOT carried — it's a cross-bit hack that dissolves once the office
                   itself is ported to a 64-bit toolchain.)
