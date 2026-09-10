# Apache OpenOffice — Apple Silicon build harness

Working reference for getting AOO `trunk` building on this Mac. Branch: **`mac-silicon-minimal`**.
Keep the **Status / Next** section at the bottom current as work proceeds.

---

## Goal

Smallest sensible diff to `trunk` that builds a **real production config** on Apple Silicon
(Java, bundled dictionaries, wiki-publisher, category-b, `.dmg`). "Minimal" = small patch,
not a stripped feature set.

**Current milestone:** `cd main && autoconf && ./configure <flags> && ./bootstrap` all
succeed. No `build --all` yet.

## Machine

| | |
| --- | --- |
| Host | Mac mini M2 (`T8112`), macOS 26.3.1 (Darwin 25.3.0), arm64 |
| Xcode | 16.4 (`/Applications/Xcode.app`), Apple clang 17.0.0 |
| SDK | only `MacOSX26.sdk` present (`xcrun --show-sdk-path`) |
| Disk | data volume ~98% full — user is freeing space before `build --all` |

## What `trunk` already has (don't re-do)

`trunk` carries a landed arm64 port: commit `5e139d9fe4` + ~20 follow-ups (2026-06/07).

- `main/configure.ac:1247` — Apple-Silicon detect. `config.guess` → `build_cpu=aarch64`.
- `--with-macosx-target` **defaults to 11.0**; `< 11` is a hard `AC_MSG_ERROR`.
- `main/configure.ac:~4500` — SDK auto-detect (`xcodebuild -showsdks`, newest; regex allows
  macOS 11–99; falls back to `xcrun`). Exports `SDKROOT`.
- `main/configure.ac:1234` — if `CXX` unset, configure **generates** `main/macos_c11`
  (`clang++ -std=c++11` wrapper) and uses it as `CXX`. Stale copy on disk regenerates.
- `main/configure.ac:999` — hard gate: errors if `configure` older than `configure.ac`.
  **Always run `autoconf` first.**
- `main/set_soenv.in:826` — `aarch64|arm64` Darwin branch → `OUTPATH=unxmaccr`, `CPU=R`,
  `CPUNAME=AARCH64`, env file `MacOSXAARCH64Env.Set`. Default `.pro` → `INPATH=unxmaccr.pro`.
- New `solenv/inc/unxmaccr.mk` (`-arch arm64`, `-DAARCH64`, `BUILD64=1`),
  `solenv/bin/modules/osarch.pm` arm64 rows, `solenv/gbuild/platform/macosx.mk` `AARCH64`.
- Python: on macOS, configure **defaults to requiring system Python ≥ 3.11**. Bundled
  Python 3.10.x is used only with `--without-system-python` (our choice).

## Decisions

| Topic | Choice | Notes |
| --- | --- | --- |
| JDK | **Temurin 11 (arm64)** — `brew install --cask temurin@11` | configure needs ≥ 1.5, no upper bound |
| Python | **`--without-system-python`** (bundled 3.10.x) | no Homebrew Python |
| Perl | **`export PERL=/usr/bin/perl`** | Apple system perl has all 6 required modules; Homebrew perl 5.42 is missing `Archive::Zip`, `LWP::UserAgent`, `LWP::Protocol::https`, `XML::Parser` |
| SDK | **macOS 26 SDK + deployment target 11.0** (trunk default) | no `--with-macosx-sdk`, no `../xcodelegacy` |
| dmake | prebuilt **`../dmake/dmake` v4.13.0 arm64** (≥ 4.11 ✓) | `--with-dmake-path=...` |
| epm | **`../epm` = jimjag EPM 5.0.0** (has `--aoo-mode`) — just needs `./configure && make` | satisfies configure's ≥ 5.0 check |
| Packaging | `--with-package-format=dmg` | epm above covers it |
| Languages | `--with-lang="en-US de fr es it nl pt ru"` | Western set |
| Add-ons | `--enable-wiki-publisher --enable-bundled-dictionaries --enable-category-b` | |

## Toolchain status

OK: autoconf 2.72 (trunk requires exactly 2.72 on macOS), automake, `/usr/bin/perl`
(+ all required modules), pkg-config, ant 1.10 at `/opt/homebrew/opt/ant/libexec`
(≥ 1.9.1 ✓), gperf 3.0.3 (major ≥ 3 ✓), Xcode `make` 3.81 (meets floor), `../dmake/dmake`
4.13.0 arm64.
Built/installed during this milestone: `../epm/epm` (jimjag EPM 5.0.0, arm64);
`gpatch` 2.8 (`brew install gpatch`).
Still TODO (for `build --all`, not configure): Temurin 11 (`brew install --cask temurin@11`,
needs admin password).

