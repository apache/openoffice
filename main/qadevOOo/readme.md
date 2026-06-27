# qadevOOo — Bazel migration status: 🔨 (OOoRunner.jar done)

## What builds

| Target | Output | Description |
|--------|--------|-------------|
| `//main/qadevOOo:OOoRunner` | `libOOoRunner.jar` (3.5 MB, ~2137 classes) | The qadevOOo QA test framework (`lib.*`, `base.*`, `util.*`, `share.*`, `mod.*`, `ifc.*`, `helper.*`, `org.openoffice.*`). |

`OOoRunner.jar` is the artifact the **bridges java_uno tests** need on their
classpath (`test/java_uno/acquire`, `com/sun/star/lib/.../java_remote/*`) — it was
the blocker noted in the bridges test matrix.

## Key decisions

- **Classpath = 5 migrated jars** (from `java/OOoRunner/build.xml` `main.classpath`):
  `ridl`, `unoil`, `jurt`, `juh_jar`, `java_uno_jar`.
  - ⚠️ The javaunohelper jar target is **`//main/javaunohelper:juh_jar`**, NOT
    `:juh` (which is the `juh.dll` cc_binary). Depending on `:juh` gives no Java
    classes → `package com.sun.star.comp.helper does not exist`
    (Bootstrap/ComponentContext live there).
- **`--release 8 -XepDisableAllChecks`** — 1538 legacy Java 1.4-era sources; modern
  JDK compiles them with deprecation / unreachable-catch warnings only (same as
  jurt). No source changes (out of scope).
- **No resources in the jar.** `src/main/resources` holds ONLY 670
  `objdsc/**/*.csv` object-description files, and the upstream ant jar target's
  include list excludes `.csv` — the runner reads them from the filesystem via an
  `-objdsc` run parameter, not the classpath. Jar = compiled classes only.
- **Manifest omitted.** The hand-written manifest (`Class-Path: ridl.jar unoil.jar`
  + `RegistrationClassName: org.openoffice.RunnerService`) is a deployment /
  UNO-registration detail, irrelevant for classpath use — same treatment as
  ridljar/unoloader.

## Not yet migrated (rest of the module)

- `java/OOoRunnerLight` (subset jar), `qa/complex/*` (junit complex tests),
  `JunitTest_qadevOOo_unoapi` (the big UNO-API subsequent-test suite), `testdocs/*`
  (C++/java test fixtures). These need the running-soffice / OfficeConnection
  fixture and are part of the deferred subsequent-test bucket.
