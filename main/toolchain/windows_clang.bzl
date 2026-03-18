load("@bazel_tools//tools/cpp:toolchains.bzl", "register_toolchains")

def _impl(ctx):
    """Defines a Clang-cl toolchain for Windows."""
    native.genrule(
        name = "clang-cl-toolchain",
        srcs = [],
        outs = ["toolchain"],
        cmd = """
            echo 'toolchain_type = "@bazel_tools//tools/cpp:toolchain_type"' > $@
            echo 'toolchain = {' >> $@
            echo '  "compiler": "clang-cl",' >> $@
            echo '  "cpu": "x86_windows",' >> $@
            echo '  "compiler_path": "%{compiler_path}",' >> $@
            echo '  "compiler_rt": "%{compiler_rt}",' >> $@
            echo '  "includes": ["%{includes}"],' >> $@
            echo '  "linker_path": "%{linker_path}",' >> $@
            echo '  "linker_rt": "%{linker_rt}",' >> $@
            echo '  "libraries": ["%{libraries}"],' >> $@
            echo '  "system_includes": ["%{system_includes}"],' >> $@
            echo '  "defines": ["%{defines}"],' >> $@
            echo '  "cxxopts": ["%{cxxopts}"],' >> $@
            echo '  "linkopts": ["%{linkopts}"],' >> $@
            echo '}' >> $@
        """,
    )

def _register_toolchains(ctx):
    _define_clang_cl_toolchain(ctx)
    register_toolchains(ctx)

def register_windows_clang_toolchain():
    """Registers the Windows Clang toolchain."""
    _register_toolchains(ctx)
