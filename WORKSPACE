# WORKSPACE
workspace(name = "aoo")

# ── Bazel rule sets ──────────────────────────────────────────────────────────
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")


http_archive(
    name = "rules_foreign_cc",
    sha256 = "283fa1cdaaf172337898749cf4b9b1ef5ea269da59540954e51fba0e7b8f277a",
    strip_prefix = "rules_foreign_cc-0.2.17",
    url = "https://github.com/bazelbuild/rules_foreign_cc/releases/download/0.2.17/rules_foreign_cc-0.2.17.tar.gz",
)

load("@rules_foreign_cc//cc:extensions.bzl", "compatibility_proxy_repo")
rules_foreign_cc_dependencies()
# ── AOO toolchains ────────────────────────────────────────────────────────────
# ---- External tools ----
# GNU Make
http_archive(
    name = "make",
    build_file = "//bazel/third_party:make.BUILD",
    urls = ["https://ftp.gnu.org/gnu/make/make-4.4.tar.gz"],
)

# Apache Ant
http_archive(
    name = "ant",
    build_file = "//bazel/third_party:ant.BUILD",
    urls = ["https://downloads.apache.org/ant/binaries/apache-ant-1.10.14-bin.tar.gz"],
)

# Register it
register_windows_clang_toolchain()
# ─── Platform-specific configurations ─────────────────────────────────────────
# Platform detection
load("//bazel/platforms:detect.bzl", "detect_platform")
detect_platform(name = "ooo_platform")


