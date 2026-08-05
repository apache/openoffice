"""
Java pipeline rules for Apache OpenOffice Bazel build.

Pipeline:  .rdb --(javamaker)--> Java .class files tree --(jar)--> .jar

javamaker is an MSVC-compiled Windows PE that reads a UNO type registry
(.rdb) and writes Java .class files directly (one per UNO type) into the
output directory.  No javac step is involved in this rule.

DLL staging follows the same pattern as idl_pipeline.bzl: all tool DLLs
are hardlinked into a single flat directory via ctx.actions.symlink (pure
Bazel, no shell), and PATH is set to that directory for the action so the
Windows DLL loader can find them.

The -O path must start with "./" so osl's convertToFileUrl uses
getAbsoluteFileURL (relative-to-workdir) rather than failing on the bare
relative path — same constraint as cppumaker in idl_pipeline.bzl.
"""

_DLL_RENAME = [
    # (attr name,          runtime name Windows expects)
    ("_sal_dll",           "sal3.dll"),
    ("_salhelper_dll",     "salhelper3MSC.dll"),
    ("_store_dll",         "store.dll"),
    ("_reg_dll",           "reg.dll"),
]

def _javamaker_impl(ctx):
    javamaker = ctx.executable._javamaker

    dll_files    = [getattr(ctx.file, attr) for attr, _ in _DLL_RENAME]
    crt_files    = ctx.files._crt_dlls
    app_manifest = ctx.file._app_manifest

    rename = {}
    for attr, runtime_name in _DLL_RENAME:
        rename[getattr(ctx.file, attr)] = runtime_name

    # ── Stage javamaker.exe + all runtime DLLs ───────────────────────────
    all_to_stage = [javamaker] + dll_files + crt_files
    staged = {}
    for f in all_to_stage:
        dst_name = rename.get(f, f.basename)
        out = ctx.actions.declare_file(ctx.label.name + "_tools/" + dst_name)
        ctx.actions.symlink(output = out, target_file = f)
        staged[dst_name] = out

    # Side-car .manifest so Windows finds the VC90 CRT assembly
    out = ctx.actions.declare_file(ctx.label.name + "_tools/javamaker.exe.manifest")
    ctx.actions.symlink(output = out, target_file = app_manifest)
    staged["javamaker.exe.manifest"] = out

    tools_dir  = staged["javamaker.exe"].dirname
    all_staged = list(staged.values())

    # ── Resolve the .rdb input ───────────────────────────────────────────
    rdb_files = [f for f in ctx.files.rdb if f.extension == "rdb"]
    if len(rdb_files) != 1:
        fail("javamaker_classes: expected exactly one .rdb file from rdb=, got: " +
             str([f.path for f in rdb_files]))
    rdb = rdb_files[0]

    # ── Optionally resolve exclusion RDB (-X flag) ───────────────────────
    extra_inputs = []
    extra_args   = []
    if ctx.files.extra_rdb:
        xrdb_files = [f for f in ctx.files.extra_rdb if f.extension == "rdb"]
        if len(xrdb_files) != 1:
            fail("javamaker_classes: expected exactly one .rdb file from extra_rdb=, got: " +
                 str([f.path for f in xrdb_files]))
        extra_inputs = xrdb_files
        extra_args   = ["-X" + xrdb_files[0].path]

    # ── Run javamaker → tree of .class files ────────────────────────────
    # declare_directory: Bazel doesn't need to enumerate every output file —
    # javamaker creates one .class per UNO type in nested package dirs.
    # "./" prefix on -O: required so osl's path logic uses getAbsoluteFileURL.
    classes_dir = ctx.actions.declare_directory(ctx.label.name)

    ctx.actions.run(
        executable = staged["javamaker.exe"].path,
        arguments  = ["-BUCR", "-O./" + classes_dir.path, rdb.path] + extra_args,
        inputs           = [rdb] + extra_inputs + all_staged,
        outputs          = [classes_dir],
        env              = {"PATH": tools_dir},
        mnemonic         = "JavaMaker",
        progress_message = "Generating Java .class files from %s" % rdb.basename,
        use_default_shell_env = False,
    )

    return [DefaultInfo(files = depset([classes_dir]))]

