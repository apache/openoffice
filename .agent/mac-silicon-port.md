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

## Status / Next

**Milestone (configure + bootstrap) DONE.** Committed on `mac-silicon-minimal`:
`main/mac-silicon-configure.sh`, `main/mac-silicon-bootstrap.sh`, `main/.gitignore`
(`/macos_c11`), this doc. (`unowinreg.dll` is gitignored, present locally only.)

**Next (new milestone — reassess with user first):**
`cd main && . ./MacOSXAARCH64Env.Set.sh && cd instsetoo_native && ../solenv/bin/build.pl --all`
— expect real C++/clang-17 port work (NSS 3.39 too old for arm64; bundled curl/python/
hunspell under clang 17; Java build under JDK 25 vs 11). ~20 GB, hours.
