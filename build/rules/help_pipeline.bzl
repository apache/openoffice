"""The office help: .xhp sources -> the compiled help/<lang>/ directory.

Mirrors helpcontent2's dmake build (auxiliary/makefile.mk, util/target.pmk)
for one language.  Three rules:

  help_xhp_tree  lays the .xhp sources out as <name>/<lang>/text/...  HelpLinker
                 resolves every page AND every cross-module embed against
                 <src>/<lang>/, so each module's link sees the whole tree.
                 For en-US upstream's "merge" (helpex) is a plain copy, so this
                 is just a re-rooting of source/text.
  help_trees     runs upstream's helpers/update_tree.pl once: every topic title
                 in the auxiliary *.tree files is replaced by the page's real
                 <title>, and topics whose page no longer exists are commented
                 out.  That is the Contents tab.
  help_module    one module (swriter, scalc, ...): build/rules/help_link.pl runs
                 HelpLinker then HelpIndexerTool in a private work dir and copies
                 out exactly the declared files.

Outputs land at <package>/help/<installed lang dir>/..., ready for a
tree_install into the staged install.
"""

_TOOL_DLLS = [
    # (attr, runtime name) — sal.dll's import library records LIBRARY sal3.
    ("_sal_dll", "sal3.dll"),
]

def _xhp_root(ctx):
    return ctx.label.name

# ── help_xhp_tree ────────────────────────────────────────────────────────────

HelpXhpTreeInfo = provider(
    doc = "The re-rooted .xhp tree: root dir (exec path) and the files under it.",
    fields = ["root", "lang", "files"],
)

def _help_xhp_tree_impl(ctx):
    outs = []
    marker = "/source/"
    for f in ctx.files.srcs:
        i = f.short_path.find(marker)
        if i < 0:
            fail("%s is not under a source/ directory" % f.short_path)
        rel = f.short_path[i + len(marker):]  # "text/swriter/00/....xhp"
        out = ctx.actions.declare_file("%s/%s/%s" % (_xhp_root(ctx), ctx.attr.lang, rel))
        ctx.actions.symlink(output = out, target_file = f)
        outs.append(out)
    root = outs[0].path[:outs[0].path.find("/" + ctx.attr.lang + "/")]
    return [
        DefaultInfo(files = depset(outs)),
        HelpXhpTreeInfo(root = root, lang = ctx.attr.lang, files = depset(outs)),
    ]

help_xhp_tree = rule(
    implementation = _help_xhp_tree_impl,
    attrs = {
        "srcs": attr.label_list(allow_files = [".xhp"], mandatory = True),
        "lang": attr.string(default = "en-US"),
    },
)

# ── help_trees ───────────────────────────────────────────────────────────────

def _help_trees_impl(ctx):
    # update_tree.pl (ENVPRJ branch) reads   $ENVPRJ/source/auxiliary/*.tree,
    # $ENVPRJ/source/<topic id>.xhp and $ENVPRJ/source/text/shared/
    # tree_strings.xhp, and writes $ENVPRJ/$INPATH/misc/en-US/<name>.tree.
    # INPATH is therefore a path from the package to our output directory.
    prj = ctx.label.package
    outs = [
        ctx.actions.declare_file("%s/misc/en-US/%s" % (ctx.label.name, t.basename))
        for t in ctx.files.trees
    ]
    out_root = outs[0].path[:outs[0].path.rfind("/misc/en-US/")]
    inpath = "/".join([".."] * len(prj.split("/"))) + "/" + out_root
    ctx.actions.run(
        executable = ctx.file._perl,
        arguments = [ctx.file.script.path],
        inputs = ctx.files.trees + ctx.files.xhp + [ctx.file.script],
        outputs = outs,
        env = {
            "ENVPRJ": prj,
            "INPATH": inpath,
            "WITH_LANG": "",
            # read_loc's localized-title pass looks for localize.sdf under
            # $LOCALIZESDF; none exist for en-US, so point it at nothing.
            "LOCALIZESDF": prj + "/no-localize-sdf/auxiliary/localize.sdf",
        },
        use_default_shell_env = True,
        mnemonic = "HelpTrees",
        progress_message = "Updating help tree files %{label}",
    )
    return [DefaultInfo(files = depset(outs))]

help_trees = rule(
    implementation = _help_trees_impl,
    attrs = {
        "script": attr.label(allow_single_file = True, mandatory = True),
        "trees": attr.label_list(allow_files = [".tree"], mandatory = True),
        "xhp": attr.label_list(allow_files = [".xhp"], mandatory = True),
        "_perl": attr.label(
            default = "@strawberry-perl//:perl_exe",
            allow_single_file = True,
            cfg = "exec",
        ),
    },
)

# ── help_module ──────────────────────────────────────────────────────────────

# Lucene's file set for one optimized, single-segment compound index written
# by HelpIndexerTool (create + optimize + close).  Verified against the dmake
# output of all 8 linking modules, x86 and x64.  A module that links no page
# (shared) gets NO index: indexDocs finds nothing and the tool deletes it.
_IDXL_FILES = ["_0.cfs", "_0.cfx", "segments.gen", "segments_2"]

