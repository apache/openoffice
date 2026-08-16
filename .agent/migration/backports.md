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

# Trunk backports

Periodically port upstream trunk fixes that bazel-migration lacks.  METHOD:
`git cherry bazel-migration origin/trunk <merge-base>` — `-` marks commits ALREADY
present by patch-id (our own win64 + sal fixes went upstream, so they show `-`;
never re-apply those), `+` marks real candidates.  Then `git cherry-pick -x` so the
upstream hash is recorded in the message.

BACKPORT #1 LANDED 2026-07-26 (branch trunk-backports off bazel-migration, 14 commits,
GREEN per user build).  Two groups:
  • cross-platform bug fixes: package ZipPackageFolder (HSQLDB doc corruption+crash
    regression, 7f52a288cb), package ZipFile bounds check, extensions/update crash+
    uninit-return, sc dbdocimp check, ww8scan missing includes, ww8graf invalid-enum
    removal, connectivity jdbc Tools.java cyclic-cause recursion (Java, inert until
    the Java bucket lands).
  • Damjan's Win64 SOURCE fixes, complementary to our x64 port (NOT duplicates):
    ADO driver types (2), unixODBC/OFunctiondefs types (2), dbase Min() types,
    avmedia IMediaEvent::GetEvent, and winaccessibility amd64 (7d7b15ea9e — changes
    PUBLIC XMSAAService.idl long→hyper; offapi + UAccCOM MIDL regen; merged cleanly
    with our existing x64 salframe.cxx edits).
  DEVIATION: 9d1a529d5e's macOS-only s5abi_macosx_x86-64/cpp2uno.cxx hunk was DROPPED
  (depends on upstream 11db77d7e7 macOS x86 bridge rework, not carried here); only its
  extensions/update files were taken.  Noted in that commit's body.

BACKPORT #2 LANDED 2026-07-29 (idxf DXF import filter, SQUASHED to one commit).
Unlike #1 this work ORIGINATED here: it was developed on the local branch
dxf-filter-fix using Bazel purely as the fast iteration harness, went upstream as
apache/openoffice#491 (merge c082241a9b, 2026-07-28), and was then brought back from
trunk.  LANDMINE for the `git cherry` method above: because it was squashed, the 11
upstream commits have NO patch-id match here and WILL show as `+` candidates
(999d0e5d3c 22079cd995 65c7e1576f e345bd9383 030749d53e 82ba6d6560 c4b51fde1e
5ed424c6ce 3931960203 05e2156ffd 96b0524be2).  Do NOT re-apply them — they are
already in, verified tree-identical to trunk's post-PR state; the list also lives in
the squash commit's body.  Source-only, 11 files:
`main/filter/source/graphicfilter/idxf/*` (scaling/extents, arc+ellipse+spline geometry,
LWPOLYLINE bulge, linetype dots, HATCH patterns, 3D OCS/WCS + VPORT, $DWGCODEPAGE
encoding) + main/svtools/source/filter/filter.cxx (DXF detector tolerates leading 999
comments).  Root causes per defect: main/filter/source/graphicfilter/idxf/Readme.md.
  DEVIATION: the PR's .gitignore hunk was DROPPED — bazel-migration's .gitignore
  already covers bazel-*/user.bazelrc/.vscode, and trunk's `*.lock` + whole-dir
  `.claude/` are wrong here (MODULE.bazel.lock is tracked).
  NOT carried to AOO42X/AOO41X yet.  The 13.5 MB Bugzilla sample corpus and the
  still-open cases (99892 Phase B encoding-chooser UI, text-dense thumbnail) live on
  the local branch bugfix-dxf-filter-open, deliberately out of this tree.

