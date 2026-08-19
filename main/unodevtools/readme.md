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

# Notes for unodevtools (done)

One executable, `uno-skeletonmaker.exe`, from 9 `.cxx` files — the whole module.
It reads a UNOIDL type library and prints either a declaration dump or a
complete component skeleton, in C++ or Java.

- Ported from `Executable_uno-skeletonmaker.mk`; the source list is that file's
  `add_exception_objects` verbatim, which is also every `.cxx` in the module.
- Deps: `//main/codemaker` (`codemaker` + `commoncpp` + `commonjava` static
  libs), cppu, cppuhelper, reg, sal, salhelper, `udkapi_idl_headers`, stlport.
- `stlport` is not optional: `inc/unodevtools/typemanager.hxx` uses `<hash_map>`
  and `skeletoncommon.hxx` `<hash_set>`.
- `_DEFINES` / `_COPTS` mirror `//main/codemaker` rather than being derived
  fresh. This tool links that module's static libs, so it has to agree with them
  on `/GR` and `/EHsc`.

## It is a C++ module, whatever the frontier said

The migration frontier listed `unodevtools` under "Remaining: Java-based" as
"(UNO component inspector, Java)". Both halves of that are wrong, and the entry
is corrected. There is no Java source here and nothing to wait for `rules_java`
on — Java is only one of the *output* languages of the skeletons it emits, which
is why `commonjava` is linked. That is also why the module came out of a bucket
that is otherwise blocked and built on the first try.

## The manifest has to be embedded, not staged beside it

Every other `/MD` tool this build produces — cppumaker, javamaker, regcomp, rsc
— is *build-internal*: some rule runs it, and that rule stages the VC90 CRT DLLs
and an external `<exe>.manifest` next to it (`build/rules/idl_pipeline.bzl` and
friends). Nothing in this build runs `uno-skeletonmaker`; it is an SDK tool a
component author runs from wherever the SDK put it, so there is no staging step
to hang an external manifest on. Without one the process dies at load with
`0xC0000135` (STATUS_DLL_NOT_FOUND) before `main`, because the `/MD` CRT
reference cannot be resolved.

The fix is the resource `gtest_test` already uses for the same reason:
`//main/external/msvcp90:vc90_app_manifest_res`, the CRT manifest compiled to a
`.res` and linked in at `RT_MANIFEST` id 1, so the image carries its own
activation context. It is arch-selected (`vc90_app_manifest_amd64.rc` on x64),
so it needs no per-arch handling here. Upstream reaches the same place from the
other side: solenv embeds a manifest in *every* executable it links.

## Running it needs an installation, and must not be run from inside one

`unodevtools/typemanager.cxx` calls `defaultBootstrap_InitialComponentContext()`
— this is a fixture-(a) consumer in the sense of the test buckets. Give it a
type library and let the UNO DLL closure come from `program/` on `PATH`:

```powershell
$prog  = (Resolve-Path "bazel-winXP-x86-bin\main\staging\program").Path
$env:PATH = "$prog;$env:PATH"
$types = "file:///" + ($prog -replace '\','/') + "/types.rdb"
uno-skeletonmaker.exe "-env:UNO_TYPES=$types" dump --cpp -t com.sun.star.text.XTextRange
uno-skeletonmaker.exe "-env:UNO_TYPES=$types" component --cpp   -o out -n MyComp             -t com.sun.star.lang.XInitialization
uno-skeletonmaker.exe "-env:UNO_TYPES=$types" component --java5 -o out -n org.example.MyComp -t com.sun.star.lang.XInitialization
```

The exe must sit in its **own** directory, not in `program/`. The co-located-UNO
-DLL landmine applies here exactly as it does to the gtest targets: `cppu`
resolves `uno.ini` beside whichever `cppuhelper3MSC.dll` won the loader search,
and if that is a copy next to the exe, every `vnd.sun.star.expand:` URI silently
expands to nothing.

Verified on both arches: all three commands above exit 0 and produce correct
output under `--config=winXP-x86` and `--config=winXP-x64`. Between them they
exercise all four generators — `cpptypemaker`, `cppcompskeleton`,
`javatypemaker`, `javacompskeleton`.

