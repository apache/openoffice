<!--
 Licensed to the Apache Software Foundation (ASF) under one
 or more contributor license agreements.  See the NOTICE file
 distributed with this work for additional information
 regarding copyright ownership.  The ASF licenses this file
 to you under the Apache License, Version 2.0 (the
 "License"); you may not use this file except in compliance
 with the License.  You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing,
 software distributed under the License is distributed on an
 "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 KIND, either express or implied.  See the License for the
 specific language governing permissions and limitations
 under the License.
-->

# Notes for packimages (done)

Migrated 2026-10-08.  `share/config/images.zip` plus the three alternative icon
themes, `images_{hicontrast,industrial,classic}.zip`, as scp2 `file_ooo.scp`
installs them.  Identical on all three configs.

## How it is built

`pack/makefile.mk`'s pipeline, with upstream's own scripts run unchanged
(`solenv/bin/packimages.pl`, `image-sort.pl`, `hicontrast-to-theme.pl`);
`packimages_driver.pl` only lays out their inputs.

- **The entry set is the image lists, not a glob.**  Every `rsc_res` declares the
  `.ilst` rsc2 writes beside its `.res` (output group `ilst`): one line per image
  that bundle references.  Add `commandimagelist.ilst` (every
  `res/commandimagelist/**.png`, generated as the makefile's `find | sed` does),
  helpcontent2's `helpimg.ilst` (the help pages' pictures) and connectivity's
  `hsqldb_res` (built only for its list; never installed).  The bundle list is
  `//main/staging:res_bundles.bzl`, shared with staging so the two cannot drift.
- **Every theme zip is a complete set.**  vcl's `ImplImageTree::setStyle()` adds
  `images.zip` only for the `"default"` style; any other style reads its own zip
  and nothing else.  So each entry is taken from the theme directory if it has
  it, else from `default_images` (packimages.pl `-c` before `-g/-m`).
  - industrial: `ooo_custom_images/industrial`
  - hicontrast: `hicontrast-to-theme.pl` output -- `*_h.png`, `lch_*`, `sch_*`
    ... renamed onto their normal names.  Windows' high-contrast mode selects
    this theme automatically.
  - classic: `ooo_custom_images/classic/classic_images.tar.gz`, unpacked.
- `sorted.lst` (zip entry ORDER, a load-time optimisation): `image-sort.pl`
  reads the toolbar XML under `uiconfig/modules/...`, fed from
  `//main/postprocess:uiconfig_tree`.
- `packimages.pl` demands at least one `-c`; upstream always passes
  `-c $(RSCCUSTOMIMG)`, an image-free directory, so the driver passes an empty
  one.

## What it fixed

Before, `images.zip` was a glob of `default_images` and
`images_industrial.zip` held only the industrial overrides (2435 entries).
`images_hicontrast.zip` and `images_classic.zip` did not exist.  So:

- choosing **Industrial** (Tools > Options > View) left every icon it does not
  override blank;
- **Windows high-contrast mode** found no theme zip at all -- no icons;
- **Classic** was not offered by anything that could load.

Building from the `.ilst` lists also exposed that `rsc_res` had been dropping
images.  Upstream compiles resources in two phases: `rsc -fp=x.srs` (rsc2 `-s`,
which parses the source and writes it back one attribute per line), then the
link.  rsc2's image resolver (`rsc.cxx PreprocessSrsFile`) only rewrites a line
with a SINGLE `=`, so it relies on that normalisation; our rule fed it raw
preprocessor output, and every one-line `ImageBitmap = Bitmap { File = "x.png"; };`
(1033 references in 19 modules) kept a bare basename that no zip entry matches.
`rsc_res` now runs the `-s` phase (`build/rules/rsc_pipeline.bzl`, step 1.7).
That is the root cause of the blank wizard thumbnails recorded in
`main/wizards/readme.md` -- it was ours, not upstream's.  Three bundles then
needed upstream's image search path (`adabasui_res`, `abp_res`, `dbp_res`), and
`normalize_idlist.pl` had to stop turning an rscpp line marker inside an
`IdList` into `# 123;`.

## Verified (2026-10-08)

- Against an AOO 4.1.17 install (older source, so structure, not bytes): the
  oracle's four zips share one entry set of 7923; ours share one of 7976.  Of
  the oracle's entries, every one whose image still exists in our tree is in
  ours except 5: `svx/res/notcertificate_16{,_h}.png` (moved to xmlsecurity/res
  since, and shipped there) and three SANE scanner-dialog images (not used on
  Windows).  Images, hicontrast and industrial are 92-93% byte-identical to
  the oracle's; classic 57%, because trunk commit e1299e96dc replaced the
  classic tarball -- every classic entry of ours equals its file in the current
  tarball (3724/3724).
- Against the old glob zip: 554 entries dropped, none referenced anywhere in
  the source by path or basename, none in the oracle.  All 120
  `private:graphicrepository/` / `private:imagerepository/` references resolve
  as before; the 3 that miss in both are upstream's own (basctl asks for
  `sd/res/triangle_*.png`, which live in svtools/res; `vcl/res/spinner-` is a
  prefix the code completes).
- All 63 `.res` keep exactly the same top-level resources in the same order;
  the only content changes are image names becoming path-qualified (1435
  strings), plus rsc's uninitialised padding bytes.
- Headless x86 office, fresh profile, pyuno GraphicProvider: loads
  `res/commandimagelist/lc_open.png`, `svtools/res/prnfont.png`,
  `database/linked_text_table.png`, `res/helpimg/area1.png` from images.zip.

Not yet looked at by eye: the themes in the GUI (Tools > Options > View > Icon
size and style), the wizard thumbnails, and the dialogs whose one-line images
now resolve.

## Not built

`images_brand.zip`: upstream `$(TOUCH)`es an empty file, which is not a valid
zip -- vcl drops a zip it cannot open, so its absence is the same thing.
