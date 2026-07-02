"""Repository rule that generates VS2008 / Windows SDK path constants.

Override per-machine in user.bazelrc (use forward slashes, double-quote the
flag value so bazelrc doesn't strip backslashes):

  build --repo_env="VS_PATH=C:/Program Files (x86)/Microsoft Visual Studio 9.0"
  build --repo_env="SDK_PATH=C:/Program Files/Microsoft SDKs/Windows/v7.0"
  build --repo_env="MSVC_TMP=C:/Users/<you>/AppData/Local/Temp"
  build --repo_env="PERL_PATH=C:/msys64/usr/bin/perl.exe"
  build --repo_env="VS_MODERN_PATH=C:/Program Files (x86)/Microsoft Visual Studio/2019/BuildTools"

The generated @vs_config//:paths.bzl exposes VS, VC, SDK, MSVC_TMP,
PERL, DEBUG_CRT_DIR, LINK_WRAPPER, and PDB_LINK.

PDB_LINK is a modern MSVC link.exe (auto-discovered under VS_MODERN_PATH).
The C++ toolchain routes generate_pdb (debug .pdb) links to it instead of
VC9 link.exe: the VC9 linker's single mspdbsrv cannot write PDBs in parallel
(LNK1318 0x6BA, no /FS), while a modern link.exe reads the VC9 /Z7 objects
natively and its mspdbsrv handles parallel PDB writes — so debug builds run
at full --jobs.  (LLVM lld-link was evaluated and rejected: it cannot write
PDBs from VC9-era CodeView — "Stream too short".)  Non-PDB links stay on VC9
link.exe via LINK_WRAPPER, below, so shipped binaries are unchanged.

LINK_WRAPPER is a generated batch file that wraps link.exe and gives every
link action its own _MSPDBSRV_ENDPOINT_ (derived from the unique per-target
linker param-file name).  This stops concurrent /DEBUG links from sharing one
mspdbsrv.exe instance — the shared server falls over under parallelism with
LNK1318 RPC_S_SERVER_UNAVAILABLE (0x6BA), which is the only reason debug builds
ever needed --jobs=1.  Routing all links through the wrapper lets debug builds
run fully parallel.  VS2008/VC9 has no /FS flag (added in VS2013), so the
per-process endpoint is the only fix available here.

Endpoint isolation prevents shared-server *corruption*, but each isolated link
must still spawn its own mspdbsrv.exe, and a wide parallel link phase can still
transiently lose the RPC handshake (LNK1318 0x6BA) when many servers start at
once.  So the wrapper also retries link.exe on LNK1318 with a linear backoff,
handing each attempt a *fresh* endpoint (…_a1, _a2, …) so a retry never
reconnects to a half-dead server from the previous try.  Non-PDB link errors
(unresolved externals, etc.) are detected and returned immediately without
wasting retries.  Tunable via LINK_MAX_RETRY / LINK_RETRY_BASE_SEC env vars.
"""

_DEFAULT_VS   = "C:\\Program Files (x86)\\Microsoft Visual Studio 9.0"
_DEFAULT_SDK  = "C:\\Program Files\\Microsoft SDKs\\Windows\\v7.0"
_DEFAULT_TMP  = "C:\\Temp"
_DEFAULT_PERL = "C:\\msys64\\usr\\bin\\perl.exe"
# Modern MSVC (VS2017+/BuildTools) install root — only used to source a newer
# link.exe for the generate_pdb path (see _find_modern_link / PDB_LINK below).
_DEFAULT_VS_MODERN = "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\BuildTools"

def _find_modern_link(rctx, vs_modern):
    """Locate the newest VS2017+ x86-target link.exe under a BuildTools/VS root.

    The VC9 link.exe cannot write PDBs in parallel (single mspdbsrv, no /FS, the
    LNK1318 bug); LLVM lld-link cannot write PDBs from VC9-era CodeView at all
    ("Stream too short"). A modern MSVC link.exe reads the VC9 /Z7 objects natively
    and its mspdbsrv handles parallel PDB writes, so the generate_pdb link path
    uses it. Returns a placeholder if none is found (only breaks generate_pdb builds).
    """
    tools = rctx.path(vs_modern + "\\VC\\Tools\\MSVC")
    if not tools.exists:
        return "modern_link_not_found.bat"
    best = None
    for ver in tools.readdir():
        for host in ["Hostx64", "Hostx86"]:
            link = ver.get_child("bin").get_child(host).get_child("x86").get_child("link.exe")
            if link.exists:
                if best == None or ver.basename > best[0]:
                    best = (ver.basename, str(link))
                break
    if best == None:
        return "modern_link_not_found.bat"
    return best[1]

