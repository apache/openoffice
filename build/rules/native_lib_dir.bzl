"""native_lib_dir — gather native DLLs into one directory for java.library.path.

A JNI test loads its native library at runtime: System.loadLibrary(name) resolves
it by name from java.library.path, and the Windows loader then resolves that DLL's
transitive deps from its own directory. So all of a JNI test's native libs (plus
the VC90 CRT) must be co-located in a single directory. Bazel `data` scatters them
across per-package dirs; this rule copies the given DLLs/manifests into one tree
artifact that java_test points -Djava.library.path at via $(location).
"""

def _native_lib_dir_impl(ctx):
    out = ctx.actions.declare_directory(ctx.label.name)
    files = [f for f in ctx.files.dlls if f.extension in ("dll", "manifest", "exe")]
    inputs = list(files)
    cmds = [
        'mkdir -p "%s"' % out.path,
        'cp %s "%s/"' % (" ".join(['"%s"' % f.path for f in files]), out.path),
    ]
    # Optional external <dll>.manifest: gives the named DLL a VC90-CRT activation
    # context when it is loaded into a foreign process (e.g. a JVM), so the CRT
    # resolves via the co-located SxS assembly instead of loose (avoids R6034).
    if ctx.file.app_manifest and ctx.attr.manifest_as:
        inputs.append(ctx.file.app_manifest)
        cmds.append('cp "%s" "%s/%s"' % (ctx.file.app_manifest.path, out.path, ctx.attr.manifest_as))
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
            doc = "DLL / CRT-manifest providing targets to co-locate in one dir.",
        ),
        "app_manifest": attr.label(
            allow_single_file = True,
            doc = "Optional manifest to copy in under manifest_as (e.g. the VC90-CRT app manifest).",
        ),
        "manifest_as": attr.string(
            default = "",
            doc = "Filename to give app_manifest, e.g. '<primary_dll>.dll.manifest'.",
        ),
    },
)
