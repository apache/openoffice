###############################################################################
# uno_junit_test — a JUnit suite that drives a real office over UNO.
#
# The Java counterpart of gtest_test(office_connection = True).  Both implement
# the SAME fixture — launch soffice with -accept=…;urp, resolve a component
# context over it, terminate it afterwards — but from opposite sides:
#
#   C++   test::OfficeConnection      reads "arg-soffice" / "arg-user" through
#         (main/test/source/cpp)      rtl::Bootstrap, i.e. the ENVIRONMENT
#   Java  org.openoffice.test.        reads "org.openoffice.test.arg.soffice" /
#         OfficeConnection            "…arg.user" through System.getProperty,
#         (main/test/java)            i.e. -D on the java command line
#
# and they differ in transport: the C++ side uses a socket, the Java side a
# NAMED PIPE (pipe,name=oootest<uuid>), which is why this rule needs no
# port bookkeeping and no `exclusive` tag — concurrent runs cannot collide.
#
# The recipe ported here is solenv/inc/installationtest.mk's `javatest` rule
# (identical in solenv/gbuild/JunitTest.mk), which is:
#
#     rm -rf <user> && mkdir -p <user>
#     java -cp "<junit>;<classpath>" \
#          -Dorg.openoffice.test.arg.soffice=path:<soffice> \
#          -Dorg.openoffice.test.arg.user=file:///<user> \
#          <-Dorg.openoffice.test.arg.testarg.*> \
#          org.junit.runner.JUnitCore <classes>
#     rm -rf <user>
#
# Usage:
#   load("//build/rules:junit_test.bzl", "uno_junit_test")
#   uno_junit_test(
#       name = "qa_complex_junitskeleton",
#       srcs = glob(["qa/complex/junitskeleton/*.java"]),
#       classes = ["complex.junitskeleton.Skeleton"],
#       deps = [":OOoRunner", "//main/test:test_jar", ...],
#       data_tree = {"//main/qadevOOo:…README.txt": "test_documents/README.txt"},
#       uno_install = "//main/staging:install",
#   )
###############################################################################

"""JUnit suites that drive a staged office over UNO (see header above)."""

load("@rules_java//java:defs.bzl", "java_library")
load("@rules_java//java/common:java_info.bzl", "JavaInfo")
load("//build:jre.bzl", "jre_home_native", "jre_java_exe")
load(":gtest_test.bzl", "launcher_expand_tokens", "launcher_relpath")

_JUNIT = "//build/third_party/junit:junit"
_RUNNER = "org.junit.runner.JUnitCore"

