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

# Apache OpenOffice — Bazel Migration

Migrated packages have been moved to: Migrated-packages.md

## Current frontier

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
                   genuinely left is not standalone — svl/qa/complex + svtools/qa/unoapi
                   + sfx2 + writerfilter/qa/complex are Java/UNO; svl/qa/test_URIHelper,
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
testtools     🔨  bridgetest GREEN 2026-08-05, BOTH halves — //main/testtools:bridgetest
                   (C++ object, ~1.1s) and :bridgetest_java (Java object over the
                   java_uno JNI bridge, ~1.4s); :bridgetest_tests runs both.  This is
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
                   See main/testtools/readme.md.  STILL ⬜: cli + cliversioning + qa/cli
                   (cli_ure bucket), pyuno variant (reachable now), source/performance
                   (a benchmark), and the socket-URP client/server variants.
qadevOOo      🔨  OOoRunner.jar built (//main/qadevOOo:OOoRunner — qadevOOo QA
                   framework, ~2137 classes; classpath ridl/unoil/jurt/juh_jar/
                   java_uno_jar; .csv objdsc NOT jarred, manifest omitted).
                   Unblocks bridges java_uno tests (acquire, java_remote).  STILL
                   ⬜: OOoRunnerLight, qa/complex, JunitTest_qadevOOo_unoapi,
                   testdocs — need running-soffice/OfficeConnection fixture.
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
NOTE: rules_java 8.11.0 IS now wired (MODULE.bazel) and the core Java UNO
runtime is migrated & green — ridljar/jurt/jvmaccess/javaunohelper/jvmfwk/
bridges (incl. the java_uno JNI bridge: java_uno.dll + java_uno.jar, done
2026-06-26)/stoc/io/remotebridges/unotools/unoil/pyuno.  The items below are
the higher-level Java apps/extensions still to do (no longer gated on rules_java
itself; per-module UNO-Java component/packaging rules may still be needed).
reportbuilder ⬜  (pure Java .oxt extension; blockers: JFreeReport suite not on Maven, SourceForge ZIPs have token-based URLs; wizards dep also deferred)
bean          ⬜  (Java bean component)
saxon         ⬜  (XSLT 2.0 processor — IN SCOPE for full migration.  Feeds the
                   Java xsltfilter UNO component (UOF/DocBook/user XSLT-2.0 filters);
                   without it those filters silently drop (upstream gates the whole
                   xsltfilter lib behind DISABLE_SAXON).  Leaf of the Java bucket —
                   only useful once Java/UNO-component rules exist, so sequence AFTER
                   rules_java enablement, not before.  saxon9.jar = 3rd-party tarball
                   source-9.0.0.7-bj (ant/build.xml) → http_archive + java build or
                   vendored jar.  When built, compile against the toolchain JDK's
                   javax.xml.stream — no bundled stax jar.  See Dropped: stax below.)
wizards       ⬜  (Java-based document wizards)
xmerge        ⬜  (document format converter, Java)
javainstaller2 ⬜ (Java installer UI)
swext         ⬜  (Writer Java extensions e.g. mediawiki)
unodevtools   ⬜  (UNO component inspector, Java)

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

