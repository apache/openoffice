"""stage_install — assemble Bazel outputs into a runnable OpenOffice layout.

Produces a directory tree:

  <name>/
    program/          ← all DLLs, EXEs, services.rdb, types.rdb, offapi.rdb
    share/registry/   ← *.xcd configuration packages

Usage:

  stage_install(
      name          = "install",
      program       = [list of DLL/EXE labels],
      soffice_exe   = "//main/desktop:officeloader",   # → soffice.exe
      soffice_bin   = "//main/desktop:soffice",         # → soffice.bin
      types_rdb     = "//main/udkapi:udkapi_idl",       # → types.rdb
      offapi_rdb    = "//main/offapi:offapi_idl",        # → offapi.rdb
      services_rdb  = "//main/postprocess:services_rdb", # → services.rdb
      ooo_srdb      = "//main/postprocess:ooo_services_rdb", # → ooo-services.rdb
      registry      = ["//main/postprocess:all_xcd"],
  )

Then:
  bazel build //main/staging:install
  .\\bazel-bin\\main\\staging\\install\\program\\soffice.exe
"""

_PERL = "@strawberry-perl//:perl_exe"

def _stage_install_impl(ctx):
    install_dir = ctx.actions.declare_directory(ctx.label.name)
    manifest    = ctx.actions.declare_file(ctx.label.name + "_manifest.txt")
    perl        = ctx.file._perl
    script      = ctx.file._script

    lines = []

    # ── program/ — DLLs and plain EXEs (keep original basename) ─────────────
    for f in ctx.files.program:
        # Skip .lib import-libraries that sometimes leak through
        if f.extension not in ("lib", "pdb", "exp"):
            lines.append("program\t" + f.path)

    # ── program/ — renamed binaries ──────────────────────────────────────────
    for f in ctx.files.soffice_exe:
        if f.extension == "exe":
            lines.append("program\t{}\tsoffice.exe".format(f.path))
            break

    for f in ctx.files.soffice_bin:
        if f.extension == "exe":
            lines.append("program\t{}\tsoffice.bin".format(f.path))
            break

    # ── program/ — auto-stage vc90_app_manifest as <exe>.manifest ────────────
    # Any EXE without an explicit .manifest already in the program list gets
    # the generic VC90 CRT application manifest so Windows activates the SxS
    # context and MSVCR90.dll loads without r6034.
    if ctx.files.vc90_app_manifest:
        app_manifest_src = ctx.files.vc90_app_manifest[0]
        existing_manifests = {}
        for f in ctx.files.program:
            if f.extension == "manifest":
                existing_manifests[f.basename] = True
        for f in ctx.files.program:
            if f.extension == "exe":
                mname = f.basename + ".manifest"
                if mname not in existing_manifests:
                    lines.append("program\t{}\t{}".format(app_manifest_src.path, mname))
        # soffice.exe and soffice.bin are renamed at staging time
        if "soffice.exe.manifest" not in existing_manifests:
            lines.append("program\t{}\tsoffice.exe.manifest".format(app_manifest_src.path))
        if "soffice.bin.manifest" not in existing_manifests:
            lines.append("program\t{}\tsoffice.bin.manifest".format(app_manifest_src.path))

    # ── program/ — RDB files (filter .rdb from multi-file targets) ───────────
    for f in ctx.files.types_rdb:
        if f.extension == "rdb":
            lines.append("program\t{}\ttypes.rdb".format(f.path))
            break

    for f in ctx.files.offapi_rdb:
        if f.extension == "rdb":
            lines.append("program\t{}\toffapi.rdb".format(f.path))
            break

    for f in ctx.files.services_rdb:
        if f.extension == "rdb":
            lines.append("program\t{}\tservices.rdb".format(f.path))
            break

    for f in ctx.files.ooo_srdb:
        if f.extension == "rdb":
            lines.append("program\t{}\tooo-services.rdb".format(f.path))
            break

    # ── share/registry/ — XCD packages ───────────────────────────────────────
    for f in ctx.files.registry:
        if f.extension == "xcd":
            lines.append("share/registry\t" + f.path)

    ctx.actions.write(output = manifest, content = "\n".join(lines) + "\n")

    all_inputs = (
        ctx.files.program +
        ctx.files.soffice_exe +
        ctx.files.soffice_bin +
        ctx.files.types_rdb +
        ctx.files.offapi_rdb +
        ctx.files.services_rdb +
        ctx.files.ooo_srdb +
        ctx.files.registry +
        ctx.files.vc90_app_manifest
    )

    ctx.actions.run(
        executable       = perl,
        arguments        = [script.path, install_dir.path, manifest.path],
        inputs           = depset([perl, script, manifest] + all_inputs),
        outputs          = [install_dir],
        mnemonic         = "StageInstall",
        progress_message = "Staging OpenOffice install → {}".format(
            install_dir.short_path),
    )

    return [DefaultInfo(files = depset([install_dir]))]

stage_install = rule(
    implementation = _stage_install_impl,
    attrs = {
        # DLLs and EXEs that keep their built filename
        "program": attr.label_list(
            allow_files = True,
            doc = "DLLs and EXEs placed in program/ under their original names",
        ),
        # Renamed binaries
        "soffice_exe": attr.label_list(
            allow_files = True,
            doc = "officeloader.exe → program/soffice.exe",
        ),
        "soffice_bin": attr.label_list(
            allow_files = True,
            doc = "soffice.exe → program/soffice.bin",
        ),
        # RDB files (multi-file targets; filtered by .rdb extension)
        "types_rdb": attr.label_list(
            allow_files = True,
            doc = "UNO type registry → program/types.rdb",
        ),
        "offapi_rdb": attr.label_list(
            allow_files = True,
            doc = "Office API types → program/offapi.rdb",
        ),
        "services_rdb": attr.label_list(
            allow_files = True,
            doc = "Component service registry → program/services.rdb",
        ),
        "ooo_srdb": attr.label_list(
            allow_files = True,
            doc = "OOo extra services → program/ooo-services.rdb",
        ),
        # Configuration packages
        "registry": attr.label_list(
            allow_files = True,
            doc = "XCD files → share/registry/",
        ),
        # Generic VC90 CRT application manifest — auto-staged as <exe>.manifest
        # for every EXE in program/ that doesn't already have an explicit one.
        "vc90_app_manifest": attr.label(
            default       = "//main/external/msvcp90:vc90_app_manifest",
            allow_files   = True,
            doc           = "VC90 CRT app manifest staged as <exe>.manifest for EXEs without one",
        ),
        # Tool deps
        "_perl": attr.label(
            default         = _PERL,
            allow_single_file = True,
            cfg             = "exec",
        ),
        "_script": attr.label(
            default         = "//build/tools:stage_install.pl",
            allow_single_file = True,
        ),
    },
)
