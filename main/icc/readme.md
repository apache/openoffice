# icc — Bazel migration notes

## Targets

| Target | Output | Description |
|--------|--------|-------------|
| `create_sRGB_profile` | `create_sRGB_profile.exe` | Generates the sRGB ICC profile and C++ header |
| `gen_srgb` | `sRGB-IEC61966-2.1.hxx`, `sRGB-IEC61966-2.1.icc` | genrule that runs the tool at build time |
| `srgb_icc` | (header-only) | Exposes `<icc/sRGB-IEC61966-2.1.hxx>` for VCL's PDF/A-1a export |

## External dependency: @sampleicc

SampleICC 1.3.2 is fetched via the local BCR at `ext_libraries/modules/sampleicc/`.
It provides two cc_library targets:
- `@sampleicc//:iccproflib` — `IccProfLib/` sources (ICC profile read/write)
- `@sampleicc//:icc_utils` — `Contrib/ICC_utils/` sources (validation helpers)

### Patching strategy — all-overlay, no patches section

Bazel 9.0.1 has a bug in its Java `PatchUtil`: it always fails with
`CONTENT_DOES_NOT_MATCH_TARGET` on the **last file section** in a `remote_patches` patch
file (reports "near line 6666", a sentinel value). GNU patch applies all hunks correctly.

Workaround: every modified file is placed as a pre-patched copy in
`ext_libraries/modules/sampleicc/1.3.2/overlay/` — the `patches` section in
`source.json` is omitted entirely.

Files in the overlay fall into two categories:

| File | Reason |
|------|--------|
| `Contrib/ICC_utils/Stubs.h` | CRLF in archive — Bazel Java splits on `\n`, leaving `\r` on each line, so LF patches can't match |
| `IccProfLib/IccTagProfSeqId.cpp` | Same CRLF issue |
| `IccProfLib/IccTagProfSeqId.h` | Same CRLF issue |
| `Contrib/ICC_utils/Vetters.cpp` | OOo patch context mismatch (blank-line differences vs. published archive) |
| `IccProfLib/icProfileHeader.h` | Last-file Bazel bug |
| `IccProfLib/IccUtil.cpp` | Last-file Bazel bug |
| All other 13 patched files | Last-file Bazel bug (only workaround is to move everything to overlay) |

### How overlay files were generated

```bash
# 1. Extract from tar.gz
tar -xzf ext_sources/SampleICC-1.3.2.tar.gz SampleICC-1.3.2/<path>
# 2. Apply OOo patch (with -l for lenient whitespace matching)
patch -p2 -l -o patched.cpp original.cpp ooo_patch.patch
# 3. Compute base64-sha256 for source.json
sha256sum patched.cpp | awk '{print $1}' | xxd -r -p | base64 -w0
# 4. Copy to overlay at the matching relative path
```

## genrule: running create_sRGB_profile

The binary writes `sRGB-IEC61966-2.1.hxx` and `.icc` to its working directory (hardcoded,
no output-dir argument). The genrule must `cd` to `$(RULEDIR)` first so the files land in
the correct output location.

After `cd`, the `$(location :create_sRGB_profile)` path (relative to exec root) no longer
resolves. Fix: capture the exec root with `%cd%` *before* the `cd`, then use delayed
expansion (`!_WS!`) to reference the exe from the new directory:

```python
cmd_bat = "set \"_WS=%cd%\"&& cd /d \"$(RULEDIR)\"&& \"!_WS!\\$(location :create_sRGB_profile)\"",
```