def _help_module_impl(ctx):
    m = ctx.attr.module
    tree = ctx.attr.xhp_tree[HelpXhpTreeInfo]
    dest = "help/%s/" % ctx.attr.install_lang

    # Tool dir: HelpLinker.exe + sal3.dll + CRT + an external manifest.
    tools_prefix = ctx.label.name + "_tools/"
    staged = []
    for f, name in [(ctx.executable._helplinker, "HelpLinker.exe")] + \
                   [(getattr(ctx.file, a), n) for a, n in _TOOL_DLLS] + \
                   [(f, f.basename) for f in ctx.files._crt_dlls]:
        out = ctx.actions.declare_file(tools_prefix + name)
        ctx.actions.symlink(output = out, target_file = f)
        staged.append(out)
    manifest = ctx.actions.declare_file(tools_prefix + "HelpLinker.exe.manifest")
    ctx.actions.symlink(output = manifest, target_file = ctx.file._app_manifest)
    staged.append(manifest)
    helplinker = staged[0]

    links = ctx.actions.declare_file(ctx.label.name + "_links.txt")
    ctx.actions.write(links, "\n".join(ctx.attr.links) + "\n")
    jar_list = ctx.actions.declare_file(ctx.label.name + "_jar.txt")
    ctx.actions.write(jar_list, "\n".join(ctx.attr.jar_pages) + "\n")

    args = ctx.actions.args()
    args.add(ctx.file._driver)
    args.add("--helplinker", helplinker)
    args.add("--tools-dir", helplinker.dirname)
    java_rt = ctx.toolchains["@bazel_tools//tools/jdk:runtime_toolchain_type"].java_runtime
    args.add("--java", java_rt.java_executable_exec_path)
    args.add("--indexer", ctx.file._indexer)
    args.add("--module", m)
    args.add("--lang", tree.lang)
    args.add("--src", tree.root)
    args.add("--sty", ctx.file._embed_xsl)
    args.add("--idxcaption", ctx.file._idxcaption_xsl)
    args.add("--idxcontent", ctx.file._idxcontent_xsl)
    args.add("--links", links)
    args.add("--jar-list", jar_list)
    args.add("--jar-root", ctx.attr.jar_root)

    work = ctx.actions.declare_directory(ctx.label.name + "_work")
    args.add("--work", work.path)

    # -add files: the module's own .tree (if help_trees produced one) plus the
    # explicit LINKADDEDFILES.
    adds = {}  # installed name -> File
    for f in ctx.files.trees:
        if f.basename == m + ".tree":
            adds[f.basename] = f
    for target, name in ctx.attr.added_files.items():
        adds[name] = target.files.to_list()[0]
    for name, f in adds.items():
        args.add("--add", "%s=%s" % (name, f.path))

    # Declared products, by their path inside the module's -zipdir.
    outs = []
    for rel in [m + ".db", m + ".ht", m + ".key", m + ".jar"] + adds.keys() + \
               ([m + ".idxl/" + f for f in _IDXL_FILES] if ctx.attr.links else []):
        out = ctx.actions.declare_file(dest + rel)
        args.add("--out", "%s=%s" % (rel, out.path))
        outs.append(out)

    added_inputs = adds.values()

    ctx.actions.run(
        executable = ctx.file._perl,
        arguments = [args],
        inputs = depset(
            staged + [links, jar_list, ctx.file._driver, ctx.file._indexer, ctx.file._embed_xsl,
                      ctx.file._idxcaption_xsl, ctx.file._idxcontent_xsl] + added_inputs,
            transitive = [tree.files, java_rt.files],
        ),
        outputs = outs + [work],
        use_default_shell_env = True,
        mnemonic = "HelpLink",
        progress_message = "Linking help module %s" % m,
    )
    return [DefaultInfo(files = depset(outs))]

help_module = rule(
    implementation = _help_module_impl,
    toolchains = ["@bazel_tools//tools/jdk:runtime_toolchain_type"],
    attrs = {
        "module": attr.string(mandatory = True),
        "links": attr.string_list(doc = "Pages to compile, as text/<module>/....xhp (LINKLINKFILES)."),
        "jar_pages": attr.string_list(doc = "Pages zipped into <module>.jar (ZIP1LIST, expanded)."),
        "jar_root": attr.string(
            doc = "Directory the jar's ZIP1LIST glob is rooted at; directory entries are made " +
                  "only below it.  Empty when ZIP1LIST names a single page.",
        ),
        "added_files": attr.label_keyed_string_dict(
            allow_files = True,
            doc = "Extra files copied into the module, label -> installed name (LINKADDEDFILES).",
        ),
        "xhp_tree": attr.label(mandatory = True, providers = [HelpXhpTreeInfo]),
        "trees": attr.label(
            allow_files = [".tree"],
            doc = "A help_trees target; the module takes <module>.tree from it if present.",
        ),
        "install_lang": attr.string(
            default = "en",
            doc = "Directory under help/.  Upstream installs en-US as help/en; " +
                  "xmlhelp's processLang falls back from en-US to en either way.",
        ),
        "_driver": attr.label(default = "//build/rules:help_link.pl", allow_single_file = True),
        "_perl": attr.label(
            default = "@strawberry-perl//:perl_exe",
            allow_single_file = True,
            cfg = "exec",
        ),
        "_helplinker": attr.label(
            default = "//main/l10ntools:HelpLinker",
            executable = True,
            cfg = "exec",
        ),
        "_sal_dll": attr.label(default = "//main/sal:sal", allow_single_file = True, cfg = "exec"),
        "_crt_dlls": attr.label(default = "//main/external/msvcp90:msvcp90", allow_files = True, cfg = "exec"),
        "_app_manifest": attr.label(
            default = "//main/external/msvcp90:vc90_app_manifest",
            allow_single_file = True,
            cfg = "exec",
        ),
        "_indexer": attr.label(
            default = "//main/l10ntools:HelpIndexerTool_deploy.jar",
            allow_single_file = True,
            cfg = "exec",
        ),
        "_embed_xsl": attr.label(default = "//main/xmlhelp:util/embed.xsl", allow_single_file = True),
        "_idxcaption_xsl": attr.label(default = "//main/xmlhelp:util/idxcaption.xsl", allow_single_file = True),
        "_idxcontent_xsl": attr.label(default = "//main/xmlhelp:util/idxcontent.xsl", allow_single_file = True),
    },
)
