"""Office extensions (.oxt): place files at archive paths, process XCU, zip.

An .oxt is a zip whose layout IS its interface: META-INF/manifest.xml names
each component, configuration file and help directory by its path in the
archive, and the extension manager resolves them there.  So every rule here
works in terms of ARCHIVE PATHS, carried by OxtEntriesInfo, and oxt_package
only zips what those say -- there is no directory tree on disk to walk, and so
no way for a stale output of an earlier build to slip into the archive.

  ext_files            put files at <dest>/<name> in the archive, optionally
                       through Ant-filterset-style token substitution.
  xcu_default          a configuration file's locale-independent form
                       (alllang.xsl, no locale) -- tg_config.mk's registry/data
                       step, which is what an extension ships for en-US.
  oxt_package          zip the entries into the .oxt.

Help for an extension is help_pipeline.bzl's help_extension, which returns the
same provider.
"""

OxtEntriesInfo = provider(
    doc = "Files of an extension archive, keyed by their path inside it.",
    fields = {"entries": "dict: archive path -> File"},
)

def _join(dest, name):
    return dest + "/" + name if dest else name

# ── ext_files ────────────────────────────────────────────────────────────────

def _ext_files_impl(ctx):
    entries = {}
    outs = []
    for f in ctx.files.srcs:
        if ctx.attr.strip_prefix:
            p = f.short_path
            if not p.startswith(ctx.attr.strip_prefix + "/"):
                fail("%s is not under strip_prefix %s" % (p, ctx.attr.strip_prefix))
            rel = p[len(ctx.attr.strip_prefix) + 1:]
        else:
            rel = f.basename
        if rel in ctx.attr.rename:
            rel = ctx.attr.rename[rel]
        path = _join(ctx.attr.dest, rel)
        if path in entries:
            fail("two sources map to %s" % path)
        out = ctx.actions.declare_file(ctx.label.name + "/" + path)
        if ctx.attr.substitutions:
            ctx.actions.expand_template(
                template = f,
                output = out,
                substitutions = ctx.attr.substitutions,
            )
        else:
            ctx.actions.symlink(output = out, target_file = f)
        entries[path] = out
        outs.append(out)
    return [DefaultInfo(files = depset(outs)), OxtEntriesInfo(entries = entries)]

ext_files = rule(
    implementation = _ext_files_impl,
    attrs = {
        "srcs": attr.label_list(allow_files = True, mandatory = True),
        "dest": attr.string(doc = "Directory in the archive; empty for the root."),
        "strip_prefix": attr.string(
            doc = "Keep each file's path below this short_path prefix.  Empty: basename only.",
        ),
        "rename": attr.string_dict(doc = "Relative name -> name in the archive."),
        "substitutions": attr.string_dict(
            doc = "Literal token -> value, e.g. Ant's <filterset> ({\"@ID@\": \"...\"}).  " +
                  "Every occurrence in every file is replaced; the bytes are otherwise kept.",
        ),
    },
)

# ── xcu_default ──────────────────────────────────────────────────────────────

_DATA = "registry/data/"
_SCHEMA = "registry/schema/"

def _after(f, marker):
    p = f.short_path
    i = p.find(marker)
    if i < 0:
        fail("%s is not under a %s directory" % (p, marker))
    return p[i + len(marker):]

def _xcu_default_impl(ctx):
    # alllang.xsl resolves both the xcs and schemaRoot params against ITS OWN
    # base URI, and needs the schema of the file's component -- the prop
    # templates decide from it which props are localized (see the
    # alllang_default comment in //main/officecfg).  An extension's schemas
    # live in two places (officecfg's, and the extension's own), so both are
    # laid out next to a copy of the stylesheet:
    #   <name>_xsl/util/alllang.xsl
    #   <name>_xsl/registry/schema/<package path>.xcs
    work = ctx.label.name + "_xsl/"
    xsl = ctx.actions.declare_file(work + "util/alllang.xsl")
    ctx.actions.symlink(output = xsl, target_file = ctx.file._alllang)
    staged = [xsl]
    seen = {}
    for f in ctx.files.schemas:
        rel = _after(f, _SCHEMA)
        if rel in seen:
            continue  # the same component from two sources: first one wins
        seen[rel] = True
        out = ctx.actions.declare_file(work + _SCHEMA + rel)
        ctx.actions.symlink(output = out, target_file = f)
        staged.append(out)

    outs = []
    for f in ctx.files.srcs:
        rel = _after(f, _DATA)
        xcs = rel[:-len(".xcu")] + ".xcs"
        if xcs not in seen:
            fail("%s: no schema %s among `schemas`" % (f.short_path, xcs))
        out = ctx.actions.declare_file(ctx.label.name + "/" + _DATA + rel)
        ctx.actions.run(
            executable = ctx.executable._xsltproc,
            arguments = [
                "--novalid",
                "--stringparam", "xcs", "../" + _SCHEMA + xcs,
                "--stringparam", "schemaRoot", "../" + _SCHEMA[:-1],
                "-o", out.path,
                xsl.path,
                f.path,
            ],
            inputs = staged + [f],
            outputs = [out],
            mnemonic = "XcuDefault",
            progress_message = "Processing %s" % f.short_path,
        )
        outs.append(out)
    return [DefaultInfo(files = depset(outs))]

xcu_default = rule(
    implementation = _xcu_default_impl,
    attrs = {
        "srcs": attr.label_list(
            allow_files = [".xcu"],
            mandatory = True,
            doc = "Data files, each under a registry/data/ directory.",
        ),
        "schemas": attr.label_list(
            allow_files = [".xcs"],
            mandatory = True,
            doc = "Every schema the files' components need, each under registry/schema/.",
        ),
        "_alllang": attr.label(
            default = "//main/officecfg:util/alllang.xsl",
            allow_single_file = True,
        ),
        "_xsltproc": attr.label(
            default = "@libxslt//:xsltproc",
            executable = True,
            cfg = "exec",
        ),
    },
)

# ── oxt_package ──────────────────────────────────────────────────────────────

def _oxt_package_impl(ctx):
    entries = {}
    for t in ctx.attr.contents:
        for path, f in t[OxtEntriesInfo].entries.items():
            if path in entries:
                fail("%s is contributed twice (again by %s)" % (path, t.label))
            entries[path] = f
    out = ctx.actions.declare_file(ctx.attr.out)
    args = ctx.actions.args()
    args.add("cC")  # create, deflate
    args.add(out)
    for path in sorted(entries.keys()):
        args.add("%s=%s" % (path, entries[path].path))
    ctx.actions.run(
        executable = ctx.executable._zipper,
        arguments = [args],
        inputs = entries.values(),
        outputs = [out],
        mnemonic = "OxtPackage",
        progress_message = "Packaging %s" % ctx.attr.out,
    )
    return [DefaultInfo(files = depset([out]))]

oxt_package = rule(
    implementation = _oxt_package_impl,
    attrs = {
        "out": attr.string(mandatory = True, doc = "Archive filename, e.g. \"wiki-publisher.oxt\"."),
        "contents": attr.label_list(mandatory = True, providers = [OxtEntriesInfo]),
        "_zipper": attr.label(
            default = "@bazel_tools//tools/zip:zipper",
            executable = True,
            cfg = "exec",
        ),
    },
)
