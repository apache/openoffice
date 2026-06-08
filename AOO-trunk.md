# trunk vs AOO42X — Functional Difference Analysis

**Date:** 2026-06-08
**Repository:** apache/openoffice
**Branches compared:** `trunk` (future 4.5.x / "dev") vs `AOO42X` (the 4.2.x release branch)

---

## 1. Summary

| | trunk | AOO42X |
|---|---|---|
| Branch tip | `9d1a529d5e` | `4ccec83d37` |
| Tip commit subject | *Fix crash, uninitialized-return, and buffer bugs…* | *(same)* |
| Common ancestor | `a341bc611d` (2019-01-09) | (same) |
| Commits ahead of merge-base | 2933 | 2486 |
| Commits with **no patch-equivalent** on the other side | 711 | 318 |
| **Python baseline** | **3.10.18** | **2.7.18** |
| Apple Silicon (arm64) support | yes | yes |
| Win64 native UNO C++ bridge | **yes** | **no** |

### Headline findings

1. **No functional capability exists in AOO42X that is missing from trunk.** Every
   functional AOO42X-only commit was verified to be present in trunk (most under a
   different commit hash because of whitespace/pre-commit divergence — see §2).
   The 318 "AOO42X-only" commits by patch-id are almost entirely **release-branch
   bookkeeping** (version/build-number bumps, copyright-year updates, SVN-era
   `Merge rNNNNN from trunk` backports) plus **spelling / pre-commit whitespace**
   commits that were applied to AOO42X in a different order than trunk.

2. **The two branches are functionally very close.** The vast majority of the 711
   trunk-only commits are **non-functional**: ~270 are cleanup/typo/spelling/whitespace,
   plus a large block of README/help/translation/graphics-asset updates and test-tree
   reorganization. Most genuinely-functional work (editeng 32-bit paragraph indices,
   WebDAV→curl port, dynamic OpenSSL, gbuild port of pyuno/ure, AdoptOpenJDK detection,
   frame-toolbar fix) has **already been merged to AOO42X** via squash/merge commits and
   is present on both branches. The Python file-editing/embedding base feature was merged
   to both branches on the same day (2026-05-24) as parallel commits (`4bb26738ff` trunk,
   `c779acf6cb` AOO42X), but trunk has since accumulated ~10 additional post-merge
   improvements to that feature that are not yet on AOO42X (see §3a).

