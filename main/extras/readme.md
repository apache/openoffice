# extras — Bazel Migration

Pure-data package; no compilation step.  All content comes from `source/` subdirectories
and is copied directly into the staging layout by `//main/staging`.

## Staging destinations

| Source (`main/extras/source/`) | Staging destination |
|---|---|
| `autotext/lang/<lang>/` | `share/autotext/<lang>/` and `presets/autotext/<lang>/` |
| `wordbook/` | `share/wordbook/` |
| `gallery/` | `share/gallery/` |
| `templates/` | `share/template/` |
| `palettes/` | `presets/config/` |
| `misc_config/` | `share/config/` (wizard CSS/images, `autotbl.fmt`) |
| `database/` | `share/database/` |
| `truetype/symbol/opens___.ttf` | `share/fonts/truetype/` (OpenSymbol font) |

## presets/ — initial user profile template

`UserInstall::finalize()` in `desktop/source/app/userinstall.cxx` copies
`$OOO_BASE_DIR/presets/` → `$UserInstallation/user/` on first launch.
Staging autotext and palette files into `presets/` ensures this directory
exists and the user gets a useful initial profile.

`bootstrap.ini` uses `UserInstallation=$SYSUSERCONFIG/OpenOffice/4bazel` so
the Bazel-built OOo never reads or writes the production OOo profile at
`%APPDATA%\OpenOffice\4`.
