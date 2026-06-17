"""Product branding propagated from MODULE.bazel.

MODULE.bazel is the single source of truth for the product identity:

  * version  → the root module's `module(version = "...")`
  * name / full name → a `product.info(...)` tag (a bzlmod module() name is
    restricted to lowercase letters/digits/`._-` with no spaces, so the
    human-readable product name cannot be the module name itself)

A generated `@product_info` repo exposes these to BUILD files:

    load("@product_info//:defs.bzl",
         "PRODUCT_NAME", "PRODUCT_FULLNAME", "PRODUCT_VERSION")

Consumers: //main/postprocess (brand Setup.xcu placeholder substitution →
ooName/ooSetupVersion → window title, About box, %PRODUCTNAME runtime tokens).
"""

def _product_info_repo_impl(rctx):
    rctx.file("BUILD.bazel", "exports_files([\"defs.bzl\"])\n")
    rctx.file("defs.bzl", "\n".join([
        "# Generated from MODULE.bazel by //build:product.bzl — do not edit.",
        "PRODUCT_NAME = \"{}\"".format(rctx.attr.product_name),
        "PRODUCT_FULLNAME = \"{}\"".format(rctx.attr.product_fullname),
        "PRODUCT_VERSION = \"{}\"".format(rctx.attr.product_version),
        "",
    ]))

_product_info_repo = repository_rule(
    implementation = _product_info_repo_impl,
    attrs = {
        "product_name": attr.string(mandatory = True),
        "product_fullname": attr.string(mandatory = True),
        "product_version": attr.string(mandatory = True),
    },
)

def _product_impl(module_ctx):
    name = ""
    full_name = ""
    version = ""
    for mod in module_ctx.modules:
        if mod.is_root:
            version = mod.version
            for info in mod.tags.info:
                name = info.name
                full_name = info.full_name

    if not name:
        fail("MODULE.bazel must declare product.info(name = ..., full_name = ...) " +
             "for the //build:product.bzl 'product' extension.")
    if not version:
        fail("The root module() must declare a version = \"...\" — it is the " +
             "product version source for //build:product.bzl.")

    _product_info_repo(
        name = "product_info",
        product_name = name,
        product_fullname = full_name,
        product_version = version,
    )

_info = tag_class(attrs = {
    "name": attr.string(
        mandatory = True,
        doc = "Human-readable product name (window title / %PRODUCTNAME), e.g. 'Apache OpenOffice'.",
    ),
    "full_name": attr.string(
        mandatory = True,
        doc = "Full product name shown in the About box (ooFullname), e.g. 'Apache OpenOffice'.",
    ),
})

product = module_extension(
    implementation = _product_impl,
    tag_classes = {"info": _info},
)
