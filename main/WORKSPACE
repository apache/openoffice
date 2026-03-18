# WORKSPACE
workspace(name = "aoo")

# ── Bazel rule sets ──────────────────────────────────────────────────────────
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")


http_archive(
    name = "rules_cc",
    sha256 = "283fa1cdaaf172337898749cf4b9b1ef5ea269da59540954e51fba0e7b8f277a",
    strip_prefix = "rules_cc-0.2.17",
    url = "https://github.com/bazelbuild/rules_cc/releases/download/0.2.17/rules_cc-0.2.17.tar.gz",
)

load("@rules_cc//cc:extensions.bzl", "compatibility_proxy_repo")

compatibility_proxy_repo()
# ── AOO toolchains ────────────────────────────────────────────────────────────
load("@bazel_tools//tools/cpp:toolchains.bzl", "register_toolchains")

# Load your custom toolchain
load("//toolchains:windows_clang.bzl", "register_windows_clang_toolchain")

# Register it
register_windows_clang_toolchain()
# ─── Platform-specific configurations ─────────────────────────────────────────
# Platform-specific configuration
load("@bazel_tools//tools/cpp:platforms.bzl", "platform")


# ── Boost (bundled tarball) ───────────────────────────────────────────────────
# We reference the already-downloaded tarball in ext_sources/
#new_local_repository(
#    name = "boost",
#    path = "boost",          # populated by bootstrap; stub for now
#    build_file = "//third_party:boost.BUILD",
#)
