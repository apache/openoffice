"""
RSC resource compilation pipeline for Apache OpenOffice Bazel build.

Also exports rename_file: a tiny rule that symlinks a source file under a
different output name (useful when legacy .mk builds copied *_tmpl.hrc files
to a plain .hrc name that downstream .src files #include).


Pipeline:  .src files --(rscpp C preprocessor)--> .srs intermediates (one per .src)
                      --(rsc2 resource compiler)--> .res binary

Bazel runs each step as a separate action, giving full control over inputs,
outputs, and execution environment.  The rsc.exe launcher is NOT used — calling
rscpp and rsc2 directly avoids subprocess-spawning issues (CWD inheritance,
path resolution) that plagued the launcher approach.

Step 1 — RscPreprocess (one action per .src):
    rscpp.exe  -I<inc>...  -D<def>...  input.src  output.srs

Step 2 — Rsc2Compile (one action for all .srs):
    rsc2.exe  -fp=<combined.srs>  -fs=<output.res>  -lgEN_US  -LITTLEENDIAN
              -I<inc>...  -D<def>...  -lip=<images_dir>
              <all_intermediate_srs_files...>

Tool staging follows the same pattern as idl_pipeline.bzl:
  - rscpp.exe and rsc2.exe are staged into <name>_tools/ with all DLLs.
  - sal.dll is staged under the name sal3.dll (the LIBRARY name in its DEF).
  - VS2008 CRT DLLs + a .manifest file are staged alongside each EXE.
  - Images are staged FLAT into the same <name>_tools/ directory so that
    -lip=tools_dir finds them by basename without any subdirectory issues.
  - env = {"PATH": tools_dir} lets the Windows DLL loader find everything.

Usage in a BUILD.bazel:

    load("//build/rules:rsc_pipeline.bzl", "rsc_res")

    rsc_res(
        name     = "vcl_res",
        srcs     = glob(["source/src/*.src"]),
        hdrs     = glob(["inc/**/*.hrc"]),
        includes = ["main/vcl/inc"],
        images   = ["//main/default_images:vcl_images"],
        visibility = ["//visibility:public"],
    )

The rule emits a single <name>.res file in DefaultInfo.
An intermediate <name>.srs is also declared but not returned to callers.
"""

def _rename_file_impl(ctx):
    out = ctx.actions.declare_file(ctx.attr.out)
    ctx.actions.symlink(output = out, target_file = ctx.file.src)
    return [DefaultInfo(files = depset([out]))]

rename_file = rule(
    implementation = _rename_file_impl,
    attrs = {
        "src": attr.label(allow_single_file = True, doc = "Source file to link"),
        "out": attr.string(doc = "Output filename (basename only, within the package)"),
    },
    doc = "Expose a source file under a different name via a Bazel symlink action.",
)

# DLL files that need to be staged, with the runtime name Windows expects.
# sal.dll must be staged as sal3.dll — its import library records LIBRARY sal3.
_DLL_RENAME = [
    # (attr_name,        runtime_name)
    ("_sal_dll",         "sal3.dll"),
    ("_tl_dll",          "tl.dll"),
    ("_cppu_dll",        "cppu3.dll"),
    ("_cppuhelper_dll",  "cppuhelper3MSC.dll"),
    ("_salhelper_dll",   "salhelper3MSC.dll"),
    ("_comphelp_dll",    "comphelpMSC.dll"),
    ("_ucbhelper_dll",   "ucbhelperMSC.dll"),
    ("_basegfx_dll",     "basegfx.dll"),
    ("_vos_dll",         "vos3MSC.dll"),
    ("_i18nisolang_dll", "i18nisolang1MSC.dll"),
]

_DEFAULT_DEFINES = [
    "WNT", "GUI", "WIN32", "INTEL", "_X86_=1",
    "CPPU_ENV=msci",
]

