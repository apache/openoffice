###############################################################################
# gtest_test — runnable GoogleTest target for the /MD MSVC toolchain.
#
# Problem: the custom VS2008 toolchain links /MD and embeds NO manifest, so a
# bare cc_test .exe cannot launch under `bazel test`:
#   * its runtime DLLs (sal3.dll, …) and the VC90 CRT (msvcr90.dll) are not
#     beside the exe — `data` deps land in runfiles SUBDIRS, which the Windows
#     loader does not search;
#   * loose msvcr90.dll with no activation context raises R6034.
#
# Solution (the test analog of //main/idl:svidl_bundle): stage the exe + all
# runtime DLLs + the VC90 CRT assembly + an external "<exe>.manifest" into ONE
# flat directory via ctx.actions.symlink, and make THAT staged exe the test
# executable.  Co-located DLLs are found; the external manifest supplies the
# CRT activation context (no R6034).
#
# Usage:
#   load("//build/rules:gtest_test.bzl", "gtest_test")
#   gtest_test(
#       name = "o3tl_test",
#       srcs = glob(["qa/*.cxx", "qa/*.hxx"]),
#       copts = [...], defines = [...],
#       deps = [":o3tl", "//main/sal:sal_headers", "//main/stlport:stlport"],
#       additional_linker_inputs = ["//main/sal:sal_implib"],
#       linkopts = ["$(execpath //main/sal:sal_implib)"],
#       runtime_dlls = ["//main/sal:sal3"],   # DLLs the exe loads at run time
#   )
###############################################################################

"""Runnable GoogleTest targets for the /MD MSVC toolchain (see header above)."""

load("@rules_cc//cc:defs.bzl", "cc_binary")

_CRT = "//main/external/msvcp90:crt_dlls"
_APP_MANIFEST = "//main/external/msvcp90:vc90_app_manifest"

# VC90-CRT manifest compiled to a .res, linked into every test exe so it sits in
# the image at RT_MANIFEST id 1.  The staged external <exe>.manifest below only
# covers DLLs bound at process start; a test that osl_loadModule()s a UNO DLL
# later (any cppu Mapping pulls in the C++/UNO bridge on demand) needs the CRT
# activation context to still apply at that point, or the late load resolves
# msvcr90 loose → R6034 → DllMain fails → exit 0xC0000142.
_APP_MANIFEST_RES = "//main/external/msvcp90:vc90_app_manifest_res"

def _windows_relpath(from_dir, to_dir):
    """Backslash relative path from one execroot-relative dir to another.

    Used to make the launcher .bat self-locating via %~dp0 instead of trusting
    the working directory: `bazel test` does NOT run tests with the working
    directory set to the execroot, so a %CD%-relative path to the staged install
    resolves to nothing ("The system cannot find the path specified").
    """
    f = from_dir.split("/")
    t = to_dir.split("/")
    common = 0
    for i in range(min(len(f), len(t))):
        if f[i] != t[i]:
            break
        common = i + 1
    return "..\\" * (len(f) - common) + "\\".join(t[common:])

# Tokens usable in `env` values and `prerun` command lines.  They stand for
# paths only knowable at run time, and expand to launcher variables:
#   $(RUNDIR)      the staged directory (native, no trailing separator)
#   $(PROGRAM)     the staged install's program/ dir (needs uno_install)
#   $(SCRATCH)     a fresh, empty, WRITABLE per-run directory (native)
# and a forward-slash variant of each, for building a `file:///` URL:
#   $(RUNDIR_URL)  $(PROGRAM_URL)  $(SCRATCH_URL)
# Everything a test writes belongs under $(SCRATCH): the staged dir lives in
# bazel-out and must be treated as read-only build output.
#
# Prefer the _URL forms wherever the consumer takes a URL — UNO bootstrap
# variables all do.  Forward slashes sidestep the backslash-escaping trap in
# rtl::Bootstrap entirely (see the arg-user comment further down).
_TOKENS = [
    # The _URL forms MUST come first — otherwise the plain token matches their
    # prefix and leaves a stray "_URL" behind.
    ("$(SCRATCH_URL)", "%_SCRATCHU%"),
    ("$(RUNDIR_URL)", "%_RUNU%"),
    ("$(PROGRAM_URL)", "%_PROGU%"),
    ("$(SCRATCH)", "%_SCRATCH%"),
    ("$(RUNDIR)", "%_RUN%"),
    ("$(PROGRAM)", "%_PROG%"),
]

