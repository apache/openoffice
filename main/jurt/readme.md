# jurt — Bazel Migration

## Outputs

| Target | Output | Description |
|--------|--------|-------------|
| `//main/jurt:jurt` | `libjurt.jar` | UNO Java URP bridge library |
| `//main/jurt:jpipx` | `jpipx.dll` | JNI pipe implementation (real impl) |
| `//main/jurt:jpipe` | `jpipe.dll` | JNI pipe wrapper loaded by Java |

## Build structure

### jurt.jar
51 Java sources compiled against `ridl.jar` (UDK type registry) and `unoloader.jar`.
Implements URP protocol, pipe/socket connections, bridge factory, service manager,
URL resolver, thread pool, and the Java UNO environment.

Two compiler flags are required for legacy code:
- `--release 8`: `Thread.suspend()` / `Thread.resume()` were removed in Java 21;
  compiling against the Java 8 API (via ct.sym) makes them visible again.
- `-XepDisableAllChecks`: disables Google Error Prone lint-as-errors that fire
  on legitimate but old-style patterns (ArrayToString, EqualsHashCode).

#### Error Prone — known violations and how to resolve them

Error Prone is Bazel's default Java linter and treats certain patterns as hard
errors.  The flag above silences all checks globally.  If you ever want to
re-enable Error Prone (e.g. to catch regressions in new code), remove
`-XepDisableAllChecks` and fix the two remaining violations first:

| File | Check | What it flags | Fix |
|------|-------|---------------|-----|
| `lib/connections/socket/SocketConnection.java:175,197` | `ArrayToString` | `bytes[0]` and `aData` concatenated into a string — arrays don't have a useful `toString()` | Replace with `Arrays.toString(bytes[0])` / `Arrays.toString(aData)` |
| `lib/uno/environments/java/java_environment.java:149` | `EqualsHashCode` | Class overrides `equals()` but not `hashCode()` | Add a `hashCode()` implementation, or suppress with `@SuppressWarnings("EqualsHashCode")` if identity-based hashing is intentional |

You can also suppress individual violations inline without touching logic:
```java
@SuppressWarnings("ArrayToString")
// or
@SuppressWarnings("EqualsHashCode")
```
and then remove `-XepDisableAllChecks` from `javacopts` in the BUILD file to
restore per-check enforcement for all other code.

### jpipx.dll / jpipe.dll (Windows pipe JNI bridge)
On Windows, Java's `System.loadLibrary("jpipe")` loads `jpipe.dll`.  `jpipe.dll`
then loads `jpipx.dll` from the same directory via
`LoadLibraryEx(LOAD_WITH_ALTERED_SEARCH_PATH)` and forwards all five JNI calls
via `GetProcAddress`.

**Why two DLLs?**  `jpipe.dll` must not have dependencies on any other URE DLLs
(sal3.dll etc.) because the Java class loader calls `LoadLibrary` before the URE
DLL directory is on the search path.  The real implementation (`jpipx.dll`) can
link against sal3.dll normally, since it is loaded by `jpipe.dll` with an altered
search path that includes the URE directory.

**Calling conventions and DEF files:**
- `jpipx.dll` exports plain `__cdecl` names (`PipeConnection_create` etc.).
  A DEF file is included for clarity, though `__cdecl` exports are undecorated
  by default in MSVC.
- `jpipe.dll` exports `JNICALL` (`__stdcall`) functions with the JNI naming
  convention (`Java_com_sun_star_lib_connections_pipe_PipeConnection_createJNI`).
  The DEF file is required: without it, MSVC 32-bit emits `_func@N` decorated
  names in the export table, which the JVM cannot find via `GetProcAddress`.

## Dependencies
- Java: `//main/ridljar:ridl`, `//main/ridljar:unoloader`
- C: `//main/sal:sal_headers`, `//main/sal:sal_implib`, `@rules_java//toolchains:jni`
