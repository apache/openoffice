"""UNO fact graph — export the registration/staging edges Bazel already knows.

The UNO type graph (services, interfaces, methods) is recovered separately by
dumping the built .rdb with //main/registry:regview.  What no dump can supply is
the other half of the chain:

    createInstance("com.sun.star.frame.Desktop")
      -> service       (from the .rdb)
      -> implementation + library   (from the .component / postprocess dict)
      -> Bazel target  (from this aspect)
      -> staged path   (from this aspect)

This aspect emits that second half as JSON-lines fragments.  It is a pure
observer: it declares no actions on the critical path and produces nothing
unless its output group is explicitly requested, so a normal build pays zero.

Usage:

    bazel build //main/staging:install \
        --aspects=//build/rules:unograph.bzl%uno_provenance_aspect \
        --output_groups=uno_facts

then join the fragments with //build/tools:uno_graph.py.  Fact schema and the
contract between the two halves live in build/tools/unograph_schema.md.

Deliberately mirrors //main/staging:collect_files_aspect.bzl (same traversal
attrs) but records PROVENANCE -- which target produced which file -- rather
than collecting the files themselves.
"""

UnoFactsInfo = provider(
    doc = "Transitive UNO fact fragments (JSON lines).",
    fields = {"facts": "depset of File"},
)

# Same traversal as collect_files_aspect, and it must stay the same.  Two of
# these are load-bearing in ways that are easy to miss:
#   srcs                     -- //main/staging:install is a filegroup that
#                               aggregates the _install_* targets through srcs.
#   additional_linker_inputs -- how one DLL depends on another in this tree, via
#                               the <name>_implib filegroups.  Omitting it loses
#                               almost every leaf cc_binary: the aggregate
#                               staging targets still re-export the .dll files,
#                               so the graph looks populated while every library
#                               is attributed to //main/staging:all_files.
_TRAVERSAL_ATTRS = ["deps", "srcs", "data", "exports", "additional_linker_inputs"]

# Rule kinds that place files into the staged install tree.  Their outputs ARE
# the staged paths, which is why no separate staging scan is needed.
_STAGING_KINDS = ["flat_install", "tree_install", "res_stage"]

def _fact(ctx, name, payload):
    f = ctx.actions.declare_file(name)
    ctx.actions.write(f, json.encode(payload) + "\n")
    return f

def _uno_provenance_aspect_impl(target, ctx):
    transitive = [
        dep[UnoFactsInfo].facts
        for attr in _TRAVERSAL_ATTRS
        if hasattr(ctx.rule.attr, attr)
        for dep in getattr(ctx.rule.attr, attr)
        if type(dep) == "Target" and UnoFactsInfo in dep
    ]

    kind = getattr(ctx.rule, "kind", "")
    direct = []

    # ── provenance: label -> the files this target produces ──────────────────
    outs = [f.short_path for f in target[DefaultInfo].files.to_list()]
    if outs:
        direct.append(_fact(ctx, ctx.label.name + ".prov.unofacts.json", {
            "fact": "provenance",
            "label": str(target.label),
            "kind": kind,
            "staged": kind in _STAGING_KINDS,
            "outputs": outs,
        }))

    # ── registration: read the declared components dict off services_rdb ─────
    # This is NOT discovered by scanning .component files: postprocess declares
    # {component file: URI} explicitly, so impl -> library is exact.  Reading
    # the attr also cannot pick up commented-out example entries the way a grep
    # over the BUILD file does.
    if kind == "services_rdb" and hasattr(ctx.rule.attr, "components"):
        regs = []
        for comp_target, uri in ctx.rule.attr.components.items():
            files = comp_target.files.to_list()
            if not files:
                continue
            regs.append({
                "component": files[0].short_path,
                "label": str(comp_target.label),
                "uri": uri,
            })
        direct.append(_fact(ctx, ctx.label.name + ".reg.unofacts.json", {
            "fact": "registration",
            "label": str(target.label),
            "components": regs,
        }))

    facts = depset(direct, transitive = transitive)

    # OutputGroupInfo is what makes this opt-in: without --output_groups=uno_facts
    # Bazel never requests these files, so the write actions never execute.
    return [
        UnoFactsInfo(facts = facts),
        OutputGroupInfo(uno_facts = facts),
    ]

uno_provenance_aspect = aspect(
    implementation = _uno_provenance_aspect_impl,
    attr_aspects = _TRAVERSAL_ATTRS,
    provides = [UnoFactsInfo],
    doc = "Emits UNO provenance/registration facts into the `uno_facts` output group.",
)