def _expand_tokens(s):
    # A literal '%' has to be doubled FIRST, before the tokens below introduce
    # the launcher's own %VAR% references (which must stay single).
    #
    # LANDMINE: '%' is a metacharacter in a .bat, and a percent-DIGIT sequence is
    # the worst case because it fails silently — cmd reads %2 as the script's
    # second argument, which is empty, and drops it.  A %20-escaped file URL is
    # exactly that shape, so
    #     file:///C:/Program%20Files%20(x86)/…
    # reaches the test as
    #     file:///C:/Program0Files0(x86)/…
    # with no error anywhere.  It cost a debugging session via
    # UNO_JAVA_JFW_JREHOME, where the only symptom was jvmfwk reporting the JRE
    # "could not be recognized".
    s = s.replace("%", "%%")
    for token, var in _TOKENS:
        s = s.replace(token, var)
    return s

def _uses_token(strings, token):
    for s in strings:
        if s.find(token) != -1:
            return True
    return False

def _env_lines(env):
    # sorted() so the launcher is byte-identical across analyses (a dict's
    # iteration order is stable in Starlark, but the attr comes from a BUILD file
    # where the author's ordering carries no meaning).
    return ['set "%s=%s"' % (k, _expand_tokens(env[k])) for k in sorted(env)]

def _arg_string(run_args):
    # Quoted so an argument containing a space survives cmd's tokenizing, and
    # token-expanded like `env` values are — a fixed command line routinely has
    # to name a staged file, which is only locatable at run time.
    return "".join([' "%s"' % _expand_tokens(a) for a in run_args])

def _prerun_lines(prerun):
    # `|| exit /b 1` on every one: a fixture that failed to build must fail the
    # test loudly, not leave the suite to report a confusing downstream error.
    out = []
    for cmd in prerun:
        out.append(_expand_tokens(cmd) + " || exit /b 1")
    return out

def _server_lines(server_exe, server_args, port):
    """Start the background server, then wait until it is actually listening.

    `start "" /b` detaches without a console window; the empty first argument is
    the window TITLE, and it has to be there — otherwise cmd reads the quoted
    exe path as the title and there is nothing left to run.  The server inherits
    the environment set above it (URE_BOOTSTRAP, PATH) and the working
    directory, which is what makes its relative `-ro` registry names resolve.
    """
    lines = [
        'start "" /b "%%~dp0%s"%s' % (server_exe, _arg_string(server_args)),
    ]
    if not port:
        return lines

    # Without this the test is a race the client usually loses: the server has
    # to load the whole UNO stack before it calls accept(), which takes about a
    # second, and the client's UnoUrlResolver::resolve() does ONE connect() and
    # throws NoConnectException if nothing is listening yet.
    #
    # netstat rather than a connect probe, because a probe that succeeds has
    # consumed the single connection --singleaccept will serve.
    #
    # LANDMINE: netstat's STATE column is LOCALIZED — on a German Windows the
    # listening row reads "ABHÖREN", not "LISTENING", so matching the state word
    # makes the wait time out on every run for reasons that have nothing to do
    # with the test.  Match the FOREIGN ADDRESS instead: a listening socket is
    # the only kind whose peer is 0.0.0.0:0, and that is a number, not a word.
    # Both patterns have to hit the SAME line, which the pipeline gives for free.
    #
    # The trailing space after the port is load-bearing too: without it ":2002"
    # also matches ":20020".
    #
    # `ping -n 2` is the portable batch sleep (~1s).  timeout.exe is the modern
    # spelling but aborts with "Input redirection is not supported" whenever
    # stdin is not a console, which under `bazel test` it never is.
    return lines + [
        "set /a _TRY=0",
        ":_waitport",
        'netstat -an | findstr /c:"127.0.0.1:%s " | findstr /c:"0.0.0.0:0" >nul' % port,
        "if not errorlevel 1 goto _portup",
        "set /a _TRY+=1",
        # ~30s: the server needs about one to reach accept(), so this is pure
        # headroom, and it has to stay well inside the "small" 60s test budget
        # or bazel's timeout would preempt the readable error below.
        "if %_TRY% GEQ 30 goto _porttimeout",
        "ping -n 2 127.0.0.1 >nul",
        "goto _waitport",
        ":_porttimeout",
        'echo ERROR: server never listened on 127.0.0.1:%s 1>&2' % port,
        'taskkill /f /im "%s" >nul 2>nul' % server_exe,
        "exit /b 1",
        ":_portup",
    ]

