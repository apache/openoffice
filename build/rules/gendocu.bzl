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
