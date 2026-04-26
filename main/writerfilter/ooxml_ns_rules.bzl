# Generates per-namespace OOXMLFactory_<ns>.cxx and .hxx genrules.
# Called from BUILD.bazel; top-level for loops are forbidden in BUILD files.

OOXML_NS = [
    "wml",
    "dml-stylesheet",
    "dml-styleDefaults",
    "dml-shape3DLighting",
    "dml-shape3DScene",
    "dml-shape3DStyles",
    "dml-shape3DCamera",
    "dml-baseStylesheet",
    "dml-textCharacter",
    "dml-shapeEffects",
    "dml-shapeLineProperties",
    "dml-shapeProperties",
    "dml-baseTypes",
    "dml-documentProperties",
    "dml-graphicalObject",
    "dml-shapeGeometry",
    "dml-wordprocessingDrawing",
    "sml-customXmlMappings",
    "shared-math",
    "shared-relationshipReference",
    "dml-chartDrawing",
    "vml-main",
    "vml-officeDrawing",
    "vml-wordprocessingDrawing",
]

def ooxml_ns_factory_rules():
    for ns in OOXML_NS:
        safe = ns.replace("-", "_")

        native.genrule(
            name = "gen_ooxml_factory_%s_cxx" % safe,
            srcs = [
                "source/ooxml/factoryimpl_ns.xsl",
                "source/ooxml/factorytools.xsl",
                ":gen_model_preprocessed",
            ],
            outs = ["OOXMLFactory_%s.cxx" % ns],
            tools = ["@libxslt//:xsltproc"],
            cmd_bat = (
                "$(location @libxslt//:xsltproc) --novalid " +
                "--stringparam file OOXMLFactory_%s.cxx " % ns +
                "\"$(location source/ooxml/factoryimpl_ns.xsl)\" " +
                "\"$(location :gen_model_preprocessed)\" > \"$@\""
            ),
        )

        native.genrule(
            name = "gen_ooxml_factory_%s_hxx" % safe,
            srcs = [
                "source/ooxml/factory_ns.xsl",
                "source/ooxml/factorytools.xsl",
                ":gen_model_preprocessed",
            ],
            outs = ["OOXMLFactory_%s.hxx" % ns],
            tools = ["@libxslt//:xsltproc"],
            cmd_bat = (
                "$(location @libxslt//:xsltproc) --novalid " +
                "--stringparam file OOXMLFactory_%s.hxx " % ns +
                "\"$(location source/ooxml/factory_ns.xsl)\" " +
                "\"$(location :gen_model_preprocessed)\" > \"$@\""
            ),
        )
