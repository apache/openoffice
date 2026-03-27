
def zlib_repository():
    http_archive(
        name = "zlib",
        url = "https://zlib.net/zlib-1.3.1.tar.gz",
        sha256 = "9a93b2b7dfdac77ceba5a558a580e74667dd6fede4585b91eefb60f03b72df23",
        strip_prefix = "zlib-1.3.1",
        build_file = "//ext_Libraries/zlib/prj:BUILD.bazel",
    )