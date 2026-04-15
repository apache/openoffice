# ridljar — Bazel build status: ✅

## What builds

| Target | Output | Description |
|--------|--------|-------------|
| `//main/ridljar:udkapi_java_classes` | `udkapi_java_classes/` (dir) | javamaker-generated `.class` files from `udkapi.rdb` |
| `//main/ridljar:udkapi_java_jar` | `udkapi_java_jar.jar` | that directory packaged into a JAR |
| `//main/ridljar:udkapi_java` | (java_import) | classpath entry exposing the generated UDK types |
| `//main/ridljar:ridl` | `libridl.jar` | 27 hand-written ridl Java sources compiled against `:udkapi_java` |
| `//main/ridljar:unoloader` | `libunoloader.jar` | UnoClassLoader + UnoLoader (no external deps) |

## Pipeline

```
udkapi.rdb
   │
   ▼ javamaker_classes (java_pipeline.bzl)
udkapi_java_classes/   ← declare_directory of .class files
   │
   ▼ jar_from_directory (java_pipeline.bzl)
udkapi_java_jar.jar
   │
   ▼ java_import
:udkapi_java            ← compile-time classpath entry
   │
   ▼ java_library (javac, 27 .java sources)
libridl.jar
```

## Key design decisions

- **`jar_from_directory` Starlark rule** — replaces a broken genrule.  
  The genrule approach (`cmd_ps = '& "./$(JAVABASE)/bin/jar" cf ...'`) fails on
  Windows because `$(JAVABASE)` expands to a relative path containing `+` chars
  (e.g. `external/rules_java++toolchains+local_jdk`) that PowerShell's `&`
  operator cannot resolve.  The Starlark rule uses `ctx.actions.run` with
  `jar.exe` located by scanning `JavaRuntimeInfo.files` — no shell involved.

- **Toolchain type** — `@bazel_tools//tools/jdk:runtime_toolchain_type` is the
  correct Starlark toolchain type for the JDK runtime.
  `@rules_java//toolchains:current_java_runtime` is a genrule-only alias that
  does NOT provide `ToolchainTypeInfo` and cannot be used in custom rules.

- **Deprecation warnings** — expected; the ridl sources are Java 1.4-era code
  compiled by a modern JDK.  Source code is out of scope for this migration.
