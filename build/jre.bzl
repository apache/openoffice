"""The JRE a *test* runs a JVM from, per target architecture.

The arch has to match the TARGET, not the host, and for two independent reasons:

  * a test that puts jvmfwk into DIRECT mode (UNO_JAVA_JFW_JREHOME) loads a
    jvm.dll into its OWN process — see jre_home_env();
  * a JUnit test that drives an office (uno_junit_test) runs in a JVM that loads
    jpipe.dll, the JNI half of the named-pipe UNO connection, out of the staged
    program/ — see jre_java_exe().

Either way a 32-bit default build cannot use the 64-bit JVM that $JAVA_HOME
almost certainly points at, and one JAVA_HOME cannot be right for both arches.
Hence a select(), and hence not the build JDK: @remotejdk21_win is the toolchain
that COMPILES (and is x64-only), which is a different question from which JVM a
test may run.

This is machine-specific and the one genuinely unhermetic input in the test
suite.  It lives here rather than in each BUILD file so that pointing the tree
at a different JDK is a single edit; //main/bridges' java_run_test still takes
its own `jvm_path_dirs` (it needs the DIRECTORY containing jvm.dll for PATH,
not a home URL), so that is a related but distinct knob.

The native path is the source of truth and the URL is DERIVED from it, because
the same JDK has to be spelled two ways and a hand-maintained pair drifts:
  * a native path for a command line (`"<home>\\bin\\java.exe"`);
  * a file:/// URL for a bootstrap variable, which needs forward slashes (every
    rtl::Bootstrap value is macro-expanded, where a backslash is an ESCAPE
    character, so "C:\\Program" comes back "C:Program") and %20 for spaces
    (osl's file-URL parsing requires it).
And note the launcher doubles '%' for cmd before substituting (gtest_test.bzl
_expand_tokens); without that "%20" reaches the test as "0".
"""

_JDK_X86 = "C:\\Program Files (x86)\\Eclipse Adoptium\\jdk-8.0.452.9-hotspot"
_JDK_X64 = "C:\\Program Files\\Eclipse Adoptium\\jdk-8.0.452.9-hotspot"

def _url(path):
    return "file:///" + path.replace("\\", "/").replace(" ", "%20")

_JRE_X86 = _url(_JDK_X86)
_JRE_X64 = _url(_JDK_X64)

def jre_home_env():
    """select() giving UNO_JAVA_JFW_JREHOME for the target arch.

    Merge it into a gtest_test/staged_run_test `env` dict:

        env = { ... } | jre_home_env()

    Setting any UNO_JAVA_JFW_{JREHOME,ENV_JREHOME,CLASSPATH,PARAMETER_1} is what
    selects jvmfwk's DIRECT mode (fwkbase.cxx getMode).  Its other mode,
    JFW_MODE_APPLICATION, reads a javasettings_<os>_<arch>.xml recording which
    JRE a human picked in Tools > Options > Java; a fixture has no such history,
    so jfw_startVM() would return JFW_E_JAVA_DISABLED before looking for a JVM.
    Direct mode skips only that settings layer — sunjavaplugin.dll still probes
    the JRE, so vendor recognition is still exercised.
    """
    return select({
        "//build:arch_x64": {"UNO_JAVA_JFW_JREHOME": _JRE_X64},
        "//conditions:default": {"UNO_JAVA_JFW_JREHOME": _JRE_X86},
    })

def jre_java_exe():
    """select() giving the native path of java.exe for the target arch.

    For uno_junit_test, which runs the JUnit suite in a JVM of its own (the
    office it drives is a separate process, started by the test).  The arch has
    to match the target anyway, because org.openoffice.test.OfficeConnection
    connects over a NAMED PIPE and jurt's PipeConnection is JNI: the JVM loads
    jpipe.dll out of the staged program/, and a 64-bit JVM cannot load the
    32-bit one a default build produces.

    JDK 8 also happens to be what these sources were written for — qadevOOo and
    the qa/complex suites are Java 1.4/5-era code compiled here with --release 8.
    """
    return select({
        "//build:arch_x64": _JDK_X64 + "\\bin\\java.exe",
        "//conditions:default": _JDK_X86 + "\\bin\\java.exe",
    })

def jre_home_native():
    """select() giving the native path of the JRE home for the target arch.

    Exported for JAVA_HOME, which is how the office under test picks its own JVM
    once a test passes -env:UNO_JAVA_JFW_ENV_JREHOME=true (which
    OfficeConnection always does).  Without it the office would take whatever
    JAVA_HOME the developer's shell happens to hold — very likely the 64-bit JDK,
    which a 32-bit office cannot load.
    """
    return select({
        "//build:arch_x64": _JDK_X64,
        "//conditions:default": _JDK_X86,
    })