## Upstream quirk found while testing: a failed dump still exits 0

`skeletonmaker.cxx` catches `CannotDumpException`, prints `Error: <type>` to
stderr, and then falls through to `return 0`. So an unknown or unresolvable type
is *reported* but not *signalled* — a script driving the tool cannot tell success
from failure by exit code, only by scraping stderr. This is upstream behaviour,
reproduced identically here, and is left alone: source is not being changed in
this migration. Worth knowing before anything automates the tool.

## qa: `//main/unodevtools:skeletonmaker_test`, and why it needs no install

Seven GoogleTest cases over the generators. Upstream has no `qa/` here, so these
are new, not ported.

They exist because the tool cannot usefully be tested through its command line:
by the section above, a failed generation exits 0, so a test judging the exe by
its exit code could never fail. `staged_run_test` — the generic
"stage an arbitrary exe and run it", which testtools' bridgetest uses — is
therefore not usable here, because its verdict *is* the exit code.

So the tests call the generators directly. Two facts about the module make that
cheap, and they are worth knowing before touching this BUILD:

1. The generators are **UNO-free**. Every `com/sun/star/...` string in
   `cppcompskeleton.cxx` and friends is part of the code they *emit*; none of
   them includes a UNO header. Hence the split into `skeletonmaker_lib` (the
   five writers, needing only codemaker + registry + sal) and
   `unodevtools_uno` (`UnoTypeManager` + `typeblob` + CLI options), which is
   what actually drags in cppu/cppuhelper and is linked only by the exe.
2. The generators take **`TypeManager const &`**, codemaker's base class. The
   tool feeds them `UnoTypeManager`, which bootstraps a component context; the
   test feeds them codemaker's `RegistryTypeManager`, which reads a `.rdb`
   straight off disk. Neither can tell the difference.

The result is a suite that runs against `//main/udkapi`'s rdb with three DLLs
beside the exe — `sal3`, `reg`, `store` — and no staged install, no bootstrap,
no `services.rdb`. Every type it names is a udkapi type for that reason.
Concretely: 435 targets configured, and the suite runs in 0.3 s.

An earlier draft used `uno_install = "//main/staging:install"`, on the reasoning
that nine other test targets already depend on it so the cost is marginal. That
reasoning was wrong — it made a test of a standalone code generator depend on
the entire office, NSS and Python included. If a future test here seems to need
the install, re-read point 2 first.

### The landmine: `setBase("UCR")`

`RegistryTypeManager` defaults its base key to `/`, and `searchTypeKey()`
composes `base + "/" + name`. `idl_pipeline.bzl` merges the `.urd` files under
the key `UCR` and runs cppumaker with `-BUCR`, so every type in the rdb lives at
`UCR/com/sun/star/...`. Leave the base at its default and the lookup asks for
`//com/sun/star/...`, which misses **every** type — and it does not surface as a
failed `init()`. `init()` succeeds, and each generator call then throws
`CannotDumpException`, i.e. it looks exactly like "the tool cannot resolve this
type" rather than "the registry is mounted at the wrong root".

### Getting just the .rdb out of an `idl_library`

`idl_library` returns the rdb *and* the generated-header directory in one
`DefaultInfo`. Anything that stages files individually — `data_files` here —
fails on the directory with *"symlink() with target_file directory param"*. The
rule now also returns `OutputGroupInfo(rdb=..., headers=...)`, so a consumer can
pick a side; `//main/udkapi:udkapi_rdb` is that filegroup for udkapi.

## Not migrated: what consumes the tool

`main/odk` ships `uno-skeletonmaker` in the SDK and is still `⬜`. Nothing else
in the tree calls it, so there is no staging entry to add yet; the target is
`//visibility:public` and ready for odk when that bucket lands.

Also note `main/codemaker/source/bonobowrappermaker`, a sibling of the
`commoncpp`/`commonjava` helpers this tool links: it is dead code, in no
`BUILD.bazel` and no `build.lst`, and stays that way.
