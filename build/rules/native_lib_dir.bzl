"""native_lib_dir — gather native DLLs into one directory for java.library.path.

A JNI test loads its native library at runtime: System.loadLibrary(name) resolves
it by name from java.library.path, and the Windows loader then resolves that DLL's
transitive deps from its own directory. So all of a JNI test's native libs
must be co-located in a single directory. Bazel `data` scatters them
across per-package dirs; this rule copies the given DLLs/exes into one tree
artifact that java_test points -Djava.library.path at via $(location).
"""

def _native_lib_dir_impl(ctx):
    out = ctx.actions.declare_directory(ctx.label.name)
    files = [f for f in ctx.files.dlls if f.extension in ("dll", "exe")]
    inputs = list(files)
    cmds = [
        'mkdir -p "%s"' % out.path,
        'cp %s "%s/"' % (" ".join(['"%s"' % f.path for f in files]), out.path),
    ]
    ctx.actions.run_shell(
        inputs = inputs,
        outputs = [out],
        command = " && ".join(cmds),
        mnemonic = "NativeLibDir",
        progress_message = "Staging %d native libs into %s" % (len(files), out.short_path),
    )
    return [DefaultInfo(files = depset([out]))]

native_lib_dir = rule(
    implementation = _native_lib_dir_impl,
    attrs = {
        "dlls": attr.label_list(
            allow_files = True,
            doc = "DLL / exe providing targets to co-locate in one dir.",
        ),
    },
)
