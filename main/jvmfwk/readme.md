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

# Notes for jvmfwk (done)

- Two DLLs: `jvmfwk.dll` and `sunjavaplugin.dll`
- Deps: cppu, cppuhelper, sal, salhelper, udkapi_idl_headers, stlport, boost.legacy, @libxml2//:libxml2, @rules_java//toolchains:jni, advapi32
- Exports: 21 C-linkage `jfw_*` functions from `inc/jvmfwk/` via `jvmfwk.def`

## `SOLAR_JAVA` is the difference between a library and a stub

`jvmfwk.dll` was originally built without it. That is not a feature switch:
`source/framework.cxx` compiles `jfw_startVM()` to a bare
`return JFW_E_ERROR;` under `#ifndef SOLAR_JAVA`, and `inc/jvmfwk/framework.h`
then declares `JavaVM` / `JNIEnv` / `JavaVMOption` as incomplete types instead of
including `<jni.h>`. Built that way, no configuration can make Java work —
stoc's `javavm.uno`, cui's Java options page and `javaldx` all get a generic
error however correct the JRE, `javavendors.xml` and `jvmfwk3.ini` are.

Upstream drives it from `configure`; here it is unconditional, since `rules_java`
is a hard dependency. It brings in `@rules_java//toolchains:jni` for the real
`<jni.h>`. The exported ABI is unaffected — those types appear only behind
pointers, so consumers that include `jvmfwk/framework.h` *without* `SOLAR_JAVA`
(desktop, cui, svl, svtools) still compile and link against the same entry
points.

Same landmine, third occurrence: see `//main/bridges` (java_uno JNI bridge) and
`//main/stoc` (javaloader/javavm).

## `sunjavaplugin.dll` — the half that knows about JREs

`jvmfwk` itself contains no JRE knowledge at all. `javavendors.xml` maps each
supported vendor to
`vnd.sun.star.expand:$URE_INTERNAL_LIB_DIR/sunjavaplugin.dll`, which jvmfwk
`osl_loadModule()`s; the plugin enumerates candidate JREs (registry
`HKLM\SOFTWARE\JavaSoft` — hence `advapi32`), reports vendor/version/runtime lib,
and is what actually calls `JNI_CreateJavaVM`.

- Sources: the 8 TUs of `plugins/sunmajor/pluginlib` (pch
  `precompiled_plugin.hxx`, not `precompiled_jvmfwk.hxx`).
- Exports: 4 functions converted from `plugins/sunmajor/pluginlib/sunjavaplugin.map`
  to `util/sunjavaplugin.def`. These are a **private jvmfwk plugin interface, not
  UNO component entry points** — no `component_getFactory`, and nothing in
  `services.rdb`.
- **Nothing links it**, so it must be listed explicitly in
  `//main/staging` (same class of runtime-only dependency as the C++/UNO bridge
  DLL). Missing, the office reports no Java installed however many JDKs are present.
- No `/Zc:wchar_t-`: unlike `javavm`, this code never hands `sal_Unicode*` to a
  JNI string call.

## How a JRE is actually recognised — and `JREProperties.class`

`sunjavaplugin` does not parse files to identify a JRE. It **runs** the
candidate:

```
<jre>/bin/java -classpath <dir of sunjavaplugin.dll> JREProperties
```

and reads `java.vendor` / `java.version` / `java.home` off stdout, character
code by character code (`util.cxx` `getJavaProps`). So `JREProperties.class`
must be a **loose class file next to the plugin** — a jar sitting in that
directory is not on that `-classpath`. Upstream builds it with Ant
(`Ant_jreproperties.mk`); here `javac_classes`
(`build/rules/java_pipeline.bzl`) is the equivalent, at `--release 8` because
it runs on the *candidate* JRE, not on the build JDK.

Without it, every JRE on the machine is rejected with the singularly
unhelpful:

```
[Java framework] The JRE specified by the bootstrap variable UNO_JAVA_JFW_JREHOME
or UNO_JAVA_JFW_ENV_JREHOME could not be recognized.
```

That message covers *every* failure mode of `jfw_getJavaInfoByPath` — missing
probe class, unsupported vendor, unreadable path — so treat it as "look at all
three", not as a vendor problem.

**The vendor gate is a second, independent filter.** `vendorlist.cxx` maps a
`java.vendor` string to a handler class, and `javavendors.xml` lists which
vendors are acceptable at which minimum version. A JRE passes only if it
appears in **both**. `Temurin` was in neither — see the source-fix commit.

## Runtime configuration staged into `program/`

- `javavendors.xml` — from `distributions/OpenOfficeorg/javavendors_wnt.xml`, via
  `copy_file` because the bootstrap value names the bare filename.
- `JREProperties.class` — the probe above.
- `sunjavaplugin.ini` — from `plugins/sunmajor/pluginlib/sunjavapluginrc`. Read
  by the plugin from its own directory (`util.cxx` `InitBootstrapData`,
  `SAL_CONFIGFILE("/sunjavaplugin")`); its one key adds `noaccessibility` to the
  probe run so it skips `java.awt.Toolkit.getDefaultToolkit()`. Measured *not*
  to be required — `//main/cppuhelper:cppuhelper_qa_propertysetmixin` passes 6/6
  without it — but staged for parity, since that toolkit call is the probe's only
  dependency on a usable display and would bite headless.
- `jvmfwk3.ini` — in `main/staging/`. The name is fixed by
  `source/fwkutil.hxx`, which hardcodes `SAL_CONFIGFILE("/jvmfwk3")` relative to
  its own library directory, so it stays `jvmfwk3` even though the library here
  is `jvmfwk.dll` rather than upstream's `jvmfwk3.dll`.

`URE_INTERNAL_LIB_DIR` — which `javavendors.xml`'s plugin URI expands through —
is **not** needed in `program/fundamental.ini`. It reads like it should be (the
URI is expanded by `cppu::bootstrap_expandUri` against `URE_BOOTSTRAP`), but
`findPlugin` then resolves the result relative to the jvmfwk library's own
directory, which is already `program/`. Measured: adding it changes nothing.

See `main/stoc/readme.md` for the full Java2-loader bootstrap chain.

## libxml2 overlay fixes for VS2008

- `config.h` overlay: win32/VC10/config.h as base, HAVE_STDINT_H guarded to VS2010+, SEND_ARG2_CAST defined empty
- `libxml2-configure.patch` disables FTP, HTTP, iconv, debug, zlib, lzma in xmlversion.h
- nanoftp.c and nanohttp.c excluded from LIBXML2_SRCS (networking not needed)
- After any overlay/patch hash change: `bazel mod deps --lockfile_mode=refresh`