# ── shared with junit_test.bzl ───────────────────────────────────────────────
# The Java/UNO tests emit a launcher .bat of their own and need exactly these two
# primitives: a %~dp0-relative path to the staged install (bazel test's working
# directory is neither the execroot nor the exe's directory), and the escaping
# rules for a value baked into a .bat (the percent landmine above).  Re-exported
# rather than copied, so both launchers keep one definition of each.
launcher_relpath = _windows_relpath
launcher_expand_tokens = _expand_tokens

def _staged_gtest_test_impl(ctx):
    # The staging dir is normally "<name>.run".  bin_layout makes it
    # "<name>.run/bin" instead — the ONE thing the child-process suites need.
    # They locate their helper exe with the testshl2-era idiom
    #     dir-of(own module) → strip last path component → + "bin"
    # (osl_process.cxx::getExecutablePath, rtl_Process/rtl_Bootstrap::
    # getModulePath).  Under dmake both parent and child lived in solver/bin, so
    # that round-trip was the IDENTITY; it only looks like an "../bin" lookup.
    # Naming the staging dir "bin" reproduces the identity exactly, so the
    # helpers staged beside the exe are found with no source change.
    d = ctx.label.name + ".run" + ("/bin" if ctx.attr.bin_layout else "")
    staged = []
    seen = {}

    def stage(f, name = None):
        name = name or f.basename
        if name in seen:
            return
        seen[name] = True
        o = ctx.actions.declare_file(d + "/" + name)
        ctx.actions.symlink(output = o, target_file = f)
        staged.append(o)

    # The gtest exe, renamed to the test target's name.
    staged_exe = ctx.actions.declare_file(d + "/" + ctx.label.name + ".exe")
    ctx.actions.symlink(output = staged_exe, target_file = ctx.executable.binary)
    seen[staged_exe.basename] = True
    staged.append(staged_exe)

    # Runtime DLLs + VC90 CRT (msvcr90/msvcp90/msvcm90 + Microsoft.VC90.CRT.manifest).
    for f in ctx.files.runtime:
        stage(f)

    # Companion helper exes (e.g. a child process the test spawns by name).
    # Staged under their own basename + a matching <name>.exe.manifest so they
    # too satisfy the VC90 CRT activation context when launched.
    for c in ctx.attr.companions:
        cexe = c[DefaultInfo].files_to_run.executable
        co = ctx.actions.declare_file(d + "/" + cexe.basename)
        ctx.actions.symlink(output = co, target_file = cexe)
        staged.append(co)
        cman = ctx.actions.declare_file(d + "/" + cexe.basename + ".manifest")
        ctx.actions.symlink(output = cman, target_file = ctx.file.app_manifest)
        staged.append(cman)

    # External application manifest, named <exe>.manifest, declaring the VC90
    # CRT dependency so the loader builds an activation context (no R6034).
    man = ctx.actions.declare_file(d + "/" + ctx.label.name + ".exe.manifest")
    ctx.actions.symlink(output = man, target_file = ctx.file.app_manifest)
    staged.append(man)

    # A client/server test runs the SAME binary twice, so the background half is
    # staged a second time under its own name.  That is not cosmetic: it is what
    # makes the cleanup `taskkill /f /im` precise.  Killing by the shared image
    # name would take the client — and any concurrently running test using the
    # same tool — down with it.
    server_exe_name = ctx.label.name + "_server.exe"
    if ctx.attr.server_args:
        srv = ctx.actions.declare_file(d + "/" + server_exe_name)
        ctx.actions.symlink(output = srv, target_file = ctx.executable.binary)
        staged.append(srv)
        srvman = ctx.actions.declare_file(d + "/" + server_exe_name + ".manifest")
        ctx.actions.symlink(output = srvman, target_file = ctx.file.app_manifest)
        staged.append(srvman)

    # data_tree — fixture files staged at an EXPLICIT relative path rather than
    # flat by basename (which is all `runtime` can express).  Needed whenever the
    # shape of the tree is what is under test: a mini UNO installation resolves
    # $ORIGIN against the ini's own directory, so basis/program/uno.ini has to be
    # exactly there, and two files in it are both named "bootstrap.ini".
    for label, rel in ctx.attr.data_tree.items():
        files = label[DefaultInfo].files.to_list()
        want = rel.split("/")[-1]
        if len(files) > 1:
            # Several rules legitimately return more than the one artifact you
            # want (services_rdb also emits services.input; idl_library emits
            # its .rdb plus a header directory).  Disambiguate by the basename
            # you asked to stage it AS, which is unambiguous and needs no extra
            # filegroup boilerplate at every call site.
            files = [f for f in files if f.basename == want]
        if len(files) != 1:
            fail(("data_tree entry %s → %s: expected one file, got %d. Give " +
                  "the staged path the same basename as the artifact you " +
                  "want, or point at a target that provides a single file.") %
                 (label.label, rel, len(files)))
        o = ctx.actions.declare_file(d + "/" + rel)
        ctx.actions.symlink(output = o, target_file = files[0])
        staged.append(o)

    # ── UNO environment (subsequent / in-process-bootstrap tests) ────────────
    # A test that calls cppu::defaultBootstrap_InitialComponentContext() needs a
    # real UNO installation: type + service rdbs, and every component DLL named
    # in services.rdb.  Rather than reinvent that, point it at the staged office
    # via URE_BOOTSTRAP, the documented override for "which fundamental.ini
    # describes this installation".  fundamental.ini resolves ${ORIGIN} against
    # its OWN directory, so that one variable transitively supplies UNO_TYPES,
    # UNO_SERVICES, URE_INTERNAL_LIB_DIR and BRAND_BASE_DIR — no need to
    # duplicate any of them here, and no drift when the ini changes.
    #
    # The install root is a fixed bazel-out path (tree_install declares its
    # outputs in //main/staging), so locating program/fundamental.ini among the
    # install files at analysis time yields the execroot-relative program dir.
    # The launcher then reaches it from its OWN location (%~dp0) — see
    # _windows_relpath; never from %CD%, which is not the execroot.
    uno_program_dir = None
    for f in ctx.files.uno_install:
        if f.path.endswith("/program/fundamental.ini"):
            uno_program_dir = f.dirname
            break
    if ctx.files.uno_install and uno_program_dir == None:
        fail("uno_install does not contain program/fundamental.ini — is it //main/staging:install?")

    # System32 SHADOWS one of our staged DLLs, and putting program/ on PATH does
    # NOT beat it: the Windows loader searches System32 *before* the working
    # directory and PATH.  Windows ships its own C:\Windows\System32\icuuc.dll (a
    # ~36 KB stub) while AOO bundles ICU 49.1.2 (~1.3 MB); everything importing
    # icuuc — sw.dll, sfx.dll, … — then binds the system one, whose exports do
    # not match, and the process dies at load with STATUS_ENTRYPOINT_NOT_FOUND
    # (0xC0000139) and an EMPTY test log.  soffice.exe is immune only because it
    # lives in program/ next to our copy, and the exe's own directory IS searched
    # first.  So mirror that: co-locate ICU with the test exe.
    # icuuc.dll is the only one of the 248 staged DLLs that collides; the other
    # two come along to keep the loaded ICU coherent.
    if uno_program_dir:
        for f in ctx.files.uno_install:
            if f.dirname == uno_program_dir and f.basename in [
                "icuuc.dll",
                "icui18n.dll",
                "icudata.dll",
            ]:
                stage(f)

    if ctx.attr.office_connection and not uno_program_dir:
        fail("office_connection requires uno_install — test::OfficeConnection " +
             "bootstraps an in-process context to build the URL resolver, and " +
             "the soffice it launches comes from the staged install.")

    # A launcher .bat is needed for anything the bare exe cannot express itself:
    # a working directory, an environment, or a fixture built at run time.
    env = ctx.attr.env
    prerun = ctx.attr.prerun
    run_args = ctx.attr.run_args
    server_args = ctx.attr.server_args
    argstr = _arg_string(run_args)
    srvlines = (_server_lines(server_exe_name, server_args, ctx.attr.server_ready_port)
                if server_args else [])
    values = env.values() + prerun + run_args + server_args
    need_scratch = (ctx.attr.office_connection or
                    _uses_token(values, "$(SCRATCH"))
    if _uses_token(values, "$(PROGRAM)") and not uno_program_dir:
        fail("$(PROGRAM) needs uno_install — there is no staged install to point at.")
    if ctx.attr.ure_bootstrap and not uno_program_dir:
        fail("ure_bootstrap needs uno_install: it only REDIRECTS the bootstrap " +
             "ini, it does not supply the UNO DLL closure the test still loads.")
    if ctx.attr.server_ready_port and not server_args:
        fail("server_ready_port without server_args — there is no server to wait for.")

    executable = staged_exe
    if (ctx.attr.run_in_staged_dir or uno_program_dir or env or prerun or
        run_args or server_args):
        launcher_dir = staged_exe.dirname  # the .bat sits beside the staged exe
        lines = ["@echo off", "setlocal"]

        # %~dp0 keeps its trailing backslash, which does not concatenate cleanly
        # and cannot be quoted next to one; "%~dp0." normalized by %%~fI gives the
        # same directory without it.
        lines += [
            'for %%I in ("%~dp0.") do set "_RUN=%%~fI"',
            # file:/// URL form: on Windows only the separators differ.
            'set "_RUNU=%_RUN:\\=/%"',
        ]

        if need_scratch:
            # Everything the test writes goes here.  Wiped BEFORE as well as
            # after: a run that dies without cleaning up must not hand its state
            # to the next one (a stale user installation is exactly the kind of
            # thing that makes a failure unreproducible).
            lines += [
                'set "_SCRATCH=%TEST_TMPDIR%\\scratch"',
                'if "%TEST_TMPDIR%"=="" set "_SCRATCH=%_RUN%\\scratch"',
                # TEST_TMPDIR arrives with forward slashes; the native form is
                # what osl's getFileURLFromSystemPath and cmd both want.
                'set "_SCRATCH=%_SCRATCH:/=\\%"',
                'if exist "%_SCRATCH%" rmdir /s /q "%_SCRATCH%"',
                'mkdir "%_SCRATCH%" || exit /b 1',
                # file:/// URL form: only the separators differ on Windows.
                'set "_SCRATCHU=%_SCRATCH:\\=/%"',
            ]

        if uno_program_dir:
            lines += [
                'set "_EXE=%~dp0' + staged_exe.basename + '"',
                # Resolved from the launcher's own location (%~dp0), not %CD%.
                'for %%I in ("%~dp0' + _windows_relpath(launcher_dir, uno_program_dir) +
                '") do set "_PROG=%%~fI"',
                'set "_PROGU=%_PROG:\\=/%"',
            ]

            # Which installation the UNO bootstrap describes.  Normally the
            # staged office (program/fundamental.ini, whose ${ORIGIN} then
            # supplies UNO_TYPES / UNO_SERVICES / URE_INTERNAL_LIB_DIR).  A test
            # that is ABOUT the configuration layer needs its own tiny
            # installation instead, so it can assert on data it controls rather
            # than on whatever the real registry happens to hold — that is what
            # ure_bootstrap redirects to.  The DLLs still come from program/ via
            # PATH below; only the data root moves.
            #
            # NOTE single backslashes here, unlike arg-soffice/arg-user further
            # down.  URE_BOOTSTRAP is consumed by the bootstrap machinery itself
            # (rtl_bootstrap_args_open on the ini path) and is NOT put through
            # macro expansion, so its separators survive.  Values that a test
            # later reads back via rtl::Bootstrap::get() ARE expanded, and those
            # must double their backslashes.
            #
            # vnd.sun.star.pathname: takes a native path, not a file URL.
            if ctx.attr.ure_bootstrap:
                lines += [
                    'set "URE_BOOTSTRAP=vnd.sun.star.pathname:%_RUN%\\' +
                    ctx.attr.ure_bootstrap.replace("/", "\\") + '"',
                ]
            else:
                lines += [
                    'set "URE_BOOTSTRAP=vnd.sun.star.pathname:%_PROG%\\fundamental.ini"',
                ]

            lines += [
                # Component DLLs named in services.rdb are loaded at run time and
                # live in program/; the exe's own directory still wins for what it
                # imports directly, so its staged copies are unaffected.
                'set "PATH=%_PROG%;%PATH%"',
            ]

            # Working directory.  program/ by default (what the UNO bootstrap
            # tests have always used), but the staged dir when the test also
            # opens a fixture by bare relative name — the two demands collide,
            # and PATH already covers the only reason program/ was the cwd.
            if ctx.attr.run_in_staged_dir:
                lines += ['cd /d "%~dp0" || exit /b 1']
            else:
                lines += ['cd /d "%_PROG%" || exit /b 1']

            if ctx.attr.office_connection:
                # test::OfficeConnection reads its two arguments through
                # rtl::Bootstrap under an "arg-" prefix (test/getargument.cxx).
                # There is no dmake recipe to copy: solenv's C++ APP1TEST rule
                # runs the bare exe with only --gtest_output, so it never sets
                # these at all; the complete recipe is the JAVA one
                # (installationtest.mk::javatest), which this mirrors.
                #   * arg-soffice — "path:<exe>" makes OfficeConnection LAUNCH an
                #     office (with -accept=pipe,name=…;urp); "connect:<desc>"
                #     would attach to an already-running one.
                #   * arg-user — a fresh user installation, wiped before AND after
                #     as javatest does, so no state leaks between runs.  NOTE it
                #     is a NATIVE PATH, not the file:// URL the Java side passes:
                #     the C++ side feeds it to toAbsoluteFileUrl(), i.e.
                #     osl::FileBase::getFileURLFromSystemPath().
                #
                # Passed as ENVIRONMENT VARIABLES, not as "-env:arg-…" command
                # line arguments, which is what the dmake/Java side implies.
                # rtl::Bootstrap resolves a key by trying the command line first
                # and the environment second (bootstrap.cxx, Bootstrap_Impl::
                # getValue), but the command-line half reads
                # osl_getCommandArgCount(), which is only ever populated by
                # sal_detail_initialize() — i.e. by SAL_IMPLEMENT_MAIN.  AOO's
                # GoogleTest suites declare a plain main() instead and skip that,
                # so -env: arguments are INVISIBLE to them.  Same root cause as
                # //build/testsupport:sal_process_init (no WSAStartup for the
                # socket suites); the environment route needs no shim TU and
                # cannot be broken by a future suite's choice of main().
                # The user installation is just the general scratch directory —
                # created and wiped by the block above, which is why there is no
                # per-run bookkeeping here.
                lines += [
                    'set "_SOFFICE=%_PROG%\\soffice.exe"',
                    # A BACKSLASH IS AN ESCAPE CHARACTER in a bootstrap value:
                    # rtl::Bootstrap runs every value it returns through macro
                    # expansion, where read() (sal/rtl/source/bootstrap.cxx)
                    # turns "\X" into "X" and "\uXXXX" into a code point.  So a
                    # raw Windows path loses EVERY separator on the way out —
                    # "C:\Users\x" comes back as "C:Usersx" and
                    # getFileURLFromSystemPath then fails with 21.  Double them
                    # so one survives each unescape.  (The dmake/Java side never
                    # hit this: it passes a file:// URL, which has no
                    # backslashes.)
                    'set "arg-soffice=path:%_SOFFICE:\\=\\\\%"',
                    'set "arg-user=%_SCRATCH:\\=\\\\%"',
                ]
            lines += _env_lines(env) + _prerun_lines(prerun) + srvlines
            lines += ['"%_EXE%"' + argstr + " %*"]
        else:
            # Co-locating a data file with the exe is not enough for a test that
            # opens it by bare relative name: the working directory is the
            # execroot, not the exe's directory (the loader finds the staged DLLs
            # via the exe's own path, which is why those work regardless).
            # Only when that was actually asked for — a launcher that exists
            # solely to set `env` must not silently move the cwd as well.
            if ctx.attr.run_in_staged_dir:
                lines += ['cd /d "%~dp0" || exit /b 1']
            lines += _env_lines(env) + _prerun_lines(prerun) + srvlines
            lines += ['"%~dp0' + staged_exe.basename + '"' + argstr + " %*"]

        # Capture the exit code BEFORE any cleanup — rmdir would clobber it.
        lines += ['set "_RC=%ERRORLEVEL%"']
        if server_args:
            # Normally a no-op: a --singleaccept server exits on its own once the
            # client drops the connection.  It is the FAILURE path that needs
            # this — a client that died before connecting would otherwise leave
            # the server blocked in accept() forever, holding the port and
            # poisoning every later run.
            lines += ['taskkill /f /im "%s" >nul 2>nul' % server_exe_name]
        if need_scratch:
            lines += ['rmdir /s /q "%_SCRATCH%" 2>nul']
        lines += ["exit /b %_RC%", ""]

        launcher = ctx.actions.declare_file(d + "/" + ctx.label.name + "_run.bat")
        ctx.actions.write(output = launcher, content = "\r\n".join(lines), is_executable = True)
        staged.append(launcher)
        executable = launcher

    return [DefaultInfo(
        executable = executable,
        runfiles = ctx.runfiles(files = staged + ctx.files.uno_install),
        files = depset([executable]),
    )]

