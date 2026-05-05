# reportbuilder — Bazel Migration Notes

## Status: Deferred

This module is a pure Java UNO extension (`.oxt` package). It does not produce
any C++ DLLs and is not on the critical path to `desktop`.

## What this module builds

- `sun-report-builder.jar` — compiled from ~194 Java sources under
  `java/com/sun/star/report/` (API classes, util, function/metadata, pentaho engine)
- `report-builder.oxt` — extension package bundling the JAR, registry config,
  templates, images, and the JFreeReport dependency JARs

## Blockers

### 1. JFreeReport suite — no Bazel-compatible source

The Pentaho engine code (`pentaho/` package) compiles against a suite of
JFreeReport support libraries that are **not available on Maven Central**:

| JAR | Version | SourceForge location |
|-----|---------|----------------------|
| flute | 1.1.6 | `02. Libraries/1.1.6-stable/flute-1.1.6.zip` |
| libbase | 1.1.6 | `02. Libraries/1.1.6-stable/libbase-1.1.6.zip` |
| libfonts | 1.1.6 | `02. Libraries/1.1.6-stable/libfonts-1.1.6.zip` |
| libformula | 1.1.7 | `02. Libraries/` (1.1.7 folder) |
| liblayout | 0.2.10 | `10. Flow Engine/0.9.4/liblayout-0.2.10.zip` |
| libloader | 1.1.6 | `02. Libraries/1.1.6-stable/libloader-1.1.6.zip` |
| librepository | 1.1.6 | `02. Libraries/1.1.6-stable/librepository-1.1.6.zip` |
| libserializer | 1.1.6 | `02. Libraries/1.1.6-stable/libserializer-1.1.6.zip` |
| libxml | 1.1.7 | `02. Libraries/` (1.1.7 folder) |
| flow-engine | 0.9.4 | `10. Flow Engine/0.9.4/flow-engine-0.9.4.zip` |
| sac | — | bundled with JFreeReport |
| commons-logging | 1.1.3 | Maven Central (`commons-logging:commons-logging:1.1.3`) |

SourceForge (`https://sourceforge.net/projects/jfreereport/files/`) uses
token-based redirect URLs with timestamps, making stable `source.json` entries
for the local Bazel registry impractical without manually downloading each ZIP,
computing its SHA-256, and storing it.

### 2. `reportbuilderwizard.jar` — comes from deferred `wizards` module

The extension package includes `reportbuilderwizard.jar` which is produced by
the `wizards` module. That module is itself deferred (Java-based).

### 3. `apache-commons` ext_library has no BUILD.bazel yet

`ext_libraries/modules/apache-commons/` exists but has only legacy makefiles —
no `source.json` or overlay `BUILD.bazel`.

## What needs to happen to unblock

1. Create bzlmod modules in `ext_libraries/modules/` for each JFreeReport JAR
   (or a single bundled `jfreereport` module), with correct SHA-256 integrity
   values computed from the SourceForge ZIP downloads.
2. Add a `commons-logging` bzlmod module (or use `rules_jvm_external` + Maven).
3. Migrate the `wizards` module (provides `reportbuilderwizard.jar`).

## Java compilation dependencies (from makefiles)

```
ridl.jar        → //main/ridljar:ridl
unoil.jar       → //main/unoil:unoil
jurt.jar        → //main/jurt:jurt
juh.jar         → //main/javaunohelper:juh_jar
java_uno.jar    → (bridges module, not yet identified in Bazel)
+ JFreeReport suite (see table above)
+ commons-logging-1.1.3.jar
```

## Registry/config files in the extension

15 files across `registry/schema/` and `registry/data/` — all straightforward
`filegroup` or copy targets once the JAR dependency is resolved.