def _vs_config_impl(rctx):
    vs   = rctx.os.environ.get("VS_PATH",   _DEFAULT_VS)
    sdk  = rctx.os.environ.get("SDK_PATH",  _DEFAULT_SDK)
    tmp  = rctx.os.environ.get("MSVC_TMP",  _DEFAULT_TMP)
    perl = rctx.os.environ.get("PERL_PATH", _DEFAULT_PERL)
    vs_modern = rctx.os.environ.get("VS_MODERN_PATH", _DEFAULT_VS_MODERN)
    vc   = vs + "\\VC"
    pdb_link = _find_modern_link(rctx, vs_modern)
    debug_crt = vc + "\\redist\\Debug_NonRedist\\x86\\Microsoft.VC90.DebugCRT"

    def q(p):
        return p.replace("\\", "\\\\")

    # link.exe wrapper: per-action _MSPDBSRV_ENDPOINT_ so concurrent /DEBUG links
    # each get a private mspdbsrv.exe instead of sharing one (LNK1318 0x6BA), plus
    # a bounded retry-with-backoff on LNK1318 (fresh endpoint per attempt) for the
    # residual "server unavailable" that isolation alone can't prevent.
    # The endpoint is derived from the linker @param-file name (unique per target,
    # always on the command line); falls back to %RANDOM% if no @file is present.
    # Output is captured so we can tell a transient PDB failure (retry) from a real
    # link error (return immediately).  Dynamic wait grows linearly per attempt.
    link_exe = vc + "\\bin\\link.exe"
    bat = "\r\n".join([
        "@echo off",
        "setlocal enabledelayedexpansion",
        "set \"ep=\"",
        "for %%A in (%*) do (",
        "  set \"arg=%%A\"",
        "  if \"!arg:~0,1!\"==\"@\" set \"ep=!arg!\"",
        ")",
        "if not defined ep set \"ep=%RANDOM%%RANDOM%\"",
        "set \"ep=!ep:@=!\"",
        "set \"ep=!ep:\\=_!\"",
        "set \"ep=!ep:/=_!\"",
        "set \"ep=!ep:.=_!\"",
        "set \"ep=!ep::=_!\"",
        # Tunables (override via --action_env at build time), with sane defaults.
        "if not defined LINK_MAX_RETRY set \"LINK_MAX_RETRY=5\"",
        "if not defined LINK_RETRY_BASE_SEC set \"LINK_RETRY_BASE_SEC=2\"",
        "set \"log=%TEMP%\\bzl_link_%RANDOM%%RANDOM%.log\"",
        "set \"n=0\"",
        ":retry",
        "set /a n+=1",
        # Fresh endpoint each attempt so a retry spawns a brand-new mspdbsrv.exe
        # instead of reconnecting to a hung/half-dead one from the previous try.
        "set \"_MSPDBSRV_ENDPOINT_=bzl_!ep!_a!n!\"",
        "\"" + link_exe + "\" %* > \"!log!\" 2>&1",
        "set \"rc=!ERRORLEVEL!\"",
        "if !rc! equ 0 goto done",
        # Only LNK1318 (PDB RPC) is transient; anything else is a real failure.
        "findstr /c:\"LNK1318\" \"!log!\" >nul",
        "if errorlevel 1 goto done",
        "if !n! geq !LINK_MAX_RETRY! goto done",
        "set /a wait=!n!*!LINK_RETRY_BASE_SEC!",
        "set /a pings=!wait!+1",
        "echo [msvc_link] LNK1318 PDB RPC error; retry !n!/!LINK_MAX_RETRY! after !wait!s (endpoint bzl_!ep!_a!n!)>&2",
        "ping -n !pings! 127.0.0.1 >nul",
        "goto retry",
        ":done",
        "type \"!log!\"",
        "del \"!log!\" >nul 2>&1",
        "exit /b !rc!",
        "",
    ])
    rctx.file("msvc_link.bat", bat)
    link_wrapper = str(rctx.path("msvc_link.bat"))

    content = "\n".join([
        "# Auto-generated by //build:vs_config_repo.bzl — do not edit.",
        "# Set VS_PATH / SDK_PATH / MSVC_TMP / PERL_PATH via --repo_env in user.bazelrc.",
        'VS       = "{}"'.format(q(vs)),
        'VC       = "{}"'.format(q(vc)),
        'SDK      = "{}"'.format(q(sdk)),
        'MSVC_TMP = "{}"'.format(q(tmp)),
        'PERL     = "{}"'.format(q(perl)),
        'DEBUG_CRT_DIR = "{}"'.format(q(debug_crt)),
        'LINK_WRAPPER = "{}"'.format(q(link_wrapper)),
        'PDB_LINK = "{}"'.format(q(pdb_link)),
        "",
    ])
    rctx.file("paths.bzl", content)
    rctx.file("BUILD.bazel", "")

vs_config_repo = repository_rule(
    implementation = _vs_config_impl,
    environ = ["VS_PATH", "SDK_PATH", "MSVC_TMP", "PERL_PATH", "VS_MODERN_PATH"],
)