_staged_gtest_test = rule(
    implementation = _staged_gtest_test_impl,
    test = True,
    attrs = {
        "binary": attr.label(executable = True, cfg = "target", mandatory = True),
        "runtime": attr.label_list(allow_files = True),
        "companions": attr.label_list(cfg = "target"),
        "app_manifest": attr.label(allow_single_file = True, default = _APP_MANIFEST),
        "run_in_staged_dir": attr.bool(default = False),
        "bin_layout": attr.bool(default = False),
        "office_connection": attr.bool(default = False),
        "uno_install": attr.label(allow_files = True),
        "data_tree": attr.label_keyed_string_dict(allow_files = True),
        "env": attr.string_dict(),
        "prerun": attr.string_list(),
        "run_args": attr.string_list(),
        "server_args": attr.string_list(),
        "server_ready_port": attr.string(),
        "ure_bootstrap": attr.string(),
    },
)

# The staging rule above is NOT gtest-specific: it stages an arbitrary /MD exe +
# runtime DLLs + VC90 CRT + external manifest into one flat dir and runs it as a
# test (pass/fail = process exit code).  Exposed for non-gtest runnable tests,
# e.g. bridges' inter_libs_exc (a cross-DLL C++ exception smoke test whose exe
# loads two sibling DLLs and exits 0 iff exception propagation works), and
# testtools' bridgetest, where the "test binary" is the generic //main/cpputools
# uno.exe and the suite is selected entirely by `run_args`.
#
# run_args: a FIXED command line baked into the launcher, token-expanded like
# `env` (so it can name a staged file).  Anything passed on the `bazel test`
# command line still follows it, via %*.  Deliberately not the native `args`
# attribute: baking it in keeps `bazel run` on the target reproducing exactly
# what `bazel test` ran.
#
# server_args / server_ready_port: a CLIENT/SERVER test.  The same `binary` is
# staged a second time as "<name>_server.exe" and started detached with
# server_args (token-expanded like run_args) just before the client runs; the
# launcher then waits until something is LISTENING on server_ready_port, and
# kills any surviving server afterwards.  The test result is still purely the
# CLIENT's exit code — the server is fixture, not verdict.
#
# It exists for testtools' bridgetest_urp, where the round trip has to cross a
# process boundary to exercise the URP bridge at all; the in-process variants
# need none of it.  A test using it should carry tags = ["exclusive"], since the
# port is a fixed, machine-global resource (upstream hardcodes 2002).
staged_run_test = _staged_gtest_test