def _uno_junit_test_impl(ctx):
    d = ctx.label.name + ".run"
    staged = []

    # ── classpath ───────────────────────────────────────────────────────────
    # Every runtime jar staged flat beside the launcher, so the classpath is a
    # list of names under one directory the launcher locates with %~dp0 — the
    # same self-locating trick gtest_test uses, and for the same reason: `bazel
    # test` sets the working directory to neither the execroot nor the staged
    # dir, so nothing relative to %CD% resolves.
    jars = depset(transitive = [
        dep[JavaInfo].transitive_runtime_jars
        for dep in ctx.attr.runtime_deps
    ]).to_list()

    cp_names = []
    seen = {}
    for j in jars:
        name = j.basename
        if name in seen:
            if seen[name] == j.path:
                continue

            # Two different jars with one basename: Bazel names a java_library's
            # output after the TARGET, and target names are only unique within a
            # package.  Disambiguate rather than silently dropping one, which
            # would surface as a NoClassDefFoundError far from the cause.
            name = str(len(seen)) + "_" + name
        seen[name] = j.path
        o = ctx.actions.declare_file(d + "/" + name)
        ctx.actions.symlink(output = o, target_file = j)
        staged.append(o)
        cp_names.append(name)

    # ── fixture files ───────────────────────────────────────────────────────
    # Staged at a chosen path, like gtest_test's data_tree: a test document is
    # reached as new File("test_documents", name) relative to the working
    # directory, so the SHAPE matters, not just co-location.
    for label, rel in ctx.attr.data_tree.items():
        files = label[DefaultInfo].files.to_list()
        want = rel.split("/")[-1]
        if len(files) > 1:
            files = [f for f in files if f.basename == want]
        if len(files) != 1:
            fail(("data_tree entry %s → %s: expected one file, got %d. Give the " +
                  "staged path the same basename as the artifact you want.") %
                 (label.label, rel, len(files)))
        o = ctx.actions.declare_file(d + "/" + rel)
        ctx.actions.symlink(output = o, target_file = files[0])
        staged.append(o)

    # ── the staged office ───────────────────────────────────────────────────
    uno_program_dir = None
    for f in ctx.files.uno_install:
        if f.path.endswith("/program/fundamental.ini"):
            uno_program_dir = f.dirname
            break
    if uno_program_dir == None:
        fail("uno_junit_test needs uno_install = //main/staging:install — the " +
             "office the suite launches, and the jpipe.dll its JVM loads, both " +
             "come from the staged program/.")

    launcher = ctx.actions.declare_file(d + "/" + ctx.label.name + "_run.bat")
    lines = [
        "@echo off",
        "setlocal",
        'for %%I in ("%~dp0.") do set "_RUN=%%~fI"',
        'set "_RUNU=%_RUN:\\=/%"',

        # The per-run user installation.  javatest wipes it before AND after, so
        # a run that died without cleaning up cannot hand its state to the next
        # one — a stale user profile is exactly what makes an office failure
        # unreproducible.
        'set "_SCRATCH=%TEST_TMPDIR%\\scratch"',
        'if "%TEST_TMPDIR%"=="" set "_SCRATCH=%_RUN%\\scratch"',
        'set "_SCRATCH=%_SCRATCH:/=\\%"',
        'if exist "%_SCRATCH%" rmdir /s /q "%_SCRATCH%"',
        'mkdir "%_SCRATCH%" || exit /b 1',
        'set "_SCRATCHU=%_SCRATCH:\\=/%"',
        'for %%I in ("%~dp0' + launcher_relpath(launcher.dirname, uno_program_dir) +
        '") do set "_PROG=%%~fI"',
        'set "_PROGU=%_PROG:\\=/%"',

        # jpipe.dll (the JNI half of jurt's named-pipe connection) is loaded by
        # System.loadLibrary from the TEST's JVM, and java.library.path includes
        # PATH on Windows.  The office itself needs nothing from PATH — it lives
        # in program/, whose own directory the loader searches first.
        'set "PATH=%_PROG%;%PATH%"',

        # OfficeConnection always passes -env:UNO_JAVA_JFW_ENV_JREHOME=true to
        # the office, which makes ITS jvmfwk take JAVA_HOME.  Pin it to the
        # arch-matched JDK: inheriting the developer's shell value would hand a
        # 32-bit office the 64-bit JDK it cannot load, and only for the tests
        # that actually use office-side Java, i.e. intermittently.
        'set "JAVA_HOME=' + ctx.attr.java_home + '"',

        # Non-product builds route tools-library assertions to a MESSAGE BOX by
        # default.  A modal dialog in a launched office is invisible under
        # `bazel test` and hangs the run until the timeout kills it; javatest
        # exports the same value for the same reason.
        'set "DBGSV_ERROR_OUT=shell"',

        # The working directory is the staged dir, not program/ (the C++ default):
        # a Java suite reaches its fixture documents by relative path, and unlike
        # the C++ side it needs nothing from program/ as a working directory.
        'cd /d "%~dp0" || exit /b 1',
    ]

    if ctx.attr.fixture_starts_office:
        # ── the office is the FIXTURE's, not the test's ──────────────────────
        # `-Dorg...soffice=connect:<desc>` makes OfficeConnection ATTACH to an
        # office someone else started (`path:` makes it launch one). Then
        # `process` stays null, and its tearDown — which is what calls
        # XDesktop.terminate() and blocks in Process.waitFor() until the office
        # exits — becomes a no-op.
        #
        # That is the point. A suite that leaves the office in a state where it
        # will not shut down otherwise hangs until the test timeout, with its own
        # assertions long since green. Moving the office's lifetime into the
        # launcher makes the verdict the JUnit result, and makes the shutdown a
        # `taskkill`. It is the same division of labour as `server_args` in
        # gtest_test (bridgetest_urp).
        #
        # It is a DEVIATION from installationtest.mk::javatest, and it costs the
        # one check tearDown performs — that the office terminates cleanly and
        # exits 0. So it is opt-in, not the default: use it only for a suite that
        # cannot run without it, and only when the shutdown failure is understood
        # and recorded. Suites that shut down cleanly must keep proving it.
        #
        # The pipe name has to be unique per run (two of these must not attach to
        # each other's office) and has to be recoverable afterwards to kill the
        # right process — %RANDOM% twice, then matched against the command line.
        lines += [
            'set "_PIPE=oootest%RANDOM%%RANDOM%"',
            'start "" /b "%_PROG%\\soffice.exe" -quickstart=no -nofirststartwizard' +
            ' -norestore "-accept=pipe,name=%_PIPE%;urp"' +
            ' "-env:UserInstallation=file:///%_SCRATCHU%"' +
            ' -env:UNO_JAVA_JFW_ENV_JREHOME=true',
            # Readiness, the pipe analogue of gtest_test's server_ready_port.
            # Not optional: OfficeConnection's resolve loop has NO sleep when
            # `process` is null (it only waits on the process it STARTED), so a
            # premature start would spin a core until the office came up.
            #
            # osl's Windows pipes appear in the pipe device namespace as
            # \\.\pipe\OSL_PIPE_<SID>_<name>, so match the NAME at the end — the
            # SID part is per-user.  The whole wait is ONE PowerShell process
            # rather than a batch loop for two reasons: cmd cannot list that
            # namespace at all (`dir \\.\pipe\` answers "invalid parameter" —
            # it is a device path, not a directory), and re-launching PowerShell
            # per iteration would cost more than the office's own startup.
            'powershell -NoProfile -Command "$n = $env:_PIPE; for ($i = 0; ' +
            "$i -lt 120; $i++) { if ([System.IO.Directory]::GetFiles('\\\\.\\pipe\\') " +
            "-match ('_' + $n + '$')) { exit 0 }; Start-Sleep -Milliseconds 500 }; " +
            'exit 1"',
            "if not errorlevel 1 goto _pipeup",
            'echo ERROR: office never accepted on pipe %_PIPE% 1>&2',
            "exit /b 1",
            ":_pipeup",
        ]

    # "path:" makes OfficeConnection LAUNCH an office; "connect:<desc>" attaches
    # to the one the launcher started (see fixture_starts_office).  NOTE the
    # connect form is NOT token-expanded: it names a launcher variable directly,
    # and _expand_tokens doubles every literal '%' for cmd.
    soffice_arg = ("connect:pipe,name=%_PIPE%" if ctx.attr.fixture_starts_office
                   else launcher_expand_tokens("path:$(PROGRAM)\\soffice.exe"))
    args = [
        "-Dorg.openoffice.test.arg.soffice=" + soffice_arg,
        # A file:/// URL, NOT the native path the C++ side takes: Java passes it
        # straight to -env:UserInstallation=, which wants a URL.
        "-Dorg.openoffice.test.arg.user=" +
        launcher_expand_tokens("file:///$(SCRATCH_URL)"),
    ]
    args += ['-D%s=%s' % (k, launcher_expand_tokens(ctx.attr.test_args[k]))
             for k in sorted(ctx.attr.test_args)]

    cp = ";".join(["%_RUN%\\" + n for n in cp_names])
    lines += [
        '"%s" %s -cp "%s" %s %s %s %%*' % (
            ctx.attr.java_exe,
            " ".join(ctx.attr.jvm_flags),
            cp,
            " ".join(['"%s"' % a for a in args]),
            _RUNNER,
            " ".join(ctx.attr.classes),
        ),
        # JUnitCore's main() exits 1 on any failure, so the verdict needs no
        # wrapper.  Capture it before the cleanup clobbers ERRORLEVEL.
        'set "_RC=%ERRORLEVEL%"',
    ]

    if ctx.attr.fixture_starts_office:
        # Kill OUR office and nothing else.  `taskkill /im soffice.exe` would
        # take down a developer's session and any concurrent suite; the unique
        # pipe name is on the command line, so match on that.  Ignore failure —
        # a suite whose tearDown DID manage to terminate leaves nothing to kill.
        #
        # No nested double quotes anywhere in the PowerShell: cmd would eat them.
        # The pipe name is read from the ENVIRONMENT ($env:_PIPE), which is also
        # why nothing here needs %-escaping.
        lines += [
            'powershell -NoProfile -Command "Get-CimInstance Win32_Process | ' +
            "Where-Object { $_.Name -eq 'soffice.exe' -and $_.CommandLine -like " +
            "('*' + $env:_PIPE + '*') } | ForEach-Object { Stop-Process -Id " +
            '$_.ProcessId -Force }" >nul 2>nul',
        ]

    lines += [
        'rmdir /s /q "%_SCRATCH%" 2>nul',
        "exit /b %_RC%",
        "",
    ]

    ctx.actions.write(output = launcher, content = "\r\n".join(lines), is_executable = True)
    staged.append(launcher)

    return [DefaultInfo(
        executable = launcher,
        runfiles = ctx.runfiles(files = staged + ctx.files.uno_install),
        files = depset([launcher]),
    )]

