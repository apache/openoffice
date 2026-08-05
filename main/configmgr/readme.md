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

# Notes for configmgr (done)

- Single DLL: `configmgr.uno.dll`
- Deps: sal, cppu, cppuhelper, salhelper, comphelper, xmlreader, udkapi, offapi, stlport, boost.legacy
- Requires `/Zc:wchar_t-` and stlport (uses boost::unordered_map, hash_map)
- Exports via SAL_DLLPUBLIC_EXPORT — no DEF file

## qa/unit — `//main/configmgr:configmgr_qa_unit`

> **Status: wired and RED. It cannot go green without a source change.**
>
> It builds and links, then dies in the `prerun` fixture step with
> `configmgr.uno.dll: cannot get symbol: component_writeInfo`.
>
> `regcomp -register` registers a component by calling `component_writeInfo()`
> on it — the **pre-`.component` registration mechanism**. AOO retired that:
> `configmgr/source/services.cxx` exports only `component_getFactory` and
> `component_getImplementationEnvironment`, and the tree now has 438
> `.component` files against a handful of `component_writeInfo` exporters (odk
> examples, workbench samples, legacy tests). `qa/unit/makefile.mk` was never
> updated to match.
>
> The obvious escape — hand it a **textual** (XML) services registry instead of
> a binary one — is closed. `test.cxx` calls the *one-argument*
> `createRegistryServiceFactory` overload, which defaults `bReadOnly` to
> `sal_False`, so `SimpleRegistry::open()` receives `(readonly=false,
> create=true)`; the textual branch is reachable only when
> `bReadOnly && !bCreate`. `UNO_SERVICES` does not help either — that belongs to
> `defaultBootstrap_InitialComponentContext`, not to this deprecated path.
>
> **Upstream knows.** The commit that moved this suite to GoogleTest
> (`7231f715d2`, `#i125003#`) says so in its own message: *"All tests fail and on
> Windows it doesn't start running, but these look like problems from before and
> at least it doesn't run during the build."* dmake gates every `qa/` dir behind
> `ENABLE_UNIT_TESTS=NO`, so nobody has run it since. Same class of bit-rot as
> the missing `<windows.h>` in sal's `osl/process` suites — just fatal rather
> than cosmetic.
>
> It stays wired because it *builds*, per the policy that a buildable test gets
> wired even when red on its own merits (cf.
> `//main/writerfilter:writerfilter_qa_doctok`, red on a fixture never checked
> in). The `regcomp` line is deliberately left in: its error names the retired
> mechanism exactly, which beats a vaguer failure later inside `Test::SetUp`.
>
> Fixing it means one of these, all **source** changes: port `test.cxx` to
> `defaultBootstrap_InitialComponentContext` + a `services.rdb` (i.e. make it
> fixture (a), already supported); or pass `sal_True` for `bReadOnly` and feed it
> a textual services registry; or re-export `component_writeInfo` from configmgr
> (reviving what AOO retired — the worst of the three).
>
> Everything below is verified working, and is why the target stays: the mini
> installation stages and resolves, and the four `gtest_test` capabilities it
> drove (`data_tree` / `ure_bootstrap` / `env` / `prerun`) are general.

Mirrors the `TEST` target in `qa/unit/makefile.mk`.  A THIRD test fixture
flavour, next to the two described in `main/test/readme.md`: not an in-process
bootstrap of the staged office (a), and not a launched soffice over URP (b).
The test builds its own service manager with
`cppu::createRegistryServiceFactory(unit.rdb)` and runs against a four-file
installation of its own, whose entire `share/registry` is the `data.xcd` it
ships.  That is the point of it — the assertions are on
`/org.openoffice.Setup Test/AString`, a key the real registry does not have, and
the write cases must not touch a real user profile.

Staged layout (`data_tree`, so the paths are exact — `urebootstrap.ini` resolves
`$ORIGIN` against its own directory, and two files are named `bootstrap.ini`):

    unitdata/urebootstrap.ini                  OOO_BASE_DIR=$ORIGIN/basis, BRAND_BASE_DIR=$ORIGIN/brand
    unitdata/basis/program/uno.ini             generated
    unitdata/basis/program/bootstrap.ini       generated  (UserInstallation)
    unitdata/basis/share/registry/data.xcd     the whole registry under test
    unitdata/brand/program/bootstrap.ini       generated  (same content)

`ure_bootstrap` points `URE_BOOTSTRAP` at `unitdata/urebootstrap.ini` instead of
the staged office's `program/fundamental.ini`; `uno_install` is still needed,
because only the DATA root moves — the UNO DLL closure (including
`configmgr.uno.dll` itself) still comes from `program/` via `PATH`.

### Two divergences from the dmake recipe, both forced

**`unit.rdb` is built by the launcher, not by a build action.**  `regcomp`
bootstraps a UNO service manager, so as a build action it would need its own
staged DLL closure and CRT manifest; worse, the location it records for a
component is an ABSOLUTE path (`registercomponent.cxx::convertToFileUrl`), which
has no business inside a cacheable artifact.  Built per run in the scratch dir,
the path it bakes in is the staged install this run is actually using.
`-br types.rdb` is added (dmake omits it) to keep regcomp's own bootstrap off the
default registry — with it, cppuhelper builds the service manager from the type
registry plus its hardcoded `bootstrap.uno.dll` components, so regcomp never
consults the `URE_BOOTSTRAP` we just redirected at the mini installation.

**`bootstrap.ini` is written to `basis/program/` as well as `brand/program/`.**
The makefile writes it only under `brand/`, but AOO's
`Components::getModificationFileUrl()` reads
`${$OOO_BASE_DIR/program/bootstrap.ini:UserInstallation}` — `OOO_BASE_DIR`, i.e.
`basis/`.  Under the recipe as written that lookup yields nothing and every write
test silently loses its changes; the fixture is stale, not the code.  It is
generated build data rather than source, so it is fixed here rather than worked
around.  `UserInstallation` also cannot be the makefile's `$ORIGIN/../../user`,
which resolves inside `bazel-out` — build output, to be treated as read-only.  It
indirects through `$CONFIGMGR_UNIT_USER`, which the launcher points at the
per-run scratch dir (`writeModFile()` does `Directory::createPath`, so the leaf
need not pre-exist).

`CONFIGMGR_UNIT_FORWARD_STRING` (the path to `unit.rdb`) is read with plain
`getenv()` in `Test::SetUp`, so unlike `arg-user` in fixture (b) it must NOT have
its backslashes doubled — nothing macro-expands it.