## Steps

### 0. Harness doc — this file. Keep Status/Next updated.

### 1. Branch + clean stale state  ✅ branch created
- `git checkout -b mac-silicon-minimal` (done).
- Remove 2024-attempt leftovers (all gitignored): `main/configure`, `main/autom4te.cache/`,
  `main/config.log`, `main/config_2.log`, `main/config.old.log`, `main/config.parms`,
  `main/a.out`, `main/warn`, `main/conftest1.c`, `main/macos_c11`, `main/configure.bac`.
- Root `configure.sh` → copy to scratchpad, remove from tree.
- Leave staged `AOO/AOO.xcodeproj/` alone (unrelated).

### 2. Prerequisites
- `brew install --cask temurin@11` → `/Library/Java/JavaVirtualMachines/temurin-11.jdk/Contents/Home`.
- `cd ../epm && ./configure && make` → `../epm/epm` binary.

### 3. `main/mac-silicon-configure.sh` (tracked) — replaces stale root `configure.sh`
No `CFLAGS/CXXFLAGS/SDKROOT/-std=c++11` exports; let configure.ac detect SDK + C++ std.
```sh
#!/bin/sh
set -e
SRCDIR=$(cd "$(dirname "$0")" && pwd)
export JAVA_HOME=/Library/Java/JavaVirtualMachines/temurin-11.jdk/Contents/Home
export PERL=/usr/bin/perl
./configure \
  --with-jdk-home="$JAVA_HOME" \
  --with-ant-home=/opt/homebrew/opt/ant/libexec \
  --with-dmake-path="$SRCDIR/../dmake/dmake" \
  --with-epm="$SRCDIR/../epm/epm" \
  --with-package-format=dmg \
  --with-macosx-target=11.0 \
  --without-system-python \
  --without-junit \
  --without-stlport \
  --with-alloc=internal \
  --enable-category-b \
  --enable-bundled-dictionaries \
  --enable-wiki-publisher \
  --disable-systray \
  --with-lang="en-US de fr es it nl pt ru"
```
(`--with-openldap`: add only if configure asks; Homebrew `openldap` is installed.)

### 4. Iterate: `cd main && autoconf` then run the script
Fix failures one at a time, commit each minimal change. Watch for:
- macOS "26" SDK version-string parsing in `configure.ac` / `set_soenv.in`
  (`MACOSX_SDK_PATH`, deployment-target math at `configure.ac:1247`).
- `--without-system-python` — confirm no stray Darwin system-python probe still fires;
  `SYSTEM_PYTHON=NO`, `BUILD_TYPE += PYTHON`.
- JDK 11 layout (no `jre/`, no `tools.jar`); `javac` + `javadoc` must resolve under
  `--with-jdk-home`. JNI includes via `$JAVA_HOME/include{,/darwin}` (`set_soenv.in`).
- Bundled sub-configures (nss/nspr, hunspell) under autoconf 2.72.

`./configure` → writes `config.status`, `set_soenv`, `Makefile`; runs `set_soenv` →
emits `MacOSXAARCH64Env.Set{,.sh}`, `source_soenv.sh`, `clean`, `bootstrap`.