3. **The single biggest functional divergence is the platform baseline:**
   trunk targets **Python 3.10** and ships a **native 64-bit Windows UNO bridge**
   (`msvc_win64_x86-64`); AOO42X deliberately stays on **Python 2.7** and has **no
   Win64 bridge**. These are intentional release-stability decisions, *not* missing
   cherry-picks (cf. the explicit AOO42X commit *"revert the Python 3.10 changes,
   keep arm64 support"*).

4. A **small set of discrete bug fixes is genuinely trunk-only and is suitable for
   cherry-pick** to AOO42X (see §3).

---

## 2. Why the commit counts overstate the divergence

`git cherry`/patch-id reports 711 trunk-only and 318 AOO42X-only commits, but most are
*not* real functional gaps:

- **Pre-commit whitespace hooks.** trunk adopted `pre-commit` end-of-file-fixer,
  trailing-whitespace, and mixed-line-ending hooks and ran them across the whole tree
  (PRs #255–#478). AOO42X ran an equivalent but separately-ordered set. This reformats
  thousands of files, so the *same logical change* produces *different patch-ids* on the
  two branches and shows up as "unique" on both sides.
  - Example: the X11 autoscroll-cursor fix (`assw_curs.h`) appears unique to AOO42X, but
    the actual hex byte values are **identical** to trunk — only the per-line formatting
    (tabs, bytes-per-line) differs.

- **Release bookkeeping (AOO42X side).** Build-number pre-allocation, `Update copyright
  year to 2022…2026`, `Bump up revision after tagging DevN`, MSI version strings, and
  SVN-era `Merge rNNNNN from trunk` commits dominate the AOO42X-only list.

- **Squash/merge re-landing.** Large features were merged into AOO42X as single squash
  commits (e.g. `c779acf6 Merge branch 'python-editing-embedding'`,
  `08fafdc14b AOO42X: revert the Python 3.10 changes, keep arm64 support`), so the
  individual trunk commits that compose them never match by patch-id even though the
  resulting source is identical.

### Functional AOO42X-only commits — all verified present in trunk

| AOO42X commit | Verified in trunk as |
|---|---|
| WebDAV provider serf/apr → curl (`9646dec`) | `51ba086bf1` (+ follow-ups `e469ab6`, `f7b97bf`, `88ba7bc`…) |
| OpenSSL built as dynamic library (`e350def`) | `8eb9a7e66a`, `0ca5b4b7b8` |
| editeng 32-bit paragraph indices (PR #164) | `d5edfd0976` + full series; `GetParagraphCount()` returns `sal_uInt32` on both |
| SpreadsheetML `ss:DateTime` import (`48b860b`) | `195282cf3d` |
| SpreadsheetML R1C1→A1 column bug (`0f570a5`) | `577fe17932` |
| ODF `covered-table-cell` / `ss:MergeAcross=0` (`b9aa069`) | `a896732bfc` |
| icu module parallel build fix (`93c12e1`) | `17604d2661` |
| APR 1.6.x upgrade / Xcode12 (`316716d`) | `b1dc36c016` |
| Basic single-line-if comment test #i126272 (`3b5ec47`) | `07758af81c` |
| Tab function test document (`a1409b3`) | `3b297b4615` |
| X11 / OS/2 / macOS pointer & cursor corrections | byte-identical in trunk (whitespace-only diff) |
| macOS SDK / C++ build tweaks (`0e91ffb`, `3d8a723`) | superseded by trunk's newer arm64 `configure.ac` |
| beanshell d.lst path/version (`2aea6da`, `2dbdbb8`) | identical `bsh-2.1.0.jar` mapping in trunk |

**Conclusion for the "no regression" requirement: PASS.** AOO42X contains nothing
functional that trunk lacks.

---

## 3. Trunk-only functionality SUITABLE for cherry-pick to AOO42X

These are discrete, self-contained, low-risk fixes confirmed by file-content diff to be
trunk-only (the touched file genuinely differs between the branches), and they do **not**
depend on the Python-3 or Win64 baseline.

### 3a. Recommended cherry-picks (bug fixes)

| Area | trunk commit(s) | What it fixes | Risk |
|---|---|---|---|
| **Calc / MS Excel 2003 XML import** | `d10bb11432` | Importing references to **columns 677–702** (and beyond) was wrong: the old XSLT used a `÷26 − 1` scheme instead of base-26² (676). Touches `spreadsheetml2ooo.xsl`. | Low — isolated XSLT + test doc |
| **Calc / Excel 2003 XML export** | `b045a72b`, `3714104178`, `50f8b0f2cf` | Use the correct `of:`/`ooow:` namespace and R1C1 form when exporting ODF formulas to Excel 2003 XML; allow exporting ODF <1.2 formulas. | Low–medium |
| **Win64 portability — file handles** | `5a41e572de` | `SvStream::GetFileHandle()` returns `sal_uInt32` capped at 32 bits; widen to support 64-bit Windows. (`tools/inc/tools/stream.hxx`, `strmunx.cxx`) | Low (also benefits non-Win64) |
| **Win64 portability — time_t** | `926379c890` | `rsc` preprocessor used a 32-bit-capped `long` for `time_t`; use native `time_t`. (`rsc/source/rscpp/cpp3.c`) | Low |
| **i18n unit tests** | `de76a5a`…`cc5c65fb` | New/relocated i18n JUnit API tests under `test/testuno/source/api` (11 files, absent on AOO42X). | None (test-only) |
| **`senddoc.sh` — xdg-email** | `a946b6b` | On \*nix, call `xdg-email` to send emails when `MAILER` is unset. The 42-line block is entirely absent from AOO42X's `senddoc.sh` (55-line diff confirmed). | Low — shell script only |
| **`senddoc.sh` — claws-mail** | `14c7d0b586` | Add claws-mail as a recognised mailer in `senddoc.sh`. Same file, same low risk as xdg-email entry above. | Low — shell script only |
| **Python editor post-merge fixes** | `a7a66e361b`, `8e23be7474`, `1a9bb40f52`, `1b85a9366d`, `d2cc3a1ab9`, `36f82a9eeb`, `511d7659d6` | ~10 commits improving the Python file-editing/embedding feature landed on trunk after the parallel 2026-05-24 merge. Includes: fixed-width font in editor, `pop()`/`del` key-error fix, module-cache reload fix (ZIP timestamp bug), PythonScriptProvider marked active. | Low–medium (evaluate as a group) |

> Note: the *frame-toolbar-missing-in-Writer* fix (`6bd5b10f`, `1714b106`),
> AdoptOpenJDK/Java>1.8 detection (`2bf1cfb`, `779db4a`), and the Python
> file-editing/embedding *base* feature were **checked and are already present on
> AOO42X** — do **not** re-cherry-pick those. The `xdg-email`, claws-mail, and
> post-merge Python fixes listed above are **not** present on AOO42X.

### 3b. Build-system / infrastructure (cherry-pick only if AOO42X wants the same CI)

| Area | trunk commit(s) | Notes |
|---|---|---|
| **gbuild port** of `pyuno`, `ure`, `packimages`, `apple_remote`, `qadevOOo`, `test`, `testgraphical` | `1c25f25`, `453cfdf`, `147182f`, `e2cbcf0`, `62e2780`, `19ee18f`… | Large; trunk migrated dmake→gbuild for these modules. AOO42X already received the pyuno/ure `.mk` files via merge, but the **JUnit/Ant test migration in `qadevOOo` is the single largest divergence** (2310 files): trunk moved tests into `test/testuno` + JUnit; AOO42X keeps the legacy `qadevOOo/objdsc` CSV-driven framework. Migrating this to a release branch is **high-effort and not recommended** mid-release. |
| **pre-commit hooks + GitHub Actions linter** | `cc42a29`, `4ae038e`, `819addb`, `ffa4afe`, `98279f3` | trunk's `.pre-commit-config.yaml`, codespell word-list, `actions/checkout@v6`, `actions/cache@v5`. AOO42X already backported an equivalent set (`f30d168 Backport pre-commit and CI configuration from trunk`). |

### 3c. NOT suitable for cherry-pick (intentional baseline / release-policy decisions)

| Item | trunk | AOO42X | Why not |
|---|---|---|---|
| **Python 3.10 baseline** | `PYMAJOR=3 / 3.10.18` | `PYMAJOR=2 / 2.7.18` | AOO42X **explicitly reverted** the Py3.10 change (`08fafdc14b`) to keep the 4.2.x release on Python 2.7. Cherry-picking would change the shipped runtime. |
| **Native Win64 UNO C++ bridge** (`msvc_win64_x86-64/`) | full directory present | directory **absent** | A whole new architecture target (`b146d3b`, `b7f28a5`, `4844d31`, `2cf0c06`, `d35e03c`, `8a51f61`…). New platform support belongs in a feature release, not a maintenance branch. |
| Boost / `-std=gnu++98` Clang build tweaks (`8ef8ad0`) | applied | n/a | Tied to trunk's toolchain modernization. |
| Various version-string / build-number / dev-build branding | trunk = 4.5.x line | AOO42X = 4.2.x line | Branch-identity, must stay divergent. |

---

## 4. Largest divergence areas (by changed-file count, `git diff trunk AOO42X`)

```
2310  main/qadevOOo      <- test framework migration (trunk: JUnit/Ant; AOO42X: legacy)
  28  main/filter        <- mostly whitespace + the SpreadsheetML fixes (§3a)
  26  main/jurt          <- JUnit test ports
  25  main/test          <- gbuild/Google-Test port
  23  main/bridges       <- Win64 bridge (§3c) + whitespace
  21  main/forms         <- JUnit complex-test cleanup
  13  main/connectivity
  11  test/testuno       <- new JUnit api tests (§3a)
  11  main/solenv
  10  main/vcl           <- pointer assets (whitespace) + Win64
  10  main/bean          <- Win64 fixes for main/bean
```

The dominant `qadevOOo` delta is **test-infrastructure modernization**, not product
functionality. End-user behavior of the two branches is nearly identical apart from the
Python runtime and Win64 availability.

---

## 5. Methodology

1. `git merge-base trunk AOO42X` → common ancestor `a341bc611d` (2019-01-09, the 4.2.x
   branch point).
2. `git cherry -v AOO42X trunk` / `git cherry -v trunk AOO42X` → patch-id-unique commits
   in each direction (711 / 318).
3. Filtered out non-functional commits (cleanup, whitespace, spelling, pre-commit,
   copyright/version bumps, merges, README/translation/graphics) → ~434 trunk-only
   "signal" commits, manually triaged.
4. For every functional AOO42X-only commit, verified presence in trunk by **commit
   message search and by file-content diff** (`git diff -w trunk AOO42X -- <file>`),
   distinguishing whitespace-only differences from real ones.
5. Confirmed key baselines directly from source: `main/python/pyversion.mk`
   (`PYMAJOR` 3 vs 2), `main/bridges/source/cpp_uno/` directory listing (Win64 bridge),
   editeng `GetParagraphCount()` signature, pyuno gbuild `.mk` presence.

*All commit hashes in this document were resolved against the working tree on 2026-06-08.*