```

## Trunk backports
Periodically port upstream trunk fixes that bazel-migration lacks.  METHOD:
`git cherry bazel-migration origin/trunk <merge-base>` — `-` marks commits ALREADY
present by patch-id (our own win64 + sal fixes went upstream, so they show `-`;
never re-apply those), `+` marks real candidates.  Then `git cherry-pick -x` so the
upstream hash is recorded in the message.

BACKPORT #1 LANDED 2026-07-26 (branch trunk-backports off bazel-migration, 14 commits,
GREEN per user build).  Two groups:
  • cross-platform bug fixes: package ZipPackageFolder (HSQLDB doc corruption+crash
    regression, 7f52a288cb), package ZipFile bounds check, extensions/update crash+
    uninit-return, sc dbdocimp check, ww8scan missing includes, ww8graf invalid-enum
    removal, connectivity jdbc Tools.java cyclic-cause recursion (Java, inert until
    the Java bucket lands).
  • Damjan's Win64 SOURCE fixes, complementary to our x64 port (NOT duplicates):
    ADO driver types (2), unixODBC/OFunctiondefs types (2), dbase Min() types,
    avmedia IMediaEvent::GetEvent, and winaccessibility amd64 (7d7b15ea9e — changes
    PUBLIC XMSAAService.idl long→hyper; offapi + UAccCOM MIDL regen; merged cleanly
    with our existing x64 salframe.cxx edits).
  DEVIATION: 9d1a529d5e's macOS-only s5abi_macosx_x86-64/cpp2uno.cxx hunk was DROPPED
  (depends on upstream 11db77d7e7 macOS x86 bridge rework, not carried here); only its
  extensions/update files were taken.  Noted in that commit's body.

BACKPORT #2 LANDED 2026-07-29 (idxf DXF import filter, SQUASHED to one commit).
Unlike #1 this work ORIGINATED here: it was developed on the local branch
dxf-filter-fix using Bazel purely as the fast iteration harness, went upstream as
apache/openoffice#491 (merge c082241a9b, 2026-07-28), and was then brought back from
trunk.  LANDMINE for the `git cherry` method above: because it was squashed, the 11
upstream commits have NO patch-id match here and WILL show as `+` candidates
(999d0e5d3c 22079cd995 65c7e1576f e345bd9383 030749d53e 82ba6d6560 c4b51fde1e
5ed424c6ce 3931960203 05e2156ffd 96b0524be2).  Do NOT re-apply them — they are
already in, verified tree-identical to trunk's post-PR state; the list also lives in
the squash commit's body.  Source-only, 11 files:
main/filter/source/graphicfilter/idxf/* (scaling/extents, arc+ellipse+spline geometry,
LWPOLYLINE bulge, linetype dots, HATCH patterns, 3D OCS/WCS + VPORT, $DWGCODEPAGE
encoding) + main/svtools/source/filter/filter.cxx (DXF detector tolerates leading 999
comments).  Root causes per defect: main/filter/source/graphicfilter/idxf/Readme.md.
  DEVIATION: the PR's .gitignore hunk was DROPPED — bazel-migration's .gitignore
  already covers bazel-*/user.bazelrc/.vscode, and trunk's `*.lock` + whole-dir
  `.claude/` are wrong here (MODULE.bazel.lock is tracked).
  NOT carried to AOO42X/AOO41X yet.  The 13.5 MB Bugzilla sample corpus and the
  still-open cases (99892 Phase B encoding-chooser UI, text-dense thumbnail) live on
  the local branch bugfix-dxf-filter-open, deliberately out of this tree.

BACKPORT #3 LANDED 2026-08-02 (Damjan's Win64 type-correctness follow-ups, 4 commits,
GREEN on BOTH arches per user build).  These refine hunks ALREADY in this branch —
two of them (dllmgr, sfx2 Get10ThSec) are trunk rewriting OUR OWN upstreamed edits, so
skipping them would have left those files permanently divergent and re-flagged by every
`git cherry` run.  None fixes a live x64 defect (uInt32→uLong is a widening, IMR_* are
small constants, Windows clock_t is 32-bit long on both arches) — the value is drift
removal.  Source-only, 5 files: vcl salgdi.cxx ImplPreparePolyDraw nCurrPoint(s)
sal_uLong→sal_uInt32 (48f8cc8f69, completes 30f18d3fb8), vcl salframe.cxx drop the
static_cast<int>(wParam) on IMR_* (1e5d8e72b5), basic dllmgr.cxx guard
!defined(_WIN64)→defined(INTEL) (14a9b8c1d9 — equivalent here, the toolchain injects
INTEL/X86_64 globally, see build/toolchain/windows_cc_toolchain_config.bzl), sfx2
Get10ThSec() sal_uInt32→clock_t + SfxStatusIndicator::_nStartTime long→clock_t
(448dc86220; progress.cxx already includes <time.h> at line 54, no new include needed).
  NOT PORTED from the same trunk batch, because the Bazel side is ALREADY AHEAD:
  29ca9883b9 (link `version` for VerQueryValueW on x64) — vcl/BUILD.bazel already has
  version.lib; 25912db745 (Win64 vcl entrypoint) — we already select /ENTRY:LibMain on
  x64, and trunk's `/ENTRY:LibMain@16` looks WRONG: MSVC x64 drops the stdcall @N
  decoration, so the symbol from vcl/win/source/app/salshl.cxx is undecorated LibMain
  (that is what links and boots here).  Report upstream.
  ALSO NOT PORTED: f116fde0f6 + the packregistry half of 70996efe05 (ADO component path
  `drivers/ado`→`source/drivers/ado`, ADABAS platform condition) — both already correct
  in main/postprocess/BUILD.bazel, which resolves components by TARGET LABEL so the
  malformed path cannot exist in our form.  70996efe05's BFunctions.cxx half is a
  UNX/MACOSX-only refactor (WNT branch untouched) = no-op here.  The gbuild ports in
  that batch (connectivity 07bf101f83/5db65c3a88, chart2 89e8f0e82d/f1c404e081/
  93fbba6999 + 0a9d806e68 version-map workaround, officecfg ef56053619/323ac1e9a6/
  9e90d1d104, solenv Configuration.mk 431aafefb5/4dc45a5a3e, Ant jar sealing 861c48608f)
  have NO Bazel equivalent — Bazel replaces that layer.
  DRIFT TO WATCH (not actionable yet): 861c48608f also MOVES the DataAccess *.xcu out of
  connectivity into officecfg registry/data/org/openoffice/Office/DataAccess/ and adds
  *.xcs schemas for them; main/postprocess/BUILD.bazel still references the old
  //main/connectivity:source/drivers/*/*.xcu paths and would break on a future sync.
  d50e6300ed (drop -Xbootclasspath, unsupported on Java > 8) is inert now but is a real
  constraint for scripting/java when the Java bucket lands.

DO NOT PORT (VC9 island landmines — see the trunk drift audit above): boost 1.84
(2e60be5056), pyuno→Python 3 (27dee00ef8), C++11 floor (7ce5b5df31), autoconf 2.72,
the macOS/Apple-Silicon batch, GTK/iODBC/lld/icc-Clang16.

STILL OPEN — dmake-only trunk commits with NO cherry-pick equivalent, but whose Win64
facts our Bazel targets may be missing: crashrep must link psapi on x64 (7e339f4bc5),
OpenSSL lib names carry an `-x64` suffix (2cc1933ab8 + 6a1563a7a3 CPUNAME in SCPDEFS),
coinmp x64 build flags (23448c919c), NSS/NSPR win64 patch (22f8d3ac55).  These are a
BUILD-file audit, not a port.

## Goal
Replace the Perl/dmake/gmake orchestration layer with Bazel.
Primary constraint: eliminate the Cygwin dependency from the build.
Source code is NOT being changed — only the build system.

## Environment
- OS: Windows 11
- Shell during migration work: bash (via Cygwin, being phased out)
- Compiler: MSVC VS2008 (VC9) — both **x86** and **x64** (VC9 cross-tools); configured via user.bazelrc
- Bazel configured via MODULE.bazel and user.bazelrc

## Build strategy
- Third-party deps: wrap with rules_foreign_cc (cmake/make as appropriate)
- First-party modules: migrate to native Bazel cc_library/cc_binary
- Goal: eliminate gmake and dmake from first-party builds entirely
- rules_foreign_cc is a bridge for external code, not a destination

## Toolchain — dual-arch (32-bit x86 + 64-bit x64), VS2008/VC9
- **Two** custom MSVC toolchains at //build/toolchain, both registered in MODULE.bazel:
  `cc_toolchain_x86_vs2008_def` (x86, the DEFAULT) and `cc_toolchain_x64_vs2008_def`
  (x64, VC9 cross-tools: `x86_amd64` cl/ml64 + SDK v7.0 `lib\x64`).  x86 stays the
  default build and must remain green; x64 is the Win64 port (boots to the Start Center).
- Arch is chosen by **target platform**, not a stringly-typed flag.  Toolchain
  resolution auto-picks the matching cc_toolchain from the platform:
  `//build/platforms:winXP-x86` (→ @platforms//cpu:x86_32, default) vs `winXP-x64`
  (→ x86_64), each composed with the `//build/constraints:win_version` constraint
  (target space = {winxp, win10} × {x86, x64}; a future win10 target is a new
  constraint value, not a new config).
