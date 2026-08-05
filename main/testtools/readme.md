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

# Notes for testtools (bridgetest: C++ and Java green)

`bridgetest` is the UNO bridge round-trip suite: every construct the type
system has — each simple type, strings, enums, structs, polymorphic structs,
sequences, anys, interfaces, attributes, out/inout parameters, exceptions,
multiple inheritance, the current context, recursive and sequence-of-calls
dispatch — pushed through a call chain and checked coming back.

Two targets, one driver:

| target | object under test | fixture | time |
| --- | --- | --- | --- |
| `//main/testtools:bridgetest` | `CppTestObject` (C++) | self-contained | ~1.1 s |
| `//main/testtools:bridgetest_java` | `JavaTestObject` (Java) | `uno_install` | ~1.4 s |

`//main/testtools:bridgetest_tests` runs both.

## It is not a GoogleTest, and should not be made into one

Every other test in this tree is a gtest binary. This one is three UNO
*components*, run by the generic `//main/cpputools` `uno.exe`:

```
uno -ro uno_services.rdb -ro uno_types.rdb \
    -s com.sun.star.test.bridge.BridgeTest -- com.sun.star.test.bridge.CppTestObject
```

`uno.exe` instantiates the service named by `-s`, queries `XMain`, and calls
`run()` with everything after `--`. So the driver receives the *name* of the
object to test and instantiates it itself — which is exactly what lets one
driver serve the C++, Java, Python and CLI test objects. Only that last
argument changes between the two targets above. `run()`'s return value is the
process exit code, so the pass/fail signal needs no wrapper, and wrapping it
would mean changing source, which is out of scope.

This is what `run_args` on `staged_run_test` was added for
(`build/rules/gtest_test.bzl`): a fixed command line baked into the launcher
and token-expanded like `env`, so the "test binary" can be a generic tool that
a suite selects behaviour from. Anything passed on the `bazel test` command
line still follows it via `%*`.

## The C++ target is a fourth fixture kind

Not fixture (a): `uno.exe`'s `-ro` path calls
`bootstrap_InitialComponentContext(registry)`, **not**
`defaultBootstrap_InitialComponentContext()`. There is no `fundamental.ini`, no
`URE_BOOTSTRAP`, and no staged office — hence no `uno_install`, and hence a test
that runs in about a second. Everything the service manager needs beyond the two
registries is hardcoded in cppuhelper's `bootstrapInitialSF()`
(`ORegistryServiceManager`, `DLLComponentLoader`, `SimpleRegistry`,
`NestedRegistry`, `TypeDescriptionManager`, …), all loaded out of
`bootstrap.uno.dll` beside the exe.

Two consequences worth knowing before adding another registry-bootstrap test:

- **The textual (XML) registry works here**, unlike in `configmgr/qa/unit`.
  `openRegistry()` passes `(bReadOnly=true, bCreate=false)`, and
  `SimpleRegistry::open()`'s textual branch requires exactly
  `bReadOnly && !bCreate`. configmgr's suite is red because it goes through the
  one-argument `createRegistryServiceFactory` overload, which defaults
  `bReadOnly=false`. Same class of registry, opposite outcome, decided by two
  booleans.
- **Relative component URIs are the right choice**, not
  `vnd.sun.star.expand:`. stoc's `textualservices.cxx` resolves a component
  `uri` with `rtl::Uri::convertRelToAbs()` against the URL of the registry
  *file*, so dmake's `./cppobj.uno.dll` just means "beside this rdb" and needs
  no bootstrap variable pointing at the staged directory. That is why the C++
  target sets no `env` at all.

## The Java target, and the landmine that inverts the DLL rule

`bridgetest_java` swaps in `com.sun.star.test.bridge.JavaTestObject` from
`testComponent.jar`, so every type in `bridgetest.idl` now crosses the
`java_uno` JNI bridge twice. `cppuhelper/qa/propertysetmixin` proved a Java UNO
component can be *loaded*; this is the first thing to check that the
marshalling is correct across the whole type vocabulary.

`noCurrentContext` is the second argument dmake passes and it is not optional —
the Java side has no `XCurrentContextChecker` to hand back, so the driver skips
that sub-test.

Three things differ from the C++ target, and each cost a debugging round:

**1. `$URE_INTERNAL_LIB_DIR` is not in `fundamental.ini`.** Registering the
Java2 loader halves with that macro yields the amputated
`loading component library failed: /javaloader.uno.dll` — an undefined
bootstrap macro expands to nothing rather than failing. The tree's convention
is `$OOO_BASE_DIR/program/…` (`basis_native()`), and that macro *is* defined.