# ---------------------------------------------------------------------------

javamaker_classes = rule(
    implementation = _javamaker_impl,
    attrs = {
        "rdb": attr.label(
            allow_files = True,
            mandatory   = True,
            doc         = "Label that provides the .rdb file (may also emit other files).",
        ),
        "extra_rdb": attr.label(
            allow_files = True,
            mandatory   = False,
            doc         = "Optional exclusion registry: types already in this .rdb are skipped (-X flag).",
        ),
        "_javamaker": attr.label(
            default     = "//main/codemaker:javamaker",
            executable  = True,
            cfg         = "exec",
            allow_files = True,
        ),
        "_sal_dll": attr.label(
            default           = "//main/sal:sal",
            allow_single_file = True,
            cfg               = "exec",
        ),
        "_salhelper_dll": attr.label(
            default           = "//main/salhelper:salhelper3MSC",
            allow_single_file = True,
            cfg               = "exec",
        ),
        "_store_dll": attr.label(
            default           = "//main/store:store",
            allow_single_file = True,
            cfg               = "exec",
        ),
        "_reg_dll": attr.label(
            default           = "//main/registry:reg",
            allow_single_file = True,
            cfg               = "exec",
        ),
        "_crt_dlls": attr.label(
            default     = "//main/external/msvcp90:msvcp90",
            allow_files = True,
            cfg         = "exec",
        ),
        "_app_manifest": attr.label(
            default           = "//main/external/msvcp90:vc90_app_manifest",
            allow_single_file = True,
            cfg               = "exec",
        ),
    },
    doc = """
Run javamaker.exe on a UNO type registry (.rdb) to produce Java .class files.

The output is a declare_directory containing the full package tree of generated
.class files (one per UNO type).  Downstream targets consume this via
jar_from_directory, then a java_import to expose it as a classpath entry.
""",
)

# ---------------------------------------------------------------------------
# jar_from_directory
# ---------------------------------------------------------------------------
# Packages a declare_directory full of .class files into a JAR by calling
# jar.exe directly via ctx.actions.run (no shell).  This avoids the PowerShell
# path-resolution problems that break a genrule using $(JAVABASE)/bin/jar when
# the JDK sandbox path contains '+' characters.
# ---------------------------------------------------------------------------

def _jar_from_directory_impl(ctx):
    # Resolve the Java runtime toolchain to locate jar.exe.
    java_rt = ctx.toolchains["@bazel_tools//tools/jdk:runtime_toolchain_type"].java_runtime

    jar_exe = None
    for f in java_rt.files.to_list():
        if f.basename in ("jar.exe", "jar") and (
            "/bin/" in f.path or "\\bin\\" in f.path
        ):
            jar_exe = f
            break
    if jar_exe == None:
        fail("jar_from_directory: could not locate jar(.exe) in the JDK runtime")

    classes_dir = ctx.file.classes_dir
    jar_out = ctx.actions.declare_file(ctx.label.name + ".jar")

    ctx.actions.run(
        executable       = jar_exe.path,
        arguments        = ["cf", jar_out.path, "-C", classes_dir.path, "."],
        inputs           = depset([classes_dir], transitive = [java_rt.files]),
        outputs          = [jar_out],
        mnemonic         = "JarPackage",
        progress_message = "Creating %s" % jar_out.basename,
        use_default_shell_env = False,
    )

    return [DefaultInfo(files = depset([jar_out]))]

