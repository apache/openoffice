# default_images — Bazel Migration

Pure-data package. Contains the raw PNG/BMP source images for all OOo toolbar icons
and UI decorations. No compilation step is needed — files are either embedded into
`.res` files by the RSC pipeline or packed into `images.zip` for runtime loading.

## Targets

| Target | Output | Use |
|---|---|---|
| `vcl_images` | filegroup | VCL `.res` embedded images (vcl/res/**) |
| `shared_images` | filegroup | Other shared images (res/*.png, res/*.bmp) |
| `desktop_res_images` | filegroup | Desktop + commandimagelist PNGs |
| `<module>_res_images` | filegroup | Per-module images fed to RSC pipeline |
| `images` | `images.zip` | Runtime toolbar icons staged to `share/config/` |

## images.zip

Since 2026-10-08 all four `images*.zip` are built by `//main/packimages` the way
upstream's `packimages.pl` does it: the entry set is the `.ilst` image lists the
resource compiler writes, not a glob of this directory.  See
`main/packimages/readme.md`.  This package only exposes the images as
filegroups (`all_png` for packimages, the per-module ones for `rsc_res`).