**2. Co-locating the core UNO DLLs breaks the bootstrap — and the mechanism is
`uno.ini`, not just `bootstrap.uno.dll`.** The existing note about
`get_this_libpath()` covers only half of it. `cppu::get_unorc()`
(`cppuhelper/source/bootstrap.cxx`) opens

```
get_this_libpath() + "/" + SAL_CONFIGFILE("uno")
```

that is, `uno.ini` **beside `cppuhelper3MSC.dll`** — and `uno.ini` is where
`URE_INTERNAL_LIB_DIR` is defined. *Every* `vnd.sun.star.expand:` URI in the
tree resolves through that one handle (`cppu::bootstrap_expandUri` →
`cppuhelper::detail::expandMacros`). Stage `cppuhelper3MSC.dll` next to the exe
and the exe's own directory wins the loader search, so the lookup lands in the
test directory, finds no `uno.ini`, and every such macro silently expands to
nothing. Here it surfaced two layers away, as

```
[Java framework] The file: vnd.sun.star.expand:$URE_INTERNAL_LIB_DIR/sunjavaplugin.dll does not exist.
[Java framework] The JRE specified by … could not be recognized.
```

So with `uno_install`, `runtime` must list only genuinely test-only files. The
C++ target lists the whole core stack and the Java one lists none — the
opposite of the usual advice, and correct in both cases.

**3. The third registry is not optional.** dmake adds
`$(SOLARXMLDIR)/ure/services.rdb` and it is tempting to replace that with just
the two Java2-loader components. That does not work: `javaloader` resolves the
component URL through `com.sun.star.uri.UriReferenceFactory`, itself a URE
component (stoc's `uriproc`), so naming the loaders without the registry
providing what *they* need only moves the failure one service along. The
installation's `program/services.rdb` is nested in as a `file:///` URL, which
`unoexe`'s `convertToFileUrl` passes through untouched.

`uno_install` here earns its keep for the **data**, not the DLLs: the Java2
loader's entire configuration (`URE_INTERNAL_JAVA_DIR` and the five jars
`javavm.cxx` opens by hardcoded name, `URE_MORE_JAVA_TYPES`,
`URE_INTERNAL_JAVA_CLASSPATH`, `URE_OVERRIDE_JAVA_JFW_{SHARED,USER}_DATA`) is
keys in `program/fundamental.ini`, and `jvmfwk.dll` finds `javavendors.xml`,
`jvmfwk3.ini`, `sunjavaplugin.dll` and `JREProperties.class` beside itself
there. One `URE_BOOTSTRAP` supplies the lot with no list to keep in sync.

## Build notes

- `/Zc:wchar_t-` throughout — `sal_Unicode` is not VS2008's native `wchar_t`.
- `//main/stlport:stlport` — `osl/diagnose.hxx` (the C++ header, unlike
  `diagnose.h`) declares an `std::hash_set` member.
- Each component DLL exports **two** functions, not the usual three: none of
  the three sources defines `component_canUnload`, so the `.def` files
  converted from `solenv/src/component.map` list only
  `component_getImplementationEnvironment` and `component_getFactory`.
- `bridgetest.uno` and `cppobj.uno` each compile their own copy of
  `multi.cxx` / `currentcontextchecker.cxx`; dmake builds two static libs from
  the same objects for the same reason.
- The JRE for the Java target comes from `//build:jre.bzl`, shared with
  `cppuhelper/qa/propertysetmixin`. It is machine-specific and arch-selected —
  the test loads `jvm.dll` into its own process, so a 32-bit build needs a
  32-bit JRE and `$JAVA_HOME` cannot answer for both arches.

## Not migrated

- `source/bridgetest/cli` (C#/VB round trip) — blocked on the `cli_ure`
  bucket's C# and C++/CLI toolchains.
- `source/bridgetest/pyuno` — the Python variant of the same driver; reachable
  now that pyuno is wired, not yet done.
- `source/performance` (`perftest.uno`) — a benchmark, not a correctness test.
  Note `TestComponent.java` *does* implement
  `com.sun.star.test.performance.XPerformanceTest`, whose IDL lives in udkapi,
  so the Java half of it is already compiled here.
- `source/cliversioning`, `qa/cli`, `qa/cliversioning` — `cli_ure` bucket.
- `bridgetest_server` / `bridgetest_client` / `bridgetest_javaserver` — the
  socket-URP variants of the same driver. They need an acceptor and a second
  process; the in-process targets cover the marshalling, and URP itself is
  covered by `//main/binaryurp` and `//main/test:test_qa_officeconnection`.
