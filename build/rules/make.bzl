def _make_target_impl(ctx):
    """Wraps an existing Makefile target in a Bazel rule."""
    
    # Declare outputs
    outputs = ctx.outputs.outs
    
    # Build environment variables (mimicking solenv)
    env = {
        "SOLARVER": ctx.attr.solar_version,
        "INPATH": ctx.attr.inpath,
        "OUTPATH": ctx.attr.outpath,
        "SRCDIR": ctx.workspace_root.path,
    }
    env.update(ctx.attr.extra_env)
    
    # Collect inputs (source files + dependencies)
    inputs = depset(
        ctx.files.srcs,
        transitive = [dep[DefaultInfo].files for dep in ctx.attr.deps],
    )
    
    args = ctx.actions.args()
    args.add("-C", ctx.attr.directory)
    args.add("-f", ctx.attr.makefile)
    args.add(ctx.attr.make_target)
    args.add("VERBOSE=1")
    
    ctx.actions.run(
        inputs = inputs,
        outputs = outputs,
        executable = ctx.executable._make,
        arguments = [args],
        env = env,
        mnemonic = "OOoMake",
        progress_message = "Building OOo module: %s" % ctx.attr.directory,
    )
    
    return [DefaultInfo(files = depset(outputs))]


make_target = rule(
    implementation = _make_target_impl,
    attrs = {
        "srcs": attr.label_list(allow_files = True),
        "deps": attr.label_list(),
        "outs": attr.output_list(mandatory = True),
        "directory": attr.string(mandatory = True),
        "makefile": attr.string(default = "makefile.mk"),
        "make_target": attr.string(default = "all"),
        "solar_version": attr.string(default = "680"),
        "inpath": attr.string(),
        "outpath": attr.string(),
        "extra_env": attr.string_dict(),
        "_make": attr.label(
            default = "@make//:make",
            executable = True,
            cfg = "exec",
        ),
    },
)