_uno_junit_test = rule(
    implementation = _uno_junit_test_impl,
    test = True,
    attrs = {
        "runtime_deps": attr.label_list(providers = [[JavaInfo]]),
        "classes": attr.string_list(mandatory = True),
        "test_args": attr.string_dict(),
        "jvm_flags": attr.string_list(),
        "uno_install": attr.label(allow_files = True, mandatory = True),
        "data_tree": attr.label_keyed_string_dict(allow_files = True),
        "java_exe": attr.string(mandatory = True),
        "java_home": attr.string(mandatory = True),
        "fixture_starts_office": attr.bool(default = False),
    },
)

def uno_junit_test(
        name,
        srcs,
        classes,
        deps = [],
        uno_install = "//main/staging:install",
        data_tree = {},
        test_args = {},
        jvm_flags = [],
        fixture_starts_office = False,
        javacopts = ["--release", "8", "-XepDisableAllChecks"],
        size = "medium",
        **kwargs):
    """A JUnit suite run against a freshly launched office.

    srcs / deps / javacopts: compiled into one java_library. JUnit itself is
    added automatically — every one of these suites uses org.junit.Assert, and
    org.openoffice.test.OfficeConnection does too. javacopts defaults to the
    legacy-source settings the rest of the tree uses (these are Java 1.4/5-era
    sources; modern javac compiles them with warnings only).

    classes: fully-qualified test classes handed to JUnitCore — upstream's
    JAVATESTFILES, i.e. only the files that carry an @Test.

    test_args: extra "org.openoffice.test.arg.*" system properties, e.g. the
    `sce`/`xcl` pair the qadevOOo unoapi runner takes. Values expand $(RUNDIR),
    $(SCRATCH), $(PROGRAM) and the _URL form of each, exactly as in gtest_test —
    which is how a value can name a staged fixture file.

    data_tree: {label: "relative/staged/path"} fixture files. The test runs with
    its working directory set to the staged dir, so a suite that does
    new File("test_documents", …) finds them.

    fixture_starts_office: the LAUNCHER starts and kills the office, and the
    test attaches to it (`connect:`) instead of launching it (`path:`). Use this
    only for a suite that leaves the office unable to shut down, which otherwise
    hangs in OfficeConnection.tearDown()'s Process.waitFor() until the test
    timeout with all its own assertions already green. It costs the one check
    tearDown performs — that the office terminates cleanly and exits 0 — so it
    is opt-in and each use must record WHY the shutdown fails.

    size defaults to "medium" (300s): this fixture BOOTS AN OFFICE, always cold
    because the user installation is recreated per run, and OfficeConnection's
    resolve loop is unbounded — the timeout is the only thing that ends a run
    where the office never comes up. Do not drop it to "small".
    """
    java_library(
        name = name + "_lib",
        srcs = srcs,
        deps = deps + [_JUNIT],
        javacopts = javacopts,
        testonly = True,
    )
    _uno_junit_test(
        name = name,
        runtime_deps = [":" + name + "_lib"],
        classes = classes,
        test_args = test_args,
        jvm_flags = jvm_flags,
        fixture_starts_office = fixture_starts_office,
        uno_install = uno_install,
        data_tree = data_tree,
        java_exe = jre_java_exe(),
        java_home = jre_home_native(),
        size = size,
        **kwargs
    )
