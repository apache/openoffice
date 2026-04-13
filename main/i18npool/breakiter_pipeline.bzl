# breakiter_pipeline.bzl
# Starlark rules for i18npool's breakiterator data generation pipeline.
#
# Pipeline:
#   .txt rules  →  genbrk  →  .brk  →  genccode  →  _brk.c
#                                ↓
#                   gencmn (all .brk) → OpenOffice_dat.c
#
# All of these generated .c files are compiled into i18npool.dll.
# Using ctx.actions.run() (not genrule) to avoid Windows cmd.exe shell issues.

"""Starlark rule for ICU breakiterator data generation pipeline in i18npool."""

def _breakiter_data_impl(ctx):
    genbrk = ctx.executable.genbrk
    gencmn = ctx.executable.gencmn
    genccode = ctx.executable.genccode
    dat_file = ctx.file.icudt_dat

    brk_files = []
    c_files = []

    # ── Step 1: compile each .txt break rule → .brk binary ──────────
    for txt in ctx.files.srcs:
        name = txt.basename[:-4]  # strip ".txt"
        brk = ctx.actions.declare_file(name + ".brk")
        brk_files.append(brk)

        ctx.actions.run(
            executable = genbrk,
            arguments = [
                "-i", dat_file.dirname,  # ICU data directory (icudt49l.dat)
                "-r", txt.path,           # input rules file
                "-o", brk.path,           # output .brk file
            ],
            inputs = [txt, dat_file],
            tools = [genbrk],
            outputs = [brk],
            mnemonic = "GenBrk",
            progress_message = "Compiling break rules: %s" % txt.basename,
        )

    # ── Step 2: genccode each .brk → _brk.c C array ─────────────────
    # genccode -n OpenOffice -d <destdir> <brkfile>
    # Produces: <destdir>/<name>_brk.c
    # Note: -n sets the C symbol prefix (OpenOffice_<name>_brk_dat), NOT the filename.
    # The filename is always <inputbase_with_dots_to_underscores>.c, e.g. count_word_brk.c.
    for brk in brk_files:
        name = brk.basename[:-4]  # strip ".brk"
        c = ctx.actions.declare_file(name + "_brk.c")
        c_files.append(c)

        ctx.actions.run(
            executable = genccode,
            arguments = [
                "-n", "OpenOffice",
                "-d", c.dirname,
                brk.path,
            ],
            inputs = [brk],
            tools = [genccode],
            outputs = [c],
            mnemonic = "GenCCode",
            progress_message = "Encoding break data: %s" % brk.basename,
        )

    # ── Step 3: write gencmn response file with .brk paths ──────────
    # gencmn opens each file by the path written in the response file.
    # Using exec-root-relative paths (file.path) works because Bazel
    # sets CWD to the execroot when running actions.
    list_file = ctx.actions.declare_file("brk_list.txt")
    ctx.actions.write(
        output = list_file,
        content = "\n".join([b.path for b in brk_files]) + "\n",
    )

    # ── Step 4: gencmn → OpenOffice_dat.c package index ─────────────
    # gencmn -n OpenOffice -S -d <outdir> O listfile
    # Produces: <outdir>/OpenOffice_dat.c
    dat_c = ctx.actions.declare_file("OpenOffice_dat.c")
    ctx.actions.run(
        executable = gencmn,
        arguments = [
            "-n", "OpenOffice",
            "-S",              # source (.c output, not binary)
            "-d", dat_c.dirname,
            "O",               # open source: list-file format
            list_file.path,
        ],
        inputs = depset(brk_files + [list_file]),
        tools = [gencmn],
        outputs = [dat_c],
        mnemonic = "GenCmn",
        progress_message = "Packaging ICU break data",
    )

    return [DefaultInfo(files = depset(c_files + [dat_c]))]

breakiter_data = rule(
    implementation = _breakiter_data_impl,
    attrs = {
        "srcs": attr.label_list(
            allow_files = [".txt"],
            doc = "Break rule .txt source files",
        ),
        "genbrk": attr.label(
            cfg = "exec",
            executable = True,
            doc = "genbrk ICU tool",
        ),
        "gencmn": attr.label(
            cfg = "exec",
            executable = True,
            doc = "gencmn ICU tool",
        ),
        "genccode": attr.label(
            cfg = "exec",
            executable = True,
            doc = "genccode ICU tool",
        ),
        "icudt_dat": attr.label(
            allow_single_file = True,
            doc = "Precompiled ICU data file (icudt49l.dat)",
        ),
    },
    doc = "Compile ICU break rule .txt files into C arrays for i18npool.",
)
