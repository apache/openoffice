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

# Notes for testtools (bridgetest green: in-process C++ and Java, plus C++ over URP)

`bridgetest` is the UNO bridge round-trip suite: every construct the type
system has — each simple type, strings, enums, structs, polymorphic structs,
sequences, anys, interfaces, attributes, out/inout parameters, exceptions,
multiple inheritance, the current context, recursive and sequence-of-calls
dispatch — pushed through a call chain and checked coming back.

Three targets, one driver:

| target | object under test | fixture | time |
| --- | --- | --- | --- |
| `//main/testtools:bridgetest` | `CppTestObject` (C++), in-process | self-contained | ~1.1 s |
| `//main/testtools:bridgetest_java` | `JavaTestObject` (Java), in-process | `uno_install` | ~1.4 s |
| `//main/testtools:bridgetest_urp` | `CppTestObject` in a **second process**, over socket URP | `uno_install`, client/server | ~4 s |

`//main/testtools:bridgetest_tests` runs all three. The first two check that
the *bridges* marshal correctly (`msci_uno`/`mscx_uno` and `java_uno`); the
third checks that the *remote* path does — a different bridge implementation
(`//main/binaryurp`) over a real connection.

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

## The URP target — the round trip across a process boundary

`//main/testtools:bridgetest_urp` is dmake's `bridgetest_server` +
`bridgetest_client` pair. Same driver, same assertions, same C++ object as
`:bridgetest` — but the object lives in **another process**, so every call is
marshalled by the binary URP bridge over a TCP connection.

The single difference on the command line is that `-u <uno url>` comes *after*
`--`, which makes it an argument to `BridgeTest::run()` rather than to `unoexe`:
the driver then resolves the object through
`com.sun.star.bridge.UnoUrlResolver` instead of instantiating it locally
(`bridgetest.cxx` around the `remote` flag). The server side is the same
`uno.exe` with its *own* `-u`, plus `--singleaccept` so it serves one
connection and exits.

It is the only test in the tree that puts a real object graph across a real
connection: acceptor and connector (`//main/io`), the URP protocol
(`//main/binaryurp`, whose own qa/ suites cover only its cache and unmarshal
helpers in isolation), `UnoUrlResolver` (`//main/remotebridges`), and bridge
lifetime. It is also **wider than the Java target**, which must pass
`noCurrentContext` because the Java object has no `XCurrentContext` to hand
back — here that sub-test runs, so this is the only check that a UNO current
context propagates across a process boundary.

### The `.uno` naming divergence, finally biting

dmake gives the **server** only the two test registries and lets `unoexe.cxx`
cover the rest: when `createInstance()` cannot find a service it falls back to
`loadSharedLibComponentFactory()` on the hardcoded names `acceptor.uno.dll`,
`connector.uno.dll` and `binaryurp.uno.dll`.

Bazel emits those as `acceptor.dll` / `connector.dll` / `binaryurp.dll` (the
known component-DLL naming divergence in CLAUDE.md), so that fallback can never
fire here — and this is the *first* place the divergence has actually mattered.
CLAUDE.md predicted exactly this site ("remote-UNO/URP bootstrap").

The fix is not a rename. The server gets the installation's own
`program/services.rdb` nested in, the same third registry the Java target
already uses, and all three services then resolve **by service name** on the
first attempt, so the hardcoded-filename path is never reached. A rename would
have to move `services.rdb` in lockstep; naming the services does not.

### Orchestration: `server_args` / `server_ready_port`

There is no dmake recipe to copy. dmake only *generates* the two `.bat` scripts
— they are in `ALLTAR`, but nothing runs them; only the in-process `runtest` is
executed automatically. So `gtest_test.bzl` grew two attributes:

- **`server_args`** — stage `binary` a second time as `<name>_server.exe` and
  start it detached (`start "" /b`) just before the client. A second staged
  name rather than reusing the client's is what makes the cleanup
  `taskkill /f /im` precise; killing by a shared image name would take the
  client with it.
- **`server_ready_port`** — poll `netstat` until something is LISTENING there.
  Without it the test is a race the client usually loses: the server needs
  about a second to load the UNO stack before it reaches `accept()`, and
  `UnoUrlResolver::resolve()` makes **one** `connect()` and throws
  `NoConnectException`. A connect probe would be worse than useless — a probe
  that succeeds consumes the single connection `--singleaccept` will serve, so
  it has to be `netstat`.

The verdict is purely the **client's** exit code; the server is fixture. Any
straggling server is killed afterwards, so a client that dies before connecting
cannot leave one blocked in `accept()` holding the port. `tags = ["exclusive"]`
because port 2002 is upstream's hardcoded choice and is machine-global.

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
- `source/bridgetest/pyuno` — **BLOCKED on a retired mechanism, not on the
  fixture.** It was recorded here as "reachable now that pyuno is wired", which
  was wrong: the Python variant is not the same driver at all. It is a
  `unittest` suite (`main.py` → `importer` / `core` / `impl`), and its very
  first statement registers the C++ objects dynamically:

  ```python
  unohelper.addComponentsToContext(
      ctx, ctx, (FOO+"/cppobj.uno", FOO+"/bridgetest.uno", …),
      "com.sun.star.loader.SharedLibrary")
  ```

  That path is `com.sun.star.registry.ImplementationRegistration` →
  `DllComponentLoader::writeRegistryInfo()` →
  `cppuhelper::writeSharedLibComponentInfo()`, which resolves
  **`component_writeInfo`** — the pre-`.component` registration mechanism.
  `cppobj.cxx` and `bridgetest.cxx` export only
  `component_getImplementationEnvironment` and `component_getFactory`, so
  `shlib.cxx` throws `cannot get symbol: component_writeInfo`. Registering the
  components in `UNO_SERVICES` instead does not help — the call is made
  unconditionally, before any test runs — and `importer.py`'s
  `testDynamicComponentRegistration` does the same thing again with
  `acceptor.uno` / `connector.uno`, so it is the suite's premise, not one line.

  This is the **same wall as `configmgr/qa/unit`** (see CLAUDE.md): a qa/ dir
  gated off for a decade encoding a mechanism the product no longer has. Fixing
  it means porting `main.py` to a services registry, i.e. a source change.
  Note also that `main.py` never checks the runner's result and never calls
  `sys.exit()`, so a faithful port would be **green whatever it reported** —
  a second reason it cannot be wired as-is.
- `source/performance` (`perftest.uno`) — a benchmark, not a correctness test.
  Note `TestComponent.java` *does* implement
  `com.sun.star.test.performance.XPerformanceTest`, whose IDL lives in udkapi,
  so the Java half of it is already compiled here.
- `source/cliversioning`, `qa/cli`, `qa/cliversioning` — `cli_ure` bucket.
- `bridgetest_javaserver` — the socket-URP variant with a **Java** server
  (`com.sun.star.comp.bridge.TestComponentMain … singleaccept`). The C++
  client/server pair is now wired as `:bridgetest_urp`; this one additionally
  needs the background process to be a `java` command line rather than a second
  copy of `binary`, which `server_args` does not currently express. Its
  coverage — Java marshalling — is already reached in-process by
  `:bridgetest_java`; what it would add is Java *over URP*.
