"""autodoc-driven API reference generation (odk pack/gendocu, pack/copying).

autodoc parses the UNO IDL (and the C++ headers) and writes a whole TREE of
HTML whose file set depends on the API, so the output is a tree artifact
(declare_directory) rather than a declared file list.  One ctx.actions.run,
no shell.

  autodoc_html   run autodoc -html <dir> <args...> -t <root>...  -> <dir> tree

The `roots` are exec-root-relative source directories (e.g. "main/udkapi"); the
matching source files must be listed in `srcs` so they are present in the
sandbox at those paths.  `args` are the literal autodoc flags that go between
`-html <dir>` and the trailing `-t <root>` list (language, -name, -dvgroot,
-dvgfile, -ignoredefine, -p <proj> <root>, ...).
"""

def _autodoc_html_impl(ctx):
    out = ctx.actions.declare_directory(ctx.attr.out_dir)

    args = ctx.actions.args()
    args.add("-html", out.path)
    args.add_all(ctx.attr.args)
    for root in ctx.attr.roots:
        args.add("-t", root)

    ctx.actions.run(
        executable = ctx.executable._autodoc,
        arguments = [args],
        inputs = ctx.files.srcs,
        outputs = [out],
        mnemonic = "Autodoc",
        progress_message = "Generating %s with autodoc" % ctx.attr.out_dir,
    )
    return DefaultInfo(files = depset([out]))

autodoc_html = rule(
    implementation = _autodoc_html_impl,
    attrs = {
        "out_dir": attr.string(
            mandatory = True,
            doc = "Name of the output directory (tree artifact) autodoc fills.",
        ),
        "srcs": attr.label_list(
            allow_files = True,
            mandatory = True,
            doc = "Every input autodoc reads: the IDL/header source trees and " +
                  "any -dvgfile.  They land in the sandbox at their exec paths, " +
                  "which is what `roots` points -t at.",
        ),
        "roots": attr.string_list(
            mandatory = True,
            doc = "Exec-root-relative source roots passed as -t <root> (e.g. " +
                  "\"main/udkapi\").  autodoc walks each for com/sun/star.",
        ),
        "args": attr.string_list(
            doc = "Literal autodoc flags between -html <dir> and the -t roots.",
        ),
        "_autodoc": attr.label(
            default = "//main/autodoc:autodoc",
            executable = True,
            cfg = "exec",
        ),
    },
)

# ── javadoc_html ─────────────────────────────────────────────────────────────
# gendocu's Java reference: javadoc over the UNO Java runtime sources.  Same
# tree-artifact shape as autodoc_html.  javadoc comes from the JDK runtime
# toolchain, located the way java_pipeline.bzl locates javac/jar.

def _javadoc_html_impl(ctx):
    java_rt = ctx.toolchains["@bazel_tools//tools/jdk:runtime_toolchain_type"].java_runtime

    javadoc = None
    for f in java_rt.files.to_list():
        if f.basename in ("javadoc.exe", "javadoc") and (
            "/bin/" in f.path or "\\bin\\" in f.path
        ):
            javadoc = f
            break
    if javadoc == None:
        fail("javadoc_html: could not locate javadoc(.exe) in the JDK runtime")

    out = ctx.actions.declare_directory(ctx.attr.out_dir)

    args = ctx.actions.args()
    args.add("-J-Xmx256m")
    args.add("-quiet")
    args.add("-d", out.path)
    args.add_joined("-sourcepath", ctx.attr.sourcepath, join_with = ";")
    if ctx.files.classpath:
        args.add_joined(
            "-classpath",
            [j.path for j in ctx.files.classpath],
            join_with = ";",
        )
    args.add_all(ctx.attr.args)
    args.add_all(ctx.attr.tail)

    ctx.actions.run(
        executable = javadoc.path,
        arguments = [args],
        inputs = depset(
            ctx.files.srcs + ctx.files.classpath,
            transitive = [java_rt.files],
        ),
        outputs = [out],
        mnemonic = "Javadoc",
        progress_message = "Generating %s with javadoc" % ctx.attr.out_dir,
        use_default_shell_env = False,
    )
    return DefaultInfo(files = depset([out]))

javadoc_html = rule(
    implementation = _javadoc_html_impl,
    attrs = {
        "out_dir": attr.string(mandatory = True, doc = "Output tree-artifact dir."),
        "srcs": attr.label_list(
            allow_files = [".java"],
            mandatory = True,
            doc = "Every .java input; must cover the sourcepath packages and any " +
                  "loose file named in `tail`.",
        ),
        "sourcepath": attr.string_list(
            mandatory = True,
            doc = "Exec-root-relative source roots for -sourcepath (joined with ;).",
        ),
        "classpath": attr.label_list(
            allow_files = [".jar"],
            doc = "Jars for -classpath so javadoc resolves referenced types.",
        ),
        "args": attr.string_list(doc = "Literal javadoc flags (-use, -header, ...)."),
        "tail": attr.string_list(
            mandatory = True,
            doc = "Trailing operands: package names, and/or loose .java exec paths.",
        ),
    },
    toolchains = ["@bazel_tools//tools/jdk:runtime_toolchain_type"],
)
