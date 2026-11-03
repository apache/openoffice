"""Code-generation steps of the CPython 3.11 build, as Bazel actions.

CPython 3.11 compiles part of its own standard library into the interpreter:
  1. _freeze_module turns a .py file into a C header holding its marshalled
     code object (Python/frozen_modules/<name>.h).
  2. Tools/scripts/deepfreeze.py turns those headers into C source that builds
     the code objects statically (Python/deepfreeze/deepfreeze.c).

PCbuild runs step 2 with an already installed Python found on the machine
("PythonForBuild").  The Unix build can do without one: it links
_bootstrap_python -- the core minus frozen.c/deepfreeze.c, with only importlib
frozen -- and runs deepfreeze.py under that.  This file does the latter, so
the build needs no Python on the host.

Both tools are built for the exec platform and run directly through
ctx.actions.run: no shell, and nothing inherited from the host environment.
"""

def _freeze_impl(ctx):
    outs = []
    for src, name in zip(ctx.files.srcs, ctx.attr.module_names):
        out = ctx.actions.declare_file("Python/frozen_modules/%s.h" % name)
        ctx.actions.run(
            executable = ctx.executable.tool,
            arguments = [name, src.path, out.path],
            inputs = [src],
            outputs = [out],
            mnemonic = "PyFreeze",
            progress_message = "Freezing Python module %s" % name,
        )
        outs.append(out)
    return [DefaultInfo(files = depset(outs))]

python_freeze = rule(
    implementation = _freeze_impl,
    doc = "Runs _freeze_module once per module: srcs[i] is frozen as module_names[i].",
    attrs = {
        "srcs": attr.label_list(allow_files = [".py"], mandatory = True),
        "module_names": attr.string_list(mandatory = True),
        "tool": attr.label(executable = True, cfg = "exec", mandatory = True),
    },
)

def _deepfreeze_impl(ctx):
    out = ctx.actions.declare_file("Python/deepfreeze/deepfreeze.c")

    # PYTHONHOME is the source root, so the bootstrap interpreter finds its
    # stdlib in Lib/; it is the directory two above Lib/os.py.
    home = ctx.file.os_py.dirname.rpartition("/")[0]

    args = ctx.actions.args()
    args.add(ctx.file.script)
    for header, name in zip(ctx.files.frozen, ctx.attr.module_names):
        args.add("%s:%s" % (header.path, name))
    args.add("-o", out)

    ctx.actions.run(
        executable = ctx.executable.tool,
        arguments = [args],
        inputs = depset(
            [ctx.file.script, ctx.file.os_py] + ctx.files.frozen,
            transitive = [depset(ctx.files.read_inputs)],
        ),
        outputs = [out],
        # The bootstrap interpreter's config is isolated: it reads PYTHONHOME
        # only because PC/aoo_bootstrap_python.c makes it, and it never writes
        # bytecode (no __pycache__ in the source repo), so no
        # PYTHONDONTWRITEBYTECODE is needed.
        env = {"PYTHONHOME": home},
        mnemonic = "PyDeepfreeze",
        progress_message = "Deep-freezing %d Python modules" % len(ctx.files.frozen),
    )
    return [DefaultInfo(files = depset([out]))]

python_deepfreeze = rule(
    implementation = _deepfreeze_impl,
    doc = "Runs Tools/scripts/deepfreeze.py under the bootstrap interpreter.",
    attrs = {
        "frozen": attr.label_list(allow_files = [".h"], mandatory = True),
        "module_names": attr.string_list(mandatory = True),
        "script": attr.label(allow_single_file = [".py"], mandatory = True),
        "os_py": attr.label(
            allow_single_file = True,
            mandatory = True,
            doc = "Lib/os.py, which locates the stdlib for PYTHONHOME.",
        ),
        # Deliberately not named "data" or "srcs": //main/staging's collection
        # aspect walks those, and this list holds the whole stdlib as .py files,
        # which it would otherwise install flat into program/.
        "read_inputs": attr.label_list(
            allow_files = True,
            doc = "Everything the run reads: the stdlib, the helper scripts, " +
                  "and the C sources generate_global_objects.py scans.",
        ),
        "tool": attr.label(executable = True, cfg = "exec", mandatory = True),
    },
)

def _rename_impl(ctx):
    out = ctx.actions.declare_file(ctx.attr.out)
    ctx.actions.symlink(output = out, target_file = ctx.file.src)
    return [DefaultInfo(files = depset([out]))]

rename_file = rule(
    implementation = _rename_impl,
    doc = """Re-exposes one file under another name.

A linkshared cc_binary is always named <target>.dll on Windows, and Python's
importer accepts only .pyd for an extension module (Python/dynload_win.c).""",
    attrs = {
        "src": attr.label(allow_single_file = True, mandatory = True),
        "out": attr.string(mandatory = True),
    },
)