BACKPORT #3 LANDED 2026-08-02 (Damjan's Win64 type-correctness follow-ups, 4 commits,
GREEN on BOTH arches per user build).  These refine hunks ALREADY in this branch —
two of them (dllmgr, sfx2 Get10ThSec) are trunk rewriting OUR OWN upstreamed edits, so
skipping them would have left those files permanently divergent and re-flagged by every
`git cherry` run.  None fixes a live x64 defect (uInt32→uLong is a widening, `IMR_*` are
small constants, Windows clock_t is 32-bit long on both arches) — the value is drift
removal.  Source-only, 5 files: vcl salgdi.cxx ImplPreparePolyDraw nCurrPoint(s)
sal_uLong→sal_uInt32 (48f8cc8f69, completes 30f18d3fb8), vcl salframe.cxx drop the
static_cast<int>(wParam) on IMR_* (1e5d8e72b5), basic dllmgr.cxx guard
!defined(_WIN64)→defined(INTEL) (14a9b8c1d9 — equivalent here, the toolchain injects
INTEL/X86_64 globally, see build/toolchain/windows_cc_toolchain_config.bzl), sfx2
Get10ThSec() sal_uInt32→clock_t + SfxStatusIndicator::_nStartTime long→clock_t
(448dc86220; progress.cxx already includes <time.h> at line 54, no new include needed).
  NOT PORTED from the same trunk batch, because the Bazel side is ALREADY AHEAD:
  29ca9883b9 (link `version` for VerQueryValueW on x64) — vcl/BUILD.bazel already has
  version.lib; 25912db745 (Win64 vcl entrypoint) — we already select /ENTRY:LibMain on
  x64, and trunk's `/ENTRY:LibMain@16` looks WRONG: MSVC x64 drops the stdcall @N
  decoration, so the symbol from vcl/win/source/app/salshl.cxx is undecorated LibMain
  (that is what links and boots here).  Report upstream.
  ALSO NOT PORTED: f116fde0f6 + the packregistry half of 70996efe05 (ADO component path
  `drivers/ado`→`source/drivers/ado`, ADABAS platform condition) — both already correct
  in main/postprocess/BUILD.bazel, which resolves components by TARGET LABEL so the
  malformed path cannot exist in our form.  70996efe05's BFunctions.cxx half is a
  UNX/MACOSX-only refactor (WNT branch untouched) = no-op here.  The gbuild ports in
  that batch (connectivity 07bf101f83/5db65c3a88, chart2 89e8f0e82d/f1c404e081/
  93fbba6999 + 0a9d806e68 version-map workaround, officecfg ef56053619/323ac1e9a6/
  9e90d1d104, solenv Configuration.mk 431aafefb5/4dc45a5a3e, Ant jar sealing 861c48608f)
  have NO Bazel equivalent — Bazel replaces that layer.
  DRIFT TO WATCH (not actionable yet): 861c48608f also MOVES the DataAccess *.xcu out of
  connectivity into officecfg registry/data/org/openoffice/Office/DataAccess/ and adds
  *.xcs schemas for them; main/postprocess/BUILD.bazel still references the old
  //main/connectivity:source/drivers/*/*.xcu paths and would break on a future sync.
  d50e6300ed (drop -Xbootclasspath, unsupported on Java > 8) is inert now but is a real
  constraint for scripting/java when the Java bucket lands.

DO NOT PORT (VC9 island landmines — see the trunk drift audit above): boost 1.84
(2e60be5056), pyuno→Python 3 (27dee00ef8), C++11 floor (7ce5b5df31), autoconf 2.72,
the macOS/Apple-Silicon batch, GTK/iODBC/lld/icc-Clang16.

STILL OPEN — dmake-only trunk commits with NO cherry-pick equivalent, but whose Win64
facts our Bazel targets may be missing: crashrep must link psapi on x64 (7e339f4bc5),
OpenSSL lib names carry an `-x64` suffix (2cc1933ab8 + 6a1563a7a3 CPUNAME in SCPDEFS),
coinmp x64 build flags (23448c919c), NSS/NSPR win64 patch (22f8d3ac55).  These are a
BUILD-file audit, not a port.
