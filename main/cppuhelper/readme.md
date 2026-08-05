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

# Notes for cppuhelper (done)

- `cppuhelper3MSC.dll`; private type `XExceptionThrower.idl` via `idl_library`
  (`cppuhelper_unotypes_idl`, rooted at `unotypes/` so the generated header
  path is `cppuhelper/detail/XExceptionThrower.hpp`)
- Standalone qa suites: `cppuhelper_qa_ifcontainer` (5), `cppuhelper_qa_unourl`,
  `cppuhelper_qa_weak`, gathered in `:cppuhelper_tests`

## qa/propertysetmixin — `//main/cppuhelper:cppuhelper_qa_propertysetmixin`

`cppu::PropertySetMixin` turns IDL `[attribute]`s into a
`com.sun.star.beans.XPropertySet`. The only way to exercise it is through a real
UNO component that *uses* the mixin, so the suite ships one — which is why this
is the most involved qa/ wiring in the tree so far. Four artifacts, not one:

| Target | What it is |
| --- | --- |
| `:cppuhelper_qa_psm_idl` | `qa/propertysetmixin/types.idl` → `.rdb` + C++ headers |
| `:cppuhelper_qa_psm_types` | `merge_rdb` re-emitting just the registry as `psm_types.rdb` |
| `:qa_propertysetmixin.uno` | the component DLL (`comp_propertysetmixin.cxx`) |
| `:cppuhelper_qa_psm_services` | `services_rdb` registering that DLL → `psm_services.rdb` |

`merge_rdb` is there only because `idl_library` returns its `.rdb` *and* a
header directory; the merge re-emits the registry alone, under the name the test
stages it as.

**This is fixture (a)** — an in-process bootstrap, no soffice — despite the
makefile sitting behind `OOO_SUBSEQUENT_TESTS`. An earlier note in
`BUILD.bazel` claimed it needed `OfficeConnection`; it does not. Unlike
`configmgr/qa/unit` it uses the **modern `.component` mechanism**, so it does
not hit that suite's `component_writeInfo` wall (see
`main/configmgr/readme.md`).

### Environment

The dmake recipe sets `UNO_TYPES`, `UNO_SERVICES`, `URE_INTERNAL_LIB_DIR` and
`OOO_INBUILD_SHAREDLIB_DIR` by hand. The middle two come free from
`uno_install`'s `URE_BOOTSTRAP`, but the registries must still be set
explicitly, because the test's own have to be **appended** to the install's —
and `rtl::Bootstrap` has no append. It resolves the environment *before* the
ini, so these values **replace** `fundamental.ini`'s and therefore have to
repeat them:

    UNO_TYPES     = <program>/types.rdb <program>/oovbaapi.rdb <rundir>/psm_types.rdb
    UNO_SERVICES  = <program>/services.rdb <rundir>/psm_services.rdb

> **Drift watch:** the leading entries mirror `main/staging/fundamental.ini`.
> If that file gains a registry, add it here too. The symptom of forgetting is
> a service the office normally provides going missing.

`OOO_INBUILD_SHAREDLIB_DIR` is dmake's `COMPONENTPREFIX_INBUILD_NATIVE`
(`solenv/inc/settings.mk`) — "the component lives wherever this build put it".
The `vnd.sun.star.expand:` URI in `psm_services.rdb` resolves through it to the
staged test directory.

Co-locating the component DLL is safe here: the "co-located UNO DLL breaks the
bootstrap" landmine applies to `cppuhelper3MSC` and friends, whose directory
cppuhelper resolves from its *own* module handle. This one is reached only
through the services.rdb URI.

### Expected: 3 of 6 red

`testJavaEmpty1` / `testJavaEmpty2` / `testJavaFull` ask for
`test.cppuhelper.propertysetmixin.JavaSupplier`, which lives in the suite's
*other* component — a Java one (`JavaSupplier.java` +
`qa_propertysetmixin.java.component` + a `.uno.jar` built by `javamaker`). That
belongs to the Java bucket and is not wired, so those three cannot instantiate
their service. UNO exceptions do not derive from `std::exception`, so gtest
reports them as a bare "Unknown C++ exception" — that is the expected shape of
the miss, not a crash.

The three C++ cases are the ones that actually exercise `PropertySetMixin`.
Wiring the suite now means the mixin is covered, and the Java half turns green
for free once the Java bucket lands.

### DEF file

`qa/propertysetmixin/qa_propertysetmixin.uno.def` is the Windows equivalent of
`solenv/src/component.map` (`SHL2VERSIONMAP`). **Two** exports, not the usual
three: `comp_propertysetmixin.cxx` defines no `component_canUnload`, so listing
it would be an unresolved-symbol link error.
