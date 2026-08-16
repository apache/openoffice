"""Wizard Java UNO components.

The nine Jar_*.mk recipes in this module are the same shape nine times over:
one Java package compiled against the UNO runtime plus commonwizards, jarred
under an exact filename, with a manifest javaloader reads at load time.  This
macro is that shape, once.

Everything about the manifest is load-bearing, none of it is decoration:

  RegistrationClassName  javaloader has no other way to find the class to
                         register — this is why the jars need `uno_jar` rather
                         than a plain `java_library`.

  UNO-Type-Path          MUST be present and MUST be empty.  UnoClassLoader
                         .getClassLoader() (ridljar unoloader) substitutes "<>"
                         when the header is ABSENT, and "<>" resolves to the
                         jar's own URL, which it then addURL()s into the SHARED
                         UnoClassLoader — permanently hoisting a wizard jar into
                         the global UNO type path.  An EMPTY value walks a
                         zero-length path and adds nothing, which is what these
                         jars want: they carry implementations, not UNO types.

  Class-Path             gb_Jar_set_jarclasspath.  URLClassLoader honours it
                         relative to the jar, which is how each component finds
                         commonwizards.jar next to it in program/classes/.
"""

load("@rules_java//java:defs.bzl", "java_library")
load("//build/rules:java_pipeline.bzl", "uno_jar")

# ridl comes in transitively — //main/unoil:unoil exports it.  java_uno.jar is
# deliberately NOT here although the dmake JARFILES lists it: it is a RUNTIME
# dependency of the JNI bridge, not a compile dependency of this source (same
# call as //main/cppuhelper's propertysetmixin supplier).
UNO_DEPS = [
    "//main/unoil:unoil",
    "//main/jurt:jurt",
    "//main/javaunohelper:juh_jar",
]

JAVACOPTS = ["--release", "8", "-XepDisableAllChecks"]

def wizard_jar(
        name,
        out = None,
        registration_class = None,
        class_path = ["commonwizards.jar"],
        deps = []):
    """One wizard jar: <name>_classes (java_library) + <name>_jar (the artifact).

    Args:
      name:               the package directory under com/sun/star/wizards/.
      out:                jar filename; defaults to <name>.jar.  Only
                          reportbuilder differs (reportbuilderwizard.jar).
      registration_class: the UNO implementation entry point.  Omitted for
                          reportbuilder, which is not a registered component —
                          ReportWizard loads its layouters reflectively.
      class_path:         Class-Path manifest entry, space-joined.
      deps:               extra compile deps beyond UNO_DEPS + commonwizards.
    """
    java_library(
        name = name + "_classes",
        srcs = native.glob(["com/sun/star/wizards/%s/**/*.java" % name]),
        javacopts = JAVACOPTS,
        deps = UNO_DEPS + [":commonwizards"] + deps,
    )

    manifest_lines = []
    if registration_class:
        manifest_lines.append("RegistrationClassName: " + registration_class)
        # Trailing space, not a typo: singlejar splits the line on ": ", so an
        # empty value still needs the separator.  See the header comment for
        # why empty and absent are not the same thing here.
        manifest_lines.append("UNO-Type-Path: ")
    if class_path:
        manifest_lines.append("Class-Path: " + " ".join(class_path))

    uno_jar(
        name = name + "_jar",
        out = out if out else name + ".jar",
        jars = [":" + name + "_classes"],
        manifest_lines = manifest_lines,
    )

# A BASIC library directory: .xba modules, .xdl dialogs and the two .xlb
# library indices.  The three extensions are the filter that keeps the
# co-located .src files (euro.src, dbwizres.src, …) out — those are inputs to
# the resource bundles, not BASIC sources.  Each glob matches its Zip_*.mk file
# list exactly.
_BASIC_EXTS = [
    "*.xba",
    "*.xdl",
    "*.xlb",
]

def basic_library(name, dir = None):
    """filegroup basic_<name> over source/<dir>/, defaulting dir to name."""
    src_dir = dir if dir else name
    native.filegroup(
        name = "basic_" + name,
        # allow_empty: not every library has dialogs — launcher is three
        # files, all .xba/.xlb, so its *.xdl pattern legitimately matches none.
        srcs = native.glob(
            ["source/%s/%s" % (src_dir, e) for e in _BASIC_EXTS],
            allow_empty = True,
        ),
    )
