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

# Notes for instsetoo_native (in progress)

The Windows installer (MSI).  Upstream runs `solenv/bin/make_installer.pl`
(`util/makefile.mk`, `MAKE_INSTALLER_COMMAND`) over scp2's `setup_osl.inf`,
looking every file up BY NAME on the `include` path of `util/openoffice.lst` --
solver directories, `$SOLARVERSION/<OUTPATH>/{bin,lib,pck,xml}` (no
`UPDMINOR` ⇒ no `.{minor}` suffix).  So the Bazel job is: lay out a
solver-shaped tree under the names scp2 asks for, then run the script.

Decision (2026-10-08, owner): run upstream's installer modules under the
build's **Strawberry Perl**, ported -- not Cygwin Perl, not a rewrite.

## Done

1. **Native-Perl port** (`main/solenv/bin`, commit "installer: run
   make_installer.pl under a native Windows Perl"): `MSWin32` platform branch in
   `globals.pm`; no `zip.exe` requirement on MSWin32 (msi builds never run it);
   no `chmod >/dev/null` on MSWin32; File items with no `Name` in any language are
   dropped (an en-US-only build turns `EXTRA_ALL_LANG_BUT_EN_US` into exactly
   that).  It now runs through all setup-script analysis to file collection.
2. `//main/setup_native:ulfconv` -- `.ulf` → `.mlf` for `-msilanguage`.

## How it was run (the spike, to be turned into a rule)

- Perl: `@strawberry-perl`, with its `c/bin` on `PATH` (XML::Parser needs
  `libexpat-1_.dll` from there), `PERL5LIB=main/solenv/bin/modules`.
- `PATH` must put `C:\Windows\system32` first (Git's Unix `expand.exe` would
  otherwise win) and contain the pinned SDK v7.0 `Bin` (msidb, msiinfo,
  msitran, uuidgen); `makecab.exe` is Windows'.
- Env: `SOLARVERSION=<solver root> OUTPATH=wntmsci12 INPATH=wntmsci12.pro
  GUI=WNT WORK_STAMP=AOO420 LOCAL_OUT LOCAL_COMMON_OUT SOLARENV TEMP TMP`.
- `make_installer.pl -f openoffice.lst -l en-US -u <out> -buildid N -format msi
  -p Apache_OpenOffice -s <setup_osl.inf> -msitemplate <dir> -msilanguage <dir>`,
  run from `main/instsetoo_native/util`.
  - `-msitemplate`: a copy of `inc_openoffice/windows/msi_templates`.
  - `-msilanguage`: `ulfconv -o X.mlf -t msi-encodinglist.txt X.ulf` for each
    `inc_openoffice/windows/msi_languages/*.ulf`, plus `Nsis.ulf` copied to
    `Nsis.uulf`.
- Solver `bin` also needs: `msi-encodinglist.txt`, setup_native's
  `source/win32/{patchlist,desktophelper}.txt`, `get_retval.bat`, `nsis/*`,
  `source/packinfo/*.{txt,pcp}`, `scripts/*.txt` (its `prj/d.lst`);
  `cliureversion.mk` = `cli_ure/version/version.txt`,
  `clioootypesversion.mk` = `unoil/climaker/version.txt`;
  `setup_native/scripts/userland.txt`; `solenv/inc/version.lst`.
- `remove_Files_Without_Sourcedirectory` makes ANY missing file fatal, so the
  script itself is the completeness oracle.

## Left: 379 files make_installer cannot find (2026-10-08, x86, en-US)

With a solver `bin` holding every staged file by basename:

| n | kind | what it takes |
|---|---|---|
| 89 | DLL/EXE | scp2 defines that match this build (CLI, ado/adabas, ICU 42→49, statically linked externals such as libxml2/curl/OpenSSL/NSS/glib/pango); `MSC_uno.dll` = `msci_uno`/`mscx_uno`; `jvmfwk3.dll`; setup_native's custom-action DLLs (`qslnkmsi`, `reg4allmsdoc`, `shlxtmsi`, `sdqsmsi`, `reg64msi`, `regactivex`, `instooofiltmsi`, `lngpckinsthlp`, `patchmsi`, `sn_tools`, `relnotes`, …) |
| 73 | `*.lm` | textcat language models -- not staged |
| 72 | XML/text | the `xslt/...` trees are named WITH their path; DTDs; LICENSE/NOTICE |
| 67 | zip | 45 `ARCHIVE` zips (basic, gallery, autocorr, templates, …) to build from what staging already lays out, plus packed data (help, autotext, …) |
| 40 | `.uno.dll` | the known naming divergence -- see `.agent/common/20-build-conventions.md` |
| 33 | other | CLI `.config`, Math DTD `.mod` files, `PY_FULL_DLL_NAME` (unresolved macro) |
| 5 | `.res` (3), `.xcd`, jar | name differences (`foruien-US.res`, `pdffilteren-US.res`, `t602filteren-US.res`, `fcfg_langpack_en-US.xcd`, `java_uno_accessbridge.jar`) |

Order agreed: (2) make scp2's defines describe this build, (3) the solver tree
(name map + ARCHIVE zips) as a Bazel rule, (4) setup_native's custom actions,
(5) the MSI, verified by `msiexec /a` against the staged install.

The parked Win64 MSI work on the dmake branch `win10-msvc-trunk` (memory:
project_win64_installer_parked) is a separate line; its findings (D1
same-version upgrade, missing `Error.idt` rows 9010-9012) apply once this
produces an x64 MSI.