- `.bazelrc` convenience configs: `--config=winXP-x86` (default) / `--config=winXP-x64`,
  each pinning `--platforms` + a distinct `--platform_suffix` (separate bazel-out
  trees, no clobber) + `--symlink_prefix` (`bazel-winXP-x86-` / `bazel-winXP-x64-`).
- Per-arch defines are injected **globally by the toolchain** (not per-module):
  x86 → `_X86_=1`/`INTEL`/`CPPU_ENV=msci`, x64 → `_AMD64_=1`/`X86_64`/`CPPU_ENV=mscx`;
  `WIN32` stays defined on BOTH arches (the x64 compiler auto-defines `_WIN64`).
- windows_cc_toolchain_config.bzl: default_cpp_std disabled (no /std: flag), remove_unreferenced_code disabled (no /Zc:inline)
- tool_bin_path = VC\bin (not msvc_env_path) sets PATH for actions
- BAZEL_DO_NOT_DETECT_CPP_TOOLCHAIN=1 disables auto-detection

## Key conventions
- BUILD-file authoring conventions (module skeleton, naming, idioms, anti-patterns):
  see STYLEGUIDE.md.  The rules below are the operating/landmine notes; STYLEGUIDE.md
  is the copy-paste pattern reference for new modules.
- BUILD.bazel files live at main/<package>/BUILD.bazel (NOT prj/)
  - prj/ convention only worked for nmake() wrappers; cc_library needs
    glob() access to sources which requires the BUILD at module root