### 5. `cd main && ./bootstrap`
Sources env, `mkdir -p solenv/unxmaccr.pro/bin`, `chmod +x` solenv perl scripts,
`cp -f "$DMAKE_PATH" solenv/unxmaccr.pro/bin/dmake` (our `--with-dmake-path` branch).
With `DO_FETCH_TARBALLS=yes`: `download_external_dependencies.pl external_deps.lst`
(~1–2 GB → `ext_sources/`), then `download_missing_extensions.pl` (wiki-publisher jars).
Does NOT run autoconf/configure/set_soenv; does NOT build epm (that's a later module build).

### 6. Commit + document
New configure script, any `configure.ac`/`set_soenv.in`/`solenv` fixes, this file updated
with final steps + exact versions. Stop; reassess before `build --all`.

## Verification (milestone done when all pass)

- `cd main && ./mac-silicon-configure.sh` completes; `main/config.status` exists.
- `ls main/*Env.Set.sh` → `MacOSXAARCH64Env.Set.sh`.
- `. main/MacOSXAARCH64Env.Set.sh && echo "$INPATH $CPUNAME"` → `unxmaccr.pro AARCH64`.
- `cd main && ./bootstrap` exits 0.
- `file main/solenv/unxmaccr.pro/bin/dmake` → `Mach-O 64-bit executable arm64`.

## Known later-milestone issues (not this milestone)

- `build --all` needs ~20 GB + hours.
- NSS 3.39 in-tree is too old for arm64 (deferred in the original arm64 commit).
- Code-signing infra not included on the arm64 branch.
- JDK 11 vs a codebase historically targeting 7–9 — build-time Java risk.

---

## Milestone result — configure + bootstrap CLEAN ✅ (2026-09-03)

`cd main && ./mac-silicon-configure.sh` (runs `autoconf` + `./configure`) → exit 0.
`cd main && ./mac-silicon-bootstrap.sh` (runs `./bootstrap`) → exit 0.

Verified:
- `main/MacOSXAARCH64Env.Set.sh` sources → `INPATH=unxmaccr.pro CPUNAME=AARCH64 OS=MACOSX`.
- `main/solenv/unxmaccr.pro/bin/dmake` → Mach-O arm64, Version 4.13.0.
- `main/ext_sources/` 351 MB / 54 files, all MD5-verified; 11 dict extensions fetched.
- configure `warn` file empty.

### What it took (beyond trunk's existing arm64 port)

| Blocker | Fix |
| --- | --- |
| `configure` older than `configure.ac` (hard gate) + 2024 leftovers | `git clean -fdx main/`; wrapper runs `autoconf` first |
| epm >= 5.0 required | built `../epm` (jimjag EPM 5.0.0): `cd ../epm && ./configure && make` |
| Homebrew perl (first on PATH) missing `Archive::Zip` / `LWP` / `XML::Parser` | `export PERL=/usr/bin/perl` (Apple perl has all modules) |
| ODK: `external/unowinreg/unowinreg.dll` + no mingw | dropped in prebuilt DLL (6144 B, PE32; sha256 `f563e5…653c`) from `openoffice.org/tools/unowinreg_prebuild/680/`. **gitignored** (`main/.gitignore:59`) — must be re-fetched on a fresh clone. |
| system curl = SecureTransport/LibreSSL, not OpenSSL | `--without-system-curl` (build bundled curl) |
| `/usr/bin/patch` is BSD, not GNU | `brew install gpatch`; `--with-gnu-patch=/opt/homebrew/opt/gpatch/bin/gpatch` |
| `bootstrap` downloads: `500 Can't verify SSL peers` (Apple perl has no `Mozilla::CA`) | `mac-silicon-bootstrap.sh` exports `SSL_CERT_FILE` / `PERL_LWP_SSL_CA_FILE` / `CURL_CA_BUNDLE` = `/etc/ssl/cert.pem` |
| `main/macos_c11` (configure-generated CXX wrapper) not ignored | added `/macos_c11` to `main/.gitignore` |

Non-blocking notes:
- **JDK**: used already-installed **OpenJDK 25 (arm64)** — passes configure. **Temurin 11
  still not installed** (Homebrew cask needs an admin password). Install it and re-run
  `JAVA_HOME=.../temurin-11.../Contents/Home ./mac-silicon-configure.sh` before `build --all`.
- SDK auto-detected = **`MacOSX15.5.sdk`** (what Xcode 16.4 ships; not "26"). Deployment
  target 11.0. The "macOS 26 SDK" concern was moot.
- `--without-system-python` → bundled Python (3.10.x, `BUILD_TYPE += PYTHON`), fetched OK.
- Production shippability: `libxml`/`libxslt` resolved external (`/usr/bin/xml2-config`,
  `/opt/homebrew/opt/libxslt`). Fine to build, but a shipping DMG may need
  `--with-static-system-libs` or internal builds so it runs without Homebrew. Revisit
  before packaging.
- Disk: user freed space — 37 GiB free at bootstrap time.

## Milestone 2 — `build.pl --all` (in progress, 2026-09-03)

- Installed **Temurin 11.0.32.1 (arm64)**, re-ran configure (JDK 11.0.32.1, `JDK=sun`),
  re-ran bootstrap, started `build.pl --all -P4 --stoponerror`.
- **~4 min in: `apache-commons/commons-logging` failed** — Temurin 11's `javac` rejects
  the bundled `build.xml` `source="1.3"` / `target="1.5"` ("Source option 1.5 is no
  longer supported. Use 1.6 or later."). This is the classic JDK-11+ vs old-Java-build
  wall; `solenv/inc/antsettings.mk:90` sets `-Dant.build.javac.source/target=1.8` but
  commons-logging's build.xml overrides it with explicit `<javac source target>`.
- **Decision: build with JDK 8 instead** (AOO historically targets JDK 7–9). Switching
  the JDK sidesteps this whole class of failures rather than patching each bundled
  Java build. Need an **arm64** JDK 8 — the installed AdoptOpenJDK 8 is x86_64.
  `main/mac-silicon-configure.sh` default JAVA_HOME → `.../temurin-8.jdk/...`.
- Build process stopped; `logging.patch` reverted to pristine.

### JDK 8 must be native arm64 (no Rosetta)

User is disabling Rosetta, so the x86_64 Temurin 8 / AdoptOpenJDK 8 are out.
`temurin@8` has **no** macOS arm64 build. **Amazon Corretto 8 does**
(`corretto@8` cask → `amazon-corretto-8.504.01.1-macosx-aarch64.pkg`). Azul Zulu 8
arm64 also exists as an alternative. Everything else in the toolchain is already
native arm64 (clang, dmake, epm, gpatch, gawk; `/usr/bin/perl` is universal).

### `build.pl --all` failures seen so far

| Module | Failure | Fix |
| --- | --- | --- |
| `apache-commons/commons-logging` | javac `source 1.5` rejected | switch to JDK 8 (done) |
| `hyphen` | `awk: calling undefined function gensub` — the bundled `hyphen-2.8.8` Makefile calls `awk`; macOS `awk` is BSD, `gensub` is gawk-only | `brew install gawk` (done); `mac-silicon-build.sh` puts `/opt/homebrew/opt/gawk/libexec/gnubin` (exposes gawk as `awk`) first on PATH |
| `apache-commons/commons-logging` (2nd) | `servlet-api.jar ... class file has wrong version 55.0, should be 52.0` — the earlier JDK 11 run had delivered Java-11 `servlet-api.jar` / `saxon.jar` / `hsqldb.jar` into `solver/` | wiped all 21 `*/unxmaccr.pro` dirs + `main/solver`, re-bootstrapped, full clean rebuild under arm64 Corretto 8 |

**Full clean rebuild started under arm64 Corretto 8** (build-all-4.log). Prior partial
builds were a JDK-11/JDK-8/x86_64 mix — not trustworthy for a production build.

| `xmlsecurity` (~149 modules in, run 4) | `clang++: error: no such file or directory: '.../MacOSX15.5.sdk/usr/lib/libxml2.a'` — `xmlsecurity/util/makefile.mk:179` links `$(LIBXML_PREFIX)/lib/libxml2.{dylib,a}`; configure had picked the SDK's `/usr/bin/xml2-config` as "system libxml", but the SDK ships only `.tbd` stubs (and its libxml2 is too old for bundled xmlsec1 per the makefile's own comments) | `--without-system-libxml --without-system-libxslt` in `mac-silicon-configure.sh` → build both bundled (also correct for a self-contained `.dmg`). On Darwin configure defaults these to system unless explicitly `no`; both must be disabled together (interlock). Re-bootstrap fetched `libxml2-2.9.10` + `libxslt-1.1.34`. Resumed → build-all-5.log. |

vcl (native aqua backend), the arm64 UNO bridge (jvmfwk/bridges), svx, sfx2, framework,
editeng all built clean before this — the arm64 C++ port is largely sound.

New wrapper: **`main/mac-silicon-build.sh`** — prepends the gawk gnubin dir, sets the
CA bundle, sources `MacOSXAARCH64Env.Set.sh`, runs `build.pl --all -P4 --stoponerror`
(passes through any args, e.g. `--from hyphen`).

## Milestone 2 — RESULT: whole codebase compiles on Apple Silicon ✅ (2026-09-03)

`./mac-silicon-build.sh` (build-all-6.log): **166 modules built, zero compile errors**,
under native arm64 Corretto 8 / clang 17 / macOS 15.5 SDK / deployment target 11.0.
Includes VCL (native Cocoa/aqua backend), the arm64 UNO C++ bridge (`bridges`,
`jvmfwk`), `sal`/`cppu`, `svx`, `sfx2`, `framework`, `editeng`, `sw` (Writer),
`sc` (Calc), `sd` (Impress), `chart2`, `dbaccess`, `reportdesign`, `xmlsecurity`,
bundled `python`/`curl`/`libxml2`/`libxslt`/`openssl`/`nss`/`hunspell`/`icu`/`coinmp`.

**The `.dmg` assembly (`instsetoo_native`) is the only thing still failing**, on
install-manifest (`scp2`) vs build-output mismatches:

| Missing file (per `remove_Files_Without_Sourcedirectory`) | Cause | Status |
| --- | --- | --- |
| `libcrypto.dylib.3`, `libssl.dylib.3` | `scp2/inc/macros.inc:548` `SCP2_URE_DL_VER` emits Linux-style `libNAME.dylib.V`; `main/openssl` (openssl-3.0.20) delivers macOS-style `libNAME.V.dylib` | **fixed** — `MACOSX` branch in `scp2/source/ooo/file_library_ooo.scp` (`gid_File_Lib_Openssl`, `gid_File_Lib_Crypto`) → `libssl.3.dylib` / `libcrypto.3.dylib` |
| `OOoPython.framework.zip` | `scp2/source/python/*.scp` still expects the old macOS **framework** layout (`OOoPython.framework`, `Python.app` symlinks, `PYTHONHOME` rel to framework). The Python-3 migration made `python/makefile.mk` build a **plain `--enable-shared` Unix install** on all UNX incl. macOS — no `--enable-framework`, no `.zip`. | **OPEN — next work item** |

### Next work item: macOS bundled-Python packaging

Pick one:
- **(a)** `python/makefile.mk` macOS branch: `./configure --enable-framework=<path>/OOoPython.framework`
  + produce `OOoPython.framework.zip`; keep `scp2/source/python/*.scp` as-is. (Closest to
  what scp2 already wants; risk is `--enable-framework` on arm64 CPython 3.10 + AOO patches.)
- **(b)** Rewrite `scp2/source/python/{file,profileitem,makefile,module}_python.scp` for the
  Unix layout on macOS (mirror the Linux `#else` paths: `python-core-3.10.zip`,
  `libpython3.10.dylib`, `program/python-core-3.10/lib/python3.10/...`), and fix pyuno's
  runtime path resolution accordingly.

After that: rerun `./mac-silicon-build.sh`; if the DMG builds, smoke-test
`instsetoo_native/.../Apache_OpenOffice.app` (launch Writer, run a Basic macro, try a
`pyuno` script). Then: code-signing / notarization for distribution (not done on this
branch).

## Milestone 3 — RESULT: the `.dmg` builds ✅ (2026-09-03)

`./mac-silicon-build.sh --from scp2` → **`Successful packaging process!`**. Produced
under `main/instsetoo_native/unxmaccr.pro/`:
- `Apache_OpenOffice/dmg/install/<lang>/Apache_OpenOffice_5.1.0_MacOS_aarch64_install_<lang>.dmg`
  for en-US de es fr it nl pt ru (~155–198 MB each)
- `Apache_OpenOffice_SDK/dmg/install/en-US/Apache_OpenOffice-SDK_5.1.0_MacOS_aarch64_install_en-US.dmg`

Verified (mounted en-US dmg): proper layout (`OpenOffice.app`, `Applications` symlink,
LICENSEs, READMEs); `OpenOffice.app/Contents/MacOS/soffice` = **Mach-O arm64**;
`python-core-3.11.15/lib/urllib/` complete.

### Milestone-3 fix: macOS bundled-Python packaging (option b)

`scp2` was written for a pre-2011 macOS Python **framework** (`OOoPython.framework.zip`)
that no build has produced in years — `python/makefile.mk` + `pyuno/zipcore/makefile.mk`
build the Unix `python-core-$(PYVERSION).zip` layout on macOS just like Linux. Switched
the macOS `scp2` branches to that layout:
- `scp2/source/python/file_python.scp`: `gid_File_Py_Python_Core` → `python-core-<ver>.zip`
  for macOS too; enable `gid_File_Py_Python_Bin` on macOS; replace the whole framework
  `Directory`/`Unixlink` block with a plain `gid_File_Lib_Python_So` (`libpython3.11.dylib`
  into `program/`).
- `scp2/source/python/profileitem_python.scp`: macOS `PYUNO_LOADER_PYTHONHOME` /
  `PYTHONPATH` use `$ORIGIN/python-core-<ver>` like the other UNX.
- `scp2/source/ooo/file_library_ooo.scp`: `gid_File_Lib_Xslt` — skip on macOS (bundled
  libxslt is static there, like libxml2, so no shared lib to ship).

### Benign warnings (verified harmless)
- "ERROR: The following errors occurred in packaging process: ... Copy: .../urllib/error.py"
  — `error.py` is nonetheless present in the mounted dmg; the installer double-lists that
  first file and logs a scary line. dmake returns 0, "Successful packaging process!".
- "Some modules contain old output trees" — leftover from earlier partial runs; cosmetic.

## Milestone 4 — a signable bundle layout ✅ (2026-09-05)

The `.dmg` now ships an application that passes `codesign --verify --deep --strict`.
The fix is in the build and the installer, not in a post-processing pass.

    ./mac-silicon-sign.sh /Applications/OpenOffice.app                  # ad-hoc
    ./mac-silicon-sign.sh -i "Developer ID Application: NAME (TEAMID)" OpenOffice.app
    ./mac-silicon-sign.sh --verify OpenOffice.app
    MACOSX_CODESIGNING_IDENTITY=- ./mac-silicon-build.sh                # signed .dmg

### What was wrong

The linker ad-hoc-signs every Mach-O on arm64 (`flags=0x20002 adhoc,linker-signed`), which
is why all 367 dylibs load. But no *bundle* was ever sealed: `Sealed Resources=none`, no
`_CodeSignature/`, identifier `soffice.21391-1788411067` (linker-invented). Upstream never
fixed it — **official AOO 4.1.15 is "code object is not signed at all"** with this same
layout.

`codesign` refuses to seal a bundle that has anything but Mach-O in `Contents/MacOS`
(verified: one stray data file is enough), and `gid_Brand_Dir_Program` put AOO's entire
payload there — 993 non-Mach-O files plus loose `NOTICE`/`README` in `Contents`.

### The layout change

`Contents/MacOS` now holds only the `soffice` launcher; the installation sits in
`Contents/program`, the same directory name every other UNX platform uses. Nothing moves
relative to anything else.

| Change | Why |
| --- | --- |
| `scp2/source/ooo/common_brand.scp`: `gid_Brand_Dir_Program` → `Contents/program`; `gid_Brand_File_Bin_Soffice` → new `gid_Dir_Bundle_Contents_MacOS`; dropped `gid_Brand_Unixlink_Program` (`program -> MacOS`) and `gid_Brand_Unixlink_Urelibs` (dangling in *every* AOO build incl. 4.1.15 — a pre-3.4 basis/ure leftover that `--deep --strict` rejects); added `gid_Brand_Unixlink_Soffice` (`program/soffice -> ../MacOS/soffice`) | the payload leaves `MacOS`; the launcher stays as `CFBundleExecutable` (codesign rejects a symlinked main executable — verified); the compat link keeps `findsofficepath()` and the SDK working |
| `sal/rtl/source/bootstrap.cxx` (`#ifdef MACOSX`) | `getIniFileName_Impl` maps `…/Contents/MacOS/<exe>rc` to `…/Contents/program/`. `$ORIGIN` comes from the *ini file's* path (`bootstrap.cxx:496`), so this one hop anchors the whole chain — `URE_LIB_DIR`, `OOO_BASE_DIR`, services.rdb, unorc. Falls back to the old path if the file is not there (plain bundles, the mdimporter) |
| `desktop/util/makefile.mk` + `macosx-change-install-names.pl` + `unxmac{c,x}.mk` | new mac-only `BRANDBIN` rpath location = `@executable_path/../program`, used for `soffice` only. `OXT`/`BOXT` libraries switched from `@executable_path` to `@executable_path/../program` — correct from both the launcher and the helper binaries in `program` |
| `icu/icu4c-4_2_1-src.patch` (+ `icu-darwin.patch`) | ICU baked `-install_name @executable_path/…`; now `@loader_path/`, since ICU libs are loaded by their neighbours, not by the launcher |
| `installer/scriptitems.pm` (`ismacosx` guard) | the generic "copy README/LICENSE to the installation root" step targets OFFICEDIRECTORY = `Contents` on macOS. Suppressed there; the copies users actually see are the ones in the `.dmg` root |
| `scp2/source/ooo/ooo_brand.scp` | `NOTICE` went to `Contents` on macOS only; now next to `LICENSE` in `program` on every platform |
| `installer/simplepackage.pm` | `hdiutil makehybrid -hfs` stamps an empty `com.apple.FinderInfo` on **every** file in the image, which makes `--strict` reject the app inside the `.dmg` (and would fail notarization). Replaced with `hdiutil create -srcfolder`. The `-hfs-openfolder` auto-open it also did is dead anyway: `bless --openfolder` is unsupported on Apple Silicon. Also signs each `.app` before imaging when `MACOSX_CODESIGNING_IDENTITY` is set |

Everything is macOS-scoped: `#ifdef MACOSX` / `.IF "$(OS)" == "MACOSX"` / `ismacosx`,
mac-only files (`unxmac*.mk`, `macosx-change-install-names.pl`), or Darwin-only patch hunks
(`config/mh-darwin`). The one cross-platform line, `gid_Brand_Dir_Program`'s `DosName`, was
already `"program"` everywhere but macOS.

### Verified

- App inside the built `.dmg`: `codesign --verify --deep --strict` → **valid on disk,
  satisfies its Designated Requirement**. Same after copying it out of the image.
- Boots to the first-start wizard (sampled `FirstStart::execute`), signed and unsigned.
- `Contents/program/python -c "import uno"` → `pyuno ok 3.11.15`; `unopkg list` lists the
  bundled dictionary extensions (exercises the OXT install-name path).
- `Contents/MacOS` contains exactly one file; no dangling symlinks anywhere in the bundle.

### Bisected along the way

`Contents/presets` must stay a real directory in `Contents` — relocating it kills startup
in `Desktop::HandleBootstrapErrors → FatalError`. Directories in `Contents` seal fine as
resources, so only the loose files ever needed to move.

### Adding artwork: packimages does not rebuild on its own

The Start Center's PNGs are **not** compiled into the `.res` (`fween-US.res` is ~8 KB).
`Bitmap ... File = "x.png"` entries are resolved at runtime from
`Contents/share/config/images.zip`, which `packimages` builds. Its rule is

    $(WORKDIR)/CustomTarget/packimages/bin/images.zip : $(COMMAND_IMAGE_LIST)

so it depends only on `commandimagelist.ilst` -- not on the per-module `.ilst` lists
and not on `default_images/`. Adding an image therefore does **nothing**: `make` in
`packimages` reports success, `images.zip` keeps its old timestamp, and the image is
simply absent at runtime. Nothing fails; the picture just never appears.

To actually pick up new artwork, delete the zip first:

    rm -f solver/510/unxmaccr.pro/workdir/CustomTarget/packimages/bin/images.zip \
          solver/510/unxmaccr.pro/bin/images.zip
    (cd packimages && make)
    unzip -l solver/510/unxmaccr.pro/bin/images.zip | grep <your image>

Verify the count before packaging -- a clean exit is not evidence the asset shipped.
`rsc` *does* pick new `Bitmap` entries up into `solver/.../res/img/*.ilst` correctly,
so the list is right; only the zip is stale.

Also note: two packaging runs must never overlap. Both write
`instsetoo_native/unxmaccr.pro/.../dmg/install/en-US_inprogress`, and the second dies
with `Could not create parent directory` (dmake error 255), leaving a stray
`Apache_OpenOffice_*_install_en-US/` directory that must be removed by hand.

### Still open for distribution

- A signing identity now exists: `Apple Development: Peter Kovacs (GW985ZS85D)`, issued
  to `O=The Apache Software Foundation` (Team `2GLGAFWEQD`), in the login keychain,
  valid to 2027-09-09. Select it with
  `export MACOSX_CODESIGNING_IDENTITY="Apple Development: Peter Kovacs (GW985ZS85D)"`;
  no `MACOSX_CODESIGNING_KEYCHAIN` is needed, the login keychain is already searched.
  - Its issuer is WWDR **G3**, an intermediate that ships in *neither* the login nor the
    System keychain. Without it `codesign` fails with `unable to build chain to
    self-signed root` / `errSecInternalComponent` and `find-identity -v` reports 0 valid
    identities — while `security verify-cert` *succeeds*, because it fetches the chain
    over the network and codesign does not. Import the copy Xcode already ships:
    `security import /Applications/Xcode.app/Contents/SharedFrameworks/DVTFoundation.framework/Versions/A/Resources/AppleWWDRCA-2030.cer -k ~/Library/Keychains/login.keychain-db`
    (`AppleWWDRCA-2030.cer` is G3 despite the name; `AppleWWDRCAG6.cer` is G6, the wrong
    generation for this leaf.)
- **Signing through the build is verified working** (2026-09-09). `simplepackage.pm`
  signs each `.app` before `hdiutil create`; no code change was needed, only the env var:

        cd main && source ./MacOSXAARCH64Env.Set.sh
        unset SDKROOT MACOSX_SDK_PATH      # else xcrun inherits the stale pin below
        export SDKROOT="$(xcrun --show-sdk-path)" MACOSX_SDK_PATH="$SDKROOT"
        export MACOSX_CODESIGNING_IDENTITY="Apple Development: Peter Kovacs (GW985ZS85D)"
        cd instsetoo_native/util && dmake openoffice_en-US.dmg

  Produces 367 signed Mach-O objects, `Sealed Resources version=2 rules=13 files=4036`,
  identifier `org.openoffice.script` (was the linker-invented `soffice.<n>-<n>`), and the
  app inside the `.dmg` passes `codesign --verify --deep --strict`.
- **Unattended signing needs the key ACL opened once, or the build hangs forever.** By
  default the private key prompts on every use; `codesign` spawns SecurityAgent and
  blocks with no output and no error, so the packaging step stalls indefinitely rather
  than failing. Symptom: a `codesign` process in state `S` for hours, while
  `codesign --sign -` still works (ad-hoc touches no keychain). Answer one *live* prompt
  with **Always Allow**, or run
  `security set-key-partition-list -S apple-tool:,apple:,codesign: -s -l "<cert CN>" ~/Library/Keychains/login.keychain-db`.
  A prompt whose requesting process has already died is a zombie: no password will ever
  be accepted, so dismiss it before retrying.
- The packaging run ends with `ERROR: The following errors occurred in packaging process`
  naming a `Copy:` of `python-core-*/lib/urllib/error.py`. **False positive** — the file
  is copied correctly; `make_installer.pl` greps its own log for error patterns and hits
  the *filename*. dmake exits 0 and the `.dmg` is sound.
- Every entitlement is a `com.apple.security.cs.*` relaxation plus apple-events; there is
  no sandbox, App Group or iCloud entitlement, so the bundle needs **no embedded
  provisioning profile** and is not restricted to registered devices.
- `MacOSXAARCH64Env.Set.sh` still pins `MacOSX15.5.sdk`, which is not installed (only
  `MacOSX.sdk`, `MacOSX26.5.sdk`, `MacOSX26.sdk` are). The override above works per-run;
  the real fix belongs in `set_soenv.in` at configure time, not in the generated file.
- **`Apple Development` is not a distribution certificate.** It cannot be notarized, so
  `spctl --assess` still rejects and a downloaded `.dmg` is blocked on first launch. That
  is workable for hand-to-hand testing — each tester overrides once via System Settings
  → Privacy & Security → "Open Anyway", or strips quarantine with
  `xattr -dr com.apple.quarantine /Applications/OpenOffice.app` — but it is not fit for
  public release. That needs **Developer ID Application**, which only ASF's Account Holder
  can issue from the team's Apple Developer Program membership.
- Then `xcrun notarytool submit --wait` the `.dmg`, `xcrun stapler staple`, and sign the
  `.dmg` itself (`mac-silicon-sign.sh -i <ID> foo.dmg`).
- Installing a *shared* extension writes into `share/uno_packages` inside the bundle and
  breaks the seal. That is inherent to signing an app that modifies itself, not to this
  layout change.

## Status / Next

**A native arm64 Apache OpenOffice 5.1.0 `.dmg` for Apple Silicon builds from trunk, and
the application it installs is code-signable and verifies strictly.**

**Next (not done):**
1. Full 8-language + SDK repackage (only `openoffice_en-US` has been rebuilt since the
   layout change; `dmake openoffice_en-US` in `instsetoo_native/util` is the fast loop).
2. Developer ID + notarization (needs the certificate above).
3. Fuller smoke test: Writer/Calc round-trip, a Basic macro, a pyuno script against a
   running soffice.
4. From-scratch `./mac-silicon-build.sh` on a clean tree to confirm no ordering luck.
5. NSS 3.39 age — compiled fine; watch at runtime (signatures / cert UI).

**Milestone 1 (configure + bootstrap) DONE** — committed on `mac-silicon-minimal`
(`503d3e75c9`): `main/mac-silicon-configure.sh`, `main/mac-silicon-bootstrap.sh`,
`main/.gitignore` (`/macos_c11`), this doc. (`unowinreg.dll` gitignored, local only.)