def _rsc_res_impl(ctx):
    rscpp = ctx.executable._rscpp
    rsc2  = ctx.executable._rsc2

    # ── Build rename map: File → staged name ─────────────────────────────
    rename = {}
    for attr_name, runtime_name in _DLL_RENAME:
        f = getattr(ctx.file, attr_name)
        rename[f] = runtime_name

    # ── Stage rscpp, rsc2, and all DLLs into <name>_tools/ ───────────────
    tools_prefix = ctx.label.name + "_tools/"
    staged = {}

    all_to_stage = [rscpp, rsc2] + [getattr(ctx.file, a) for a, _ in _DLL_RENAME] + ctx.files._crt_dlls
    for f in all_to_stage:
        dst_name = rename.get(f, f.basename)
        out = ctx.actions.declare_file(tools_prefix + dst_name)
        ctx.actions.symlink(output = out, target_file = f)
        staged[dst_name] = out

    # Stage manifests for rscpp.exe and rsc2.exe so Windows finds VS2008 CRT.
    manifest_src = ctx.file._app_manifest
    for exe in ["rscpp.exe", "rsc2.exe"]:
        out = ctx.actions.declare_file(tools_prefix + exe + ".manifest")
        ctx.actions.symlink(output = out, target_file = manifest_src)
        staged[exe + ".manifest"] = out

    tools_dir = staged["rscpp.exe"].dirname

    # ── Stage image files into tools_dir ─────────────────────────────────
    # If images_root is set, preserve the path relative to that root so that
    # subdirectory references in .src (e.g. "minimizer/foo.png") resolve under
    # tools_dir/minimizer/.  Without images_root, stage flat by basename
    # (original behaviour for vcl and others that use bare "foo.png" paths).
    # When multiple filegroups supply an image with the same relative path,
    # the first occurrence wins (module-specific images precede shared_images
    # in the images = [...] list, so module files take priority).
    staged_images = []
    seen_image_rels = {}
    images_root = ctx.attr.images_root
    for img in ctx.files.images:
        if images_root and img.path.startswith(images_root + "/"):
            rel = img.path[len(images_root) + 1:]
        else:
            rel = img.basename
        if rel in seen_image_rels:
            continue
        seen_image_rels[rel] = True
        out = ctx.actions.declare_file(tools_prefix + rel)
        ctx.actions.symlink(output = out, target_file = img)
        staged_images.append(out)

    all_staged = list(staged.values()) + staged_images

    # ── Declare output ────────────────────────────────────────────────────
    # rsc2 writes EITHER .srs (with -s / NOLINK_FLAG) OR .res (normal mode).
    # We run in normal mode so only .res is produced.
    res_out = ctx.actions.declare_file(ctx.label.name + ".res")

    # ── Common argument fragments ─────────────────────────────────────────
    include_args = ["-I" + d for d in ctx.attr.includes]
    define_args  = ["-D" + d for d in ctx.attr.defines]

    # For generated header files (e.g. globlmn.hrc from a genrule), the file
    # lives under bazel-out/…/bin/ rather than the source tree, so fixed
    # includes = ["main/svx/inc"] will not find it.  Derive -I paths
    # automatically:
    #   - add f.dirname so that #include "foo.hrc" (flat) resolves
    #   - add parent-of-dirname so that #include <dir/foo.hrc> resolves
    gen_dirs = {}
    for f in ctx.files.hdrs:
        if not f.is_source:
            gen_dirs[f.dirname] = True
            if "/" in f.dirname:
                gen_dirs[f.dirname.rsplit("/", 1)[0]] = True
    gen_include_args = ["-I" + d for d in gen_dirs.keys()]
    # -lip=   tells rsc2 where to search for image files by basename.
    # -subimages= provides the path-replacement prefix so GetImageFilePath
    # records bFound=true: the found path must start with the replacement value,
    # and the relative tail (e.g. "framework/res/backing.png") is stored in .res.
    #
    # When images_root = "main/default_images" (the images.zip strip_prefix),
    # images are staged at tools_dir/<zip-entry-path> so that the stored path
    # matches the zip entry name exactly (e.g. "framework/res/backing.png").
    # We add one -lip= per unique staging subdirectory so rsc2 can find each
    # image by its bare filename as written in the .src File= declaration.
    # Without per-directory -lip= entries, rsc2 only looks in the flat tools_dir
    # root and misses files staged in subdirectories, breaking Bitmap{} resources.
    if staged_images:
        lip_dirs = {}
        for _img_out in staged_images:
            lip_dirs[_img_out.dirname] = True
        image_args = ["-lip=" + _d for _d in lip_dirs] + ["-subimages=" + tools_dir]
    else:
        image_args = []

    # ── Step 1: Preprocess each .src → intermediate .srs ─────────────────
    srs_intermediates = []
    for src in ctx.files.srcs:
        srs_int = ctx.actions.declare_file(
            ctx.label.name + "_srs/" + src.basename + ".srs",
        )
        # Add the source file's own directory so that quoted #include "foo.hrc"
        # resolves relative to the .src file, matching normal cpp behaviour.
        src_dir_args = ["-I" + src.dirname]
        ctx.actions.run(
            executable            = staged["rscpp.exe"].path,
            arguments             = src_dir_args + gen_include_args + include_args + define_args + [src.path, srs_int.path],
            inputs                = [src] + ctx.files.hdrs + all_staged,
            outputs               = [srs_int],
            env                   = {"PATH": tools_dir},
            mnemonic              = "RscPreprocess",
            progress_message      = "Preprocessing RSC %s" % src.basename,
            use_default_shell_env = False,
        )
        srs_intermediates.append(srs_int)

    # ── Step 1.5: Fix single-line Bitmap declarations ─────────────────────
    # rsc2's PreprocessSrsFile() reads .srs files line-by-line and replaces
    # any line containing "File =" (exactly one '=') with the resolved image
    # path, replacing the ENTIRE line.  Single-line Bitmap blocks like
    #   Bitmap NAME{ File = "path"; };
    # have the class keyword and File= on the same line, so the whole block
    # is corrupted to a bare "File = ...;" that the parser rejects.
    # Split every such block to multi-line so "File =" is alone on its line.
    srs_for_rsc2 = []
    for srs_int in srs_intermediates:
        srs_fixed = ctx.actions.declare_file(
            ctx.label.name + "_srs_fix/" + srs_int.basename,
        )
        ctx.actions.run_shell(
            command = (
                "perl -pe 's/^(Bitmap\\s+[^\\{]+?)\\s*\\{\\s*(File\\s*=\\s*[^;]+;)\\s*\\}\\s*;\\s*$/\\1\\n\\{\\n\\2\\n\\};/g' '" +
                srs_int.path + "' > '" + srs_fixed.path + "'"
            ),
            inputs                = [srs_int],
            outputs               = [srs_fixed],
            use_default_shell_env = True,
            mnemonic              = "RscFixBitmaps",
            progress_message      = "Fixing Bitmap blocks in %s" % srs_int.basename,
        )
        srs_for_rsc2.append(srs_fixed)

    # ── Step 2: Compile all .srs intermediates → .res with rsc2 ──────────
    ctx.actions.run(
        executable = staged["rsc2.exe"].path,
        arguments  = (
            [
                "-fs=" + res_out.path,
                "-lgEN_US",
                "-BIGENDIAN",
            ]
            + include_args
            + define_args
            + image_args
            + [f.path for f in srs_for_rsc2]
        ),
        inputs                = srs_for_rsc2 + all_staged,
        outputs               = [res_out],
        env                   = {"PATH": tools_dir},
        mnemonic              = "Rsc2Compile",
        progress_message      = "Compiling RSC resources for %s" % ctx.label.name,
        use_default_shell_env = False,
    )

    return [DefaultInfo(files = depset([res_out]))]

