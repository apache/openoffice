"""The JRE a *test* starts in-process, per target architecture.

Any test that puts jvmfwk into DIRECT mode (UNO_JAVA_JFW_JREHOME) loads a
jvm.dll into its OWN process, so the JRE has to match the TARGET arch, not the
host's — a 32-bit default build cannot load the 64-bit JVM that $JAVA_HOME
almost certainly points at, and one JAVA_HOME cannot be right for both arches
anyway.  Hence a select(), and hence not the build JDK: @remotejdk21_win is the
toolchain that COMPILES (and is x64-only), which is a different question from
which JVM a test may load.

This is machine-specific and the one genuinely unhermetic input in the test
suite.  It lives here rather than in each BUILD file so that pointing the tree
at a different JDK is a single edit; //main/bridges' java_run_test still takes
its own `jvm_path_dirs` (it needs the DIRECTORY containing jvm.dll for PATH,
not a home URL), so that is a related but distinct knob.

A URL, not a path, and mind two encodings:
  * forward slashes — every rtl::Bootstrap value is macro-expanded, where a
    backslash is an ESCAPE character, so "C:\\Program" comes back "C:Program";
  * %20 for spaces — osl's file-URL parsing requires it.
And note the launcher doubles '%' for cmd before substituting (gtest_test.bzl
_expand_tokens); without that "%20" reaches the test as "0".
"""

_JRE_X86 = "file:///C:/Program%20Files%20(x86)/Eclipse%20Adoptium/jdk-8.0.452.9-hotspot"
_JRE_X64 = "file:///C:/Program%20Files/Eclipse%20Adoptium/jdk-8.0.452.9-hotspot"

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