- build.lst drives dep graph: parse it to determine deps = []
- .tab files in textenc are #include'd data tables → use textual_hdrs
- All sal targets need deps = [":sal_headers", ":sal_pch"]
- Generated UNO headers: depend on //main/udkapi:udkapi_idl_headers
  (provides -I path to cppumaker output via includes = ["udkapi_idl_inc"])
- DLL export pattern: convert <name>.map GNU ld script → <name>.def Windows DEF,
  use win_def_file = "util/<name>.def", expose implib via filegroup output_group = "interface_library"
- KNOWN DIVERGENCE — component DLL naming: Bazel emits UNO component libs as bare
  <name>.dll where upstream/scp2 uses <name>.uno.dll (e.g. acceptor.dll vs
  acceptor.uno.dll, binaryurp/fastsax/expwrap/fpicker/fps_office/fsstorage/...).
  Harmless because the `.uno` infix has NO loader semantics: the SharedLibrary
  loader opens whatever name is registered in services.rdb, and both names are
  produced by the same Bazel pipeline so they stay consistent (proven by boot).
  Could only bite where a lib is named by a hardcoded string OUTSIDE generated
  services.rdb: remote-UNO/URP bootstrap (uno.exe -c acceptor), third-party .oxt
  manifests, or a deferred module's config referencing the canonical .uno.dll name
  → same "loading component library failed" class as fileacc/uui. To reach upstream
  parity (cosmetic): apply .uno infix in the link/staging step AND regenerate
  services.rdb together so both sides move in lockstep. Left as-is for the en-US demo.
- IDL pipeline: tools link /MD (dynamic CRT); stage msvcr90/msvcp90 DLLs +
  external .manifest files alongside EXEs (no mt.exe embedding); use
  ctx.actions.symlink (pure Bazel, no shell) for staging
- cppumaker output dir: prefix -O with "./" so osl's convertToFileUrl uses
  getAbsoluteFileURL (relative-to-workdir) instead of failing on relative paths
- never manipulate the cache directly, if you have to reset it use: bazel mod deps --lockfile_mode=refresh or ask the user for cleanup.
- add a migration summary to the module as readme.md
- let the user build
- update frontier after build is successful.
- XCU pipeline: LOCALIZED XCU files (those containing <value xml:lang="...">) must be pre-processed
  by alllang.xsl (no locale, no module) before being packed into XCDs — matches dmake XCU_DEFAULT
  pipeline in tg_config.mk.  Without this, configmgr crashes on empty xs:boolean values.
  Implemented via alllang_default_<xcu> genrules in officecfg/BUILD.bazel and _LOCALIZED_XCUS
  routing in postprocess.bzl's oc_xcu() function.  Both lists must stay in sync.