jar_from_directory = rule(
    implementation = _jar_from_directory_impl,
    attrs = {
        "classes_dir": attr.label(
            allow_single_file = True,
            mandatory         = True,
            doc               = "A declare_directory target whose .class files to jar.",
        ),
    },
    toolchains = ["@bazel_tools//tools/jdk:runtime_toolchain_type"],
    doc = """
Package a declare_directory of Java .class files into a JAR.

Uses jar.exe from the JDK runtime toolchain, invoked via ctx.actions.run so
no shell quoting or PATH issues arise.  The output JAR is named
<name>.jar and placed in the package output directory.
""",
)

# ---------------------------------------------------------------------------
# uno_jar
# ---------------------------------------------------------------------------
# Produce a jar under the EXACT filename the UNO runtime looks it up by.
#
# Three problems, one rule:
#
#  (a) Bazel jar names are not runtime jar names.  java_library(name = "x")
#      emits libx.jar, but com/sun/star/lib/unoloader/UnoClassLoader.java
#      hardcodes "java_uno.jar" / "juh.jar" / "jurt.jar" / "ridl.jar" and
#      stoc/source/javavm/javavm.cxx hardcodes "unoloader.jar".  These are
#      exact-string lookups relative to $URE_INTERNAL_JAVA_DIR, so the staged
#      basename is load-bearing.  `out =` names the output directly rather
#      than renaming at staging time, so the name lives next to the target
#      that produces it.
#
#  (b) Some runtime jars are the union of several Bazel targets.  ridl.jar
#      upstream holds the javamaker-generated UDK types AND the hand-written
#      ridl sources; here those are two targets and java_library's `exports`
#      is COMPILE-time only, so libridl.jar alone has no com.sun.star.uno.*.
#      `jars =` takes any number of inputs and merges them.
#
#  (c) The manifest main attributes are read at runtime.  javaloader reads
#      RegistrationClassName to register a Java component; UnoClassLoader
#      .getClassLoader() reads UNO-Type-Path.  Neither java_library nor
#      jar_from_directory (a bare `jar cf`) writes a manifest.
#
# singlejar does all three, and is already part of the Bazel JDK tooling —
# no new external dependency.
# ---------------------------------------------------------------------------

def _uno_jar_impl(ctx):
    jar_out = ctx.actions.declare_file(ctx.attr.out)

    srcs = []
    for t in ctx.attr.jars:
        srcs += [f for f in t.files.to_list() if f.extension == "jar"]
    if not srcs:
        fail("uno_jar '%s': jars = produced no .jar files" % ctx.label.name)

    args = ctx.actions.args()
    args.add("--output", jar_out)
    args.add("--sources")
    args.add_all(srcs)
    if ctx.attr.manifest_lines:
        args.add("--deploy_manifest_lines")
        args.add_all(ctx.attr.manifest_lines)
    # Deterministic output: no timestamps, no build-data.properties stamp.
    args.add("--normalize")
    args.add("--exclude_build_data")

    ctx.actions.run(
        executable       = ctx.executable._singlejar,
        arguments        = [args],
        inputs           = depset(srcs),
        outputs          = [jar_out],
        mnemonic         = "UnoJar",
        progress_message = "Assembling %s" % jar_out.basename,
    )

    return [DefaultInfo(files = depset([jar_out]))]

uno_jar = rule(
    implementation = _uno_jar_impl,
    attrs = {
        "out": attr.string(
            mandatory = True,
            doc       = "Exact output filename, e.g. \"ridl.jar\" — the name the UNO runtime looks up.",
        ),
        "jars": attr.label_list(
            allow_files = True,
            mandatory   = True,
            doc         = "Jars to merge, in order.  A single entry simply restamps name/manifest.",
        ),
        "manifest_lines": attr.string_list(
            doc = "Manifest MAIN-section attributes, e.g. [\"RegistrationClassName: com.sun.star.comp.Foo\"].",
        ),
        "_singlejar": attr.label(
            default    = "@bazel_tools//tools/jdk:singlejar",
            executable = True,
            cfg        = "exec",
        ),
    },
    doc = """
Assemble a runtime UNO jar: merge jars, stamp manifest main attributes, and
emit under an exact filename.  See the block comment above for why each of
the three is needed.
""",
)