rsc_res = rule(
    implementation = _rsc_res_impl,
    attrs = {
        "srcs": attr.label_list(
            allow_files = [".src"],
            doc         = ".src resource source files",
        ),
        "hdrs": attr.label_list(
            allow_files = True,
            doc         = ".hrc header files included by .src files (inputs to rscpp)",
        ),
        "includes": attr.string_list(
            doc = "execroot-relative include paths (passed as -I to rscpp)",
        ),
        "images": attr.label_list(
            allow_files = True,
            default     = [],
            doc         = "PNG/BMP image files embedded by Bitmap{} blocks",
        ),
        "images_root": attr.string(
            default = "",
            doc     = "exec-root-relative prefix to strip from image paths when staging (e.g. 'main/default_images'). When empty, images are staged flat by basename.",
        ),
        "defines": attr.string_list(
            default = _DEFAULT_DEFINES,
            doc     = "Preprocessor defines (passed as -D to rscpp and rsc2)",
        ),
        # ── Tool executables ─────────────────────────────────────────────
        "_rscpp": attr.label(
            default     = "//main/rsc:rscpp",
            executable  = True,
            cfg         = "exec",
            allow_files = True,
        ),
        "_rsc2": attr.label(
            default     = "//main/rsc:rsc2",
            executable  = True,
            cfg         = "exec",
            allow_files = True,
        ),
        # ── Runtime DLLs (individual single-file attrs) ──────────────────
        "_sal_dll": attr.label(
            default           = "//main/sal:sal",
            allow_single_file = True,
            cfg               = "exec",
        ),
        "_tl_dll": attr.label(
            default           = "//main/tools:tl",
            allow_single_file = True,
            cfg               = "exec",
        ),
        "_cppu_dll": attr.label(
            default           = "//main/cppu:cppu3",
            allow_single_file = True,
            cfg               = "exec",
        ),
        "_cppuhelper_dll": attr.label(
            default           = "//main/cppuhelper:cppuhelper3MSC",
            allow_single_file = True,
            cfg               = "exec",
        ),
        "_salhelper_dll": attr.label(
            default           = "//main/salhelper:salhelper3MSC",
            allow_single_file = True,
            cfg               = "exec",
        ),
        "_comphelp_dll": attr.label(
            default           = "//main/comphelper:comphelpMSC",
            allow_single_file = True,
            cfg               = "exec",
        ),
        "_ucbhelper_dll": attr.label(
            default           = "//main/ucbhelper:ucbhelperMSC",
            allow_single_file = True,
            cfg               = "exec",
        ),
        "_basegfx_dll": attr.label(
            default           = "//main/basegfx:basegfx",
            allow_single_file = True,
            cfg               = "exec",
        ),
        "_vos_dll": attr.label(
            default           = "//main/vos:vos3MSC",
            allow_single_file = True,
            cfg               = "exec",
        ),
        "_i18nisolang_dll": attr.label(
            default           = "//main/i18npool:i18nisolang1MSC",
            allow_single_file = True,
            cfg               = "exec",
        ),
        # ── CRT and manifest ─────────────────────────────────────────────
        "_crt_dlls": attr.label(
            default     = "//main/external/msvcp90:msvcp90",
            allow_files = True,
            cfg         = "exec",
        ),
        "_app_manifest": attr.label(
            default           = "//main/external/msvcp90:vc90_app_manifest",
            allow_single_file = True,
            cfg               = "exec",
        ),
    },
    doc = "Compile .src resource files to a .res binary via rscpp (preprocess) + rsc2 (compile).",
)