- forcedefault.xcd / ${PRODUCTLANGUAGE}: the ForceDefaultLanguage spool sets
  Linguistic/General/UILocale to the literal installer placeholder ${PRODUCTLANGUAGE},
  which the MSI (installer/languages.pm → $officestartlanguage) would substitute.
  Bazel staging never runs the MSI, so a genrule (postprocess/BUILD.bazel
  forcedefault_linguistic_xcu) substitutes it to en-US before pack_registry.
  Without it: first-start (langselect.cxx) copies the unexpanded placeholder into
  Setup/L10N/ooLocale, dp_resource.cxx::toLocale() throws "Invalid language string.",
  surfacing as '[context="user"] caught unexpected exception' + startup FatalError.
  When parameterize languages (see localization section) this literal becomes the knob.
  - uiconfig (UI config) MUST be staged as a FOLDER TREE at share/config/soffice.cfg/,
  not a zip.  framework PresetHandler (presethandler.cxx) opens soffice.cfg as a folder
  (FileSystemStorageFactory) and reads modules/<ModuleShortName>/<restype>/*.xml; dmake's
  scp2 ARCHIVE style extracts uiconfig.zip on install but Bazel skips that.  Missing folder
  ⇒ CorruptedUIConfigurationException ⇒ "error loading user interface configuration data"
  FatalError (Start Center = modules/startmodule loads first).  Implemented via uiconfig_tree
  rule in postprocess.bzl (//main/postprocess:uiconfig_tree) + staging _install_uiconfig;
  source <module>/uiconfig/<short>/... → modules/<short>/..., chart2 → modules/schart.

## Cross-cutting compiler flags & defines
These apply to many packages — check before building any new module:

- `/Zc:wchar_t-` — required for any module using `sal_Unicode`; VS2008 native `wchar_t`
  differs from `unsigned short`, breaking `Sequence<sal_Unicode>` / `cppu_detail_getUnoType`
- `snprintf=_snprintf` — VS2008 MSVCRT only exports `_snprintf`, not `snprintf`
- `snwprintf=_snwprintf` — same for the wide-char variant; needed by framework and potentially others
- `stlport` dep — required for modules using `boost::unordered_map` or `hash_map` via boost
- `Z_PREFIX` + `SYSTEM_ZLIB` — required for all zlib consumers (all symbols prefixed with `z_`)
- `/Imain/soltools/winunistd` — for modules that `#include <unistd.h>` unconditionally
- UNO component DEF exports: `component_getImplementationEnvironment`, `component_getFactory`,
  `component_canUnload` (standard unloadable component pattern for all future component DLLs)
- `CURL_STATICLIB` — required for any target using `@curl//:curl`; without it curl.h uses `__declspec(dllimport)` which breaks static linking
- MASM `.asm` files: list directly in `srcs`; toolchain `assemble` action uses `ml.exe` with
  `masm_flags` feature (`/c /coff /Cx`). `/Cx` is critical — without it MASM uppercases all
  PUBLIC symbols, breaking the link. Build-system define `SUPD=680` (Solar Update version).
- `_HAS_ITERATOR_DEBUGGING=0` — set GLOBALLY in build/toolchain/windows_cc_toolchain_config.bzl
  (default_compile_flags_list).  This is an STL-ABI macro: at =1 (the /MDd _DEBUG default) MSVC
  adds a _Container_proxy to std::vector/hash_map, changing layout.  It MUST be identical across
  every DLL or a container built in one (e.g. comphelper::SequenceAsHashMap, exported from
  comphelp.dll) and read inline in another (e.g. desktop/spl FirstStart::execute) indexes a
  garbage bucket → "vector subscript out of range" assert (debug only; release defaults to 0 so
  it never showed).  Cannot be =1: ~8 modules (comphelper, connectivity, sw, oox, fpicker, tools,
  svtools, pyuno) don't compile at =1 (heterogeneous comparators).  Global =0 matches upstream
  solenv; the per-module /D_HAS_ITERATOR_DEBUGGING=0 in those BUILD files are now redundant.
- rsc_res `images_root` MUST be `"main/default_images"` for every module that uses
  `Bitmap { File = "xxx.png" }` resources.  `BitmapEx(ResId)` reads the filename from .res and
  looks it up by exact name in `images.zip`.  `images.zip` uses `strip_prefix="main/default_images"`
  so entries are stored as `"framework/res/backing.png"`, `"res/odt_32.png"`, etc.  Setting
  `images_root` to a module-specific subdirectory (e.g. `"main/default_images/framework/res"`)
  stages images flat → .res stores bare basenames → exact-name lookup always fails → all
  `BitmapEx(ResId)` loads return empty bitmaps → Start Center shows no buttons or text.
## Localization / language builds (planned — not yet wired)
Current staging is en-US only.  This is the deliberate demo baseline; AOO must
start first.  When adding languages, keep THREE independent axes distinct
(conflating them is the trap) — dmake drives them from separate variables:

1. Installed locales (which UI languages are SELECTABLE) — dmake `alllangiso`/
   `WITH_LANG`, a LIST.  Each lang gets a Langpack-<lang>.xcd registering it in
   Setup/Office/InstalledLocales (+ fcfg_langpack_<lang>.xcd, registry_<lang>.xcd).
   Bazel today: only langpack_en_us_xcd.  Future: a macro over OOO_LANGS emitting
   one pack_registry per lang, each added to all_xcd via
   select({"//build:build_<code>": [...], "//conditions:default": []}) so
   --//build:lang_de=True auto-includes Langpack-de.xcd.  Mirrors the
   {$(alllangiso)} brace-expansion in postprocess/packregistry/makefile.mk.
2. Default UI language (the ONE start language) — dmake `PRODUCTLANGUAGE`, a
   SCALAR (installer/languages.pm $officestartlanguage; NOT the list).  This is
   the forcedefault.xcd ${PRODUCTLANGUAGE} substitution above.  Future: replace
   the hardcoded 'en-US' with a make-var, e.g. --define=office_start_lang=de
   (default en-US) read in the forcedefault_linguistic_xcu genrule cmd.
3. Localized content (the big lift) — per-lang .src/.res (rsc pipeline),
   localized .xcu (oc_xcu alllang spool), help, autotext, wordbook.  Same
   select()-per-lang pattern, but needs the TRANSLATION DATA wired in (see SDF).

Existing scaffolding (unconsumed): build/langs.bzl (OOO_LANGS, lang_id),
build/BUILD.bazel emits //build:lang_<code> bool_flags (en-US default True) and
//build:build_<code> config_settings.  user.bazelrc can flip lang_de/lang_fr;
nothing reads them yet.

### SDF files and the missing Pootle→SDF rule
Translation data flows as SDF (a.k.a. GSI) — the build's merge-database format,
one TAB-delimited line per translatable string
(project\path\file\type\gid\lid\helpid\platform\width\langid\text\...).
- Consumption (merge): l10ntools `transex`/export.cxx and friends read a source
  file + an SDF via `-m <sdf> -l <lang>` and emit the localized resource
  (the merge step before rsc compiles .src→.res, and for helpex/cfgex/xrmex).
  l10ntools localize.cxx does the reverse — EXTRACTS source strings into one
  merged .sdf (POT-equivalent) for translators.
- The gap: Pootle stores translations as .po (per lang/module).  AOO's build
  consumes .sdf, NOT .po.  The .po↔.sdf bridge is translate-toolkit's
  oo2po (sdf→po, to seed/update Pootle) and po2oo (po→sdf, to feed the build).
  Neither translate-toolkit nor any po/sdf conversion exists in this tree
  (grep: no oo2po/po2oo/po2sdf).  So axis 3 needs a NEW Bazel rule that runs
  po2oo over the Pootle .po export to (re)generate the per-lang .sdf the merge
  tools expect — this rule does not exist yet and is a prerequisite for any
  real (translated) language build.  Until then, language builds can only do
  axes 1+2 (German/French DEFAULT UI, but strings still English).

# dependency notes
icu - ext_libraries\modules\icu\Readme.md
redland / raptor2 / rasqal (RDF stack, native-cc static) + unordf.dll - rdf-readme.md
  (the modules\<lib>\README files are the original autotools docs; the Bazel
   migration notes — overlay mechanism, *_INTERNAL/HAVE_CONFIG_H defines,
   local_defines vs defines, *_STATIC, RAPTOR_WWW_NONE/S_ISREG — are in rdf-readme.md)
rhino - ext_libraries\modules\rhino\README
sal_pch - was merged with sal_headers
## Out of scope
- Modifying source code
