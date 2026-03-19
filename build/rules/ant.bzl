def _ant_build_impl(ctx):
    """Wraps existing Ant build files."""
    
    outputs = ctx.outputs.outs
    
    inputs = depset(
        ctx.files.srcs + ctx.files.build_files,
        transitive = [dep[DefaultInfo].files for dep in ctx.attr.deps],
    )
    
    args = ctx.actions.args()
    args.add("-f", ctx.file.build_file.path)
    args.add(ctx.attr.ant_target)
    
    # Pass properties
    for key, value in ctx.attr.properties.items():
        args.add("-D%s=%s" % (key, value))
    
    ctx.actions.run(
        inputs = inputs,
        outputs = outputs,
        executable = ctx.executable._ant,
        arguments = [args],
        mnemonic = "OOoAnt",
        progress_message = "Ant build: %s/%s" % (
            ctx.attr.directory, ctx.attr.ant_target
        ),
    )
    
    return [DefaultInfo(files = depset(outputs))]


ant_build = rule(
    implementation = _ant_build_impl,
    attrs = {
        "srcs": attr.label_list(allow_files = True),
        "build_file": attr.label(allow_single_file = True, mandatory = True),
        "build_files": attr.label_list(allow_files = True),
        "deps": attr.label_list(),
        "outs": attr.output_list(mandatory = True),
        "directory": attr.string(mandatory = True),
        "ant_target": attr.string(default = "all"),
        "properties": attr.string_dict(),
        "_ant": attr.label(
            default = "@ant//:ant",
            executable = True,
            cfg = "exec",
        ),
    },
)
