"""VS2008 non-redistributable debug CRT repository rule.

Stages MSVCR90D.dll and MSVCP90D.dll into a Bazel external repository so the
staging rule can include them alongside the application when building with
--compilation_mode=dbg (toolchain uses /MDd which requires the debug CRT).

These DLLs are NOT in System32 and NOT part of any redistributable package;
they are only available on machines with a full VS2008 installation.

The default path is derived from VS_PATH (same env var used by vs_config_repo).
Override per-machine in user.bazelrc:
  build --repo_env=MSVC_DEBUG_CRT_PATH=C:\\path\\to\\Microsoft.VC90.DebugCRT
  build --repo_env=VS_PATH=C:\\path\\to\\Microsoft Visual Studio 9.0

When MSVC_DEBUG_CRT_PATH is not set and the default path does not exist
(CI, release machines), this repository produces an empty 'dlls' filegroup
so non-debug builds are unaffected.
"""

_DEFAULT_VS = "C:\\Program Files (x86)\\Microsoft Visual Studio 9.0"

_DLLS = ["msvcr90d.dll", "msvcp90d.dll", "msvcm90d.dll"]

_BUILD_DLLS = """\
filegroup(
    name = "dlls",
    srcs = glob(["*.dll"]),
    visibility = ["//visibility:public"],
)
"""

_BUILD_EMPTY = """\
# MSVC_DEBUG_CRT_PATH not set / path not found; debug CRT DLLs unavailable.
# Add to user.bazelrc to enable:
#   build --repo_env=MSVC_DEBUG_CRT_PATH=C:\\\\path\\\\to\\\\Microsoft.VC90.DebugCRT
filegroup(
    name = "dlls",
    srcs = [],
    visibility = ["//visibility:public"],
)
"""

def _msvc_debug_crt_impl(rctx):
    vs = rctx.os.environ.get("VS_PATH", _DEFAULT_VS)
    default_crt = vs + "\\VC\\redist\\Debug_NonRedist\\x86\\Microsoft.VC90.DebugCRT"
    path = rctx.os.environ.get("MSVC_DEBUG_CRT_PATH", default_crt)

    for fname in _DLLS:
        res = rctx.execute(["cmd", "/c", "copy", "/Y", path + "\\" + fname, fname])
        if res.return_code != 0:
            # Path missing or inaccessible — produce empty repo so non-debug builds pass.
            rctx.file("BUILD.bazel", _BUILD_EMPTY)
            return

    rctx.file("BUILD.bazel", _BUILD_DLLS)

msvc_debug_crt_repo = repository_rule(
    implementation = _msvc_debug_crt_impl,
    environ = ["MSVC_DEBUG_CRT_PATH", "VS_PATH"],
)
