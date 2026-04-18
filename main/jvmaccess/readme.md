# jvmaccess — Bazel Migration

## Output
- `jvmaccess.dll` — C++ JNI helper library for safe interaction with a Java VM from UNO/C++ code

## Targets
| Label | Description |
|---|---|
| `//main/jvmaccess:jvmaccess` | `jvmaccess.dll` (linkshared cc_binary) |
| `//main/jvmaccess:jvmaccess_headers` | Public headers under `inc/jvmaccess/` |
| `//main/jvmaccess:jvmaccess_implib` | `jvmaccess.if.lib` import library for downstream DLLs |

## Key classes
- `VirtualMachine` / `AttachGuard` — RAII wrapper for JNI thread attach/detach
- `UnoVirtualMachine` — wraps `VirtualMachine` with a UNO class loader global ref
- `ClassPath` — translates OUString class-path tokens to `java.net.URL[]` / loads classes

## DEF file
`util/jvmaccess.def` was derived from `util/msvc_win32_intel.map` (GNU ld version-script format → Windows EXPORTS format). Exports cover UDK_3.1–3.4 symbol sets.

## Dependencies
`sal`, `salhelper`, `cppu`, `udkapi` (IDL headers), JNI (`@rules_java//toolchains:jni`)

## Compiler flags
- `/Zc:wchar_t-` — required for `sal_Unicode` / UNO type compatibility with VS2008
- `SOLAR_JAVA` defined — enables JNI code paths in all three source files
- `JVMACCESS_DLLIMPLEMENTATION` — switches `jvmaccessdllapi.h` to `SAL_DLLPUBLIC_EXPORT`
