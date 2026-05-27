# ooo_custom_images — Bazel Migration

Pure-data package. Contains image sets that override or supplement the default icons
from `default_images/`. Each subdirectory is a named theme:

| Directory | Theme | Notes |
|---|---|---|
| `industrial/` | Industrial/Tango icons | 2435 PNGs; fully staged |
| `nologo/` | No-logo intro screen | Single intro.png |
| `dev_nologo/` | Dev/no-logo intro screen | Single intro.png |
| `classic/` | Classic icon set | Pre-packaged as `classic_images.tar.gz` |

## Targets

| Target | Output | Staging destination |
|---|---|---|
| `images_industrial` | `images_industrial.zip` | `share/config/images_industrial.zip` |
| `nologo_intro` | filegroup | (not yet staged) |
| `dev_nologo_intro` | filegroup | (not yet staged) |
| `classic_images_tar` | filegroup | deferred — needs tar→zip conversion |

## Deferred

`images_classic.zip`: requires extracting `classic/classic_images.tar.gz` and
repacking as a zip. Deferred until the packimages pipeline is complete.

`images_hicontrast.zip`: high-contrast variants are mixed into `default_images/`
using the `_hc`/`h`-suffix naming convention; separating them requires the `.ilst`
files produced by the RSC pipeline.
