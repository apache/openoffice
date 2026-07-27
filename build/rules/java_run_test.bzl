"""staged_java_test — run a native-bridge JVM test via an in-process JVM host.

A JNI bridge test loads 32-bit /MD UNO DLLs that need a VC90-CRT activation
context for every load — including the ones the UNO machinery loads at runtime
via osl::Module. A stock java.exe provides no such context (R6034). So instead of
running java.exe directly, this rule runs jni_test_launcher.exe, which carries the
VC90-CRT manifest embedded and hosts the JVM in-process: its process-default
activation context covers every UNO DLL the JVM loads.

The launcher lives inside library_dir (co-located with the UNO DLLs + the CRT SxS
assembly). The emitted .bat puts the JDK dirs and library_dir on PATH (so jvm.dll
and the DLLs' transitive deps resolve) and runs the launcher with the classpath,
java.library.path and main class. Requires build --enable_runfiles.

A non-zero exit fails the test (TestJni calls System.exit(1)).
"""

load("@rules_java//java/common:java_info.bzl", "JavaInfo")

def _staged_java_test_impl(ctx):
    def rf(short_path):
        return "%TEST_SRCDIR%\\%TEST_WORKSPACE%\\" + short_path.replace("/", "\\")

    libdir = rf(ctx.file.library_dir.short_path)
    launcher = libdir + "\\" + ctx.attr.launcher_name

    jars = depset(transitive = [
        dep[JavaInfo].transitive_runtime_jars
        for dep in ctx.attr.runtime_deps
    ]).to_list()
    classpath = ";".join([rf(j.short_path) for j in jars])

    main_slash = ctx.attr.main_class.replace(".", "/")
    path_prefix = ";".join(ctx.attr.jvm_path_dirs + [libdir])

    out = ctx.actions.declare_file(ctx.label.name + ".bat")
    ctx.actions.write(
        output = out,
        content = "\r\n".join([
            "@echo off",
            "setlocal",
            'set "PATH=' + path_prefix + ';%PATH%"',
            '"' + launcher + '" "' + classpath + '" "' + libdir + '" "' +
                main_slash + '" %*',
            "exit /b %ERRORLEVEL%",
            "",
        ]),
        is_executable = True,
    )

    runfiles = ctx.runfiles(files = [ctx.file.library_dir] + jars)
    return [DefaultInfo(executable = out, runfiles = runfiles)]

staged_java_test = rule(
    implementation = _staged_java_test_impl,
    test = True,
    attrs = {
        "main_class": attr.string(mandatory = True, doc = "Dotted main class (run via the launcher)."),
        "runtime_deps": attr.label_list(
            providers = [[JavaInfo]],
            doc = "Java deps whose transitive runtime jars form the classpath.",
        ),
        "library_dir": attr.label(
            allow_single_file = True,
            doc = "native_lib_dir tree artifact: UNO DLLs + CRT SxS assembly + the launcher exe.",
        ),
        "launcher_name": attr.string(
            default = "jni_test_launcher.exe",
            doc = "Basename of the in-process JVM host exe inside library_dir.",
        ),
        "jvm_path_dirs": attr.string_list(
            doc = "Absolute JDK dirs to prepend to PATH so jvm.dll + its deps resolve " +
                  "(e.g. <jdk>\\jre\\bin\\server and <jdk>\\jre\\bin). Machine-specific.",
        ),
    },
)