def gtest_test(
        name,
        srcs,
        deps = [],
        copts = [],
        defines = [],
        runtime_dlls = [],
        data_files = [],
        uno_install = None,
        companions = [],
        bin_layout = False,
        office_connection = False,
        data_tree = {},
        env = {},
        prerun = [],
        ure_bootstrap = None,
        additional_linker_inputs = [],
        linkopts = [],
        size = None,
        **kwargs):
    """A GoogleTest suite that actually runs under `bazel test` on Windows/MD.

    data_files: inputs the test opens by relative path (fixture documents, …).
    They are staged beside the exe AND the test is launched with its working
    directory set to that staged dir, which co-location alone does not give you
    (see run_in_staged_dir in the staging rule).

    uno_install: run the test INSIDE the staged office install (pass
    //main/staging:install). Two independent things come from this, and either
    one alone is a good enough reason to use it:
      * a UNO bootstrap — URE_BOOTSTRAP is exported, so a test that calls
        cppu::defaultBootstrap_InitialComponentContext() gets a real component
        context (the *subsequent* test case);
      * the whole office DLL closure on PATH — which beats enumerating dozens of
        transitive DLLs in runtime_dlls for anything linking a big library like
        sfx.dll or sw.dll.
    It does make the test depend on the entire install, but a slow test beats no
    test; build time is not a reason to skip wiring something.

    companions: helper exes the test spawns by name (a child process). Staged
    beside the exe with their own external CRT manifest.

    bin_layout: stage into "<name>.run/bin" instead of "<name>.run", for suites
    that derive the helper's directory as <parent-of-own-dir>/bin (see the
    staging rule). Implies run_in_staged_dir so the child's working directory
    matches too.

    office_connection: the test uses test::OfficeConnection, i.e. it LAUNCHES a
    real soffice and talks to it over URP (fixture (b)). Requires uno_install.
    Supplies arg-soffice / arg-user and a per-run user installation.
    Defaults size to "medium" (300s): an office boot is far past the "small"
    60s budget — ~15s observed here, and every run builds a FRESH user
    installation so it is always a cold start. The headroom also bounds the
    failure mode: OfficeConnection::setUp() retries the resolve in an UNBOUNDED
    loop, so if the office never comes up the test timeout is the only thing
    that ends it. Do not drop this to "small".

    data_tree: {label: "relative/staged/path"} — like data_files, but staged at
    a path you choose instead of flat by basename. Use it when the SHAPE of the
    tree is the fixture: a mini UNO installation resolves $ORIGIN against the
    ini's own directory, and two of its files are both named "bootstrap.ini".

    env / prerun: extra environment variables, and command lines run in the
    launcher just before the exe (each is `|| exit /b 1`-guarded). Both expand
    $(RUNDIR) / $(PROGRAM) / $(SCRATCH) / $(SCRATCH_URL) — see _TOKENS.
    prerun exists for fixtures that can only be BUILT at run time: a registry a
    UNO tool has to write, for instance, where doing it as a build action would
    mean bundling that tool with its own CRT manifest and DLL closure, and would
    bake an absolute bazel-out path into a cached artifact.
    NOTE an env value that the test reads back through rtl::Bootstrap::get()
    goes through macro expansion, so its backslashes must be DOUBLED; one read
    with plain getenv() must not be.

    ure_bootstrap: staged-relative path (a data_tree entry) to a bootstrap ini
    to use as URE_BOOTSTRAP instead of the install's program/fundamental.ini —
    i.e. "run against THIS installation's data". For tests of the configuration
    layer itself, which need a registry they control rather than the real one.
    Requires uno_install: it moves the data root only, and the UNO DLL closure
    still comes from program/ via PATH.
    """
    if size == None:
        size = "medium" if office_connection else "small"
    cc_binary(
        name = name + "_bin",
        srcs = srcs,
        deps = deps + ["@gtest//:gtest"],
        # /Zc:wchar_t- MUST match @gtest's own build (see its overlay BUILD):
        # gtest.lib is compiled with wchar_t == unsigned short, so every test TU
        # linking it must agree, or PrintTo(wchar_t)/PrintTo(unsigned short)
        # mangle differently → LNK2019.
        copts = ["/Zc:wchar_t-"] + copts,
        defines = defines,
        # /MANIFEST:NO keeps the LINKER from generating one; the .res supplies
        # the manifest as a resource instead (see _APP_MANIFEST_RES).
        additional_linker_inputs = additional_linker_inputs + [_APP_MANIFEST_RES],
        linkopts = linkopts + [
            "$(execpath %s)" % _APP_MANIFEST_RES,
            "/MANIFEST:NO",
        ],
        testonly = True,
        **kwargs
    )
    _staged_gtest_test(
        name = name,
        binary = ":" + name + "_bin",
        runtime = runtime_dlls + data_files + [_CRT],
        companions = companions,
        run_in_staged_dir = bool(data_files) or bin_layout,
        bin_layout = bin_layout,
        office_connection = office_connection,
        uno_install = uno_install,
        data_tree = data_tree,
        env = env,
        prerun = prerun,
        ure_bootstrap = ure_bootstrap or "",
        size = size,
    )
