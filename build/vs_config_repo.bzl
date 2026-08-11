"""Repository rule that generates VS2008 / Windows SDK path constants.

Override per-machine in user.bazelrc (use forward slashes, double-quote the
flag value so bazelrc doesn't strip backslashes):

  build --repo_env="VS_PATH=C:/Program Files (x86)/Microsoft Visual Studio 9.0"
  build --repo_env="SDK_PATH=C:/Program Files/Microsoft SDKs/Windows/v7.0"
  build --repo_env="MSVC_TMP=C:/Users/<you>/AppData/Local/Temp"
  build --repo_env="PERL_PATH=C:/msys64/usr/bin/perl.exe"
  build --repo_env="VS_MODERN_PATH=C:/Program Files (x86)/Microsoft Visual Studio/2019/BuildTools"
  build --repo_env="WIN10_SDK_PATH=C:/Program Files (x86)/Windows Kits/10"
  build --repo_env="MSVC_TOOLSET_VERSION=14.29.30133"   # optional pin
  build --repo_env="WIN10_SDK_VERSION=10.0.19041.0"     # optional pin

The generated @vs_config//:paths.bzl exposes VS, VC, SDK, MSVC_TMP,
PERL, DEBUG_CRT_DIR, LINK_WRAPPER, and PDB_LINK.

## Modern toolchain discovery (win10 target, M1)

Two independent installs are located, both under their own env var and both
overridable with an explicit version pin:

  * the **MSVC toolset** under VS_MODERN_PATH — VC\\Tools\\MSVC\\<ver>, holding
    cl/link/lib/ml64 (per host x target arch), the C++ standard library headers
    and the vcruntime import libs.  Exposed as MSVC_TOOLSET_VER, MSVC_TOOLSET,
    MSVC_BIN/MSVC_LIB (dicts keyed "x86"/"x64") and MSVC_INCLUDE.
  * the **Windows 10 SDK** under WIN10_SDK_PATH.  Its layout is versioned and
    split in a way the VC9-era SDK v7.0 is not — one flat include\\ and lib\\
    there, four include trees and two lib trees here:

        Include\\<ver>\\{ucrt,um,shared,winrt}
        Lib\\<ver>\\{ucrt,um}\\<arch>
        bin\\<ver>\\<arch>              (rc.exe, mt.exe)

    Exposed as SDK10_VER, SDK10_ROOT, SDK10_INCLUDE (list, in the order they
    belong on INCLUDE), SDK10_LIB and SDK10_BIN (dicts keyed by arch).

Version selection is **newest-first by numeric component** (so 14.9 sorts below
14.29, which a plain string compare gets wrong), restricted to versions that are
actually complete: a toolset must have cl.exe for the arch plus its own headers,
and an SDK version must carry both an Include\\<ver> and a Lib\\<ver> tree.  The
bin\\ directory is deliberately not part of that test — it commonly holds several
more versions than Include/Lib do (six vs two on the reference machine).

When nothing usable is found the constants are set to the sentinel strings
MODERN_MSVC_TOOLSET_NOT_FOUND / WIN10_SDK_NOT_FOUND rather than "" — an empty
string silently degrades into a valid-looking relative path, while the sentinel
puts its own name into the first compiler error.

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
# Modern MSVC (VS2017+/BuildTools) install root.  Two consumers: the win10
# toolchain proper, and the newer link.exe the VC9 toolchains use for their
# generate_pdb path (see _find_msvc_toolset / _modern_link / PDB_LINK below).
_DEFAULT_VS_MODERN = "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\BuildTools"
_DEFAULT_WIN10_SDK = "C:\\Program Files (x86)\\Windows Kits\\10"

_NO_TOOLSET = "MODERN_MSVC_TOOLSET_NOT_FOUND"
_NO_SDK10 = "WIN10_SDK_NOT_FOUND"

# Target architectures discovered for the modern toolchain.  These strings are
# also the on-disk directory names in BOTH layouts (VC\Tools\MSVC\<ver>\lib\<arch>
# and Windows Kits\10\Lib\<ver>\um\<arch>), which is why they are not translated.
_ARCHES = ["x86", "x64"]

_DIGITS = "0123456789"

def _slash(p):
    """Normalize a path to forward slashes.

    Discovered paths come out of rctx.path() with forward slashes while env-var
    roots keep however the user spelled them (user.bazelrc uses '/'), so without
    this the same constant reads differently depending on where it came from.
    MSVC accepts either separator; the point is that the emitted file is stable.
    """
    return p.replace("\\", "/")

def _version_key(name):
    """Split a dotted all-numeric version into a padded list of ints, or None.

    None means "not a version directory" — the caller skips it.  That is how
    non-version siblings are filtered out without hardcoding their names
    (Windows Kits\\10\\bin holds `arm64`, `x64`, `x86` next to the versions).

    Padded to a fixed width so two versions with different component counts still
    compare component-by-component (`14.29.30133` is 3 components, `10.0.19041.0`
    is 4).  More than 4 is not a shape either layout uses, so it is rejected
    rather than silently truncated.
    """
    key = []
    for part in name.split("."):
        if not part:
            return None
        n = 0
        for ch in part.elems():
            digit = _DIGITS.find(ch)
            if digit < 0:
                return None
            n = n * 10 + digit
        key.append(n)
    if len(key) > 4:
        return None
    return key + [0] * (4 - len(key))

def _newer(a, b):
    """True if version key a is strictly newer than b.  b == None means "nothing yet"."""
    if b == None:
        return True
    for i in range(len(a)):
        if a[i] != b[i]:
            return a[i] > b[i]
    return False

def _find_msvc_toolset(rctx, vs_modern, pin):
    """Locate the newest complete VS2017+ MSVC toolset under a BuildTools/VS root.

    "Complete" = it has its own headers and an x64-targeting cl.exe — x64 because
    that is the arch this toolchain is for, so a hypothetical x86-only toolset is
    correctly not a candidate.  The host is chosen once (Hostx64 preferred — the
    build host is x64) and then used for every target, so an x86 target build is
    the x64-hosted cross compiler rather than a second host's native one.

    Returns a struct(version, root, host, bin{}, lib{}, include) or None.
    """
    tools = rctx.path(vs_modern + "\\VC\\Tools\\MSVC")
    if not tools.exists:
        return None

    best = None
    best_key = None
    for ver in tools.readdir():
        if pin and ver.basename != pin:
            continue
        key = _version_key(ver.basename)
        if key == None or not _newer(key, best_key):
            continue
        if not ver.get_child("include").get_child("vcruntime.h").exists:
            continue
        for host in ["Hostx64", "Hostx86"]:
            host_dir = ver.get_child("bin").get_child(host)
            if not host_dir.get_child("x64").get_child("cl.exe").exists:
                continue
            best = struct(
                version = ver.basename,
                root = _slash(str(ver)),
                host = host,
                bin = {a: _slash(str(host_dir.get_child(a))) for a in _ARCHES},
                lib = {a: _slash(str(ver.get_child("lib").get_child(a))) for a in _ARCHES},
                include = _slash(str(ver.get_child("include"))),
            )
            best_key = key
            break
    return best

def _find_win10_sdk(rctx, sdk_root, pin):
    """Locate the newest complete Windows 10 SDK version under a Windows Kits root.

    "Complete" = Include\\<ver> carries both the um and ucrt trees AND a matching
    Lib\\<ver> exists.  bin\\ is not part of the test: it accumulates versions that
    were never installed as header/lib sets (six vs two on the reference machine),
    so selecting on it would pick a version with no headers at all.

    Returns a struct(version, root, include[], lib{}, bin{}) or None.  `include` is
    ordered as it must appear on INCLUDE: ucrt (the C runtime) first, then um, then
    shared, then winrt.
    """
    include_root = rctx.path(sdk_root + "\\Include")
    lib_root = rctx.path(sdk_root + "\\Lib")
    if not include_root.exists or not lib_root.exists:
        return None

    best = None
    best_key = None
    for inc in include_root.readdir():
        version = inc.basename
        if pin and version != pin:
            continue
        key = _version_key(version)
        if key == None or not _newer(key, best_key):
            continue
        if not inc.get_child("um").get_child("windows.h").exists:
            continue
        if not inc.get_child("ucrt").get_child("stdio.h").exists:
            continue
        lib = lib_root.get_child(version)
        if not lib.get_child("um").exists or not lib.get_child("ucrt").exists:
            continue

        # bin\<ver>\<arch> is the modern layout; fall back to the flat bin\<arch>
        # of older kits so rc.exe/mt.exe are still found there.
        bin_root = rctx.path(sdk_root + "\\bin")
        versioned_bin = bin_root.get_child(version)
        bin_base = versioned_bin if versioned_bin.exists else bin_root

        best = struct(
            version = version,
            root = _slash(sdk_root),
            include = [_slash(str(inc.get_child(t))) for t in ["ucrt", "um", "shared", "winrt"]],
            lib = {a: [_slash(str(lib.get_child(t).get_child(a))) for t in ["ucrt", "um"]] for a in _ARCHES},
            bin = {a: _slash(str(bin_base.get_child(a))) for a in _ARCHES},
        )
        best_key = key
    return best

def _modern_link(toolset, target):
    """The modern link.exe for `target`, used by the VC9 toolchains' generate_pdb path.

    The VC9 link.exe cannot write PDBs in parallel (single mspdbsrv, no /FS, the
    LNK1318 bug); LLVM lld-link cannot write PDBs from VC9-era CodeView at all
    ("Stream too short"). A modern MSVC link.exe reads the VC9 /Z7 objects natively
    and its mspdbsrv handles parallel PDB writes, so the generate_pdb link path
    uses it. Returns a placeholder if none is found (only breaks generate_pdb builds).
    """
    if toolset == None:
        return "modern_link_not_found.bat"
    return toolset.bin[target] + "/link.exe"

def _vs_config_impl(rctx):
    vs   = rctx.os.environ.get("VS_PATH",   _DEFAULT_VS)
    sdk  = rctx.os.environ.get("SDK_PATH",  _DEFAULT_SDK)
    tmp  = rctx.os.environ.get("MSVC_TMP",  _DEFAULT_TMP)
    perl = rctx.os.environ.get("PERL_PATH", _DEFAULT_PERL)
    vs_modern = rctx.os.environ.get("VS_MODERN_PATH", _DEFAULT_VS_MODERN)
    sdk10 = rctx.os.environ.get("WIN10_SDK_PATH", _DEFAULT_WIN10_SDK)
    vc   = vs + "\\VC"

    toolset = _find_msvc_toolset(rctx, vs_modern, rctx.os.environ.get("MSVC_TOOLSET_VERSION"))
    sdk10_found = _find_win10_sdk(rctx, sdk10, rctx.os.environ.get("WIN10_SDK_VERSION"))

    pdb_link = _modern_link(toolset, "x86")
    pdb_link_x64 = _modern_link(toolset, "x64")
    debug_crt = vc + "\\redist\\Debug_NonRedist\\x86\\Microsoft.VC90.DebugCRT"

    def q(p):
        return p.replace("\\", "\\\\")

    def q_list(items):
        return "[" + ", ".join(['"{}"'.format(q(i)) for i in items]) + "]"

    def q_dict(d, value_fmt):
        return "{" + ", ".join(['"{}": {}'.format(k, value_fmt(d[k])) for k in sorted(d)]) + "}"

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
        # The action PATH is just VC\\bin; findstr/ping (used below for the LNK1318
        # retry classifier and its backoff) live in System32 — put it on PATH.
        # Appended, so the VC9 toolchain binaries still take precedence.
        "set \"PATH=%PATH%;%SystemRoot%\\System32\"",
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
        # Log name MUST be unique per concurrent link.  %RANDOM% is clock-seeded, so
        # links launched in the same tick (e.g. the 7 applauncher EXEs) collided on
        # one log path → the second '>' redirect hit ERROR_SHARING_VIOLATION and the
        # link never ran ("output X was not created").  !ep! is derived from the
        # per-target @param-file name, so it is unique across concurrent targets.
        "set \"log=%TEMP%\\bzl_link_!ep!.log\"",
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

    # ── modern MSVC toolset (win10 target) ───────────────────────────────────
    if toolset != None:
        modern = [
            "# Modern MSVC toolset — discovered under VS_MODERN_PATH.",
            "# Pin a specific one with --repo_env=MSVC_TOOLSET_VERSION=<ver>.",
            'MSVC_TOOLSET_VER = "{}"'.format(toolset.version),
            'MSVC_TOOLSET     = "{}"'.format(q(toolset.root)),
            'MSVC_HOST        = "{}"'.format(toolset.host),
            "MSVC_BIN         = {}".format(q_dict(toolset.bin, lambda v: '"{}"'.format(q(v)))),
            "MSVC_LIB         = {}".format(q_dict(toolset.lib, lambda v: '"{}"'.format(q(v)))),
            'MSVC_INCLUDE     = "{}"'.format(q(toolset.include)),
        ]
    else:
        modern = [
            "# No modern MSVC toolset found under VS_MODERN_PATH — the win10 toolchain",
            "# cannot resolve, and generate_pdb links on the VC9 toolchains fall back to",
            "# a placeholder.  Set --repo_env=VS_MODERN_PATH in user.bazelrc.",
            'MSVC_TOOLSET_VER = "{}"'.format(_NO_TOOLSET),
            'MSVC_TOOLSET     = "{}"'.format(_NO_TOOLSET),
            'MSVC_HOST        = "{}"'.format(_NO_TOOLSET),
            "MSVC_BIN         = {}".format(q_dict({a: _NO_TOOLSET for a in _ARCHES}, lambda v: '"{}"'.format(v))),
            "MSVC_LIB         = {}".format(q_dict({a: _NO_TOOLSET for a in _ARCHES}, lambda v: '"{}"'.format(v))),
            'MSVC_INCLUDE     = "{}"'.format(_NO_TOOLSET),
        ]

    # ── Windows 10 SDK (win10 target) ────────────────────────────────────────
    if sdk10_found != None:
        kit = [
            "# Windows 10 SDK — discovered under WIN10_SDK_PATH.",
            "# Pin a specific one with --repo_env=WIN10_SDK_VERSION=<ver>.",
            "# SDK10_INCLUDE is ordered for INCLUDE; SDK10_LIB values are per-arch lists.",
            'SDK10_VER     = "{}"'.format(sdk10_found.version),
            'SDK10_ROOT    = "{}"'.format(q(sdk10_found.root)),
            "SDK10_INCLUDE = {}".format(q_list(sdk10_found.include)),
            "SDK10_LIB     = {}".format(q_dict(sdk10_found.lib, q_list)),
            "SDK10_BIN     = {}".format(q_dict(sdk10_found.bin, lambda v: '"{}"'.format(q(v)))),
        ]
    else:
        kit = [
            "# No complete Windows 10 SDK found under WIN10_SDK_PATH (a version needs",
            "# BOTH an Include\\<ver> and a Lib\\<ver> tree).  The win10 toolchain cannot",
            "# resolve.  Set --repo_env=WIN10_SDK_PATH in user.bazelrc.",
            'SDK10_VER     = "{}"'.format(_NO_SDK10),
            'SDK10_ROOT    = "{}"'.format(_NO_SDK10),
            "SDK10_INCLUDE = {}".format(q_list([_NO_SDK10])),
            "SDK10_LIB     = {}".format(q_dict({a: [_NO_SDK10] for a in _ARCHES}, q_list)),
            "SDK10_BIN     = {}".format(q_dict({a: _NO_SDK10 for a in _ARCHES}, lambda v: '"{}"'.format(v))),
        ]

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
        'PDB_LINK_X64 = "{}"'.format(q(pdb_link_x64)),
        "",
    ] + modern + [""] + kit + [""])
    rctx.file("paths.bzl", content)
    rctx.file("BUILD.bazel", "")

vs_config_repo = repository_rule(
    implementation = _vs_config_impl,
    environ = [
        "VS_PATH",
        "SDK_PATH",
        "MSVC_TMP",
        "PERL_PATH",
        "VS_MODERN_PATH",
        "WIN10_SDK_PATH",
        "MSVC_TOOLSET_VERSION",
        "WIN10_SDK_VERSION",
    ],
)
