"""Product branding propagated from MODULE.bazel.

MODULE.bazel is the single source of truth for the product identity:

  * version  → the root module's `module(version = "...")`
  * name / full name → a `product.info(...)` tag (a bzlmod module() name is
    restricted to lowercase letters/digits/`._-` with no spaces, so the
    human-readable product name cannot be the module name itself)

A generated `@product_info` repo exposes these to BUILD files, together with
every value upstream derives from the version (installer openoffice.lst,
solenv minor.mk), computed HERE so that no BUILD or ini file carries a copy:

    PRODUCT_NAME, PRODUCT_FULLNAME, PRODUCT_VERSION   e.g. "5.1.0"
    PRODUCT_BASE_VERSION    <major>.<minor>, "5.1"    (OOOBASEVERSION)
    PRODUCT_USERDIR_VERSION <major>, "5"              (USERDIRPRODUCTVERSION)
    PRODUCT_KEY             "<name> <version>"        (scriptitems.pm productkey)
    PRODUCT_SUPD            <major><minor><micro>, "510"
    SUPD_DEFINE             "SUPD=510", for local_defines

SUPD is upstream's UPD, i.e. minor.mk RSCVERSION, which every module compiles
in as -DSUPD (settings.mk).  It is live code: Basic's GetSolarVersion()
returns it and the office's single-instance pipe name contains it.

Consumers: //main/postprocess (brand Setup.xcu placeholder substitution →
ooName/ooSetupVersion → window title, About box, %PRODUCTNAME runtime tokens),
//main/staging (version.ini, bootstrap.ini, fundamental.ini), and every
module that defines SUPD.
"""

def _product_info_repo_impl(rctx):
    v = rctx.attr.product_version
    parts = v.split(".")
    if len(parts) != 3 or not all([p.isdigit() and len(p) == 1 for p in parts]):
        fail("module(version) must be <major>.<minor>.<micro>, one digit each " +
             "(SUPD concatenates them, as minor.mk's RSCVERSION does); got " + v)
    major, minor, micro = parts
    supd = major + minor + micro
    rctx.file("BUILD.bazel", "exports_files([\"defs.bzl\"])\n")
    rctx.file("defs.bzl", "\n".join([
        "# Generated from MODULE.bazel by //build:product.bzl — do not edit.",
        "PRODUCT_NAME = \"{}\"".format(rctx.attr.product_name),
        "PRODUCT_FULLNAME = \"{}\"".format(rctx.attr.product_fullname),
        "PRODUCT_VERSION = \"{}\"".format(v),
        "PRODUCT_BASE_VERSION = \"{}.{}\"".format(major, minor),
        "PRODUCT_USERDIR_VERSION = \"{}\"".format(major),
        "PRODUCT_KEY = \"{} {}\"".format(rctx.attr.product_name, v),
        "PRODUCT_SUPD = \"{}\"".format(supd),
        "SUPD_DEFINE = \"SUPD={}\"".format(supd),
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
